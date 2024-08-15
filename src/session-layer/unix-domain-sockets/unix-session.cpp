/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <string_view>
#include "unix-session.hpp"
namespace unix_session
{
    static const std::size_t PAGE = 4096;

    void uSession::read(){
        std::array<char, PAGE> buf;
        boost::system::error_code ec;
        do{
            std::size_t len = _socket.read_some(boost::asio::mutable_buffer(buf.data(), buf.size()), ec);
            if(!ec){
                std::unique_lock<std::mutex> lk = lock();
                std::string_view s(buf.data(), len);
                rbuf << s;
            }
        } while(!ec);
    }

    void uSession::write(){
        std::unique_lock<std::mutex> lk = lock();
        std::string buf(wbuf.str());
        boost::system::error_code ec;
        boost::asio::const_buffer out(buf.data(), buf.size());
        do{
            std::size_t len = _socket.write_some(out, ec);
            out += len;
        }while(out.size() > 0);
    }

    void uServer::open(const boost::asio::local::stream_protocol::endpoint& endpoint){
        boost::asio::local::stream_protocol::socket socket(_ioc);
        socket.non_blocking(true);
        socket.connect(endpoint);
        std::shared_ptr<uSession> session = std::make_shared<uSession>(std::move(socket), *this);
        {
            std::unique_lock<std::mutex> lk = lock();
            push_back(session);
        }
    }
    void uServer::open(){
        std::unique_lock<std::mutex> lk = lock();
        _acceptor.open();
    }

    void uServer::accept(std::function<void(const std::error_code& ec, std::shared_ptr<uSession> session)> fn){
        _acceptor.async_accept([&, fn](const boost::system::error_code& ec, boost::asio::local::stream_protocol::socket socket){
            if(!ec){
                std::shared_ptr<uSession> session = std::make_shared<uSession>(std::move(socket), *this);
                socket.non_blocking(true);
                {
                    std::unique_lock<std::mutex> lk = lock();
                    push_back(session);
                }
                std::error_code errc(ec.value(), std::system_category());
                fn(errc, session);
                accept(fn);
            }
        });
    }
}
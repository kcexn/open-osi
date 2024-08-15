/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <string_view>
#include <filesystem>
#include "unix-session.hpp"
namespace unix_session
{
    static const std::size_t PAGE = 4096;

    void uSession::read(){
        std::array<char, PAGE> buf;
        boost::system::error_code ec;
        do{
            std::size_t len = _socket.read_some(boost::asio::mutable_buffer(buf.data(), PAGE), ec);
            if(!ec){
                auto lk = lock();
                std::string_view s(buf.data(), len);
                rbuf << s;
            }
        } while(!ec);
    }

    void uSession::async_read(){
        _socket.async_wait(
            boost::asio::local::stream_protocol::socket::wait_type::wait_read,
            [&](const boost::system::error_code& ec){
                if(!ec){
                    read();
                }
            }
        );
    }

    void uSession::write(){
        std::array<char, PAGE> buf;
        boost::system::error_code ec;
        auto lk = lock();
        auto rlen = wbuf.readsome(buf.data(), PAGE);
        while(rlen > 0){
            boost::asio::const_buffer out(buf.data(), rlen);
            do{
                std::size_t len = _socket.write_some(out, ec);
                out += len;
            }while(out.size() > 0 && !ec);
            if(!ec){
                rlen = wbuf.readsome(buf.data(), PAGE);
            }
        }
    }

    void uSession::async_write(){
        _socket.async_wait(
            boost::asio::local::stream_protocol::socket::wait_type::wait_write,
            [&](const boost::system::error_code& ec){
                if(!ec){
                    write();
                }
            }
        );
    }

    void uServer::open(const boost::asio::local::stream_protocol::endpoint& endpoint){
        boost::asio::local::stream_protocol::socket socket(_ioc);
        socket.non_blocking(true);
        socket.connect(endpoint);
        std::shared_ptr<uSession> session = std::make_shared<uSession>(std::move(socket), *this);
        {
            auto lk = lock();
            push_back(session);
        }
    }
    void uServer::open(){}

    void uServer::accept(std::function<void(const std::error_code& ec, std::shared_ptr<uSession> session)> fn){
        _acceptor.async_accept([&, fn](const boost::system::error_code& ec, boost::asio::local::stream_protocol::socket socket){
            if(!ec){
                socket.non_blocking(true);
                std::shared_ptr<uSession> session = std::make_shared<uSession>(std::move(socket), *this);
                {
                    auto lk = lock();
                    push_back(session);
                }
                std::error_code errc(ec.value(), std::system_category());
                fn(errc, session);
                accept(fn);
            }
        });
    }

    uServer::~uServer(){
        if(_endpoint != boost::asio::local::stream_protocol::endpoint()){
            std::filesystem::path p(_endpoint.path());
            std::filesystem::remove(p);
        }
    }
}
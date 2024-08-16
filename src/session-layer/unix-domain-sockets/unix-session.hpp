/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef UNIX_DOMAIN_SESSIONS_HPP
#define UNIX_DOMAIN_SESSIONS_HPP
#include <boost/asio.hpp>
#include "../session.hpp"
namespace unix_session
{
    // Forward Declarations
    class uServer;
    /* 
    *  Sessions own a low level interface to the underlying transport byte stream. 
    *  Sessions present an iostream of bytes for higher level presentation layers to interpret.
    *  Sessions provide i/o operations such as read, write, and their async counterparts.
    *  Sessions are equal to each other iff they are each other. 
    */
    class uSession: public session::Session
    {
        typedef boost::asio::local::stream_protocol::socket socket;
        socket _socket;
        
        public:
            uSession(socket&& socket, session::Server& server): session::Session(server), _socket(std::move(socket)) {}

            void read() override;
            void async_read(std::function<void(std::error_code ec)> cb) override;

            void write() override;
            void async_write(std::function<void(std::error_code ec)> cb) override;

            ~uSession() = default;
    };

    /*
    *  Servers aggregate and hold all sessions of the same type together.
    *  Servers manage the lifetime of network sessions.
    *  This means that servers must implement methods to open, maintain, and close network sessions.
    */
    class uServer: public session::Server
    {
        typedef boost::asio::local::stream_protocol::endpoint endpoint;
        typedef boost::asio::local::stream_protocol::acceptor acceptor;
        typedef boost::asio::local::stream_protocol::socket socket;

        boost::asio::io_context& _ioc;
        endpoint _endpoint;
        acceptor _acceptor;

        public:
            uServer(boost::asio::io_context& ioc): _ioc(ioc), _acceptor(ioc) {}
            uServer(boost::asio::io_context& ioc, const endpoint& endpoint): _ioc(ioc), _endpoint(endpoint), _acceptor(ioc, endpoint) {}

            void open(const endpoint& endpoint);
            void open() override;

            void accept(std::function<void(const std::error_code& ec, std::shared_ptr<uSession> session)> fn);

            ~uServer();           
    };
}
#endif
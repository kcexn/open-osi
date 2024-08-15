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
        public:
            uSession(boost::asio::local::stream_protocol::socket&& socket, session::Server& server): session::Session(server),_socket(std::move(socket)) {}

            void read() override;
            void write() override;

            ~uSession() = default;
        private:
            boost::asio::local::stream_protocol::socket _socket;
    };

    /*
    *  Servers aggregate and hold all sessions of the same type together.
    *  Servers manage the lifetime of network sessions.
    *  This means that servers must implement methods to open, maintain, and close network sessions.
    */
    class uServer: public session::Server
    {
        public:
            uServer(boost::asio::io_context& ioc, const boost::asio::local::stream_protocol::endpoint& endpoint): _ioc(ioc), _endpoint(endpoint), _acceptor(ioc) {}

            void open(const boost::asio::local::stream_protocol::endpoint& endpoint);
            void open() override;

            void accept(std::function<void(const std::error_code& ec, std::shared_ptr<uSession> session)> fn);

            virtual ~uServer() = default;
        private:
            boost::asio::io_context& _ioc;
            boost::asio::local::stream_protocol::endpoint _endpoint;
            boost::asio::local::stream_protocol::acceptor _acceptor;
    };
}



// #include <boost/asio.hpp>
// #include "../server/server.hpp"

// #include <iostream>

// namespace UnixServer{
//     class unix_session : public server::Session
//     {
//     public:
//         unix_session(boost::asio::io_context& ioc, server::Server& server): server::Session(server), socket_(ioc) {
//             boost::system::error_code ec;
//             socket_.native_non_blocking(true, ec);
//             if(ec){
//                 std::cerr << ec.message() << std::endl;
//                 throw "what?";
//             }
//         }
//         unix_session(boost::asio::local::stream_protocol::socket&& socket, server::Server& server): server::Session(server), socket_(std::move(socket)) {
//             boost::system::error_code ec;
//             socket_.native_non_blocking(true, ec);
//             if(ec){
//                 std::cerr << ec.message() << std::endl;
//                 throw "what?";
//             }
//         }
//         void async_read(std::function<void(boost::system::error_code ec, std::size_t length)> fn) override;
//         void async_write(const boost::asio::const_buffer& write_buffer, const std::function<void(const std::error_code& ec)>& fn) override;
//         void close() override;

//         void async_connect(const boost::asio::local::stream_protocol::endpoint&, std::function<void(const boost::system::error_code&)>);

//         ~unix_session() {
//             boost::system::error_code ec;
//             boost::asio::socket_base::linger opt(true, 30);
//             socket_.set_option(opt, ec);
//             if(ec){
//                 std::cerr << "unix-server.hpp:39:setting socket linger failed with:" << ec.message() << std::endl;
//             }
//             socket_.shutdown(boost::asio::local::stream_protocol::socket::shutdown_both, ec);
//             if(ec){
//                 std::cerr << "unix-server.hpp:43:shutting down socket failed with:" << ec.message() << std::endl;
//             }
//             socket_.close(ec);
//             if(ec){
//                 std::cerr << "unix-server.hpp:47:closing socket failed with:" << ec.message() << std::endl;
//             }
//         }
    
//     private:
//         boost::asio::local::stream_protocol::socket socket_;
//     };

//     class unix_server : public server::Server
//     {
//     public:
//         unix_server(boost::asio::io_context& ioc);
//         unix_server(boost::asio::io_context& ioc, const boost::asio::local::stream_protocol::endpoint& endpoint);

//         void async_connect(server::Remote addr, std::function<void(const boost::system::error_code&, const std::shared_ptr<server::Session>&)> fn) override;

//         void accept(std::function<void(const boost::system::error_code& ec, std::shared_ptr<UnixServer::unix_session> session)> fn);
//         void stop() { acceptor_.close(); return; }
//         ~unix_server();
            
//     private:
//         boost::asio::local::stream_protocol::endpoint endpoint_;
//         boost::asio::local::stream_protocol::acceptor acceptor_;
//     };
// }//Namespace UnixServer
#endif
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef SESSION_HPP
#define SESSION_HPP
#include <algorithm>
#include <sstream>
#include <memory>
#include <functional>
#include <system_error>
#include <cstddef>
#include <mutex>
namespace session
{
    // Forward Declarations
    class Server;
    /* 
    *  Sessions own a low level interface to the underlying transport byte stream. 
    *  Sessions present an iostream of bytes for higher level presentation layers to interpret.
    *  Sessions provide i/o operations such as read, write, and their async counterparts.
    *  Sessions are equal to each other iff they are each other. 
    */
    class Session: public std::enable_shared_from_this<Session>
    {
        public:
            Session(Server& server): rbuf(), wbuf(), _server(server){}

            virtual void read()=0;
            virtual void write()=0;
            
            std::unique_lock<std::mutex> lock() { return std::unique_lock<std::mutex>(_mtx); }
            std::stringstream rbuf;
            std::stringstream wbuf;

            virtual ~Session() = default;
        private:
            Server& _server;
            std::mutex _mtx;
    };

    /*
    *  Servers aggregate and hold all sessions of the same type together.
    *  Servers manage the lifetime of network sessions.
    *  This means that servers must implement methods to open, maintain, and close network sessions.
    */
    class Server: public std::vector<std::shared_ptr<Session> >
    {
        public:
            Server(){}

            virtual void open()=0;
            void close(const std::shared_ptr<Session>& sp){
                auto lk = lock();
                auto it = std::find(this->cbegin(), this->cend(), sp);
                if(it != this->cend()){
                    this->erase(it);
                }
            }
            std::unique_lock<std::mutex> lock() { return std::unique_lock<std::mutex>(_mtx); }

            virtual ~Server() = default;
        private:
            std::mutex _mtx;
    };
}
#endif
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "http-presentation.hpp"
namespace http
{
    namespace h_presentation
    {
        void HttpPresentation::read(){
            auto lk1 = lock();
            {
                auto lk2 = session->lock();
                session->rbuf >> std::get<http::HttpRequest>(*this);
            }  
        }

        void HttpPresentation::async_read(std::function<void(std::error_code ec)> cb){
            session->async_read([&, cb](std::error_code ec){
                if(!ec){
                    read();
                }
                cb(ec);
            });
        }

        void HttpPresentation::write(){
            auto lk1 = lock();
            auto& res = std::get<http::HttpResponse>(*this);
            {
                auto lk2 = session->lock();
                session->wbuf << res;
            }
            res.status_line_finished = true;
            res.next_header = res.headers.size();
            res.next_chunk = res.chunks.size();
            session->write();
        }
        
        void HttpPresentation::async_write(std::function<void(std::error_code ec)> cb){
            auto lk1 = lock();
            auto& res = std::get<http::HttpResponse>(*this);
            {
                auto lk2 = session->lock();
                session->wbuf << res;
            }
            res.status_line_finished = true;
            res.next_header = res.headers.size();
            res.next_chunk = res.chunks.size();
            session->async_write(cb);
        }

        //Http client sessions reverse the http server session logic.
        void HttpClientPresentation::read(){
            auto lk1 = lock();
            {
                auto lk2 = session->lock();
                session->rbuf >> std::get<http::HttpResponse>(*this);
            }
        }

        void HttpClientPresentation::async_read(std::function<void(std::error_code ec)> cb){
            session->async_read([&, cb](std::error_code ec){
                if(!ec){
                    read();
                }
                cb(ec);
            });
        }

        void HttpClientPresentation::write(){
            auto lk1 = lock();
            auto& req = std::get<http::HttpRequest>(*this);
            {
                auto lk2 = session->lock();
                session->wbuf << req;
            }
            req.http_request_line_complete = true;
            req.next_header = req.headers.size();
            req.next_chunk = req.chunks.size();
            session->write();
        }

        void HttpClientPresentation::async_write(std::function<void(std::error_code ec)> cb){
            auto lk1 = lock();
            auto& req = std::get<http::HttpRequest>(*this);
            {
                auto lk2 = session->lock();
                session->wbuf << req;
            }
            req.http_request_line_complete = true;
            req.next_header = req.headers.size();
            req.next_chunk = req.chunks.size();
            session->async_write(cb);
        }
    }
}
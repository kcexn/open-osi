/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef HTTP_PRESENTATION_HPP
#define HTTP_PRESENTATION_HPP
#include "http-requests.hpp"
#include "../presentation.hpp"
namespace http
{
    namespace h_presentation
    {
        typedef std::tuple<http::HttpRequest, http::HttpResponse> HttpReqRes;
        typedef presentation::Presentation<http::HttpRequest, http::HttpResponse> Presentation;
        typedef presentation::Presentations<http::HttpRequest, http::HttpResponse> HttpPresentations;

        //Http Sessions Contain a single request, and a single response.
        class HttpPresentation: public Presentation
        {
        public:
            HttpPresentation(HttpPresentations& server): Presentation(server) {}
            HttpPresentation(HttpPresentations& server, const std::shared_ptr<session::Session>& sp): Presentation(server, sp) {}

            void read() override;
            void async_read(std::function<void(std::error_code ec)> cb) override;
            void write() override;
            void async_write(std::function<void(std::error_code ec)> cb) override;

            ~HttpPresentation() = default;
        };

        //Http client sessions reverse the http server session logic.
        class HttpClientPresentation: public Presentation
        {
        public:
            HttpClientPresentation(HttpPresentations& server): Presentation(server) {}
            HttpClientPresentation(HttpPresentations& server, const std::shared_ptr<session::Session>& sp): Presentation(server, sp) {}

            void read() override;
            void async_read(std::function<void(std::error_code ec)> cb) override;
            void write() override;
            void async_write(std::function<void(std::error_code ec)> cb) override;

            ~HttpClientPresentation() = default;
        };
    }
}
#endif
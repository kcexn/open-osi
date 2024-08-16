/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef PRESENTATION_HPP
#define PRESENTATION_HPP
#include <tuple>
#include <memory>
#include "../session-layer/session.hpp"

namespace presentation{
    /*Forward Declaration*/
    template<class... Types>
    class Presentations;

    // A presentation is a container for Layer 6 Presentation layer in the OSI model.
    // The presentation provides utility funcions and machinery to:
    // 1) Bind layer 5 sessions to Application contexts.
    // 2) Convert data between their byte stream representations and application native data types.
    //      Layer 7 applications should operate purely on native user defined data structures.
    //      The presentation is responsible for converting between byte streams,
    //      (such as those streamed off a network socket, or a unix domain socket, or even a plain file descriptor),
    //      and application data structures.
    // 3) Read and Write data from any Layer 5 session.
    // 
    // Presentations are equal to each other iff they are each other.
    template<class... Types>
    class Presentation: public std::tuple<Types...>, public std::enable_shared_from_this<Presentation<Types...> >
    {
        Presentations<Types...>& _presentations;
        std::mutex _mtx;
        public:
            std::shared_ptr<session::Session> session;

            Presentation(Presentations<Types...>& presentations): _presentations(presentations){}
            Presentation(Presentations<Types...>& presentations, const std::shared_ptr<session::Session>& session): _presentations(presentations), session(session){}

            // Read from and Write to a Layer 5 Session.
            virtual void read()=0;
            virtual void async_read(std::function<void(std::error_code ec)> cb)=0;
            virtual void write()=0;
            virtual void async_write(std::function<void(std::error_code ec)> cb)=0;

            std::tuple<Types...> get(){
                auto lk = lock();
                auto tmp = std::tuple<Types...>(*this);
                return tmp;
            }

            // Set application data in application native types.
            Presentation& operator=(const std::tuple<Types...>& other){ 
                auto lk = lock();
                std::tuple<Types...>::operator=(other); 
                return *this; 
            }
            Presentation& operator=(std::tuple<Types...>&& other){ 
                auto lk = lock();
                std::tuple<Types...>::operator=(std::move(other)); 
                return *this; 
            }

            std::unique_lock<std::mutex> lock() { return std::unique_lock<std::mutex>(_mtx); }

            virtual ~Presentation() = default;
    };


    // Presentations is a container for managing presentations
    // As such it must support the relevant operations for creating, managing, and destroying presentations.
    template<class... Types>
    class Presentations: public std::vector<std::shared_ptr<Presentation<Types...> > >
    {
        std::mutex _mtx;
        public:
            Presentations(): std::vector<std::shared_ptr<Presentation<Types...> > >() {}

            // Create new presentations
            auto create(){
                auto lk = lock();
                auto presentation = std::make_shared<Presentation<Types...> >(*this);
                this->push_back(presentation);
                return presentation;
            }
            auto create(const std::shared_ptr<session::Session>& session){
                auto lk = lock();
                auto presentation = std::make_shared<Presentation<Types...> >(*this, session);
                this->push_back(presentation);
                return presentation;
            }
            
            // Delete unneeded presentations.
            void close(const std::shared_ptr<Presentation<Types...> >& pp){
                auto lk = lock();
                auto it = std::find(this->cbegin(), this->cend(), pp);
                if(it != this->cend()){
                    this->erase(it);
                }
            }
            std::unique_lock<std::mutex> lock() { return std::unique_lock<std::mutex>(_mtx); }

            ~Presentations() = default;
    };
}
#endif
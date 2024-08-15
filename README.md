# open-osi

## What is this?
open-osi strives to be a one-stop shop for all the networking abstractions that a c++ developer might need. open-osi does not have a focus on a specific application 
so anyone is welcome to fork it and add something new. The MPL license should mean that extending this library to meet anyone's specific application needs will always be a 
pain-free process, and in case the license terms haven't been understood properly, open-osi is built to be extended and so we encourage others to use it as a starting point 
to build their applications regardless of how they choose to license their extensions.

open-osi provides templates and abstractions to encapsulate layers 5 and 6 of the [OSI networking model](https://en.wikipedia.org/wiki/OSI_model) which are:
- Layer 5: session layer
- Layer 6: presentation layer

These layers are often neglected by libraries that provide some sort of user functionality. For instance, [libcurl](https://github.com/curl/curl),
and [zeromq](https://zeromq.org/) are both robust, and highly capable libraries that are useful for building applications that need an HTTP client or a messaging service
respectively; however, their APIs make it very difficult to integrate these libraries with applications that want complete control over how I/O is performed.
Critically, the problem is that these libraries spill out over layers 5 and 6 of the OSI networking model, this means that a communication protocol like HTTP/1.1 or ZMTP
can't be easily separated from the rest of the library. While this is convenient for developers who just need to get something working, if we have found ourselves in a
situation where we are using, or are considering the use of C++ for development, then it is very likely we are in a situation where *performance* is critical.

open-osi fixes this problem by explicitly separating the various parts of the networking stack along the layers defined by the OSI networking model.

### Layer 7: Application Layer
This is the layer that application users *really* care about. It encapsulates all of the logic that is needed to get an application up and running. This is also 
where libraries like [libcurl](https://github.com/curl/curl), and [zeromq](https://zeromq.org/) present an API to the developer that allows them to build something bigger. 
If the application layer is all you care about, then open-osi is not for you. 

### Layer 6: Presentation Layer
The presentation layer defines an application development interface to the networking stack. The user facing side of the presentation layer presents an 
interface that is human understandable, while the network facing side of the presentation layer is a byte stream that will eventually be written to the network interface.
The HTTP API that is provided by [NodeJS](https://nodejs.org/api/http.html) is an excellent example of how the presentation layer should be *used* by developers.

### Layer 5: Session Layer
The session layer defines how transport connections should be opened, closed, and otherwise managed by the application. For developers that have are comfortable with the 
Berkeley sockets API, the session layer for TCP is provided by the sequence:
```
socket()
bind()
listen()
accept()
```
or:
```
socket()
connect()
```
both of which will leave you with a file descriptor that represents a connected TCP session. If you are a developer working with a more exotic transport protocol like SCTP, 
or perhaps you are working in an environment that is using raw sockets. Then the Berkeley sockets API may be cumbersome to work with, and a more sophisticated layer 5 
abstraction is needed.

### 5+6+7
Typically the OSI layers are shown in a big column that resembles a stack. For applications that have some complexity though, the use of OSI more closely resembles a tree 
rooted at the Application. An application may use many different protocols: HTTP, ZMTP, FTP, etc., and therefore require many different presentation layers. Each presentation 
layer may also use many different transport protocols: TCP, UDP, SCTP, RTP etc., and therefore require a separate session layer to manage each transport. 

### < 5
open-osi assumes that the operating system and computer hardware will manage all of the layers below layer 5: Transport, Network, Data-link, and Physical.

## Dependencies:
[boost/asio](https://www.boost.org/doc/libs/1_86_0/doc/html/boost_asio.html)

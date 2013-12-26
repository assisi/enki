Installation
------------

Dependencies:

 * zmq (3.2.2, from source)
 * cppzmq (manually from github!, just clone and copy)
 * nzmqt (header-only) (3.2.2)


TODO
----

Change zmq integration. Right now, ZMQ is integrated directly into the Qt GUI through nzmqt.
A Qt signal is emitted every time a ZMQ message comes in.

A better (more modular) approach would be to integrate ZMQ into Enki::World. My current idea
is to do all communication (read all incoming messages, and publish all sensor data) on each
World::step. This way, we could have "headless" simulations controlled through ZMQ. Have to check if this makes sense. It would probably require the decoupling of World::step from GLupdate in the GUI (they seem to be coupled now, see ../viewer/Viewer.cpp)

Update to Qt5

 * Update cmake: http://qt-project.org/doc/qt-5.0/qtdoc/cmake-manual.html

NOTES
-----

 * GUI stuff doesn't seem to work in debug mode
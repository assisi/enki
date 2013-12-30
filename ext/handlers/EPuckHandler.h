/* \file  EPuckHandler.h
   \brief Communication handler for the EPuck robot.

 */

#ifndef ENKI_EPUCK_HANDLER_H
#define ENKI_EPUCK_HANDLER_H

#include <map>

#include "ext/handlers/RobotHandler.h"

namespace Enki
{
    class World;
    class EPuck;
    
    //! Abstract base class, defines the message-handling interface for Enki
    /*! Users should implement their own message handling according for each robot type.

     */
    class EPuckHandler : public RobotHandler
    {
    public:
        EPuckHandler() { }
        virtual ~EPuckHandler() { }
        
        //! Robot factory method
        /*! Creates E-Pucks.

            Keeps a pointer to the created robot, but does not
            delete it in the destructor.

            \return Returns the name of the created robot.

         */
        virtual std::string createRobot(zmq::socket_t* sock, World* world);

        //! Handle incoming message
        /*! Handles E-Puck motion commands.

         */
        virtual int handleIncoming(zmq::socket_t* sock, std::string name);

        //! Assemble outgoing messages
        /*! Sends E-Puck sensor data messages.
         */
        virtual int sendOutgoing(zmq::socket_t* sock);

    private:
        std::map<std::string, EPuck*> epucks_;
    };

}

#endif

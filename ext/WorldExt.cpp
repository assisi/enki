/* World extension implementation.

 */

#include <cstdio>
#include <iostream>

#include <boost/foreach.hpp>

#include <zmq.hpp>
#include "ext/zmq_helpers.h"

#include "enki/robots/e-puck/EPuck.h"
#include "ext/handlers/RobotHandler.h"
#include "WorldExt.h"


using namespace std;
using namespace zmq;


namespace Enki
{

// -----------------------------------------------------------------------------

    WorldExt::WorldExt(double r, 
                       const string& pub_address, 
                       const string& sub_address,
                       const Color& wallsColor, 
                       unsigned texWidth, 
                       unsigned texHeight, 
                       const uint32_t* texData)
        : World(r, wallsColor, texWidth, texHeight, texData),
          pub_address_(pub_address), sub_address_(sub_address)
    {
        context_ = new zmq::context_t(1);
        publisher_ = new socket_t(*context_, ZMQ_PUB);
        subscriber_ = new socket_t(*context_, ZMQ_SUB);

        publisher_->bind(pub_address_.c_str());
        subscriber_->connect(sub_address_.c_str());
        subscriber_->setsockopt(ZMQ_SUBSCRIBE, "sim", 3);
    }

// -----------------------------------------------------------------------------

    WorldExt::~WorldExt()
    {
        // We own the handlers, so delete them
        BOOST_FOREACH(const HandlerMap::value_type& rh, handlers_)
        {
            delete rh.second;
        }

        delete subscriber_;
        delete publisher_;
        delete context_;
     }

// -----------------------------------------------------------------------------

    bool WorldExt::addHandler(string type, RobotHandler* handler)
    {
        if (handlers_.count(type) > 0)
        {
            return false;
        }
        else
        {
            handlers_[type] = handler;
        }
    }

// -----------------------------------------------------------------------------

    /* virtual */
    void WorldExt::controlStep(double dt)
    {
        // TODO: Check if this update sequence is correct

        // Read all incoming messages & apply to robots
        // Icoming messages represent controller outputs
        int in_count = 0;

        message_t msg;
        // Read message header
        int len = subscriber_->recv(&msg, ZMQ_DONTWAIT);
        while (len > 0)
        {
            in_count++;
            string target = msg_to_str(msg);
            // TODO: Implement "Sim handler" ???
            // This message "peeling" is quite a pain...
            if (target == "sim")
            {
                handleSim_();
            }
            else if (handlers_by_object_.count(target) > 0)
            {
                handlers_by_object_[target]->handleIncoming(subscriber_, target);
            }
            else
            {
                cerr << "Unknown object: " << target << endl;
            }
                
            cout << "Received: " << string(static_cast<char*>(msg.data())) << std::endl;
            len = subscriber_->recv(&msg, ZMQ_DONTWAIT);
        }

        // Publish sensor data
        int out_count = 0;

        // Gather messages from all handlers
        BOOST_FOREACH(const HandlerMap::value_type& rh, handlers_)
        {
            rh.second->sendOutgoing(publisher_);
        }

    }

// -----------------------------------------------------------------------------

    bool WorldExt::handleSim_(void)
    {
        message_t msg;
        if (!last_part(*subscriber_))
        {
            // Read command
            subscriber_->recv(&msg);
            string sim_cmd = msg_to_str(msg);
            if (sim_cmd == "spawn")
            {
                // Command is spawn
                // Read command contents and spawn robot
                if (!last_part(*subscriber_))
                {
                    subscriber_->recv(&msg);
                    // TODO: Parse protobuf message here!
                    string robot_type = "EPuck";
                    if (handlers_.count(robot_type) > 0)
                    {
                        string name = handlers_[robot_type]->createRobot(subscriber_, 
                                                                         this);
                        if (name.length() > 0)
                        {
                            // New robot was spawned
                            handlers_by_object_[name] = handlers_[robot_type];
                            subscriber_->setsockopt(ZMQ_SUBSCRIBE, 
                                                    name.c_str(), 
                                                    name.length());
                        }
                    }
                }
            }
            else
            {
                cerr << "Unknown command to sim!" << std::endl;
            }
        }
        else
        {
            cerr << "Missing message body for sim message!" << endl; 
        }
    }

// -----------------------------------------------------------------------------

}

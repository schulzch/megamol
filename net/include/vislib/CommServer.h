/*
 * CommServer.h
 *
 * Copyright (C) 2006 - 2010 by Visualisierungsinstitut Universitaet Stuttgart. 
 * Alle Rechte vorbehalten.
 */

#ifndef VISLIB_COMMSERVER_H_INCLUDED
#define VISLIB_COMMSERVER_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */
#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(push, off)
#endif /* defined(_WIN32) && defined(_MANAGED) */


#include "vislib/AbstractServerEndPoint.h"
#include "vislib/CommServerListener.h"
#include "vislib/CriticalSection.h"
#include "vislib/Runnable.h"
#include "vislib/SingleLinkedList.h"
#include "vislib/StackTrace.h"


namespace vislib {
namespace net {


    /**
     * TODO: comment class
     */
    class CommServer : public vislib::sys::Runnable {

    public:

        class Configuration : public ReferenceCounted {

        public:
            
            Configuration(AbstractServerEndPoint *serverEndPoint,
                const wchar_t *bindAddress);

            Configuration(SmartRef<AbstractServerEndPoint>& serverEndPoint,
                const wchar_t *bindAddress);

            Configuration(AbstractServerEndPoint *serverEndPoint,
                const char *bindAddress);

            Configuration(SmartRef<AbstractServerEndPoint>& serverEndPoint,
                const char *bindAddress);

            Configuration(const Configuration& rhs);
            
            const vislib::StringW& GetBindAddress(void) const {
                VLSTACKTRACE("Configuration::GetBindAddress", __FILE__, 
                    __LINE__);
                return this->bindAddress;
            }

            const SmartRef<AbstractServerEndPoint>& GetServerEndPoint(
                    void) const {
                VLSTACKTRACE("Configuration::GetServerEndPoint", __FILE__,
                    __LINE__);
                return this->serverEndPoint;
            }

        protected:

            virtual ~Configuration(void);

            vislib::StringW bindAddress;

            SmartRef<AbstractServerEndPoint> serverEndPoint;

        }; /* end class Configuration */


        /** Ctor. */
        CommServer(void);

        /** Dtor. */
        virtual ~CommServer(void);

        /**
         * Add a new CommServerListener to be informed about events of this 
         * server.
         *
         * The caller remains owner of the memory designated by 'listener' and
         * must ensure that the object exists as long as the listener is 
         * registered.
         *
         * This method is thread-safe.
         *
         * @param listener The listener to register. This must not be NULL.
         */
        void AddListener(CommServerListener *listener);

        /**
         * Removes, if registered, 'listener' from the list of objects informed
         * about events events.
         * 
         * The caller remains owner of the memory designated by 'listener'.
         *
         * This method is thread-safe.
         *
         * @param listener The listener to be removed. Nothing happens, if the
         *                 listener was not registered.
         */
        void RemoveListener(CommServerListener *listener);

        /**
         * Perform the work of a thread.
         *
         * @param configuration A pointer to a configuration instance on the
         *                      heap (allocated using new). You can release
         *                      your reference whenever you want, the server 
         *                      will call AddRef as first action.
         *
         * @return The application dependent return code of the thread. This 
         *         must not be STILL_ACTIVE (259).
         */
        virtual DWORD Run(void *configuration);

        /**
         * Abort the work of the server by forcefully closing the underlying
         * communication channel.
         *
         * @return true.
         */
        virtual bool Terminate(void);

    private:

        /** A thread-safe list for the message listeners. */
        typedef SingleLinkedList<CommServerListener *, 
            sys::CriticalSection> ListenerList;

        void createNewTcpServerEndPoint(void);

        /**
         * Inform all registered listener about a connection that was 
         * established.
         *
         * This method is thread-safe.
         *
         * @param channel The client communication channel.
         *
         * @return true if the connection was accepted, false otherwise.
         */
        bool fireNewConnection(SmartRef<AbstractCommChannel>& channel);

        /**
         * Inform all registered listener about an exception that was caught.
         *
         * This method is thread-safe.
         *
         * @param exception The exception that was caught.
         *
         * @return Accumulated (ANDed) return values of all listeners.
         */
        bool fireServerError(const vislib::Exception& exception);

        /**
         * Inform all registered listener about that the server is exiting.
         *
         * This method is thread-safe.
         */
        void fireServerExited(void);

        /**
         * Inform all registered listener about that the server is starting.
         *
         * This method is thread-safe.
         */
        void fireServerStarted(void);

        /** The list of listeners. */
        ListenerList listeners;

        /** The end point the server is using for waiting for clients. */
        SmartRef<AbstractServerEndPoint> serverEndPoint;

    };
    
} /* end namespace net */
} /* end namespace vislib */

#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(pop)
#endif /* defined(_WIN32) && defined(_MANAGED) */
#endif /* VISLIB_COMMSERVER_H_INCLUDED */


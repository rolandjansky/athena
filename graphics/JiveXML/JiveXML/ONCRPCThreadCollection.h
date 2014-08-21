/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_ONCRPCTHREADCOLLECTION_H
#define JIVEXML_ONCRPCTHREADCOLLECTION_H

#include <vector>
#include <pthread.h>
#include <semaphore.h>

namespace JiveXML{
  
  /**
   * This class handles a collection of threads
   */
  class ThreadCollection : private std::vector<pthread_t> {

    public:
      
      //Constructor
      ThreadCollection();

      //Destructor
      ~ThreadCollection();

      /** @name Public interface functions */
      //@{
      //Add a thread
      void AddThread( const pthread_t& thread );

      //Wait till the thread has been added
      void WaitAdd();

      //Remove a thread
      void RemoveThread( const pthread_t& thread );

      //Wait for all threads to finish
      void JoinAll();

      //Return number of threads
      //Can't be const as it requires mutex
      int NumberOfThreads();
      //@}
      
    private:
      //mutex to assure no conflicts when adding/removing threads
      pthread_mutex_t mutex;
      //Semaphore to assure we add all threads
      sem_t  semaphore;
  };

}
#endif


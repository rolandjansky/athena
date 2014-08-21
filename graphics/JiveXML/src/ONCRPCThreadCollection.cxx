/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <JiveXML/ONCRPCThreadCollection.h>

namespace JiveXML{
  
   //Constructor
  ThreadCollection::ThreadCollection(){

    //intialize the mutex with default attributes
    pthread_mutex_init(&mutex, NULL);

    //initialize the semaphore
    sem_init(&semaphore,0,0);

  }

   //Constructor
  ThreadCollection::~ThreadCollection(){

    //destroy the mutex
    pthread_mutex_destroy(&mutex);

    //and the semaphore
    sem_destroy(&semaphore);

  }

  //Add a thread to the container - don't need to be too strict
  //If a thread is double added, joinAll will still remove it.
  void ThreadCollection::AddThread( const pthread_t& thread ){
    
    //First get a mutex //RETVAL for all of them!
    pthread_mutex_lock(&mutex);

    //Now add the thread to the list of vectors
    push_back(thread);

    //Then remove mutex again
    pthread_mutex_unlock(&mutex);

    //And signal any potentially waiting threads
    sem_post(&semaphore);
    
  }

  //Wait until a thread has been added
  void ThreadCollection::WaitAdd(){

    //simply wait for the access semaphore to be set
    sem_wait(&semaphore);
  }

  //Remove a thread
  void ThreadCollection::RemoveThread( const pthread_t& thread ){
    
    //First get a mutex
    pthread_mutex_lock(&mutex);

    //Loop over list and find that entry
    ThreadCollection::iterator threadItr = begin();
    while ( threadItr != end() ){
      //See if this is the thread we are looking for
      if ( *threadItr == thread ){
        //remove it from the collection
        erase(threadItr);
        //iterator is invalid, we removed thread, so stop looping
        break ;
      }
      //Go to next thread
      ++threadItr;
    }

    //Then remove mutex again
    pthread_mutex_unlock(&mutex);

    //Set this threads state to detached, so its
    //resources are reclaimed once it
    //finishes.
    pthread_detach(thread);
    
  }

  //Wait for all threads to finish
  void ThreadCollection::JoinAll(){
    
    //The threads are removing themselves from the list,
    //so iterators are getting invalid while we are looping.
    //However, some threads may have crashed w/o being able to remove themselves
    //So wait for all of them to finish w/o keeping the mutex locked while
    //waiting
    
    //Loop till all threads are gone
    while ( size() > 0 ){
      
      //First get a mutex //RETVAL for all of them!
      pthread_mutex_lock(&mutex);

      //Order is not important - take the first element
      pthread_t thread = *begin();

      //Then remove mutex again
      pthread_mutex_unlock(&mutex);

      //Wait for that thread to finish
      pthread_join(thread,NULL);

      //Now remove it - if it has already removed itself, nothing will happen
      //If it hadn't removed itself, we will remove it
      RemoveThread(thread);
    }
  }

    
  //Return number of elements in the vector
  int ThreadCollection::NumberOfThreads() {

    //First get a mutex
    pthread_mutex_lock(&mutex);

    //Get number of elements
    int NThreads = size();

    //Then remove mutex again
    pthread_mutex_unlock(&mutex);
    
    //finally return size
    return NThreads;
  }

}//namespace

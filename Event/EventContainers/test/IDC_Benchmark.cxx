/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "EventContainers/IdentifiableContainerMT.h"
#include "EventContainers/IdentifiableCache.h"
#include <chrono>

using namespace EventContainers;


template<typename T>
void timedelete(std::string name, T* ptr){
   auto start1 = std::chrono::steady_clock::now();
   delete ptr;
   auto end1 = std::chrono::steady_clock::now();
   std::chrono::duration<double> time = end1-start1;
   std::cout << name  << " delete time " << time.count() << std::endl;
}

void accessTime(std::string name, IdentifiableContainerMT<long unsigned int>& container){

   auto start3 = std::chrono::steady_clock::now();
   auto offlinecnt = container.GetAllCurrentHashes();
   for(auto hash : offlinecnt) if(hash !=  *container.indexFindPtr(hash) ) std::abort();
   auto end3 = std::chrono::steady_clock::now();
   std::chrono::duration<double> offlinerandom = end3-start3;
   std::cout << name << " random time " << offlinerandom.count() << std::endl;

   auto start4 = std::chrono::steady_clock::now();
   size_t i=0;
   for(auto hash : container) {if(*hash != offlinecnt[i].value() ) std::abort(); i++; }
   auto end4 = std::chrono::steady_clock::now();
   std::chrono::duration<double> offlineit = end4-start4;
   std::cout << name << " iteration time " << offlineit.count() << std::endl;

   auto start5 = std::chrono::steady_clock::now();
   const auto& directaccess = container.GetAllHashPtrPair();
   for(const auto &[hashId, ptr] : directaccess) {if(hashId != *ptr ) std::abort(); i++; }
   auto end5 = std::chrono::steady_clock::now();
   std::chrono::duration<double> offlinedir = end5-start5;
   std::cout << name << " direct time " << offlinedir.count() << std::endl;
}

int main(){
   
   auto start1 = std::chrono::steady_clock::now();
   auto offline = new IdentifiableContainerMT<long unsigned int>(50000);
   for(size_t i =3;i<50000;i+=3){
      offline->addCollection(new long unsigned int(i) ,i).ignore();
   }
   auto end1 = std::chrono::steady_clock::now();
   std::chrono::duration<double> offlinefill = end1-start1;
   std::cout << "offline fill time " << offlinefill.count() << std::endl;
   auto start2 = std::chrono::steady_clock::now();
   auto cache  = new IdentifiableCache<long unsigned int>(50000, nullptr);
   auto online = new IdentifiableContainerMT<long unsigned int>(cache);

   for(size_t i =3;i<50000;i+=3){
      online->addCollection(new long unsigned int(i) , i).ignore();
   }
   auto end2 = std::chrono::steady_clock::now();
   std::chrono::duration<double> onlinefill = end2-start2;
   std::cout << "online fill time " << onlinefill.count() << std::endl;

   auto start3 = std::chrono::steady_clock::now();
   auto offlinefast = new IdentifiableContainerMT<long unsigned int>(50000, EventContainers::Mode::OfflineFast);
   for(size_t i =3;i<50000;i+=3){
      offlinefast->addCollection(new long unsigned int(i) ,i).ignore();
   }
   auto end3 = std::chrono::steady_clock::now();
   std::chrono::duration<double> offlinefillfast = end3-start3;
   std::cout << "offlinefast fill time " << offlinefillfast.count() << std::endl;
   accessTime("online ", *online);
   accessTime("offline ", *offline);
   accessTime("offlinefast ", *offlinefast);
   timedelete("onlineCont ", online);
   timedelete("onlineCache ", cache);
   timedelete("offline ", offline);
   timedelete("offlinefast ", offlinefast);
   std::cout << "Test Successful" << std::endl;
   return 0;
}

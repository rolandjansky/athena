/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "EventContainers/IdentifiableValueContainer.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <cassert>

typedef IdentifiableValueCache<int> int100cache;
typedef IdentifiableValueContainer<int> int100container;

int main(){
   auto *cache = new int100cache(100, std::numeric_limits<int>::min());
   auto *container = new int100container(cache);
   auto *container2 = new int100container(cache);
   assert(cache->emptyValue() == std::numeric_limits<int>::min());
   if(container->maxSize()!= 100) std::abort();
   if(!container->setOrDrop(50, 29)) std::abort();
   if(!container->setOrDrop(51, -29)) std::abort();
   if(!container->setOrDrop(52, -9)) std::abort();
   if(container->setOrDrop(52, 10) == true) std::abort(); //check if repeated setting passes
   if(container->retrieve(52) != -9) std::abort();

   if(container->present(50)==false || container->present(51)==false || container->present(52)==false) std::abort();
   if(container2->tryAddFromCache(50) == false) std::abort();
   if(container2->getAll().size()!= 1) std::abort();
   if(container->getAll().size()!= 3) std::abort();
   if(cache->getAll().size()!= 3) std::abort();
   if(container2->retrieve(50) != 29) std::abort();
   if(container2->present(51) == true) std::abort();
   if(container2->retrieve(51) != container2->emptyValue()) std::abort();
   bool except =false;
   try{
         container->setOrDrop(150, -29);
   }catch(const std::out_of_range& ex)
   {
      except = true;
      std::cout << "Threw exception correctly" << std::endl;
   }
   if(except == false) std::abort();
   delete container2;
   delete container;
   delete cache;
   std::cout << "Completed - no errors" << std::endl;
   return 0;
}

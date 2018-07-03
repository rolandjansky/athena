/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IDC_WAIT_H
#define IDC_WAIT_H
#include <vector>
#include <mutex>
#include <utility>
#include <atomic>
class IDC_Wait{
   std::vector<std::atomic<const void*>* > m_locklist;
   void Wait();
public:
   ~IDC_Wait();
   void AddWait(std::atomic<const void*>* at);
   void WaitAndClear();
};

#endif


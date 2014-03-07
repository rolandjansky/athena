/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ClassCounts
   @brief xx
*/

#ifndef LArSamples_ClassCounts_H
#define LArSamples_ClassCounts_H

#include "TString.h"
#include <map>

namespace LArSamples {
  
  class ClassCounts {
  
    public:
   
     /** @brief Constructor  */
     ClassCounts(const TString& name);
      
     virtual ~ClassCounts();

     const TString& className() const { return m_className; }
     static std::map<TString, int>& counts();

     void incrementInstanceCount() const { incrementInstanceCount(className()); }
     void decrementInstanceCount() const { decrementInstanceCount(className()); }

     static void incrementInstanceCount(const TString& name) { counts()[name]++; }
     static void decrementInstanceCount(const TString& name) { counts()[name]--; }
     
     int instanceCount() const { return instanceCount(className()); }
     static int instanceCount(const TString& name) { return m_counts ? (*m_counts)[name] : 0; }
          
     static void printCountsTable();
     
    private:
      
     TString m_className;      
     static std::map<TString, int>* m_counts;
  };
}
#endif

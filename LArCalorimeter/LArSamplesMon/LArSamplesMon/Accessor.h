/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::Accessor
   @brief A base class for accessing ntuple data
*/

#ifndef LArSamples_Accessor_H
#define LArSamples_Accessor_H

#include "LArCafJobs/Definitions.h"
#include "LArSamplesMon/AbsLArCells.h"

namespace LArSamples {
  
  class Data;
  class CellInfo;
  class History;
  class EventData;
  class RunData;
  
  class Accessor : public AbsLArCells {
  
    public:
      
      Accessor() { }
      virtual ~Accessor() { }
      
      virtual const EventData* eventData(unsigned int i) const = 0;
      virtual const RunData* runData(unsigned int i) const = 0;

      virtual unsigned int nEvents() const  = 0;
      virtual unsigned int nRuns() const  = 0;

      virtual unsigned int historySize(unsigned int i) const = 0;
      virtual bool writeToFile(const TString& fileName) const = 0;
      
    protected:
      
  };
}
  
#endif

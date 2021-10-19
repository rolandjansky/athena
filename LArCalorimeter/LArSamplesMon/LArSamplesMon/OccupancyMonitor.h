/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::OccupancyMonitor
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_OccupancyMonitor_H
#define LArSamples_OccupancyMonitor_H

#include "LArSamplesMon/MonitorBase.h"
#include "LArCafJobs/CaloId.h"

class TH1I;
  
namespace LArSamples {
    
  class OccupancyMonitor : public MonitorBase  
  {
  
    public:
   
      /** @brief Constructor  */
      OccupancyMonitor(const Interface& interface) : MonitorBase(interface) { }
    
      TH1I* runOccupancyHistory() const;
      TH1I* eventOccupancyHistory(unsigned int occForDump = 0) const;
      TH1I* eventOccupancy() const;
      TH1I* cellOccupancy(int minForPrintout = -1) const;
      TH1I* febOccupancy(CaloId calo) const;
      TH2D* partitionOccupancy(PartitionId part) const;
      TH2D* etaPhiOccupancy(CaloId calo, short layer, bool useEnergy = false) const;

      void cellAndRingOccupancy(CaloId calo, unsigned int nMin = 20) const;
      
      void dump(short verbosity = 2) const;
  };
}
  
#endif

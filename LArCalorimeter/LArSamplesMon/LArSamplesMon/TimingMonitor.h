/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::TimingMonitor
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_TimingMonitor_H
#define LArSamples_TimingMonitor_H

#include "TH1D.h"
#include "TH2D.h"
#include "LArSamplesMon/MonitorBase.h"
#include "LArCafJobs/CaloId.h"

namespace LArSamples {
  
  class TimingMonitor : public MonitorBase  
  {
  
    public:
    
    /** @brief Constructor  */

      
      TimingMonitor(const Interface& interface) : MonitorBase(interface) { } 
    

      TH2D** timingPlots(PartitionId part, unsigned int tBins = 100, 
			 double tMin = -50, double tMax = 50) const;
      
      
      TH1F* timing(PartitionId part, unsigned int ft, unsigned int feb, 
                   unsigned int tBins = 100, double tMin = -50, double tMax = 50) const;
      
      /**TH1F*** time(Id::PartitionId part, unsigned int tBins = 100, double tMin = -50, double tMax = 50) const;
	 
      */

    private:
      
  };
}
  
#endif

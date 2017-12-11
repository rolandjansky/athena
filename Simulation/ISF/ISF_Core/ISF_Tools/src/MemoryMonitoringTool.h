/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MemoryMonitoringTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_MEMORYMONITORINGTOOL_H
#define ISF_TOOLS_MEMORYMONITORINGTOOL_H 1

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_Interfaces/IMonitoringTool.h"
#include "ISF_Interfaces/ISimulationSelector.h"

// stl includes
#include <utility>
#include <vector>

namespace ISF {

  /** these datatypes used to store (infoStr,memUsage) paired info */
  typedef std::pair <const char* , int>   InfoUsagePair;
  typedef std::vector <InfoUsagePair*>    InfoUsagePairVector;


  /** @class MemoryMonitoringTool
  
      the code actually computing the memory usage is kindly stolen from:
      atlasoff:: Simulation/G4Sim/SimHelpers/SimHelpers/MemorySnooper.h

      @author Elmar.Ritsch -at- cern.ch
     */
  class MemoryMonitoringTool : public extends<AthAlgTool, IMonitoringTool> {
      
    public: 
     /** Constructor with parameters */
     MemoryMonitoringTool( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~MemoryMonitoringTool();

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** return current memory usage in kBytes */
     virtual double getCurrent() const;

     /** dump current memory monitoring information */
     virtual void dumpCurrent(const char *infoStr,
                              bool considerInSummary);

     /** store the current memory monitoring information internally */
     virtual void recordCurrent(const char *infoStr);

     /** dump all internally stored memory monitoring information */
     virtual void dumpSummary(const char *dumpCallDescr) const;

	  private:
     /** consolidate /proc to determine current memory usage */
     int computeCurMemoryUsage() const;

     /** needed to compute per event stats */
     unsigned int                   m_numCalls; 
     unsigned int                   m_prevCallMemUsage;
     double                         m_accumulatedCallMemory;
     double                         m_accumulatedIncrMemory;

     /** storing all (infoStr,memUsage) pairs created in recordCurrent(..) calls */
     InfoUsagePairVector            m_table;

  }; 
  
}


#endif //> !ISF_TOOLS_MEMORYMONITORINGTOOL_H

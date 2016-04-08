/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILTERUSINGMBTSTIMING_H
#define FILTERUSINGMBTSTIMING_H

#include "AthenaBaseComps/AthAlgorithm.h"

//Forward declarations
class TileTBID;

class FilterUsingMBTSTiming : public AthAlgorithm {

  public:
 
    //Constructor
    FilterUsingMBTSTiming (const std::string& name, ISvcLocator* pSvcLocator);

    //Default Athena algorithm methods
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    /** @name Configurable options */  
    //@{
    ///MBTSContainer", "StoreGate key of the MBTS container (default=\"MBTSContainer\")");
    std::string m_mbtsContainerName;
    ///Only count cell above this threshold (default = 40/222[pC])");
    float m_MBTSThreshold;
    ///Maximum difference of the average time of hits on A and C-side (default=10[ns])");
    float m_maxTimeDifference;
    //@}

    // Handle to the Tile identifier helper
    const TileTBID* m_tileTBID;
};
#endif

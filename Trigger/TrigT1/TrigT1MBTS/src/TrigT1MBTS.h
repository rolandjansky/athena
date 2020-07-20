/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1MBTS_H
#define TRIGT1MBTS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TileEvent/TileContainer.h"
#include "TileIdentifier/TileTBID.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include <string>
#include <vector>

namespace LVL1 {
  /** @class TrigT1MBTS

  @author W. H. Bell <W.Bell@cern.ch>

  An algorithm to simulate the level 1 MBTS trigger.  This algorithm
  records a data object of MbtsCTP type into StoreGate.  The object
  contains the input bits for the CTP simulation.

  */  
  class TrigT1MBTS: public AthAlgorithm {
  public:
    TrigT1MBTS(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  private:
    /** A data member to retain a connection to the level 1
	configuration service */
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
    
    /** A data member to retain a pointer to the Tile Test Beam
    Identifier service.  The MBTS identifiers were added after the
    other tile identifiers.  To avoid confusion and extra work they
    were added to the Test Bem (TB) Identifier service. */ 
    const TileTBID* m_tileTBID;

    /** A data member to contain the name of the TileTTL1 container
	that contains the input MBTS information.  This data member is
	used as a job option propertty
    */
    std::string m_tileTTL1ContainerName;

    /** A data member to contain a pointer to the TileTTL1 container
	that contains the input MBTS information. */
    const TileTTL1Container* m_tileTTL1MBTS;

    /** The thresholds for the level 1 MBTS trigger. */
    std::vector<float> m_thresholds_a; 	     // backward thresholds
    std::vector<float> m_thresholds_short_a; // backward thresholds with size 12
    std::vector<float> m_thresholds_c; 	     // forward thresholds 
    std::vector<float> m_thresholds_short_c; // forward thresholds with size 12 

    int m_cablestart_a; // backward threshold cable start
    int m_cablestart_c; // forward threshold cable start
    std::vector<int> m_cablestarts_a; // backward threshold cable starts
    std::vector<int> m_cablestarts_c; // forward threshold cable starts

    /** A data member containing the mapping from channel,phi to
	threshold number **/
    std::vector<std::vector<unsigned int> > m_thresholdNumber;
    std::vector<std::vector<unsigned int> > m_thresholdNumber12;

    /** The index of the sample bin corresponding to a particle
	arriving from the IP. */
    unsigned int m_tZeroBin;
    float m_CFD_fraction;

    /** A flag to prevent the trigger simulation from running over bad
	input data.  */
    bool m_badDataFound;
    
    bool m_ThrVecSize12;
    /** Flag to indicate whether or not the individual counter thresholds
	are defined separately in the menu */
    //bool m_singleCounterInputs;
  };
}

#endif

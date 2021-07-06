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
    
  private:
    /** A data member to retain a connection to the level 1
	configuration service */
    Gaudi::Property<bool> m_useNewConfig{this, "UseNewConfig", false, "When true, read the menu from detector store, when false use the L1ConfigSvc"};
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc{this, "LVL1ConfigSvc", "LVL1ConfigSvc", "The LVL1ConfigSvc providing L1 configuration for Run 2"};
    
    /** A data member to retain a pointer to the Tile Test Beam
    Identifier service.  The MBTS identifiers were added after the
    other tile identifiers.  To avoid confusion and extra work they
    were added to the Test Bem (TB) Identifier service. */ 
    const TileTBID* m_tileTBID{nullptr};

    /** A data member to contain the name of the TileTTL1 container
	   * that contains the input MBTS information.  This data member is
	   * used as a job option property
     */
    Gaudi::Property<std::string> m_tileTTL1ContainerName{this, "TileTTL1ContainerName", "TileTTL1MBTS"};

    /** The thresholds for the level 1 MBTS trigger. */
    std::vector<float> m_thresholds_a; 	     // backward thresholds
    std::vector<float> m_thresholds_short_a; // backward thresholds with size 12
    std::vector<float> m_thresholds_c; 	     // forward thresholds 
    std::vector<float> m_thresholds_short_c; // forward thresholds with size 12 

    int m_cablestart_a{0}; // backward threshold cable start
    int m_cablestart_c{0}; // forward threshold cable start
    std::vector<int> m_cablestarts_a; // backward threshold cable starts
    std::vector<int> m_cablestarts_c; // forward threshold cable starts

    /** A data member containing the mapping from channel,phi to threshold number **/
    std::vector<std::vector<unsigned int> > m_thresholdNumber;
    std::vector<std::vector<unsigned int> > m_thresholdNumber12;

    /** The index of the sample bin corresponding to a particle arriving from the IP. */
    Gaudi::Property<unsigned int> m_tZeroBin{this, "TzeroBin", 3};
    float m_CFD_fraction{0.9};

    /** A flag to prevent the trigger simulation from running over bad input data.  */
    bool m_badDataFound{false};
    
    bool m_ThrVecSize12{false};
    /** Flag to indicate whether or not the individual counter thresholds
	are defined separately in the menu */
    //bool m_singleCounterInputs;
  };
}

#endif

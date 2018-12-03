// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1Muctpi.h 681356 2015-07-08 12:17:52Z wengler $
#ifndef TRIGT1MUCTPI_L1MUCTPI_H
#define TRIGT1MUCTPI_L1MUCTPI_H

// STL include(s):
#include <string>

// Athena/Gaudi include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration(s):
namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
}

/// Namespace for the MuCTPI simulation
/**
* This namespace should contain all classes, functions, enumerations, ... which are
* used in the MuCTPI simulation.
*/
namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MuctpiSim;

   /**
    *   $Date: 2015-07-08 14:17:52 +0200 (Wed, 08 Jul 2015) $
    *
    *   @short Main Athena algorithm of the MuCTPI simulation
    *
    *          The algorithm reads the MuCTPI's configuration from the DetectorStore
    *          put there by TrigT1Config, and configures the MuCTPI simulation with it.
    *          For each event it reads the output of the RPC and TGC detector simulations,
    *          and uses them as an input to the MuCTPI simulation. It produces a readout
    *          object (MuCTPI_RDO), an RoI object (ROIB::MuCTPIResult) and the object
    *          sent to the CTP (LVL1::MuCTPICTP).
    *
    *     @see MuctpiSim
    *     @see LVL1MUONIF::Lvl1MuCTPIInput
    *     @see MuCTPI_RDO
    *     @see ROIB::MuCTPIResult
    *     @see LVL1::MuCTPICTP
    *
    *  @author $Author: krasznaa $
    * @version $Revision: 681356 $
    *
    */
   class L1Muctpi : public AthAlgorithm {

   public:
      /// Regular Gaudi algorithm constructor
      L1Muctpi( const std::string& name, ISvcLocator* pSvcLocator );
      /// A destructor for actually cleaning up
      virtual ~L1Muctpi();

      /// Regular Gaudi algorithm initialization function
      virtual StatusCode initialize();
      /// Regular Gaudi algorithm finalization function
      virtual StatusCode finalize();
      /// Regular Gaudi algorithm execute function
      virtual StatusCode execute();
      /// Regular Gaudi algorithm beginRun function
      virtual StatusCode beginRun();

   private:
      /// Event loop method for running as part of digitization
      StatusCode executeFromDigi();
      /// Event loop method for running on an AOD file
      StatusCode executeFromAOD();
      /// Event loop method for running on an RDO file
      StatusCode executeFromRDO();
      /// Validate the muon threshold configuration
      StatusCode validate( const std::vector< TrigConf::TriggerThreshold* >& thresholds ) const;
      /// Update the data word for the RDO object
      StatusCode updateMuCTPI_RDO(uint32_t& can, std::vector< uint32_t >& dataWord);
      /// Save the MuCTPI RDO object
      StatusCode saveOutput_MuCTPI_RDO(uint32_t& can, std::vector< uint32_t >& dataWord);
      /// Save the outputs of the simulation into StoreGate
      StatusCode saveOutput(int bcidOffset = 0);

      /// The LVL1 configuration service
      ServiceHandle< TrigConf::ILVL1ConfigSvc > m_configSvc;

      /// The simulation top level object
      MuctpiSim* m_theMuctpi;

      /// The BCID slices to be checked in addition to the central one in the input objects from Digi
      std::vector<int> m_bcidOffsetList = {-2,-1,1,2};

      // Locations of the inputs and outputs of the simulation in StoreGate:
      static const std::string m_DEFAULT_locationMuCTPItoCTP;
      static const std::string m_DEFAULT_locationMuCTPItoL1Topo;
      static const std::string m_DEFAULT_locationMuCTPItoRoIB;
      static const std::string m_DEFAULT_L1MuctpiStoreLocationRPC;
      static const std::string m_DEFAULT_L1MuctpiStoreLocationTGC;
      static const std::string m_DEFAULT_AODLocID;
      static const std::string m_DEFAULT_RDOLocID;

      // These properties control the way the overlap handling functions:
      std::string m_overlapStrategyName;
      std::string m_lutXMLFile;
      std::string m_runPeriod;
      bool m_flagMode;

     // These properties control the way the outputs for the L1Topo are generated
     std::string m_geometryXMLFile;

      // These properties control how the multiplicity summation happens:
      std::string m_multiplicityStrategyName;
      std::string m_multiplicityXMLFile;

      // Property for the input selection, and the locations of the various
      // input and output objects:
      std::string m_inputSource;
      std::string m_aodLocId;
      std::string m_rdoLocId;
      std::string m_rdoOutputLocId;
      std::string m_roiOutputLocId;
      std::string m_ctpOutputLocId;
      std::string m_l1topoOutputLocId;
      std::string m_tgcLocId;
      std::string m_rpcLocId;

      /// Property telling if the LUTs should be printed:
      bool m_dumpLut;

      /// Property telling if input file is data or simulation 
      bool m_IsData;           

      // Properties controlling the NIM outputs provided by the simulation
      bool m_doNimOutput;
      std::string m_nimOutputLocId;
      unsigned int m_nimBarrelBit;
      unsigned int m_nimEndcapBit;
      
      /// Function pointer to the execute function we want to use:
      StatusCode ( LVL1MUCTPI::L1Muctpi::*m_executeFunction )( void );

   }; // class L1Muctpi

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_L1MUCTPI_H

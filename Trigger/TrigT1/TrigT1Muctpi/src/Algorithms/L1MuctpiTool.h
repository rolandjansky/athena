/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1MUCTPI_L1MUCTPITOOL_H
#define TRIGT1MUCTPI_L1MUCTPITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1Interfaces/IMuctpiSimTool.h"
#include <vector>
#include <string>

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"

namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
}


namespace LVL1MUCTPI {

  // Forward declaration(s):
  class MuctpiSim;

  class L1MuctpiTool : public extends2<AthAlgTool, IMuctpiSimTool, IIncidentListener> {
   public:
      L1MuctpiTool(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~L1MuctpiTool();

      virtual StatusCode initialize() override;

      virtual StatusCode fillMuCTPIL1Topo(LVL1::MuCTPIL1Topo&, int bcidOffset=0) const; 

      virtual void handle(const Incident&);


      // left over from ALgorithm implementation ... can I just leave them in here
      // and use them as normal funcitons?
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
      static const std::string m_DEFAULT_roibLocation;

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
      std::string m_roibLocation;

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
      StatusCode ( LVL1MUCTPI::L1MuctpiTool::*m_executeFunction )( void );

   };
}

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTMONITORING_DERIVESIMULATIONINPUTS_H
#define TRIGT1CTMONITORING_DERIVESIMULATIONINPUTS_H

// Basic includes:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// Forward includes:
class IAtRndmGenSvc;
class IMonitorToolBase;
class MuCTPI_RDO;


namespace LVL1MUCTPI {
  //class MuctpiSim;
  class Converter;
  class Configuration;
  //class L1Muctpi;
}

namespace LVL1 {
  class MuCTPICTP;
  class EmTauCTP;
  class JetCTP;
  class EnergyCTP;
  class MbtsCTP;
  class BcmCTP;
  class TrtCTP;
  class LucidCTP;
  class ZdcCTP;
  class BptxCTP;
  class NimCTP;
  class FrontPanelCTP;
}

namespace TrigConf {
  class ITrigConfigSvc;
  class DBLoader;
  class PIT;
  class TriggerThreshold;
  class TrigConfCoolFolderSpec;  
}


namespace TrigT1CTMonitoring {

   class DeriveSimulationInputs : public AthAlgorithm {
    
   public:
    
      DeriveSimulationInputs( const std::string& name, ISvcLocator* pSvcLocator );
      StatusCode DeriveL1CaloInput();
      StatusCode DeriveL1CaloInput_fromSim();
      StatusCode ReadInputMappingFromCool( IOVSVC_CALLBACK_ARGS );
      StatusCode fillPitMap(unsigned int pitNum);
      StatusCode fillFPIMap(unsigned int bitNum);
      StatusCode fillStoreGate(unsigned int ctpVersionNum);

      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();
        
      bool do_MuCTPI_input {false};
      bool do_L1Calo_input {false};
      bool do_L1Calo_sim   {false};
		
      unsigned int N_cablebits {31}; // same for CTPIN boards and front panel
      unsigned int N_slots_ctpin {3};
      unsigned int N_connectors_ctpin {4};
      unsigned int N_connectors_fp {3}; //front panel

      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

      int CTPINCables[31][3][4]; //[cableBits][slots][connectors] for inputs arriving at CTPIN boards
      int FPCables[32][3][2]; //[cableBits][connectors][clock] for inputs arriving at CTPCORE front panel (as of runII)
      std::vector<TrigConf::PIT*> m_pitVector;

      ~DeriveSimulationInputs();

   private:
    
      std::vector<std::string> m_par_atrlist;   
      std::vector<std::string> m_par_atrcollist;
      const DataHandle<CondAttrListCollection> m_ctpCoreMapping;
   }; 
}
#endif //
  

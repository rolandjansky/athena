/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CTMONITORING_DERIVESIMULATIONINPUTS_H
#define TRIGT1CTMONITORING_DERIVESIMULATIONINPUTS_H

// Basic includes:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/DataHandle.h"
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
    StatusCode ReadPitConfigFromCool();
    StatusCode fillPitMap(int pitNum, std::vector<TrigConf::PIT*> m_pitVector);
    StatusCode fillStoreGate();

    virtual StatusCode initialize();
    virtual StatusCode beginRun();
    virtual StatusCode execute();
    virtual StatusCode finalize();
        
    bool do_MuCTPI_input;
    bool do_L1Calo_input;
    bool do_L1Calo_sim;
    int N_cablebits;
    int N_slots;
    int N_connectors;

    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

    int Cables[31][3][4];
    std::vector<TrigConf::PIT*> m_pitVector;

    ~DeriveSimulationInputs();

  private:
    
    std::vector<std::string> par_atrlist;   
    std::vector<std::string> par_atrcollist;
    
  }; 
}
#endif //
  

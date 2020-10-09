/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Topo_LVL1_L1TopoSimulationTest_h
#define L1Topo_LVL1_L1TopoSimulationTest_h

#include "TrigConfBase/MsgStream.h"


#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "L1TopoCoreSim/TopoASCIIReader.h"

#include <memory>

class ITHistSvc;

namespace TCS {
  class TopoSteering;
}

namespace TrigConf {
   class IL1TopoConfigSvc;
}

namespace LVL1 {

  class L1TopoSimulationTest : public AthAlgorithm {
  public:
    L1TopoSimulationTest(const std::string &name, ISvcLocator* pSvcLocator);
    ~L1TopoSimulationTest();

    virtual StatusCode initialize() override;

    virtual StatusCode execute() override;

    virtual StatusCode finalize() override;

    // make algorithm is clonable
    virtual bool isClonable() const override;

  private:

    TCS::TopoASCIIReader m_Offreader;

    ServiceHandle<ITHistSvc> m_OffhistSvc;
    
    int m_OfftopoOutputLevel{TrigConf::MSGTC::WARNING};                                  // property to set the outputlevel of the topo algorithms
    int m_OfftopoSteeringOutputLevel{TrigConf::MSGTC::WARNING};                          // property to set the outputlevel of the topo steering

    StringProperty  m_OffhistBaseDir; //! sets base dir for monitoring histograms
    StringProperty  m_OffinputASCIIFile { "" }; // input dump file
    StringProperty  m_OffinputXMLFile { "" }; // XML file can be register in case of enabling the ASCII file read for validation purposes

    std::unique_ptr<TCS::TopoSteering>  m_OfftopoSteering; //!< the topo steering
     

  };

}
#endif

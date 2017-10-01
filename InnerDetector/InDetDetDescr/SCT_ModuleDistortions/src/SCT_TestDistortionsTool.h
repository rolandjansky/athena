/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_TestDistortionsTool_h
#define SCT_TestDistortionsTool_h

#include <string>
#include <memory>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TH2F.h"
#include "TH3F.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"



//Gaudi
#include "GaudiKernel/ToolHandle.h"

//package includes
#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"

class ITHistSvc;
class TH2I;
class TH2F;
class TH3F;
namespace InDetDD
{
  class SiDetectorManager;
}

class SCT_TestDistortionsTool:public AthAlgorithm{
  public:
    SCT_TestDistortionsTool( const std::string & name, ISvcLocator * pSvcLocator);
    ~SCT_TestDistortionsTool();
    // Standard Gaudi functions
    StatusCode initialize(); //!< Gaudi initialiser
    StatusCode execute();    //!< Gaudi executer
    StatusCode finalize();   //!< Gaudi finaliser
    
  private:
    ToolHandle<ISCT_ModuleDistortionsTool> m_SCTDistoTool;

    std::unique_ptr<TH2F> m_ZvsX;
    std::unique_ptr<TH2F> m_ZvsY;
    std::unique_ptr<TH3F> m_XYZ;
    std::unique_ptr<TH2F> m_outerXedge;
    std::unique_ptr<TH2F> m_outerYedge;
    std::unique_ptr<TH2F> m_outerX;
    std::unique_ptr<TH2F> m_outerY;

};
#endif



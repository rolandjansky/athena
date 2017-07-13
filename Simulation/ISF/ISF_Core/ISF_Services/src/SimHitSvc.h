/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_SERVICES_SIMHITSVC_H
#define ISF_SERVICES_SIMHITSVC_H 1

#include "ISF_Interfaces/ISimHitSvc.h"
#include "AthenaBaseComps/AthService.h"

#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"

#include "GaudiKernel/ToolHandle.h"

namespace ISF {

  /** @class SimHitSvc

      @author Andreas.Salzburger -at- cern.ch
  */

  class SimHitSvc : public ISimHitSvc, virtual public AthService {
  public:

    //** Constructor with parameters */
    SimHitSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    ~SimHitSvc();

    /** Initialize the event */
    StatusCode  initializeEvent() override final;

    /** Release the event */
    StatusCode releaseEvent() override final;

    /** Query the interfaces. **/
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override final;

  private:

    ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
    ToolHandle<IFastSimulationMasterTool> m_fastSimTool;

  };
}


#endif //> !ISF_SERVICES_SIMHITSVC_H

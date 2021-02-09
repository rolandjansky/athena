/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMSVCV2_H
#define ISF_FASTCALOSIMSVCV2_H 1

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

// FastCaloSim includes
#include "IFastCaloSimParamSvc.h"
#include "ISF_FastCaloSimParametrization/IFastCaloSimCaloExtrapolation.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"

#include "CaloInterface/ICaloCellMakerTool.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/TileID.h"

#include "ISF_FastCaloSimInterfaces/IPunchThroughTool.h"


namespace CLHEP
{
  class HepRandomEngine;
}

//forward declarations
class CaloCellContainer;
class TFCSParametrizationBase;


namespace ISF {

  class IParticleBroker;
  class IPunchThroughTool;

  /** @class FastCaloSimSvcV2

      @author Elmar.Ritsch -at- cern.ch, Geraldine.Conti -at- cern.ch, Flavia.Dias -at- cern.ch
  */

  class FastCaloSimSvcV2 : public BaseSimulationSvc
  {
  public:
    /** Constructor with parameters */
    FastCaloSimSvcV2(const std::string& name, ISvcLocator* pSvcLocator);

    /** Destructor */
    virtual ~FastCaloSimSvcV2() = default;

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override final;

    /** Simulation Call */
    virtual StatusCode simulate(const ISFParticle& isp) override final;

    /** Setup Event chain - in case of a begin-of event action is needed */
    virtual StatusCode setupEvent() override final;

    /** Release Event chain - in case of an end-of event action is needed */
    virtual StatusCode releaseEvent() override final;

    ServiceHandle<IFastCaloSimParamSvc> m_paramSvc;

    ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsSetup ;
    ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsRelease ;

    ToolHandle<IFastCaloSimCaloExtrapolation> m_FastCaloSimCaloExtrapolation;
    ToolHandle<Trk::ITimedExtrapolator> m_extrapolator;

    CaloCellContainer *       m_theContainer;

    ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
    CLHEP::HepRandomEngine*         m_randomEngine;
    std::string                     m_randomEngineName;

    std::string  m_caloCellsOutputName;

    bool m_doPunchThrough;
    ToolHandle< IPunchThroughTool >     m_punchThroughTool;
    ServiceHandle<ISF::IParticleBroker> m_particleBroker;

  };

}

#endif //> !ISF_FASTCALOSIMSVCV2_H

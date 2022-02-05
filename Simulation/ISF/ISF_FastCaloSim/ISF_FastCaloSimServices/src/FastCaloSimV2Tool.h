/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMV2TOOL_H
#define ISF_FASTCALOSIMV2TOOL_H 1

// ISF includes
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Interfaces/ITruthSvc.h"

// FastCaloSim includes
#include "IFastCaloSimParamSvc.h"
#include "ISF_FastCaloSimParametrization/IFastCaloSimCaloExtrapolation.h"
#include "ISF_FastCaloSimInterfaces/IPunchThroughTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"

#include "AthenaKernel/IAthRNGSvc.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/TileID.h"

namespace CLHEP
{
  class HepRandomEngine;
}

//forward declarations
class CaloCellContainer;

namespace ISF {
  /** @class FastCaloSimV2Tool

      @author Elmar.Ritsch -at- cern.ch, Geraldine.Conti -at- cern.ch, Flavia.Dias -at- cern.ch
  */

  class FastCaloSimV2Tool : public BaseSimulatorTool
  {
  public:
    /** Constructor with parameters */
    FastCaloSimV2Tool( const std::string& type, const std::string& name,  const IInterface* parent);

    /** Destructor */
    virtual ~FastCaloSimV2Tool() = default;

    /** Athena algorithm's interface methods */
    virtual StatusCode initialize() override final;

    /** Simulation Call */
    virtual StatusCode simulate(const ISFParticle& isp, ISFParticleContainer&, McEventCollection* mcEventCollection) const override final;

    /** Setup Event chain - in case of a begin-of event action is needed */
    virtual StatusCode setupEvent(const EventContext&) override final;

    virtual StatusCode setupEventST() override final;

    /** Release Event chain - in case of an end-of event action is needed */
    virtual StatusCode releaseEvent(const EventContext&) override final;

    virtual StatusCode releaseEventST() override final;

    virtual SimulationFlavor simFlavor() const override final { return ISF::FastCaloSimV2; };
  private:
    StatusCode commonSetup(const EventContext& ctx);

    ServiceHandle<IFastCaloSimParamSvc> m_paramSvc{this, "ParamSvc", "ISF_FastCaloSimV2ParamSvc"};
    bool m_doPunchThrough{false};
    ToolHandle< IPunchThroughTool >     m_punchThroughTool{this, "PunchThroughTool", ""};
    PublicToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsSetup{this, "CaloCellMakerTools_setup", {}, ""};
    PublicToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsRelease{this, "CaloCellMakerTools_release", {}, ""};

    PublicToolHandle<IFastCaloSimCaloExtrapolation> m_FastCaloSimCaloExtrapolation{this, "FastCaloSimCaloExtrapolation", "", ""};

    CaloCellContainer*        m_theContainer{};
    SG::WriteHandleKey< CaloCellContainer > m_caloCellKey{ this, "CaloCells", "DefaultCaloCellContainer", "The name of the output CaloCellContainer" };

    ServiceHandle<IAthRNGSvc> m_rndmGenSvc{this, "RandomSvc", "AthRNGSvc", ""};
    Gaudi::Property<std::string>    m_randomEngineName{this, "RandomStream", ""};
    Gaudi::Property<std::string>    m_caloCellsOutputName{this, "CaloCellsOutputName", "AllCalo"};
    ServiceHandle<ISF::ITruthSvc> m_truthRecordSvc{this,"ParticleTruthSvc", "ISF_TruthRecordSvc", "ISF Particle Truth Svc"};
  };

}

#endif //> !ISF_FASTCALOSIMV2TOOL_H

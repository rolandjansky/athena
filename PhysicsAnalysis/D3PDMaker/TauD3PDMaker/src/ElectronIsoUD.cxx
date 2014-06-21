/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ElectronIsoUD.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODEgamma/Electron.h"
#include "egammaEvent/ElectronContainer.h"
#include "IsolationTool/ITrackIsolationTool.h"

#include <string>



ElectronIsoUD::ElectronIsoUD (const std::string& name, ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_getter (this),
    m_track_iso_tool("TrackIsolationTool")
{
    declareProperty ("Getter", m_getter, "Getter instance for the input objects.");
    declareProperty ("track_iso_tool", m_track_iso_tool );
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode ElectronIsoUD::initialize()
{
    CHECK( AthAlgorithm::initialize() );
    CHECK( m_getter.retrieve() );
    CHECK( m_getter->configureD3PD<xAOD::Electron>() );
    CHECK( m_track_iso_tool.retrieve() );
    return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode ElectronIsoUD::execute()
{
    CHECK( m_getter->reset() );
    while (const xAOD::Electron* e = m_getter->next<xAOD::Electron>())
    {
        const xAOD::TrackParticle* track = e->trackParticle();

        xAOD::TrackIsolation result;
        std::vector<xAOD::Iso::IsolationType> cones = { xAOD::Iso::ptcone20,
                                                        xAOD::Iso::ptcone30,
                                                        xAOD::Iso::ptcone40 };
        if (track && m_track_iso_tool->trackIsolation (result, *track, cones)) {
          assert (result.ptcones.size() == 3);
          assert (result.nucones.size() == 3);

          static SG::AuxElement::Decorator<int> nucone20 ("nucone20");
          static SG::AuxElement::Decorator<int> nucone30 ("nucone30");
          static SG::AuxElement::Decorator<int> nucone40 ("nucone40");
          static SG::AuxElement::Decorator<float> ptcone20 ("ptcone20");
          static SG::AuxElement::Decorator<float> ptcone30 ("ptcone30");
          static SG::AuxElement::Decorator<float> ptcone40 ("ptcone40");

          nucone20(*e) = result.nucones[0];
          nucone30(*e) = result.nucones[1];
          nucone40(*e) = result.nucones[2];
          ptcone20(*e) = result.ptcones[0];
          ptcone30(*e) = result.ptcones[1];
          ptcone40(*e) = result.ptcones[2];
        }
    }
    return StatusCode::SUCCESS;
}



/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MetAnalysisAlgorithms/MetMakerAlg.h>

#include <xAODMissingET/MissingETAuxContainer.h>

//
// method implementations
//

namespace CP
{
  MetMakerAlg ::
  MetMakerAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_makerTool ("METMaker", this)
  {
    declareProperty ("makerTool", m_makerTool, "the METMaker tool we apply");
    declareProperty ("metCore", m_metCoreName, "the name of the core MissingETContainer");
    declareProperty ("metAssociation", m_metAssociationName, "the name of the core MissingETContainer");
    declareProperty ("particlesType", m_particlesType, "the type of the particle container in particles");
    declareProperty ("particlesKey", m_particlesKey, "the key of the particle container in particles");
    declareProperty ("metJetKey", m_metJetKey, "the key for jets");
    declareProperty ("softTerm", m_softTerm, "the soft term key");
    declareProperty ("doTrackMet", m_doTrackMet, "whether to use track-met instead of jet-met");
    declareProperty ("doJetJVT", m_doJetJVT, "whether to do jet JVT");
  }



  StatusCode MetMakerAlg ::
  initialize ()
  {
    ANA_CHECK (m_makerTool.retrieve());
    ANA_CHECK (m_particlesHandle.initialize());
    m_systematicsList.addHandle (m_particlesHandle);
    m_systematicsList.addHandle (m_jetsHandle);
    m_systematicsList.addHandle (m_metHandle);
    ANA_CHECK (m_systematicsList.initialize());

    if (m_particlesType.size() != m_particlesHandle.size())
    {
      ANA_MSG_ERROR ("size mismatch between particles ( " << m_particlesHandle.size() << ") and particlesType (" << m_particlesType.size() << ") properties");
      return StatusCode::FAILURE;
    }
    if (m_particlesKey.size() != m_particlesHandle.size())
    {
      ANA_MSG_ERROR ("size mismatch between particles ( " << m_particlesHandle.size() << ") and particlesKey (" << m_particlesKey.size() << ") properties");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode MetMakerAlg ::
  execute ()
  {
    const xAOD::MissingETContainer* metcore {nullptr};
    ANA_CHECK (evtStore()->retrieve(metcore, m_metCoreName));

    const xAOD::MissingETAssociationMap* metMap {nullptr};
    ANA_CHECK (evtStore()->retrieve(metMap, m_metAssociationName));

    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        auto met = std::make_unique<xAOD::MissingETContainer> ();
        auto aux = std::make_unique<xAOD::MissingETAuxContainer> ();
        met->setStore (aux.get());

        metMap->resetObjSelectionFlags();

        for (std::size_t iter = 0, end = m_particlesHandle.size();
             iter != end; ++ iter)
        {
          const xAOD::IParticleContainer *particles {nullptr};
          ANA_CHECK (m_particlesHandle.retrieve (particles, sys, iter));
          ANA_CHECK (m_makerTool->rebuildMET (m_particlesKey[iter], xAOD::Type::ObjectType (m_particlesType[iter]), met.get(), particles, metMap));
        }

        const xAOD::JetContainer *jets {nullptr};
        ANA_CHECK (m_jetsHandle.retrieve (jets, sys));
        if (m_doTrackMet)
        {
          ANA_CHECK (m_makerTool->rebuildTrackMET (m_metJetKey, m_softTerm, met.get(), jets, metcore, metMap, m_doJetJVT));
        } else
        {
          ANA_CHECK (m_makerTool->rebuildJetMET (m_metJetKey, m_softTerm, met.get(), jets, metcore, metMap, m_doJetJVT));
        }

        ANA_CHECK (m_metHandle.record (std::move (met), std::move (aux), sys));
        return StatusCode::SUCCESS;
      });
  }
}

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef MET_ANALYSIS_ALGORITHMS__MET_MAKER_ALG_H
#define MET_ANALYSIS_ALGORITHMS__MET_MAKER_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <SystematicsHandles/SysWriteHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <METInterface/IMETMaker.h>

#include <xAODBase/IParticleContainer.h>
#include <xAODMissingET/MissingETContainer.h>
#include <xAODMissingET/MissingETAuxContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IMETMaker
  ///
  /// This algorithm is fairly complex for a common CP algorithm.  The
  /// main issue here is that the MET tools store temporary
  /// information on the xAOD objects that gets reset on each
  /// systematic, so a lot of actions have to happen in one go.
  /// Despite that complexity it still can't be run on its own, you
  /// always have to call \ref MetBuilderAlg afterwards to build the
  /// final MET.

  class MetMakerAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    MetMakerAlg (const std::string& name, 
                 ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IMETMaker> m_makerTool;

    /// \brief the name of the core MissingETContainer
  private:
    std::string m_metCoreName;

    /// \brief the name of the MissingETAssociationMap
  private:
    std::string m_metAssociationName;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the electron container to use
  private:
    SysReadHandle<xAOD::IParticleContainer> m_electronsHandle {
      this, "electrons", "", "the electron container to use" };

    /// \brief the key for \ref m_electronsHandle
  private:
    std::string m_electronsKey {"RefEle"};

    /// \brief the photon container to use
  private:
    SysReadHandle<xAOD::IParticleContainer> m_photonsHandle {
      this, "photons", "", "the photon container to use" };

    /// \brief the key for \ref m_photonsHandle
  private:
    std::string m_photonsKey {"RefGamma"};

    /// \brief the muon container to use
  private:
    SysReadHandle<xAOD::IParticleContainer> m_muonsHandle {
      this, "muons", "", "the muon container to use" };

    /// \brief the key for \ref m_muonsHandle
  private:
    std::string m_muonsKey {"Muons"};

    /// \brief the electron container to use
  private:
    SysReadHandle<xAOD::IParticleContainer> m_tausHandle {
      this, "taus", "", "the tau container to use" };

    /// \brief the key for \ref m_tausHandle
  private:
    std::string m_tausKey {"RefTau"};

    /// \brief the input jet collection we run on
  private:
    SysReadHandle<xAOD::JetContainer> m_jetsHandle {
      this, "jets", "", "the jet collection we use"};

  private:
    SysReadHandle<xAOD::IParticleContainer> m_invisHandle {
      this, "invisible", "", "Any particles to treat as invisible."};

    /// \brief the key for \ref m_jetsHandle
  private:
    std::string m_jetsKey {"RefJet"};

    /// \brief the soft term key
  private:
    std::string m_softTermKey {"PVSoftTrk"};

    /// \brief whether to use track-met instead of jet-met
  private:
    bool m_doTrackMet {false};

    /// \brief whether to do jet JVT
  private:
    bool m_doJetJVT {true};

    /// \brief the met collection we run on
  private:
    SysWriteHandle<xAOD::MissingETContainer,xAOD::MissingETAuxContainer> m_metHandle {
      this, "met", "MissingET_%SYS%", "the met collection we produce"};
  };
}

#endif

/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: ObjectCollectionMaker.h 790414 2016-12-19 00:49:08Z tpelzer $
#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_OBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_OBJECTCOLLECTIONMAKER_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief ObjectCollectionMaker
 *   Makes all systematic variations of all objects
 *
 * $Revision: 790414 $
 * $Date: 2016-12-19 00:49:08 +0000 (Mon, 19 Dec 2016) $
 *
 **/

// system include(s):
#include <vector>
#include <memory>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

#include "TopSystematicObjectMaker/EgammaObjectCollectionMaker.h"
#include "TopSystematicObjectMaker/MuonObjectCollectionMaker.h"
#include "TopSystematicObjectMaker/SoftMuonObjectCollectionMaker.h"
#include "TopSystematicObjectMaker/TauObjectCollectionMaker.h"
#include "TopSystematicObjectMaker/JetObjectCollectionMaker.h"
#include "TopSystematicObjectMaker/MissingETObjectCollectionMaker.h"
#include "TopSystematicObjectMaker/GhostTrackSystematicsMaker.h"
#include "TopSystematicObjectMaker/TrackSystematicsMaker.h"

// Forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class ObjectCollectionMaker final: public asg::AsgTool {
  public:
    explicit ObjectCollectionMaker(const std::string& name);
    virtual ~ObjectCollectionMaker() {}

    ObjectCollectionMaker(const ObjectCollectionMaker& rhs) = delete;
    ObjectCollectionMaker(ObjectCollectionMaker&& rhs) = delete;
    ObjectCollectionMaker& operator = (const ObjectCollectionMaker& rhs) = delete;

    StatusCode initialize();
    StatusCode execute(bool);
    StatusCode executeNominal();
    StatusCode executeSystematics();
    StatusCode recalculateMET(bool);
    StatusCode printout();

    bool isTruthDxAOD() const;
  private:
    std::shared_ptr<top::TopConfig> m_config;
    std::unique_ptr<top::EgammaObjectCollectionMaker>    m_egammaMaker;
    std::unique_ptr<top::MuonObjectCollectionMaker>      m_muonMaker;
    std::unique_ptr<top::SoftMuonObjectCollectionMaker>      m_softmuonMaker;
    std::unique_ptr<top::TauObjectCollectionMaker>       m_tauMaker;
    std::unique_ptr<top::JetObjectCollectionMaker>       m_jetMaker;
    std::unique_ptr<top::MissingETObjectCollectionMaker> m_metMaker;
    std::unique_ptr<top::GhostTrackSystematicsMaker>     m_ghostTrackSystMaker;
    std::unique_ptr<top::TrackSystematicsMaker>          m_trackSystMaker;

  };
} // namespace

#endif

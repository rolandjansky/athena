/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackClassifier_TrackClassifier_H
#define TrackClassifier_TrackClassifier_H

#include <AsgTools/AsgTool.h>
#include <FlavorTagDiscriminants/ITrackClassifier.h>

#include <xAODJet/JetContainer.h>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <memory>
#include <fstream>
#include "lwtnn/lightweight_network_config.hh"
#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"

#include <math.h>

namespace FlavorTagDiscriminants {
  class TrackClassifier final : public asg::AsgTool, virtual public ITrackClassifier
  {

    ASG_TOOL_CLASS( TrackClassifier, ITrackClassifier)
  
  public:
    // this is a standard tool constructor
    TrackClassifier (const std::string& name);
    virtual ~TrackClassifier();

    // inherited interface
    virtual StatusCode initialize () override;
    virtual double compute_HF_Score(const xAOD::TrackParticle* track, const xAOD::Jet* jet) const;

  private:

    std::unique_ptr<lwt::LightweightGraph> m_lwtnn_network;
    std::string m_NNModelFilepath;
    
    int get(const xAOD::TrackParticle* part, xAOD::SummaryType info) const;
    std::map<std::string, double> ComputeScore( const std::unique_ptr<lwt::LightweightGraph>& net, const xAOD::TrackParticle* track, const xAOD::Jet* jet) const;
  };
}
#endif

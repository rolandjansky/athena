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
    virtual std::map<std::string, double> ComputeScore( const xAOD::TrackParticle* track, const xAOD::Jet* jet) const override;
    virtual double compute_HF_Score(const xAOD::TrackParticle* track, const xAOD::Jet* jet) const override;
    virtual bool selectTrack(const xAOD::TrackParticle* track, const xAOD::Jet* jet) const override;

  private:

    std::unique_ptr<lwt::LightweightGraph> m_lwtnn_network;
    std::string m_NNModelFilepath;

    // the HF score cut in each jet pt bin (expressed in GeV),
    //resembling 99% WP signal (heavy flavor tracks) efficiency in singleB jets:
    const std::map<double, double>
      m_WPcuts = {
      {0.0, -1.},
      {250.0, 0.08892564848065376},
      {500.0, 0.046739218756556504},
      {750.0, 0.03829977475106716},
      {1000.0, 0.033665632829070084},
      {1500.0, 0.033094046637415886},
      {2000.0, 0.029787263832986355},
      {2500.0, 0.02972860541194677},
      {3000.0, 0.029779289849102497},
      {4000.0, 0.026585028506815437},
      {5000.0, 0.025235182605683804}
    };
    
    int get(const xAOD::TrackParticle* part, xAOD::SummaryType info) const;
    bool pass_cut(const double score, const xAOD::Jet* jet) const;

  };
}
#endif

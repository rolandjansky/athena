/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <FlavorTagDiscriminants/TrackClassifier.h>
#include <numeric>

namespace FlavorTagDiscriminants {
TrackClassifier :: TrackClassifier (const std::string& name) : AsgTool (name)
{
  declareProperty( "NNModelFilepath", m_NNModelFilepath = "neural_net.json", "Trained and exported ltwnn model");
}

TrackClassifier::~TrackClassifier() {}

StatusCode TrackClassifier :: initialize ()
{
    // load the trained model
    ATH_MSG_INFO ("Loading lwtnn model for fake track classification: " << m_NNModelFilepath);
    std::ifstream inFileNN(m_NNModelFilepath);
    lwt::GraphConfig config(lwt::parse_json_graph(inFileNN));
    m_lwtnn_network.reset(new lwt::LightweightGraph(config));
    inFileNN.close();

  return StatusCode::SUCCESS;
}

int TrackClassifier :: get(const xAOD::TrackParticle* track, xAOD::SummaryType info) const {
  uint8_t val;
  bool ok = track->summaryValue(val, info);
  if (!ok) throw std::logic_error("problem getting track summary value");
  return val;
}


double TrackClassifier :: compute_HF_Score(const xAOD::TrackParticle *track, const xAOD::Jet *jet) const
{ 
  std::map<std::string, double> track_outputs = ComputeScore( m_lwtnn_network, track, jet );
  double HF_score = track_outputs["FromB"]+track_outputs["FromBC"]+track_outputs["FromC"];
  return HF_score;
}


std::map<std::string, double> TrackClassifier::ComputeScore(const std::unique_ptr<lwt::LightweightGraph>& net, const xAOD::TrackParticle* track, const xAOD::Jet* jet) const
{
  double dphi = (jet->p4()).DeltaPhi(track->p4());  
  double deta = jet->eta() - track->eta();
  double dr = (track->p4()).DeltaR(jet->p4());
  double ptfrac = (track->pt())/(jet->pt());

  // Build dictionary of inputs for lwtnn to use
  // It is ok to fill this with more variables than the model uses
  // as long as no variables are missing
  std::map<std::string, double> track_inputs{
        {"eta_btagJes",            (double) jet->eta()},
	{"log_pt_btagJes",         (double) std::log(jet->pt()*0.001)},//GeV
	{"dphi",                   (double) dphi},
	{"deta",                   (double) deta},
	{"log_dr",                 (double) std::log(dr)},
	{"log_ptfrac",             (double) std::log(ptfrac)},
        {"qOverP",                 (double) track->qOverP()},
	{"numberOfPixelHits",               (double) get(track, xAOD::numberOfPixelHits)},
	{"numberOfSCTHits",               (double) get(track, xAOD::numberOfSCTHits)},
	{"numberOfInnermostPixelLayerHits",               (double) get(track, xAOD::numberOfInnermostPixelLayerHits)},
	{"numberOfNextToInnermostPixelLayerHits",           (double) get(track, xAOD::numberOfNextToInnermostPixelLayerHits)},
	{"numberOfInnermostPixelLayerSharedHits",             (double) get(track, xAOD::numberOfInnermostPixelLayerSharedHits)},
	{"numberOfInnermostPixelLayerSplitHits",              (double) get(track, xAOD::numberOfInnermostPixelLayerSplitHits)},
	{"numberOfPixelSharedHits",             (double) get(track, xAOD::numberOfPixelSharedHits)},
	{"numberOfPixelSplitHits",              (double) get(track, xAOD::numberOfPixelSplitHits)},
        {"numberOfSCTSharedHits",             (double) get(track, xAOD::numberOfSCTSharedHits)},
	{"numberOfPixelHoles",              (double) get(track, xAOD::numberOfPixelHoles)},
	{"numberOfSCTHoles",              (double) get(track, xAOD::numberOfSCTHoles)},
	{"AMVFWeightPV",           (double) track->auxdata<float>("AMVFWeightPV")}
    };

  // Set up the nodes used for inputs
  std::map<std::string, std::map<std::string, double> > inputs {
        {"track_inputs", track_inputs}
  };

  // Evaluate the network
  lwt::ValueMap discriminant = net->compute(inputs);

  std::map<std::string, double> track_outputs{
    {"Pileup",(double) discriminant["Pileup"]},
    {"Fake",(double) discriminant["Fake"]},
    {"Primary",(double) discriminant["Primary"]},
    {"FromB",(double) discriminant["FromB"]},
    {"FromBC",(double) discriminant["FromBC"]},
    {"FromC",(double) discriminant["FromC"]},
    {"FromTau",(double) discriminant["FromTau"]},
    {"OtherSecondary",(double) discriminant["OtherSecondary"]}
  };

  return track_outputs;
}
}

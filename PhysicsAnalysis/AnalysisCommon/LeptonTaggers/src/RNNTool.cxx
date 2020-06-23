
// Local
#include "RNNTool.h"

// External
#include "lwtnn/parse_json.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

// C/C++
#include <fstream>

using namespace std;

//=============================================================================
Prompt::RNNTool::RNNTool(const std::string &name, const std::string &type, const IInterface *parent):
  AthAlgTool(name, type, parent)
{
  declareInterface<Prompt::IRNNTool>(this);

  declareProperty("configPathRNN",     m_configPathRNN,                    "Path of the local RNN json file you want o study/test, it will override the PathResolverFindCalibFile file");  
  declareProperty("configRNNVersion",  m_configRNNVersion,                 "RNN version in cvmfs");  
  declareProperty("configRNNJsonFile", m_configRNNJsonFile,                "Name of the RNN json file in cvmfs");  
  declareProperty("debug",             m_debug,                            "Help to debug the RNNTool");

  declareProperty("inputSequenceName", m_inputSequenceName = "Trk_inputs", "Prefix of the variables used in the RNN json file");
  declareProperty("inputSequenceSize", m_inputSequenceSize = 5,            "Number of tracks used in the RNN"); 
}

//=============================================================================
StatusCode Prompt::RNNTool::initialize()
{
  //
  // Get path to xml training file
  //
  std::string fullPathToFile;

  if(!m_configPathRNN.empty()) {
    ATH_MSG_INFO("Override PathResolver to this path: " << m_configPathRNN);
    fullPathToFile = m_configPathRNN;
  }
  else {
    fullPathToFile = PathResolverFindCalibFile("JetTagNonPromptLepton/"
                                               + m_configRNNVersion + "/"
                                               + m_configRNNJsonFile);
  }

  ATH_MSG_INFO("initialize RNNTool - ConfigPathRNN: \"" << fullPathToFile);

  //
  // Configure RNN
  //
  std::ifstream input_stream(fullPathToFile);
  
  lwt::GraphConfig graph_config = lwt::parse_json_graph(input_stream);  

  m_graph = std::make_unique<lwt::LightweightGraph>(graph_config);

  for(const auto &o: graph_config.outputs) {
    ATH_MSG_INFO("  output name: " << o.first << ", node_index=" << o.second.node_index);

    for(const auto &l: o.second.labels) {
      ATH_MSG_INFO("      label=" << l);

      if(!m_outputLabels.insert(l).second) {
	ATH_MSG_WARNING("Duplicate output label=\"" << l << "\"");
      } 
    }
  }

  ATH_MSG_INFO("Number of input sequences: " << graph_config.input_sequences.size());

  for(const auto &n: graph_config.input_sequences) {
    ATH_MSG_INFO("  sequence name=" << n.name);

    for(const lwt::Input &v: n.variables) {
      ATH_MSG_INFO("    variable=" << v.name);
    }
  }

  ATH_MSG_INFO("Number of inputs: " << graph_config.inputs.size());
  
  for(const auto &n: graph_config.inputs) {
    ATH_MSG_INFO("  input name=" << n.name);

    for(const lwt::Input &v: n.variables) {
      ATH_MSG_INFO("    variable=" << v.name);
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
std::map<std::string, double> Prompt::RNNTool::computeRNNOutput(const std::vector<Prompt::VarHolder> &tracks)
{
  lwt::ValueMap values;

  lwt::LightweightGraph::NodeMap nodes;
  lwt::LightweightGraph::SeqNodeMap seqs;

  lwt::VectorMap &vmap = seqs[m_inputSequenceName];

  AddVariable(tracks, Def::NumberOfPIXHits,       vmap["m_cone_tracks_numberOfPixelHits"]);
  AddVariable(tracks, Def::NumberOfSCTHits,       vmap["m_cone_tracks_numberOfSCTHits"]);
  AddVariable(tracks, Def::Z0Sin,                 vmap["m_cone_tracks_Z0Sin"]);
  AddVariable(tracks, Def::D0Sig,                 vmap["m_cone_tracks_D0Sig"]);
  AddVariable(tracks, Def::TrackJetDR,            vmap["m_cone_tracks_DRTrackJet"]);
  AddVariable(tracks, Def::TrackPtOverTrackJetPt, vmap["m_cone_tracks_PtRelOverTrackJetPt"]);

  if(vmap.size() != 6) {
    ATH_MSG_WARNING("RNNTool::computeRNNOutput - incomplete variables: return empty result");    
    return lwt::ValueMap();
  }

  unsigned nwid = 0;

  for(const lwt::VectorMap::value_type &v: vmap) {
    nwid = std::max<unsigned>(v.first.size(), nwid);
  }

  for(const lwt::VectorMap::value_type &v: vmap) {
    ATH_MSG_DEBUG(std::setw(nwid+1) << std::left << v.first << " ");

    for(const double d: v.second) {
      ATH_MSG_DEBUG(d << ", ");
    }
  }
  
  lwt::ValueMap result = m_graph->compute(nodes, seqs);

  for(const lwt::ValueMap::value_type &v: result) {
    ATH_MSG_DEBUG(v.first << " score=" << std::setprecision(10) << v.second);
  }
  
  return result;
}

//=============================================================================
void Prompt::RNNTool::AddVariable(const std::vector<Prompt::VarHolder> &tracks,
			          unsigned var,
			          std::vector<double> &values)
{
  //
  // Read values
  //
  const unsigned nvar = std::min<unsigned>(tracks.size(), m_inputSequenceSize);

  for(unsigned i = 0; i < nvar; ++i) {
    double value = 0.0;

    if(i < tracks.size()) {
      const Prompt::VarHolder &track = tracks.at(i);
      
      if(!track.GetVar(var, value)) {
	ATH_MSG_WARNING("RNNTool::AddVariable - missing variable: " + Def::AsStr(Def::Convert2Var(var)));
      }
    }
    
    values.push_back(value);
  }
}

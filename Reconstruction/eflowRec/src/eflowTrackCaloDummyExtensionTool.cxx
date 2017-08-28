/********************************************************************

 NAME:     eflowTrackCaloDummyExtensionTool.cxx
 PACKAGE:  offline/Reconstruction/eflowRec

 AUTHORS:  T.Guttenberger
 CREATED:  19th September, 2014

 ********************************************************************/

#include <eflowRec/eflowTrackCaloDummyExtensionTool.h>

#include "eflowRec/eflowTrackCaloPoints.h"

#include "TrkParameters/TrackParameters.h"  // typedef
#include "xAODTracking/Vertex.h"  // typedef

#include "GaudiKernel/ListItem.h"
//#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <utility>

#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TF1.h"


//using std::pair;

std::vector<eflowCalo::LAYER> getLayerList(float trketa) {
  std::vector<eflowCalo::LAYER> list;
  for(unsigned int ilayer=eflowCalo::EMB1; ilayer<eflowCalo::nRegions; ++ilayer) {
    eflowCalo::LAYER layer = eflowCalo::LAYER(ilayer);
    switch(layer) {
    case eflowCalo::EMB1:
    case eflowCalo::EMB2:
      if(fabs(trketa)<1.475) {list.push_back(layer);}
      break;
    case eflowCalo::EMB3:
      if(fabs(trketa)<1.35) {list.push_back(layer);}
      break;

    case eflowCalo::EME1:
    case eflowCalo::EME2:
      if(fabs(trketa)>1.375 && fabs(trketa)<3.2) {list.push_back(layer);}
      break;
    case eflowCalo::EME3:
      if(fabs(trketa)>1.5 && fabs(trketa)<2.5) {list.push_back(layer);}
      break;

    case eflowCalo::Tile1:
    case eflowCalo::Tile2:
    case eflowCalo::Tile3:
      if(fabs(trketa)<1.7) {list.push_back(layer);}
      break;
  
    case eflowCalo::HEC1:
    case eflowCalo::HEC2:
    case eflowCalo::HEC3:
    case eflowCalo::HEC4:
      if(fabs(trketa)>1.5) {list.push_back(layer);}
      break;
    default:
      break;
    }
  }
  return list;
}

namespace SimpleExtrap {
  const static SG::AuxElement::Decorator<float> dec_etaEM1("eta_EM1_simple");
  const static SG::AuxElement::Decorator<float> dec_phiEM1("phi_EM1_simple");
  const static SG::AuxElement::Decorator<float> dec_etaEM2("eta_EM2_simple");
  const static SG::AuxElement::Decorator<float> dec_phiEM2("phi_EM2_simple");
  const static SG::AuxElement::Decorator<float> dec_etaEM3("eta_EM3_simple");
  const static SG::AuxElement::Decorator<float> dec_phiEM3("phi_EM3_simple");

  const static SG::AuxElement::Decorator<float> dec_etaHAD1("eta_HAD1_simple");
  const static SG::AuxElement::Decorator<float> dec_phiHAD1("phi_HAD1_simple");
  const static SG::AuxElement::Decorator<float> dec_etaHAD2("eta_HAD2_simple");
  const static SG::AuxElement::Decorator<float> dec_phiHAD2("phi_HAD2_simple");
  const static SG::AuxElement::Decorator<float> dec_etaHAD3("eta_HAD3_simple");
  const static SG::AuxElement::Decorator<float> dec_phiHAD3("phi_HAD3_simple");
  const static SG::AuxElement::Decorator<float> dec_etaHAD4("eta_HAD4_simple");
  const static SG::AuxElement::Decorator<float> dec_phiHAD4("phi_HAD4_simple");

  void decorateWithLayerEtaPhi(const xAOD::TrackParticle& tp, const eflowTrackCaloPoints& tcp) {
    for(unsigned int ilayer=eflowCalo::EMB1; ilayer<eflowCalo::nRegions; ++ilayer) {
      eflowCalo::LAYER layer = eflowCalo::LAYER(ilayer);
      float etalayer = tcp.getEta(layer);
      float philayer = tcp.getPhi(layer);
      if(etalayer>-998) {
	switch(layer) {
	case eflowCalo::EMB1:
	case eflowCalo::EME1:
	  dec_etaEM1(tp) = etalayer;
	  dec_phiEM1(tp) = philayer;
	  break;
	case eflowCalo::EMB2:
	case eflowCalo::EME2:
	  dec_etaEM2(tp) = etalayer;
	  dec_phiEM2(tp) = philayer;
	  break;
	case eflowCalo::EMB3:
	case eflowCalo::EME3:
	  dec_etaEM3(tp) = etalayer;
	  dec_phiEM3(tp) = philayer;
	  break;
	case eflowCalo::HEC1:
	case eflowCalo::Tile1:
	  dec_etaHAD1(tp) = etalayer;
	  dec_phiHAD1(tp) = philayer;
	  break;
	case eflowCalo::HEC2:
	case eflowCalo::Tile2:
	  dec_etaHAD2(tp) = etalayer;
	  dec_phiHAD2(tp) = philayer;
	  break;
	case eflowCalo::HEC3:
	case eflowCalo::Tile3:
	  dec_etaHAD3(tp) = etalayer;
	  dec_phiHAD3(tp) = philayer;
	  break;
	case eflowCalo::HEC4:
	  dec_etaHAD4(tp) = etalayer;
	  dec_phiHAD4(tp) = philayer;
	  break;
	default:
	  break;
	}
      }
    }
  }
}

eflowTrackCaloDummyExtensionTool::eflowTrackCaloDummyExtensionTool(const std::string& type, const std::string& name, const IInterface* parent)  :
    AthAlgTool(type, name, parent) {
  declareInterface<eflowTrackExtrapolatorBaseAlgTool>(this);

  declareProperty( "doMapEstimation",          m_doMapEstimation   = false);
  declareProperty( "mapEstimationPath",        m_mapEstimationPath = "eflowRec/data/");//The last slash is needed!
  declareProperty( "ParameterGraphName",       m_parameterGraphName = "Paramter_vs_eta");

}

eflowTrackCaloDummyExtensionTool::~eflowTrackCaloDummyExtensionTool() {
}

StatusCode eflowTrackCaloDummyExtensionTool::initialize() {

  /* Loading estimation map. */
  if(m_doMapEstimation){
 
    std::string namefile_had1 = PathResolver::find_file(m_mapEstimationPath+"Hist_HAD1.root","DATAPATH");
    FillEstimationMap(namefile_had1.c_str(),m_parameterGraphName.c_str(),eflowCalo::HEC1);
    FillEstimationMap(namefile_had1.c_str(),m_parameterGraphName.c_str(),eflowCalo::Tile1);
    ATH_MSG_INFO("Loading HAD1 extrapolation file from "<< namefile_had1 << ".");
 
    std::string namefile_had2 = PathResolver::find_file(m_mapEstimationPath+"Hist_HAD2.root","DATAPATH");
    FillEstimationMap(namefile_had2.c_str(),m_parameterGraphName.c_str(),eflowCalo::HEC2);
    FillEstimationMap(namefile_had2.c_str(),m_parameterGraphName.c_str(),eflowCalo::Tile2);
    ATH_MSG_INFO("Loading HAD2 extrapolation file from "<< namefile_had2 << ".");
 
    std::string namefile_had3 = PathResolver::find_file(m_mapEstimationPath+"Hist_HAD3.root","DATAPATH");
    FillEstimationMap(namefile_had3.c_str(),m_parameterGraphName.c_str(),eflowCalo::HEC3);
    FillEstimationMap(namefile_had3.c_str(),m_parameterGraphName.c_str(),eflowCalo::Tile3);
    ATH_MSG_INFO("Loading HAD3 extrapolation file from "<< namefile_had3 << ".");
 
    //FIX THIS
    std::string namefile_had4 = PathResolver::find_file(m_mapEstimationPath+"Hist_HAD4.root","DATAPATH");
    FillEstimationMap(namefile_had4.c_str(),m_parameterGraphName.c_str(),eflowCalo::HEC4);
    ATH_MSG_INFO("Loading HAD4 extrapolation file from "<< namefile_had4 << ".");
 
    std::string namefile_em1  = PathResolver::find_file(m_mapEstimationPath+"Hist_EM1.root","DATAPATH");
    FillEstimationMap(namefile_em1.c_str(),m_parameterGraphName.c_str(),eflowCalo::EMB1);
    FillEstimationMap(namefile_em1.c_str(),m_parameterGraphName.c_str(),eflowCalo::EME1);
    ATH_MSG_INFO("Loading EM1 extrapolation file from "<< namefile_em1 << ".");
 
    std::string namefile_em2  = PathResolver::find_file(m_mapEstimationPath+"Hist_EM2.root","DATAPATH");
    FillEstimationMap(namefile_em2.c_str(),m_parameterGraphName.c_str(),eflowCalo::EMB2);
    FillEstimationMap(namefile_em2.c_str(),m_parameterGraphName.c_str(),eflowCalo::EME2);
    ATH_MSG_INFO("Loading EM2 extrapolation file from "<< namefile_em2 << ".");
 
    std::string namefile_em3  = PathResolver::find_file(m_mapEstimationPath+"Hist_EM3.root","DATAPATH");
    FillEstimationMap(namefile_em3.c_str(),m_parameterGraphName.c_str(),eflowCalo::EMB3);
    FillEstimationMap(namefile_em3.c_str(),m_parameterGraphName.c_str(),eflowCalo::EME3);
    ATH_MSG_INFO("Loading EM3 extrapolation file from "<< namefile_em3 << ".");
  }

  return StatusCode::SUCCESS;
}

std::unique_ptr<eflowTrackCaloPoints> eflowTrackCaloDummyExtensionTool::execute(const xAOD::TrackParticle* track) const {
  msg(MSG::VERBOSE) << " Now running eflowTrackCaloDummyExtensionTool" << endmsg;

  float trackP = fabs(1./track->qOverP());
  Amg::Vector3D dummyMom(trackP*cos(track->phi())*sin(track->theta()),
			 trackP*sin(track->phi())*sin(track->theta()),
			 trackP*cos(track->theta()));
  // Use the eta/phi direction at the PV to stand in for the extrapolated position in the calo
  Trk::CurvilinearParameters dummyPars(dummyMom,dummyMom,track->qOverP()>0);

  /*make the map*/
  std::map<eflowCalo::LAYER, const Trk::TrackParameters*> parametersMap;

  /*fill the map*/
  for(eflowCalo::LAYER layer : getLayerList(track->eta())) {
    /*Correct the map*/
    if(m_doMapEstimation) {
      parametersMap[layer] = CorrectLayerPosition(layer, track);
    }
    else{
      parametersMap[layer] = dummyPars.clone();
    }
  }

  if (!m_doMapEstimation) return std::make_unique<eflowTrackCaloPoints>(parametersMap);
  else{
    std::unique_ptr<eflowTrackCaloPoints> eflowTrackCaloPoints_uniquePtr = std::make_unique<eflowTrackCaloPoints>(parametersMap);
    SimpleExtrap::decorateWithLayerEtaPhi(*track, *(eflowTrackCaloPoints_uniquePtr.get()));
    return eflowTrackCaloPoints_uniquePtr;
  }
    
}

StatusCode eflowTrackCaloDummyExtensionTool::finalize() {
  return StatusCode::SUCCESS;
}


void eflowTrackCaloDummyExtensionTool::FillEstimationMap(const char * filename, const char * graphname, eflowCalo::LAYER layer) {
  TFile * file = new TFile(filename);
  m_mapEstimation[layer] = (TGraphErrors*)file->Get(graphname);
  file->Close();
}

Trk::CurvilinearParameters* eflowTrackCaloDummyExtensionTool::CorrectLayerPosition(eflowCalo::LAYER layer, const xAOD::TrackParticle* track) const {

  /* Getting the correction parameter */
  std::map<eflowCalo::LAYER, TGraphErrors*>::const_iterator theIterator = m_mapEstimation.find(layer);
  if (theIterator != m_mapEstimation.end()){
    TGraphErrors * graph = (*theIterator).second;
    ATH_MSG_DEBUG("Correction graph: " << graph);
    ATH_MSG_DEBUG("Correction layer: " << layer);
    double p0 = graph->Eval(fabs(track->eta())); // An linerly interpolated value is retrieved for the estimation of p0 between two eta graph points
    float absDeltaPhi = p0*1000./fabs(track->pt()); //Barbara provided the hyperbolic fits dividing the pt of tracks by 1000.
    ATH_MSG_DEBUG("Phi: " << track->phi());
    ATH_MSG_DEBUG("Eta: " << fabs(track->eta()));
    ATH_MSG_DEBUG("pt: " << fabs(track->pt()/1000.));
    ATH_MSG_DEBUG("|DeltaPhi|: " << absDeltaPhi);
    ATH_MSG_DEBUG("Charge: " << track->charge());
    float corrected_phi = 0.;
    corrected_phi = track->phi() + track->charge()*absDeltaPhi;
    ATH_MSG_DEBUG("CorrectedPhi: " << corrected_phi);
    
    float trackP = fabs(1./track->qOverP());
    Amg::Vector3D curvilinearCorrMom(trackP*cos(corrected_phi)*sin(track->theta()),
				     trackP*sin(corrected_phi)*sin(track->theta()),
				     trackP*cos(track->theta()));
    // Use the eta/phi direction at the PV to stand in for the extrapolated position in the calo
    Trk::CurvilinearParameters * ret = new Trk::CurvilinearParameters(curvilinearCorrMom,curvilinearCorrMom,track->qOverP()>0);

    return ret;
  }
  else{
    ATH_MSG_WARNING("Unable to find entry in  m_mapEstimation for calorimeter layer: " << layer);
    return nullptr;
  }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "AthLinks/ElementLinkVector.h"
#include "./TrackHypoAlg.h"

TrackHypoAlg::TrackHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_hypos(this),
    m_assocTool(this),
    m_inputTracks("SiSPSeededTracks"), 
    m_inputRoIs("OutputRoIs"),
    m_output("Output"), 
    m_outputAux("OutputAux.") 
{
  declareProperty("Hypotheses", m_hypos, "");
  declareProperty("TrackRoIAssociationTool", m_assocTool, "");
  declareProperty("TrackCollection", m_inputTracks, "Input tracks colleciton");
  declareProperty("RoiCollection",  m_inputRoIs, "Input RoIs colleciton");
  declareProperty("OutputDecisions", m_output, "Output collection");
  declareProperty("OutputDecisionsAux", m_outputAux, "");
}

StatusCode TrackHypoAlg::initialize() {
  CHECK(m_hypos.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TrackHypoAlg::execute() {
  if ( not m_inputTracks.isValid() ) {
    ATH_MSG_ERROR("Missing Input tracks");
    return StatusCode::FAILURE;    
  }

  if ( not m_inputRoIs.isValid() ) {
    ATH_MSG_ERROR("Missing Input RoIs");
    return StatusCode::FAILURE;    
  }



  m_output = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer >();  
  m_output->setStore(m_outputAux.ptr());


  if (m_inputTracks->size()==0)
    return StatusCode::SUCCESS;

  // associate Tracks to RoIs into TCs
   m_assocTool->associate(m_inputTracks , m_inputRoIs , m_output);


  // here apply the hypos
  int nTC=0;
    for ( auto proxy : *m_output.cptr() ) {
     
      //get RoI
      double roi_eta;
      double roi_phi;
      auto objWithLinkToRoI = ViewAlgs::find(proxy, ViewAlgs::HasObject("initialRoI")); 
      if ( objWithLinkToRoI ) {
	auto roi = objWithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");     
	roi_eta = (*roi)->eta();
	roi_phi = (*roi)->phi();
	ATH_MSG_DEBUG("TC "<<nTC<<" from collection "<< m_output.key()<<" has link to this RoI: eta="<<roi_eta<<" phi="<<roi_phi);
      }
      else{
	ATH_MSG_ERROR("TC "<<nTC<<" from collection "<< m_output.key()<<" has no link to any RoI");
	continue;
      }

      //get tracks
      ElementLinkVector<TrackCollection> linkVector;
      m_assocTool->getTrackLinks(proxy,  linkVector);
     
	// can apply hypo to singel track or to all tracks (multiplicity)

	//loop over tracks
	// for (auto link:linkVector ){

	//   float trackPerigeeTheta = (*link)->perigeeParameters()->parameters()[Trk::theta];
	//   float trackPerigeeEta = -log(tan(0.5*trackPerigeeTheta));
	//   double eta = trackPerigeeEta ;
	//   double phi = (*link)->perigeeParameters()->parameters()[Trk::phi]; 
	//   double zed = (*link)->perigeeParameters()->parameters()[Trk::z0] ;
	//   double pT  = fabs(1./((*link)->perigeeParameters()->parameters()[Trk::qOverP]* 1000.)) ;
	//   //doing something...
	// }
	
      ATH_MSG_DEBUG("Now running the hypos on TC "<<nTC);
      //hypo on track multiplicity
      std::vector<TriggerElementID> passedHypoIDs;
      for ( const auto& hypoTool : m_hypos ) {
	auto d = hypoTool->decision(linkVector);
	CHECK( d.sc() );	
	ATH_MSG_DEBUG("Hypo Tool " << hypoTool->name() << " decided " << d.content() 
		      << " on RoI: " << roi_eta <<" "<<roi_phi);
	
	if ( d.content() ) 
	  passedHypoIDs.push_back(TrigConf::HLTUtils::string2hash(hypoTool.name(), "TE")); // hypo name is the tag	
      }
      proxy->setDetail("Passed", passedHypoIDs);
      nTC++;
    }




  return StatusCode::SUCCESS;
}

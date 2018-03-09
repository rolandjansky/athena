/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthAssociationAlg.cxx
//   Implementation file for class MuonTruthAssociationAlg
///////////////////////////////////////////////////////////////////

#include "MuonTruthAssociationAlg.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

// Constructor with parameters:
MuonTruthAssociationAlg::MuonTruthAssociationAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{  
    
    // Get parameter values from jobOptions file
    declareProperty("MuonTruthParticleContainerName", m_muonTruthParticleContainerName = "MuonTruthParticles");
    declareProperty("MuonContainerName"             , m_muonName = "Muons");
    declareProperty("AssociateWithInDetTP"          , m_associateWithInDetTP = false); // hack: set true for conversion from release 17
    declareProperty("BarcodeOffset"                 , m_barcodeOffset = 1000000 );
}

// Initialize method:
StatusCode MuonTruthAssociationAlg::initialize()
{
  ATH_CHECK(m_idHelper.retrieve());
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode MuonTruthAssociationAlg::execute() 
{
    
    // skip if no input data found
    if ( !evtStore()->contains<xAOD::TruthParticleContainer>(m_muonTruthParticleContainerName) ) return StatusCode::SUCCESS;
    if ( !evtStore()->contains<xAOD::MuonContainer>(m_muonName) ) return StatusCode::SUCCESS;

    // retrieve Truth, exit if fails
    const xAOD::TruthParticleContainer* truthContainer{0};
    ATH_CHECK(evtStore()->retrieve(truthContainer,m_muonTruthParticleContainerName ));

    // get muon container
    const xAOD::MuonContainer* muons = 0;
    ATH_CHECK(evtStore()->retrieve(muons, m_muonName));
    
    /////////DEBUG/////////
    //list all reco muons
    // ATH_MSG_DEBUG("All reco muons:");
    // for( const auto & muon : *muons ){
    //   ATH_MSG_DEBUG(" auth " <<  muon->author() <<" pt " << muon->pt() << " eta " << muon->eta() );
    //   //xAOD::TrackParticle* tp  = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
    //   const xAOD::TrackParticle* tp  = muon->trackParticle(xAOD::Muon_v1::InnerDetectorTrackParticle);
    //   if( !tp ) continue;
    //   ATH_MSG_VERBOSE("  trackParticle pt " << tp->pt() << " eta " << tp->eta() );
    //   if (!tp->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink")) {
    //     ATH_MSG_VERBOSE("  truthParticleLink not available for this track particle");
    //     continue;
    //   }
    //   ElementLink<xAOD::TruthParticleContainer> truthLink = tp->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
    //   if (!truthLink.isValid()) { 
    //     ATH_MSG_VERBOSE("  Muon track particle has no truth association, skipping");
    //     continue;
    //   }
    //   ATH_MSG_DEBUG("  truthParticleLink status " << (*truthLink)->status() << " barcode " << (*truthLink)->barcode() << " pt "<< (*truthLink)->pt() << " eta "<< (*truthLink)->eta());
    // }
    //////////////////////////


    // add limk to reco muons and viceversa
    addMuon(*truthContainer, *muons);
  
    return StatusCode::SUCCESS;
}

void MuonTruthAssociationAlg::addMuon( const xAOD::TruthParticleContainer& truthParticles, const xAOD::MuonContainer& muons ) {
    
  // loop over muons 
  for( const auto& muon : muons ){
    // use primary track particle to get the truth link
    //xAOD::TrackParticle* tp = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
    //// hack: when converting from release 17, these truth links are absent (must fix); use the inDetParticle instead
    const xAOD::TrackParticle* tp(0);
    if (m_associateWithInDetTP || muon->author()==2) {
      tp = muon->trackParticle(xAOD::Muon_v1::InnerDetectorTrackParticle);
    } 
    else{
      tp = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
    }
    
    if( !tp ){
      setDummyTruthInfo(*muon);
      continue;
    }
    
    // special case for STACO muons where the combined TP is not truth matched
    if( muon->muonType() == xAOD::Muon::Combined && !tp->track() ){
      tp = const_cast<xAOD::TrackParticle*>(*muon->inDetTrackParticleLink());
    }
      
    // Associate reco with truth muon. Loop over reconstructed muons, get track particle for each one. 
    //Each track particle should carry a link to the corresponding truth particle. Then compare this truth particle link with the given truth muon particle
    try {
      ElementLink< xAOD::TruthParticleContainer > truthLink = tp->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
      if( truthLink.isValid() ){
	// loop over truth particles
	bool foundTruth=false;
	for( const auto& truthParticle : truthParticles ){
	  if( truthParticle->status() != 1 ) continue;   
	  ATH_MSG_DEBUG("Adding recoMuonLink for truth muon with barcode " << truthParticle->barcode() << " pt "<< truthParticle->pt());
	  ElementLink< xAOD::MuonContainer > muonLink;
	  
	  if( ((*truthLink)->barcode())%m_barcodeOffset == truthParticle->barcode() ) {
	    foundTruth=true;
	    ATH_MSG_VERBOSE(" Got truth link -> creating link with truth particle " << (*truthLink)->barcode() );
	    muonLink = ElementLink< xAOD::MuonContainer >(muon,muons);
	    // add the link from xAOD::Muon to TruthParticle in m_muonTruthParticleContainerName
	    ElementLink< xAOD::TruthParticleContainer > muonTruthLink = ElementLink< xAOD::TruthParticleContainer >(truthParticle, truthParticles);
	    muonTruthLink.toPersistent();
	    const_cast<xAOD::Muon&>(*muon).auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") = muonTruthLink;
	    const_cast<xAOD::Muon&>(*muon).auxdata<int>("truthType") = tp->auxdata<int>("truthType");
	    const_cast<xAOD::Muon&>(*muon).auxdata<int>("truthOrigin") = tp->auxdata<int>("truthOrigin");
	    if(muon->author()==1 || muon->author()==5 || muon->author()==6){ //only match hits for muons with MS tracks
	      if(!truthParticle->isAvailable<std::vector<unsigned long long> >("truthMdtHits")){
		ATH_MSG_DEBUG("muon with author "<<muon->author()<<" has no truth hits vector in the truth association alg");
	      }
	      else{
		std::vector<unsigned long long> mdtTruth=truthParticle->auxdata<std::vector<unsigned long long> >("truthMdtHits");
		std::vector<unsigned long long> cscTruth=truthParticle->auxdata<std::vector<unsigned long long> >("truthCscHits");
		std::vector<unsigned long long> rpcTruth=truthParticle->auxdata<std::vector<unsigned long long> >("truthRpcHits");
		std::vector<unsigned long long> tgcTruth=truthParticle->auxdata<std::vector<unsigned long long> >("truthTgcHits");
		const Trk::Track* ptrk=tp->track();
		const DataVector<const Trk::TrackStateOnSurface>* trkstates = ptrk->trackStateOnSurfaces();
		DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator tsit = trkstates->rbegin();
		DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator tsit_end = trkstates->rend();
		std::vector<unsigned int> nprecHitsPerChamberLayer;
		std::vector<unsigned int> nphiHitsPerChamberLayer;
		std::vector<unsigned int> ntrigEtaHitsPerChamberLayer;
		nprecHitsPerChamberLayer.resize(Muon::MuonStationIndex::ChIndexMax);
		nphiHitsPerChamberLayer.resize(Muon::MuonStationIndex::PhiIndexMax);
		ntrigEtaHitsPerChamberLayer.resize(Muon::MuonStationIndex::PhiIndexMax);
		//zero-suppression: don't want to store meaningless zeroes (no truth or reco hits found)
		for(unsigned int i=0;i<nprecHitsPerChamberLayer.size();i++) nprecHitsPerChamberLayer[i]=999;
		for(unsigned int i=0;i<nphiHitsPerChamberLayer.size();i++) nphiHitsPerChamberLayer[i]=999;
		for(unsigned int i=0;i<ntrigEtaHitsPerChamberLayer.size();i++) ntrigEtaHitsPerChamberLayer[i]=999;
		for( ; tsit!=tsit_end ; ++tsit ){
		  if(!*tsit) continue;
		  if(!(*tsit)->trackParameters() || !(*tsit)->measurementOnTrack()) continue;
		  const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
		  Identifier id;
		  const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
		  if(rot) id=rot->identify();
		  else{
		    const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
		    if(crot){
		      if( !crot->containedROTs().empty() && crot->containedROTs().front() ) id=crot->containedROTs().front()->identify();
		    }
		  }
		  if(!m_idHelper->isMuon(id)) continue;
		  bool measPhi = m_idHelper->measuresPhi(id);
		  bool isTgc = m_idHelper->isTgc(id);
		  Muon::MuonStationIndex::ChIndex chIndex = !isTgc ? m_idHelper->chamberIndex(id) : Muon::MuonStationIndex::ChUnknown;
		  bool found=false;
		  for(unsigned int i=0;i<mdtTruth.size();i++){
		    if(id==mdtTruth[i]){
		      if(nprecHitsPerChamberLayer[chIndex]==999) nprecHitsPerChamberLayer[chIndex]=1;
		      else ++nprecHitsPerChamberLayer[chIndex];
		      found=true;
		      break;
		    }
		  }
		  if(found) continue;
		  for(unsigned int i=0;i<cscTruth.size();i++){
		    if(id==cscTruth[i]){
		      if( measPhi ) {
			Muon::MuonStationIndex::PhiIndex index = m_idHelper->phiIndex(id);
			if(nphiHitsPerChamberLayer[index]==999) nphiHitsPerChamberLayer[index]=1;
			else ++nphiHitsPerChamberLayer[index];
		      }
		      else{
			if(nprecHitsPerChamberLayer[chIndex]==999) nprecHitsPerChamberLayer[chIndex]=1;
			else ++nprecHitsPerChamberLayer[chIndex];
		      }
		      found=true;
		      break;
		    }
		  }
		  if(found) continue;
		  for(unsigned int i=0;i<rpcTruth.size();i++){
		    if(id==rpcTruth[i]){
		      int index = m_idHelper->phiIndex(id);
		      if( measPhi ){
			if(nphiHitsPerChamberLayer[index]==999) nphiHitsPerChamberLayer[index]=1;
			else ++nphiHitsPerChamberLayer[index];
		      }
		      else{
			if(ntrigEtaHitsPerChamberLayer[index]==999) ntrigEtaHitsPerChamberLayer[index]=1;
			else ++ntrigEtaHitsPerChamberLayer[index];
		      }
		      found=true;
		      break;
		    }
		  }
		  if(found) continue;
		  for(unsigned int i=0;i<tgcTruth.size();i++){
		    if(id==tgcTruth[i]){
		      int index = m_idHelper->phiIndex(id);
		      if( measPhi ){
			if(nphiHitsPerChamberLayer[index]==999) nphiHitsPerChamberLayer[index]=1;
			else ++nphiHitsPerChamberLayer[index];
		      }
		      else{
			if(ntrigEtaHitsPerChamberLayer[index]==999) ntrigEtaHitsPerChamberLayer[index]=1;
			else ++ntrigEtaHitsPerChamberLayer[index];
		      }
		      found=true;
		      break;
		    }
		  }
		} //end loop over TSOS
		ATH_MSG_DEBUG("finished loop over TSOS");
		//now, have to check if there are non-zero truth hits in indices without reco hits
		for(unsigned int i=0;i<nprecHitsPerChamberLayer.size();i++){
		  if(nprecHitsPerChamberLayer[i]==999){
		    bool found=false;
		    for(unsigned int j=0;j<mdtTruth.size();j++){
		      Identifier id(mdtTruth[j]);
		      if(m_idHelper->chamberIndex(id)==(Muon::MuonStationIndex::ChIndex)i){ nprecHitsPerChamberLayer[i]=0; found=true; break;}
		    }
		    if(found) continue;
		    for(unsigned int j=0;j<cscTruth.size();j++){
		      Identifier id(cscTruth[j]);
		      if(!m_idHelper->measuresPhi(id)){
			if(m_idHelper->chamberIndex(id)==(Muon::MuonStationIndex::ChIndex)i){ nprecHitsPerChamberLayer[i]=0; break;}
		      }
		    }
		  }
		}
		for(unsigned int i=0;i<nphiHitsPerChamberLayer.size();i++){
		  if(nphiHitsPerChamberLayer[i]==999){
		    bool found=false;
		    for(unsigned int j=0;j<cscTruth.size();j++){
		      Identifier id(cscTruth[j]);
		      if(m_idHelper->measuresPhi(id)){
			if(m_idHelper->phiIndex(id)==(Muon::MuonStationIndex::PhiIndex)i){nphiHitsPerChamberLayer[i]=0; found=true; break;}
		      }
		    }
		    if(found) continue;
		    for(unsigned int j=0;j<rpcTruth.size();j++){
		      Identifier id(rpcTruth[j]);
		      if(m_idHelper->measuresPhi(id)){
			if(m_idHelper->phiIndex(id)==(Muon::MuonStationIndex::PhiIndex)i){nphiHitsPerChamberLayer[i]=0; found=true; break;}
		      }
		    }
		    if(found) continue;
		    for(unsigned int j=0;j<tgcTruth.size();j++){
		      Identifier id(tgcTruth[j]);
		      if(m_idHelper->measuresPhi(id)){
			if(m_idHelper->phiIndex(id)==(Muon::MuonStationIndex::PhiIndex)i){nphiHitsPerChamberLayer[i]=0; break;}
		      }
		    }
		  }
		}
		for(unsigned int i=0;i<ntrigEtaHitsPerChamberLayer.size();i++){
		  if(ntrigEtaHitsPerChamberLayer[i]==999){
		    bool found=false;
		    for(unsigned int j=0;j<rpcTruth.size();j++){
		      Identifier id(rpcTruth[j]);
		      if(!m_idHelper->measuresPhi(id)){
			if(m_idHelper->phiIndex(id)==(Muon::MuonStationIndex::PhiIndex)i){nphiHitsPerChamberLayer[i]=0; found=true; break;}
		      }
		    }
		    if(found) continue;
		    for(unsigned int j=0;j<tgcTruth.size();j++){
		      Identifier id(tgcTruth[j]);
		      if(!m_idHelper->measuresPhi(id)){
			if(m_idHelper->phiIndex(id)==(Muon::MuonStationIndex::PhiIndex)i){nphiHitsPerChamberLayer[i]=0; break;}
		      }
		    }
		  }
		}
		const_cast<xAOD::Muon&>(*muon).auxdata<std::vector<unsigned int> >("nprecMatchedHitsPerChamberLayer")=nprecHitsPerChamberLayer;
		const_cast<xAOD::Muon&>(*muon).auxdata<std::vector<unsigned int> >("nphiMatchedHitsPerChamberLayer")=nphiHitsPerChamberLayer;
		const_cast<xAOD::Muon&>(*muon).auxdata<std::vector<unsigned int> >("ntrigEtaMatchedHitsPerChamberLayer")=ntrigEtaHitsPerChamberLayer;
	      }
	    }
	    muonLink.toPersistent();
	    const_cast<xAOD::TruthParticle&>(*truthParticle).auxdata<ElementLink< xAOD::MuonContainer > >("recoMuonLink") = muonLink;
	    break;
	  }
	}
	if(!foundTruth){
	  ATH_MSG_DEBUG("failed to find a status=1 truth particle to match the truth link");
	  setDummyTruthInfo(*muon);
	}
      }
      else{ //no truth link, add a dummy
	setDummyTruthInfo(*muon);
	ATH_MSG_VERBOSE(" Reco muon has no truth association");
      }
    }catch ( SG::ExcBadAuxVar& ) {
      ATH_MSG_WARNING("Track particle is missing truthParticleLink variable!");
      //there should always be a truthParticleLink, but just in case
      setDummyTruthInfo(*muon);
    }
  }

  //one more thing: need to have muonlink set for all truth particles to avoid ELReset errors
  for( const auto& truthParticle : truthParticles ){
    if(!truthParticle->isAvailable<ElementLink< xAOD::MuonContainer > >("recoMuonLink")){
      const_cast<xAOD::TruthParticle&>(*truthParticle).auxdata<ElementLink< xAOD::MuonContainer > >("recoMuonLink")=ElementLink< xAOD::MuonContainer > ();
    }
  }
}

// Finalize method:
StatusCode MuonTruthAssociationAlg::finalize() 
{
    return StatusCode::SUCCESS;
}

void MuonTruthAssociationAlg::setDummyTruthInfo(const xAOD::Muon& muon){
  const_cast<xAOD::Muon&>(muon).auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" )=ElementLink<xAOD::TruthParticleContainer>();
  const_cast<xAOD::Muon&>(muon).auxdata<int>("truthType") = -99999;
  const_cast<xAOD::Muon&>(muon).auxdata<int>("truthOrigin") = -99999;
  //add these empty vectors
  std::vector<unsigned int> nprecHitsPerChamberLayer;
  std::vector<unsigned int> nphiHitsPerChamberLayer;
  std::vector<unsigned int> ntrigEtaHitsPerChamberLayer;
  const_cast<xAOD::Muon&>(muon).auxdata<std::vector<unsigned int> >("nprecMatchedHitsPerChamberLayer")=nprecHitsPerChamberLayer;
  const_cast<xAOD::Muon&>(muon).auxdata<std::vector<unsigned int> >("nphiMatchedHitsPerChamberLayer")=nphiHitsPerChamberLayer;
  const_cast<xAOD::Muon&>(muon).auxdata<std::vector<unsigned int> >("ntrigEtaMatchedHitsPerChamberLayer")=ntrigEtaHitsPerChamberLayer;
}

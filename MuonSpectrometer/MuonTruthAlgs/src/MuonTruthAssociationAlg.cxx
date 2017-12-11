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
#include "StoreGate/WriteDecorHandle.h"

// Constructor with parameters:
MuonTruthAssociationAlg::MuonTruthAssociationAlg(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{}

// Initialize method:
StatusCode MuonTruthAssociationAlg::initialize()
{
  ATH_CHECK(m_idHelper.retrieve());
  m_muonTruthParticleContainerName=m_muonTruthParticleContainerName.key()+".recoMuonLink";
  ATH_CHECK(m_muonTruthParticleContainerName.initialize());
  m_muonTruthParticleLink=m_muonName+".truthParticleLink";
  m_muonTruthParticleType=m_muonName+".truthType";
  m_muonTruthParticleOrigin=m_muonName+".truthOrigin";
  m_muonTruthParticleNPrecMatched=m_muonName+".nprecMatchedHitsPerChamberLayer";
  m_muonTruthParticleNPhiMatched=m_muonName+".nphiMatchedHitsPerChamberLayer";
  m_muonTruthParticleNTrigEtaMatched=m_muonName+".ntrigEtaMatchedHitsPerChamberLayer";
  ATH_CHECK(m_muonTruthParticleLink.initialize());
  ATH_CHECK(m_muonTruthParticleOrigin.initialize());
  ATH_CHECK(m_muonTruthParticleType.initialize());
  ATH_CHECK(m_muonTruthParticleNPrecMatched.initialize());
  ATH_CHECK(m_muonTruthParticleNPhiMatched.initialize());
  ATH_CHECK(m_muonTruthParticleNTrigEtaMatched.initialize());
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode MuonTruthAssociationAlg::execute() 
{

  SG::WriteDecorHandle<xAOD::TruthParticleContainer,ElementLink< xAOD::MuonContainer > > muonTruthParticleRecoLink(m_muonTruthParticleContainerName);
  if(!muonTruthParticleRecoLink.isPresent()) return StatusCode::SUCCESS;
  if(!muonTruthParticleRecoLink.isValid()){
    ATH_MSG_WARNING("truth particle container not valid");
    return StatusCode::FAILURE;
  }
  SG::WriteDecorHandle<xAOD::MuonContainer,ElementLink<xAOD::TruthParticleContainer> > muonTruthParticleLink(m_muonTruthParticleLink);
  if(!muonTruthParticleLink.isPresent()) return StatusCode::SUCCESS;
  if(!muonTruthParticleLink.isValid()){
    ATH_MSG_WARNING("muon particle container not valid");
    return StatusCode::FAILURE;
  }
  SG::WriteDecorHandle<xAOD::MuonContainer,int > muonTruthParticleType(m_muonTruthParticleType);
  SG::WriteDecorHandle<xAOD::MuonContainer,int > muonTruthParticleOrigin(m_muonTruthParticleOrigin);
  SG::WriteDecorHandle<xAOD::MuonContainer,std::vector<unsigned int> > muonTruthParticleNPrecMatched(m_muonTruthParticleNPrecMatched);
  SG::WriteDecorHandle<xAOD::MuonContainer,std::vector<unsigned int> > muonTruthParticleNPhiMatched(m_muonTruthParticleNPhiMatched);
  SG::WriteDecorHandle<xAOD::MuonContainer,std::vector<unsigned int> > muonTruthParticleNTrigEtaMatched(m_muonTruthParticleNTrigEtaMatched);
    
  // add link to reco muons and viceversa
  
  // loop over muons 
  int muonInd=0;
  for( const auto& muon : *muonTruthParticleLink ){
    // use primary track particle to get the truth link (except for the case of STACO, where we must use the ID track particle, as the combined is not truth-matched)
    const xAOD::TrackParticle* tp(0);
    if (m_associateWithInDetTP || muon->author()==2) {
      tp = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    } 
    else{
      //tp = const_cast<xAOD::TrackParticle*>(muon->primaryTrackParticle());
      tp=muon->primaryTrackParticle();
    }
    
    if( !tp ){
      muonTruthParticleLink(*muon)=ElementLink<xAOD::TruthParticleContainer>();
      muonTruthParticleOrigin(muonInd)=-99999;
      muonTruthParticleType(muonInd)=-99999;
      //add these empty vectors
      std::vector<unsigned int> nprecHitsPerChamberLayer;
      std::vector<unsigned int> nphiHitsPerChamberLayer;
      std::vector<unsigned int> ntrigEtaHitsPerChamberLayer;
      muonTruthParticleNPrecMatched(muonInd)=nprecHitsPerChamberLayer;
      muonTruthParticleNPhiMatched(muonInd)=nphiHitsPerChamberLayer;
      muonTruthParticleNTrigEtaMatched(muonInd)=ntrigEtaHitsPerChamberLayer;
      continue;
    }
    
    // Associate reco with truth muon. Loop over reconstructed muons, get track particle for each one. 
    //Each track particle should carry a link to the corresponding truth particle. Then compare this truth particle link with the given truth muon particle
    try {
      ElementLink< xAOD::TruthParticleContainer > truthLink = tp->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
      if( truthLink.isValid() ){
	// loop over truth particles
	bool foundTruth=false;
	for( const auto& truthParticle : *muonTruthParticleRecoLink ){
	  if( truthParticle->status() != 1 ) continue;
	  ATH_MSG_DEBUG("Adding recoMuonLink for truth muon with barcode " << truthParticle->barcode() << " pt "<< truthParticle->pt());
	  ElementLink< xAOD::MuonContainer > muonLink;
	  
	  if( ((*truthLink)->barcode())%m_barcodeOffset == truthParticle->barcode() ) {
	    ATH_MSG_VERBOSE(" Got truth link -> creating link with truth particle " << (*truthLink)->barcode() );
	    foundTruth=true;
	    muonLink = ElementLink< xAOD::MuonContainer >(muon,*muonTruthParticleLink);
	    // add the link from xAOD::Muon to TruthParticle in m_muonTruthParticleContainerName
	    ElementLink< xAOD::TruthParticleContainer > muonTruthLink = ElementLink< xAOD::TruthParticleContainer >(truthParticle, *muonTruthParticleRecoLink);
	    muonTruthLink.toPersistent();
	    muonTruthParticleLink(*muon)=muonTruthLink;
	    muonTruthParticleOrigin(muonInd)=tp->auxdata<int>("truthType");
	    muonTruthParticleType(muonInd)=tp->auxdata<int>("truthOrigin");
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
		muonTruthParticleNPrecMatched(muonInd)=nprecHitsPerChamberLayer;
		muonTruthParticleNPhiMatched(muonInd)=nphiHitsPerChamberLayer;
		muonTruthParticleNTrigEtaMatched(muonInd)=ntrigEtaHitsPerChamberLayer;
	      }
	    }
	    muonLink.toPersistent();
	    muonTruthParticleRecoLink(*truthParticle)=muonLink;
	    break;
	  }
	}
	if(!foundTruth){
	  ATH_MSG_DEBUG("failed to find a status=1 truth particle to match the truth link");
	  muonTruthParticleLink(*muon)=ElementLink<xAOD::TruthParticleContainer>();
	  muonTruthParticleOrigin(muonInd)=-99999;
	  muonTruthParticleType(muonInd)=-99999;
	  std::vector<unsigned int> nprecHitsPerChamberLayer;
	  std::vector<unsigned int> nphiHitsPerChamberLayer;
	  std::vector<unsigned int> ntrigEtaHitsPerChamberLayer;
	  muonTruthParticleNPrecMatched(muonInd)=nprecHitsPerChamberLayer;
	  muonTruthParticleNPhiMatched(muonInd)=nphiHitsPerChamberLayer;
	  muonTruthParticleNTrigEtaMatched(muonInd)=ntrigEtaHitsPerChamberLayer;
	}
      }
      else{ //no truth link, add a dummy
	ATH_MSG_VERBOSE(" Reco muon has no truth association");
	muonTruthParticleLink(*muon)=ElementLink<xAOD::TruthParticleContainer>();
	muonTruthParticleOrigin(muonInd)=-99999;
	muonTruthParticleType(muonInd)=-99999;
	//add these empty vectors
	std::vector<unsigned int> nprecHitsPerChamberLayer;
	std::vector<unsigned int> nphiHitsPerChamberLayer;
	std::vector<unsigned int> ntrigEtaHitsPerChamberLayer;
	muonTruthParticleNPrecMatched(muonInd)=nprecHitsPerChamberLayer;
	muonTruthParticleNPhiMatched(muonInd)=nphiHitsPerChamberLayer;
	muonTruthParticleNTrigEtaMatched(muonInd)=ntrigEtaHitsPerChamberLayer;
      }
    }catch ( SG::ExcBadAuxVar& ) {
      ATH_MSG_WARNING("Track particle is missing truthParticleLink variable!");
      //there should always be a truthParticleLink, but just in case
      muonTruthParticleLink(*muon)=ElementLink<xAOD::TruthParticleContainer>();
      muonTruthParticleOrigin(muonInd)=-99999;
      muonTruthParticleType(muonInd)=-99999;
      std::vector<unsigned int> nprecHitsPerChamberLayer;
      std::vector<unsigned int> nphiHitsPerChamberLayer;
      std::vector<unsigned int> ntrigEtaHitsPerChamberLayer;
      muonTruthParticleNPrecMatched(muonInd)=nprecHitsPerChamberLayer;
      muonTruthParticleNPhiMatched(muonInd)=nphiHitsPerChamberLayer;
      muonTruthParticleNTrigEtaMatched(muonInd)=ntrigEtaHitsPerChamberLayer;
    }
    muonInd++;
  }

  //one more thing: need to have muonlink set for all truth particles to avoid ELReset errors
  for( const auto& truthParticle : *muonTruthParticleRecoLink ){
    if(!truthParticle->isAvailable<ElementLink< xAOD::MuonContainer > >("recoMuonLink")){
      muonTruthParticleRecoLink(*truthParticle)=ElementLink< xAOD::MuonContainer > ();
    }
  }
    return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode MuonTruthAssociationAlg::finalize() 
{
    return StatusCode::SUCCESS;
}


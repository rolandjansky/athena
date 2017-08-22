/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/SharedHitMapper.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkTrack/Track.h"
#include <string>
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"
#include <vector>
#include <utility>
#include <iostream>

typedef std::pair<const xAOD::TrackParticle*, const Trk::RIO_OnTrack*> pairTrkRio;
typedef std::vector<pairTrkRio> vecpairTrkRio;


SharedHitTrackAssoc::SharedHitTrackAssoc() {
}
SharedHitTrackAssoc::~SharedHitTrackAssoc() {
}
void SharedHitTrackAssoc::add(const xAOD::TrackParticle* const trk, int shPattern) {
  m_assocs.insert(std::make_pair(trk,shPattern));
}
void SharedHitTrackAssoc::add(const xAOD::TrackParticle* const trk, int shB, int shP, int shS) {
  int shPattern = 100000*shB + 1000*shS + 100*shP;
  m_assocs.insert(std::make_pair(trk,shPattern));
}
int SharedHitTrackAssoc::numberSharedBLayer(const xAOD::TrackParticle* const trk) const {
  int tempSharedPattern(0);
  AssocIter aE = m_assocs.end();
  AssocIter aI = m_assocs.find(trk);
  if(aI!=aE) tempSharedPattern = aI->second;
  return tempSharedPattern/100000;
}
int SharedHitTrackAssoc::numberSharedPix(const xAOD::TrackParticle* const trk) const {
  int tempSharedPattern(0);
  AssocIter aE = m_assocs.end();
  AssocIter aI = m_assocs.find(trk);
  if(aI!=aE) tempSharedPattern = aI->second;
  return ((tempSharedPattern%100000)%1000)/100;
}
int SharedHitTrackAssoc::numberSharedSct(const xAOD::TrackParticle* const trk) const {
  int tempSharedPattern(0);
  AssocIter aE = m_assocs.end();
  AssocIter aI = m_assocs.find(trk);
  if(aI!=aE) tempSharedPattern = aI->second;
  return (tempSharedPattern%100000)/1000;
}

void SharedHitTrackAssoc::dump() const {
  std::cout<<"SharedHitTrackAssoc has "<<this->size()<<" elements:"<<std::endl;
  AssocIter aI = m_assocs.begin();
  AssocIter aE = m_assocs.end();
  for(; aI!=aE; ++aI) {
    std::cout << "--> track: "
	      << " Eta= " << aI->first->eta()
	      << " Phi= " << aI->first->phi()
	      << " pT= " << aI->first->pt()
	      << " Shared pattern= " << aI->second 
      // what follows is not nice...
	      << " shB= " << this->numberSharedBLayer(aI->first)
	      << " shP= " << this->numberSharedPix(aI->first)
	      << " shS= " << this->numberSharedSct(aI->first)
	      << std::endl;
  }
}


SharedHitMapper::SharedHitMapper(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator) {
  declareProperty("inputTrackCollection", m_inputTrackCollection = "TrackParticleCandidate");
  declareProperty("sharedHitMapLocation", m_sharedHitMapLocation = "SharedHitMap");
  declareProperty("deltaRCut", m_deltaRCut = 1.0);
  declareProperty("QualityOrdering", m_qualOrder = false);
  declareProperty("useTrackSummaryShared", m_useTrackSummaryShared = true);
}

SharedHitMapper::~SharedHitMapper() {
}

StatusCode SharedHitMapper::initialize() {
  StatusCode sc = service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("StoreGate service not found !");
    return StatusCode::FAILURE;
  }
  if(!m_useTrackSummaryShared) {
    StoreGateSvc* detStore = 0;
    sc = service( "DetectorStore", detStore );
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not get DetectorStore");
      return sc;
    }
    sc = detStore->retrieve(m_pixelId, "PixelID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not get PixelID helper !");
    }
    sc = detStore->retrieve(m_sctId, "SCT_ID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not get SCT_ID helper !");
    }
  }
  std::cout<<name()<<" with input tracks "<<m_inputTrackCollection<<std::endl;
  std::cout<<name()<<" initialized"<<std::endl;
  return StatusCode::SUCCESS;
}

StatusCode SharedHitMapper::execute() {

  MsgStream mlog(msgSvc(), name()); 

  /** retrieve input tracks: */
  const xAOD::TrackParticleContainer* inputTracks(0);
  StatusCode sc = m_StoreGate->retrieve(inputTracks, m_inputTrackCollection);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("TrackParticles " << m_inputTrackCollection << " not found in StoreGate.");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_VERBOSE("TrackParticleContainer " << m_inputTrackCollection << " found.");

  SharedHitTrackAssoc* assoc = new SharedHitTrackAssoc();
  ATH_MSG_INFO("Number of initial TrackParticles: " << inputTracks->size());

  if(m_useTrackSummaryShared) {
    // --- Use shared hit information from TrackSummary:
    // loop on first track:
    xAOD::TrackParticleContainer::const_iterator trk1I(inputTracks->begin());
    xAOD::TrackParticleContainer::const_iterator trkE(inputTracks->end());
    for (; trk1I!=trkE; ++trk1I) {
      int nbs, nps, nss;
      (*trk1I)->summaryValue(nbs, xAOD::numberOfInnermostPixelLayerSharedHits);
      (*trk1I)->summaryValue(nps, xAOD::numberOfPixelSharedHits);
      (*trk1I)->summaryValue(nss, xAOD::numberOfSCTSharedHits);
      if (nbs < 0) nbs = 0;
      if (nps < 0) nps = 0;
      if (nss < 0) nss = 0;
      if(nbs+nps+nss>0) assoc->add( (*trk1I), nbs, nps, nss);
    }
  } else {
    // --- Recompute shared hit information from TrkTracks:

    vecpairTrkRio myvecTrkRio(0);

    // loop on first track:
    xAOD::TrackParticleContainer::const_iterator trk1I(inputTracks->begin());
    xAOD::TrackParticleContainer::const_iterator trkE(inputTracks->end());
    for (; trk1I!=trkE; ++trk1I) {
      const xAOD::TrackParticle* trk1 = (*trk1I);//->track();
      if(trk1==0) {
	ATH_MSG_ERROR("Can't get the original track ! Please run on ESD");
	return StatusCode::SUCCESS;
      }
      //
      // For the quality computation
      int q1 = 0 ,q2 = 0;
      int np1 = 0, np2 = 0, ns1 = 0, ns2 = 0;
      int nhp1 = 0, nhp2 = 0, nhs1 = 0, nhs2 = 0, nh1 = 0, nh2 = 0;
      int nprec1 = 0, nprec2 = 0;
      double chi21 = 0., chi22 = 0.;
      if (m_qualOrder) { 
	chi21 = (*trk1I)->chiSquared();
	(*trk1I)->summaryValue(np1, xAOD::numberOfPixelHits); if (np1<0)np1=0;
	(*trk1I)->summaryValue(ns1, xAOD::numberOfSCTHits); if (ns1<0)ns1=0;
	nprec1 = np1 + ns1;
	(*trk1I)->summaryValue(nhp1, xAOD::numberOfPixelHoles); if (nhp1<0)nhp1=0;
	(*trk1I)->summaryValue(nhs1, xAOD::numberOfSCTHoles); if (nhs1<0)nhs1=0;
	nh1 = nhp1 + nhs1;
	q1     = 10000*(2*nprec1-nh1) - int(20.*chi21/float(nprec1));

      }
      //
      xAOD::IParticle::FourMom_t t1p4 = (*trk1I)->p4();
      const DataVector<const Trk::MeasurementBase>* mb = (*trk1->track())->measurementsOnTrack();
      DataVector<const Trk::MeasurementBase>::const_iterator nextMb;
      DataVector<const Trk::MeasurementBase>::const_iterator lastMb(mb->end());
      ATH_MSG_VERBOSE("First track (pT="<<t1p4.Pt()<<", eta="<<t1p4.Eta ()<<") has " << mb->size() << " RIOs on track");
      // init arrays for pixels:
      bool pl[m_nbpl]; // layers
      for(int i=0;i<m_nbpl;i++) m_npl[i] = 0;
      bool pd[m_nbpd]; // disks
      for(int i=0;i<m_nbpd;i++) m_npd[i] = 0;
      // init arrays for sct:
      bool sl[m_nbsl];
      for(int i=0;i<m_nbsl;i++) m_nsl[i] = 0;
      bool sd[m_nbsd];
      for(int i=0;i<m_nbsd;i++) m_nsd[i] = 0;
      // loop on second track:
      xAOD::TrackParticleContainer::const_iterator trk2I(inputTracks->begin());
      for (; trk2I!=trkE; ++trk2I) {
	if(trk2I==trk1I) continue;
	const xAOD::TrackParticle* trk2 = (*trk2I);
	if(trk2==0) {
	  ATH_MSG_ERROR("Can't get the original track ! Please run on ESD");
	  return StatusCode::SUCCESS;
	}
	xAOD::IParticle::FourMom_t t2p4 = (*trk2I)->p4();
	double deltaR = t1p4.DeltaR(t2p4);
	if(deltaR<m_deltaRCut) {
	  // Assign the shared hit to the worst quality track 
	  // If a hit is shared by more than 2 tracks, this will not be perfect...
	  // From IG : Quality = 10000*(2*Nhit-Nhole) - int(20.*Xi2/float(Nhit))
	  if (m_qualOrder) { 
	    chi22 = (*trk2I)->chiSquared();
	    (*trk2I)->summaryValue(np2, xAOD::numberOfPixelHits); if (np2<0)np2=0;
	    (*trk2I)->summaryValue(ns2, xAOD::numberOfSCTHits); if (ns2<0)ns2=0;
	    nprec1 = np1 + ns1;
	    (*trk2I)->summaryValue(nhp2, xAOD::numberOfPixelHoles); if (nhp2<0)nhp2=0;
	    (*trk2I)->summaryValue(nhs2, xAOD::numberOfSCTHoles); if (nhs2<0)nhs2=0;
	    nh2 = nhp2 + nhs2;
	    q2     = 10000*(2*nprec2-nh2) - int(20.*chi22/float(nprec2));
	    if (q1 > q2) continue;
	  }
	  // loop on 1st track clusters:
	  nextMb = mb->begin(); 
	  for(; nextMb != lastMb; ++nextMb) {
	    const Trk::RIO_OnTrack* nextRio = dynamic_cast<const Trk::RIO_OnTrack*>(*nextMb);
	    bool pixl = m_pixelId->is_pixel(nextRio->identify());
	    bool ssct = m_sctId->is_sct(nextRio->identify());
	    if( (!pixl) && (!ssct) ) continue;
	    // pixels:
	    for(int i=0;i<m_nbpl;i++) pl[i] = false;
	    for(int i=0;i<m_nbpd;i++) pd[i] = false;
	    if(pixl) {
	      pl[0] = m_pixelId->is_blayer(nextRio->identify());
	      if(m_pixelId->is_barrel(nextRio->identify())) {
		int player = m_pixelId->layer_disk(nextRio->identify());
		for(int i=1;i<m_nbpl;i++) pl[i] = ( player == i );
	      } else {
		int pdisk = m_pixelId->layer_disk(nextRio->identify());
		for(int i=0;i<m_nbpd;i++) pd[i] = ( pdisk == i );
	      }
	    }
	    // sct:
	    for(int i=0;i<m_nbsl;i++) sl[i] = false;
	    for(int i=0;i<m_nbsd;i++) sd[i] = false;
	    if(ssct) {
	      if(m_sctId->is_barrel(nextRio->identify())) {
		int slayer = m_sctId->layer_disk(nextRio->identify());
		for(int i=0;i<m_nbsl;i++) sl[i] = ( slayer == i );
	      } else {
		int sdisk = m_sctId->layer_disk(nextRio->identify());
		for(int i=0;i<m_nbsd;i++) sd[i] = ( sdisk == i );
	      }
	    }
	    // loop on 2nd track clusters:
	    const DataVector<const Trk::MeasurementBase>* mb2 = (*trk2->track())->measurementsOnTrack();
	    DataVector<const Trk::MeasurementBase>::const_iterator nextMb2(mb2->begin());
	    DataVector<const Trk::MeasurementBase>::const_iterator lastMb2(mb2->end());
	    if (nextMb == mb->begin()) ATH_MSG_VERBOSE("Second track (pT="<<t2p4.Pt()<<") has " << mb2->size() << " RIOs on track");
	    int nriosi = 0;
	    for(; nextMb2 != lastMb2; ++nextMb2) {
	      const Trk::RIO_OnTrack* nextRio2 = dynamic_cast<const Trk::RIO_OnTrack*>(*nextMb2);
	      bool pixl2 = m_pixelId->is_pixel(nextRio2->identify());
	      bool ssct2 = m_sctId->is_sct(nextRio2->identify());
	      if( (!pixl2) && (!ssct2) ) continue;
	      nriosi++;
	      if( nextRio2->identify() == nextRio->identify() ) {
		ATH_MSG_DEBUG("Shared hit ! Track1 Pt = " << t1p4.Pt() <<" Track2 Pt = " << t2p4.Pt());
		if (m_qualOrder) {
		  ATH_MSG_VERBOSE("Track1 chi2 = " << chi21 << " nprec = " << nprec1 << " (np,ns) = (" << np1 << " , " << ns1 << ") " << " nhole = " << nh1    << " (nhp,nhs) = (" << nhp1 << " , " << nhs1 << ") " << " quality = " << q1);
		  ATH_MSG_VERBOSE("Track2 chi2 = " << chi22 << " nprec = " << nprec2 << " (np,ns) = (" << np2 << " , " << ns2 << ") " << " nhole = " << nh2    << " (nhp,nhs) = (" << nhp2 << " , " << nhs2 << ") " << " quality = " << q2);
		}
		ATH_MSG_DEBUG(pl[0] << " "<< pl[1] << " " << pl[2]);
		ATH_MSG_DEBUG(pd[0] << " "<< pd[1] << " " << pd[2]);
		ATH_MSG_DEBUG(sl[0] << " "<< sl[1] << " " << sl[2] << " " << sl[3]);
		ATH_MSG_DEBUG(sd[0] << " "<< sd[1] << " " << sd[2] << " " << sd[3] << " " << sd[4] << " "<< sd[5] << " " << sd[6] << " " << sd[7] << " " << sd[8]);
		pairTrkRio p = std::make_pair(*trk1I,nextRio);
		for(int i=0;i<m_nbpl;i++) {
		  if (pl[i] && std::find(myvecTrkRio.begin(), myvecTrkRio.end(), p) == myvecTrkRio.end()) {m_npl[i]++;myvecTrkRio.push_back(p);}
		}
		for(int i=0;i<m_nbpd;i++) {
		  if (pd[i] && std::find(myvecTrkRio.begin(), myvecTrkRio.end(), p) == myvecTrkRio.end()) {m_npd[i]++;myvecTrkRio.push_back(p);}
		}
		for(int i=0;i<m_nbsl;i++) {
		  if (sl[i] && std::find(myvecTrkRio.begin(), myvecTrkRio.end(), p) == myvecTrkRio.end()) {m_nsl[i]++;myvecTrkRio.push_back(p);}
		}
		for(int i=0;i<m_nbsd;i++) {
		  if (sd[i] && std::find(myvecTrkRio.begin(), myvecTrkRio.end(), p) == myvecTrkRio.end()) {m_nsd[i]++;myvecTrkRio.push_back(p);}
		}
	      }
	    } // end loop on rio2
	  } // end loop on rio1
	}
      } // end loop on trk2
	// now store the result:
      if(this->numberSharedBLayer()+this->numberSharedPix()+this->numberSharedSct()>0) {
	assoc->add( (*trk1I), this->numberSharedBLayer(), 
		    this->numberSharedPix(),
		    this->numberSharedSct() );
      }
    } // end loop on trk1
  }

  ATH_MSG_INFO("Size of SharedHitMap: " << assoc->size());

  if (mlog.level() <= MSG::DEBUG) assoc->dump();

  if(m_StoreGate->record(assoc,m_sharedHitMapLocation,false).isFailure()) {
    ATH_MSG_ERROR("recording of SharedHitAssoc " << m_sharedHitMapLocation << " failed.");
  } else {
    ATH_MSG_VERBOSE("Recording of SharedHitAssoc " << m_sharedHitMapLocation << " succeeded, coll is now const." );
  }


  return StatusCode::SUCCESS;
}

StatusCode SharedHitMapper::finalize() {
  return StatusCode::SUCCESS;
}

int SharedHitMapper::numberSharedBLayer() const {
  return m_npl[0];
}

int SharedHitMapper::numberSharedPix() const {
  int nbs = 0;
  for(int i=0;i<m_nbpl;i++) nbs += m_npl[i];
  for(int i=0;i<m_nbpd;i++) nbs += m_npd[i];
  return nbs;
}

int SharedHitMapper::numberSharedSct() const {
  int nbs = 0;
  for(int i=0;i<m_nbsl;i++) nbs += m_nsl[i];
  for(int i=0;i<m_nbsd;i++) nbs += m_nsd[i];
  return nbs;
}



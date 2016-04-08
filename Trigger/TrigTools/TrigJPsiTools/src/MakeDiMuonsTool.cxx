/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***********************************************************************
// MakeDiMuonsTool.cxx
// Returns a vector of DiMuon objects from input tracks.
// Possible to switch on/off tag-and-probe method.

// AUTHORS: Weina Ji (weina.ji@cern.ch)
//          Peter Wijeratne (paw@hep.ucl.ac.uk)
//          Christian Gutschow (chris.g@cern.ch)
// ***********************************************************************

#include "TrigJPsiTools/MakeDiMuonsTool.h"
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TLorentzVector.h"

#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"

#include <stdexcept>

MakeDiMuonsTool::MakeDiMuonsTool()
  :m_trigEffJpsiTools("TrigEffJpsiTools")
{
  m_ptcut=0*CLHEP::GeV;
  m_tagptcut=0*CLHEP::GeV;
  m_etacut=999;
  m_tagetacut=999;
  m_TagAndProbe=false;
  m_useTrigger=false;
  //  m_trkmass = 105.66*CLHEP::MeV;
  m_trkmass=0.511*CLHEP::MeV;
  m_tracks.clear();
  m_pairs.clear();
  m_MuonCollection=0;
  m_TrigCollection=0;
  m_upperlimit = 99999;
  m_lowerlimit = -99999;
  m_taggedMuons.clear();
  m_selectaProbe.cleanup();
  m_selectaTag.cleanup();
}

MakeDiMuonsTool::~MakeDiMuonsTool() { }

bool MakeDiMuonsTool::initializeTools()
{
  if(m_trigEffJpsiTools.retrieve().isFailure()) return false; 
  else return true;
}

void MakeDiMuonsTool::initializeMembers()
{
  m_ptcut=0*CLHEP::GeV;
  m_tagptcut=0*CLHEP::GeV;
  m_etacut=999;
  m_tagetacut=999;
  m_TagAndProbe=false;
  m_useTrigger=false;
  //m_trkmass = 105.66*CLHEP::MeV;
  m_trkmass=0.511*CLHEP::MeV;
  m_tracks.clear();
  for(std::vector<Wrapper::MuonTrack*>::const_iterator it = m_tracks.begin(); it != m_tracks.end(); it++){
    delete (*it);
  }
  m_pairs.clear();
  m_MuonCollection=0;
  m_TrigCollection=0;
  m_upperlimit = 999;
  m_lowerlimit = -999;
  m_taggedMuons.clear();
  m_selectaProbe.cleanup();
  m_selectaTag.cleanup();
}

// input tracks, cuts or turn on tag-and-probe

void MakeDiMuonsTool::setProbes(std::vector<const TrigMuonEFTrack*> trksIn, std::vector<int> roi) {
  for( unsigned i=0; i<trksIn.size(); ++i ) m_selectaProbe.muonSelector( trksIn[i], NULL, roi[i] );
  m_tracks = m_selectaProbe.tracks();
}

void MakeDiMuonsTool::setProbes(std::vector<const CaloCluster*> CombinedCollection, std::vector<int> roi) {
  for( unsigned i=0; i<CombinedCollection.size(); ++i ) m_selectaProbe.muonSelector( CombinedCollection[i], roi[i] );
  m_tracks = m_selectaProbe.tracks();
}

void MakeDiMuonsTool::setProbes(std::vector<const Rec::TrackParticle*> trksIn) {
  for( unsigned i=0; i<trksIn.size(); ++i ) m_selectaProbe.muonSelector( trksIn[i] );
  m_tracks = m_selectaProbe.tracks();
}

void MakeDiMuonsTool::setProbes( const TrigInDetTrackCollection* m_TrigCollection ){ 
  TrigInDetTrackCollection::const_iterator tIt = m_TrigCollection->begin();
  TrigInDetTrackCollection::const_iterator tLast = m_TrigCollection->end();
  while(tIt != tLast){
    m_selectaProbe.muonSelector( *tIt );
    tIt++;
  }
  m_tracks = m_selectaTag.tracks();
  
}


void MakeDiMuonsTool::setTags( const Analysis::MuonContainer* MuonCollection) {
  m_MuonCollection = MuonCollection;
  TagMuons(m_MuonCollection);
}

void MakeDiMuonsTool::setTags( std::vector< const TrigMuonEFCbTrack* > CombinedCollection, std::vector<int> roiNums ){
  m_CombinedCollection = CombinedCollection;
  TagMuons(m_CombinedCollection, roiNums);
}

void MakeDiMuonsTool::doTagAndProbe(bool doTagAndProbe) {m_TagAndProbe = doTagAndProbe;}
void MakeDiMuonsTool::useTrigger(bool useTrigger) {m_useTrigger=useTrigger;}
void MakeDiMuonsTool::setPtcut(double ptcut) {m_ptcut = ptcut;}
void MakeDiMuonsTool::setTagPtcut(double ptcut) {m_tagptcut = ptcut;}
void MakeDiMuonsTool::setEtacut(double etacut) {m_etacut=etacut;}
void MakeDiMuonsTool::setTagEtacut(double etacut) {m_tagetacut=etacut;}
void MakeDiMuonsTool::setTrackMass(double muonmass) {m_trkmass=muonmass;}
void MakeDiMuonsTool::setUpperInvMassLimit(double upperlimit) {m_upperlimit=upperlimit;}
void MakeDiMuonsTool::setLowerInvMassLimit(double lowerlimit) {m_lowerlimit=lowerlimit;}

void MakeDiMuonsTool::TagMuons( const Analysis::MuonContainer* m_MuonCollection ){ 
  Analysis::MuonContainer::const_iterator muonItr;
  for (muonItr=m_MuonCollection->begin(); muonItr != m_MuonCollection->end(); ++muonItr)
    {
      if ((*muonItr)->hasInDetTrackParticle()) {
	if (m_TagAndProbe) {
	  if (m_useTrigger && m_trigEffJpsiTools->isTriggeredMuon( (*muonItr)->inDetTrackParticle(),1)) {
	    m_selectaTag.muonSelector( (*muonItr)->inDetTrackParticle());
	  }
	  else if (!m_useTrigger && (*muonItr)->isCombinedMuon()) {
	    m_selectaTag.muonSelector( (*muonItr)->inDetTrackParticle());
	  }
	}
      }
    }

  m_taggedMuons = m_selectaTag.tracks();
  
}

void MakeDiMuonsTool::TagMuons( std::vector<const TrigMuonEFCbTrack*> m_CombinedCollection, std::vector<int> roiNums ){
  std::vector<const TrigMuonEFCbTrack*>::const_iterator tagItr;
  if (!m_CombinedCollection.empty()){
    int i = 0;
    for (tagItr=m_CombinedCollection.begin(); tagItr!=m_CombinedCollection.end();++tagItr){
      if ( (*tagItr) ){
	if (m_TagAndProbe){
	  m_selectaTag.muonSelector( (*tagItr), NULL, roiNums[i] );
	}
      }
      i++;
    }
  }
 
  m_taggedMuons = m_selectaTag.tracks();

}

void MakeDiMuonsTool::setTags(std::vector<const egamma*> trksIn, std::vector<int> roi) {
    for(unsigned int i=0; i < trksIn.size(); ++i) {
      if(trksIn[i] && m_TagAndProbe) {
        m_selectaTag.muonSelector(trksIn[i], roi[i]);
      }
    }
  m_taggedMuons = m_selectaTag.tracks();
}

//Execute
std::vector<DiMuonTool*> MakeDiMuonsTool::execute()
{
  if (m_TagAndProbe) makePairs(m_taggedMuons,m_tracks);
  else makePairs(m_tracks);
  return m_pairs;
}
void MakeDiMuonsTool::makePairs(std::vector<Wrapper::MuonTrack*> Tracks){
  std::vector<Wrapper::MuonTrack*>::const_iterator track1;
  std::vector<Wrapper::MuonTrack*>::const_iterator track2;
  if(Tracks.size()>=2){
    for(track1=Tracks.begin();track1<Tracks.end();track1++){
      for(track2=(track1+1);track2!=Tracks.end();track2++){
	if ((*track1)->pt()/CLHEP::GeV>(m_ptcut/CLHEP::GeV) && (fabs((*track1)->eta())<m_etacut))
	  if ((*track2)->pt()/CLHEP::GeV>(m_ptcut/CLHEP::GeV) && (fabs((*track2)->eta())<m_etacut))
	  if (((*track1)->charge()*(*track2)->charge())<0) {
	      DiMuonTool* dimuon = new DiMuonTool();
	      dimuon->setTrackMass(m_trkmass);
	      dimuon->setTagAndProbe(m_TagAndProbe);
	      if ((*track1)->charge()>0){
		dimuon->setTracks(*track1,*track2);
	      }
	      if ((*track2)->charge()>0){
		dimuon->setTracks(*track2,*track1);
	      }
	      if ( dimuon->InvariantMass()<m_upperlimit && dimuon->InvariantMass()>m_lowerlimit ) 
		{
                  m_pairs.push_back(dimuon);
		}
	      else delete dimuon;
	  } 
      }
    }
  }
} //Muons(+,-)

void MakeDiMuonsTool::makePairs(std::vector<Wrapper::MuonTrack*> taggedTracks,std::vector<Wrapper::MuonTrack*> probedTracks) {
  //std::cout << "JPSI TOOLS -- makePair sizes - tag: " << taggedTracks.size() << " and probe:" << probedTracks.size() << std::endl;
  std::vector<Wrapper::MuonTrack*>::const_iterator tagItr;
  std::vector<Wrapper::MuonTrack*>::const_iterator probeItr;
  if(taggedTracks.size()>=1 && probedTracks.size()>=1 ) {
    for(tagItr=taggedTracks.begin();tagItr!=taggedTracks.end();tagItr++) {
      //std::cout << "tag pt: " << (*tagItr)->pt() << " cut: " << m_tagptcut << std::endl;
      //std::cout << "tag eta: " << (*tagItr)->eta() << " cut: " << m_tagetacut << std::endl;
      if((*tagItr)->pt() > m_tagptcut && fabs((*tagItr)->eta()) < m_tagetacut) {
        for(probeItr=probedTracks.begin();probeItr!=probedTracks.end();probeItr++) { 
          //std::cout << "probe pt: " << (*probeItr)->pt() << " cut: " << m_ptcut << std::endl;
          //std::cout << "probe eta: " << (*probeItr)->eta() << " cut: " << m_etacut << std::endl;
          if((*probeItr)->pt()/CLHEP::GeV>(m_ptcut/CLHEP::GeV) && (fabs((*probeItr)->eta())<m_etacut)) {
            //std::cout << "probe charge: " << (*probeItr)->charge() << " tag charge: " << (*tagItr)->charge() << std::endl;
	          //if(((*tagItr)->charge()*(*probeItr)->charge())<0) {    //!!!!!!!!!!!!!!!!!!!!!!!!!!!! <----- !!!!!
              DiMuonTool* dimuon = new DiMuonTool();
              dimuon->setTrackMass(m_trkmass);
              dimuon->setTagAndProbe(m_TagAndProbe);
              dimuon->setTracks(*tagItr,*probeItr);
              //std::cout << "mass : " << dimuon->InvariantMass()<< " upper/lower limit: " << m_upperlimit << " , " << m_lowerlimit << std::endl;
	            if( dimuon->InvariantMass()<m_upperlimit && dimuon->InvariantMass()>m_lowerlimit) {
                //std::cout << "Winning probe pt " <<  (*probeItr)->pt()/CLHEP::GeV << " eta " << (*probeItr)->eta()/CLHEP::GeV
	      			  //<< "\nWinning tag pt " <<  (*tagItr)->pt()/CLHEP::GeV << " eta " << (*tagItr)->eta()/CLHEP::GeV 
	      			  //<< std::endl;
	              m_pairs.push_back(dimuon);
              } 
	            else {
	              //std::cout << "FAILED at invmass cut with invmass " << dimuon->InvariantMass() << std::endl;
	              delete dimuon;
	            }
	          //} //else std::cout << "FAILED at charge cut." << std::endl;
	        } //else  std::cout << "FAILED at probe pt or eta cut. " << std::endl;
        }
      }	//else  std::cout << "FAILED at tag pt or eta cut. " << std::endl;
    }
  }
  //  return(myPairs);
} //Muons(tag,probe)

bool MakeDiMuonsTool::isTriggeredEF(const Rec::TrackParticle* track,std::string chainName) const {
  if (m_useTrigger) return m_trigEffJpsiTools->isTriggeredMuonEF(track, chainName);
  else return false;
}


bool MakeDiMuonsTool::isTriggeredL2(const Rec::TrackParticle* track,std::string chainName) const {
  if (m_useTrigger) return m_trigEffJpsiTools->isTriggeredMuonL2(track, chainName);
  else return false;
}

bool MakeDiMuonsTool::isTriggeredL1(const Rec::TrackParticle* track,std::string triggerName) const {
  if (m_useTrigger) return m_trigEffJpsiTools->isTriggeredMuonL1(track, triggerName);
  else return false;
}

void MakeDiMuonsTool::clear(){

  m_selectaProbe.cleanup();
  m_selectaTag.cleanup();
  m_tracks.clear();
  m_taggedMuons.clear();
  m_CombinedCollection.clear();
  for(std::vector<DiMuonTool*>::const_iterator it = m_pairs.begin(); it != m_pairs.end(); it++){
    (*it)->clear();
    delete (*it);
  }   
  m_pairs.clear();

}

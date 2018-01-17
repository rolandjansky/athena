/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
MuonPerformanceAlg
***************************************************************************/

#include "MuonPerformanceAlg.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"


//#include "MuonID/MuonSelectionTool.h"

//  CONSTRUCTOR:
MuonPerformanceAlg::MuonPerformanceAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : 
  AthAlgorithm(name, pSvcLocator),
  m_writeToFile (false),
  m_nevents(0),
  m_runNumber(0),
  m_eventNumber(0)
{
  declareProperty("writeToFile",        m_writeToFile = false);
  declareProperty("FileName",           m_fileName = "MuonPerformanceAlg.txt" );
  declareProperty("ConsideredPDGs",             m_pdgsToBeConsidered );


}

// INITIALIZE METHOD:
 
StatusCode MuonPerformanceAlg::initialize()
{

  // initialize cuts, please make sure the number of bins and the sizes of the cuts + string are always the same
  unsigned int nbins = 12;
  m_nevents = 0;
  m_ntruth.resize(nbins);
  m_ntruth5.resize(nbins);
  m_ntruth10.resize(nbins);
  m_nfound.resize(nbins);
  m_nfound5.resize(nbins);
  m_nfound10.resize(nbins);
  m_nfoundr.resize(nbins);
  m_nfoundr5.resize(nbins);
  m_nfoundr10.resize(nbins);
  m_nreco.resize(nbins);
  m_nreco5.resize(nbins);
  m_nreco10.resize(nbins);
  m_hitCutString = { "SA 2.0   ", "CB all   ", "MuidCB   ", "MuGirl   ", "Tag      ","Calo     ", "ID       ","SA(no ID)","Tight    ", "Medium   ", "Loose    ","Combined " };

  // add muons 
  if( m_pdgsToBeConsidered.value().empty() ){
    m_selectedPdgs.insert(13);
    m_selectedPdgs.insert(-13);
  }else{
    // add pdgs
    for( auto pdg : m_pdgsToBeConsidered.value() ) m_selectedPdgs.insert(pdg);
  }
  if(!m_muonsNameKey.key().empty()) ATH_CHECK(m_muonsNameKey.initialize());

  ATH_CHECK(m_eventInfo.initialize());
  ATH_CHECK(m_truthMuons.initialize());
  return StatusCode::SUCCESS;
}
 

/////////////////////////////////////////////////////////////////

bool MuonPerformanceAlg::passID(const xAOD::TrackParticle* tp, bool debug) const
{

  uint8_t nblh   = 0x0;
  uint8_t eblh   = 0x0;
  uint8_t nblo   = 0x0;
  uint8_t nphi   = 0x0;
  uint8_t npds   = 0x0;
  uint8_t nsctds = 0x0;
  uint8_t nscthi = 0x0;
  uint8_t nsctho = 0x0;
  uint8_t npho   = 0x0;
  uint8_t ntrthi = 0x0;
  uint8_t ntrtol = 0x0;
      
  int tightness = 0;

  bool passesIDcuts = false;

  if( tp  ){
      
    // check blayer 
    tp->summaryValue(eblh,xAOD::expectInnermostPixelLayerHit);
    tp->summaryValue(nblh,xAOD::numberOfInnermostPixelLayerHits);
    tp->summaryValue(nblo,xAOD::numberOfInnermostPixelLayerOutliers);
    if( static_cast<int>(eblh) ==0 ||
	static_cast<int>(nblh) + static_cast<int>(nblo) > 0 )   tightness += 1;

    // pixel hit counts
    tp->summaryValue(nphi,xAOD::numberOfPixelHits);
    tp->summaryValue(npds,xAOD::numberOfPixelDeadSensors);
    if( static_cast<int>(nphi) + static_cast<int>(npds) >= 2 ) tightness +=2;

    // sct hit counts
    tp->summaryValue(nscthi,xAOD::numberOfSCTHits);
    tp->summaryValue(nsctds,xAOD::numberOfSCTDeadSensors);
    if( static_cast<int>(nscthi) + static_cast<int>(nsctds) > 5 )      tightness +=4;

    // hole cuts
    tp->summaryValue(npho,xAOD::numberOfPixelHoles);
    tp->summaryValue(nsctho,xAOD::numberOfSCTHoles);
    if( static_cast<int>(npho) + static_cast<int>(nsctho) < 2 )         tightness +=8;

    // trt cuts
    tp->summaryValue(ntrthi,xAOD::numberOfTRTHits);
    tp->summaryValue(ntrtol,xAOD::numberOfTRTOutliers );
    int ntrt = static_cast<int>(ntrthi) + static_cast<int>(ntrtol);
    bool pass = true;
    if(  ntrt > 5 && static_cast<int>(ntrtol)/(double)ntrt > 0.9 ) pass = false;                             
    if( (fabs(tp->eta()) > 0.1 && fabs(tp->eta()) < 1.9 ) && ntrt <= 5 )                  pass = false; 
    if( pass ) tightness += 16;
//
// B layer is not a requirement anymore 
//
    if(pass && tightness > 29) passesIDcuts = true;

    if(passesIDcuts&&debug) ATH_MSG_VERBOSE("ID trackparticle PASSED IDcuts with tightness " <<  tightness);

  }
  if(!passesIDcuts&&debug) ATH_MSG_DEBUG("ID trackparticle FAILED IDcuts with tightness " <<  tightness);

  return passesIDcuts;

}



StatusCode MuonPerformanceAlg::execute()
{

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);

  m_runNumber = eventInfo->runNumber();
  m_eventNumber = eventInfo->eventNumber();

  SG::ReadHandle<xAOD::TruthParticleContainer> TruthMuons(m_truthMuons);
  if(!TruthMuons.isPresent()){
    ATH_MSG_DEBUG("no truth muon collection present");
    return StatusCode::SUCCESS;
  }
  if(!TruthMuons.isValid()){
    ATH_MSG_WARNING(m_truthMuons.key()<<" not valid");
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Retrieved truth muons " << TruthMuons->size());
  typedef ElementLink< xAOD::MuonContainer > MuonLink;

  m_nevents +=1;

  for (const auto truthMu: *TruthMuons){
    MuonLink link;
    if( truthMu->pt() < 2000. || fabs(truthMu->eta()) > 2.5 ) continue;
    const int& theType   = truthMu->auxdata<int>("truthType");
    const int& theOrigin = truthMu->auxdata<int>("truthOrigin");
    if( theType != 6 && theType != 7 ) continue;
    if( theOrigin == 0 || theOrigin > 17 ) continue;
    ATH_MSG_VERBOSE("first loop: type "<< theType << " origin " << theOrigin );
    bool insideID = false;
    if( fabs(truthMu->eta()) < 2.0 ) insideID = true;
    ATH_MSG_VERBOSE("Truth muon: pt " << truthMu->pt() << " eta " << truthMu->eta() );

    if(!insideID) m_ntruth[0]+=1;
    if(!insideID) m_ntruth[8]+=1;
    if(!insideID) m_ntruth[9]+=1;
    if(!insideID) m_ntruth[10]+=1;
    if(insideID) for(int n=1; n<6; n++) m_ntruth[n]+=1;
    if(insideID) for(int n=7; n<11; n++) m_ntruth[n]+=1;

    if(truthMu->pt() > 5000.) {
      if(!insideID) m_ntruth5[0]+=1;
      if(!insideID) m_ntruth5[8]+=1;
      if(!insideID) m_ntruth5[9]+=1;
      if(!insideID) m_ntruth5[10]+=1;
      if(insideID) for(int n=1; n<6; n++) m_ntruth5[n]+=1;
      if(insideID) for(int n=7; n<11; n++) m_ntruth5[n]+=1;
    } 

    if(truthMu->pt() > 10000.) {
      if(!insideID) m_ntruth10[0]+=1;
      if(!insideID) m_ntruth10[8]+=1;
      if(!insideID) m_ntruth10[9]+=1;
      if(!insideID) m_ntruth10[10]+=1;
      if(insideID) for(int n=1; n<6; n++) m_ntruth10[n]+=1;
      if(insideID) for(int n=7; n<11; n++) m_ntruth10[n]+=1;
    } 

    if( truthMu->isAvailable< MuonLink >("recoMuonLink") ) link = truthMu->auxdata< MuonLink >("recoMuonLink");
    else {
      ATH_MSG_VERBOSE("No muon found for " << "pt " << truthMu->pt() << " eta " << truthMu->eta() << " precisionHits " << truthMu->auxdata<uint8_t>("nprecLayers") );
      print(" Muon not found  by CaloTag and Calolikelihood ",  truthMu );
      print(" Muon not found by MuidSA ", truthMu);
      print(" Muon not found by MuTagIMO ", truthMu);
      print(" Muon not found by MuGirl ", truthMu); 
      print(" Muon not found by MuidCo ", truthMu);
      print(" Combined Muon not found ", truthMu);
    }
    ATH_MSG_VERBOSE(" link " << link.isValid() );
    if( link.isValid() ){
        bool loose = false;
        bool medium = false;
        bool tight = false;
        if(((*link)->quality() == xAOD::Muon_v1::Loose)) loose = true;
        if(((*link)->quality() == xAOD::Muon_v1::Medium)) loose = true;
        if(((*link)->quality() == xAOD::Muon_v1::Tight)) loose = true;
        if(((*link)->quality() == xAOD::Muon_v1::Medium)) medium = true;
        if(((*link)->quality() == xAOD::Muon_v1::Tight)) medium = true;
        if(((*link)->quality() == xAOD::Muon_v1::Tight)) tight = true;
        if(insideID){
          const xAOD::TrackParticle* tp  = (*link)->primaryTrackParticle();
          if(tp) {
          } else ATH_MSG_VERBOSE("No Track particle found on recoMuonLink");
// ID efficiency can only be measured on muons in the container 

          m_ntruth[6]+=1;
          if(truthMu->pt() > 5000.) m_ntruth5[6]+=1;
          if(truthMu->pt() > 10000.) m_ntruth10[6]+=1;

          bool passesIDcuts = passID(tp,true);
          ATH_MSG_VERBOSE(" all authors " << (*link)->allAuthors() << " passesIDcuts " << passesIDcuts );
          if(passesIDcuts!=(*link)->passesIDCuts()) {  
            ATH_MSG_DEBUG(" PROBLEM passedIDcuts from xAOD muon: " << (*link)->passesIDCuts() << " BUT MuonPerformancAlg code gives " << passesIDcuts);
          } 
// Use xAOD Muon ID hit selection cuts
          passesIDcuts = (*link)->passesIDCuts();
  	  if(passesIDcuts){
            if(truthMu->auxdata<uint8_t>("nprecLayers")>0) {
              m_ntruth[11]+=1;
              if(truthMu->pt() > 5000.) m_ntruth5[11]+=1;
              if(truthMu->pt() > 10000.) m_ntruth10[11]+=1;
            }
	    if(loose) {
              m_nfound[10]+=1;
            } else print(" Muon not found by Loose ", truthMu);
	    if(medium) {
              m_nfound[9]+=1;
            } else if(!loose) print(" Muon not found by Medium ", truthMu);
	    if(tight) {
              m_nfound[8]+=1;
            } else if(!medium) print(" Muon not found by Tight ", truthMu);
	    m_nfound[6]+=1;
            if(truthMu->pt() > 5000.) {
              if(loose) m_nfound5[10]+=1;
              if(medium) m_nfound5[9]+=1;
              if(tight) m_nfound5[8]+=1;
	      m_nfound5[6]+=1;
            }
            if(truthMu->pt() > 10000.) {
              if(loose) m_nfound10[10]+=1;
              if(medium) m_nfound10[9]+=1;
              if(tight) m_nfound10[8]+=1;
	       m_nfound10[6]+=1;
            }
	    //CaloTag or Calolikelihood
	    if(((*link)->allAuthors() & 64*4) || ((*link)->allAuthors() & 64*8) ) {
              m_nfound[5]+=1;
              if(truthMu->pt() > 5000.) m_nfound5[5]+=1;
              if(truthMu->pt() > 10000.) m_nfound10[5]+=1;
            } else print(" Muon not found  by CaloTag and Calolikelihood ",  truthMu );
	    //MuidSA
	    if(((*link)->allAuthors() & 32)) {
              m_nfound[7]+=1;
              if(truthMu->pt() > 5000.) m_nfound5[7]+=1;
              if(truthMu->pt() > 10000.) m_nfound10[7]+=1;
            } else if(!((*link)->allAuthors() & 2))  print(" Muon not found by MuidSA ", truthMu);
	    //MuTag or  MuTagIMO
	    if(((*link)->allAuthors() & 8) || ((*link)->allAuthors() & 16) ) {
              m_nfound[4]+=1;
              if(truthMu->pt() > 5000.) m_nfound5[4]+=1;
              if(truthMu->pt() > 10000.) m_nfound10[4]+=1;
            } else print(" Muon not found by MuTagIMO ", truthMu);
	    //MuGirl
	    if(((*link)->allAuthors() & 64) ) {
              m_nfound[3]+=1;
              if(truthMu->pt() > 5000.) m_nfound5[3]+=1;
              if(truthMu->pt() > 10000.) m_nfound10[3]+=1;
            } else print(" Muon not found by MuGirl ", truthMu);
	    //MuidCo
	    if(((*link)->allAuthors() & 2) ) {
              m_nfound[2]+=1;
              if(truthMu->pt() > 5000.) m_nfound5[2]+=1;
              if(truthMu->pt() > 10000.) m_nfound10[2]+=1;
            } else print(" Muon not found by MuidCo ", truthMu);
	    //MuidCo || STACO || Combined
	    if(((*link)->allAuthors() & 2) || ((*link)->allAuthors() & 4) || (*link)->muonType() == xAOD::Muon::MuonType::Combined ) {
              m_nfound[1]+=1;
              if(truthMu->pt() > 5000.) m_nfound5[1]+=1;
              if(truthMu->pt() > 10000.) m_nfound10[1]+=1;
              if(truthMu->auxdata<uint8_t>("nprecLayers")>0) {
                m_nfound[11]+=1;
                if(truthMu->pt() > 5000.) m_nfound5[11]+=1;
                if(truthMu->pt() > 10000.) m_nfound10[11]+=1;
              }
            } else print(" Combined Muon not found ", truthMu);
	  } else {
// SA or CB  not passing ID cuts
	    if(((*link)->allAuthors() & 32)|| (*link)->muonType() == xAOD::Muon::MuonType::Combined ) {
              m_nfound[7]+=1;
              if(truthMu->pt() > 5000.) m_nfound5[7]+=1;
              if(truthMu->pt() > 10000.) m_nfound10[7]+=1;
            } else print(" SA (CB) Muon not found ", truthMu);
          }
        }
      //MuidSA
        else if(!insideID){
	  if( (((*link)->allAuthors() & 32)) || ((*link)->allAuthors() & 2) || ((*link)->allAuthors() & 4) ) {
            m_nfound[0]+=1;
            if(truthMu->pt() > 5000.) m_nfound5[0]+=1;
            if(truthMu->pt() > 10000.) m_nfound10[0]+=1;
          } else  print(" Muon not found by MuidSA endcap ", truthMu);
	  if(loose) {
            m_nfound[10]+=1;
            if(truthMu->pt() > 5000.) m_nfound5[10]+=1;
            if(truthMu->pt() > 10000.) m_nfound10[10]+=1;
          } else print(" Muon not found by Loose endcap ", truthMu);
	  if(medium) {
            m_nfound[9]+=1;
            if(truthMu->pt() > 5000.) m_nfound5[9]+=1;
            if(truthMu->pt() > 10000.) m_nfound10[9]+=1;
          } else if(!loose) print(" Muon not found by Medium endcap ", truthMu);
	  if(tight) {
            m_nfound[8]+=1;
            if(truthMu->pt() > 5000.) m_nfound5[8]+=1;
            if(truthMu->pt() > 10000.) m_nfound10[8]+=1;
          } else if(!medium) print(" Muon not found by Tight endcap ", truthMu);
        }
    } else {
      print(" No link Muon not found  by CaloTag and Calolikelihood ",  truthMu );
      print(" No link Muon not found by MuidSA ", truthMu);
      print(" No link Muon not found by MuTagIMO ", truthMu);
      print(" No link Muon not found by MuGirl ", truthMu); 
      print(" No link Muon not found by MuidCo ", truthMu);
      print(" No link Combined Muon not found ", truthMu);
    }


    for (const auto truthMu: *TruthMuons){
      MuonLink link;
      if( truthMu->pt() < 2000. || fabs(truthMu->eta()) > 3. ) continue;
      const int& theType   = truthMu->auxdata<int>("truthType");
      const int& theOrigin = truthMu->auxdata<int>("truthOrigin");
      if( theType != 6 && theType != 7 ) continue;
      if( theOrigin == 0 || theOrigin > 17 ) continue;
      bool insideID = false;
      if( truthMu->isAvailable< MuonLink >("recoMuonLink") ) link = truthMu->auxdata< MuonLink >("recoMuonLink");
      else {
      }
      if( link.isValid() ){
	const xAOD::TrackParticle* tp  = (*link)->primaryTrackParticle();
        if(tp) {
	  if( (*link)->pt() < 2000. || fabs((*link)->eta()) > 2.5 ) continue;
	  if( fabs((*link)->eta()) < 2.0 ) insideID = true;
          bool loose = false;
          bool medium = false;
          bool tight = false;
          if(((*link)->quality() == xAOD::Muon_v1::Loose)) loose = true;
          if(((*link)->quality() == xAOD::Muon_v1::Medium)) loose = true;
          if(((*link)->quality() == xAOD::Muon_v1::Tight)) loose = true;
          if(((*link)->quality() == xAOD::Muon_v1::Medium)) medium = true;
          if(((*link)->quality() == xAOD::Muon_v1::Tight)) medium = true;
          if(((*link)->quality() == xAOD::Muon_v1::Tight)) tight = true;
	  if(insideID){
 	    bool passesIDcuts = passID(tp,false);
// Use xAOD Muon ID hit selection cuts
            passesIDcuts = (*link)->passesIDCuts();
	    if(passesIDcuts){
	      if(loose) m_nfoundr[10]+=1;
	      if(medium) m_nfoundr[9]+=1;
	      if(tight) m_nfoundr[8]+=1;
	      m_nfoundr[6]+=1;
	      if(((*link)->allAuthors() & 64*4) || ((*link)->allAuthors() & 64*8) ) m_nfoundr[5]+=1;
	      if(((*link)->allAuthors() & 32)) m_nfoundr[7]+=1;
	      if(((*link)->allAuthors() & 8) || ((*link)->allAuthors() & 16) ) m_nfoundr[4]+=1;
	      if(((*link)->allAuthors() & 64) ) m_nfoundr[3]+=1;
	      if(((*link)->allAuthors() & 2) ) m_nfoundr[2]+=1;
	      if(((*link)->allAuthors() & 2) || ((*link)->allAuthors() & 4) || (*link)->muonType() == xAOD::Muon::MuonType::Combined ) m_nfoundr[1]+=1;
              if((*link)->pt() > 5000.) {
	        if(loose) m_nfoundr5[10]+=1;
	        if(medium) m_nfoundr5[9]+=1;
	        if(tight) m_nfoundr5[8]+=1;
	        m_nfoundr5[6]+=1;
	        if(((*link)->allAuthors() & 64*4) || ((*link)->allAuthors() & 64*8) ) m_nfoundr5[5]+=1;
	        if(((*link)->allAuthors() & 32)) m_nfoundr5[7]+=1;
	        if(((*link)->allAuthors() & 8) || ((*link)->allAuthors() & 16) ) m_nfoundr5[4]+=1;
	        if(((*link)->allAuthors() & 64) ) m_nfoundr5[3]+=1;
	        if(((*link)->allAuthors() & 2) ) m_nfoundr5[2]+=1;
	        if(((*link)->allAuthors() & 2) || ((*link)->allAuthors() & 4) || (*link)->muonType() == xAOD::Muon::MuonType::Combined ) m_nfoundr5[1]+=1;
              }
              if((*link)->pt() > 10000.) {
	        if(loose) m_nfoundr10[10]+=1;
	        if(medium) m_nfoundr10[9]+=1;
	        if(tight) m_nfoundr10[8]+=1;
	        m_nfoundr10[6]+=1;
	        if(((*link)->allAuthors() & 64*4) || ((*link)->allAuthors() & 64*8) ) m_nfoundr10[5]+=1;
	        if(((*link)->allAuthors() & 32)) m_nfoundr10[7]+=1;
	        if(((*link)->allAuthors() & 8) || ((*link)->allAuthors() & 16) ) m_nfoundr10[4]+=1;
	        if(((*link)->allAuthors() & 64) ) m_nfoundr10[3]+=1;
	        if(((*link)->allAuthors() & 2) ) m_nfoundr10[2]+=1;
	        if(((*link)->allAuthors() & 2) || ((*link)->allAuthors() & 4) || (*link)->muonType() == xAOD::Muon::MuonType::Combined ) m_nfoundr10[1]+=1;
              }
	    } else  { 
	      if(((*link)->allAuthors() & 32)|| (*link)->muonType() == xAOD::Muon::MuonType::Combined ) {
                m_nfoundr[7]+=1;
                if((*link)->pt() > 5000.) m_nfoundr5[7]+=1;
                if((*link)->pt() > 10000.) m_nfoundr10[7]+=1;
              }
            }
	  }
	  else if(!insideID){
	    if( (((*link)->allAuthors() & 32)) || ((*link)->allAuthors() & 2) || ((*link)->allAuthors() & 4) ) {
              m_nfoundr[0]+=1;
              if((*link)->pt() > 5000.) m_nfoundr5[0]+=1; 
              if((*link)->pt() > 10000.) m_nfoundr10[0]+=1; 
            }
	    if(loose) m_nfoundr[10]+=1;
	    if(medium) m_nfoundr[9]+=1;
	    if(tight) m_nfoundr[8]+=1;
            if((*link)->pt() > 5000.) {
	      if(loose) m_nfoundr5[10]+=1;
	      if(medium) m_nfoundr5[9]+=1;
	      if(tight) m_nfoundr5[8]+=1;
            }
            if((*link)->pt() > 10000.) {
	      if(loose) m_nfoundr10[10]+=1;
	      if(medium) m_nfoundr10[9]+=1;
	      if(tight) m_nfoundr10[8]+=1;
            }
	  }
          
        } else ATH_MSG_VERBOSE("No Track particle found on recoMuonLink");
      }
    }


    SG::ReadHandle<xAOD::MuonContainer> Muons(m_muonsNameKey);

    if (!Muons.isValid()) {
      ATH_MSG_ERROR ("Couldn't retrieve Muons container with key: " << m_muonsNameKey.key());
      return StatusCode::FAILURE;
    } 
    ATH_MSG_VERBOSE("Retrieved muons " << Muons->size());

    for(const auto mu : *Muons){
      if( mu->pt() < 2000. || fabs(mu->eta()) > 2.5 ) continue;
      const xAOD::TrackParticle* tp  = mu->primaryTrackParticle();

      if(tp) {
        ElementLink< xAOD::TruthParticleContainer > truthLink;
        bool passesIDcuts = passID(tp,false);
// Use xAOD Muon ID hit selection cuts
        passesIDcuts = mu->passesIDCuts();
 
        bool insideID = false;
        if( fabs(mu->eta()) < 2.0 ) insideID = true;
        if( tp->isAvailable< ElementLink< xAOD::TruthParticleContainer > > ("truthParticleLink") ) truthLink = tp->auxdata< ElementLink< xAOD::TruthParticleContainer >  >("truthParticleLink");
        bool fake = true;
        if(truthLink.isValid()) {
//          if( (*truthLink)->auxdata<int>("truthType") == 6 || (*truthLink)->auxdata<int>("truthType") == 7 ) {
//            if( (*truthLink)->auxdata<int>("truthOrigin") > 0 && (*truthLink)->auxdata<int>("truthOrigin") <= 17 ) {
          if( selectPdg( (*truthLink)->pdgId()) ) fake = false; 
//            }
//          }
        }
        bool loose = false;
        bool medium = false;
        bool tight = false;
        if(mu->quality() == xAOD::Muon_v1::Loose) loose = true;
        if(mu->quality() == xAOD::Muon_v1::Medium) loose = true;
        if(mu->quality() == xAOD::Muon_v1::Tight) loose = true;
        if(mu->quality() == xAOD::Muon_v1::Medium) medium = true;
        if(mu->quality() == xAOD::Muon_v1::Tight) medium = true;
        if(mu->quality() == xAOD::Muon_v1::Tight) tight = true;
        if(insideID){
  	  if(passesIDcuts){
	    if(loose) m_nreco[10]+=1;
	    if(medium) m_nreco[9]+=1;
	    if(tight) m_nreco[8]+=1;
	    m_nreco[6]+=1;
	    if((mu->allAuthors() & 64*4) || (mu->allAuthors() & 64*8) ) m_nreco[5]+=1;
	    if((mu->allAuthors() & 32)) m_nreco[7]+=1;
	    if((mu->allAuthors() & 8) || (mu->allAuthors() & 16) ) m_nreco[4]+=1;
	    if((mu->allAuthors() & 64) ) m_nreco[3]+=1;
	    if((mu->allAuthors() & 2) ) m_nreco[2]+=1;
	    if((mu->allAuthors() & 2) || (mu->allAuthors() & 4) || mu->muonType() == xAOD::Muon::MuonType::Combined ) m_nreco[1]+=1;
            if(mu->pt() > 5000.) {
	      if(loose) m_nreco5[10]+=1;
	      if(medium) m_nreco5[9]+=1;
	      if(tight) m_nreco5[8]+=1;
	      m_nreco5[6]+=1;
	      if((mu->allAuthors() & 64*4) || (mu->allAuthors() & 64*8) ) m_nreco5[5]+=1;
	      if((mu->allAuthors() & 32)) m_nreco5[7]+=1;
	      if((mu->allAuthors() & 8) || (mu->allAuthors() & 16) ) m_nreco5[4]+=1;
	      if((mu->allAuthors() & 64) ) m_nreco5[3]+=1;
	      if((mu->allAuthors() & 2) ) m_nreco5[2]+=1;
	      if((mu->allAuthors() & 2) || (mu->allAuthors() & 4) || mu->muonType() == xAOD::Muon::MuonType::Combined ) m_nreco5[1]+=1;
            }
            if(mu->pt() > 10000.) {
	      if(loose) m_nreco10[10]+=1;
	      if(medium) m_nreco10[9]+=1;
	      if(tight) m_nreco10[8]+=1;
	      m_nreco10[6]+=1;
	      if((mu->allAuthors() & 64*4) || (mu->allAuthors() & 64*8) ) m_nreco10[5]+=1;
	      if((mu->allAuthors() & 32)) m_nreco10[7]+=1;
	      if((mu->allAuthors() & 8) || (mu->allAuthors() & 16) ) m_nreco10[4]+=1;
	      if((mu->allAuthors() & 64) ) m_nreco10[3]+=1;
	      if((mu->allAuthors() & 2) ) m_nreco10[2]+=1;
	      if((mu->allAuthors() & 2) || (mu->allAuthors() & 4) || mu->muonType() == xAOD::Muon::MuonType::Combined ) m_nreco10[1]+=1;
            }
            if(fake) {
	      if((mu->quality() == xAOD::Muon_v1::Loose)) print(" Fake muon found by Loose ", mu); 
	      if((mu->quality() == xAOD::Muon_v1::Medium)) print(" Fake muon found by Medium ", mu); 
	      if((mu->quality() == xAOD::Muon_v1::Tight)) print(" Fake muon found by Tight ", mu);
	      if((mu->allAuthors() & 64*4) || (mu->allAuthors() & 64*8) ) print(" Fake muon found by CaloTag and Calolikelihood ", mu);
	      if((mu->allAuthors() & 32)) print(" Fake muon found by MuidSA ", mu); 
	      if((mu->allAuthors() & 8) || (mu->allAuthors() & 16) ) print(" Fake muon found by MuTagIMO ", mu); 
	      if((mu->allAuthors() & 64) )  print(" Fake muon found by MuGirl ", mu);
              if((mu->allAuthors() & 2) ) print(" Fake muon found by MuidCo ", mu);
              if((mu->allAuthors() & 2) || (mu->allAuthors() & 4) || mu->muonType() == xAOD::Muon::MuonType::Combined )  print(" Fake Combined muon ", mu);
            }     
	  } else { 
	    if((mu->allAuthors() & 32)|| mu->muonType() == xAOD::Muon::MuonType::Combined ) {
              m_nreco[7]+=1;
	      if(fake) print(" Fake muon found by MuidSA with no ID", mu); 
              if(mu->pt() > 5000.) m_nreco5[7]+=1;
              if(mu->pt() > 10000.) m_nreco10[7]+=1;
            }
          }
        }
        else if(!insideID){
	  if(loose) m_nreco[10]+=1;
	  if(medium) m_nreco[9]+=1;
	  if(tight) m_nreco[8]+=1;
	  if(loose&&fake) print(" Fake muon found by Loose Endcap ", mu); 
	  if(medium&&fake&&!loose) print(" Fake muon found by Medium Endcap ", mu); 
	  if(tight&&!medium&&fake) print(" Fake muon found by Tight Endcap ", mu); 
          if(mu->pt() > 5000.) {
	    if(loose) m_nreco5[10]+=1;
	    if(medium) m_nreco5[9]+=1;
	    if(tight) m_nreco5[8]+=1;
          }  
          if(mu->pt() > 10000.) {
	    if(loose) m_nreco10[10]+=1;
	    if(medium) m_nreco10[9]+=1;
	    if(tight) m_nreco10[8]+=1;
          }  
	  if( ((mu->allAuthors() & 32)) || (mu->allAuthors() & 2) || (mu->allAuthors() & 4) ) {
            m_nreco[0]+=1;
	    if(fake) print(" Fake muon found by MuidSA Endcap ", mu); 
            if(mu->pt() > 5000.) m_nreco5[0]+=1;
            if(mu->pt() > 10000.) m_nreco10[0]+=1;
          }
        }
      }
    }
  }       

  return StatusCode::SUCCESS;
  
}
void MuonPerformanceAlg::print(std::string txt, const xAOD::TruthParticle* muon)  
{

// Truth: r  4251 z   3147 theta 0.91843 phi -0.781 q*p(GeV)  1.081e+02 pt(Gev) 8.589e+01
// barcode 1489 mother 23 production vertex: r  1.06363 z -102.414
    int q  = 1;
    if(muon->pdgId()>0) q = -1;
    double p = sqrt(muon->e()*muon->e()-muon->m()*muon->m());
    ATH_MSG_DEBUG(txt << " run " << m_runNumber << " event " << m_eventNumber << std::endl  << " Truth:  pdgId " << muon->pdgId()  <<  " barcode " << muon->barcode() << " eta " << muon->eta() << " phi " << muon->phi() << " q*p (GeV) " << q*p/1000. << " pt (GeV) " << muon->pt()/1000. << " precisionLayers " << static_cast<int>(muon->auxdata<uint8_t>("nprecLayers")) );

}
void MuonPerformanceAlg::print(std::string txt, const xAOD::Muon* muon)  
{
    int nprec = 0;
    uint8_t nPrecision=0;
    if(muon->primaryTrackParticleLink().isValid()) {
      muon->primaryTrackParticle()->summaryValue(nPrecision,xAOD::numberOfPrecisionLayers);
      nprec = static_cast <int> (nPrecision);
    }
    double p = sqrt(muon->e()*muon->e()-muon->m()*muon->m());
    ATH_MSG_DEBUG(txt << " run " << m_runNumber << " event " << m_eventNumber << std::endl  << " eta " << muon->eta() << " phi " << muon->phi() << " q*p (GeV) " << muon->charge()*p/1000. << " pt (GeV) " << muon->pt()/1000. << " precisionLayers " << nprec << " nr segments " << muon->nMuonSegments() );
}

/////////////////////////////////////////////////////////////////
// ATHENA FINALIZE:


StatusCode MuonPerformanceAlg::finalize() 
{

  unsigned int width = 9;
  unsigned int precision = 3;

  msg(MSG::INFO) << std::endl;
  msg(MSG::INFO) <<  " Summary of the xAOD Muon performance "  << std::endl;
  msg(MSG::INFO) << "  Muon type  #Truth muons   Efficiency #Truth muons   Efficiency #Truth muons   Efficiency"<< std::endl;
  msg(MSG::INFO) << "                        (pt>2)                   (pt>5)                 (pt>10 GeV/c)    "<< std::endl;
  for( unsigned int i=0;i<12;++i ){
    if(  m_ntruth[i] == 0  ) { 
      msg(MSG::INFO) << " " << std::endl;
    } else {
      msg(MSG::INFO) << "    " << m_hitCutString[i];
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_ntruth[i]);  
      msg(MSG::INFO) << "       ";
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_nfound[i])/static_cast<double>(m_ntruth[i]);
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_ntruth5[i]);  
      msg(MSG::INFO) << "       ";
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      if(m_ntruth5[i]!=0) {
        msg(MSG::INFO) << static_cast<double>(m_nfound5[i])/static_cast<double>(m_ntruth5[i]);
      } else msg(MSG::INFO) << 0.; 

      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_ntruth10[i]);  
       msg(MSG::INFO) << "       ";
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      if(m_ntruth10[i]!=0) {
        msg(MSG::INFO) << static_cast<double>(m_nfound10[i])/static_cast<double>(m_ntruth10[i]) << std::endl;
      } else msg(MSG::INFO) << 0. << std::endl; 
    }
  }
  msg(MSG::INFO) << " The efficiency of the ID is calculated inside |eta| < 2 for the MCP ID hit selection cuts (it uses identified - Combined, Tagged or CaloTagged - muons with an ID track)"<< std::endl;
  msg(MSG::INFO) << " The efficiencies for CB all, MuidCB, MuGirl, Tag and Calo include the MCP ID cuts"<< std::endl;
  msg(MSG::INFO) << " The Tight, Medium and Loose efficiencies include MCP ID cuts for muons |eta| < 2"<< std::endl;
  msg(MSG::INFO) << " The SA 2.0 for |eta| >2 and SA (no ID) for |eta| < 2  doesnot include MCP ID cuts"<< std::endl;
  msg(MSG::INFO) << " The Combined efficiency is defined ID tracks after MCP cuts and requiring at least 1 muon station at truth level"<< std::endl;
  msg(MSG::INFO) << " Fakes are calculated with the selections listed above"<< std::endl;
  msg(MSG::INFO) << std::endl;

  msg(MSG::INFO) << "  Muon type #Fake muons rate #Fake muons rate  #Fake muons rate   "<< std::endl;
  msg(MSG::INFO) << "             (all pt>0,2)           (pt>5)        (pt>10 GeV/c)    "<< std::endl;
  for( unsigned int i=0;i<11;++i ){
    if(i==6) continue;
    if(  m_nevents == 0 ) { 
      msg(MSG::INFO) << " " << std::endl;
    } else {
      msg(MSG::INFO) << "  " << m_hitCutString[i];
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_nreco[i]-m_nfoundr[i]); 
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_nreco[i]-m_nfoundr[i])/static_cast<double>(m_nevents);
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_nreco5[i]-m_nfoundr5[i]);
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_nreco5[i]-m_nfoundr5[i])/static_cast<double>(m_nevents);
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) << static_cast<double>(m_nreco10[i]-m_nfoundr10[i]);
      msg(MSG::INFO) << std::setw(width) << std::setprecision(precision);
      msg(MSG::INFO) <<  static_cast<double>(m_nreco10[i]-m_nfoundr10[i])/static_cast<double>(m_nevents) << std::endl;
    }
  }

  msg(MSG::INFO) << std::endl;
  msg(MSG::INFO) << endmsg;

  return StatusCode::SUCCESS;
}

 

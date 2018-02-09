/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonResonancePairingTool.cxx

#include "MuonResonanceTools/MuonResonancePairingTool.h"

//**********************************************************************

MuonResonancePairingTool::MuonResonancePairingTool(std::string myname)
: AsgTool(myname) {

  declareProperty("HighMassWindow", m_highMassWindow = 110000.0);
  declareProperty("LowMassWindow",  m_lowMassWindow = 90000.0);

}

StatusCode MuonResonancePairingTool::initialize()
{
  // ATH_CHECK(m_muonPairingTool.retrieve());

  ATH_MSG_INFO("Pairing summary :  ");
  ATH_MSG_INFO("HighMassWindow   = " <<  m_highMassWindow );
  ATH_MSG_INFO("LowMassWindow    = " <<  m_lowMassWindow  );

  return StatusCode::SUCCESS;
}


//**********************************************************************

//std::vector<std::pair<xAOD::Muon*, xAOD::Muon*> >  MuonResonancePairingTool:: buildPairs(std::pair<xAOD::MuonContainer*,xAOD::MuonContainer*> muons) const{
std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> >  MuonResonancePairingTool:: buildPairs(std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> > muons) const{

  // received as input the vectors of positive and negative muons

  // create returned object

  //  ATH_MSG_DEBUG("building pairs "<<muons.first->size()<< " "<<muons.second->size());
  ATH_MSG_DEBUG("building pairs "<<muons.first.size()<< " "<<muons.second.size());

  std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > results;

  // we need at least one positive and one negative muon

  //  if(muons.first->size()==0 || muons.second->size()==0) return results;
  if(muons.first.size()==0 || muons.second.size()==0) return results;

  ATH_MSG_DEBUG("ok, enough muons, looping");

  //  for(xAOD::MuonContainer::iterator muplus = muons.first->begin();muplus!=muons.first->end();++muplus){
  //for(auto muplus :  * muons.first){
 for(auto muplus :  muons.first){
    //for(xAOD::MuonContainer::iterator muminus = muons.second->begin();muminus!=muons.second->end();++muminus){
   // for(auto muminus :  * muons.second){
for(auto muminus :  muons.second){
      
  ATH_MSG_DEBUG("getting trkparticle of muon with pt "<<muminus->pt());

  //      const xAOD::TrackParticle* primaryTrkPlus = muminus->trackParticle(xAOD::Muon::Primary);

      //ATH_MSG_DEBUG("test"<<muplus->trackParticle(xAOD::Muon::Primary)->p4());
      //ATH_MSG_DEBUG("test "<< primaryTrkPlus->p4().M());
      
  double mass=(muplus->trackParticle(xAOD::Muon::Primary)->p4()+muminus->trackParticle(xAOD::Muon::Primary)->p4()).M();
      
      ATH_MSG_DEBUG("MASS IS "<<mass);
      
      if(mass<m_lowMassWindow || mass>m_highMassWindow) continue;
      
      results.push_back(std::pair<const xAOD::Muon*,const  xAOD::Muon*>(muplus, muminus));
      
    }
    
  }

  return results;
  
}

//**********************************************************************


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         TrigInDetTrackTruthMap.cxx
 **
 **   Description:  - Stores a vector of pointers to GenParticles which match 
 **                   a TrigInDetTrack and associated matching quality 
 **                   quantities (nr of common hits only for now)
 **
 **   Author:       R.Goncalo    
 **
 **   Created:      Sat Jan  18 19:55:56 GMT 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 

#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include <iomanip>
#include <iostream>
#include <cassert>

/** accessors to fill map */
void TrigInDetTrackTruthMap::addMatch(const TrigInDetTrackCollection* trkColl, 
				      unsigned int trk_indx, 
				      TrigInDetTrackTruth& p_trk_tru) 
{
  std::string thisName("TrigInDetTrackTruthMap::addMatch");
  MsgStream log(Athena::getMessageSvc(), thisName);

  log << MSG::DEBUG << "Adding TrigInDetTrack to map" << endreq;

  // check entry for this track doesn't exist (otherwise we have a multimap)
  const TrigInDetTrack* p_trig_trk((*trkColl)[trk_indx]);

  if ( !hasTruth( p_trig_trk ) ) {
    ElementLink< TrigInDetTrackCollection > p_trk_lnk(*trkColl,trk_indx);
    m_elink_vec.push_back(p_trk_lnk);
    bool status = m_elink_vec.back().toPersistent();
    if (!status) 
      log << MSG::DEBUG << "ERROR: could not set ElementLink persistent" << endreq;
    m_truth_vec.push_back(p_trk_tru);
  } else {
    log << MSG::DEBUG << "TrigInDetTrack already in map!" << endreq;
  }
}  

/** methods to get truth-match objects */
// returns true if truth association exists for track
bool TrigInDetTrackTruthMap::hasTruth(const TrigInDetTrack* p_trig_trk) const
{
  // must loop over map because ElementLink is a unidirectional link
  bool found=false;  
  for (unsigned int i=0; i < m_elink_vec.size(); ++i) {
    //    if ( p_trig_trk == *(m_elink_vec[i]) ) {
    if(!m_elink_vec[i].isValid()) 
      continue;
    if ((*(m_elink_vec[i]))->param()) {
      if ((*(m_elink_vec[i]))->algorithmId()   == p_trig_trk->algorithmId() &&
	  (*(m_elink_vec[i]))->param()->pT()   == p_trig_trk->param()->pT() &&
	  (*(m_elink_vec[i]))->param()->eta()  == p_trig_trk->param()->eta() &&
	  (*(m_elink_vec[i]))->param()->phi0() == p_trig_trk->param()->phi0()) {
	found=true;
	break;
      }
    }
  }
  // true if this track pointer exists at least once in the vector
  return (found);
}


// returns the track truth association object
const TrigInDetTrackTruth* TrigInDetTrackTruthMap::truth(const TrigInDetTrack* p_trig_trk) const
{
  std::string thisName("TrigInDetTrackTruthMap::truth");
  MsgStream log(Athena::getMessageSvc(), thisName);

  log << MSG::DEBUG<<"Searching truth for track at ptr="<<p_trig_trk<<endreq;

  // must loop over map because ElementLink is a unidirectional link
  for (unsigned int i=0; i < m_elink_vec.size(); ++i) {
    //    if ( p_trig_trk == *(m_elink_vec[i]) ) {
    if(!m_elink_vec[i].isValid()) 
      continue;
    if ((*(m_elink_vec[i]))->param()) {
      if ((*(m_elink_vec[i]))->algorithmId()   == p_trig_trk->algorithmId() &&
	  (*(m_elink_vec[i]))->param()->pT()   == p_trig_trk->param()->pT() &&
	  (*(m_elink_vec[i]))->param()->eta()  == p_trig_trk->param()->eta() &&
	  (*(m_elink_vec[i]))->param()->phi0() == p_trig_trk->param()->phi0()) {
	// found position in vector corresponding to this track pointer
	log << MSG::DEBUG << "Truth match for track at ptr=" << p_trig_trk 
	    << " found in map at index " << i <<endreq;
	return &m_truth_vec[i];  
      }
    }
  } 
  // didn't find it: return null pointer
  log << MSG::DEBUG <<"Truth match for track at ptr="<<p_trig_trk <<" not in map"<<endreq;
  return NULL;
}


// to make the map more useful: return the link to GenParticle which better 
// matches this track  according to number of common hits
const HepMcParticleLink* TrigInDetTrackTruthMap::bestMatchSi(const TrigInDetTrack* p_trig_trk) const
{
  if ( hasTruth(p_trig_trk) ) {
    return ( truth(p_trig_trk)->bestSiMatch() );
  } else {
    return NULL;
  }
}

int TrigInDetTrackTruthMap::bestMatchSiHits(const TrigInDetTrack* p_trig_trk) const
{
  if ( hasTruth(p_trig_trk) ) {
    return ( truth(p_trig_trk)->nrCommonHitsBestSi() );
  } else {
    return 0;
  }
}

// to make the map more useful: return the link to GenParticle which better 
// matches this track  according to number of common hits
const HepMcParticleLink* TrigInDetTrackTruthMap::bestMatchTRT(const TrigInDetTrack* p_trig_trk) const
{
  if ( hasTruth(p_trig_trk) ) {
    return ( truth(p_trig_trk)->bestTRTMatch() );
  } else {
    return NULL;
  }
}

int TrigInDetTrackTruthMap::bestMatchTRTHits(const TrigInDetTrack* p_trig_trk) const
{
  if ( hasTruth(p_trig_trk) ) {
    return ( truth(p_trig_trk)->nrCommonHitsBestTRT() );
  } else {
    return 0;
  }
}


void TrigInDetTrackTruthMap::print() const
{
  std::string thisName("TrigInDetTrackTruthMap::print");
  MsgStream log(Athena::getMessageSvc(), thisName);

  std::ostringstream oss;
  oss << "TrigInDetTruthMap: " << m_elink_vec.size() 
	    << " track-truth associations" << std::endl;

  oss << "---------------------------------------------------------------------------------------------------------------------------------" << std::endl;
  oss << "#track|algo|  pointer  |       pT     |    eta   |    phi   |#match|mother|Sihits|TRThits|ev.index| barcode |  pdg id  |      pT      |   eta    |    phi   |"<< std::endl;
  for (unsigned int i=0; i < m_elink_vec.size(); ++i) {
    oss << std::setiosflags(std::ios::dec) << std::setw(6) << i << "|"
	<< std::setiosflags(std::ios::dec) << std::setw(4) << (*(m_elink_vec[i]))->algorithmId() << "|"
	<< std::setw(11) << *(m_elink_vec[i]) << "|";

    if(!m_elink_vec[i].isValid()) {
      oss << std::setiosflags(std::ios::dec) << "Invalid TrigInDetTrack link !"
	  << std::setiosflags(std::ios::dec) << std::setw(8) << "|";
    }
    else {
      // print reconstructed track parameters
      if ((*(m_elink_vec[i]))->param()) {
	oss << std::setiosflags(std::ios::dec) << std::setw(14) << (*(m_elink_vec[i]))->param()->pT()  << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(10) << (*(m_elink_vec[i]))->param()->eta() << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(10) << (*(m_elink_vec[i]))->param()->phi0()<< "|";
      } else {
	oss << std::setiosflags(std::ios::dec) << std::setw(15) << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(11) << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(11) << "|";
      }
    }

    if (m_truth_vec[i].nrMatches() == 0) oss << std::endl;
    // print parameters of truth particles which have hits in common with track
    for (unsigned int j=0; j < m_truth_vec[i].nrMatches(); ++j) {

      // find if this matching true particle has a mother which also matches the track
      HepMcParticleLink p_link( *((m_truth_vec[i]).truthMatch(j)) );
      int child_indx  = (m_truth_vec[i]).index(p_link);
      int mother_indx = -1;
      if (child_indx >= 0) mother_indx = (m_truth_vec[i]).motherIndexInChain(child_indx);

      if (j>0) {

	oss << std::setiosflags(std::ios::dec) << std::setw(7)  << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(5)  << "|";
	oss << std::setiosflags(std::ios::dec) << std::setw(15) << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(11) << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(11) << "|";
      }

      oss << std::setiosflags(std::ios::dec) << std::setw(6) << j+1 << "|";

      if (mother_indx >= 0) {
	oss << std::setiosflags(std::ios::dec) << std::setw(6) << mother_indx << "|";
      } else {
	oss << "  --  |";
      }
      oss << std::setiosflags(std::ios::dec) << std::setw(6) << (m_truth_vec[i]).nrCommonSiHits(j) << "|"
	  << std::setiosflags(std::ios::dec) << std::setw(7) << (m_truth_vec[i]).nrCommonTRTHits(j) << "|"
	  << std::setiosflags(std::ios::dec) << std::setw(8) << (m_truth_vec[i]).truthMatch(j)->eventIndex() << "|"
	  << std::setiosflags(std::ios::dec) << std::setw(9) << (m_truth_vec[i]).truthMatch(j)->barcode()    << "|";

      if ((m_truth_vec[i]).truthMatch(j)->cptr()) {

	oss << std::setiosflags(std::ios::dec) << std::setw(10) << (m_truth_vec[i]).truthMatch(j)->cptr()->pdg_id() << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(14) << (m_truth_vec[i]).truthMatch(j)->cptr()->momentum().perp()<< "|"
	    << std::setiosflags(std::ios::dec) << std::setw(10) << (m_truth_vec[i]).truthMatch(j)->cptr()->momentum().eta() << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(10) << (m_truth_vec[i]).truthMatch(j)->cptr()->momentum().phi() << "|";
      } else {
	oss << std::setiosflags(std::ios::dec) << std::setw(11) << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(15) << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(11) << "|"
	    << std::setiosflags(std::ios::dec) << std::setw(11) << "|";
      }
      oss << std::endl;
    }
  }
  oss << "---------------------------------------------------------------------------------------------------------------------------------" << std::endl;

  log << MSG::DEBUG << oss.str() << endreq;
}

size_t TrigInDetTrackTruthMap::size() const
{
  assert (m_elink_vec.size() == m_truth_vec.size());
  return m_elink_vec.size();
}

const TrigInDetTrackTruth*
TrigInDetTrackTruthMap::truthi (size_t i) const
{
  assert (i < m_truth_vec.size());
  return &m_truth_vec[i];
}

const TrigInDetTrack*
TrigInDetTrackTruthMap::tracki (size_t i) const
{
  assert (i < m_elink_vec.size());
  return *m_elink_vec[i];
}

const ElementLink<TrigInDetTrackCollection>
TrigInDetTrackTruthMap::trackiLink (size_t i) const
{
  assert (i < m_elink_vec.size());
  return m_elink_vec[i];
}

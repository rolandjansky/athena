/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         TrigInDetTrackTruth.cxx
 **
 **   Description:  - Stores a vector of pointers to GenParticles which match 
 **                   a TrigInDetTrack and associated matching quality 
 **                   quantities (nr of common hits only for now)
 **
 **   Author:       R.Goncalo    
 **
 **   Created:      Sat Jan  18 19:55:56 GMT 2006
 **   Modified:     RG 1/9/06 - added method for object to update own family tree
 **                 RG 2/9/06 - was going to change set of two vectors acting 
 **   as a map<HepMcPart.Link,TrigIDHitStats> into a GaudiUtils::VectorMap to
 **   improve readout speed but decided against this since it complicates 
 **   matters and creates dependencies, and because only a few (~1-5) GenParticles 
 **   can be expected to match any TrigInDetTrack, so the overhead is small.
 **   Changed m_family_tree from map<unsigned int,unsigned int> into
 **   vector< pair<unsigned int,unsigned int>> to avoid persistency problems and
 **   also to make it easier to use either mother or daughter indices as keys
 **************************************************************************/ 

#include "TrigInDetTruthEvent/TrigInDetTrackTruth.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "AtlasHepMC/Operators.h"

/** accessor to fill object: returns index of new entry in vectors */
int TrigInDetTrackTruth::addMatch(HepMcParticleLink p_tru_part,TrigIDHitStats hits)
{
  std::string thisName("TrigInDetTrackTruth::addMatch");
  MsgStream log(Athena::getMessageSvc(), thisName);

  log << MSG::DEBUG<< "Inserting HepMcParticleLink and TrigIDHitStats to TrigInDetTrackTruth map" << endmsg;

  int indx = index(p_tru_part);
  if ( indx >= 0 ) {
    // HepMcParticleLink already exists: replace in vectors
    log << MSG::DEBUG<< "HepMcParticleLink already in map: replacing" 
	<< endmsg;
    m_true_part_vec[indx]  = p_tru_part; 
    m_nr_common_hits[indx] = hits;
  } else {
    // push back into vectors to use HepMcParticleLink as key
    m_true_part_vec.push_back(p_tru_part); 
    m_nr_common_hits.push_back( hits );
    indx = m_true_part_vec.size() - 1;
  }

  // to know the best match in total nr.hits 
  if ( m_best_match_hits == -1 || 
       (unsigned int)hits.total() > nrCommonHits(m_best_match_hits) )
    m_best_match_hits = m_true_part_vec.size() - 1;

  if ( m_best_Si_match_hits == -1 || 
       (unsigned int)(hits.pixhits() + hits.scthits())> nrCommonSiHits(m_best_Si_match_hits) )
    m_best_Si_match_hits = m_true_part_vec.size() - 1;

  if ( m_best_TRT_match_hits == -1 || 
       (unsigned int)hits.trthits() > nrCommonTRTHits(m_best_TRT_match_hits) )
    m_best_TRT_match_hits = m_true_part_vec.size() - 1;

  return indx;
}

/** method to find if a given HepMcParticleLink already exists in "map" and, if 
    so, what is its index; if not found returns -1; relies on 
    HepMcParticleLink overloaded == method (i.e. compares only barcode) */
int TrigInDetTrackTruth::index(HepMcParticleLink& hep_link) const {

  std::vector<HepMcParticleLink>::const_iterator it1, end=m_true_part_vec.end();
  int indx = 0;
  for (it1=m_true_part_vec.begin(); it1 != end; ++it1) {
    if (hep_link == (*it1)) {
      // HepMcParticleLink found in vector: return index
      return indx;
    }
    ++indx;
  }
  // HepMcParticleLink not found: return default -1
  return -1;
}

/** accessor to fill family tree: for each HepMcParticleLink in the internal 
    vector of HepMC::GenParticles, this method searches for its mother in the 
    same vector. If the mother is in the vector, the method adds the relation 
    as a a mother-daughter pair to the family tree vector 
    note: this method can be used as many times as necessary as long as the 
    association object is not constant */
int TrigInDetTrackTruth::updateFamilyTree() 
{

  std::string thisName("TrigInDetTrackTruth::updateFamilyTree");
  MsgStream log(Athena::getMessageSvc(), thisName);

  log << MSG::DEBUG<< "In TrigInDetTrackTruth::updateFamilyTree()" << endmsg;

  int nr_mothers_found=0;
  int child=-1; 
  std::vector<HepMcParticleLink>::iterator it1, end=m_true_part_vec.end();

  for (it1=m_true_part_vec.begin(); it1 != end; ++it1) 
    {
      child++;
      /* get production vertex GenParticle pointed to by this link */
      log << MSG::DEBUG<< "Looking for mother of matching particle nr "<<child<<endmsg;

      // first get GenParticle pointer
      if ( !it1->isValid() ) continue; 
   
      auto p_child = (*it1);
      log << MSG::DEBUG << "GenParticle " << child << " (" << p_child << "); PDG id=" 
	  << p_child->pdg_id() << "; status=" << p_child->status() 
	  << "; pT=" << p_child->momentum().perp() 
	  << "; searches mother..." 
	  << endmsg;
      
      // then get production vertex (check against null)
      auto p_child_vtx = p_child->production_vertex();
      if ( !p_child_vtx) 
	{
	  log << MSG::DEBUG<<"GenVertex pointer null: jump to next particle"<<endmsg;
	  continue;
	}
      log << MSG::DEBUG<< "GenParticle "<< child << " comes from vertex with pointer "
	  << p_child_vtx << endmsg;
      
      /* find mother: there should be only one for final state particles 
	 (particles which can leave energy deposits in detectors)        */
#ifdef HEPMC3
     // check a mother was found
      if ( p_child_vtx->particles_in().size()==0)
	{
	  log << MSG::DEBUG<< "Mother not found: go to next particle" <<endmsg;
	  continue;
	}  
     auto p_mum = p_child_vtx->particles_in().begin();
      
#else
      HepMC::GenVertex::particles_in_const_iterator p_mum = p_child_vtx->particles_in_const_begin();
      
      // check a mother was found
      if ( p_mum == p_child_vtx->particles_in_const_end() )
	{
	  log << MSG::DEBUG<< "Mother not found: go to next particle" <<endmsg;
	  continue;
	} 
#endif
      log << MSG::DEBUG<< "Mother GenParticle (" << *p_mum << ") found; PDG id=" 
	  << (*p_mum)->pdg_id() << "; status=" << (*p_mum)->status()
	  << "; pT=" << (*p_mum)->momentum().perp() 
	  << "; does it match track?" 
	  << endmsg;
      // mother is (*p_mum); still have to see if it is a match to this track
      std::vector<HepMcParticleLink>::iterator it2=m_true_part_vec.begin();
      
      bool mum_found=false;
      for (unsigned int mum=0; it2 != end; ++it2, ++mum) 
	{
	  log << MSG::DEBUG << "* Trying daughter index=" << child 
	      << " and mother index=" << mum << endmsg;
          auto p2 = *it2;
          if ( *p_mum == p2 )
	    { // mother also matches track
	      m_family_tree.push_back( std::pair<unsigned int, unsigned int>(mum,child) );
	      mum_found=true;
	      nr_mothers_found++;
	      
	      log << MSG::DEBUG << "* Mother also matches track! " 
		  << nr_mothers_found 
		  << " mother-daughter relations found so far" << endmsg;
	      log << MSG::DEBUG << "Daughter "<< child <<" (PDG id="  
		  << p_child->pdg_id() << "; pT=" << p_child->momentum().perp() 
		  << ") comes from mother " << mum << " (PDG id=" 
		  << (*p_mum)->status() << "; pT=" << p_child->momentum().perp() 
		  << ")" << endmsg;
	    }
	}
      if (!mum_found) log << MSG::DEBUG << "* Mother doesn't match track"
			      << endmsg; 
    }
  return nr_mothers_found;
}

/** returns best match according to the number of hits */
const HepMcParticleLink* TrigInDetTrackTruth::bestMatch() const
{
  return &(m_true_part_vec[m_best_match_hits]);
}

/** returns best match according to the number of hits */
const HepMcParticleLink* TrigInDetTrackTruth::bestSiMatch() const
{
  return &(m_true_part_vec[m_best_Si_match_hits]);
}

/** returns best match according to the number of hits */
const HepMcParticleLink* TrigInDetTrackTruth::bestTRTMatch() const
{
  return &(m_true_part_vec[m_best_TRT_match_hits]);
}

/** returns matching true particle number i */
const HepMcParticleLink* TrigInDetTrackTruth::truthMatch(unsigned int i) const
{
  if (i < m_true_part_vec.size()) return &(m_true_part_vec[i]);
  else return NULL;
}


/** returns number of common hits from true particle i and TrigInDetTrack */
unsigned int TrigInDetTrackTruth::nrCommonHits(unsigned int i) const
{
  if (i < m_true_part_vec.size()) return m_nr_common_hits[i].total();
  else return 0;
}

/** returns number of common hits from true particle i and TrigInDetTrack */
unsigned int TrigInDetTrackTruth::nrCommonSiHits(unsigned int i) const
{
  if (i < m_true_part_vec.size()) return (m_nr_common_hits[i].pixhits() + m_nr_common_hits[i].scthits());
  else return 0;
}

/** returns number of common hits from true particle i and TrigInDetTrack */
unsigned int TrigInDetTrackTruth::nrCommonTRTHits(unsigned int i) const
{
  if (i < m_true_part_vec.size()) return m_nr_common_hits[i].trthits();
  else return 0;
}

/** returns total number of common hits from best match true particle and TrigInDetTrack */
unsigned int TrigInDetTrackTruth::nrCommonHitsBestSi() const
{
  return (m_nr_common_hits[m_best_Si_match_hits].scthits() + m_nr_common_hits[m_best_Si_match_hits].pixhits());
}

/** returns total number of common hits from best match true particle and TrigInDetTrack */
unsigned int TrigInDetTrackTruth::nrCommonHitsBestTRT() const
{
  return m_nr_common_hits[m_best_TRT_match_hits].trthits();
}


/** returns number of matching particles */
unsigned int TrigInDetTrackTruth::nrMatches() const 
{
  return m_true_part_vec.size();
}

/** returns copy of family tree "map" */
std::vector< std::pair<unsigned int, unsigned int> > TrigInDetTrackTruth::getFamilyTree() const 
{
  return m_family_tree;
}

/** given index of a GenParticle which matches the track returns index of  
    its mother, if it also matches the track, or -1 if not */
int TrigInDetTrackTruth::motherIndexInChain(unsigned int daughter) const {

  if ( m_family_tree.empty() ) return -1;

  std::vector< std::pair<unsigned int, unsigned int> >::const_iterator it,it_end = m_family_tree.end();

  for (it = m_family_tree.begin(); it != it_end; ++it) {
    if (daughter == (*it).second) return (int)(*it).first; 
  }
  return -1;
}

/** given index of a GenParticle which matches the track returns true
    if its mother also matches the track and false if not */
bool TrigInDetTrackTruth::motherInChain(unsigned int daughter) const {
  return ( motherIndexInChain(daughter) >=0 );
}

/** given index of a GenParticle which matches the track returns vector with indices 
    of its daughters, if they also matches the track, or an empty vector if not */
std::vector<unsigned int> TrigInDetTrackTruth::daughterIndicesInChain(unsigned int mother) const {

  std::vector<unsigned int> v_indx;
  v_indx.clear();
  if ( m_family_tree.empty() ) return v_indx;

  std::vector< std::pair<unsigned int, unsigned int> >::const_iterator it,it_end = m_family_tree.end();
  unsigned int i=0;
  for (it = m_family_tree.begin(); it != it_end; ++it) {
    if (mother == (*it).first) {
      v_indx.push_back(i);
    }
    ++i;
  }
  return v_indx;
}

/** given index of a GenParticle which matches the track returns true
    if it has stable a daughter which also matches the track and false if not */
bool TrigInDetTrackTruth::daughtersInChain(unsigned int mother) const {  
  std::vector<unsigned int> v_indx = daughterIndicesInChain(mother);
  return ( !(v_indx.empty()) );
}

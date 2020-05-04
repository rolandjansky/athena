/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McVtxFilter.cxx 
// Implementation file for class McVtxFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

// AnalysisUtils includes
#include "AnalysisUtils/AnalysisPermutation.h"

// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
McVtxFilter::McVtxFilter() : 
  IFilterCuts(),
  m_msg ( Athena::getMessageSvc(), "McVtxFilter" ),
  m_matchSign( false ),
  m_matchBranches( false ),
  m_decayPattern( "" ),
  m_parentList(),
  m_childList()
{}

McVtxFilter::McVtxFilter( const std::string& decayPattern,
			  const bool matchSign, 
			  const bool matchBranches ) : 
  IFilterCuts(),
  m_msg ( Athena::getMessageSvc(), "McVtxFilter" ),
  m_matchSign( matchSign ),
  m_matchBranches( matchBranches ),
  m_decayPattern( decayPattern ),
  m_parentList(),
  m_childList()
{
  setDecayPattern(decayPattern);
}

McVtxFilter::McVtxFilter( const bool matchSign, 
			  const bool matchBranches ) : 
  IFilterCuts(),
  m_msg ( Athena::getMessageSvc(), "McVtxFilter" ),
  m_matchSign( matchSign ),
  m_matchBranches( matchBranches ),
  m_decayPattern( "" ),
  m_parentList(),
  m_childList()
{}

McVtxFilter::McVtxFilter( const McVtxFilter& rhs ) : 
  IFilterCuts    ( rhs                 ),
  m_msg          ( rhs.m_msg           ),
  m_matchSign    ( rhs.m_matchSign     ),
  m_matchBranches( rhs.m_matchBranches ),
  m_decayPattern ( rhs.m_decayPattern  ),
  m_parentList(),
  m_childList()
{
  // deep copy of the parent branch
  m_parentList.reserve( rhs.m_parentList.size() );
  for ( DataVector<const ParticleCandidateList>::const_iterator itr = 
	  rhs.m_parentList.begin();
	itr != rhs.m_parentList.end();
	++itr ) {
    m_parentList.push_back( new ParticleCandidateList(**itr) );
  }

  // deep copy of the child branch
  m_childList.reserve( rhs.m_childList.size() );
  for ( DataVector<const ParticleCandidateList>::const_iterator itr = 
	  rhs.m_childList.begin();
	itr != rhs.m_childList.end();
	++itr ) {
    m_childList.push_back( new ParticleCandidateList(**itr) );
  }
}

McVtxFilter & McVtxFilter::operator=(const McVtxFilter &rhs)
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
    m_matchSign     = rhs.m_matchSign;
    m_matchBranches = rhs.m_matchBranches;
    m_decayPattern  = rhs.m_decayPattern;

    // deep copy of the parent branch
    m_parentList.reserve( rhs.m_parentList.size() );
    for ( DataVector<const ParticleCandidateList>::const_iterator itr = 
	    rhs.m_parentList.begin();
	  itr != rhs.m_parentList.end();
	  ++itr ) {
      m_parentList.push_back( new ParticleCandidateList(**itr) );
    }

    // deep copy of the child branch
    m_childList.reserve( rhs.m_childList.size() );
    for ( DataVector<const ParticleCandidateList>::const_iterator itr = 
	    rhs.m_childList.begin();
	  itr != rhs.m_childList.end();
	  ++itr ) {
      m_childList.push_back( new ParticleCandidateList(**itr) );
    }
  }
  return *this;
}

/// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////
bool McVtxFilter::isAccepted( const HepMC::GenVertex * vtx ) const
{
  m_msg << MSG::VERBOSE
	<< "In McVtxFilter::isAccepted(...)" << endmsg;

  ////////////////////////////////////////////////////////////////
  /// First handle special case where the filter has only 1 child
  /// and no parent : one is looking for a stable particle with
  /// no end_vertex
  if ( m_childList.size()        == static_cast<unsigned int>( 1 ) &&
       m_parentList.size()       == static_cast<unsigned int>( 0 ) &&
       vtx->particles_out_size() == static_cast<unsigned int>( 1 ) ) {
    const HepMC::GenParticle * part = *(vtx->particles_out_const_begin());
    const ParticleCandidateList * item = *( m_childList.begin() );
    if ( item->hasInList( static_cast<PDG::pidType>(part->pdg_id()), 
			  m_matchSign ) ) {
      return true;
    } else { 
      return false;
    }
  }

  /// Skip vertices which don't match the number of branches
  /// we are looking for : check if number of parents 
  /// and of children is OK
  if ( !m_matchBranches ) {

    //
    // Test only if there is enough *output* branches to match for
    //
    if ( static_cast<unsigned int>(vtx->particles_in_size())  < m_parentList.size() ||
	 static_cast<unsigned int>(vtx->particles_out_size()) < m_childList.size()  ) {
      return false;
    }

  } else {
    //
    // Strict match of output branches
    //
    if ( static_cast<unsigned int>(vtx->particles_in_size())  != m_parentList.size() ||
	 static_cast<unsigned int>(vtx->particles_out_size()) != m_childList.size()  ) {
      return false;
    }
  }

  //////////////////////////////////////////////////////////////
  /// Then handle the case we are looking for a 1->2 body decay
  ///
  if ( m_matchBranches                                           &&
       m_parentList.size()       == static_cast<unsigned int>(1) &&
       m_childList.size()        == static_cast<unsigned int>(2) &&
       vtx->particles_out_size() >= 2 ) {
    return checkTwoBodyDecay( vtx );
  } //> two-body decay


  ////////////////////////////////
  /// Handle other generic cases
  ///
  m_msg << MSG::VERBOSE << "trying checkParentBranch(...)" << endmsg;
  if ( checkParentBranch( vtx ) == false ) return false;

  m_msg << MSG::VERBOSE << "trying checkChildBranch(...)" << endmsg;
  if ( checkChildBranch ( vtx ) == false ) return false;

  m_msg << MSG::VERBOSE << "McVtxFilter::isAccepted(...) => DONE" << endmsg;
  return true;
}

void McVtxFilter::dump( std::ostream& out ) const
{
  out << ">>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<" << std::endl;
  out << ">>> Parents : " << std::endl;
  for ( DataVector<const ParticleCandidateList>::const_iterator itr = m_parentList.begin();
	itr != m_parentList.end();
	++itr ) {
    out << "\nList for : " << *itr  
	<< " (" << (*itr)->size() << ")" << std::endl;
    (*itr)->dropList();
  }

  out << ">>> Children : " << std::endl;
  for ( DataVector<const ParticleCandidateList>::const_iterator itr = m_childList.begin();
	itr != m_childList.end();
	++itr ) {
    out << "\nList for : " << *itr  
	<< " (" << (*itr)->size() << ")" << std::endl;
    (*itr)->dropList();
  }
  out << ">>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<" << std::endl;

  return;
}


/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/// Set filter cut options
void McVtxFilter::setFilter( const IFilterCuts * filter )
{
  if ( filter ) {
    const McVtxFilter * vtxFilter = 
      dynamic_cast<const McVtxFilter*>(filter);
    
    if ( vtxFilter ) {
      operator=(*vtxFilter);
    } else {
      m_msg << MSG::ERROR
	    << "Can't dynamic_cast " << typeid(filter).name() 
	    << " to a McVtxFilter"
	    << endmsg;
    }
  } //> filter is a valid pointer
}

/// Set the filter from a decay pattern
void McVtxFilter::setDecayPattern( const std::string& decayPattern )
{
  // clear parent and child candidates
  m_parentList.clear();
  m_childList.clear();

  m_decayPattern = decayPattern;
  
  DecayParser parser( m_decayPattern );
  
  //std::cout << "Populate parent list" << std::endl;
  std::vector<std::vector<std::string> > parents = parser.getParents();
  for(std::vector<std::vector<std::string> >::const_iterator itr = parents.begin();
      itr != parents.end();
      ++itr ) {
    ParticleCandidateList * list = new ParticleCandidateList();
    const std::vector<std::string>::const_iterator candEnd  = itr->end();
    for( std::vector<std::string>::const_iterator candidate = itr->begin();
	 candidate != candEnd;
	 ++candidate ) {
      PDG::pidType pdgID = parser.pdgId( *candidate );
      list->push_back( pdgID );
    }
    if ( ! list->empty() ) {
      m_parentList.push_back( list );
    } else {
      delete list;
    }
  }
  //std::cout << "Populate children list" << std::endl;
  std::vector<std::vector<std::string> > children = parser.getChildren();
  for(std::vector<std::vector<std::string> >::const_iterator itr = children.begin();
      itr != children.end();
      ++itr ) {
    ParticleCandidateList * list = new ParticleCandidateList();
    const std::vector<std::string>::const_iterator candEnd  = itr->end();
    for( std::vector<std::string>::const_iterator candidate = itr->begin();
	 candidate != candEnd;
	 ++candidate ) {
      PDG::pidType pdgID = parser.pdgId( *candidate );
      list->push_back( pdgID );
    }
    if ( ! list->empty() ) {
      m_childList.push_back( list );
    } else {
      delete list;
    }
  }

  return;
}


/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

bool McVtxFilter::checkParentBranch( const HepMC::GenVertex * vtx ) const
{
  m_msg << MSG::VERBOSE << "In checkParentBranch..." << endmsg;

  /// Check we aren't in the "any particle" case
  if ( m_parentList.empty() ) {
    return true;
  }
  
  if ( m_msg.level() <= MSG::VERBOSE ) {
    vtx->print();
  }

  /// Check if number of parents is OK
  if ( static_cast<unsigned int>(vtx->particles_in_size()) < m_parentList.size() ){
    return false;
  }

  if ( m_msg.level() <= MSG::VERBOSE ) {
    m_msg << MSG::VERBOSE 
	  << "Number of list of parents : " 
	  << m_parentList.size() 
	  << endmsg;
    m_parentList.front()->dropList();
  }

  std::vector<int> parentIds;
  for ( HepMC::GenVertex::particles_in_const_iterator itrPart = vtx->particles_in_const_begin();
	itrPart != vtx->particles_in_const_end();
	++itrPart ) {
    parentIds.push_back( (*itrPart)->pdg_id() );
  }

  AnalysisUtils::Permutation<std::vector<int> > permute( &parentIds,
							 m_parentList.size() );
  std::vector<int> parents;

  bool accepted = false;
  while ( permute.get(parents) && !accepted ) {
    accepted = true;
    const unsigned int iMax = std::min( m_parentList.size(), parentIds.size() );
    for ( unsigned int i = 0; i != iMax; ++i ) {
      const bool hasInList = 
	m_parentList[i]->hasInList( static_cast<PDG::pidType>(parents[i]), 
				    m_matchSign );
      if ( !hasInList ) {
	// this permutation is not suiting, going to the next one
	// (if any)
	accepted = false;
	break;
      }
    }

    // everything went fine for this permutation !
    if ( accepted ) {
      break;
    }
  }

  m_msg << MSG::VERBOSE << ">>> CheckParentBranch is DONE : " 
	<< ( accepted ? "accept" : "reject" )
	<< " vtx= " << vtx->barcode()
	<< endmsg;
  return accepted;
}

bool McVtxFilter::checkChildBranch( const HepMC::GenVertex * vtx ) const
{
  m_msg << MSG::VERBOSE << "In checkChildBranch..." << endmsg;

  if ( m_msg.level() <= MSG::VERBOSE ) {
    vtx->print();
  }

  /// Check we aren't in the "any particle" case
  if ( m_childList.empty() ) {
    return true;
  }

  /// Check there is enough outgoing particles in the current vertex
  if ( static_cast<unsigned int>(vtx->particles_out_size()) < m_childList.size() ) return false;

  m_msg << MSG::VERBOSE << "Number of list of children : " 
	<< m_childList.size() << endmsg;

  std::vector<int> childIds;
  for ( HepMC::GenVertex::particles_out_const_iterator itrPart = vtx->particles_out_const_begin(); 
	itrPart != vtx->particles_out_const_end();
	++itrPart ) {
    childIds.push_back( (*itrPart)->pdg_id() );
  }

  AnalysisUtils::Permutation<std::vector<int> > permute( &childIds, 
							 m_childList.size() );
  std::vector<int> children;

  bool accepted = false;
  while ( permute.get(children) && !accepted ) {
    accepted = true;
    const unsigned int iMax = std::min( m_childList.size(), childIds.size() );
    for ( unsigned int i = 0; i != iMax; ++i ) {
      const bool hasInList = 
	m_childList[i]->hasInList( static_cast<PDG::pidType>(children[i]), 
				   m_matchSign );
      if ( !hasInList ) {
	// this permutation is not suiting, going to the next one
	// (if any)
	accepted = false;
	break;
      }
    }
  }

  m_msg << MSG::VERBOSE << ">>> CheckChildBranch is DONE : " 
	<< ( accepted ? "accept" : "reject" )
	<< " vtx= " << vtx->barcode()
	<< endmsg;
  return accepted;
}

bool McVtxFilter::checkTwoBodyDecay( const HepMC::GenVertex * vtx ) const
{
  m_msg << MSG::VERBOSE << "In checkTwoBodyDecay..." << endmsg;

  /// First check the parent branch matching decision
  /// if it doesn't fulfil our requirements, it is not worth
  /// continuing the process
  const bool parentTree = checkParentBranch( vtx );
  if ( parentTree == false ) {
    return false;
  }

  /// Now, handle the child branch
  m_msg << MSG::VERBOSE << ">>> Check child branch" << endmsg;

  /// Cache the 2 particle candidate lists
  const ParticleCandidateList * children1 = m_childList[0];
  const ParticleCandidateList * children2 = m_childList[1];

  /// Cache the id of the outgoing particles of the vertex being analysed
  HepMC::GenVertex::particles_out_const_iterator 
    itrPart = vtx->particles_out_const_begin();
  const PDG::pidType pdgId1 = static_cast<PDG::pidType>((*itrPart)->pdg_id());
  ++itrPart;
  const PDG::pidType pdgId2 = static_cast<PDG::pidType>((*itrPart)->pdg_id());

  /// Loop over candidates for the 1st child
  for( ParticleCandidateList::const_iterator itr1 = children1->begin();
       itr1 != children1->end();
       ++itr1 ) {
    /// Loop over candidate for the 2nd child
    for( ParticleCandidateList::const_iterator itr2 = children2->begin();
	 itr2 != children2->end();
	 ++itr2 ) {
      m_msg << MSG::VERBOSE << "Checking the pair : " 
	    << (*itr1) << "/" << (*itr2) 
	    << endmsg;

      /// If the strict match sign has been required, we check if
      /// the PDG ids are matching
      if ( m_matchSign && 
	   ( ( (*itr1) == pdgId1 && (*itr2) == pdgId2 ) ||
	     ( (*itr1) == pdgId2 && (*itr2) == pdgId1 ) ) ) {
	m_msg << MSG::VERBOSE << "Strict sign matching found !" << endmsg;
	return true;
      /// if we are in a loose sign request, we check only that the
      /// absolute values of the pdg IDs are matching
      } else if ( ( std::abs(*itr1) == std::abs(pdgId1) && 
		    std::abs(*itr2) == std::abs(pdgId2) )   ||
		  ( std::abs(*itr1) == std::abs(pdgId2) && 
		    std::abs(*itr2) == std::abs(pdgId1) ) ) {
	m_msg << MSG::VERBOSE << "Loose sign matching found !" << endmsg;
	return true;
      }
      m_msg << MSG::VERBOSE << "Checking next pair" << endmsg;
    }//> loop over 2nd child's candidates
  }//> loop over 1st child's candidates

  /// If we are here, then the vertex candidate didn't fulfil the 
  /// requirements we have setup
  m_msg << MSG::VERBOSE << ">>> CheckTwoBodyDecay is DONE." << endmsg;
  return false;
}

/////////////////////////////////////////////////////////////////// 
// Operators 
/////////////////////////////////////////////////////////////////// 

MsgStream& operator<<( MsgStream & msg, const McVtxFilter &obj )
{
  std::stringstream out;
  obj.dump( out );
  msg << out.str() << endmsg;
  return msg;
}

std::ostream& operator<<( std::ostream& out, const McVtxFilter &obj )
{
  obj.dump( out );
  return out;
}

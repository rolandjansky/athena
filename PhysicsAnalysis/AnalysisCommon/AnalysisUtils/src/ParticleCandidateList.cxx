/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// ParticleCandidateList.cxx 
// Implementation file for class ParticleCandidateList
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// AnalysisUtils includes
#include "AnalysisUtils/ParticleCandidateList.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////

/// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////
bool ParticleCandidateList::hasInList( const PDG::pidType& pdgID,
                                       const bool tightMatch ) const
{
  msg() << MSG::VERBOSE
	<< "In ParticleCandidateList::hasInList( pdgID ) : " << this
	<< endreq
	<< "Size of List = " << size() << endreq;
  if ( size() <= unsigned(0) ) {
    static const std::string error( "List size <=0 !!" );
    msg() << MSG::ERROR << error << endreq;
    throw GaudiException( error, "ParticleCandidateList", 
			  StatusCode::FAILURE );
  }

  for ( ParticleCandidateList::const_iterator itrPart = m_list.begin();
	itrPart != m_list.end();
	++itrPart ) {//> Loop over the list of intern particles
    msg() << MSG::VERBOSE
	  << "in loop over list of intern particle candidates" << endreq
	  << "\t>>>Comparing pid-s..." << endreq;
    if ( tightMatch && ( (*itrPart) == pdgID ) ) {
      msg() << MSG::VERBOSE
	    << ">>> " << pdgID << " is in list (" << (*itrPart) << ")"
	    << endreq;
      return true;
    } else if ( !tightMatch && 
		std::abs( (*itrPart) ) == std::abs( pdgID ) ) {
      return true;
    }
  }//> end loop over the list of intern particles

  return false;
}

void ParticleCandidateList::dropList() const
{
  msg() << MSG::VERBOSE
	<< "---------------------------------------------------" << endreq
	<< "Added those particles : " << endreq;
  for ( ParticleCandidateList::const_iterator itrPart = m_list.begin();
	itrPart != m_list.end();
	++itrPart ) {
    msg() << MSG::VERBOSE << "\tpdgID= " << (*itrPart) << endreq;
  }

  msg() << MSG::VERBOSE 
	<< "---------------------------------------------------" << endreq;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void ParticleCandidateList::addLeptons()
{
  m_list.push_back( PDG::e_minus   ); //>e-
  m_list.push_back( PDG::e_plus    ); //>e+
  m_list.push_back( PDG::nu_e      ); //>nu_e
  m_list.push_back( PDG::anti_nu_e ); //>nu_e_bar

  m_list.push_back( PDG::mu_minus  ); //>mu-
  m_list.push_back( PDG::mu_plus   ); //>mu+
  m_list.push_back( PDG::nu_mu     ); //>nuMu
  m_list.push_back( PDG::anti_nu_mu ); //>nuMu_bar

  m_list.push_back( PDG::tau_minus  ); //>tau-
  m_list.push_back( PDG::tau_plus   ); //>tau+
  m_list.push_back( PDG::nu_tau     ); //>nuTau
  m_list.push_back( PDG::anti_nu_tau ); //>nuTau_bar
}

void ParticleCandidateList::addLightJets()
{
  //MsgStream log( Athena::getMessageSvc(), "ParticleCandidateList" );
  //log <<MSG::DEBUG << "pushing_back into list..." << endreq;

  /// To cope with Full Reconstruction scheme :
  /// a light-jet, is a jet which could not have been tagged
  /// One has also to add PDG::null to the matching list
  m_list.push_back( PDG::null ); //> untagged jet : so taken light

  m_list.push_back( PDG::d      ); //> d
  m_list.push_back( PDG::anti_d ); //> d_bar
  m_list.push_back( PDG::u      ); //> u
  m_list.push_back( PDG::anti_u ); //> u_bar
  m_list.push_back( PDG::c      ); //> c
  m_list.push_back( PDG::anti_c ); //> c_bar
  m_list.push_back( PDG::s      ); //> s
  m_list.push_back( PDG::anti_s ); //> s_bar
}

void ParticleCandidateList::addLightQuarks()
{
  //MsgStream log( Athena::getMessageSvc(), "ParticleCandidateList" );
  //log <<MSG::DEBUG << "pushing_back into list..." << endreq;
  m_list.push_back( PDG::d      ); //> d
  m_list.push_back( PDG::anti_d ); //> d_bar
  m_list.push_back( PDG::u      ); //> u
  m_list.push_back( PDG::anti_u ); //> u_bar
  m_list.push_back( PDG::c      ); //> c
  m_list.push_back( PDG::anti_c ); //> c_bar
  m_list.push_back( PDG::s      ); //> s
  m_list.push_back( PDG::anti_s ); //> s_bar
}

void ParticleCandidateList::add( const std::string& list )
{
  msg() << MSG::VERBOSE
	<< "add( " << list << " )" << endreq;
  if ( list == "LightQuarks" )    addLightQuarks();
  else if ( list == "BQuark" )    addBQuark();
  else if ( list == "BbarQuark" ) addBbarQuark();
  else if ( list == "BQuarks" )   addBQuarks();
  else if ( list == "Bjet" ||
	    list == "BJet"      ) addBjet();
  else if ( list == "Leptons" || 
	    list == "leptons" )   addLeptons();
  else if ( list == "W+/-" )      addWBosons();
  else if ( list == "Z0" )        addZBoson();
  else {
    static const std::string error( "Unknown Candidate List Name !!" );
    msg() << MSG::ERROR << error << endreq;
    throw GaudiException( error, "ParticleCandidateList", 
			  StatusCode::FAILURE );
  }

  if ( msg().level() <= MSG::VERBOSE ) { dropList(); 
  }

}

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 
MsgStream& ParticleCandidateList::msg()
{
  static MsgStream msg(Athena::getMessageSvc(), "ParticleCandidateList");
  return msg;
}


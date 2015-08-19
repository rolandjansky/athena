/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*******************************************************

NAME:           TrigCaloCluster.cxx
PACKAGE:        Trigger/TrigEvent/TrigCaloEvent
AUTHORS:        Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:        Keep the important output variables
                from the Calorimeter LVL2 Trigger.
                Optionally seed ID Trigger and EF.
                Variables here should be RAW variables
                not corrected.
KNOWTOINHERIT:  TrigEmCluster, TrigTauCluster,
                TrigJetCluster
DATE:           October 17th, 2005

******************************************************/

#include "TrigCaloEvent/TrigCaloCluster.h"
#include <cmath>

// Constructor
TrigCaloCluster::TrigCaloCluster(float energy, float eta,
				 float phi, long roi_word) :
		m_rawEnergy ( energy ), m_rawEt ( 0. ), 
		m_rawEta ( eta ), m_rawPhi ( phi ),
		m_roiWord ( roi_word ), m_numberUsedCells ( 0 ),
		m_quality ( 0 )
{
	for ( int i = 0 ; i < MAXSIZE ; i++ )
		setRawEnergy((CaloSampling::CaloSample)i,0.0);
}

// Destructor
TrigCaloCluster::~TrigCaloCluster()
{}

// Copy constructor
TrigCaloCluster::TrigCaloCluster( const TrigCaloCluster* tcc ) :
		m_rawEnergy  ( tcc->rawEnergy() ), m_rawEt ( tcc->rawEt() ),
		m_rawEta ( tcc->rawEta() )  , m_rawPhi     ( tcc->rawPhi() ),
		m_roiWord ( tcc->RoIword() ),
		m_numberUsedCells ( tcc->nCells() ) , 
		m_quality    ( tcc->quality() )
{
	for ( int i = 0 ; i < MAXSIZE ; i++ )
		setRawEnergy((CaloSampling::CaloSample)i
			,tcc->rawEnergy((CaloSampling::CaloSample)i) );
}
TrigCaloCluster::TrigCaloCluster( const TrigCaloCluster& tcc ) :
		m_rawEnergy ( tcc.rawEnergy() ), m_rawEt ( tcc.rawEt() ),
		m_rawEta ( tcc.rawEta() ), m_rawPhi ( tcc.rawPhi() ), 
		m_roiWord ( tcc.RoIword() ),
		m_numberUsedCells ( tcc.nCells() ),
		m_quality   ( tcc.quality() )
{
	for ( int i = 0 ; i < MAXSIZE ; i++ )
		setRawEnergy((CaloSampling::CaloSample)i
			,tcc.rawEnergy((CaloSampling::CaloSample)i) );
}

// Simple debug method
void
TrigCaloCluster::print ( void ) const {

	std::cout << "m_rawEnergy : "       << m_rawEnergy         << "; ";
	std::cout << "m_rawEt : "           << m_rawEt             << "; ";
	std::cout << "m_rawEta : "          << m_rawEta            << "; ";
	std::cout << "m_rawPhi : "          << m_rawPhi            << "; ";
	std::cout << std::hex;
	std::cout << "m_roiWord : 0x"       << m_roiWord           << "; ";
	std::cout << std::dec;
	std::cout << "m_numberUsedCells : " << m_numberUsedCells   << "; ";
	std::cout << "m_quality : "         << m_quality           << "; ";
	std::cout << "m_rawEnergyS : ";
	for ( int i = 0 ; i < MAXSIZE ; i++ )
		std::cout << rawEnergy((CaloSampling::CaloSample)i) << "; ";
	std::cout << std::endl;
}

// Simple debug method using MsgStream
void
TrigCaloCluster::print ( MsgStream& log ) const {

	log <<MSG::DEBUG<<"m_rawEnergy : "       << m_rawEnergy         << "; ";
	log <<MSG::DEBUG<<"m_rawEt : "           << m_rawEt             << "; ";
	log <<MSG::DEBUG<<"m_rawEta : "          << m_rawEta            << "; ";
	log <<MSG::DEBUG<<"m_rawPhi : "          << m_rawPhi            << "; ";
	log << MSG::hex;
	log <<MSG::DEBUG<<"m_roiWord : "         << m_roiWord           << "; ";
	log << MSG::dec;
	log <<MSG::DEBUG<<"m_numberUsedCells : " << m_numberUsedCells   << "; ";
	log <<MSG::DEBUG<<"m_quality : "         << m_quality           << "; ";
	log <<MSG::DEBUG<<"m_rawEnergyS : ";
	for ( int i = 0 ; i < MAXSIZE ; i++ )
		log <<MSG::DEBUG<< rawEnergy((CaloSampling::CaloSample)i) << "; ";
	log << endreq;
}

// stream output
std::string str( const TrigCaloCluster& d ) {
	std::stringstream log;
	log <<"m_rawEnergy : "       << d.rawEnergy()       << "; ";
	log <<"m_rawEt : "           << d.rawEt()           << "; ";
	log <<"m_rawEta : "          << d.rawEta()          << "; ";
	log <<"m_rawPhi : "          << d.rawPhi()          << "; ";
	log << std::hex;
	log <<"m_roiWord : "         << d.RoIword()         << "; ";
	log << std::dec;
	log <<"m_numberUsedCells : " << d.nCells()          << "; ";
	log <<"m_quality : "         << d.quality()         << "; ";
	log <<"m_rawEnergyS : ";
	for ( int i = 0 ; i < MAXSIZE ; i++ )
		log << d.rawEnergy((CaloSampling::CaloSample)i) << "; ";
	return log.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigCaloCluster& d ) {
   m << str(d);
   return m;
}

// comparison
bool operator== (const TrigCaloCluster& a, const TrigCaloCluster& b) {
	double ep=0.001; // arbitrary , but seems to be reasonable
	if ( std::fabs( a.RoIword() - b.RoIword() )>ep ) return false;
	if ( std::fabs( a.rawEta()  - b.rawEta()  )>ep ) return false;
	if ( std::fabs( a.rawPhi()  - b.rawPhi()  )>ep ) return false;
	if ( std::fabs( a.rawEt()   - b.rawEt()   )>ep ) return false;
	if ( std::fabs( a.quality() - b.quality() )>ep ) return false;
	return true;
}

void diff( const TrigCaloCluster& a, const TrigCaloCluster& b,
           std::map< std::string, double >& varChange ) {

  double ep=0.001; // arbitrary , but seems to be reasonable
  if ( std::fabs( a.RoIword() - b.RoIword() )>ep ) {
    varChange[ "RoIword" ] =a.RoIword() - b.RoIword();
  }
  if ( std::fabs( a.rawEta()  - b.rawEta()  )>ep ) {
    varChange[ "rawEta" ] =a.rawEta()  - b.rawEta();
  }
  if ( std::fabs( a.rawPhi()  - b.rawPhi()  )>ep ) {
    varChange[ "rawPhi" ] =a.rawPhi()  - b.rawPhi();
  }
  if ( std::fabs( a.rawEt()   - b.rawEt()   )>ep ) {
    varChange[ "rawEt" ] =a.rawEt()   - b.rawEt();
  }
  if ( std::fabs( a.quality() - b.quality() )>ep ) {
    varChange[ "quality" ] =a.quality() - b.quality();
  }
  
  return;
}
// End of TrigCaloCluster

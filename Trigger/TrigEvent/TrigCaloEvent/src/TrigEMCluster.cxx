/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*******************************************************

NAME:           TrigEMCluster.cxx
PACKAGE:        Trigger/TrigEvent/TrigCaloEvent
AUTHORS:        Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:        Keep the important output variables
                from the Calorimeter LVL2 egamma Trigger.
                Optionally seed ID Trigger and EF.
                Variables here should be RAW variables
                not corrected.
INHERITFROM:	TrigCaloCluster
DATE:           October 17th, 2005

******************************************************/

#include "TrigCaloEvent/TrigEMCluster.h"
#include <cmath> 
// Constructor
TrigEMCluster::TrigEMCluster(float energy, float eta,
				 float phi, long roi_word) :
		TrigCaloCluster ( ) , m_Energy ( energy ),
		m_Et ( 0. ), m_Eta ( eta ), m_Phi ( phi ),
		m_e237( 0. ), m_e277 ( 999. ),
		m_fracs1 ( 0. ), m_weta2 ( 0. ),
		m_ehad1 ( 0. ), m_Eta1 ( 0. ),
		m_emaxs1 ( 999. ), m_e2tsts1 ( 0. ),
		m_e233( 0. ), m_wstot( 0. )
{	setRoIword ( roi_word );
	m_rings.reset();
}

// Destructor
TrigEMCluster::~TrigEMCluster()
{}

// Copy constructor (Note that also the base class is copied)
TrigEMCluster::TrigEMCluster( const TrigEMCluster* temc ) :
		TrigCaloCluster ( (TrigCaloCluster*) temc ),
		m_Energy  ( temc->energy() ),   m_Et     ( temc->et() ),
		m_Eta     ( temc->eta() ),      m_Phi    ( temc->phi() ),
		m_e237    ( temc->e237() ),     m_e277   ( temc->e277() ),
		m_fracs1  ( temc->fracs1() ),   m_weta2  ( temc->weta2() ),
		m_ehad1   ( temc->ehad1() ),    m_Eta1   ( temc->Eta1() ),
		m_emaxs1  ( temc->emaxs1() ),   m_e2tsts1( temc->e2tsts1() ),
		m_e233    ( temc->e233() ),     m_wstot   ( temc->wstot() )
{	for ( int i = 0 ; i < MAXSIZE ; i++ )
              setEnergy((CaloSampling::CaloSample)i
                    ,temc->energy((CaloSampling::CaloSample)i) );
	m_rings.reset();
}
TrigEMCluster::TrigEMCluster( const TrigEMCluster& temc ) :
		TrigCaloCluster ( (TrigCaloCluster&) temc ),
		m_Energy  ( temc.energy() ) , m_Et     ( temc.et() ),
		m_Eta    ( temc.eta() )     , m_Phi    ( temc.phi() ),
		m_e237   ( temc.e237() )    , m_e277   ( temc.e277() ),
		m_fracs1 ( temc.fracs1() )  , m_weta2  ( temc.weta2() ),
		m_ehad1  ( temc.ehad1() )   , m_Eta1   ( temc.Eta1() ),
		m_emaxs1 ( temc.emaxs1() )  , m_e2tsts1( temc.e2tsts1() ),
		m_e233   ( temc.e233() )    , m_wstot   ( temc.wstot() )
{	for ( int i = 0 ; i < MAXSIZE ; i++ )
              setEnergy((CaloSampling::CaloSample)i
                    ,temc.energy((CaloSampling::CaloSample)i) );
	m_rings.reset();
}

// Simple debug method
void
TrigEMCluster::print ( void ) const {
	TrigCaloCluster::print();
	std::cout << "m_Energy : "  << m_Energy  << "; ";
	std::cout << "m_Et : "      << m_Et      << "; ";
	std::cout << "m_Eta : "     << m_Eta     << "; ";
	std::cout << "m_Phi : "     << m_Phi     << "; ";
	std::cout << "m_e237 : "    << m_e237    << "; ";
	std::cout << "m_e277 : "    << m_e277    << "; ";
	std::cout << "m_fracs1 : "  << m_fracs1  << "; ";
	std::cout << "m_weta2 : "   << m_weta2   << "; ";
	std::cout << "m_ehad1 : "   << m_ehad1   << "; ";
	std::cout << "m_Eta1 : "    << m_Eta1    << "; ";
	std::cout << "m_emaxs1 : "  << m_emaxs1  << "; ";
	std::cout << "m_e2tsts1 : " << m_e2tsts1 << "; ";
	std::cout << "m_e233 : "    << m_e233    << "; ";
	std::cout << "m_wstot : "   << m_wstot   << "; ";
	std::cout << "m_EnergyS : ";
        for ( int i = 0 ; i < MAXSIZE ; i++ )
                std::cout << energy((CaloSampling::CaloSample)i) << "; ";
	std::cout << std::endl;
}

// Simple debug method using MsgStream
void
TrigEMCluster::print ( MsgStream& log ) const {
	TrigCaloCluster::print(log);
	log <<MSG::DEBUG<< "m_Energy : "  << m_Energy  << "; ";
	log <<MSG::DEBUG<< "m_Et : "      << m_Et      << "; ";
	log <<MSG::DEBUG<< "m_Eta : "     << m_Eta     << "; ";
	log <<MSG::DEBUG<< "m_Phi : "     << m_Phi     << "; ";
	log <<MSG::DEBUG<< "m_e237 : "    << m_e237    << "; ";
	log <<MSG::DEBUG<< "m_e277 : "    << m_e277    << "; ";
	log <<MSG::DEBUG<< "m_fracs1 : "  << m_fracs1  << "; ";
	log <<MSG::DEBUG<< "m_weta2 : "   << m_weta2   << "; ";
	log <<MSG::DEBUG<< "m_ehad1 : "   << m_ehad1   << "; ";
	log <<MSG::DEBUG<< "m_Eta1 : "    << m_Eta1    << "; ";
	log <<MSG::DEBUG<< "m_emaxs1 : "  << m_emaxs1  << "; ";
	log <<MSG::DEBUG<< "m_e2tsts1 : " << m_e2tsts1 << "; ";
	log <<MSG::DEBUG<< "m_e233 : "    << m_e233    << "; ";
	log <<MSG::DEBUG<< "m_wstot : "   << m_wstot   << "; ";
	log <<MSG::DEBUG<< "m_EnergyS : ";
        for ( int i = 0 ; i < MAXSIZE ; i++ )
                log <<MSG::DEBUG<< energy((CaloSampling::CaloSample)i) << "; ";
	log << endreq;
}

bool TrigEMCluster::isValid() const {
	return m_rings.isValid();
}

void TrigEMCluster::setRings(const RingerRingsContainer &ring_con, unsigned int ring_idx) {
	if (ring_idx < ring_con.size()) {
		m_rings.toIndexedElement(ring_con, ring_idx);
		if (!m_rings.isValid()) {
			m_rings.reset();
		}
	} else {
		m_rings.reset();
	}
}

void TrigEMCluster::setRings(const ElementLink<RingerRingsContainer>& ringsLink)
{
  m_rings = ringsLink;
}

const RingerRings *TrigEMCluster::rings() const {
	if (m_rings.isValid()) {
		return *m_rings;
	}
	return 0;
}

const ElementLink<RingerRingsContainer>& TrigEMCluster::ringsLink() const
{
  return m_rings;
}

// stream output
std::string str( const TrigEMCluster& d ) {
	std::stringstream log;
        log << str( (const TrigCaloCluster&) d ) ;
        log << "m_Energy : "  << d.energy()  << "; ";
        log << "m_Et : "      << d.et()      << "; ";
        log << "m_Eta : "     << d.eta()     << "; ";
        log << "m_Phi : "     << d.phi()     << "; ";
        log << "m_e237 : "    << d.e237()    << "; ";
        log << "m_e277 : "    << d.e277()    << "; ";
        log << "m_fracs1 : "  << d.fracs1()  << "; ";
        log << "m_weta2 : "   << d.weta2()   << "; ";
        log << "m_ehad1 : "   << d.ehad1()   << "; ";
        log << "m_Eta1 : "    << d.Eta1()    << "; ";
        log << "m_emaxs1 : "  << d.emaxs1()  << "; ";
        log << "m_e2tsts1 : " << d.e2tsts1() << "; ";
        log << "m_e233 : "    << d.e233()    << "; ";
        log << "m_wstot : "   << d.wstot()   << "; ";
        log << "m_EnergyS : ";
        for ( int i = 0 ; i < MAXSIZE ; i++ )
                log << d.energy((CaloSampling::CaloSample)i) << "; ";
	return log.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigEMCluster& d ) {
   m << str(d);
   return m;
} 

// comparison
bool operator== (const TrigEMCluster& a, const TrigEMCluster& b) {
	double ep=0.001; // arbitrary , but seems to be reasonable
	if ( (const TrigCaloCluster&) a !=  (const TrigCaloCluster&)b )
		return false;
	if ( std::fabs(a.eta()    - b.eta()    )>ep ) return false;
	if ( std::fabs(a.phi()    - b.phi()    )>ep ) return false;
	if ( std::fabs(a.energy() - b.energy() )>ep ) return false;
	return true;
}

// End of file TrigEMCluster.cxx

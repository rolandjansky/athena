/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*******************************************************

NAME:           TrigTauCluster.cxx
PACKAGE:        Trigger/TrigEvent/TrigCaloEvent
AUTHORS:        Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:        Keep the important output variables
                from the Calorimeter LVL2 Tau Trigger.
                Optionally seed ID Trigger and EF.
                Variables here should be RAW variables
                not corrected.
INHERITFROM:	TrigCaloCluster
DATE:           October 17th, 2005

******************************************************/


#include "TrigCaloEvent/TrigTauCluster.h"

TrigTauCluster::TrigTauCluster(float energy, float eta, float phi, long roi_word) :
				TrigCaloCluster(energy, eta, phi), m_valid(false)
{
    setRoIword(roi_word);
	m_EMenergy = 0;
	m_HADenergy = 0;
	m_eCalib = 0;
	m_EMRadius2 = 0;
	m_CaloRadius = 0;
	m_IsoFrac = 0;
	m_numStripCells = 0;
	m_numTotCells = 0;
	m_stripWidth = 0;
	m_stripWidthOffline = 0;
	m_EMRadius3S = 0;
	m_CoreFrac = 0;
	m_EMFrac = 0;
    m_HadRadius = 0;
	m_clusterDetails.reset();
}


TrigTauCluster::TrigTauCluster(const TrigTauClusterDetailsContainer *details_cont,
	                           unsigned int details_index, float energy,
                               float eta, float phi, long roi_word) :
	TrigCaloCluster(energy, eta, phi), m_valid(false) 
{
	setRoIword(roi_word);
	m_EMenergy = 0;
	m_HADenergy = 0;
	m_eCalib = 0;
	m_EMRadius2 = 0;
	m_CaloRadius = 0;
	m_IsoFrac = 0;
	m_numStripCells = 0;
	m_numTotCells = 0;
	m_stripWidth = 0;
	m_stripWidthOffline = 0;
	m_EMRadius3S = 0;
	m_CoreFrac = 0;
	m_EMFrac = 0;
    m_HadRadius = 0;

	/** Set ElementLink to the TrigTauClusterDetails */
	if(details_cont != NULL && details_cont->size() > details_index)
	{
		m_clusterDetails.toIndexedElement(*details_cont, details_index);
		if(!m_clusterDetails.isValid())
		{
			m_clusterDetails.reset();
			m_valid=false;
		}
	}
}

// Destructor
TrigTauCluster::~TrigTauCluster()
{
}

// Copy constructor (Note that also the base class is copied)
TrigTauCluster::TrigTauCluster(const TrigTauCluster* ttc) : TrigCaloCluster((TrigCaloCluster*)ttc),
	m_EMenergy(ttc->EMenergy()),
	m_HADenergy(ttc->HADenergy()),
	m_eCalib(ttc->eCalib()),
	m_EMRadius2(ttc->EMRadius2()),
	m_CaloRadius(ttc->CaloRadius()),
	m_IsoFrac(ttc->IsoFrac()),
	m_numStripCells(ttc->numStripCells()),
	m_numTotCells(ttc->numTotCells()),
	m_stripWidth(ttc->stripWidth()),
	m_stripWidthOffline(ttc->stripWidthOffline()),
	m_EMRadius3S(ttc->EMRadius3S()),
	m_CoreFrac(ttc->CoreFrac()),
	m_EMFrac(ttc->EMFrac()),
	m_HadRadius(ttc->HadRadius()),
        m_valid(false)
{
	//set ElementLink to cluster
	if(m_clusterDetails.isValid())
	{
		m_clusterDetails.toIndexedElement(ttc->m_clusterDetails.getStorableObjectRef(), ttc->m_clusterDetails.index());
	}
	else
	{
		m_clusterDetails.reset();
	}
}

TrigTauCluster::TrigTauCluster(const TrigTauCluster& ttc) : TrigCaloCluster((TrigCaloCluster&)ttc),
	m_EMenergy(ttc.EMenergy()),
	m_HADenergy(ttc.HADenergy()),
	m_eCalib(ttc.eCalib()),
	m_EMRadius2(ttc.EMRadius2()),
	m_CaloRadius(ttc.CaloRadius()),
	m_IsoFrac(ttc.IsoFrac()),
	m_numStripCells(ttc.numStripCells()),
	m_numTotCells(ttc.numTotCells()),
	m_stripWidth(ttc.stripWidth()),
	m_stripWidthOffline(ttc.stripWidthOffline()),
	m_EMRadius3S(ttc.EMRadius3S()),
	m_CoreFrac(ttc.CoreFrac()),
	m_EMFrac(ttc.EMFrac()),
	m_HadRadius(ttc.HadRadius()),
	m_valid(false)
{	
	//set ElementLink to cluster
	if(m_clusterDetails.isValid())
	{
		m_clusterDetails.toIndexedElement(ttc.m_clusterDetails.getStorableObjectRef(), ttc.m_clusterDetails.index());
	}
	else
	{
		m_clusterDetails.reset();
	}
}

// Simple debug method using MsgStream
void TrigTauCluster::print(void) const
{
	TrigCaloCluster::print();
	std::cout << "m_EMenergy : " << m_EMenergy << "; ";
	std::cout << "m_HADenergy : " << m_HADenergy << "; ";
	std::cout << "m_eCalib : " << m_eCalib << "; ";
	std::cout << "Eta : " << eta() << "; ";
	std::cout << "Phi : " << phi() << "; ";
	std::cout << "m_numStripCells : " << m_numStripCells << "; ";
	std::cout << "m_numTotCells : " << m_numTotCells << "; ";
/*	std::cout << "m_EMenergyNarrow: ";
        for ( int i = 0 ; i < NUMEMSAMP ; i++ )
                std::cout << trigtauDetails->EMenergyNarrow(i) << "; ";
	std::cout << std::endl;
	std::cout << "m_HADenergyNarrow: ";
        for ( int i = 0 ; i < NUMHADSAMP ; i++ )
                std::cout << trigtauDetails->HADenergyNarrow(i) << "; ";
	std::cout << std::endl; */
}

// Simple debug method using MsgStream
void TrigTauCluster::print ( MsgStream& log ) const
{
	TrigCaloCluster::print(log);
	log << MSG::DEBUG << "mEMenergy : " << m_EMenergy << "; ";
	log << MSG::DEBUG << "m_HADenergy : " << m_HADenergy << "; ";
	log << MSG::DEBUG << "m_eCalib : " << m_eCalib << "; ";
	log << MSG::DEBUG << "Eta : " << eta() << "; ";
	log << MSG::DEBUG << "Phi : " << phi() << "; ";
	log << MSG::DEBUG << "m_numStripCells : " << m_numStripCells << "; ";
	log << MSG::DEBUG << "m_numTotCells : " << m_numTotCells   << "; ";
/*	log <<MSG::DEBUG<< "m_EMenergyNarrow : ";
        for ( int i = 0 ; i < NUMEMSAMP ; i++ )
                log <<MSG::DEBUG<< trigtauDetails->EMenergyNarrow(i) << "; ";
	log << endreq;
	log <<MSG::DEBUG<< "m_HADenergyNarrow : ";
        for ( int i = 0 ; i < NUMHADSAMP ; i++ )
                log <<MSG::DEBUG<< trigtauDetails->HADenergyNarrow(i) << "; ";
	log << endreq; */
}

const TrigTauClusterDetails* TrigTauCluster::clusterDetails() const
{
	if(m_clusterDetails.isValid())
	{
		return (*m_clusterDetails);
	}
	else
	{
		return NULL;
	}
}

const ElementLink<TrigTauClusterDetailsContainer>&
TrigTauCluster::clusterDetailsLink() const
{
  return m_clusterDetails;
}

void TrigTauCluster::setClusterDetails(const ElementLink<TrigTauClusterDetailsContainer>& clusterDetailsLink)
{
  m_clusterDetails = clusterDetailsLink;
}


bool operator== (const TrigTauCluster& clus1, const TrigTauCluster& clus2 )
{
	//if(clus1.clusterDetails() != 0 && clus2.clusterDetails() == 0 ) return false;
	//if(clus1.clusterDetails() == 0 && clus2.clusterDetails() != 0 ) return false;
	if((const TrigCaloCluster&)clus1 != (const TrigCaloCluster&)clus2)
	{
		return false;
	}

	double epsilon=0.001;
	if(fabs(clus1.EMenergy() - clus2.EMenergy()) > epsilon) return false;
	if(fabs(clus1.HADenergy() - clus2.HADenergy()) > epsilon) return false;
	if(fabs(clus1.eCalib() - clus2.eCalib()) > epsilon) return false;
	if(fabs(clus1.EMRadius2() - clus2.EMRadius2()) > epsilon) return false;
	if(fabs(clus1.CaloRadius() - clus2.CaloRadius()) > epsilon) return false;
	if(fabs(clus1.IsoFrac() - clus2.IsoFrac()) > epsilon) return false;
	if(abs(clus1.numStripCells() - clus2.numStripCells()) > epsilon) return false;
	if(abs(clus1.numTotCells() - clus2.numTotCells()) > epsilon) return false;
	if(fabs(clus1.stripWidth() - clus2.stripWidth()) > epsilon) return false;
	if(fabs(clus1.stripWidthOffline() - clus2.stripWidthOffline() ) > epsilon) return false;
	if(fabs(clus1.EMRadius3S() - clus2.EMRadius3S()) > epsilon) return false;
	if(fabs(clus1.CoreFrac() - clus2.CoreFrac()) > epsilon) return false;
	if(fabs(clus1.EMFrac() - clus2.EMFrac()) > epsilon) return false;
    if(fabs(clus1.HadRadius() - clus2.HadRadius()) > epsilon) return false;

	if(clus1.clusterDetails() != 0 && clus2.clusterDetails() != 0 && clus1.clusterDetails() != clus2.clusterDetails())
	{
		return false;
	}

	return true;
}

std::string str(const TrigTauCluster& tau)
{
	std::stringstream stream;
	stream << str((const TrigCaloCluster&) tau)
           << "; EMenergy:" << tau.EMenergy()
           << "; HADenergy:" << tau.HADenergy()
           << "; eCalib:" << tau.eCalib()
           << "; EMRadius2:" << tau.EMRadius2()
           << "; CaloRadius:" << tau.CaloRadius()
           << "; IsoFrac:" << tau.IsoFrac()
           << "; numStripCells:" << tau.numStripCells()
           << "; numTotCells:" << tau.numTotCells()
           << "; stripWidth:" << tau.stripWidth()
           << "; stripWidthOffline:" << tau.stripWidthOffline()
           << "; EMRadius3S:" << tau.EMRadius3S()
           << "; CoreFrac:" << tau.CoreFrac()
           << "; EMFrac:" << tau.EMFrac()
           << "; HadRadius:" << tau.HadRadius();
	if(tau.clusterDetails() != 0)
	{
		stream << "; " << str(*tau.clusterDetails());
	}

	return stream.str();
}

MsgStream& operator<< (MsgStream& m, const TrigTauCluster& tau)
{
	return (m << str(tau));
}


void diff(const TrigTauCluster& clus1, const TrigTauCluster& clus2, std::map< std::string, double >& varChange)
{
	const double epsilon = 0.001;
	diff((const TrigCaloCluster&)clus1, (const TrigCaloCluster&)clus2, varChange);

	if(fabs(clus1.EMenergy() - clus2.EMenergy()) > epsilon)
	{
		varChange["EMEnergy"] = clus1.EMenergy() - clus2.EMenergy();
	}
	if(fabs(clus1.HADenergy() - clus2.HADenergy()) > epsilon)
	{
		varChange["HADenergy"] = clus1.HADenergy() - clus2.HADenergy();
	}
	if(fabs(clus1.eCalib() - clus2.eCalib()) > epsilon)
	{
		varChange["eCalib"] = clus1.eCalib() - clus2.eCalib();
	}
	if(fabs(clus1.EMRadius2() - clus2.EMRadius2()) > epsilon)
	{
		varChange["EMRadius2"] = clus1.EMRadius2() - clus2.EMRadius2();
	}
	if(fabs(clus1.CaloRadius() - clus2.CaloRadius()) > epsilon)
	{
		varChange["CaloRadius"] = clus1.CaloRadius() - clus2.CaloRadius();
	}
	if(fabs(clus1.IsoFrac() - clus2.IsoFrac()) > epsilon)
	{
		varChange["IsoFrac"] = clus1.IsoFrac() - clus2.IsoFrac();
	}
	if(abs(clus1.numStripCells() - clus2.numStripCells()) > epsilon)
	{
		varChange["numStripCells"] = clus1.numStripCells() - clus2.numStripCells();
	}
	if(abs(clus1.numTotCells() - clus2.numTotCells()) > epsilon)
	{
		varChange["numTotCells"] = clus1.numTotCells() - clus2.numTotCells();
	}
	if(fabs(clus1.stripWidth() - clus2.stripWidth()) > epsilon)
	{
		varChange["stripWidth"] = clus1.stripWidth() - clus2.stripWidth();
	}
	if(fabs(clus1.stripWidthOffline() - clus2.stripWidthOffline()) > epsilon)
	{
		varChange["stripWidthOffline"] = clus1.stripWidthOffline() - clus2.stripWidthOffline();
	}
	if(fabs(clus1.EMRadius3S() - clus2.EMRadius3S()) > epsilon)
	{
		varChange["EMRadius3S"] = clus1.EMRadius3S() - clus2.EMRadius3S();
	}
	if(fabs(clus1.CoreFrac() - clus2.CoreFrac()) > epsilon)
	{
		varChange["CoreFrac"] = clus1.CoreFrac() - clus2.CoreFrac();
	}
	if(fabs(clus1.EMFrac() - clus2.EMFrac()) > epsilon)
	{
		varChange["EMFrac"] = clus1.EMFrac() - clus2.EMFrac();
	}
	if(fabs(clus1.HadRadius() - clus2.HadRadius()) > epsilon)
	{
		varChange["HadRadius"] = clus1.HadRadius() - clus2.HadRadius();
	}

	if(clus1.clusterDetails() != 0 && clus2.clusterDetails() == 0)
	{
		varChange["clusterDetails "] = 1;
	}
	else if(clus1.clusterDetails() == 0 && clus2.clusterDetails() != 0)
	{
		varChange["clusterDetails "] = -1;
	}
	else if(clus1.clusterDetails() != 0 && clus2.clusterDetails() != 0 && clus1.clusterDetails() != clus2.clusterDetails())
	{
		diff(clus1.clusterDetails(), clus2.clusterDetails(), varChange);
	}
	return;
}
// End of file TrigTauCluster.cxx

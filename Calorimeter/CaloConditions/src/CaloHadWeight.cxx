/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloConditions/CaloHadWeight.h"


//** Default Constructor
CaloHadWeight::CaloHadWeight()
  : m_nBinEta(0),
    m_nBinSampling(0),
    m_nBinLogEClus(0),
    m_nBinLogEDensCell(0),
    m_MinEta(0),
    m_MinSampling(0),
    m_MinLogEClus(0),
    m_MinLogEDensCell(0),
    m_MaxEta(0),
    m_MaxSampling(0),
    m_MaxLogEClus(0),
    m_MaxLogEDensCell(0)
{
}


CaloHadWeight::CaloHadWeight(
			     const int   & nBinEta, 
			     const float & MinEta, 
			     const float & MaxEta, 
			     const int   & nBinSampling, 
			     const int & MinSampling,
			     const int & MaxSampling,
			     const int   & nBinLogEClus,
			     const float & MinLogEClus,
			     const float & MaxLogEClus,
			     const int   & nBinLogEDensCell,
			     const float & MinLogEDensCell,
			     const float & MaxLogEDensCell  
			     ) : m_nBinEta  	    (nBinEta), 	    
				 m_nBinSampling     (nBinSampling),    
				 m_nBinLogEClus     (nBinLogEClus),    
				 m_nBinLogEDensCell (nBinLogEDensCell),
				 m_MinEta  	    (MinEta), 	    
				 m_MinSampling 	    (MinSampling),	    
				 m_MinLogEClus 	    (MinLogEClus),	    
				 m_MinLogEDensCell  (MinLogEDensCell), 
				 m_MaxEta  	    (MaxEta), 	    
				 m_MaxSampling 	    (MaxSampling),	    
				 m_MaxLogEClus 	    (MaxLogEClus),	    
				 m_MaxLogEDensCell  (MaxLogEDensCell) 
{
  m_indexEtaSampling.resize(m_nBinEta*m_nBinSampling,-1);
} 

const struct CaloHadWeight::HadData * CaloHadWeight::getHadData(const float & eta, const int & sampling, const float & logEClus, const float & logEDensCell) const
{
  int iBin = getBin(eta,sampling,logEClus,logEDensCell);
  if ( iBin > -1 && iBin < (int)m_data.size() ) 
    return (& m_data[iBin]);
  else
    return nullptr;
}


  // set methods

void CaloHadWeight::setHadData(const float & eta, const int & sampling, const float & logEClus, const float & logEDensCell, const struct HadData & theData) {

  const int iEtaSampling = getEtaSamplingBin(eta, sampling);
  if ( iEtaSampling > -1 ) {
    if ( m_indexEtaSampling[iEtaSampling] == -1 ) {
      const int iFilledSamplings = m_data.size()/(m_nBinLogEClus*m_nBinLogEDensCell);
      m_indexEtaSampling[iEtaSampling] = iFilledSamplings;
      m_data.resize((iFilledSamplings+1)*(m_nBinLogEClus*m_nBinLogEDensCell));
    }
    const int iBin = getBin(eta,sampling,logEClus,logEDensCell);
    if ( iBin > -1 ) {
      m_data[iBin] = theData;
    }
  }
}


int CaloHadWeight::getEtaSamplingBin(const float & eta, const int & sampling) const
{
  int iEta,iSampling;

  iEta = (int)((eta-m_MinEta)/(m_MaxEta-m_MinEta)*m_nBinEta);
  if ( iEta >= 0 && iEta < m_nBinEta ) {
    iSampling = sampling-m_MinSampling;
    if ( iSampling >= 0 && iSampling < m_nBinSampling ) {
      return (iEta*m_nBinSampling+iSampling);
    }
  }
  return (-1);
}

int CaloHadWeight::getBin(const float & eta, const int & sampling, const float & logEClus, const float & logEDensCell) const
{
  int iLogEClus,iLogEDensCell;

  const int iEtaSampling = getEtaSamplingBin(eta, sampling);
  if ( iEtaSampling > -1 && m_indexEtaSampling[iEtaSampling] > -1 ) {
    iLogEClus = (int)((logEClus-m_MinLogEClus)/(m_MaxLogEClus-m_MinLogEClus)*m_nBinLogEClus);
    if ( iLogEClus >= 0 && iLogEClus < m_nBinLogEClus ) {
      iLogEDensCell = (int)((logEDensCell-m_MinLogEDensCell)/(m_MaxLogEDensCell-m_MinLogEDensCell)*m_nBinLogEDensCell);
      if ( iLogEDensCell >= 0 && iLogEDensCell < m_nBinLogEDensCell ) {
	return (iLogEDensCell + m_nBinLogEDensCell * 
		(iLogEClus + m_nBinLogEClus * 
		 m_indexEtaSampling[iEtaSampling]));
      }
    }
  }
  return -1;
}



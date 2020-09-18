/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloEMFrac.cxx,v 1.2 2007-10-26 01:47:23 ssnyder Exp $
/**
 * @file CaloConditions/src/CaloEMFrac.cxx
 * @brief Hold binned EM fraction data.
 */


#include "CaloConditions/CaloEMFrac.h"


/**
 * @brief Default Constructor.  (Only intended for use by persistency.)
 */
CaloEMFrac::CaloEMFrac()
  : m_nBinEta(0),
    m_nBinLogEClus(0),
    m_nBinLogEDens(0),
    m_nBinLogDepth(0),
    m_MinEta(0),
    m_MinLogEClus(0),
    m_MinLogEDens(0),
    m_MinLogDepth(0),
    m_MaxEta(0),
    m_MaxLogEClus(0),
    m_MaxLogEDens(0),
    m_MaxLogDepth(0)
{
}


/**
 * @brief Constructor.
 * @param nBinEta      Number of bins in eta.
 * @param MinEta       Minimum eta for the first bin.
 * @param MaxEta       Maximum eta for the last bin.
 * @param nBinLogEClus Number of bins in log10(cluster energy).
 * @param MinLogEClus  Minimum log10(cluster energy) for the first bin.
 * @param MaxLogEClus  Maximum log10(cluster energy) for the last bin.
 * @param nBinLogEDens Number of bins in log10(cluster energy density).
 * @param MinLogEDens  Minimum log10(cluster energy density) for the first bin.
 * @param MaxLogEDens  Maximum log10(cluster energy density) for the last bin.
 * @param nBinLogDepth Number of bins in log10(cluster depth).
 * @param MinLogDepth  Minimum log10(cluster depth) for the first bin.
 * @param MaxLogDepth  Maximum log10(cluster depth) for the last bin.
 */
CaloEMFrac::CaloEMFrac(
			int   nBinEta, 	   
			float MinEta, 	   
			float MaxEta, 	   
			int   nBinLogEClus,
			float MinLogEClus, 
			float MaxLogEClus, 
			int   nBinLogEDens,
			float MinLogEDens, 
			float MaxLogEDens, 
			int   nBinLogDepth,
			float MinLogDepth, 
			float MaxLogDepth  
			) : m_nBinEta  	    (nBinEta), 	    
			    m_nBinLogEClus  (nBinLogEClus),    
			    m_nBinLogEDens  (nBinLogEDens),
			    m_nBinLogDepth  (nBinLogDepth),
			    m_MinEta  	    (MinEta), 	    
			    m_MinLogEClus   (MinLogEClus),	    
			    m_MinLogEDens   (MinLogEDens), 
			    m_MinLogDepth   (MinLogDepth), 
			    m_MaxEta  	    (MaxEta), 	    
			    m_MaxLogEClus   (MaxLogEClus),	    
			    m_MaxLogEDens   (MaxLogEDens), 
			    m_MaxLogDepth   (MaxLogDepth) 
{
} 


/**
 * @brief Return EM fraction data for one bin.
 * @param eta Cluster eta.
 * @param logEClus log10(cluster energy)
 * @param logEDens log10(cluster energy density) (possibly normalized)
 * @param logDepth log10(cluster depth)
 * @returns Pointer to the bin data, or 0 if out of range.
 *
 *  The pointer may be invalidated by a subsequent setEMFracData call.
 */
const struct CaloEMFrac::EMFracData *
CaloEMFrac::getEMFracData(float eta,
                          float logEClus,
                          float logEDens,
                          float logDepth) const
{
  int iBin = getBin(eta,logEClus,logEDens,logDepth);
  if ( iBin > -1 && iBin < (int)m_data.size() ) 
    return (& m_data[iBin]);
  else
    return nullptr;
}



/**
 * @brief Set EM fraction data for one bin.
 * @param eta Cluster eta.
 * @param logEClus log10(cluster energy)
 * @param logEDens log10(cluster energy density) (possibly normalized)
 * @param logDepth log10(cluster depth)
 * @param theData The bin data.
 *
 * This will overwrite any earlier data for the same bin.
 * If the coordinates are out of range, this is a no-op.
 */
void CaloEMFrac::setEMFracData(float eta,
                               float logEClus,
                               float logEDens,
                               float logDepth,
                               const struct EMFracData & theData)
{
  const int iEtaLogEClus = getEtaLogEClusBin(eta, logEClus);
  if ( iEtaLogEClus > -1 ) {
    const int iFilledEntries = m_data.size()/(m_nBinLogEDens*m_nBinLogDepth);
    if ( iEtaLogEClus >= iFilledEntries ) 
      m_data.resize((iFilledEntries+1)*(m_nBinLogEDens*m_nBinLogDepth));
    const int iBin = getBin(eta,logEClus,logEDens,logDepth);
    if ( iBin > -1 ) {
      m_data[iBin] = theData;
    }
  }
}


/**
 * @brief Find the eta/logEClus bin number.
 * @param eta Cluster eta.
 * @param logEClus log10(cluster energy)
 * @returns Bin number in the eta/logEClus subspace, or -1 if out of range.
 */
int CaloEMFrac::getEtaLogEClusBin(float eta, float logEClus) const
{
  int iEta,iLogEClus;

  iEta = (int)((eta-m_MinEta)/(m_MaxEta-m_MinEta)*m_nBinEta);
  if ( iEta >= 0 && iEta < m_nBinEta ) {
    iLogEClus = (int)((logEClus-m_MinLogEClus)/(m_MaxLogEClus-m_MinLogEClus)*m_nBinLogEClus);
    if ( iLogEClus >= 0 && iLogEClus < m_nBinLogEClus ) {
      return (iEta*m_nBinLogEClus+iLogEClus);
    }
  }
  return (-1);

}


/**
 * @brief Find a bin number.
 * @param eta Cluster eta.
 * @param logEClus log10(cluster energy)
 * @param logEDens log10(cluster energy density) (possibly normalized)
 * @param logDepth log10(cluster depth)
 * @returns The bin number, or -1 if out of range.
 */
int CaloEMFrac::getBin(float eta,
                       float logEClus,
                       float logEDens,
                       float logDepth) const
{
  int iLogEDens,iLogDepth;

  const int iEtaLogEClus = getEtaLogEClusBin(eta, logEClus);
  if ( iEtaLogEClus > -1 ) {
    iLogEDens = (int)((logEDens-m_MinLogEDens)/(m_MaxLogEDens-m_MinLogEDens)*m_nBinLogEDens);
    if ( iLogEDens >= 0 && iLogEDens < m_nBinLogEDens ) {
      iLogDepth = (int)((logDepth-m_MinLogDepth)/(m_MaxLogDepth-m_MinLogDepth)*m_nBinLogDepth);
      if ( iLogDepth >= 0 && iLogDepth < m_nBinLogDepth ) {
	return (iLogDepth + m_nBinLogDepth * 
		(iLogEDens + m_nBinLogEDens * 
		 iEtaLogEClus));
      }
    }
  }
  return -1;
}



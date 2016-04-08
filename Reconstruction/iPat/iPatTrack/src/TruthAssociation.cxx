/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 stores the Truth information associated with a reconstructed Track

 ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TruthAssociation.cxx,v 1.7 2004-07-16 22:22:14 pop Exp $
//<version>	$Name: not supported by cvs2svn $

#include <iomanip>
#include <iostream>
#include "iPatTrack/TruthAssociation.h"

TruthAssociation::~TruthAssociation (void)
{}

void
TruthAssociation::print (void) const
{
    std::cout << "  TruthAssociation : total :   unique   spoilt   wrong    for barcode "
	      << m_barcode << "   kine " << m_kine << std::setiosflags(std::ios::fixed) 
	      << std::endl;
    std::cout << "        b_layer : " << std::setw(8) 
	      << m_uniqueB_LayerClusters + m_spoiltB_LayerClusters + m_wrongB_LayerClusters
	      << " :" << std::setw(9) << m_uniqueB_LayerClusters
	      << std::setw(9) << m_spoiltB_LayerClusters
	      << std::setw(8) << m_wrongB_LayerClusters << std::endl;
    std::cout << "   outer_pixels : " << std::setw(8) 
	      << m_uniqueOuterPixelClusters + m_spoiltOuterPixelClusters + m_wrongOuterPixelClusters
	      << " :" << std::setw(9) << m_uniqueOuterPixelClusters
	      << std::setw(9) << m_spoiltOuterPixelClusters
	      << std::setw(8) << m_wrongOuterPixelClusters << std::endl;
    std::cout << "         strips : " << std::setw(8) 
	      << m_uniqueStripClusters + m_spoiltStripClusters + m_wrongStripClusters
	      << " :" << std::setw(9) << m_uniqueStripClusters
	      << std::setw(9) << m_spoiltStripClusters
	      << std::setw(8) << m_wrongStripClusters << std::endl;
    std::cout << "          drift : " << std::setw(8)
	      << m_uniqueDriftHits + m_spoiltDriftHits + m_wrongDriftHits
	      << " :" << std::setw(9) << m_uniqueDriftHits
	      << std::setw(9) << m_spoiltDriftHits
	      << std::setw(8) << m_wrongDriftHits << std::endl;
}

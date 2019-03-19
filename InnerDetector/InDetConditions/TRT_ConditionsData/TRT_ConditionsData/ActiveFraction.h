/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTIVEFRACTION_H
#define ACTIVEFRACTION_H
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#ifndef M_PI
#define M_PI  3.141592653589793238462643383279502884197
#endif
namespace TRTCond{
  // eta,phi binned map of the active straw fraction
  class ActiveFraction{
  public:
    ActiveFraction();
    virtual ~ActiveFraction(){};
    int findEtaBin( float eta ) const;
    int findPhiBin( float phi ) const;
    float getActiveFraction( float eta, float phi ) const;
    std::vector<std::pair<float,float>> getEtaBins( ) const;
    std::vector<std::pair<float,float>> getPhiBins( ) const;
    void  setActiveFraction( unsigned int etaBin, unsigned int phiBin, float value);
 
  private:
    int m_nBinsPhi;
    std::vector<std::pair<float,float> > m_etaBins;
    std::vector<std::pair<float,float> > m_phiBins;
    std::vector<std::vector<float> > m_activeFracTable; // [etaBin,phiBin]
  };
  inline ActiveFraction::ActiveFraction() {
     m_nBinsPhi=96;
     m_etaBins.push_back( std::make_pair(-2.1,-1.75) );
     m_etaBins.push_back( std::make_pair(-1.75,-1.3) );
     m_etaBins.push_back( std::make_pair(-1.3,-1.07) );
     m_etaBins.push_back( std::make_pair(-1.07,-0.65) );
     m_etaBins.push_back( std::make_pair(-0.65,-0.1) );
     m_etaBins.push_back( std::make_pair(-0.1,0.) );
     m_etaBins.push_back( std::make_pair(0.,0.1) );
     m_etaBins.push_back( std::make_pair(0.1,0.65) );
     m_etaBins.push_back( std::make_pair(0.65,1.07) );
     m_etaBins.push_back( std::make_pair(1.07,1.3) );
     m_etaBins.push_back( std::make_pair(1.3,1.75) );
     m_etaBins.push_back( std::make_pair(1.75,2.1) );
     float phiEdgeLow = -1. * M_PI;
     float deltaPhi = 2. * M_PI / (1. * m_nBinsPhi) ;
     for ( int i = 0; i < m_nBinsPhi; ++i ) {
       m_phiBins.push_back( std::make_pair( phiEdgeLow + i*deltaPhi, phiEdgeLow + (i+1)*deltaPhi ) );
     }
     // Initialize the table with 1.'s
     std::vector<float> dummyPhiVec( m_phiBins.size(), 1. );
     std::vector<std::vector<float> > dummyTable( m_etaBins.size(), dummyPhiVec );
     m_activeFracTable = dummyTable;
  }

  inline int ActiveFraction::findEtaBin( float eta) const {
     int etaBin = 0;
     for ( ; etaBin < (int)m_etaBins.size(); ++etaBin ) {
        std::pair<float,float> theBin = m_etaBins.at(etaBin);
        if ( eta > theBin.first && eta <= theBin.second ) break;
     }
     if ( etaBin == (int)m_etaBins.size() ) return -1;

     return etaBin;
  }

  inline int ActiveFraction::findPhiBin( float phi) const {
     int phiBin = 0;
     for ( ; phiBin < (int)m_phiBins.size(); ++phiBin ) {
	std::pair<float,float> theBin = m_phiBins.at(phiBin);
	if ( phi > theBin.first && phi <= theBin.second ) break;
     }
     if ( phiBin == (int)m_phiBins.size() ) return-1;
     return phiBin;
  }

  inline float ActiveFraction::getActiveFraction( float eta, float phi ) const {

 
     int etaBin = findEtaBin(eta);
     if ( etaBin < 0 ) return 1.;
     int phiBin = findPhiBin(phi);
     if ( phiBin < 0 ) return 1.;
     return m_activeFracTable[etaBin][phiBin];
  }
  inline void ActiveFraction::setActiveFraction( unsigned int etaBin, unsigned int phiBin, float value) {

     m_activeFracTable[etaBin][phiBin] = value;
  }
  
  inline  std::vector<std::pair<float,float>> ActiveFraction::getEtaBins( ) const {
    return m_etaBins;
  }

  inline  std::vector<std::pair<float,float>> ActiveFraction::getPhiBins( ) const {
    return m_phiBins;
  }

}

CLASS_DEF(TRTCond::ActiveFraction,69272917,1)
CONDCONT_DEF(TRTCond::ActiveFraction,85974973);
#endif

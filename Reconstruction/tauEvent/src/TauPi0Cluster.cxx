/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "tauEvent/TauPi0Cluster.h"
#include "tauEvent/TauDetails.h"

namespace Analysis {

  // default constructor
  TauPi0Cluster::TauPi0Cluster() :
    m_DELTA_PHI(TauDetails::DEFAULT),
    m_DELTA_THETA(TauDetails::DEFAULT),
    m_ENG_FRAC_EM(TauDetails::DEFAULT),
    m_ENG_FRAC_MAX(TauDetails::DEFAULT),
    m_ENG_FRAC_CORE(TauDetails::DEFAULT),
    m_SECOND_ENG_DENS(TauDetails::DEFAULT),
    m_LATERAL(TauDetails::DEFAULT),
    m_LONGITUDINAL(TauDetails::DEFAULT),
    m_EM1CoreFrac(TauDetails::DEFAULT),
    m_asymmetryInEM1WRTTrk(TauDetails::DEFAULT),
    m_NHitsInEM1(TauDetails::DEFAULT_INT),
    m_NPosECells_PS(TauDetails::DEFAULT_INT),
    m_NPosECells_EM1(TauDetails::DEFAULT_INT),
    m_NPosECells_EM2(TauDetails::DEFAULT_INT),
    m_firstEtaWRTClusterPosition_EM1(TauDetails::DEFAULT),
    m_firstEtaWRTClusterPosition_EM2(TauDetails::DEFAULT),
    m_secondEtaWRTClusterPosition_EM1(TauDetails::DEFAULT),
    m_secondEtaWRTClusterPosition_EM2(TauDetails::DEFAULT),
    m_BDTScore(TauDetails::DEFAULT)
  {
  }

  // copy constructor
  TauPi0Cluster::TauPi0Cluster(const TauPi0Cluster& rhs ) :
     m_cluster(rhs.m_cluster),
     m_tauShotVector(rhs.m_tauShotVector),
     m_DELTA_PHI(rhs.m_DELTA_PHI),
     m_DELTA_THETA(rhs.m_DELTA_THETA),
     m_ENG_FRAC_EM(rhs.m_ENG_FRAC_EM),
     m_ENG_FRAC_MAX(rhs.m_ENG_FRAC_MAX),
     m_ENG_FRAC_CORE(rhs.m_ENG_FRAC_CORE),
     m_SECOND_ENG_DENS(rhs.m_SECOND_ENG_DENS),
     m_LATERAL(rhs.m_LATERAL),
     m_LONGITUDINAL(rhs.m_LONGITUDINAL),
     m_EM1CoreFrac(rhs.m_EM1CoreFrac),
     m_asymmetryInEM1WRTTrk(rhs.m_asymmetryInEM1WRTTrk),
     m_NHitsInEM1(rhs.m_NHitsInEM1),
     m_NPosECells_PS(rhs.m_NPosECells_PS),
     m_NPosECells_EM1(rhs.m_NPosECells_EM1),
     m_NPosECells_EM2(rhs.m_NPosECells_EM2),
     m_firstEtaWRTClusterPosition_EM1(rhs.m_firstEtaWRTClusterPosition_EM1),
     m_firstEtaWRTClusterPosition_EM2(rhs.m_firstEtaWRTClusterPosition_EM2),
     m_secondEtaWRTClusterPosition_EM1(rhs.m_secondEtaWRTClusterPosition_EM1),
     m_secondEtaWRTClusterPosition_EM2(rhs.m_secondEtaWRTClusterPosition_EM2),
     m_BDTScore(rhs.m_BDTScore)
  {
  }

  // Assignment operator
  TauPi0Cluster &TauPi0Cluster::operator= ( const TauPi0Cluster& rhs )
  {
    if (this!=&rhs) {
      m_cluster = rhs.m_cluster;
      m_tauShotVector = rhs.m_tauShotVector;
      m_DELTA_PHI = rhs.m_DELTA_PHI;
      m_DELTA_THETA = rhs.m_DELTA_THETA;
      m_ENG_FRAC_EM = rhs.m_ENG_FRAC_EM;
      m_ENG_FRAC_MAX = rhs.m_ENG_FRAC_MAX;
      m_ENG_FRAC_CORE = rhs.m_ENG_FRAC_CORE;
      m_SECOND_ENG_DENS = rhs.m_SECOND_ENG_DENS;
      m_LATERAL = rhs.m_LATERAL;
      m_LONGITUDINAL = rhs.m_LONGITUDINAL;
      m_EM1CoreFrac = rhs.m_EM1CoreFrac;
      m_asymmetryInEM1WRTTrk = rhs.m_asymmetryInEM1WRTTrk;
      m_NHitsInEM1 = rhs.m_NHitsInEM1;
      m_NPosECells_PS = rhs.m_NPosECells_PS;
      m_NPosECells_EM1 = rhs.m_NPosECells_EM1;
      m_NPosECells_EM2 = rhs.m_NPosECells_EM2;
      m_firstEtaWRTClusterPosition_EM1 = rhs.m_firstEtaWRTClusterPosition_EM1;
      m_firstEtaWRTClusterPosition_EM2 = rhs.m_firstEtaWRTClusterPosition_EM2;
      m_secondEtaWRTClusterPosition_EM1 = rhs.m_secondEtaWRTClusterPosition_EM1;
      m_secondEtaWRTClusterPosition_EM2 = rhs.m_secondEtaWRTClusterPosition_EM2;
      m_BDTScore = rhs.m_BDTScore;
    }
    return *this;
  }

  bool TauPi0Cluster::operator==( const TauPi0Cluster& rhs ) const{
    if(m_cluster != rhs.m_cluster) return false;
    if(m_tauShotVector.size() != rhs.m_tauShotVector.size()) return false;
    for(unsigned iShot=0; iShot!=m_tauShotVector.size();++iShot){
        if(!(*(m_tauShotVector.at(iShot))==*(rhs.m_tauShotVector.at(iShot)))) return false;
    }
    if(m_DELTA_PHI != rhs.m_DELTA_PHI) return false;
    if(m_DELTA_THETA != rhs.m_DELTA_THETA) return false;
    if(m_ENG_FRAC_EM != rhs.m_ENG_FRAC_EM) return false;
    if(m_ENG_FRAC_MAX != rhs.m_ENG_FRAC_MAX) return false;
    if(m_ENG_FRAC_CORE != rhs.m_ENG_FRAC_CORE) return false;
    if(m_SECOND_ENG_DENS != rhs.m_SECOND_ENG_DENS) return false;
    if(m_LATERAL != rhs.m_LATERAL) return false;
    if(m_LONGITUDINAL != rhs.m_LONGITUDINAL) return false;
    if(m_EM1CoreFrac != rhs.m_EM1CoreFrac) return false;
    if(m_asymmetryInEM1WRTTrk != rhs.m_asymmetryInEM1WRTTrk) return false;
    if(m_NHitsInEM1 != rhs.m_NHitsInEM1) return false;
    if(m_NPosECells_PS != rhs.m_NPosECells_PS) return false;
    if(m_NPosECells_EM1 != rhs.m_NPosECells_EM1) return false;
    if(m_NPosECells_EM2 != rhs.m_NPosECells_EM2) return false;
    if(m_firstEtaWRTClusterPosition_EM1 != rhs.m_firstEtaWRTClusterPosition_EM1) return false;
    if(m_firstEtaWRTClusterPosition_EM2 != rhs.m_firstEtaWRTClusterPosition_EM2) return false;
    if(m_secondEtaWRTClusterPosition_EM1 != rhs.m_secondEtaWRTClusterPosition_EM1) return false;
    if(m_secondEtaWRTClusterPosition_EM2 != rhs.m_secondEtaWRTClusterPosition_EM2) return false;
    if(m_BDTScore != rhs.m_BDTScore) return false;
    return true;
  }

  bool TauPi0Cluster::operator!=( const TauPi0Cluster& rhs ) const{
    if(m_cluster != rhs.m_cluster) return true;
    if(m_tauShotVector.size() != rhs.m_tauShotVector.size()) return true;
    for(unsigned iShot=0; iShot!=m_tauShotVector.size();++iShot){
        if(!(*(m_tauShotVector.at(iShot))==*(rhs.m_tauShotVector.at(iShot)))) return true;
    }
    if(m_DELTA_PHI != rhs.m_DELTA_PHI) return true;
    if(m_DELTA_THETA != rhs.m_DELTA_THETA) return true;
    if(m_ENG_FRAC_EM != rhs.m_ENG_FRAC_EM) return true;
    if(m_ENG_FRAC_MAX != rhs.m_ENG_FRAC_MAX) return true;
    if(m_ENG_FRAC_CORE != rhs.m_ENG_FRAC_CORE) return true;
    if(m_SECOND_ENG_DENS != rhs.m_SECOND_ENG_DENS) return true;
    if(m_LATERAL != rhs.m_LATERAL) return true;
    if(m_LONGITUDINAL != rhs.m_LONGITUDINAL) return true;
    if(m_EM1CoreFrac != rhs.m_EM1CoreFrac) return true;
    if(m_asymmetryInEM1WRTTrk != rhs.m_asymmetryInEM1WRTTrk) return true;
    if(m_NHitsInEM1 != rhs.m_NHitsInEM1) return true;
    if(m_NPosECells_PS != rhs.m_NPosECells_PS) return true;
    if(m_NPosECells_EM1 != rhs.m_NPosECells_EM1) return true;
    if(m_NPosECells_EM2 != rhs.m_NPosECells_EM2) return true;
    if(m_firstEtaWRTClusterPosition_EM1 != rhs.m_firstEtaWRTClusterPosition_EM1) return true;
    if(m_firstEtaWRTClusterPosition_EM2 != rhs.m_firstEtaWRTClusterPosition_EM2) return true;
    if(m_secondEtaWRTClusterPosition_EM1 != rhs.m_secondEtaWRTClusterPosition_EM1) return true;
    if(m_secondEtaWRTClusterPosition_EM2 != rhs.m_secondEtaWRTClusterPosition_EM2) return true;
    if(m_BDTScore != rhs.m_BDTScore) return true;
    return false; 
  }

  // destructor
  TauPi0Cluster::~TauPi0Cluster()
  {
  }

  // just for testing.
  void TauPi0Cluster::dump() 
  {
    int oldpr = std::cout.precision(5); // store and change precision
    std::cout << "in TauPi0Cluster::dump()"   << std::endl;
    std::cout << "-------------------------"  << std::endl;
    std::cout << "m_DELTA_PHI = "             << m_DELTA_PHI << std::endl;
    std::cout << "m_DELTA_THETA = "           << m_DELTA_THETA << std::endl;
    std::cout << "m_ENG_FRAC_EM = "           << m_ENG_FRAC_EM << std::endl;
    std::cout << "m_ENG_FRAC_MAX = "          << m_ENG_FRAC_MAX << std::endl;
    std::cout << "m_ENG_FRAC_CORE = "         << m_ENG_FRAC_CORE << std::endl;
    std::cout << "m_SECOND_ENG_DENS = "       << m_SECOND_ENG_DENS << std::endl;
    if (m_SECOND_ENG_DENS>0)
      std::cout << "log(m_SECOND_ENG_DENS) = " << log(m_SECOND_ENG_DENS) << std::endl;
    std::cout << "m_LATERAL = "               << m_LATERAL << std::endl;
    std::cout << "m_LONGITUDINAL = "          << m_LONGITUDINAL << std::endl;
    std::cout << "m_EM1CoreFrac = "           << m_EM1CoreFrac << std::endl;
    std::cout << "m_asymmetryInEM1WRTTrk = "  << m_asymmetryInEM1WRTTrk << std::endl;
    std::cout << "m_NHitsInEM1 = "            << m_NHitsInEM1 << std::endl;
    std::cout << "m_NPosECells_PS = "         << m_NPosECells_PS << std::endl;
    std::cout << "m_NPosECells_EM1 = "        << m_NPosECells_EM1 << std::endl;
    std::cout << "m_NPosECells_EM2 = "        << m_NPosECells_EM2 << std::endl;
    std::cout << "m_firstEtaWRTClusterPosition_EM1 = "  << m_firstEtaWRTClusterPosition_EM1 << std::endl;
    std::cout << "m_firstEtaWRTClusterPosition_EM2 = "  << m_firstEtaWRTClusterPosition_EM2 << std::endl;
    std::cout << "m_secondEtaWRTClusterPosition_EM1 = " << m_secondEtaWRTClusterPosition_EM1 << std::endl;
    std::cout << "m_secondEtaWRTClusterPosition_EM2 = " << m_secondEtaWRTClusterPosition_EM2 << std::endl;
    std::cout << "m_BDTScore = " << m_BDTScore << std::endl;
    if (m_cluster.isValid()) {
        std::cout << "this->cluster()->e() = "   << this->cluster()->e()   << std::endl;
        std::cout << "this->cluster()->pt() = "  << this->cluster()->pt()  << std::endl;
        std::cout << "this->cluster()->eta() = " << this->cluster()->eta() << std::endl;
        std::cout << "this->cluster()->phi() = " << this->cluster()->phi() << std::endl;
    }
    std::cout << std::endl <<std::endl;
    std::cout << "-------------------------" <<std::endl;
    std::cout.precision(oldpr); //restore previous precision
  }
}

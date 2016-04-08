/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%446626F5033F.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%446626F5033F.CLHEP::cm

//## begin module%446626F5033F.cp preserve=no
//## end module%446626F5033F.cp

//## Module: EMECDetDescr%446626F5033F; Pseudo Package body
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/src/EMECDetDescr.cxx

//## begin module%446626F5033F.additionalIncludes preserve=no
//## end module%446626F5033F.additionalIncludes

//## begin module%446626F5033F.includes preserve=yes
#include "LArReadoutGeometry/EMECDetectorManager.h"
//## end module%446626F5033F.includes

// EMECDetDescr
#include "LArReadoutGeometry/EMECDetDescr.h"
//## begin module%446626F5033F.additionalDeclarations preserve=yes
/** 
 * @brief An internal class which is none of your business
 **/

struct Boundary {
  double min;
  double max;
  unsigned int nDiv;
} etaBoundariesEndcap[4][2][6] = {{{{1.500, 1.800, 12},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0}},    // presampler outer
				   {{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0}}},   // presampler inner
				  {{{1.375, 1.425,  1},{1.425, 1.500,  3},{1.500, 1.800, 96},{1.800, 2.000, 48},{2.000, 2.400, 64},{2.400, 2.500,  4}},    // sample 1 outer 
				   {{2.500, 3.200,  7},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0}}},   // sample 1 inner
				  {{{1.375, 1.425,  1},{1.425, 2.5,   43},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0}},    // sample 2 outer
				   {{2.500, 3.200,  7},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0}}},   // sample 2 inner
				  {{{1.500, 2.500, 20},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0}},    // sample 3 outer
				   {{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0},{0.000, 0.000,  0}}}};  // sample 3 inner

//## end module%446626F5033F.additionalDeclarations


// Class EMECDetDescr 

EMECDetDescr::EMECDetDescr (const EMECDetectorManager *detManager, unsigned int sampling, unsigned int region, unsigned int radialPart, const CellBinning &phiBinning)
  //## begin EMECDetDescr::EMECDetDescr%446A2DDF00D2.hasinit preserve=no
  //## end EMECDetDescr::EMECDetDescr%446A2DDF00D2.hasinit
  //## begin EMECDetDescr::EMECDetDescr%446A2DDF00D2.initialization preserve=yes
  :m_manager(detManager),m_samplingIndex(sampling),m_regionIndex(region),m_radialIndex(radialPart),m_phiBinning(phiBinning),m_etaBinning(etaBoundariesEndcap[sampling][radialPart][region].min,
															     etaBoundariesEndcap[sampling][radialPart][region].max,
															     etaBoundariesEndcap[sampling][radialPart][region].nDiv)
  //## end EMECDetDescr::EMECDetDescr%446A2DDF00D2.initialization
{
  //## begin EMECDetDescr::EMECDetDescr%446A2DDF00D2.body preserve=yes



  // This contains some logic for decoding the sampling separation arrays:
  if (sampling==0) {
    m_halfLength.push_back(m_manager->getPresamplerFullLength()/2.0); 
    m_offset.push_back    (0);
    return;
  }

  double eta0, dEta;
  if (m_radialIndex ==0 ) {     // Outer Wheel.
    eta0=1.4;
    dEta=0.025;
  }
  else if (m_radialIndex==1) {  // Inner Wheel
    eta0=2.5;
    dEta=0.1;
  }
  else {
    throw std::range_error("Attempt to construct EMEC DetDescr with illegal index combinations");
  }

  const double inv_dEta = 1. / dEta;

  for (unsigned int i=0;i<m_etaBinning.getNumDivisions();i++) {
    double eta = m_etaBinning.binCenter(i);
    int ipad = int((eta - eta0) * inv_dEta);
    double front,back;
    if (m_radialIndex ==0 ) {     // Outer Wheel.
      if (sampling==1) {
	front=0;
	back = m_manager->getZSEP12(ipad)-m_manager->getFocalToRef()-m_manager->getRefToActive();
      }
      else if (sampling==2) {

	//  std::min is used to max out the length to the total active length; this is for
	//  cells that spill out the side.  They will have funny volumes. 

	front = m_manager->getZSEP12(ipad)-m_manager->getFocalToRef()-m_manager->getRefToActive();
	back  = std::min(m_manager->getActiveLength(),m_manager->getZSEP23(ipad/2)-m_manager->getFocalToRef()-m_manager->getRefToActive());
      }      
      else if (sampling==3) {
	front  = m_manager->getZSEP23(ipad/2)-m_manager->getFocalToRef()-m_manager->getRefToActive();
	back   = m_manager->getActiveLength();
      } 
      else {
	throw std::range_error("Attempt to construct EMEC DetDescr with illegal index combinations");
      }
     
    }
    else if (m_radialIndex==1) {  // Inner Wheel
      if (sampling==1) {
	front=0;
	back = m_manager->getZIW(ipad) - m_manager->getFocalToRef() - m_manager->getRefToActive();
      }
      else if (sampling==2) {
	front = m_manager->getZIW(ipad) - m_manager->getFocalToRef() - m_manager->getRefToActive();
	back     = m_manager->getActiveLength();
      }
      else {
	throw std::range_error("Attempt to construct EMEC DetDescr with illegal index combinations");
      }
    }
    else {
      throw std::range_error("Attempt to construct EMEC DetDescr with illegal index combinations");
    }
    m_offset.push_back((front+back)/2.);
    m_halfLength.push_back((back-front)/2.);
  }
  //## end EMECDetDescr::EMECDetDescr%446A2DDF00D2.body
}


EMECDetDescr::~EMECDetDescr()
{
  //## begin EMECDetDescr::~EMECDetDescr%446626F5033F_dest.body preserve=yes
  //## end EMECDetDescr::~EMECDetDescr%446626F5033F_dest.body
}


// Additional Declarations
  //## begin EMECDetDescr%446626F5033F.declarations preserve=yes
  //## end EMECDetDescr%446626F5033F.declarations

//## begin module%446626F5033F.epilog preserve=yes
//## end module%446626F5033F.epilog

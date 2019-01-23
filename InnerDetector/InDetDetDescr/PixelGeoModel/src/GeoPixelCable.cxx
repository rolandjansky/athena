/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelCable.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include <sstream>

GeoPixelCable::GeoPixelCable() 
  :   m_currentElement(-1)
{
  fillElements(m_gmt_mgr->GetLD(), m_gmt_mgr->Phi());
  if (m_elements.empty()) { // If no information for layer use layer 0 numbers.
    fillElements(0, m_gmt_mgr->Phi());
  }    
}

void
GeoPixelCable::fillElements(int layer, int phiSector)
{
  int nentries = m_gmt_mgr->PixelCableElements();
  int biStaveType = m_gmt_mgr->PixelBiStaveType(layer, phiSector);
  
  for (int i = 0; i < nentries; i++) {
    // bistave number
    // 0 means both staves in bistave the same
    // 1 means odd bistave
    // 2 means even bistave
    // If 0 then keep for all staves otherwise keep if odd/even correspond
    int cableBiStaveNum = m_gmt_mgr->PixelCableBiStaveNum(i); 
    if (m_gmt_mgr->PixelCableLayerNum(i) == layer) {
      if (!cableBiStaveNum || ((cableBiStaveNum%2) == biStaveType%2)) {
	m_elements.push_back(i);
      }
    }
  }
}


GeoVPhysVol* GeoPixelCable::Build() {
 
  if (m_currentElement < 0) std::cout << "ERROR: GeoPixelCableCurrent, current element not set." << std::endl; 

  int tableIndex = m_elements[m_currentElement];
  //std::cout << "GeoPixelCable: " << m_currentElement << ", " << tableIndex << std::endl;

  double zstart    =  m_gmt_mgr->PixelCableZStart(tableIndex);
  double zend      =  m_gmt_mgr->PixelCableZEnd(tableIndex);
  double length    =  std::abs(zend -  zstart);
  double width     =  m_gmt_mgr->PixelCableWidth(tableIndex);
  double thickness =  m_gmt_mgr->PixelCableThickness(tableIndex);
  double weight    =  m_gmt_mgr->PixelCableWeight(tableIndex);
  std::string label =  m_gmt_mgr->PixelCableLabel(tableIndex);
  
  std::string matName =  m_gmt_mgr->getMaterialName("Cable", m_gmt_mgr->GetLD());
  const GeoMaterial* cableMat = m_mat_mgr->getMaterial(matName);

  // If weight supplied use it to calculate the required thickness
  // otherwise use the suppled thickness 
  if (weight != 0) {
    thickness = weight / (cableMat->getDensity() * length * width);   
  } 

  const GeoBox* cableBox = new GeoBox(0.5*thickness,0.5*width,0.5*length);
  std::string logName = "Cable";
  std::ostringstream o;
  o << m_gmt_mgr->getLD_Label() << label;
  logName = logName+o.str();
  //std::cout << logName << ": " << weight/Gaudi::Units::g << " g, " << thickness << std::endl;

  GeoLogVol* theCable = new GeoLogVol(logName,cableBox,cableMat);
  GeoPhysVol* cablePhys = new GeoPhysVol(theCable);
  return cablePhys;
}


double GeoPixelCable::zpos() {

  int tableIndex = m_elements[m_currentElement];
  double zstart    =  m_gmt_mgr->PixelCableZStart(tableIndex);
  double zend      =  m_gmt_mgr->PixelCableZEnd(tableIndex);
  return 0.5*(zend +  zstart);
}

double GeoPixelCable::getStackOffset() {
  int tableIndex = m_elements[m_currentElement];
  return m_gmt_mgr->PixelCableStackOffset(tableIndex);

}


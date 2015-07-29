/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelCable.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include <sstream>

GeoPixelCable::GeoPixelCable() 
  :   m_currentElement(-1)
{
  fillElements(gmt_mgr->GetLD(), gmt_mgr->Phi());
  if (m_elements.empty()) { // If no information for layer use layer 0 numbers.
    fillElements(0, gmt_mgr->Phi());
  }    
}

void
GeoPixelCable::fillElements(int layer, int phiSector)
{
  int nentries = gmt_mgr->PixelCableElements();
  int biStaveType = gmt_mgr->PixelBiStaveType(layer, phiSector);
  
  for (int i = 0; i < nentries; i++) {
    // bistave number
    // 0 means both staves in bistave the same
    // 1 means odd bistave
    // 2 means even bistave
    // If 0 then keep for all staves otherwise keep if odd/even correspond
    int cableBiStaveNum = gmt_mgr->PixelCableBiStaveNum(i); 
    if (gmt_mgr->PixelCableLayerNum(i) == layer) {
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

  double zstart    =  gmt_mgr->PixelCableZStart(tableIndex);
  double zend      =  gmt_mgr->PixelCableZEnd(tableIndex);
  double length    =  std::abs(zend -  zstart);
  double width     =  gmt_mgr->PixelCableWidth(tableIndex);
  double thickness =  gmt_mgr->PixelCableThickness(tableIndex);
  double weight    =  gmt_mgr->PixelCableWeight(tableIndex);
  std::string label =  gmt_mgr->PixelCableLabel(tableIndex);
  
  std::string matName =  gmt_mgr->getMaterialName("Cable", gmt_mgr->GetLD());
  const GeoMaterial* cableMat = mat_mgr->getMaterial(matName);

  // If weight supplied use it to calculate the required thickness
  // otherwise use the suppled thickness 
  if (weight != 0) {
    thickness = weight / (cableMat->getDensity() * length * width);   
  } 

  const GeoBox* cableBox = new GeoBox(0.5*thickness,0.5*width,0.5*length);
  std::string logName = "Cable";
  std::ostringstream o;
  o << gmt_mgr->getLD_Label() << label;
  logName = logName+o.str();
  //std::cout << logName << ": " << weight/CLHEP::g << " g, " << thickness << std::endl;

  GeoLogVol* theCable = new GeoLogVol(logName,cableBox,cableMat);
  GeoPhysVol* cablePhys = new GeoPhysVol(theCable);
  return cablePhys;
}


double GeoPixelCable::zpos() {

  int tableIndex = m_elements[m_currentElement];
  double zstart    =  gmt_mgr->PixelCableZStart(tableIndex);
  double zend      =  gmt_mgr->PixelCableZEnd(tableIndex);
  return 0.5*(zend +  zstart);
}

double GeoPixelCable::getStackOffset() {
  int tableIndex = m_elements[m_currentElement];
  return gmt_mgr->PixelCableStackOffset(tableIndex);

}


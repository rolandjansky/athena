/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_IDENTIFIER_H
#define SCT_SLHC_GeoModel_SCT_IDENTIFIER_H

#include "Identifier/Identifier.h"
#include <iostream>

class SCT_ID;

namespace InDetDDSLHC {

class SCT_Identifier
{
public:

  SCT_Identifier(int barrelEC = 0,
		 int layerDisk = 0, 
		 int phiModule = 0,
		 int etaModule = 0,
		 int side = 0)
    : m_barrelEC(barrelEC), 
      m_layerDisk(layerDisk), 
      m_phiModule(phiModule), 
      m_etaModule(etaModule), 
      m_side(side)
  {};


  void setBarrelEC(int i) {m_barrelEC = i;}
  int  getBarrelEC() const {return m_barrelEC;}

  void setLayerDisk(int i) {m_layerDisk = i;}
  int  getLayerDisk() const {return m_layerDisk;}

  void setEtaModule(int i) {m_etaModule = i;}
  int  getEtaModule() const {return m_etaModule;}

  void setPhiModule(int i) {m_phiModule = i;}
  int  getPhiModule() const {return m_phiModule;}

  void setSide(int i) {m_side = i;}
  int  getSide() const {return m_side;}

  static void setIdHelper(const SCT_ID * idHelper) {s_idHelper = idHelper;}

  Identifier getWaferId();

  // For debugging purposes.
  void print();

private:
  int m_barrelEC;
  int m_layerDisk;
  int m_phiModule;
  int m_etaModule;
  int m_side;
  static const SCT_ID * s_idHelper;
};
}
#endif // SCT_SLHC_GeoModel_SCT_IDENTIFIER_H

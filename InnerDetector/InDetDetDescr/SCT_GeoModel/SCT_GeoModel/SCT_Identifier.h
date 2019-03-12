/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_IDENTIFIER_H
#define SCT_GEOMODEL_SCT_IDENTIFIER_H

class Identifier;
class SCT_ID;

class SCT_Identifier
{
public:

  SCT_Identifier(const SCT_ID* idHelper,
                 int barrelEC = 0,
		 int layerDisk = 0, 
		 int phiModule = 0,
		 int etaModule = 0,
		 int side = 0)
    : m_idHelper{idHelper},
      m_barrelEC{barrelEC},
      m_layerDisk{layerDisk},
      m_phiModule{phiModule},
      m_etaModule{etaModule},
      m_side{side}
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

  Identifier getWaferId();

  // For debugging purposes.
  void print();

private:
  const SCT_ID* m_idHelper;
  int m_barrelEC;
  int m_layerDisk;
  int m_phiModule;
  int m_etaModule;
  int m_side;
};

#endif // SCT_GEOMODEL_SCT_IDENTIFIER_H

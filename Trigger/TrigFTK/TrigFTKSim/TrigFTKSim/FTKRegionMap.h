/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKREGIONMAP_H
#define FTKREGIONMAP_H

#include <set>
#include <map>
#include "FTKPMap.h"
#include "FTKHit.h"

class FTKRegionMapItem {
private:
  int m_phi_min; // minimum phi ID
  int m_phi_max; // maximum phi ID
  int m_phi_tot; // total number of modules along phi
  int m_eta_min; // minimum eta ID
  int m_eta_max; // minimum eta ID
  int m_eta_tot; // total number of modules along eta
   //std::set<unsigned int> m_global_module_ids;
public:
  FTKRegionMapItem();
  FTKRegionMapItem(int phi_min, int phi_max, int phi_tot, int eta_min, int eta_max, int eta_tot):
     m_phi_min(phi_min), m_phi_max(phi_max), m_phi_tot(phi_tot), m_eta_min(eta_min), m_eta_max(eta_max), m_eta_tot(eta_tot) {}
  int getPhiMin() const { return m_phi_min; }
  int getPhiMax() const { return m_phi_max; }
  int getPhiTot() const {return m_phi_tot; }
  int getEtaMin() const { return m_eta_min; }
  int getEtaMax() const { return m_eta_max; }
  int getEtaTot() const {return m_eta_tot; }
  void setPhiMin(int v) { m_phi_min = v; }
  void setPhiMax(int v) { m_phi_max = v; }
  void setPhiTot(int v) { m_phi_tot = v; }
  void setEtaMin(int v) { m_eta_min = v; }
  void setEtaMax(int v) { m_eta_max = v; }
  void setEtaTot(int v) { m_eta_tot = v; }

  // compatibility functions for phi:
  int getMin() const { return getPhiMin(); }
  int getMax() const { return getPhiMax(); }
  int getTotMod() const {return getPhiTot()-1; } // maximum ID of phi_module
  void setMin(int v) { setPhiMin(v); }
  void setMax(int v) { setPhiMax(v); }
  void setTotMod(int v) { setPhiTot(v); }
  
  // std::set<unsigned int> getGlobalModuleIds() const { return m_global_module_ids; }
  // void addGlobalModuleId(const unsigned int& id) { m_global_module_ids.insert(id); }
  // void clearGlobalModuleIds() { m_global_module_ids.clear(); }
  // bool hasGlobalModuleId(const unsigned int& id) const { return(m_global_module_ids.find(id)!=m_global_module_ids.end()); }
};

class FTKRegionMapOfflineId {
private:
  bool m_isPixel;
  int m_barrelEC;
  int m_layer;
  int m_etamod;
  int m_phimod;
public:
  FTKRegionMapOfflineId()
  : m_isPixel(false)
  , m_barrelEC(-999)
  , m_layer(-999)
  , m_etamod(-999)
  , m_phimod(-999) {}
  FTKRegionMapOfflineId(bool isPixel, int barrelEC, int layer, int etamod, int phimod) :
     m_isPixel(isPixel), m_barrelEC(barrelEC), m_layer(layer), m_etamod(etamod), m_phimod(phimod) {}
  
  bool getIsPixel() const { return m_isPixel; }
  void setIsPixel(const bool& isPixel) { m_isPixel = isPixel; }
  int getBarrelEC() const { return m_barrelEC; }
  void setBarrelEC(const int& barrelEC)  { m_barrelEC = barrelEC; }
  int getLayer() const { return m_layer; }
  void setLayer(const int& layer)  { m_layer = layer; }
  int getEtaMod() const { return m_etamod; }
  void setEtaMod(const int& etamod)  { m_etamod = etamod; }
  int getPhiMod() const { return m_phimod; }
  void setPhiMod(const int& phimod)  { m_phimod = phimod; }
  
  bool operator==(const FTKRegionMapOfflineId& rhs) const {
    return( m_isPixel==rhs.m_isPixel &&
           m_barrelEC==rhs.m_barrelEC &&
           m_layer==rhs.m_layer &&
           m_etamod==rhs.m_etamod &&
           m_phimod==rhs.m_phimod );
  }
};


class FTKRegionMap {
public:
  typedef std::map<unsigned int,unsigned int> global_to_local_map_type;
  typedef std::map<unsigned int,global_to_local_map_type> plane_global_to_local_map_type;
  typedef std::map<unsigned int, plane_global_to_local_map_type > tower_global_to_local_map_type;
  typedef std::map<unsigned int,FTKRegionMapOfflineId> global_to_offline_map_type;

private:
  std::string m_path; // configuration file path
  bool m_isok;
  bool m_old_format; // old_format = 8 regions; otherwise 64 towers
  bool m_ITk_format; // ITk_format = specific to ITk geometry
  FTKPlaneMap *m_pmap;
  FTKPlaneMap *m_m_pmap; //whether this class 'owns' m_pmap
  int m_nregions;
  int m_nregions_phi;
  int m_nplanes;
  int *m_sections;
  FTKRegionMapItem ***m_map;

  // lookup table for local module ID within a tower:
  //   m_tower_global_to_local_map[tower id][global id] = (local id)
  tower_global_to_local_map_type m_tower_global_to_local_map;
  // conversion between global module IDs and offline
  global_to_offline_map_type m_global_to_offline_map;
  
  static const TString s_dirname;
  
private:
  void load_offline_lut(const char *path);
  
public:
  FTKRegionMap();
  virtual ~FTKRegionMap();  
  FTKRegionMap(FTKPlaneMap *, const char *);
  FTKRegionMap(TDirectory* file);

  static FTKRegionMap* GetRMapFromRootFile(TDirectory* file);
  static int GetChecksum(TDirectory* file);
  static int GetChecksumLUT(TDirectory* file);
  void WriteMapToRootFile(TDirectory* file);
  int CalcChecksum() const;
  int CalcChecksumLUT() ;

  void loadModuleIDLUT(const char *);
  bool isModuleIDLUTloaded(void) const;
  const char *getPath() const { return m_path.c_str(); }

  const FTKPlaneMap* getPlaneMap() const { return m_pmap; }
  FTKPlaneMap* getModifyablePlaneMap() const { return m_pmap; }

  const FTKRegionMapItem &getRegionMapItem(int, int, int) const;
  int getNumRegions() const { return m_nregions; }
  int getNumRegionsPhi() const { return m_nregions_phi; }

  const int& getNPlanes() const { return m_nplanes; }

  unsigned int getLocalModuleID(const FTKHit&, unsigned int) const;
  void convertLocalID(unsigned int, int, int, int, int&, int&);
  unsigned int getLocalID(int towerID,int plane,int section,
                          int modPhi,int modEta) const;

  bool isHitInRegion(const FTKHit&,int) const;
  bool isHitInRegion_ITk(const FTKHit&,int) const;
  bool isHitInRegion_old(const FTKHit&,int) const;
  
  //   (local id) == -1 => module is not in the tower
  int getLocalId(const unsigned int& towerId,const unsigned int& planeId,const unsigned int& globalModuleId) const;
  int getGlobalId(const unsigned int& towerId,const unsigned int& planeId,const unsigned int& localModuleId) const;
  std::map<unsigned int,unsigned int> getGlobalToLocalMapping(const unsigned int& towerId,const unsigned int& planeId) const;

  bool getIsOK() const { return m_isok;}
  bool operator!() const { return !m_isok; }

};

#endif // FTKREGIONMAP_H

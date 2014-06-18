/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKREGIONMAP_H
#define FTKREGIONMAP_H

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

public:
  FTKRegionMapItem();

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
};


class FTKRegionMap {
private:
  bool m_isok;

  bool m_old_format; // old_format = 8 regions; otherwise 64 towers

  std::string m_path; // configuration file path

  FTKPlaneMap *m_pmap;

  int m_nregions;
  int m_nregions_phi;

  int m_nplanes;

  int *m_sections;

  FTKRegionMapItem ***m_map;

public:
  FTKRegionMap();
  FTKRegionMap(FTKPlaneMap *, const char *);

  const char *getPath() const { return m_path.c_str(); }

  const FTKPlaneMap* getPlaneMap() const { return m_pmap; }

  FTKRegionMapItem &getRegionMapItem(int, int, int) const;
  int getNumRegions() const { return m_nregions; }
  int getNumRegionsPhi() const { return m_nregions_phi; }

  const int& getNPlanes() const { return m_nplanes; }

  unsigned int getLocalModuleID(const FTKHit&, unsigned int);
  void convertLocalID(unsigned int, int, int, int, int&, int&);

  bool isHitInRegion(const FTKHit&,int) const;
  bool isHitInRegion_old(const FTKHit&,int) const;

  bool operator!() const { return !m_isok; }

};

#endif // FTKREGIONMAP_H

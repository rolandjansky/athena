/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* ****************************************************************************
 Filename    : CaloDmDescrManager.h
 Author      : guennadi.pospelov@cern.ch
 Created     : March, 2007
 DESCRIPTION : dead material identifiers description package
 Usage:
 const CaloDmDescrManager *mgr = CaloDmDescrManager::instance();
 CaloDmDescrElement *element = mgr->get_element(dm_identifier);
 Enjoy with element->eta(), element->phi().
**************************************************************************** */

#ifndef CALODMDETDESCR_CALODMDESCRMANAGER_H
#define CALODMDETDESCR_CALODMDESCRMANAGER_H

#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloDmDetDescr/CaloDmDescrArea.h"


class Identifier;
class IdentifierHash;
class CaloDM_ID;


/* ****************************************************************************
Description element for dead material calibration hit
**************************************************************************** */
class CaloDmDescrElement {
  public:
    CaloDmDescrElement() :
      m_eta(0.0),
      m_deta(0.0),
      m_phi(0.0),
      m_dphi(0.0),
      m_r(0.0),
      m_z(0.0) {}
    CaloDmDescrElement(float xeta, float xdeta, float xphi, float xdphi, float xr, float xz) :
      m_eta( xeta ),
      m_deta( xdeta ),
      m_phi( xphi ),
      m_dphi( xdphi ),
      m_r( xr ),
      m_z( xz ) {}

    ~CaloDmDescrElement() {}

    void print() const {
      std::cout << "  eta: " << m_eta  << "  phi: " << m_phi << "  deta: " << m_deta  << "  dphi: " << m_dphi
          << "  r: " << m_r << "  z: " << m_z << std::endl;
    }

    inline float eta() const { return m_eta; }
    inline float deta() const { return m_deta; }
    inline float phi() const { return m_phi; }
    inline float dphi() const { return m_dphi; }
    inline float r() const { return m_r; }
    inline float z() const { return m_z; }

  private:
    float m_eta;
    float m_deta;
    float m_phi;
    float m_dphi;
    float m_r;
    float m_z;
};



/* ****************************************************************************
Dead Material region definition. 
**************************************************************************** */
class CaloDmRegion
{
  public:
    CaloDmRegion():
    m_region_id(0),                // first Identifier in this region (etaBin=0, phiBin=0)
    m_region_hash(0),              // hash number of this region
    m_key(0),
    m_side(0),
    m_neta_max(0),
    m_nphi_max(0),
    m_eta_min(0.0),
    m_eta_max(0.0),
    m_deta(0.0),
    m_dphi(0.0),
    m_distance(0.0),
    m_isBarrel(false) {}
    Identifier m_region_id;
    IdentifierHash m_region_hash;
    int m_key;
    int m_side;
    int m_neta_max;
    int m_nphi_max;
    float m_eta_min;
    float m_eta_max;
    float m_deta;
    float m_dphi;
    float m_distance;
    bool  m_isBarrel;
    std::vector<short > m_CaloSampleNeighbours;
    std::vector<float > m_CaloSampleEtaMin;
    std::vector<float > m_CaloSampleEtaMax;

    // Can use default copy ctor, assignment op, dtor.

    inline const IdentifierHash region_hash  () const { return m_region_hash; }
    inline const Identifier region_id  () const { return m_region_id; }
    inline float etaMin() const { return m_eta_min; }
    inline float etaMax() const { return m_eta_max; }
    inline float deta() const { return m_deta; }
    inline float dphi() const { return m_dphi; }
    inline int netaMax() const { return m_neta_max; }
    inline int nphiMax() const { return m_nphi_max; }
};



/* ****************************************************************************
Dead Material description manager to deal with dead material calibration hits.
> Contains vector of 57 Dead Material regions.
  m_caloDM_ID->lar_region_hash_max() + m_caloDM_ID->tile_region_hash_max() = 57
> Contains vector of ~80000 description element for DM calibration hits
  m_caloDM_ID->lar_zone_hash_max() + m_caloDM_ID->tile_zone_hash_max() ~ 80000
**************************************************************************** */
class CaloDmDescrManager { 
  public:
    CaloDmDescrManager();
    ~CaloDmDescrManager();

    static const CaloDmDescrManager* instance();

    int initialize();

    CaloDmDescrElement* get_element(const Identifier& cellId) const;
    CaloDmRegion* get_dm_region(const Identifier& cellId) const;
    IdentifierHash get_dm_hash(const CaloDmRegion *myRegion, float &eta, float &phi) const;
    int get_dm_key(const Identifier& cellId) const;
    int get_dm_area(const Identifier& cellId) const;
    void show_element(const Identifier& cellId) const;

    CaloCell_ID::CaloSample get_calo_sample(const std::string &SamplingName);
    CaloCell_ID::CaloSample get_calo_sample(const std::string &SamplingName, float &etaMin, float &etaMax);

    std::string m_DmRegionFileName;

    /* iterators over set of DM regions (LAr + Tiles) */
    std::vector<CaloDmRegion *>::const_iterator reg_begin    (void) const;
    std::vector<CaloDmRegion *>::const_iterator reg_end      (void) const;

  private:
    const CaloDM_ID*    m_caloDM_ID;
    const AtlasDetectorID* m_id_helper;
    StoreGateSvc* m_detStore;

    CaloDmDescrElement* build_element(const Identifier& cellId, const CaloDmRegion *myRegion);
    StatusCode load_regions(std::string DmRegionFileName);
    void build_element_vector();

    std::vector<CaloDmRegion *> m_DmRegionVector;
    std::vector<CaloDmDescrElement *> m_DmElementVector;
};


//----------------------------------------------------------------------------
inline std::vector<CaloDmRegion *>::const_iterator CaloDmDescrManager::reg_begin (void) const
{
  return(m_DmRegionVector.begin());
}

inline std::vector<CaloDmRegion *>::const_iterator CaloDmDescrManager::reg_end (void) const
{
  return(m_DmRegionVector.end());
}


#endif //CALOCALIBHITREC_CALODMDESCRMANAGER_H

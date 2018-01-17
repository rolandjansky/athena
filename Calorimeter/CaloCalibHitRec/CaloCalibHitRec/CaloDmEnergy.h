/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ****************************************************************************
 Filename    : CaloDmEnergy.h
 Author      : guennadi.pospelov@cern.ch
 Created     : May, 2007
 DESCRIPTION : calculate Dead Material energy in different DM areas
               (sum over specific groups of DM calibration hits).
 Usage       : see CBNT_CalibrationInfoDM2 algorithm for example
**************************************************************************** */

#ifndef CALOCALIBHITREC_CALODMENERGY_H
#define CALOCALIBHITREC_CALODMENERGY_H

#include <vector>

#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloDmDetDescr/CaloDmDescrManager.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class Identifier;
class CaloDM_ID;
//class xAOD::CaloClusterContainer;
//class xAOD::CaloCluster;
class Identifier;
class CaloDetDescrManager;
class CaloDmNeighbours;
class CaloCalibrationHitContainer;



/* ****************************************************************************
Special dead material cell based on dead material calibration hit 
Has a list of neighboring clusters
**************************************************************************** */
class CaloDmCell
{
  public:

    CaloDmCell(const CaloCalibrationHit *hit, const CaloDmDescrElement *element, const CaloDmRegion *region);
    ~CaloDmCell();

    Identifier m_id;
    float m_energy_total;

    void Add(CaloCalibrationHit *hit);

    inline float eta() const { return m_element->eta(); }
    inline float deta() const { return m_element->deta();}
    inline float phi() const { return m_element->phi();}
    inline float dphi() const { return m_element->dphi();}
    inline float z() const { return m_element->z();}
    inline float r() const { return m_element->r();}

    inline Identifier cellID() const { return m_id; }
    inline float energyTotal() const { return m_energy_total; }
    inline int dmAreaN() const { return m_dmarea_n; }

    inline void SetAreaN(int dm_area) { m_dmarea_n = dm_area; }

    std::vector<short > m_cls_index;
    std::vector<double > m_cls_energy;
    double m_sum_cls_energy;
    int m_dmarea_n;
    const CaloDmDescrElement *m_element;
    const CaloDmRegion *m_region;
  private:

    CaloDmCell (const CaloDmCell&);
    CaloDmCell& operator= (const CaloDmCell&);
};



/* ****************************************************************************
To calculate dead material energy in different areas
**************************************************************************** */
class CaloDmEnergy
{
  public:
    // these are keys for specific groups of dead material hits
    enum DmArea {
      DMA_ALL,              // 0,  total energy in all calibration hits
      DMA_EMB0,             // 1,  energy before PreSamplerB, |eta|<1.5
      DMA_EMB1,             // 2,  energy between PreSamplerB and EMB1
      DMA_EMB_HALVES,       // 3,  energy between two barrel halves
      DMA_EMB3_TILE0,       // 4,  energy between EMB3 and TILE0
      DMA_SCN,              // 5,  energy before scintillator
      DMA_EME_EXT,          // 6,  energy between (emec+hec) and TileExt0
      DMA_EME0,             // 7,  energy before PreSamplerE |eta|<1.5
      DMA_EME11,            // 8,  energy before EME1 |eta|<1.5
      DMA_EME12,            // 9,  energy before EME1 1.5<|eta|<1.8
      DMA_EME13,            // 10, energy before EME1 1.8<|eta|<2.5
      DMA_EME14,            // 11, energy before EME1 2.5<|eta|<3.2
      DMA_EME_INNER_OUTER,  // 12, energy between inner and outer wheel
      DMA_EME3_HEC0,        // 13, energy between EME3 and HEC0
      DMA_HECWHEEL,         // 14, energy betwee HEC1 and HEC2
      DMA_HEC_AZIMUTH,      // 15, energy in HEC azimuthal cracks
      DMA_HEC_FCAL,         // 16, energy between HEC and FCAL
      DMA_FCAL0_FCAL1,      // 17, energy between FCAL0 and FCAL1
      DMA_FCAL1_FCAL2,      // 18, energy between FCAL1 and FCAL2
      DMA_FCAL0,            // 19, energy before FCAL0
      DMA_LEAK,             // 20, leakage and energy flow (+TILE girder)
      DMA_DEFCALC,          // 21, default calclulator energy
      DMA_UNCLASS,          // 22, energy in hits out of this classification
      DMA_MAX               // 23.
    };

    CaloDmEnergy(const CaloDmDescrManager *dmMgr);
    ~CaloDmEnergy();

    StatusCode initialize();
    int assign2clusters(const std::string &clusterContainerName);
    int assign2clusters(std::vector<const CaloCalibrationHitContainer *> &v_dmcchc, const xAOD::CaloClusterContainer *theClusters, std::vector<double> &v_cluster_calib_energy);
    bool get_distance_cluster_dmcell(const xAOD::CaloCluster *cluster, const CaloDmCell *dm_cell, double &energy, double &distance);
    float GetClusterDmEnergy(int iClus, CaloDmEnergy::DmArea n_area) { return (float) m_dmcls_area_etotal[iClus][n_area]; }
    int get_area(Identifier id, float eta) const;

    /* iterators over DM cells */
    std::vector<CaloDmCell *>::const_iterator cell_begin    (void) const;
    std::vector<CaloDmCell *>::const_iterator cell_end      (void) const;

    inline int getCellVectorSize() const { return (int)m_CaloDmCellVector.size(); }

    // total DM energy in different DM areas (sum over specific group of DM calib. hits)
    double m_dmsum_etotal;
    double m_dmsum_em;
    double m_dmsum_nonem;
    double m_dmsum_invisible;
    double m_dmsum_escaped;
    std::vector< double > m_dmsum_area_etotal;
    int m_nclusters;
    // total DM energy in different DM areas after assignment
    double m_dmcls_sum_etotal;
    std::vector<std::vector<double > > m_dmcls_area_etotal;

    std::vector<std::string> m_CalibrationContainerNamesDM;

  private:
    int make_dmcell_vector(std::vector<const CaloCalibrationHitContainer *> &v_dmcchc);
    int get_key(Identifier id) const;
    int get_calo_deta_dphi(Identifier &id_cel, double &deta_cel, double &dphi_cel);

    int m_max_dmhit;
    int m_max_cluster;
    // assignment parameters:
    float m_apars_alpha;            // parameter for cluster weighting function E^{alpha}*exp(-distance/r0)
    float m_apars_r0;               // parameter for cluster weighting function E^{alpha}*exp(-distance/r0)
    float m_apars_clust_min_ener;   // minimum cluster energy
    float m_apars_clust_min_ecalib; // minimum cluster calibration energy
    float m_apars_cone_cut;         // cone cut on distance to the cluster

    std::vector<CaloDmCell *> m_CaloDmCellVector;
    std::vector<CaloDmCell *> m_CaloDmCellContainer;

    const CaloDmDescrManager *m_caloDmDescrManager;
    const CaloDmNeighbours* m_caloDmNeighbours;
    const CaloDetDescrManager* m_caloDetDescrManager;
    const CaloDM_ID*    m_caloDM_ID;
    const CaloCell_ID*    m_caloCell_ID;
    const AtlasDetectorID* m_id_helper;
    StoreGateSvc* m_storeGate;
    StoreGateSvc* m_detStore;

    CaloDmEnergy (const CaloDmEnergy&);
    CaloDmEnergy& operator= (const CaloDmEnergy&);
};

inline std::vector<CaloDmCell *>::const_iterator CaloDmEnergy::cell_begin (void) const
{
  return(m_CaloDmCellVector.begin());
}

inline std::vector<CaloDmCell *>::const_iterator CaloDmEnergy::cell_end (void) const
{
  return(m_CaloDmCellVector.end());
}


#endif

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_CALODETDESCRELEMENT_G_H
#define ISF_FASTCALOGPU_CALODETDESCRELEMENT_G_H

#  include "Identifier_g.h"

class CaloDetDescrElement_Gpu {
public:
  CUDA_HOSTDEV CaloDetDescrElement_Gpu(const long long identify, const long long hash_id, const int calosample, const float eta, const float phi, const float deta, const float dphi, const float r, const float eta_raw, const float phi_raw, const float r_raw, const float dr, const float x, const float y, const float z, const float x_raw, const float y_raw, const float z_raw, const float dx, const float dy, const float dz) {
    m_identify   = identify;
    m_hash_id    = hash_id;
    m_calosample = calosample;
    m_eta        = eta;
    m_phi        = phi;
    m_deta       = deta;
    m_dphi       = dphi;
    m_r          = r;
    m_eta_raw    = eta_raw;
    m_phi_raw    = phi_raw;
    m_r_raw      = r_raw;
    m_dr         = dr;
    m_x          = x;
    m_y          = y;
    m_z          = z;
    m_x_raw      = x_raw;
    m_y_raw      = y_raw;
    m_z_raw      = z_raw;
    m_dx         = dx;
    m_dy         = dy;
    m_dz         = dz;
  };

  CUDA_HOSTDEV CaloDetDescrElement_Gpu() {
    m_identify   = 0;
    m_hash_id    = 0;
    m_calosample = 0;
    m_eta        = 0;
    m_phi        = 0;
    m_deta       = 0;
    m_dphi       = 0;
    m_r          = 0;
    m_eta_raw    = 0;
    m_phi_raw    = 0;
    m_r_raw      = 0;
    m_dr         = 0;
    m_x          = 0;
    m_y          = 0;
    m_z          = 0;
    m_x_raw      = 0;
    m_y_raw      = 0;
    m_z_raw      = 0;
    m_dx         = 0;
    m_dy         = 0;
    m_dz         = 0;
  };

  /** @brief virtual destructor
   */
  CUDA_HOSTDEV ~CaloDetDescrElement_Gpu(){};

  /** @brief cell eta
   */
  CUDA_HOSTDEV float eta() const;
  /** @brief cell phi
   */
  CUDA_HOSTDEV float phi() const;
  /** @brief cell r
   */
  CUDA_HOSTDEV float r() const;
  /** @brief cell eta_raw
   */
  CUDA_HOSTDEV float eta_raw() const;
  /** @brief cell phi_raw
   */
  CUDA_HOSTDEV float phi_raw() const;
  /** @brief cell r_raw
   */
  CUDA_HOSTDEV float r_raw() const;
  /** @brief cell dphi
   */
  CUDA_HOSTDEV float dphi() const;
  /** @brief cell deta
   */
  CUDA_HOSTDEV float deta() const;
  /** @brief cell dr
   */
  CUDA_HOSTDEV float dr() const;

  /** @brief cell x
   */
  CUDA_HOSTDEV float x() const;
  /** @brief cell y
   */
  CUDA_HOSTDEV float y() const;
  /** @brief cell z
   */
  CUDA_HOSTDEV float z() const;
  /** @brief cell x_raw
   */
  CUDA_HOSTDEV float x_raw() const;
  /** @brief cell y_raw
   */
  CUDA_HOSTDEV float y_raw() const;
  /** @brief cell z_raw
   */
  CUDA_HOSTDEV float z_raw() const;
  /** @brief cell dx
   */
  CUDA_HOSTDEV float dx() const;
  /** @brief cell dy
   */
  CUDA_HOSTDEV float dy() const;
  /** @brief cell dz
   */
  CUDA_HOSTDEV float dz() const;

  /** @brief cell identifier
   */
  CUDA_HOSTDEV Identifier_Gpu identify() const;
  CUDA_HOSTDEV long long get_compact() const;

  CUDA_HOSTDEV unsigned long long calo_hash() const;

  CUDA_HOSTDEV int getSampling() const;

  // ACH protected:
  //
  long long m_identify;
  long long m_hash_id;

  int m_calosample;

  /** @brief cylindric coordinates : eta
   */
  float m_eta;
  /** @brief cylindric coordinates : phi
   */
  float m_phi;

  /** @brief this one is cached for algorithm working in transverse Energy
   */
  float m_sinTh;
  /** @brief this one is cached for algorithm working in transverse Energy
   */
  float m_cosTh;

  /** @brief cylindric coordinates : delta eta
   */
  float m_deta;
  /** @brief cylindric coordinates : delta phi
   */
  float m_dphi;

  /** @brief cylindric coordinates : r
   */

  float m_volume;

  /** @brief cache to allow fast px py pz computation
   */
  float m_sinPhi;

  /** @brief cache to allow fast px py pz computation
   */
  float m_cosPhi;

  /** @brief cylindric coordinates : r
   */
  float m_r;
  /** @brief cylindric coordinates : eta_raw
   */
  float m_eta_raw;
  /** @brief cylindric coordinates : phi_raw
   */
  float m_phi_raw;
  /** @brief cylindric coordinates : r_raw
   */
  float m_r_raw;
  /** @brief cylindric coordinates : delta r
   */
  float m_dr;

  /** @brief cartesian coordinates : X
   */
  float m_x;
  /** @brief cartesian coordinates : Y
   */
  float m_y;
  /** @brief cartesian coordinates : Z
   */
  float m_z;
  /** @brief cartesian coordinates : X raw
   */
  float m_x_raw;
  /** @brief cartesian coordinates : Y raw
   */
  float m_y_raw;
  /** @brief cartesian coordinates : Z raw
   */
  float m_z_raw;
  /** @brief cartesian coordinates : delta X
   */
  float m_dx;
  /** @brief cartesian coordinates : delta Y
   */
  float m_dy;
  /** @brief cartesian coordinates : delta Z
   */
  float m_dz;
};

CUDA_HOSTDEV inline Identifier_Gpu CaloDetDescrElement_Gpu::identify() const {
  Identifier_Gpu id( (unsigned long long)m_identify );
  return id;
}

CUDA_HOSTDEV inline long long CaloDetDescrElement_Gpu::get_compact() const {
  return m_identify;
}

CUDA_HOSTDEV inline unsigned long long CaloDetDescrElement_Gpu::calo_hash() const { return m_hash_id; }

CUDA_HOSTDEV inline int   CaloDetDescrElement_Gpu::getSampling() const { return m_calosample; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::eta() const { return m_eta; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::phi() const { return m_phi; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::r() const { return m_r; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::eta_raw() const { return m_eta_raw; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::phi_raw() const { return m_phi_raw; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::r_raw() const { return m_r_raw; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::deta() const { return m_deta; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::dphi() const { return m_dphi; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::dr() const { return m_dr; }

CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::x() const { return m_x; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::y() const { return m_y; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::z() const { return m_z; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::x_raw() const { return m_x_raw; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::y_raw() const { return m_y_raw; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::z_raw() const { return m_z_raw; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::dx() const { return m_dx; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::dy() const { return m_dy; }
CUDA_HOSTDEV inline float CaloDetDescrElement_Gpu::dz() const { return m_dz; }

#endif

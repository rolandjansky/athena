/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_Cluster_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Represents the persitified version of HGTD_Cluster.
 */

#ifndef HGTD_CLUSTER_P1_H
#define HGTD_CLUSTER_P1_H

#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "Identifier/Identifier.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidth_p2.h"

class HGTD_Cluster_p1 {
public:
  typedef Identifier::value_type IdType_t;

  HGTD_Cluster_p1() {}
  friend class HGTD_ClusterCnv_p1;

private:
  IdType_t m_clus_id;
  std::vector<IdType_t> m_rdo_id_list;
  float m_local_pos_x;
  float m_local_pos_y;
  float m_mat_00;
  float m_mat_01;
  float m_mat_11;
  float m_time;
  float m_time_resolution;
  std::vector<int> m_time_over_threshold;
  InDet::SiWidth_p2 m_width;
};

#endif // HGTD_CLUSTER_P1_H

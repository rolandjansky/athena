/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/src/HGTD_ClusterCnv_p1.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Implementation file for class HGTD_ClusterCnv_p1
 */
#include "HGTD_EventTPCnv/HGTD_ClusterCnv_p1.h"

#include "EventPrimitives/EventPrimitives.h"
#include <algorithm>
#include <iostream>

void HGTD_ClusterCnv_p1::persToTrans(
    const HGTD_Cluster_p1* pers_obj, HGTD_Cluster* trans_obj,
    MsgStream& log) {
  log << MSG::VERBOSE << "In HGTD_ClusterCnv_p1::persToTrans" << endmsg;
  *trans_obj = createHGTDCluster(pers_obj, nullptr, log);
}

HGTD_Cluster HGTD_ClusterCnv_p1::createHGTDCluster(
    const HGTD_Cluster_p1* pers_obj,
    const InDetDD::SolidStateDetectorElementBase* del_el, MsgStream& log) {
  log << MSG::VERBOSE << "In HGTD_ClusterCnv_p1::createHGTDCluster" << endmsg;
  // set the cluster identifier
  Identifier cluster_id(pers_obj->m_clus_id);

  std::vector<Identifier> rdo_list;
  size_t rdo_list_size = pers_obj->m_rdo_id_list.size();
  rdo_list.resize(rdo_list_size);

  for (size_t i = 0; i < rdo_list_size; i++) {
    rdo_list.at(i) = Identifier(pers_obj->m_rdo_id_list.at(i));
  }

  // Local Position
  Amg::Vector2D local_pos;
  local_pos[Trk::locX] = pers_obj->m_local_pos_x;
  local_pos[Trk::locY] = pers_obj->m_local_pos_y;

  InDet::SiWidth width;
  m_si_width_cnv.persToTrans(&pers_obj->m_width, &width, log);

  // Error matrix
  auto cmat = Amg::MatrixX(2,2);
  (cmat)(0, 0) = static_cast<double>(pers_obj->m_mat_00);
  (cmat)(1, 0) = static_cast<double>(pers_obj->m_mat_01);
  (cmat)(0, 1) = static_cast<double>(pers_obj->m_mat_01);
  (cmat)(1, 1) = static_cast<double>(pers_obj->m_mat_11);

  HGTD_Cluster cluster(cluster_id, local_pos, std::move(rdo_list),
                       std::move(width), del_el, std::move(cmat),
                       pers_obj->m_time,
                       pers_obj->m_time_resolution,
                       std::move(pers_obj->m_time_over_threshold));

  return cluster;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////s

void HGTD_ClusterCnv_p1::transToPers(const HGTD_Cluster* trans_obj,
                                           HGTD_Cluster_p1* pers_obj,
                                           MsgStream& log) {
  log << MSG::VERBOSE << "In HGTD_ClusterCnv_p1::transToPers" << endmsg;
  m_si_width_cnv.transToPers(&trans_obj->width(), &pers_obj->m_width, log);

  // Local Position
  pers_obj->m_local_pos_x = trans_obj->localPosition()[Trk::locX];
  pers_obj->m_local_pos_y = trans_obj->localPosition()[Trk::locY];

  // Error Matrix
  pers_obj->m_mat_00 = (trans_obj->localCovariance())(0, 0);
  pers_obj->m_mat_01 = (trans_obj->localCovariance())(0, 1);
  pers_obj->m_mat_11 = (trans_obj->localCovariance())(1, 1);

  // Time
  pers_obj->m_time = trans_obj->time();
  pers_obj->m_time_resolution = trans_obj->timeResolution();
  pers_obj->m_time_over_threshold = trans_obj->totList();

  // Identifiers
  pers_obj->m_clus_id = trans_obj->identify().get_compact();

  size_t rdo_size = trans_obj->rdoList().size();
  pers_obj->m_rdo_id_list.resize(rdo_size);
  for (size_t i = 0; i < rdo_size; i++) {
    pers_obj->m_rdo_id_list.at(i) = trans_obj->rdoList().at(i).get_compact();
  }
}

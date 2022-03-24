/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventAthenaPool/src/HGTD_ClusterContainerCnv.cxx
 * @author Noemi Calace <noemi.calace@cern.ch>
 * @date June, 2021
 * @brief
 */

#include "HGTD_ClusterContainerCnv.h"
#include "GaudiKernel/MsgStream.h"

#include <memory>

#include <iostream>

HGTD_ClusterContainerCnv::HGTD_ClusterContainerCnv(ISvcLocator* svcloc)
    : HGTD_ClusterContainerCnvBase(svcloc) {}

HGTD_ClusterContainer* HGTD_ClusterContainerCnv::createTransient() {

  static pool::Guid p1_guid(
      "7B3D57D6-F590-4266-974D-A0807122DA5F"); // with HGTD_Cluster_p1
  ATH_MSG_DEBUG("createTransient(): main converter");

  HGTD_ClusterContainer* p_collection(0);
  if (compareClassGuid(p1_guid)) {
    ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    std::unique_ptr<HGTD_ClusterContainer_p1> p_coll(
        poolReadObject<HGTD_ClusterContainer_p1>());
    p_collection = m_converter_p1.createTransient(p_coll.get(), msg());
  } else {
    throw std::runtime_error(
        "Unsupported persistent version of PlanarClusterContainer");
  }
  return p_collection;
}

HGTD_ClusterContainer_PERS* HGTD_ClusterContainerCnv::createPersistent(
    HGTD_ClusterContainer* transCont) {
  HGTD_ClusterContainer_PERS* pldc_p =
      m_converter_p1.createPersistent(transCont, msg());

  return pldc_p;
}

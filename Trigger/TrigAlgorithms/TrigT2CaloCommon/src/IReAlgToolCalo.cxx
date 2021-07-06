/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     IReAlgToolCalo.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
//
// AUTHOR:   Denis Oliveira Damazio
//
// REFERENCES: This will provide infrastructure for the Tools
//		to perform their algorithms
//
// ********************************************************************

#include "TrigT2CaloCommon/IReAlgToolCalo.h"

/** Constructor */
IReAlgToolCalo::IReAlgToolCalo(const std::string& type, const std::string& name,
                               const IInterface* parent) :
    AthAlgTool(type, name, parent)
{}

StatusCode IReAlgToolCalo::initialize()
{
  ATH_CHECK(detStore()->retrieve(m_larMgr));
  if (!m_dataSvc.empty()) ATH_CHECK(m_dataSvc.retrieve());
  ATH_CHECK(m_geometryTool.retrieve());

  // Calibration object initialization
  if (m_limit.size() != 0) {
    m_calib->initialize(m_limit, m_dimension, m_correction);
  }

  return StatusCode::SUCCESS;
}

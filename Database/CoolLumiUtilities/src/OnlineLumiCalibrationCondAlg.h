// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CoolLumiUtilities/src/OnlineLumiCalibrationCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2018
 * @brief 
 */


#ifndef COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDALG_H
#define COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDALG_H


#include "CoolLumiUtilities/OnlineLumiCalibrationCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


class OnlineLumiCalibrationCondAlg
  : public AthReentrantAlgorithm
{
public:
  OnlineLumiCalibrationCondAlg (const std::string& name, ISvcLocator* svcloc);


  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_calibrationFolderName { this, "CalibrationFolderName", "" };
  SG::WriteCondHandleKey<OnlineLumiCalibrationCondData> m_condDataName {this, "CondDataName", "" };
};


#endif // not COOLLUMIUTILITIES_ONLINELUMICALIBRATIONCONDALG_H


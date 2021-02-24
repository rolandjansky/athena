/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * NAME : CaloNoiseSigmaDiffCondAlg.h
 * PACKAGE : Calorimeter/CaloRec
 *
 * AUTHOR : Denis Oliveira DamaZio
 * PURPOSE : Very simple class to record simple computation on CaloNoise
 *
 * */

#ifndef CALOREC_CALONOISESIGMADIFFCONDALG_H
#define CALOREC_CALONOISESIGMADIFFCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "CaloConditions/CaloNoiseSigmaDiff.h"
#include "CaloConditions/CaloNoise.h"

class CaloNoiseSigmaDiffCondAlg : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  CaloNoiseSigmaDiffCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloNoiseSigmaDiffCondAlg() {};

  /**
   * @brief Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Execute the algorithm.
   * @param ctx Event context.
   */
  virtual StatusCode execute (const EventContext& ctx) const override;

private:
  /// Property: CaloNoise (conditions input).
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","electronicNoise","SG Key of CaloNoise data object"};

  // Output CaloNoiseSigmaDiff
  SG::WriteCondHandleKey<CaloNoiseSigmaDiff> m_noiseSigmaDiff {this, "CaloNoiseSigmaDiff","CaloNoiseSigmaDiff","Sigma Diff for CaloNoise with High and Medium gain"};


};

#endif

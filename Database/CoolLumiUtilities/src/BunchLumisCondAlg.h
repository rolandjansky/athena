// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/BunchLumisCondAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Conditions algorithm to unpack raw luminosity data from COOL.
 */


#ifndef COOLLUMIUTILITIES_BUNCHLUMISCONDALG_H
#define COOLLUMIUTILITIES_BUNCHLUMISCONDALG_H


#include "CoolLumiUtilities/BunchLumisCondData.h"
#include "CoolLumiUtilities/FillParamsCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


namespace coral {
  class Blob;
}


/**
 * @brief Conditions algorithm to unpack raw luminosity data from COOL.
 */
class BunchLumisCondAlg
  : public AthReentrantAlgorithm
{
public:
  /// Forward base class ctor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  /// Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// Algorithm execute method.
  virtual StatusCode execute (const EventContext& ctx) const override;
  virtual bool isReEntrant() const override final { return false; }

private:
  /**
   * @brief Unpack raw luminosity data for one channel.
   * @param blob Packed data to unpack.
   * @param luminousBunches BCIDs of occupied bunches, or an empty
   *                        vector if not available.
   * @param avgRawLumi Average raw luminosity, from the conditions object.
   * @param rawLumiOut[out] Unpacked per-BCID raw luminosities.
   */
  StatusCode unpackLumis (const coral::Blob& blob,
                          const std::vector<unsigned int>& luminousBunches,
                          float avgRawLumi,
                          std::vector<float>& rawLumiOut) const;

  /// Input conditions object.
  SG::ReadCondHandleKey<CondAttrListCollection> m_bunchLumisFolderInputKey
  { this, "BunchLumisFolderInputKey", "", "" };

  /// Fill parameters needed for storage mode 0.
  SG::ReadCondHandleKey<FillParamsCondData> m_fillParamsInputKey
  { this, "FillParamsInputKey", "", "" };

  /// Output conditions object.
  SG::WriteCondHandleKey<BunchLumisCondData> m_bunchLumisOutputKey
  {this, "BunchLumisOutputKey", "BunchLumisCondData", "" };
};


#endif // not COOLLUMIUTILITIES_BUNCHLUMISCONDALG_H

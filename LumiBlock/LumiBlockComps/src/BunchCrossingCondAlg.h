// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file BunchCrossingCondAlg.h
 * @author Walter Lampl <walter.lampl@cern.ch>
 * @date Oct, 2019
 * @brief Conditions algorithm to fill BunchCrossingCondData
 */


#ifndef LUMIBLOCKCOMPS_BUNCHCROSSINGCONDALG_H
#define LUMIBLOCKCOMPS_BUNCHCROSSINGCONDALG_H


#include "CoolLumiUtilities/FillParamsCondData.h"
#include "LumiBlockData/BunchCrossingCondData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"


/**
 * @brief Conditions algorithm to unpack fill parameters from COOL.
 */
class BunchCrossingCondAlg : public AthReentrantAlgorithm {

public:
  /// Forward base class ctor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  typedef BunchCrossingCondData::bunchTrain_t bunchTrain_t;


  /// Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// Algorithm execute method.
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  /// Input conditions object.
  SG::ReadCondHandleKey<AthenaAttributeList> m_fillParamsFolderKey{ this, "FillParamsFolderKey", "/TDAQ/OLC/LHC/FILLPARAMS", "" };
  /// Output conditions object.
  SG::WriteCondHandleKey<BunchCrossingCondData> m_outputKey{this, "OutputKey", "BunchCrossingData", "Key of output CDO" };

  std::vector<bunchTrain_t> findTrains(const std::bitset< BunchCrossingCondData::m_MAX_BCID>& pairsToConsume, const int maxSpacingInTrain, const unsigned minBunchesPerTrain) const;

  Gaudi::Property<unsigned> m_maxBunchSpacing{this, "MaxBunchSpacing",5, "Maximal bunch-spacing to be considered a 'bunch train'"};
  Gaudi::Property<unsigned> m_minBunchesPerTrain{this, "MinBunchesPerTrain",32, "Minimal number of bunches to be considerd a 'bunch train'"};

  Gaudi::Property<bool> m_isRun1{this,"Run1",false,"Assume run-1 database"};
};


#endif

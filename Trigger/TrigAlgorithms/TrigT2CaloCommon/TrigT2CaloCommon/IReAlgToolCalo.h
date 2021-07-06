/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     IReAlgToolCalo.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon

 AUTHOR:   D. Oliveira Damazio based on P. Casado work

 PURPOSE:  Serve as a basis for the TrigT2Calo Tools
           providing some commom framework basis, like
           data access via appropriated pointers and
           ByteStream converter objects. Also, time
           measurement items.
 KNOWTOINHERIT : TrigT2CaloEgamma/EgammaReSamp2Fex,
           EgammaReSamp1Fex, EgammaReEmEnFex, EgammaReHadEnFex
 *******************************************************************/

#ifndef TRIGT2CALOCOMMON_IREALGTOOLCALO_H
#define TRIGT2CALOCOMMON_IREALGTOOLCALO_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "TrigT2CaloCommon/T2Calibration.h"
#include "TrigT2CaloCommon/IT2GeometryTool.h"
#include "xAODTrigCalo/TrigEMCluster.h"

#include <memory>

/** Base Class for Tools used for Egamma and Tau Feature Extraction Algorithms */
class IReAlgToolCalo : public AthAlgTool {
public:
  IReAlgToolCalo(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~IReAlgToolCalo() {}

  virtual StatusCode initialize() override;

  /** @brief This is the execute method for Egamma Algorithms. These
   * interface must be common to allow all tools to be called
   * within T2CaloEgamma::hltExecute as a vector of IReAlgToolCalos.
   * @param[out] ptrigEMCluster : cluster to be filled with FEX results.
   * @param[in] eta/phi-min/max : limits of RoI.
   */
  virtual StatusCode execute(xAOD::TrigEMCluster& ptrigEMCluster, const IRoiDescriptor& roi,
                             const CaloDetDescrElement*& caloDDE,
                             const EventContext& context) const = 0;

protected:
#ifndef NDEBUG
  /** Very useful function to check about the Cluster structure.
  It prints out a grid of cells eta and phi positions and energies.
  Only exist in DEBUG versions of the code. */
  void PrintCluster(const double /*energyFromAlg*/, const int /*nCaloType*/, const int /*nCaloSamp*/,
                    const CaloSampling::CaloSample, const CaloSampling::CaloSample) const
  {}
#endif

  /** Objects will need T2Calibration. So far, a given tool will
  not need more than one of such objects. */
  std::unique_ptr<T2Calibration> m_calib;

  /** Input objects to calibration. This will be changed in such
  a way the the calibration tool will have to know alone about
  such properties. */
  std::vector<float> m_limit;
  std::vector<int> m_dimension;
  std::vector<float> m_correction;

  /** Calorimeter Id Manager for calorimeter part determination (Barrel versus EndCap) */
  const CaloIdManager* m_larMgr = nullptr;

  // Properties
  Gaudi::Property<bool> m_saveCells{this, "SaveCellsInContainer", false,
                                    "Enables saving of the RoI Calorimeter Cells in StoreGate"};

  Gaudi::Property<float> m_cellkeepthr{this, "ThresholdKeepCells", 1e5,
                                       "Threshold to keep cells incontainer"};

  ToolHandle<IT2GeometryTool> m_geometryTool{
      this, "T2GeometryTool", "T2GeometryTool/T2GeometryTool",
      "Tool to check that a cells are contained in a given cluster"};

  ServiceHandle<ITrigCaloDataAccessSvc> m_dataSvc{this, "trigDataAccessMT",
                                                  "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc",
                                                  "Data Access for LVL2 Calo Algorithms in MT"};
};

#endif

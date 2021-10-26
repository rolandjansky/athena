/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOALIGNMENTALGS_CALOSUPERCELLALIGNCONDALG_H
#define CALOALIGNMENTALGS_CALOSUPERCELLALIGNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"

// ------------
#include <memory>

/**
 * @class CaloSuperCellAlignCondAlg
 *
 * @brief Condition Algorithm for making CaloSuperCellDetDescrManager condition object
 *
 **/

class CaloSuperCellAlignCondAlg final : public AthAlgorithm
{
 public:
  using AthAlgorithm::AthAlgorithm;
  virtual ~CaloSuperCellAlignCondAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;};

 private:
  SG::ReadCondHandleKey<CaloDetDescrManager>  m_readCaloMgrKey {this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG key of the resulting CaloDetDescrManager" };

  SG::WriteCondHandleKey<CaloSuperCellDetDescrManager>  m_writeCaloSuperCellMgrKey {this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG key of the resulting CaloDetDescrManager" };

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  ToolHandle<ICaloSuperCellIDTool> m_scidTool {this
      , "CaloSuperCellIDTool"
      , "CaloSuperCellIDTool"
      , "Offline / SuperCell ID mapping tool" };

  // Helper functions
  struct MinMax {
    MinMax() : min(99999), max(-999999) {}
    void add (double x, double dx)
    {
      if (x - dx < min) min = x - dx;
      if (x + dx > max) max = x + dx;
    }
    double min;
    double max;
  };
  
  struct DescrMinMax {
    MinMax eta;
    MinMax phi;
    MinMax r;
    MinMax z;
  };

  void createDescriptors(CaloSuperCellDetDescrManager* mgr);
  void createElements(CaloSuperCellDetDescrManager* mgr);
  StatusCode updateElements(CaloSuperCellDetDescrManager* mgr
			    , const CaloDetDescrManager* cellmgr);
  void updateDescriptors(CaloSuperCellDetDescrManager* mgr
			 , const CaloDetDescrManager* cellmgr);
  void updateDescriptor(CaloDetDescriptor* desc
			, const DescrMinMax& mm
			, const CaloDetDescrManager* cellmgr);
};

#endif

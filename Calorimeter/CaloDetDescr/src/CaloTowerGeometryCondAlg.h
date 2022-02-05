//Dear emacs, this is -*-c++-*-
//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef CALODETDESC_CALOTOWERGEOMETRYCONDALG_H
#define CALODETDESC_CALOTOWERGEOMETRYCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "CaloDetDescr/CaloTowerGeometry.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

class CaloTowerGeometryCondAlg : public AthReentrantAlgorithm {

 public: 
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  virtual ~CaloTowerGeometryCondAlg() = default;

  StatusCode initialize() override final;
  StatusCode execute(const EventContext& ctx) const override final;
  StatusCode finalize() override final {return StatusCode::SUCCESS;}

 private:

  void dump(const CaloTowerGeometry* towerGeo) const;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"}; 
  SG::WriteCondHandleKey<CaloTowerGeometry> m_outputKey{this,"OutputKey","CaloTowerGeometry"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  //Properties:
  Gaudi::Property<unsigned> m_towerEtaBins{this,"TowerEtaBins",100,"Number of pseudorapidity bins in tower grid"};
  Gaudi::Property<double> m_towerEtaMin{this,"TowerEtaMin",-5.0,"Lower boundary of pseudorapidity range"};
  Gaudi::Property<double> m_towerEtaMax {this,"TowerEtaMax", 5.0,"Upper boundary of pseudorapidity range"};
  Gaudi::Property<bool> m_adjustEta {this,"AdjustFCalToTowerEtaBounds", true, "Adjust FCal cells to eta boundaries"};

  Gaudi::Property<unsigned> m_towerPhiBins {this, "TowerPhiBins",64, "Number of azimuthal bins in tower grid"};
  Gaudi::Property<double> m_towerPhiMin {this,"TowerPhiMin", -M_PI, "Lower boundary of azimuthal range"};
  Gaudi::Property<double> m_towerPhiMax {this,"TowerPhiMax", M_PI,  "Upper boundary of azimuthal range"};

  // change only for R&D
  Gaudi::Property<double> m_fcal1Xslice {this,"FCal1NSlicesX",8.0,"Number of X slices for FCal1 cells"};
  Gaudi::Property<double> m_fcal1Yslice {this,"FCal1NSlicesY",8.0,"Number of Y slices for FCal1 cells"};
 
  Gaudi::Property<double> m_fcal2Xslice {this,"FCal2NSlicesX",8.0,"Number of X slices for FCal2 cells"};
  Gaudi::Property<double> m_fcal2Yslice {this,"FCal2NSlicesY",12.0,"Number of Y slices for FCal2 cells"};

  Gaudi::Property<double> m_fcal3Xslice {this, "FCal3NSlicesX",12.0, "Number of X slices for FCal3 cells"};
  Gaudi::Property<double> m_fcal3Yslice {this, "FCal3NSlicesY",12.0, "Number of Y slices for FCal3 cells"};

  Gaudi::Property<std::string> m_towerGridFile{this,"TowerGridFile","","Text file to dump tower-grid"};

  Gaudi::Property<std::string> m_towerMapFile{this,"TowerMapFile","","Text file to dump cell->tower mapping"};

  

};
#endif

/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_FFexEMFinder_H
#define TRIGT1CALOFEXPERF_FFexEMFinder_H

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "Identifier/IdentifierHash.h"
#include "TH1.h"
#include "TH2.h"

#include "AsgTools/AnaToolHandle.h"
#include "FwdCellReader.h"

class FFexEMFinder{

 public:
  struct electron{

    float eta;
    float phi;
    float et;
    float et_full;
    float isolationEt;
    float hadEt;
    int core_nCells;
    int isolation_nCells;
    int had_nCells;
    bool isFCAL;

    float fmax;
    float depthZ;
    float varianceR;
    float varianceZ;
    float normLongMom;
    float normLatMom;

    float fractionMaxEnergyCell;
    float showerDepthZ;
    float showerVarianceR;
    float showerVarianceZ;
    float showerCentreFirstLayer_eta;
    float showerCentreFirstLayer_phi;
    float centreEta;
    float centrePhi;
    std::vector<float> emecCoreEt;
    std::vector<float> fcalCoreEt;

    bool operator < (const electron& elec) const
    {
      return (et < elec.et);
    }
    ~electron(){}
  };



  struct Seed{

    std::vector<float> eta;
    std::vector<std::vector<float>> phi;
    std::vector<std::vector<float>> et;
    std::vector<std::vector<bool>> local_max;
    std::vector<std::vector<float>> et_full;
    std::vector<std::vector<float>> isolationEt;
    std::vector<std::vector<float>> hadEt;
    std::vector<std::vector<int>> core_nCells;
    std::vector<std::vector<int>> isolation_nCells;
    std::vector<std::vector<int>> had_nCells;


    std::vector<std::vector<float>> fmax;
    std::vector<std::vector<float>> depthZ;
    std::vector<std::vector<float>> varianceR;
    std::vector<std::vector<float>> varianceZ;
    std::vector<std::vector<float>> normLongMom;
    std::vector<std::vector<float>> normLatMom;

    std::vector<std::vector<float>> showerDepthZ;
    std::vector<std::vector<float>> showerVarianceR;
    std::vector<std::vector<float>> showerVarianceZ;
    std::vector<std::vector<float>> showerCentreFirstLayer_eta;
    std::vector<std::vector<float>> showerCentreFirstLayer_phi;
    std::vector<std::vector<float>> fractionMaxEnergyCell;

    std::vector<std::vector<float>> centreEta;

    std::vector<std::vector<float>> centrePhi;
    std::vector<std::vector<std::vector<float>>> emecCoreEt;

    ~Seed(){}

  };

  struct SeedXY{

    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> z;

    std::vector<bool> local_max;
    std::vector<std::vector<int>> locMaxSeedIndices_top;
    std::vector<std::vector<int>> locMaxSeedIndices_left;
    std::vector<std::vector<int>> locMaxSeedIndices_right;
    std::vector<std::vector<int>> locMaxSeedIndices_bottom;
    std::vector<float> et;
    std::vector<float> et_full;
    std::vector<float> isolationEt;
    std::vector<float> hadEt;
    std::vector<int> core_nCells;
    std::vector<int> isolation_nCells;
    std::vector<int> had_nCells;

    std::vector<float> showerDepthZ;
    std::vector<float> showerVarianceR;
    std::vector<float> showerVarianceZ;
    std::vector<float> showerCentreFirstLayer_eta;
    std::vector<float> showerCentreFirstLayer_phi;
    std::vector<float> fractionMaxEnergyCell;

    std::vector<std::vector<float>> fcalCoreEt;

    ~SeedXY(){}

  };

  static StatusCode SeedGrid(std::vector<CaloCell*> forwardCells, std::shared_ptr<FFexEMFinder::Seed>seeds, std::shared_ptr<FFexEMFinder::SeedXY>seedsXY);
  static StatusCode SeedFinding(std::vector<CaloCell*> forwardCells, std::shared_ptr<FFexEMFinder::Seed>seeds, std::shared_ptr<FFexEMFinder::SeedXY>seedsXY, float range);

  static StatusCode BuildElectron(std::shared_ptr<FFexEMFinder::Seed>seeds, std::shared_ptr<FFexEMFinder::SeedXY>seedsXY, std::vector<std::shared_ptr<FFexEMFinder::electron>> &eles);

};
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIDIGIANALYSIS_H
#define SIDIGIANALYSIS_H

#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

class TTree;

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
 }
class SCT_ID;
class PixelID;
//
//    Analyse pixel and Strip RDO (digitized hits)
//
class SiDigiAnalysis:public AthAlgorithm {
public:
    SiDigiAnalysis(const std::string &name, ISvcLocator *pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    void clear();
private:
    ServiceHandle<StoreGateSvc> m_storeGate;
    ServiceHandle<StoreGateSvc> m_detStore;
    ServiceHandle<ITHistSvc> m_histSvc;
    const PixelID *m_pixelIdHelper;
    const SCT_ID *m_sctIdHelper;
    const InDetDD::PixelDetectorManager *m_pixelManager;
    const InDetDD::SCT_DetectorManager *m_SCT_Manager;

    TTree *m_outputTree;

    bool m_doHits;
    bool m_doRDO;
    bool m_doTruth;
    std::string m_pixelRDO_ContainerName;
    std::string m_stripRDO_ContainerName;
    std::string m_truthEventContainerName;

    int m_nPixelRDO;
    std::vector<double> m_pixelRDO_Layer;
    std::vector<double> m_pixelRDO_BarrelEndcap;
    std::vector<double> m_pixelRDO_Eta;
    std::vector<double> m_pixelRDO_Phi;
    std::vector<double> m_pixelRDO_GlobalX;
    std::vector<double> m_pixelRDO_GlobalY;
    std::vector<double> m_pixelRDO_GlobalZ;
    std::vector<double> m_pixelRDO_LocalX;
    std::vector<double> m_pixelRDO_LocalY;
    std::vector<double> m_pixelRDO_LocalZ;
    std::vector<double> m_pixelRDO_ToT;
    std::vector<double> m_pixelRDO_BCID;

    int m_nStripRDO;
    std::vector<int> m_stripRDO_BarrelEndcap;
    std::vector<int> m_stripRDO_Layer;
    std::vector<int> m_stripRDO_Phi;
    std::vector<int> m_stripRDO_Eta;
    std::vector<int> m_stripRDO_Side;
    std::vector<int> m_stripRDO_Row;
    std::vector<int> m_stripRDO_Strip;

    std::vector<int> m_stripRDO_size;

    std::vector<double> m_stripRDO_LocalX;
    std::vector<double> m_stripRDO_LocalY;
    std::vector<double> m_stripRDO_LocalZ;

    std::vector<double> m_stripRDO_x0;
    std::vector<double> m_stripRDO_y0;
    std::vector<double> m_stripRDO_z0;

    std::vector<double> m_stripRDO_x1;
    std::vector<double> m_stripRDO_y1;
    std::vector<double> m_stripRDO_z1;
   
    int m_nTruth;   
    std::vector<double> m_truthBarcode;
    std::vector<double> m_truthPdgId;
    std::vector<double> m_truthStatus;
    std::vector<double> m_truthPx;
    std::vector<double> m_truthPy;
    std::vector<double> m_truthPz;
    std::vector<double> m_truthVx;
    std::vector<double> m_truthVy;
    std::vector<double> m_truthVz;
};


#endif // SIDIGIANALYSIS

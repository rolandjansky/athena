/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

class TTree;

//
//    Analyse G4 hits
//
class SiHitAnalysis:public AthAlgorithm {
public:
    SiHitAnalysis(const std::string &name, ISvcLocator *pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    void clear();
private:
    ServiceHandle<StoreGateSvc> m_storeGate;
    ServiceHandle<ITHistSvc> m_histSvc;
    TTree *m_outputTree;

    bool m_doHits;
    std::string m_pixelSimHitContainerName; 
    std::string m_StripSimHitContainerName; 

    int m_numberPixelHits;
    std::vector<double> m_pixelHitBarrelEC;
    std::vector<double> m_pixelHitLayer;
    std::vector<double> m_pixelHitEta;
    std::vector<double> m_pixelHitPhi;
    std::vector<double> m_pixelHitLocalX;
    std::vector<double> m_pixelHitLocalY;
    std::vector<double> m_pixelHitLocalZ;
    std::vector<double> m_pixelHitEnergy;
    std::vector<double> m_pixelHitTime;
   
    int m_numberStripHits;
    std::vector<double> m_StripHitBarrelEC;
    std::vector<double> m_StripHitLayer;
    std::vector<double> m_StripHitEta;
    std::vector<double> m_StripHitPhi;
    std::vector<double> m_StripHitSide;
    std::vector<double> m_StripHitLocalX;
    std::vector<double> m_StripHitLocalY;
    std::vector<double> m_StripHitLocalZ;
    std::vector<double> m_StripHitEnergy;
    std::vector<double> m_StripHitTime;
};

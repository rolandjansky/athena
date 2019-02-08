/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorSimPlanarTool.h
//   Header file for class SensorSimPlanarTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters
//
// -numberOfSteps	Geant4:number of steps for PixelPlanar
// -numberOfCharges	Geant4:number of charges for PixelPlanar
//
//////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_SensorSimPlanarTool_H
#define PIXELDIGITIZATION_SensorSimPlanarTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandle.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

#include "SensorSimTool.h"
#include "RadDamageUtil.h"

namespace RadDam{

class SensorSimPlanarTool : public SensorSimTool {

  public:
    SensorSimPlanarTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~SensorSimPlanarTool();

    virtual StatusCode induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions);  
    //Apply slim edge inefficiencies for IBL sensors
    virtual StatusCode applySlimEdges( double &energyPerStep, double &eta_drifted);

    //Maps
    std::map<std::pair<int, int>, TH3F* > ramoPotentialMap;
    std::map<std::pair<int, int>, TH1F*> eFieldMap;
    std::map<std::pair<int, int>, TH2F*> distanceMap_e;
    std::map<std::pair<int, int>, TH2F*> distanceMap_h;
    std::map<std::pair<int, int>, TH1F*> timeMap_e;
    std::map<std::pair<int, int>, TH1F*> timeMap_h;
    std::map<std::pair<int, int>, TH2F*> lorentzMap_e;
    std::map<std::pair<int, int>, TH2F*> lorentzMap_h;

    ToolHandle<RadDamageUtil>                     m_radDamageUtil;
  
  private:
    SensorSimPlanarTool();

    int    m_numberOfSteps;
    int    m_numberOfCharges;  
    double m_diffusionConstant;

    bool m_doRadDamage;
    double m_fluence; //eventually, this should be pulled from the conditions.

    std::vector<double> m_fluence_layers;
    std::map<std::pair<int, int>, double> m_fluence_layersMaps;
    double m_trappingTimeElectrons;
    double m_trappingTimeHoles;
    ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retreive Lorentz angle"};
};

}
#endif // PIXELDIGITIZATION_SensorSimPlanarTool_H

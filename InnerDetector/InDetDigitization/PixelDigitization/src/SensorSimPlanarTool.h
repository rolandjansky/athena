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
#include "SensorSimTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

class SensorSimPlanarTool : public SensorSimTool {

  public:
    SensorSimPlanarTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~SensorSimPlanarTool();

    //TODO: change name to induceCharge
    virtual StatusCode induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions, CLHEP::HepRandomEngine *rndmEngine);  

  private:
    SensorSimPlanarTool();

    ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "PixelLorentzAngleTool", "Tool to retreive Lorentz angle"};

    int    m_numberOfSteps;
    int    m_numberOfCharges;  
    double m_diffusionConstant;
};

#endif // PIXELDIGITIZATION_SensorSimPlanarTool_H

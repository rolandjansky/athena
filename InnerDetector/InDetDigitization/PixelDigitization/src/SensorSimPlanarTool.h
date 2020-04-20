/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelDigitization/SensorSimPlanarTool.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date January, 2020
 * @brief Planar pixel sensor simulation
 */

#ifndef PIXELDIGITIZATION_SensorSimPlanarTool_H
#define PIXELDIGITIZATION_SensorSimPlanarTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SensorSimTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "RadDamageUtil.h"

class SensorSimPlanarTool : public SensorSimTool {

  public:
    SensorSimPlanarTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual ~SensorSimPlanarTool();

    virtual StatusCode induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, 
        const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, 
        std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions, CLHEP::HepRandomEngine *rndmEngine) override;

    //Apply slim edge inefficiencies for IBL sensors
    StatusCode applyIBLSlimEdges( double &energyPerStep, double &eta_drifted);

  private:
    SensorSimPlanarTool();

    // Map for radiation damage simulation
    std::map<std::pair<int, int>, TH3F*> m_ramoPotentialMap;
    std::map<std::pair<int, int>, TH1F*> m_eFieldMap;
    std::map<std::pair<int, int>, TH2F*> m_distanceMap_e;
    std::map<std::pair<int, int>, TH2F*> m_distanceMap_h;
    std::map<std::pair<int, int>, TH1F*> m_timeMap_e;
    std::map<std::pair<int, int>, TH1F*> m_timeMap_h;
    std::map<std::pair<int, int>, TH2F*> m_lorentzMap_e;
    std::map<std::pair<int, int>, TH2F*> m_lorentzMap_h;

    std::vector<double> m_fluence_layers,m_voltage_layers; //merging information from m_fluence* and m_voltage*
    std::map<std::pair<int, int>, double> m_fluence_layersMaps;

    Gaudi::Property<int> m_numberOfSteps
    {this, "numberOfSteps", 50, "Geant4:number of steps for PixelPlanar"};

    Gaudi::Property<double> m_diffusionConstant
    {this, "diffusionConstant", 0.0, "Geant4:Diffusion Constant for PixelPlanar"};

    Gaudi::Property<bool> m_doRadDamage
    {this, "doRadDamage", false, "doRadDmaage bool: should be flag"};

    Gaudi::Property<double> m_trappingTimeElectrons
    {this, "trappingTimeElectrons", 0.0, "Characteristic time till electron is trapped [ns]"};

    Gaudi::Property<double> m_trappingTimeHoles
    {this, "trappingTimeHoles", 0.0, "Characteristic time till hole is trapped [ns]"};

    Gaudi::Property<int> m_fluence
    {this, "fluence", 0, "this is the fluence benchmark, 0-6. 0 is unirradiated, 1 is start of Run 2, 5 is end of 2018 and 6 is projected end of 2018"};

    Gaudi::Property<int> m_fluenceB
    {this, "fluenceB", -1, "fluence detector has recieved in neqcm2 at the B layer."};

    Gaudi::Property<int> m_fluence1
    {this, "fluence1", -1, "fluence detector has recieved in neqcm2 at the layer 1."};

    Gaudi::Property<int> m_fluence2
    {this, "fluence2", -1, "fluence detector has recieved in neqcm2 at the layer 2."};

    Gaudi::Property<double> m_voltage
    {this, "voltage", -1.0, "this is the bias voltage applied to the IBL - if not set use values from applied at benchmark points according to fluence"};

    Gaudi::Property<double> m_voltageB
    {this, "voltageB", -1.0, "bias voltage applied to the B layer."};

    Gaudi::Property<double> m_voltage1
    {this, "voltage1", -1.0, "bias voltage applied to the layer 1."};

    Gaudi::Property<double> m_voltage2
    {this, "voltage2", -1.0, "bias voltage applied to the layer 2."};

    ToolHandle<RadDamageUtil> m_radDamageUtil
    {this, "RadDamageUtil", "RadDamageUtil", "Rad Damage utility"};

    ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool
    {this, "LorentzAngleTool", "PixelLorentzAngleTool", "Tool to retreive Lorentz angle"};

};

#endif // PIXELDIGITIZATION_SensorSimPlanarTool_H

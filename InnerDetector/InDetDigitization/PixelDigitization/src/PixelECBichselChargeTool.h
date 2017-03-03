/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelECBichselChargeTool.h
//   Header file for class PixelECBichselChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters
//
// -numberOfSteps	Geant4:number of steps for PixelEC
// -numberOfCharges	Geant4:number of charges for PixelEC
// -diffusionConstant	Geant4:Diffusion Constant for PixelEC
// -doBichsel : re-do charge deposition following Bichsel model
// -BichselMomentumCut : minimum MOMENTUM for particle to be re-simulated through Bichsel Model
// -BichselSimTool : tool that implements Bichsel model
//
//////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_PixelECBichselChargeTool_H
#define PIXELDIGITIZATION_PixelECBichselChargeTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SubChargesTool.h"

#include "BichselSimTool.h"

class PixelECBichselChargeTool : public SubChargesTool {

  public:
    PixelECBichselChargeTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~PixelECBichselChargeTool();

    virtual StatusCode charge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module);  

  private:
    PixelECBichselChargeTool();

    int    m_numberOfSteps;
    int    m_numberOfCharges;  
    double m_diffusionConstant;

    bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model ?
    //double m_doBichselMomentumCut;                     // minimum MOMENTUM for particle to be re-simulated through Bichsel Model. Unit in MeV
    double m_doBichselBetaGammaCut;                      // replace momentum cut
    bool   m_doPU;                                       // whether apply Bichsel model on PU
    ToolHandle<BichselSimTool> m_BichselSimTool;         // if yes, you need to load related tool here

    void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
};

#endif // PIXELDIGITIZATION_PixelECBichselChargeTool_H

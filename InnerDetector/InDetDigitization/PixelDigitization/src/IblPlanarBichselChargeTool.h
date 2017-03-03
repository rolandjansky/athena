/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IblPlanarBichselChargeTool.h
//   Header file for class IblPlanarBichselChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters
//
// -numberOfSteps	Geant4:number of steps for IblPlanar
// -numberOfCharges	Geant4:number of charges for IblPlanar
// -diffusionConstant	Geant4:Diffusion Constant for IblPlanar
//
//////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_IblPlanarBichselChargeTool_H
#define PIXELDIGITIZATION_IblPlanarBichselChargeTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SubChargesTool.h"

#include "BichselSimTool.h"

class IblPlanarBichselChargeTool : public SubChargesTool {

  public:
    IblPlanarBichselChargeTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~IblPlanarBichselChargeTool();

    virtual StatusCode charge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module);  

  private:
    IblPlanarBichselChargeTool();

    int m_numberOfSteps;
    int m_numberOfCharges;  
    double m_diffusionConstant;

    bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model
    double m_doBichselBetaGammaCut;                      // Momentum cut on beta-gamma
    bool   m_doDeltaRay;                                 // implement Bichsel Model into delta-ray, which does not have truth particle link. 
    // We will assume all delta-ray is electron, with all energy deposited in silicon layer. So the 4-momentum can be reconstructed using energy and direction
    bool   m_doPU;                                       // whether we apply Bichsel model on PU
    ToolHandle<BichselSimTool> m_BichselSimTool;         // if yes, you need to load related tool here

    void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
};

#endif // PIXELDIGITIZATION_IblPlanarBichselChargeTool_H

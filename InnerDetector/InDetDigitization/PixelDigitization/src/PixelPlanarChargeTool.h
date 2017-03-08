/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelPlanarChargeTool.h
//   Header file for class PixelPlanarChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters
//
// -numberOfSteps	Geant4:number of steps for PixelPlanar
// -numberOfCharges	Geant4:number of charges for PixelPlanar
// -diffusionConstant	Geant4:Diffusion Constant for PixelPlanar
// -doBichsel : re-do charge deposition following Bichsel model
// -BichselMomentumCut : minimum MOMENTUM for particle to be re-simulated through Bichsel Model
// -BichselSimTool : tool that implements Bichsel model
//
//////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_PixelPlanarChargeTool_H
#define PIXELDIGITIZATION_PixelPlanarChargeTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SubChargesTool.h"

#include "BichselSimTool.h"

class PixelPlanarChargeTool : public SubChargesTool {

  public:
    PixelPlanarChargeTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~PixelPlanarChargeTool();

    virtual StatusCode charge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module);  

  private:
    PixelPlanarChargeTool();

    int    m_numberOfSteps;
    int    m_numberOfCharges;  
    double m_diffusionConstant;

    bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model ?
    //double m_doBichselMomentumCut;                       // minimum MOMENTUM for particle to be re-simulated through Bichsel Model. Unit in MeV
    double m_doBichselBetaGammaCut;                      // replace momentum cut
    bool   m_doDeltaRay;                                 // implement Bichsel Model into delta-ray, which does not have truth particle link. 
    // We will assume all delta-ray is electron, with all energy deposited in silicon layer. So the 4-momentum can be reconstructed using energy and direction
    bool   m_doPU;                                       // Whether we apply Bichsel model on non-HS particles
    ToolHandle<BichselSimTool> m_BichselSimTool;         // if yes, you need to load related tool here

    void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
};

#endif // PIXELDIGITIZATION_PixelPlanarChargeTool_H

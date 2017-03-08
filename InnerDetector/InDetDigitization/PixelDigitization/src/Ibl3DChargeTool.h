/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_Ibl3DChargeTool_H
#define PIXELDIGITIZATION_Ibl3DChargeTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SubChargesTool.h"
#include "IChargeCollProbSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"

#include "BichselSimTool.h"

class Ibl3DChargeTool : public SubChargesTool {

  public:
    Ibl3DChargeTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~Ibl3DChargeTool();

    virtual StatusCode charge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module);  

  private:
    Ibl3DChargeTool();

    int m_numberOfSteps;
    bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model
    double m_doBichselBetaGammaCut;                      // Momentum cut on beta-gamma
    bool   m_doDeltaRay;                                 // implement Bichsel Model into delta-ray, which does not have truth particle link. 
    // We will assume all delta-ray is electron, with all energy deposited in silicon layer. So the 4-momentum can be reconstructed using energy and direction
    bool   m_doPU;                                       // whether we do Bichsel model for PU
    ToolHandle<BichselSimTool> m_BichselSimTool;         // if yes, you need to load related tool here
    ServiceHandle<IChargeCollProbSvc> m_chargeCollSvc;
};

#endif // PIXELDIGITIZATION_Ibl3DChargeTool_H

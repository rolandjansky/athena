/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief implementation of photon shot variable calculation 
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch>
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 */

#ifndef TAUSHOTVARIABLEHELPERS_H
#define TAUSHOTVARIABLEHELPERS_H

#include "xAODPFlow/PFO.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/IHadronicCalibrationTool.h"

namespace TauShotVariableHelpers {

    /** @brief get cell block with (currently) 5x2 cells in correct order for variable calculations */
    std::vector<std::vector<const CaloCell*> > getCellBlock(xAOD::PFO* shot,
                                                            const CaloCell_ID* calo_id);

    /** @brief mean eta, used by other functions */
    float mean_eta(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                   ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief mean pt, used by other functions */ 
    float mean_pt(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                  ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief pt in windows */
    float ptWindow(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                   int /*windowSize*/, 
                   ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief ws5 variable (egamma) */
    float ws5(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                          ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief standard deviation in eta WRT mean */
    float sdevEta_WRTmean(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                          ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief standard deviation in eta WRT mode */
    float sdevEta_WRTmode(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                          ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief normalized standard deviation in pt */
    float sdevPt(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                             ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief pT diff b/w lead and sub-lead cell */
    float deltaPt12_min(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                        ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief Fside variable (egamma) */
    float Fside(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                int /*largerWindow*/, 
                int /*smallerWindow*/, 
                ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief similar than Fside but in unit of eta instead of number of cells */
    float fracSide(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                   int /*largerWindow*/, 
                   int /*smallerWindow*/, 
                   ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);

    /** @brief pt window fraction */
    float ptWindowFrac(std::vector<std::vector<const CaloCell*> > /*shotCells*/, 
                       int /*largerWindow*/, 
                       int /*smallerWindow*/, 
                       ToolHandle<IHadronicCalibrationTool>& /*caloWeightTool*/);
}

#endif // TAUSHOTVARIABLEHELPERS_H


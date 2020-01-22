/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0CREATEROI_H
#define	TAUREC_TAUPI0CREATEROI_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "tauRecTools/TauRecToolBase.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTau/TauJet.h"

/**
 * @brief Create ROIs for the Pi0 finder.
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0CreateROI : public TauRecToolBase {
public:
    TauPi0CreateROI(const std::string& name);
    ASG_TOOL_CLASS2(TauPi0CreateROI, TauRecToolBase, ITauToolBase);
    virtual ~TauPi0CreateROI();

    virtual StatusCode initialize() override;
    virtual StatusCode eventInitialize() override;
    virtual StatusCode executePi0CreateROI(xAOD::TauJet& pTau, CaloCellContainer& Pi0CellContainer) override;
    virtual StatusCode eventFinalize() override;
    virtual StatusCode finalize() override;

private:

    /** @brief store cell in output container */ 
    void storeCell(const CaloCell* /* cell*/, CaloCellContainer& cellContainer);

    /** @brief hash map in order to keep track, which cells have been added to output cell container*/
    std::vector<CaloCell*> m_addedCellsMap;

    SG::ReadHandleKey<CaloCellContainer> m_caloCellInputContainer{this,"Key_caloCellInputContainer", "AllCalo", "input vertex container key"};

};

#endif	/* TAUPI0CREATEROI_H */


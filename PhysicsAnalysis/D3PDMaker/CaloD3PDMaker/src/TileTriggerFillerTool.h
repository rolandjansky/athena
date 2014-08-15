/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTriggerFillerTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on January 13, 2012, 11:20 AM
 */

#ifndef TILETRIGGERFILLERTOOL_H
#define	TILETRIGGERFILLERTOOL_H

#include "StoreGate/StoreGateSvc.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileTriggerContainer.h"
#include "TileEvent/TileTrigger.h"

class CaloLVL1_ID;

namespace D3PD{

class TileTriggerFillerTool : public BlockFillerTool<TileTrigger> {
public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileTriggerFillerTool(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    

    // Standard Gaudi initialize method.
    virtual StatusCode initialize();
    
    //Book variables for this block.
    virtual StatusCode book();

    /**
     * @brief Fill one block --- type-safe version.
     * @param p The input object.
     * 
     * This is called once per object.  The caller
     * is responsible for arranging that all the pointers for booked variables
     * are set appropriately upon entry.
     */
    virtual StatusCode fill (const TileTrigger& p);
    
private:
    /** A data member to retain a connection to the Detector Store. */
    StoreGateSvc* m_detStore;
       
    /** 
     * A data member to retain a pointer to the Tile
     * Identifier service.*/
    const CaloLVL1_ID* m_TT_ID;
    
    std::vector<int>* m_maxTowerSide;
    std::vector<float>* m_maxTowerEta;
    std::vector<int>* m_maxTowerPhi;
    std::vector<float>* m_maxTowerPulse;

};

}


#endif	/* TILETRIGGERFILLERTOOL_H */


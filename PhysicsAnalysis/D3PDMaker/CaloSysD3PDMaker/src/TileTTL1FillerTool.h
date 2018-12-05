/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTTL1FillerTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on January 13, 2012, 10:27 AM
 */

#ifndef TILETTL1FILLERTOOL_H
#define	TILETTL1FILLERTOOL_H

#include "StoreGate/StoreGateSvc.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileTTL1Container.h"
#include "TileEvent/TileTTL1.h"

class CaloLVL1_ID;
class TileTBID;

namespace D3PD{

class TileTTL1FillerTool : public BlockFillerTool<TileTTL1> {
public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileTTL1FillerTool(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    
    /// Standard Gaudi initialize method
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
    virtual StatusCode fill (const TileTTL1& p);
    
private:
    /** A data member to retain a connection to the Detector Store. */
    StoreGateSvc* m_detStore;
    
    /** 
     * A data member to retain a pointer to the Tile Test Beam
     * Identifier service.  The MBTS identifiers were added after the
     * Other tile identifiers.  To avoid confusion and extra work they
     * were added to the Test Beam (TB) Identifier service. */ 
    const TileTBID*    m_tileTBID;
    
    /** 
     * A data member to retain a pointer to the Tile
     * Identifier service.*/
    const CaloLVL1_ID* m_TT_ID;
    
    int* m_side;
    int* m_eta;
    int* m_phi;
    std::vector<float>* m_samples;

};

}


#endif	/* TILETTL1FILLERTOOL_H */


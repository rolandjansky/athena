/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileHitFillerTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on January 13, 2012, 1:11 PM
 */

#ifndef TILEHITFILLERTOOL_H
#define	TILEHITFILLERTOOL_H

#include "StoreGate/StoreGateSvc.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileSimEvent/TileHit.h"

class TileID;

namespace D3PD{

class TileHitFillerTool : public BlockFillerTool<TileHit> {
public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileHitFillerTool(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    
    /// Standard Gaudi initialize method.
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
    virtual StatusCode fill (const TileHit& p);
    
private:
    /** A data member to retain a connection to the Detector Store. */
    StoreGateSvc* m_detStore;
    
    const TileID* m_tileID;
    
    std::vector<float>* m_energy;
    std::vector<float>* m_time;
    std::vector<int>* m_pmt;
    std::vector<int>* m_detector;
    std::vector<int>* m_side;
    std::vector<int>* m_sample;
    std::vector<int>* m_eta;
    std::vector<int>* m_phi;
    

};

}

#endif	/* TILEHITFILLERTOOL_H */


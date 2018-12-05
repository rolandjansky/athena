/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileMuFillerTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on January 12, 2012, 3:59 PM
 */

#ifndef TILEMUFILLERTOOL_H
#define	TILEMUFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileMuContainer.h"
#include "TileEvent/TileMu.h"

namespace D3PD{

class TileMuFillerTool : public BlockFillerTool<TileMu> {
public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileMuFillerTool(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    
    virtual ~TileMuFillerTool();
    
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
    virtual StatusCode fill (const TileMu& p);
    
private:
    float* m_eta;
    float* m_phi;
    float* m_energy0;
    float* m_energy1;
    float* m_energy2;
    float* m_energy3;
    unsigned int* m_quality;


};

}


#endif	/* TILEMUFILLERTOOL_H */


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileL2FillerTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on January 12, 2012, 4:50 PM
 */

#ifndef TILEL2FILLERTOOL_H
#define	TILEL2FILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TileEvent/TileL2Container.h"
#include "TileEvent/TileL2.h"

namespace D3PD{

class TileL2FillerTool : public BlockFillerTool<TileL2> {
public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileL2FillerTool(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    
    virtual ~TileL2FillerTool();
    
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
    virtual StatusCode fill (const TileL2& p);
    
private:
    bool m_saveL2;
    bool m_saveMuROD;
    
    //TileL2
    float* m_Ex;
    float* m_Ey;
    float* m_Ez;
    float* m_sumEt;
    float* m_sumEz;
    float* m_sumE;
    //TileMuROD
    std::vector<float>* m_eta;
    std::vector<float>* m_phi;
    std::vector<float>* m_energy0;
    std::vector<float>* m_energy1;
    std::vector<float>* m_energy2;
    std::vector<float>* m_energy3;
    std::vector<unsigned int>* m_quality;


};

}


#endif	/* TILEL2FILLERTOOL_H */


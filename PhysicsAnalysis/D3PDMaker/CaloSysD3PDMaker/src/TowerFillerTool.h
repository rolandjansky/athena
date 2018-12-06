/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TowerFillerTool.h
 * Author: stephen
 *
 * Created on August 26, 2011, 1:36 PM
 */

#ifndef TOWERFILLERTOOL_H
#define	TOWERFILLERTOOL_H

#include <vector>

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "CaloEvent/CaloTower.h"
#include "GaudiKernel/ToolHandle.h"

namespace D3PD{
    
class TowerFillerTool : public BlockFillerTool<CaloTower> {
public:
    /*
     @brief Standard Gaudi tool constructor.
     @param type The name of the tool type.
     @param name The tool name.
     @param parent The tool's Gaudi parent.*/
    TowerFillerTool(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    //Book variables for this block.
    virtual StatusCode book();

    /*
     @brief Fill one block --- type-safe version.
     @param p The input object.

     This is called once per object.  The caller
     is responsible for arranging that all the pointers for booked variables
     are set appropriately upon entry.*/
    virtual StatusCode fill (const CaloTower& p);


private:
    bool m_SaveConstituents;
    
    //vector containing the weight for each CaloCell making up the tower
    std::vector<float>* m_cellWeights;
    //the number of constituent calorimeter cells
    unsigned int* m_nCaloCells; 
    
    unsigned int* m_statWord;
    
    //The tower pT
    float* m_pT;
    //The tower eta
    float* m_eta;
    //The tower phi
    float* m_phi;
    //The tower energy
    float* m_E;
    


};

}
#endif	/* TOWERFILLERTOOL_H */


// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSysD3PDMaker/src/CaloInfoFilterTool.h
 * @author Hong Ma 
 * @date March 2010
 * @brief Block filler tool for energy sum and hits in sub-detectors
 */

#ifndef CALOSYSD3PDMAKER_CALOINFOFITERTOOL_H
#define CALOSYSD3PDMAKER_CALOINFOFITERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class CaloCellContainer;
class StoreGateSvc; 
class LArEM_ID; 
class LArHEC_ID; 
class LArFCAL_ID; 
class TileID; 
class LArOnlineID;

namespace D3PD {


/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class CaloInfoFillerTool
  : public BlockFillerTool<CaloCellContainer>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CaloInfoFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const CaloCellContainer& p);


private:




    int*   m_Nh_Calo; 
    float*  m_Eh_Calo;  

    int*   m_Nh_EM; 
    float*  m_Eh_EM;

    int*   m_Nh_HAD; 
    float*  m_Eh_HAD;

    int*   m_Nh_PresB; 
    std::vector<float> * m_Eh_PresB;

    int*   m_Nh_EMB; 
    std::vector<float> *  m_Eh_EMB;

    int*   m_Nh_EMEC; 
    std::vector<float> * m_Eh_EMEC;

    int*   m_Nh_Tile; 
    std::vector<float> * m_Eh_Tile;

    int*   m_Nh_TileGap; 
    std::vector<float> * m_Eh_TileGap;

    int*   m_Nh_Scint; 
    std::vector<float> * m_Eh_Scint;

    int*   m_Nh_HEC; 
    std::vector<float>  * m_Eh_HEC;
 
    int*   m_Nh_FCal; 
    std::vector<float> * m_Eh_FCal;

    int*   m_Nh_PresE; 
    std::vector<float> * m_Eh_PresE;

    // property
    int m_posneg ; 
    bool m_doEt ; 

};


} // namespace D3PD


#endif // not CALOSYSD3PDMAKER_CALOINFOFITERTOOL_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/TileHitInfoFilterTool.h
 * @author Sanya Solodkov
 * @date March 2014
 * @brief Block filler tool for energy sum and hits in sub-detectors
 */

#ifndef MISCD3PDMAKERTILEHITINFOFITERTOOL_H
#define MISCD3PDMAKERTILEHITINFOFITERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TileSimEvent/TileHitVector.h"


namespace D3PD {


/**
 * @brief Block filler tool for total TileHit energy.
 */
class TileHitInfoFillerTool
  : public BlockFillerTool<TileHitVector>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TileHitInfoFillerTool (const std::string& type,
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
  virtual StatusCode fill (const TileHitVector& p);


private:

    float* m_energy;
    float* m_eInTime;
    float* m_eBefore;
    float* m_eAfter;
    float* m_eOutTime;

    float* m_time;
    float* m_tInTime;
    float* m_tBefore;
    float* m_tAfter;
    float* m_tOutTime;

    int* m_nHit;
    int* m_nHitInTime;
    int* m_nHitBefore;
    int* m_nHitAfter;
    int* m_nHitOutTime;

    int* m_nChan;
    int* m_nChanInTime;
    int* m_nChanBefore;
    int* m_nChanAfter;
    int* m_nChanOutTime;

    // property
    float m_timeMin; 
    float m_timeMax; 
    float m_timeOut; 

};


} // namespace D3PD


#endif // 

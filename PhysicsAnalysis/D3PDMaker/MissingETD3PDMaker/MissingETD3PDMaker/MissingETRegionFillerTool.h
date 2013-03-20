// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETRegionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for Missing ET objects.
 */


#ifndef EVENTCOMMOND3PPDMAKER_MISSINGETREGIONFILLERTOOL_H
#define EVENTCOMMOND3PPDMAKER_MISSINGETREGIONFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class MissingET;


namespace D3PD {


/**
 * @brief Block filler tool for Missing ET objects.
 */
class MissingETRegionFillerTool
  : public BlockFillerTool<MissingET>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETRegionFillerTool (const std::string& type,
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
  virtual StatusCode fill (const MissingET& p);


private:
  float* m_etSumCentralReg;
  float* m_etSumEndcapReg;
  float* m_etSumForwardReg;
  

  float* m_exCentralReg;
  float* m_exEndcapReg;
  float* m_exForwardReg;

  float* m_eyCentralReg;
  float* m_eyEndcapReg;
  float* m_eyForwardReg;

  float* m_ePhiCentralReg;
  float* m_ePhiEndcapReg;
  float* m_ePhiForwardReg;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PPDMAKER_MISSINGETREGIONFILLERTOOL_H

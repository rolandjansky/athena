// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LucidPMTHitFillerTool.h 351806 2011-03-15 10:05:36Z krasznaa $
/**
 * @file BackgroundD3PDMaker/src/LucidPMTHitFillerTool.h
 * @author Mark Tibbetts <mark.james.tibbetts@cern.ch>
 * @date Feb, 2011
 * @brief Block filler tool for LUCID PMT hit info.
 */

#ifndef BACKGROUNDD3PDMAKER_LUCIDPMTHITFILLERTOOL_H
#define BACKGROUNDD3PDMAKER_LUCIDPMTHITFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class LUCID_RawDataContainer;


namespace D3PD {
  
  
  /**
   * @brief Block filler tool for LUCID PMT hit info.
   */
  class LucidPMTHitFillerTool
    : public BlockFillerTool<LUCID_RawDataContainer>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    LucidPMTHitFillerTool (const std::string& type,
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
    virtual StatusCode fill (const LUCID_RawDataContainer& lucid_rdc);
    
    
  private:

    // number of A side hits
    int* m_nAhits;
    // number of C side hits
    int* m_nChits;

  };
  
  
} // namespace D3PD


#endif // not BACKGROUNDD3PDMAKER_LUCIDPMTHITFILLERTOOL_H

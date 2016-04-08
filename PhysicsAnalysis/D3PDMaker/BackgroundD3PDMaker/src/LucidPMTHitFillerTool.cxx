/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LucidPMTHitFillerTool.cxx 498883 2012-05-03 10:33:42Z ssnyder $
/**
 * @file BackgroundD3PDMaker/src/LucidPMTHitFillerTool.cxx
 * @author Mark Tibbetts <mark.james.tibbetts@cern.ch>
 * @date Feb, 2011
 * @brief Block filler tool for LUCID PMT hit info.
 */


#include "LucidPMTHitFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"


namespace D3PD {
  
  
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LucidPMTHitFillerTool::LucidPMTHitFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<LUCID_RawDataContainer> (type, name, parent)
  {
    book().ignore(); // Avoid coverity warnings.
  }
  
  
  /**
   * @brief Book variables for this block.
   */
  StatusCode LucidPMTHitFillerTool::book()
  {
    CHECK( addVariable ("nAhits",       m_nAhits,
			"LUCID PMT hits on A side.") );
    CHECK( addVariable ("nChits",     m_nChits,
			"LUCID PMT hits on C side.") );
    
    return StatusCode::SUCCESS;
  }
  
  
  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode LucidPMTHitFillerTool::fill (const LUCID_RawDataContainer& lucid_rdc)
  {

    // Reset the counters:
    *m_nAhits = 0;
    *m_nChits = 0;

    LUCID_RawDataContainer::const_iterator LUCID_RawData_itr = lucid_rdc.begin();
    LUCID_RawDataContainer::const_iterator LUCID_RawData_end = lucid_rdc.end();    

    for (; LUCID_RawData_itr != LUCID_RawData_end; ++LUCID_RawData_itr) {

      *m_nAhits+=(*LUCID_RawData_itr)->getNhitsPMTsideA();
      *m_nChits+=(*LUCID_RawData_itr)->getNhitsPMTsideC(); 
    }     

    return StatusCode::SUCCESS;
    
  }
  
} // namespace D3PD

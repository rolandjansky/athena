/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RawTagInDetSPFillerTool.cxx 498883 2012-05-03 10:33:42Z ssnyder $
/**
 * @file BackgroundD3PDMaker/src/RawTagInDetSPFillerTool.cxx
 * @author Mark Tibbetts <mark.james.tibbetts@cern.ch>
 * @date Feb, 2011
 * @brief Block filler tool for InDet spacepoints from RawInfoSummaryForTag.
 */


#include "RawTagInDetSPFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TagEvent/RawInfoSummaryForTag.h"


namespace D3PD {
  
  
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  RawTagInDetSPFillerTool::RawTagInDetSPFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<RawInfoSummaryForTag> (type, name, parent)
  {
    book().ignore(); // Avoid coverity warnings.
  }
  
  
  /**
   * @brief Book variables for this block.
   */
  StatusCode RawTagInDetSPFillerTool::book()
  {
    CHECK( addVariable ("nPixSP",       m_nPixSP,
			"Number of pixel spacepoints.") );
    CHECK( addVariable ("nSctSP",     m_nSctSP,
			"Number of SCT spacepoints.") );
    
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
  StatusCode RawTagInDetSPFillerTool::fill (const RawInfoSummaryForTag& rawtag)
  {


    *m_nPixSP=rawtag.getNpixSPs();
    *m_nSctSP=rawtag.getNsctSPs();
   

    return StatusCode::SUCCESS;
    
  }
  
} // namespace D3PD

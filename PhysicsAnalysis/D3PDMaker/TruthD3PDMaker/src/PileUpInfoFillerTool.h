/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/PileUpInfoFillerTool.h
 * @author Jacob Searcy <jsearcy1@uoregon.edu>
 * @date Jul, 18
 * @brief Block filler tool for @c PileUpInfo information.
 */

#ifndef TRUTHD3PDMAKER_PILEUPINFOFILLERTOOL_H
#define TRUTHD3PDMAKER_PILEUPINFOFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "EventInfo/PileUpEventInfo.h"


namespace D3PD {

/**
 * @brief Block filler tool for @c PileUpInfo information.
 */
class PileUpInfoFillerTool
  : public BlockFillerTool<PileUpEventInfo::SubEvent>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PileUpInfoFillerTool (const std::string& type,
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
  virtual StatusCode fill (const PileUpEventInfo::SubEvent& p);

private:
  /// Variable: Event time.
  int* m_time;
  /// Variable: Event index.  
  int* m_index;
  /// Variable: Pileup type provenance.
  int* m_type;
  /// Variable: Event number.
  int* m_run_number;
  /// Variable: Run number.
  uint64_t* m_event_number;
  /// Write out Event Number
  bool  m_do_EvtNum;
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_PILEUPINFOFILLERTOOL_H


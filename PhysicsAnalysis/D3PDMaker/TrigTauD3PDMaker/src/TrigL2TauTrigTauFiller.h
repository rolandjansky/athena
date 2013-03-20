// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTauD3PDMaker/src/TrigL2TauTrigTauFiller.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for l2 trig tau variables.
 */

#ifndef TRIGTAUD3PDMAKER_TRIGL2TAUTRIGTAUFILLER_H
#define TRIGTAUD3PDMAKER_TRIGL2TAUTRIGTAUFILLER_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class TrigTau;


namespace D3PD {

  /**
   * @brief Block filler tool for TrigTau.
   *
   */
  class TrigL2TauTrigTauFiller
    : public BlockFillerTool<TrigTau>
    {
    public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      
      TrigL2TauTrigTauFiller (const std::string& type,
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
      virtual StatusCode fill (const TrigTau& p);

    private:
      bool m_detailLvlInc;
      int  m_detailLevel;

      unsigned int* m_roiword;
      float* m_simpleEtFlow;
      int*   m_nMatchedTracks;
      float* m_trkAvgDist;
      float* m_etOverPtLeadTrk;
    };
} // namespace D3PD


#endif

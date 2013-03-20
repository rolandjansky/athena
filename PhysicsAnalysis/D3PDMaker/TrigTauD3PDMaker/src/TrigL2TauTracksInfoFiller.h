// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTauD3PDMaker/src/TrigL2TauTracksInfoFiller.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for L2 trigtautracksinfo variables.
 */

#ifndef TRIGTAUD3PDMAKER_TRIGL2TAUTRACKSINFOFILLER_H
#define TRIGTAUD3PDMAKER_TRIGL2TAUTRACKSINFOFILLER_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"


namespace D3PD {

  /**
   * @brief Block filler tool for trigtautracksinfo.
   *
   */
  class TrigL2TauTracksInfoFiller
    : public BlockFillerTool<TrigTauTracksInfo>
    {
    public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      
      TrigL2TauTracksInfoFiller (const std::string& type,
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
      virtual StatusCode fill (const TrigTauTracksInfo& p);

    private:

      int* m_nCoreTracks;
      int* m_nSlowTracks;
      int* m_nIsoTracks;  
      float* m_charge;
      float* m_leadingTrackPt;
      float* m_scalarPtSumCore;
      float* m_scalarPtSumIso;

      double* m_3fastest_pt;
      double* m_3fastest_eta;
      double* m_3fastest_phi;
      double* m_3fastest_m;
    };
} // namespace D3PD


#endif

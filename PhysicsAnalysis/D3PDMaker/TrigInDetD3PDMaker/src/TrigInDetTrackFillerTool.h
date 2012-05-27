// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFillerTool.h 340544 2011-01-18 12:51:04Z krasznaa $
#ifndef __TRIGINDETD3PDMAKER_TRIGINDETTRACKFILLERTOOL_H__
#define __TRIGINDETD3PDMAKER_TRIGINDETTRACKFILLERTOOL_H__

// EDM include(s):
#include "TrigInDetEvent/TrigInDetTrack.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool saving information about TrigInDetTracks (LVL2)
    *
    *
    * @author Dmitry Emeliyanov
    */
   class TrigInDetTrackFillerTool : public BlockFillerTool< TrigInDetTrack > {

   public:
      /// Regular AlgTool constructor
      TrigInDetTrackFillerTool( const std::string& type, const std::string& name,
                                const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigInDetTrack& );

   private:
      bool m_writeBasicInfo; ///< Write basic information about the object
      bool m_writeHypoInfo; ///< Write the variables needed to re-do the hypo cuts
      bool m_writeDetailedInfo; ///< Write the most detailed information

      int*   m_algId; ///< Track author
      short* m_status; ///< Track status
      float* m_chi2; ///< Chi2 of track fit normalized on number of DOF

      int*   m_nStrawHits; ///< Number of TRT hits associated with track
      int*   m_nTRHits; ///< Number of high-threshold TRT hits associated with track
      int*   m_nPixelSpacePoints; ///< Number of pixel spacepoints associated with track
      int*   m_nSCT_SpacePoints; ///< Number of SCT spacepoints associated with track

      long*  m_hitPattern; ///< Hit pattern of silicon spacepoints
      int*   m_nStraw; ///< Number of TRT straws intersected by track
      int*   m_nStrawTime; ///< Number of TRT straws with valid drift time intersected by track

   }; // class TrigInDetTrackFillerTool

} // namespace D3PD

#endif // __TRIGINDETD3PDMAKER_TRIGINDETTRACKFILLERTOOL_H__

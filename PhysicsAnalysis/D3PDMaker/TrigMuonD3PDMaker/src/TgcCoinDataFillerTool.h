// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGMUOND3PDMAKER_TGCCOINDATAFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TGCCOINDATAFILLERTOOL_H

// EDM include(s):
#include "MuonTrigCoinData/TgcCoinData.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling information about TgcCoinData objects into the D3PD
    *
    * @author Takayuki Kanno
    *
    * $Revision: 332515 $
    * $Date$
    */
   class TgcCoinDataFillerTool : public BlockFillerTool< Muon::TgcCoinData > {

   public:
      /// Regular AlgTool constructor
      TgcCoinDataFillerTool( const std::string& type, const std::string& name,
                             const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const Muon::TgcCoinData& tgcelem );

   private:

      bool m_writeSL;
      bool m_writeTrackletHpt;
      bool m_writeMoreInfo;

      /** Aside or Cside */
      int* m_isAside;

      /** return phi number of trigger sector */
      int* m_phi;

      /** Forward region or Endcap region */
      int* m_isForward;

      /** Strip hit or Wire hit? */
      int* m_isStrip;

      /** return trackletId (Identifier for each track) */
      int* m_trackletId;

      /** return trackletId for strip (Identifier for each track) */
      int* m_trackletIdStrip;

      /** return region width of hit on TGC1 */
      float* m_widthIn;

      /** return region width of hit on TGC2(TRACKLET) or TGC3(HIPT) */
      float* m_widthOut;

      /** return the deviation from Pt=oo track */
      int* m_delta;

      /** return ROI number */
      int* m_roi;

      /** return pt threshold value */
      int* m_pt;

      /** return subMatrix of Tracklet or hsub of HiPt */
      int* m_sub;
      
      /** return isPositiveDeltaR (isMuplus) of SL */
      int* m_isPositiveDeltaR;

      /** return global position of track on TGC1(HIPT) or TGC2(TRACKLET) */
      //from Trk::GlobalPosition& globalposIn()
      float* m_etaIn;
      float* m_phiIn;
      float* m_zIn;

      /** return global position of track on TGC3 */
      //from Trk::GlobalPosition& globalposOut()
      float* m_etaOut;
      float* m_phiOut;
      float* m_zOut;

      /** return 2d width of ROI for SL: errMat[0][0]==(width for R), errMat[1][1]==(width for phi) */
      //from Trk::ErrorMatrix &errMat()
      float* m_widthR;
      float* m_widthPhi;

      //< Not implemented
        /** return Local position of track on TGC1(HIPT) or TGC2(TRACKLET) */
        //Trk::LocalPosition &posIn

        /** return Local position of track on TGC3 */
        //Trk::LocalPosition &posOut

        /** return TgcReadoutElement of track on TGC1(HIPT) or TGC2(TRACKLET) */
        //MuonGM::TgcReadoutElement* detectorElementIn

        /** return TgcReadoutElement of track on TGC3 */
        //MuonGM::TgcReadoutElement* detectorElementOut
      //>

   }; // class TgcCoinDataFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TGCCOINDATAFILLERTOOL_H

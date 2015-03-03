/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TgcCoinDataFillerTool.h"

namespace D3PD {

   TgcCoinDataFillerTool::TgcCoinDataFillerTool( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
      : BlockFillerTool< Muon::TgcCoinData >( type, name, parent )
   {
      // Avoid coverity warnings
      m_writeSL = true;
      m_writeTrackletHpt = true;
      m_writeMoreInfo = true;
      book().ignore();

      declareProperty( "WriteSL",          m_writeSL= false );
      declareProperty( "WriteTrackletHpt", m_writeTrackletHpt = false );
      declareProperty( "WriteMoreInfo",    m_writeMoreInfo = false );
   }

   StatusCode TgcCoinDataFillerTool::book() {

      if(m_writeSL||m_writeTrackletHpt){
         CHECK( addVariable( "isAside",          m_isAside,
                             "A side or C side" ) );
         CHECK( addVariable( "isForward",        m_isForward,
                             "Forward region or Endcap region" ) );
         CHECK( addVariable( "phi",              m_phi,
                             "Phi number of trigger sector" ) );
         CHECK( addVariable( "trackletId",       m_trackletId,
                             "Identifier for each track" ) );
         CHECK( addVariable( "etaOut",           m_etaOut,
                             "Global position of track on TGC3" ) );
         CHECK( addVariable( "phiOut",           m_phiOut,
                             "Global position of track on TGC3" ) );
         CHECK( addVariable( "zOut",             m_zOut,
                             "Global position of track on TGC3" ) );
      }
      if(m_writeTrackletHpt){
         CHECK( addVariable( "isStrip",          m_isStrip,
                             "Strip hit or Wire hit?" ) );
         CHECK( addVariable( "sub",              m_sub,
                             "SubMatrix of Tracklet or hsub of HiPt" ) );
         CHECK( addVariable( "delta",            m_delta,
                             "The deviation from Pt=oo track" ) );
         CHECK( addVariable( "etaIn",            m_etaIn,
                             "Global position of track on TGC1(HIPT) or "
                             "TGC2(TRACKLET)" ) );
         CHECK( addVariable( "phiIn",            m_phiIn,
                             "Global position of track on TGC1(HIPT) or "
                             "TGC2(TRACKLET)" ) );
         CHECK( addVariable( "zIn",              m_zIn,
                             "Global position of track on TGC1(HIPT) or "
                             "TGC2(TRACKLET)" ) );
      }
      if(m_writeSL){
         CHECK( addVariable( "trackletIdStrip",  m_trackletIdStrip,
                             "TrackletId for strip (Identifier for each track)" ) );
         CHECK( addVariable( "roi",              m_roi,
                             "ROI number" ) );
         CHECK( addVariable( "pt",               m_pt,
                             "Pt threshold value" ) );
         CHECK( addVariable( "widthR",           m_widthR,
                             "Part of 2d width of ROI" ) );
         CHECK( addVariable( "widthPhi",         m_widthPhi,
                             "Part of 2d width of ROI" ) );
      }
      if(m_writeMoreInfo){
         CHECK( addVariable( "widthIn",          m_widthIn,
                             "Region width of hit on TGC1" ) );
         CHECK( addVariable( "widthOut",         m_widthOut,
                             "Region width of hit on TGC2(TRACKLET) or TGC3(HIPT)" ) );
         CHECK( addVariable( "isPositiveDeltaR", m_isPositiveDeltaR,
                             "isPositiveDeltaR (isMuplus) of SL" ) );
      }
      return StatusCode::SUCCESS;
   }

   StatusCode TgcCoinDataFillerTool::fill( const Muon::TgcCoinData& tgcelem ) {

      if(m_writeSL||m_writeTrackletHpt){
         *m_isAside          = tgcelem.isAside();
         *m_isForward        = tgcelem.isForward();
         *m_phi              = tgcelem.phi();
         *m_trackletId       = tgcelem.trackletId();
         const Amg::Vector3D& gout = tgcelem.globalposOut();
         *m_etaOut           = gout.eta();
         *m_phiOut           = gout.phi();
         *m_zOut             = gout.z();
      }
      if(m_writeTrackletHpt){
         *m_isStrip          = tgcelem.isStrip();
         *m_sub              = tgcelem.sub();
         *m_delta            = tgcelem.delta();
         const Amg::Vector3D& gin = tgcelem.globalposIn();
         *m_etaIn            = gin.eta();
         *m_phiIn            = gin.phi();
         *m_zIn              = gin.z();
      }
      if(m_writeSL){
         *m_trackletIdStrip  = tgcelem.trackletIdStrip();
         *m_roi              = tgcelem.roi();
         *m_pt               = tgcelem.pt();
         const Amg::MatrixX& e = tgcelem.errMat();
         *m_widthR           = e(0,0);
         *m_widthPhi         = e(1,1);
      }
      if(m_writeMoreInfo){
         *m_widthIn          = tgcelem.widthIn();
         *m_widthOut         = tgcelem.widthOut();
         *m_isPositiveDeltaR = tgcelem.isPositiveDeltaR();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD

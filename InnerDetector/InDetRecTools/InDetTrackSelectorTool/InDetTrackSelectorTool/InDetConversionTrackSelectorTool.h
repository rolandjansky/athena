/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTrackSelectorTool_InDetConversionTrackSelectorTool_H
#define InDetTrackSelectorTool_InDetConversionTrackSelectorTool_H

//#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticle.h"

/**
 * A tool to be used for track preselection in conversion 
 * vertex finding.
 *
 * Thomas Koffas <Thomas.Koffas@cern.ch>
 * June 2008
 */

class IBeamCondSvc;

namespace Trk
{
 class ITrackSummaryTool;
 class Vertex;
 class IExtrapolator;
 class Track;
 class TrackParticleBase;
}

namespace InDet
{
 
 class InDetConversionTrackSelectorTool : virtual public Trk::ITrackSelectorTool, public AthAlgTool  
 {
 
   public:
    
    StatusCode initialize();
    
    StatusCode finalize();
    
    InDetConversionTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p); 
 
    ~InDetConversionTrackSelectorTool();
  
    /** Select a Trk::Track  */
    bool decision(const Trk::Track& track,const Trk::Vertex* vertex) const;
  
    /** Select a Trk::TrackParticleBase  */
    bool decision(const Trk::TrackParticleBase& track,const Trk::Vertex* vertex) const;

    bool decision(const xAOD::TrackParticle&,const xAOD::Vertex*) const ;


    
   private:
   
    int getCount( const xAOD::TrackParticle& tp, xAOD::SummaryType type ) const {
      uint8_t val;
      if( !tp.summaryValue(val,type) ) return 0;
      return val > 0 ? val : 0;
    }
   
   
    ToolHandle <Trk::ITrackSummaryTool>       m_trkSumTool;   //!< Track summary tool
    ToolHandle<Trk::IExtrapolator>            m_extrapolator; //!< Extrapolator tool
    ServiceHandle<IBeamCondSvc>               m_iBeamCondSvc; //!< pointer to the beam condition service

    /** Properties for track selection:all cuts are ANDed */
    double m_maxSiD0;  //!< Maximal d0 at (0,0,0) for tracks with Si hits
    double m_maxTrtD0; //!< Maximal d0 at (0,0,0) for standalone TRT tracks
    double m_maxSiZ0;  //!< Maximal z0 at (0,0,0)
    double m_maxTrtZ0; //!< Maximal z0 at (0,0,0) for standalone TRT tracks
    double m_minPt;    //!< Minimum Pt of tracks
    double m_trRatio1; //!< TR ratio for tracks with 15-20 TRT hits
    double m_trRatio2; //!< TR ratio for tracks with 20-25 TRT hits
    double m_trRatio3; //!< TR ratio for tracks with >25 TRT hits
    double m_trRatioTRT;//!< TR ratio for TRT only tracks
    double m_trRatioV0;//!< TR ratio for pion selection during V0 reconstruction
    double m_sD0_Si;   //!< Cut on D0 significance of Si tracks
    double m_sD0_Trt;  //!< Cut on D0 significance of TRT tracks
    double m_sZ0_Trt;  //!< Cut on Z0 significance of TRT tracks
    bool   m_isConv;   //!< Conversion flag
    bool   m_PIDonlyForXe; //!< Only check TRT PID if all hits are Xe hits

 }; //end of class definitions
} //end of namespace definitions

#endif

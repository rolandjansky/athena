/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTOOLS_HITQUALSELTOOL_H
#define INDETALIGNTOOLS_HITQUALSELTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
// #include "GaudiKernel/ToolHandle.h"
#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
// #include "AthenaBaseComps/AthMsgStreamMacros.h"


/** @class InDetAlignHitQualSelTool
    @brief The InDetAlignHitQualSelTool is to select good quality hits to 
           build residuals for alignment. The following cuts are possible:
           outlier hits, hits which have too large clusters, 
           hits with large incidence angles, ganged pixels, edge channels.
    @author Oleg Brandt <http://consult.cern.ch/xwho>
    */
    // Many thanks to Carlos Escobar and Sebastian Fleischmann!

namespace Trk {
  class TrackStateOnSurface ;
  class RIO_OnTrack ;
  class PrepRawData ;
}
namespace InDetDD {
  class PixelDetectorManager ;
  class SCT_DetectorManager ;
}
class AtlasDetectorID ;
class Identifier ;

class InDetAlignHitQualSelTool : virtual public IInDetAlignHitQualSelTool, public AthAlgTool {
 public:
  InDetAlignHitQualSelTool( const std::string&, const std::string&, const IInterface* ) ;
  virtual ~InDetAlignHitQualSelTool() ;
  
  virtual StatusCode initialize() ;
  virtual StatusCode finalize  () ;

  /** main method: from a TrackStateOnSurface select a good hit cutting on outlier
  hits, hits with too many pixels/strips, hits with large incidence angles */
  const Trk::RIO_OnTrack* getGoodHit( const Trk::TrackStateOnSurface* tsos ) const ;

  bool isGoodSiHit( const Trk::TrackStateOnSurface* tsos ) const ;
  
  /** from a TrackStateOnSurface select a good hole in track cutting on
  large incidence angles only */
  bool getGoodHole( const Trk::TrackStateOnSurface* tsos ) const ;
 private:
  // methods
  /** check, whether cluster contains a ganged pixel */
  bool isGangedPixel( const Trk::PrepRawData* prd ) const ;
  /** check, whether cluster size within limits of m_maxClusterSize */
  bool isGoodClusterSize( const std::vector<Identifier>& idVec ) const ;
  /** check, whether the strip/pixel is an edge channel */
  bool isEdgeChannel( const std::vector<Identifier>& idVec ) const ;
  /** calculate track incidence angle in local x-z frame */
  double incidAngle( const Trk::TrackParameters* trkPar
                         , const InDetDD::SiDetectorElement* detEle
                         ) const ;
  /** check whether track incidence angle within limits of m_maxIncidAngle */
  bool isGoodAngle( const Trk::TrackParameters* trkPar
                        , const InDetDD::SiDetectorElement* detEle
                        ) const ;
  // steering parameters
  /** reject hits labeled as outliers by the track fitter */
  bool  m_rejectOutliers ;
  /* maximum size of the cluster forming the hit. typically, clusters with more than
  5 pixels/hits are due to brem and are of less quality for alignment. */
  int   m_maxClusterSize ;
  /** reject clusters containing edge channels */
  bool  m_rejectEdgeChannels ;
  /** reject clusters containing ganged pixels */
  bool  m_rejectGangedPixels ;

  /** maximum incidence angle of a track (to which the hit belongs) on the Si-module.
  It is caculated in the local xz frame to ensure that the only the angle component 
  perpendicular to the strips is considered. It is defined w/r/t the local z-axis. */
  float m_maxIncidAngle ;
 
  // acceptIBLHits -> if true, IBL hits are accepted if they satisfy the quality selection criteria.
  // if false, IBL hits are always rejected  
  bool m_acceptIBLHits ;

  // acceptPixelHits -> if true, pixel hits are accepted if they satisfy the quality selection criteria.
  // if false, pixel hits are always rejected  
  bool m_acceptPixelHits ;

  // acceptSCTHits -> if true, SCT hits are accepted if they satisfy the quality selection criteria.
  // if false, SCT hits are always rejected  
  bool m_acceptSCTHits ;

 // "infrastructure" members
 
  const InDetDD::PixelDetectorManager*   m_PIXManager ; //!< to get pixel phi and eta identifiers
  const InDetDD::SCT_DetectorManager*    m_SCTManager ; //!< to get strip numbers
  const PixelID*                         m_pixelid ;    //!< Pixel id helper
  const SCT_ID*                          m_sctID ;    //!< Pixel id helper
} ;

#endif // INDETALIGNTOOLS_HITQUALSELTOOL_H

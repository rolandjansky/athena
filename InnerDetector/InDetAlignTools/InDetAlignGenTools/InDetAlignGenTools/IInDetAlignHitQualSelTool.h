/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTOOLS_IHITQUALSELTOOL_IH
#define INDETALIGNTOOLS_IHITQUALSELTOOL_IH

#include "GaudiKernel/IAlgTool.h"

/** Must declare this, with name of interface*/
static const InterfaceID IID_INDETALIGN_IInDetAlignHitQualSelTool( "IInDetAlignHitQualSelTool", 1, 0 ) ;

/** @class InDetAlignHitQualSelTool
    @brief The InDetAlignHitQualSelTool is to select good quality hits for alignment to 
           build residuals with possible cuts on outlier hits, hits which are too 
           large clusters, hits with large incidence angles, etc.
    @author Oleg Brandt <http://consult.cern.ch/xwho>
    */
    // Many thanks to Carlos Escobar and Sebastian Fleischmann!

namespace Trk {
  class TrackStateOnSurface ;
  class RIO_OnTrack ;
}

class IInDetAlignHitQualSelTool : virtual public IAlgTool {
  public:
  /** for ToolHandle functionality */
  static const InterfaceID& interfaceID() ;
  
  /** main method: from a TrackStateOnSurface select a good hit cutting on outlier hits, hits with
  too many pixels/strips, hits with large incidence angles */
  virtual const Trk::RIO_OnTrack* getGoodHit( const Trk::TrackStateOnSurface* tsos ) const = 0 ;

  virtual bool isGoodSiHit( const Trk::TrackStateOnSurface* tsos ) const = 0 ;

  /** from a TrackStateOnSurface select a good hole in track cutting on
  large incidence angles only */
  virtual bool getGoodHole( const Trk::TrackStateOnSurface* tsos ) const = 0 ;
} ;

inline const InterfaceID& IInDetAlignHitQualSelTool::interfaceID() {
  return IID_INDETALIGN_IInDetAlignHitQualSelTool ; 
}

#endif // INDETALIGNTOOLS_IHITQUALSELTOOL_IH

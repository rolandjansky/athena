/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTOOLS_IOVERLAPTOOL_IH
#define INDETALIGNTOOLS_IOVERLAPTOOL_IH

#include "GaudiKernel/IAlgTool.h"
#include "InDetAlignGenTools/IInDetAlignFillTrack.h"
//#include "DataModel/DataVector.h"
#include <vector>
/** @class InDetAlignOverlapTool
    @brief The InDetAlignOverlapTool is to select good quality tracks for alignment to 
    build residuals with possible cuts on overlap hits, overlaps in SCT and PIXELs (in Phi and Eta).
*/

class AlignTrk;
class AlignSiHit;

static const InterfaceID 
IID_INDETALIGN_IInDetAlignOverlapTool( "IInDetAlignOverlapTool", 1, 0 ) ;


class IInDetAlignOverlapTool : virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID() ;

  /** main method: from a TrackStateOnSurface select an overlap hit */
  virtual int getNumberOverlapPIX( const AlignTrk& )const = 0 ;
  virtual int getNumberOverlapSCT( const AlignTrk& )const = 0 ;
  // virtual const DataVector<AlignSiHit>* getOverlapHit(const AlignTrk& )const = 0 ;
  //virtual std::vector<AlignSiHit> getOverlapHit(const AlignTrk& ) =0;//const = 0 ;
  virtual std::vector<AlignSiHit> getOverlapHit(const AlignTrk& ) = 0;//const = 0 ;
  
  
} ;

inline const InterfaceID& IInDetAlignOverlapTool::interfaceID() {
  return IID_INDETALIGN_IInDetAlignOverlapTool ; 
}

#endif // INDETALIGNTOOLS_IOVERLAPTOOL_IH



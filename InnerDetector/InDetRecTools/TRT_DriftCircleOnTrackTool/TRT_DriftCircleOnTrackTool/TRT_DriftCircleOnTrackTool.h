/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  TRT_DriftCircleOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for TRT_DriftCircleOnTrack production
///////////////////////////////////////////////////////////////////
// Version 1.0 18/02/2003 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#ifndef TRT_DriftCircleOnTrackTool_H
#define TRT_DriftCircleOnTrackTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

namespace Trk {

  class IRIO_OnTrackErrorScalingTool;
}

namespace InDet {

  /** @class TRT_DriftCircleOnTrackTool
      This tool creates TRT_DriftCircleOnTrack objects using a given
      track hypothesis. See doxygen to Trk::RIO_OnTrackCreator for details.
  */

  class TRT_DriftCircleOnTrackTool : 
    virtual public Trk::IRIO_OnTrackCreator, public AthAlgTool
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:

  TRT_DriftCircleOnTrackTool
    (const std::string&,const std::string&,const IInterface*);
  virtual ~TRT_DriftCircleOnTrackTool ();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  virtual const InDet::TRT_DriftCircleOnTrack* correct
    (const Trk::PrepRawData&,const Trk::TrackParameters&) const; 

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
  
 private:

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////

  ToolHandle<Trk::IRIO_OnTrackErrorScalingTool> m_errorScalingTool  ;
  bool                                          m_scaleTrtCov       ;
  bool                                          m_useErrorCorrection;
};

} // end of namespace InDet

#endif // TRT_DriftCircleOnTrackTool_H

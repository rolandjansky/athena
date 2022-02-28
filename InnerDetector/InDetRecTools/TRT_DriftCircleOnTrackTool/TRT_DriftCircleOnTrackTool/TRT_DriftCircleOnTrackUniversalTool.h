/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  TRT_DriftCircleOnTrackUniversalTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef TRT_DriftCircleOnTrackUniversalTool_H
#define TRT_DriftCircleOnTrackUniversalTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

namespace InDet {

  /** @class TRT_DriftCircleOnTrackUniversalTool
      This tool creates TRT_DriftCircleOnTrack objects using a given
      track hypothesis. See doxygen to Trk::RIO_OnTrackCreator for details.
  */

  class TRT_DriftCircleOnTrackUniversalTool :
    virtual public Trk::IRIO_OnTrackCreator, public AthAlgTool
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:

  TRT_DriftCircleOnTrackUniversalTool
    (const std::string&,const std::string&,const IInterface*);
  virtual ~TRT_DriftCircleOnTrackUniversalTool ();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  virtual const Trk::RIO_OnTrack* correct
    (const Trk::PrepRawData&,const Trk::TrackParameters&) const; 

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
  
 private:

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////

  ToolHandle<Trk::IRIO_OnTrackCreator> m_riontrackDrift
    {this, "RIOonTrackToolDrift", "InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"};
  ToolHandle<Trk::IRIO_OnTrackCreator> m_riontrackTube
    {this, "RIOonTrackToolTube", "InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool"};
  double                               m_scalefactor; //scale factor for hit uncertainty
};

} // end of namespace InDet

#endif // TRT_DriftCircleOnTrackUniversalTool_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  TRT_DriftCircleOnTrackRecalibrateTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef TRT_DriftCircleOnTrackRecalibrateTool_H
#define TRT_DriftCircleOnTrackRecalibrateTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "InDetRIO_OnTrack/TRTRIO_OnTrackErrorScaling.h"

#include "LumiBlockData/LuminosityCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

class ITRT_DriftFunctionTool;


namespace InDet {

  /** @class TRT_DriftCircleOnTrackRecalibrateTool
      This tool creates TRT_DriftCircleOnTrack objects using a given
      track hypothesis. See doxygen to Trk::RIO_OnTrackCreator for details.
  */

  class TRT_DriftCircleOnTrackRecalibrateTool : 
    virtual public Trk::IRIO_OnTrackCreator, public AthAlgTool
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:

  TRT_DriftCircleOnTrackRecalibrateTool
    (const std::string&,const std::string&,const IInterface*);
  virtual ~TRT_DriftCircleOnTrackRecalibrateTool ();
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

  ToolHandle<Trk::IRIO_OnTrackCreator>          m_riontrackTube   ;
  ToolHandle<ITRT_DriftFunctionTool>            m_drifttool       ;

  SG::ReadCondHandleKey<LuminosityCondData>     m_lumiDataKey
      {this, "LumiDataKey", "", "SG key for luminosity data"};

  //  SG::ReadCondHandleKey<TRTRIO_OnTrackErrorScaling> m_trtErrorScalingKey
  SG::ReadCondHandleKey<RIO_OnTrackErrorScaling> m_trtErrorScalingKey
      {this,"TRTErrorScalingKey", "/Indet/TrkErrorScalingTRT", "Key for TRT error scaling conditions data."};

  bool                                          m_useToTCorrection{};  //!< Shall the Time over Threshold correction be used?
  double                                        m_scalefactor{}     ; //scale factor for hit uncertainty

};

} // end of namespace InDet

#endif // TRT_DriftCircleOnTrackRecalibrateTool_H

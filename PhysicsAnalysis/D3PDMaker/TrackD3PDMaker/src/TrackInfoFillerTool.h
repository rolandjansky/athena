/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRACKINFOFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKINFOFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include <vector>

namespace Trk{
class TrackInfo;
}

namespace D3PD {


class TrackInfoFillerTool
  : public BlockFillerTool<Trk::TrackInfo>
{
public:
  typedef BlockFillerTool<Trk::TrackInfo> Base; 

  TrackInfoFillerTool (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const Trk::TrackInfo& info);

private:

  /* NTuple variables: **/
  // author information
  int *m_fitter;
  int *m_patternReco1;
  int *m_patternReco2;
  int *m_trackProperties;
  int *m_particleHypothesis;

}; // class TrackInfoFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRACKINFOFILLERTOOL_H

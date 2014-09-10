/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_CLUSTERASSOCPIXELFILLERTOOL_H
#define TRACKD3PDMAKER_CLUSTERASSOCPIXELFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "Identifier/Identifier.h"

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace Trk{
class TrackStateOnSurface;
}

namespace D3PD {

class ClusterAssocPixelFillerTool
  : public D3PD::BlockFillerTool<Trk::TrackStateOnSurface>
{
public:
  typedef D3PD::BlockFillerTool<Trk::TrackStateOnSurface> Base;

  ClusterAssocPixelFillerTool (const std::string& type,
				 const std::string& name,
				 const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const Trk::TrackStateOnSurface &p);

private:

  void clearData();

  const AtlasDetectorID* m_idHelper;
  const PixelID* m_pixelId;


  float                 *m_clGlobX;
  float                 *m_clGlobY;
  float                 *m_clGlobZ;
  float                 *m_clLocX;
  float                 *m_clLocY;
  float                 *m_locX;
  float                 *m_locY;
  float                 *m_err_locX;
  float                 *m_err_locY;
  float                 *m_cov_locXY;
  float                 *m_x;
  float                 *m_y;
  float                 *m_z;
  float                 *m_trkLocX;
  float                 *m_trkLocY;
  float                 *m_err_trkLocX;
  float                 *m_err_trkLocY;
  float                 *m_cov_trkLocXY;
  int                   *m_isCompetingRIO;
  int                   *m_sizePhi;
  //int                   *m_sideModule;
  float                 *m_locPhi;
  float                 *m_locTheta;
  int                   *m_tsosType;
  Identifier::value_type *m_id; // unique identifier for a cluster
  Identifier::value_type *m_detElementId; // unique identifier for a detector element
  int *m_detType;
  int *m_bec;
  int *m_layer;


}; // class ClusterAssocPixelFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_CLUSTERASSOCPIXELFILLERTOOL_H

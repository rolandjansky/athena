/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_PIXELCLUSTERFILLERTOOL_H
#define TRACKD3PDMAKER_PIXELCLUSTERFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "Identifier/Identifier.h"

class PixelID;

namespace InDet {
  class PixelCluster;
}

namespace D3PD {

class PixelClusterFillerTool
  : public D3PD::BlockFillerTool<InDet::PixelCluster>
{
public:
  typedef D3PD::BlockFillerTool<InDet::PixelCluster> Base;

  PixelClusterFillerTool (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const InDet::PixelCluster &clus);

private:

  void clearData();

  const PixelID* m_pixId; 

  /* NTuple variables: **/
  Identifier::value_type *m_id; // unique identifier for a cluster
  Identifier::value_type *m_detElementId;
  char *m_bec; // Barrel:0, End Cap: +/- 2
  char *m_layer;
  char *m_phi_module;
  char *m_eta_module;
  short *m_col;
  short *m_row;
  float *m_charge;
  int *m_ToT;
  short *m_LVL1A;
  int *m_sizePhi;
  int *m_sizeZ;
  int *m_size;
  float *m_locX;
  float *m_locY;
  float *m_x;
  float *m_y;
  float *m_z;
  char *m_isFake;
  char *m_isGanged;
  int   *m_isSplit; // defined for Pixel only
  float *m_splitProb1; // defined for Pixel only
  float *m_splitProb2; // defined for Pixel only
}; // class PixelClusterFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_PIXELCLUSTERFILLERTOOL_H

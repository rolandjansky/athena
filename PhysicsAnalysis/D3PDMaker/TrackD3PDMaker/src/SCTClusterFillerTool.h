/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SCTCLUSTERFILLERTOOL_H
#define TRACKD3PDMAKER_SCTCLUSTERFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "Identifier/Identifier.h"

class SCT_ID;

namespace InDet {
  class SCT_Cluster;
}

namespace D3PD {


class SCTClusterFillerTool
  : public D3PD::BlockFillerTool<InDet::SCT_Cluster>
{
public:
  typedef D3PD::BlockFillerTool<InDet::SCT_Cluster> Base;

  SCTClusterFillerTool (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const InDet::SCT_Cluster &clus);

private:

  void clearData();

  const SCT_ID* m_sctId; 

  /* NTuple variables: **/
  Identifier::value_type *m_id; // unique identifier for a cluster
  Identifier::value_type *m_detElementId;
  int *m_bec; // Barrel:0, End Cap: +/- 2
  int *m_layer;
  int *m_phi_module;
  int *m_eta_module;
  float *m_x;
  float *m_y;
  float *m_z;
  float *m_locX;
  float *m_locY;
  int *m_side;
  int *m_size;

}; // class SCTClusterFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_SCTCLUSTERFILLERTOOL_H

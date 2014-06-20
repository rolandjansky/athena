/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_TRKCNTSEFFILLERTOOL_H
#define TRIGMBD3PDMAKER_TRKCNTSEFFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>

class TrigTrackCountsCollection;

namespace TrigMbD3PD {

typedef TrigTrackCountsCollection  TrkCntsEfFillerObjectType;
typedef D3PD::BlockFillerTool<TrkCntsEfFillerObjectType> TrkCntsEfFillerBase;

class TrkCntsEfFillerTool
  : public TrkCntsEfFillerBase
{
public:
  TrkCntsEfFillerTool (const std::string& type,
		       const std::string& name,
		       const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const TrkCntsEfFillerObjectType& p);

private:

  void clearData();

  /* NTuple variables **/
  unsigned int *m_nElements;
  unsigned int *m_hZ0Bins;
  float *m_hZ0Min;
  float *m_hZ0Max;
  unsigned int *m_hPtBins;
  float *m_hPtMin;
  float *m_hPtMax;
  unsigned int *m_hEtaBins;
  float *m_hEtaMin;
  float *m_hEtaMax;
  unsigned int *m_hPhiBins;
  float *m_hPhiMin;
  float *m_hPhiMax;
  std::vector<float> *m_z0_pt_contents;
  std::vector<float> *m_eta_phi_contents;

}; // class TrkCntsEfFillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_TRKCNTSEFFILLERTOOL_H

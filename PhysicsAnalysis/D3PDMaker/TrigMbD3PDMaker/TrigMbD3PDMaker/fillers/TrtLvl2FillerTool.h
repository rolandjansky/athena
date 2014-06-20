/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_TRTLVL2FILLERTOOL_H
#define TRIGMBD3PDMAKER_TRTLVL2FILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>

class TrigTrtHitCountsCollection;

namespace TrigMbD3PD {

typedef TrigTrtHitCountsCollection TrtLvl2FillerObjectType;
typedef D3PD::BlockFillerTool<TrtLvl2FillerObjectType> TrtLvl2FillerBase;

class TrtLvl2FillerTool
  : public TrtLvl2FillerBase
{
public:
  TrtLvl2FillerTool (const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const TrtLvl2FillerObjectType& p);

private:

  void clearData();

  /* NTuple variables **/
  unsigned int *m_nElements;
  unsigned int *m_hTotBins;
  float *m_hTotMin;
  float *m_hTotMax;
  std::vector<float> *m_endcapC_contents;
  std::vector<float> *m_barrel_contents;
  std::vector<float> *m_endcapA_contents;

}; // class TrtLvl2FillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_TRTLVL2FILLERTOOL_H

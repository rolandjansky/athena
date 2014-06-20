/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_SPLVL2FILLERTOOL_H
#define TRIGMBD3PDMAKER_SPLVL2FILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>

class TrigSpacePointCountsCollection;

namespace TrigMbD3PD {

typedef TrigSpacePointCountsCollection SpLvl2FillerObjectType;
typedef D3PD::BlockFillerTool<SpLvl2FillerObjectType> SpLvl2FillerBase;

class SpLvl2FillerTool
  : public SpLvl2FillerBase
{
public:
  SpLvl2FillerTool (const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const SpLvl2FillerObjectType& p);

private:

  void clearData();

  /* NTuple variables **/
  unsigned int *m_nElements;
  unsigned int *m_hPixelClusTotBins;
  float *m_hPixelClusTotMin;
  float *m_hPixelClusTotMax;
  unsigned int *m_hPixelClusSizeBins;
  float *m_hPixelClusSizeMin;
  float *m_hPixelClusSizeMax;
  std::vector<float> *m_pixelClusEndcapC_contents;
  std::vector<float> *m_pixelClusBarrel_contents;
  std::vector<float> *m_pixelClusEndcapA_contents;
  std::vector<unsigned int> *m_droppedPixelModuleIds;
  unsigned int *m_sctSpEndcapC;
  unsigned int *m_sctSpBarrel;
  unsigned int *m_sctSpEndcapA;
  std::vector<unsigned int> *m_droppedSctModuleIds;

}; // class SpLvl2FillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_SPLVL2FILLERTOOL_H

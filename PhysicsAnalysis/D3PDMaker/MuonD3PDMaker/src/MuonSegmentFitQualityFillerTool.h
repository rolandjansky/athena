/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonSegmentFitQualityFillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Feb 2010
 * @brief Block filler tool with fit quality information for muon segments, detail level 1
 */

#ifndef D3PDMAKER_MUONSEGMENTFITQUALITYFILLERTOOL_H
#define D3PDMAKER_MUONSEGMENTFITQUALITYFILLERTOOL_H 1


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Trk { 
  class Segment;
}


namespace D3PD {


class MuonSegmentFitQualityFillerTool
  : public BlockFillerTool<Trk::Segment>
{
public:
  MuonSegmentFitQualityFillerTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual StatusCode book();

  virtual StatusCode fill (const Trk::Segment& p);


private:
  float *m_chi2;
  int *m_ndof;
};

} // namespace D3PD

#endif

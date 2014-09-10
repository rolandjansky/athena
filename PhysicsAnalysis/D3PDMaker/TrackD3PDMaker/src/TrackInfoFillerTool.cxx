/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackInfoFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "TrkTrack/TrackInfo.h"

#include <bitset>

namespace D3PD {


TrackInfoFillerTool::TrackInfoFillerTool (const std::string& type,
					  const std::string& name,
					  const IInterface* parent)
  : Base (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}

StatusCode TrackInfoFillerTool::initialize(){

  CHECK( Base::initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode TrackInfoFillerTool::book()
{

  // author information
  CHECK( addVariable ("fitter", m_fitter) );
  CHECK( addVariable ("patternReco1", m_patternReco1) );
  CHECK( addVariable ("patternReco2", m_patternReco2) );
  //   CHECK( addVariable ("patternReco", m_patternReco) );
  CHECK( addVariable ("trackProperties", m_trackProperties) );
  CHECK( addVariable ("particleHypothesis", m_particleHypothesis) );

  return StatusCode::SUCCESS;
}

StatusCode TrackInfoFillerTool::fill (const Trk::TrackInfo& info)
{

  // track fitter
  *m_fitter = info.trackFitter() ;

  // pattern reco info
  int pattern = 0;
  for(unsigned int i=0; i<32; i++){
    const Trk::TrackInfo::TrackPatternRecoInfo preco = (Trk::TrackInfo::TrackPatternRecoInfo)i;
    if(info.patternRecoInfo(preco)) pattern = (pattern | (1 << i));
  }
  *m_patternReco1 = pattern;
  pattern = 0;
  for(unsigned int i=32; i<Trk::TrackInfo::NumberOfTrackRecoInfo; i++){
    const Trk::TrackInfo::TrackPatternRecoInfo preco = (Trk::TrackInfo::TrackPatternRecoInfo)i;
    if(info.patternRecoInfo(preco)) pattern = (pattern | (1 << (i-32)));
  }
  *m_patternReco2 = pattern;

  //   *m_patternReco = 0;
  //   for(unsigned long i=0; i<Trk::TrackInfo::NumberOfTrackRecoInfo; i++){
  //     long long int one = 1;
  //     const Trk::TrackInfo::TrackPatternRecoInfo preco = (Trk::TrackInfo::TrackPatternRecoInfo)i;
  //     if(info.patternReco(preco)) *m_patternReco = (*m_patternReco | (one << i));
  //   }

  // track properties
  *m_trackProperties = 0;
  for(unsigned long i=0; i<Trk::TrackInfo::NumberOfTrackProperties; i++){
    int one = 1;
    const Trk::TrackInfo::TrackProperties tprop = (Trk::TrackInfo::TrackProperties)i;
    if(info.trackProperties(tprop)) *m_trackProperties = (*m_trackProperties | (one << i));
  }

  // particle hypothesis
  *m_particleHypothesis = info.particleHypothesis();

  return StatusCode::SUCCESS;
}


} // namespace D3PD

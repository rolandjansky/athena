/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackTrackSummaryFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include <bitset>


namespace D3PD {


namespace TrackSum {


enum SumType {
  HitSum,
  HoleSum,
  IDHits,   
  IDHoles,
  IDSharedHits,
  IDOutliers,
  PixelInfoPlus,   
  SCTInfoPlus, 
  TRTInfoPlus, 
  InfoPlus,
  ExpectBLayer,
  MuonHits,
  MuonHoles
};

const unsigned int   HitSumMask        = (1<<HitSum);
const unsigned int   HoleSumMask       = (1<<HoleSum);
const unsigned int   IDHitsMask        = (1<<IDHits);   
const unsigned int   IDHolesMask       = (1<<IDHoles);
const unsigned int   IDSharedHitsMask  = (1<<IDSharedHits);
const unsigned int   IDOutliersMask    = (1<<IDOutliers);
const unsigned int   PixelInfoPlusMask = (1<<PixelInfoPlus);   
const unsigned int   SCTInfoPlusMask   = (1<<SCTInfoPlus); 
const unsigned int   TRTInfoPlusMask   = (1<<TRTInfoPlus); 
const unsigned int   InfoPlusMask      = (1<<InfoPlus);
const unsigned int   ExpectBLayerMask  = (1<<ExpectBLayer);
const unsigned int   MuonHitsMask      = (1<<MuonHits);
const unsigned int   MuonHolesMask     = (1<<MuonHoles);

struct Var {
  unsigned int type;
  std::string name;
  Trk::SummaryType key;
};


Var vars[] = {
  // ID hits
  { IDHitsMask,                  "nBLHits",                   Trk::numberOfInnermostPixelLayerHits },
  { IDHitsMask   | HitSumMask,   "nPixHits",                  Trk::numberOfPixelHits },
  { IDHitsMask   | HitSumMask,   "nSCTHits",                  Trk::numberOfSCTHits },
  { IDHitsMask   | HitSumMask,   "nTRTHits",                  Trk::numberOfTRTHits },
  { IDHitsMask,                  "nTRTHighTHits",             Trk::numberOfTRTHighThresholdHits },
  { IDHitsMask,                  "nTRTXenonHits",             Trk::numberOfTRTXenonHits },
  // ID holes + dead sensors - needed for appropriate cutting
  { IDHolesMask  | HoleSumMask,  "nPixHoles",                 Trk::numberOfPixelHoles },
  { IDHolesMask  | HoleSumMask,  "nSCTHoles",                 Trk::numberOfSCTHoles },
  { IDHolesMask  | HoleSumMask,  "nTRTHoles",                 Trk::numberOfTRTHoles },
  { IDHolesMask,                 "nPixelDeadSensors",         Trk::numberOfPixelDeadSensors },
  { IDHolesMask,                 "nSCTDeadSensors",           Trk::numberOfSCTDeadSensors },
  // ID shared & Split hits
  { IDSharedHitsMask,           "nBLSharedHits",              Trk::numberOfInnermostPixelLayerSharedHits},
  { IDSharedHitsMask,           "nPixSharedHits",             Trk::numberOfPixelSharedHits},
  { IDSharedHitsMask,           "nSCTSharedHits",             Trk::numberOfSCTSharedHits },
  { IDSharedHitsMask,           "nBLayerSplitHits",           Trk::numberOfInnermostLayerSplitHits},
  { IDSharedHitsMask,           "nPixSplitHits",              Trk::numberOfPixelSplitHits},
  // ID outliers                                              
  { IDOutliersMask,             "nBLayerOutliers",            Trk::numberOfInnermostPixelLayerOutliers },
  { IDOutliersMask,             "nPixelOutliers",             Trk::numberOfPixelOutliers },
  { IDOutliersMask,             "nSCTOutliers",               Trk::numberOfSCTOutliers },
  { IDOutliersMask,             "nTRTOutliers",               Trk::numberOfTRTOutliers},
  { IDOutliersMask,             "nTRTHighTOutliers",          Trk::numberOfTRTHighThresholdOutliers},
  // Pixel info plus                                          
  { PixelInfoPlusMask,          "nContribPixelLayers",        Trk::numberOfContribPixelLayers },
  { PixelInfoPlusMask,          "nGangedPixels",              Trk::numberOfGangedPixels },
  { PixelInfoPlusMask,          "nGangedFlaggedFakes",        Trk::numberOfGangedFlaggedFakes },
  { PixelInfoPlusMask,          "nPixelSpoiltHits",           Trk::numberOfPixelSpoiltHits },
  // SCT info plus                                            
  { SCTInfoPlusMask,            "nSCTDoubleHoles",            Trk::numberOfSCTDoubleHoles },
  { SCTInfoPlusMask,            "nSCTSpoiltHits",             Trk::numberOfSCTSpoiltHits },
  // TRT info plus                                            
  { TRTInfoPlusMask,            "nTRTDeadStraws",             Trk::numberOfTRTDeadStraws },
  { TRTInfoPlusMask,            "nTRTTubeHits",               Trk::numberOfTRTTubeHits },
  // Info plus                                                
  { InfoPlusMask,               "nOutliersOnTrack",           Trk::numberOfOutliersOnTrack },
  { InfoPlusMask,               "standardDeviationOfChi2OS",  Trk::standardDeviationOfChi2OS },
  // Expect BLayer hit                                        
  { ExpectBLayerMask,           "expectInnermostPixelLayerHit",            Trk::expectInnermostPixelLayerHit },
  // Muon hits                                                
  { MuonHitsMask | HitSumMask,  "nMDTHits",                   Trk::numberOfMdtHits },
  { MuonHitsMask | HitSumMask,  "nCSCEtaHits",                Trk::numberOfCscEtaHits },
  { MuonHitsMask | HitSumMask,  "nCSCPhiHits",                Trk::numberOfCscPhiHits },
  { MuonHitsMask | HitSumMask,  "nRPCEtaHits",                Trk::numberOfRpcEtaHits },
  { MuonHitsMask | HitSumMask,  "nRPCPhiHits",                Trk::numberOfRpcPhiHits },
  { MuonHitsMask | HitSumMask,  "nTGCEtaHits",                Trk::numberOfTgcEtaHits },
  { MuonHitsMask | HitSumMask,  "nTGCPhiHits",                Trk::numberOfTgcPhiHits },
  // Muon holes                                               
  { MuonHolesMask | HoleSumMask, "nMdtHoles",                 Trk::numberOfMdtHoles },
  { MuonHolesMask | HoleSumMask, "nCscEtaHoles",              Trk::numberOfCscEtaHoles },
  { MuonHolesMask | HoleSumMask, "nCscPhiHoles",              Trk::numberOfCscPhiHoles },
  { MuonHolesMask | HoleSumMask, "nRpcEtaHoles",              Trk::numberOfRpcEtaHoles },
  { MuonHolesMask | HoleSumMask, "nRpcPhiHoles",              Trk::numberOfRpcPhiHoles },
  { MuonHolesMask | HoleSumMask, "nTgcEtaHoles",              Trk::numberOfTgcEtaHoles },
  { MuonHolesMask | HoleSumMask, "nTgcPhiHoles",              Trk::numberOfTgcPhiHoles }
};

const size_t nvars = sizeof(vars) / sizeof(vars[0]);


} // namespace TrackSum


TrackTrackSummaryFillerTool::TrackTrackSummaryFillerTool (const std::string& type,
							  const std::string& name,
							  const IInterface* parent)
  : Base (type, name, parent),
    m_typeMask (0)
{
  // summaries
  declareProperty ("IDHits",        m_doIDHits        = true);
  declareProperty ("IDHoles",       m_doIDHoles       = true);
  declareProperty ("IDSharedHits",  m_doIDSharedHits  = true);
  declareProperty ("IDOutliers",    m_doIDOutliers    = false);
  declareProperty ("PixelInfoPlus", m_doPixelInfoPlus = false);
  declareProperty ("SCTInfoPlus",   m_doSCTInfoPlus   = false);
  declareProperty ("TRTInfoPlus",   m_doTRTInfoPlus   = false);
  declareProperty ("InfoPlus",      m_doInfoPlus      = false);
  declareProperty ("MuonHits",      m_doMuonHits      = true);
  declareProperty ("MuonHoles",     m_doMuonHoles     = false);
  declareProperty ("ExpectBLayer",  m_doExpectBLayer  = true);

  // extras
  declareProperty ("HitSum",       m_doHitSum       = true);
  declareProperty ("HoleSum",      m_doHoleSum      = true);
  declareProperty ("HitPattern",   m_doHitPattern   = true);
  declareProperty ("SiHits",       m_doSiHits       = false);
  declareProperty ("TRTRatio",     m_doTRTRatio     = true);
  declareProperty ("PixeldEdx",    m_doPixeldEdx    = true);
  declareProperty ("ElectronPID",  m_doElectronPID  = false);

  // Avoid coverity warnings.
  m_storeFullSummary = true;
  book().ignore();

  // full summary
  declareProperty ("FullInfo",      m_storeFullSummary = false);
}

StatusCode TrackTrackSummaryFillerTool::initialize()
{
  m_typeMask = 0;
  if (m_doIDHits)        m_typeMask |= TrackSum::IDHitsMask;
  if (m_doIDHoles)       m_typeMask |= TrackSum::IDHolesMask;
  if (m_doIDSharedHits)  m_typeMask |= TrackSum::IDSharedHitsMask;
  if (m_doIDOutliers)    m_typeMask |= TrackSum::IDOutliersMask;
  if (m_doPixelInfoPlus) m_typeMask |= TrackSum::PixelInfoPlusMask;
  if (m_doSCTInfoPlus)   m_typeMask |= TrackSum::SCTInfoPlusMask;
  if (m_doTRTInfoPlus)   m_typeMask |= TrackSum::TRTInfoPlusMask;
  if (m_doInfoPlus)      m_typeMask |= TrackSum::InfoPlusMask;
  if (m_doExpectBLayer)  m_typeMask |= TrackSum::ExpectBLayerMask;
  if (m_doMuonHits)      m_typeMask |= TrackSum::MuonHitsMask;
  if (m_doMuonHoles)     m_typeMask |= TrackSum::MuonHolesMask;

  return StatusCode::SUCCESS;
}


StatusCode TrackTrackSummaryFillerTool::book()
{
  // track summaries
  size_t ivar = 0;
  for (size_t i = 0; i < TrackSum::nvars; i++)
    if ( (m_typeMask & TrackSum::vars[i].type) ||
	 m_storeFullSummary ) ++ivar;
  m_vars.resize (ivar);

  ivar = 0;
  for (size_t i = 0; i < TrackSum::nvars; i++) {
    if ( (m_typeMask & TrackSum::vars[i].type) || 
	 m_storeFullSummary)
      CHECK( addVariable (TrackSum::vars[i].name, m_vars[ivar++]) );
  }
  assert (ivar == m_vars.size());

  // extra information
  if (m_doHitSum || m_storeFullSummary)     CHECK( addVariable ("nHits",        m_nHits) );
  if (m_doHoleSum || m_storeFullSummary)    CHECK( addVariable ("nHoles",       m_nHoles) );
  if (m_doHitPattern || m_storeFullSummary) CHECK( addVariable ("hitPattern",   m_hitPattern) );
  if (m_doSiHits || m_storeFullSummary)     CHECK (addVariable ("nSiHits",      m_nSiHits) );
  if (m_doTRTRatio || m_storeFullSummary) {
    CHECK (addVariable ("TRTHighTHitsRatio", m_TRTHighTHitsRatio) );
    CHECK (addVariable ("TRTHighTOutliersRatio", m_TRTHighTOutliersRatio) );
  }
  if (m_doPixeldEdx || m_storeFullSummary){
    CHECK (addVariable ("pixeldEdx", m_pixeldEdx) );
    CHECK (addVariable ("nGoodHitsPixeldEdx", m_nGoodHitsPixeldEdx) );
    CHECK (addVariable ("massPixeldEdx", m_massPixeldEdx) );
    CHECK (addVariable ("likelihoodsPixeldEdx", m_likelihoodsPixeldEdx) );
  }

  if (m_doElectronPID || m_storeFullSummary) {
    CHECK (addVariable ("eProbabilityComb", m_eProbabilityComb) );
    CHECK (addVariable ("eProbabilityHT",   m_eProbabilityHT) );
    CHECK (addVariable ("eProbabilityToT",  m_eProbabilityToT) );
    CHECK (addVariable ("eProbabilityBrem", m_eProbabilityBrem) );
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackTrackSummaryFillerTool::fill (const Trk::TrackSummary& ts)
{

  this->clearData();

  // track summaries
  size_t ivar = 0;
  for (size_t i = 0; i < TrackSum::nvars; i++) {
    const TrackSum::Var& v = TrackSum::vars[i];
    int count = ts.get (v.key);
    if ( (m_typeMask & v.type) || m_storeFullSummary) {
      *m_vars[ivar++] = count;
    }
    if (count < 0) count = 0;
    if ((m_doHitSum  && (v.type & TrackSum::HitSumMask)) || m_storeFullSummary)  *m_nHits  += count;
    if ((m_doHoleSum && (v.type & TrackSum::HoleSumMask)) || m_storeFullSummary) *m_nHoles += count;
  }
  assert (ivar == m_vars.size());

  // extras:

  // hit pattern
  if (m_doHitPattern || m_storeFullSummary) {
    std::bitset<Trk::numberOfDetectorTypes> pattern;
    for(unsigned int i=0; i<pattern.size(); i++){
      const Trk::DetectorType detType = (Trk::DetectorType)i;
      pattern.set(i, ts.isHit(detType));
    }
    *m_hitPattern = (int)pattern.to_ulong();
  }
  // Si hits
  if (m_doSiHits || m_storeFullSummary)
    *m_nSiHits = ts.get(Trk::numberOfPixelHits) + ts.get(Trk::numberOfSCTHits);
  // TRT ratios
  if (m_doTRTRatio || m_storeFullSummary) {
    int nhightrt = ts.get (Trk::numberOfTRTHighThresholdHits);
    int ntrt = ts.get (Trk::numberOfTRTHits);
    int ntrtxenon = ts.get(Trk::numberOfTRTXenonHits);
    if (ntrt > 0)
      *m_TRTHighTHitsRatio = (float)nhightrt / (float)ntrt;

    int nhightrt_outl = ts.get (Trk::numberOfTRTHighThresholdOutliers);
    int ntrt_outl = ts.get (Trk::numberOfTRTOutliers);

    // since we only save one value for number of xenon straws crossed, this is the correct one
    // above variable (without outliers) shouldn't be used, anyway
    if (ntrtxenon > 0) 
       *m_TRTHighTOutliersRatio = (float)(nhightrt + nhightrt_outl) / (float)(ntrtxenon);
    else if (ntrtxenon < 0 && (ntrt + ntrt_outl) > 0) {
      // Backwards compatibility if xenon information isn't present.
      *m_TRTHighTOutliersRatio =
        (float)(nhightrt + nhightrt_outl) / (float)(ntrt + ntrt_outl);
    }
  }
  // Pixel dEdX  
  if (m_doPixeldEdx || m_storeFullSummary){
    *m_pixeldEdx = ts.getPixeldEdx();
    *m_nGoodHitsPixeldEdx = ts.numberOfUsedHitsdEdx();
    const Trk::InDetTrackSummary* inDetTS = ts.indetTrackSummary();
    if (inDetTS){
      *m_massPixeldEdx = inDetTS->massPixeldEdx();
      std::vector<float> likelihoods = inDetTS->likelihoodsPixeldEdx();
      m_likelihoodsPixeldEdx->assign(likelihoods.begin(),likelihoods.end());
    }
  }
  // electron PID
  if(m_doElectronPID || m_storeFullSummary) {
    *m_eProbabilityComb = ts.getPID(Trk::eProbabilityComb);
    *m_eProbabilityHT = ts.getPID(Trk::eProbabilityHT);
    *m_eProbabilityToT = ts.getPID(Trk::eProbabilityToT);
    *m_eProbabilityBrem = ts.getPID(Trk::eProbabilityBrem);
  }

  return StatusCode::SUCCESS;
}

void TrackTrackSummaryFillerTool::clearData(){

  size_t ivar = 0;
  for (size_t i = 0; i < TrackSum::nvars; i++) {
    const TrackSum::Var& v = TrackSum::vars[i];
    if ((m_typeMask & v.type) || m_storeFullSummary ) {
      *m_vars[ivar++] = 0;
    }
  }
  if (m_doHitSum || m_storeFullSummary)      *m_nHits = 0;
  if (m_doHoleSum || m_storeFullSummary)     *m_nHoles = 0;
  if (m_doHitPattern || m_storeFullSummary)  *m_hitPattern = 0;
  if (m_doSiHits || m_storeFullSummary)      *m_nSiHits = 0;
  if (m_doTRTRatio || m_storeFullSummary)    *m_TRTHighTHitsRatio = 0;
  if (m_doPixeldEdx || m_storeFullSummary){
    *m_pixeldEdx = 0;
    *m_nGoodHitsPixeldEdx = 0;
    *m_massPixeldEdx = -1;
    m_likelihoodsPixeldEdx->clear();
  }
  if (m_doElectronPID || m_storeFullSummary) *m_eProbabilityComb = 0;
  if (m_doElectronPID || m_storeFullSummary) *m_eProbabilityHT = 0;
  if (m_doElectronPID || m_storeFullSummary) *m_eProbabilityToT = 0;
  if (m_doElectronPID || m_storeFullSummary) *m_eProbabilityBrem = 0;
}

} // namespace D3PD

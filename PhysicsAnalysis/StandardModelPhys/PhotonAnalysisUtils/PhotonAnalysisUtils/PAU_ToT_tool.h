/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PAU_ToT_tool.h
///////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_TOT_TOOL_H
#define PHOTONANALYSISUTILS_TOT_TOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "PhotonAnalysisUtils/IPAU_ToT_tool.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
//#include "TrkEventPrimitives/ParticleHypothesis.h"
//#include "TrkEventUtils/TrkParametersComparisonFunction.h"

#include "TMath.h"
#include "TFile.h"

class TRT_ID;
namespace Trk
{
 class TrackParticleBase;
 class TrackStateOnSurface;
}

class PAU_ToT_tool : public AthAlgTool, virtual public IPAU_ToT_tool {
 public:
  
  /** AlgTool like constructor */
  PAU_ToT_tool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAU_ToT_tool();
       
  /** AlgTool initailize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  
  double getToTvariable(const Trk::TrackParticleBase* ) const;
  
  
 private:

  /** ID TRT helper */
  const TRT_ID* m_trtId;

  TFile* m_inputCorrFileBarrel ;
  TFile* m_inputCorrFileEndcapPos ;
  TFile* m_inputCorrFileEndcapNeg ;
  
  double giveToTlargerIsland(unsigned int BitPattern) const;
  double giveToTonly1bits(unsigned int BitPattern) const;

  double correctToT(double ToT, double HitZ, double HitRtrack, double HitR, int HitPart, int Layer, int Strawlayer, TFile* corr_barrel, TFile* corr_endcap_pos, TFile* corr_endcap_neg) const;
  
};


#endif // PHOTONANALYSISUTILS_TOT_TOOL_H

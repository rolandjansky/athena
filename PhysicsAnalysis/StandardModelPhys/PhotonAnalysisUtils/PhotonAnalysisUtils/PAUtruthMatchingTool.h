/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// PAUtruthMatchingTool.h, (c) ATLAS Detector software 2007
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////


#ifndef PHOTONANALYSISUTILS_TRUTHMATCHINGTOOL_H
#define PHOTONANALYSISUTILS_TRUTHMATCHINGTOOL_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "PhotonAnalysisUtils/IPAUtruthMatchingTool.h"
#include "PhotonAnalysisUtils/IPAUtruthTool.h"
#include "PhotonAnalysisUtils/IPAUhggFitter.h"

class TH1;
class IExtrapolateToCaloTool;
class CaloDepthTool;
class IExtrapolTrackToCaloTool;

#include <string>

class PAUtruthMatchingTool : public AthAlgTool, virtual public IPAUtruthMatchingTool {
 public:
  
  /** AlgTool like constructor */
  PAUtruthMatchingTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUtruthMatchingTool();
  
  /** AlgTool initialize method.*/
  virtual StatusCode initialize();
  /** AlgTool finalize method */
  virtual StatusCode finalize();

  // public methods:

  virtual StatusCode execute() ;
  virtual const PAU::egamma*     getRecoMatch(const TruthParticle* tp) const ;
  virtual const TruthParticle*  getTruthMatch(const PAU::egamma* g) const ;
  virtual bool                  extrapolToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) const ;

  class match_t {  /** any match between a rec egamma and a truth particle */
  public:
    const PAU::egamma*      rec ;    /// the rec photon
    const TruthParticle*   tp ;     /// the true particle
    int                    score ;  /// a ranking score to classify match quality
    float                  etaCaloTruth; /// extrapolation of true particle to calorimeter
    float                  phiCaloTruth; /// extrapolation of true particle to calorimeter
  } ;
  
 private:

  std::vector<match_t> m_myMatches ;

  ToolHandle<IPAUtruthTool>         m_Truth ;
  ToolHandle<IPAUhggFitter>         m_PAUhggFitter;

  IExtrapolateToCaloTool*    m_extrapolateToCalo;
  IExtrapolateToCaloTool*  m_eToCalo;
  IExtrapolateToCaloTool*  m_gToCalo;

  ToolHandle<CaloDepthTool>             m_calodepth;

  std::string m_extrapolateToCaloToolName;
  std::string m_extrapolateElectronToCaloToolName;
  std::string m_extrapolateGammaToCaloToolName;

  inline bool isGood(const match_t & m) { return m.score > 0 ; }
  inline bool conflict(const match_t & m1, const match_t & m2) const { return ( m1.rec==m2.rec || m1.tp==m2.tp ) ;}
  int score(const match_t & m) const;
  bool doMatch() ;

  bool simpleExtrapolator (const TruthParticle* tp, double& etaCalo, double& phiCalo) const ; // <-- very naive, don't use!
  bool betterExtrapolator1(const TruthParticle* tp, double& etaCalo, double& phiCalo) const ; 
  bool betterExtrapolator2(const TruthParticle* tp, double& etaCalo, double& phiCalo) const ; // <-- this seems more stable
  // followings try to build a cluster from all interaction products
  bool collectEetaphi(const TruthParticle* tp, double& sumE, double& sumEeta, double& sumEphi, int& ncalls) const ;
  bool clusterToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) const ;
};


#endif // PHOTONANALYSISUTILS_TRUTHMATCHINGTOOL_H

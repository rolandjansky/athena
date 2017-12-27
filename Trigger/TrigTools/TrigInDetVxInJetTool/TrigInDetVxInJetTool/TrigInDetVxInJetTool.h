/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// TrigInDetVxInJetTool.h - Description
//
/*
  Tool for secondary vertex inside jet reconstruction.
  It returns a pointer to TrigVertex object which contains
  information about the reconstructed vertex.
  In case of failure pointer to TrigVertex is 0.
   

  Author: Vadim Kostyukhin
  e-mail: vadim.kostyukhin@cern.ch

  -----------------------------------------------------------------------------*/



#ifndef _VKalVrt_TrigInDetVxInJetTool_H
#define _VKalVrt_TrigInDetVxInJetTool_H
// Normal STL and physical vectors
#include <vector>
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigInDetVxInJetTool/ITrigInDetVxInJetTool.h"

#include "TrigVKalFitter/TrigVKalFitter.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TH1D.h"
#include "TH2D.h"

class TrigInDetTrack;
class TrigVertex;

namespace Trk{ class RecVertex; }


typedef std::vector<double> dvect;


//------------------------------------------------------------------------

class TrigInDetVxInJetTool : public AthAlgTool, virtual public ITrigInDetVxInJetTool {


public:

  TrigInDetVxInJetTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigInDetVxInJetTool();

  StatusCode initialize();
  StatusCode finalize();

    
  const TrigVertex* findSecVertex(const Trk::RecVertex &,
                                  const CLHEP::HepLorentzVector &,
                                  const std::vector<const TrigInDetTrack*> &);

private:

  int m_CutSctHits;
  int m_CutPixelHits;
  int m_CutSiHits;
  int m_CutBLayHits;
  int m_CutSharedHits;
  float m_CutPt;
  float m_CutZVrt;
  float m_CutA0;
  float m_CutProbChi2;
  float m_CutChi2Ndof;
  float m_SecTrkChi2Cut;
  float m_ConeForTag;
  float m_Sel2VrtChi2Cut;
  float m_Sel2VrtSigCut;
  float m_TrkSigCut;
  float m_TrkSigSumCut;
  float m_A0TrkErrorCut;
  float m_ZTrkErrorCut;
  float m_AntiPileupSigRCut;
  float m_AntiPileupSigZCut;
  float m_AntiFake2trVrtCut;

  long int m_RobustFit;

  float m_Xbeampipe;
  float m_Ybeampipe;
  float m_XlayerB;
  float m_YlayerB;
  float m_Xlayer1;
  float m_Ylayer1;
  float m_Xlayer2;
  float m_Ylayer2;
  float m_Rbeampipe;

  float m_RlayerB;
  float m_Rlayer1;
  float m_Rlayer2;

  bool     m_getNegativeTail;
  bool     m_getNegativeTag;

  TrigVertex*  GetVrtSec(const std::vector<const TrigInDetTrack*> & InpTrk,
                         const Trk::RecVertex                     & PrimVrt,
                         const CLHEP::HepLorentzVector            & JetDir,
                         std::vector<double>                      & Results,
                         std::vector<const TrigInDetTrack*>       & SelSecTrk,
                         std::vector<const TrigInDetTrack*>       & TrkFromV0);

  bool m_doTrackSelection;

  /** @brief for monitoring purposes. */
  std::vector<float> m_cutFlowTrkSel;
  std::vector<float> m_cutFlowTwoTrkVtxSel;

  ToolHandle < ITrigVKalFitter > m_fitSvc;

  double m_massPi ;
  double m_massP ;
  double m_massE;
  double m_massK0;
  double m_massLam;

  // For multivertex version only

  float m_chiScale[11];

  // Gives correct mass assignment in case of nonequal masses
  double massV0( std::vector< std::vector<double> >& TrkAtVrt, double massP, double massPi ) const;
  int FindMax( std::vector<double>& Chi2PerTrk) const;


  CLHEP::HepLorentzVector TotalMom(const std::vector<const TrigInDetTrack*>& InpTrk) const; 
  double                  pTvsDir(const Amg::Vector3D &Dir, const std::vector<double>& InpTrk) const; 


  double VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                    const std::vector<double> VrtErr,double& Signif ) const;
  double VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                    const std::vector<double> SecVrtErr, const CLHEP::HepLorentzVector & JetDir) const;
  double VrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>& VrtErr1,
                    const Amg::Vector3D & Vrt2, const std::vector<double>& VrtErr2) const;
					  
  double ProjPos(const Amg::Vector3D & Vrt,const CLHEP::HepLorentzVector & JetDir) const;
  static bool sortbyPtPredicate(const TrigInDetTrack* t1, const TrigInDetTrack* t2);

  template <class Track>
  double FitCommonVrt(std::vector<const Track*>& ListSecondTracks,
                      const Trk::RecVertex & PrimVrt,
                      std::vector<double>  & InpMass, 
                      Amg::Vector3D        & FitVertex,
                      std::vector<double>  & ErrorMatrix,
                      CLHEP::HepLorentzVector     & Momentum,
                      std::vector< std::vector<double> >  & TrkAtVrt);

  void RemoveEntryInList(std::vector<const TrigInDetTrack*>& , int) const;

  int SelGoodTrigTrack(const std::vector<const TrigInDetTrack*>& InpPart,
		       const Trk::RecVertex                    & PrimVrt,
		       const CLHEP::HepLorentzVector                  & JetDir,
		       std::vector<const TrigInDetTrack*>      & SelPart);

  template <class Track>
  void Select2TrVrt(std::vector<const Track*>  & SelectedTracks,
                    std::vector<const Track*>  & TracksForFit,
                    const Trk::RecVertex       & PrimVrt,
                    const CLHEP::HepLorentzVector     & JetDir,
                    std::vector<double>        & InpMass, 
                    std::vector<const Track*>  & TrkFromV0,
                    std::vector<const Track*>  & ListSecondTracks);

  StatusCode VKalVrtFitFastBase(const std::vector<const TrigInDetTrack*>& listPart,Amg::Vector3D& Vertex);
    
  StatusCode VKalVrtFitBase(const std::vector<const TrigInDetTrack*> & listPart,
                            Amg::Vector3D                            & Vertex,
                            CLHEP::HepLorentzVector                  & Momentum,
                            long int                                 & Charge,
                            std::vector<double>                      & ErrorMatrix,
                            std::vector<double>                      & Chi2PerTrk,
                            std::vector< std::vector<double> >       & TrkAtVrt,
                            double                                   & Chi2 );

};


#endif

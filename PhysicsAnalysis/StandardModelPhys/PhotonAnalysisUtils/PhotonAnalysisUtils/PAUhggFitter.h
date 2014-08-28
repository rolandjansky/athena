/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// PAUhggFitter.h, (c) ATLAS Detector software 2007
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////


#ifndef RECOTOOLS_HGGFITTER_H
#define RECOTOOLS_HGGFITTER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "PhotonAnalysisUtils/IPAUhggFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"

class TH1;
class IBeamCondSvc;

class PAUhggFitter : public AthAlgTool, virtual public IPAUhggFitter {
 public:
  
  /** AlgTool like constructor */
  PAUhggFitter(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUhggFitter();
  
  /** AlgTool initialize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  // public methods:
 
  void SetEtas(const CaloCluster* clus1,const CaloCluster* clus2) ;
  void SetPrimVertexInfo(float Zinput,float DZinput) ;
  void SetConversion(unsigned gamma_index, const Trk::VxCandidate* conv) ;
  void SetTrack(unsigned gamma_index, const Rec::TrackParticle* trk) ;

  bool FitPAU(float& Zrec, float& ZrecRes, float& eta1rec, float& eta2rec) ;
  bool getEtaSinglePhoton(float& eta1,float& eta2) const ;
  bool gimeRZ(int layer, float eta, float& R, float& Z) const ;
  bool inEMBarrel( float eta ) const ;
  float EtaFromVertex(int layer, float eta, float Zvtx) const ;

  // old methods...
  void SetEtas(std::vector<float>, std::vector<float>) ;
  void SetPrimVertexInfo(bool usevtx,float Zinput,float DZinput) ;
  void SetConvInfo(bool usecv,bool conv1,float rcv1,float zcv1,bool conv2,float rcv2,float zcv2) ;

 private:
  
  void combineZ() ;
  bool AssignRZ(int jphot,std::vector<float>& r,std::vector<float>& z,std::vector<float>& weight) ;
  bool SinglePhotonFit(int jphot,bool FillHistos=false) ;
  void killWorst(std::vector<float>& x, std::vector<float>& y, std::vector<float>& w, unsigned iworst) ;
  bool FitDir( std::vector<float> x, std::vector<float> y, std::vector<float> weight, bool rej=false );   
  StatusCode GetErrors();  

  std::string m_beamCondSvcName;
  IBeamCondSvc* m_beamCondSvc; 
  ToolHandle< Trk::IExtrapolator > m_extrapolator;

 private:

  std::vector<int> m_p_where ;  

  // indexing: m_etas[j] = all points of (jth photon), m_etas[j][i] = (ith point) of (jth photon)
  std::vector<float> m_etas[2] ;  
  std::vector<float> m_ZVertexRec,m_ZVertexRecRes;  // Z of vertex from linear fit
  std::vector<float> m_NewRecoEtaSeparate;          // eta from fit (separately fitted photons)
  std::vector<float> m_NewRecoEta, m_NewRecoTheta;  // eta and theta from linear fit
  float m_PI ;  
  bool m_useIDVert, m_useConversion, m_isConv[2];
  float m_IDVert, m_IDVertRes, m_Rconv[2], m_Zconv[2]; 
  float m_chi2[2], m_zfit[2];
  bool m_fit_ok[2] ;
  float m_errors_ps[100], m_errors_strip[100], m_errors_mid[100]; 

  float m_chi2worst ;
  unsigned m_nfitmin ;
  bool m_usePS, m_useDummyVertex, m_useCombinedFit ;

  // variables used for fitting  y = ax + b 
  unsigned m_nFitPoints ;
  unsigned m_iworst ;
  float m_a, m_sigmaa, m_b, m_sigmab, m_chiquad, m_covab ;
  std::vector<float> m_r, m_z, m_weight, m_chi2point ;  // (r,z) points to fit
};


#endif // RECOTOOLS_HGGFITTER_H

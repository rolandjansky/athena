/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUSYTOOLS_FakeMetEstimator_h
#define SUSYTOOLS_FakeMetEstimator_h

////////  FakeMetEstimator version 4.0 ///////
//
// a tool to reject or correct events by BCH_CORR_JET (estimated fake MET fraction by jet profile method)
// emulation of detector condition to calculate inefficiency for MC is also implemented
//
// wiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/FakeMetEstimator
// contact: Yousuke Kataoka <Yousuke.Kataoka@cern.ch>
//
//////// usage /////////
//
// - setup
// root [0] .L FakeMetEstimator.h+
// root [1] FakeMetEstimator est("fest_periodF_v1.root") // root file is for emulation
//
// - fake met rejection for jet
// root [2] est.isBad(ptJet, BCH_CORR_JET, BCH_CORR_CELL, BCH_CORR_DOTX, phiJet, etxMet, etyMet) // for data or MC of correct condition
// root [3] est.isBadEmul(ptJet, etaJet, phiJet, etxMet, etyMet, BCH_CORR_JET, BCH_CORR_CELL, BCH_CORR_DOTX) // to emulate condition for MC (use parameter file) 
//
// - fake met rejection for event, reject if isBad==true (arguments are vector<float>)
// root [4] est.isBad(ptJet, BCH_CORR_JET, BCH_CORR_CELL, BCH_CORR_DOTX, phiJet, etxMet, etyMet) // for data or MC of correct condition
// root [5] est.isBadEmul(ptJet, etaJet, phiJet, etxMet, etyMet, BCH_CORR_JET, BCH_CORR_CELL, BCH_CORR_DOTX) // to emulate conditions for MC (use parameter file)
//
// - hidden arguments of above rejections (.., fakeCosPhiThreshold=10.*1000, significanceCosPhiThreshold=10., fakeThreshold=-1, significanceThreshold=-1)
//   estimator = fakeCorrectedAndObserved (estimated fake MET for jet)
//   reject if:                                                     defalut
//      estimator > fakeThreshold &&                             ... OFF 
//      MET/estimator < significanceThreshold                    ... OFF
//      estimator x cos(dphi) > fakeCosPhiThreshold &&           ... 10GeV 
//      MET/estimator x cos(dphi) < significancecosPhiThreshold  ... 10 
//
//      -> typical rejection power for fake events(MET>~100GeV) ~ 10, signal efficiency(in SR) ~ 99%
//
// - correct met by jet profile method (switching from default BCH_CORR_CELL,BCH_CORR_DOTX to BCH_CORR_JET)
// root [6] double correctionEx,correctionEy; // vector sum of corrected energy for jets
// root [7] est.correction(correctionEx, correctionEy, ptJet, BCH_CORR_JET, BCH_CORR_CELL, BCH_CORR_DOTX, phiJet) 
// root [8] double metEx = originalEx - correctionEx
// root [9] double metEy = originalEy - correctionEy
//      (if you want corrected jet too, use ptCorrected(ptJet, BCH_CORR_JET, BCH_CORR_CELL, BCH_CORR_DOTX) )
//
// - emulate scale shift for given detector condition
// root [10] double correctionEx,correctionEy; // vector sum of corrected energy for jets
// root [11] est.emulation(correctionEx, correctionEy, ptJet, etaJet, phiJet, BCH_CORR_JET, BCH_CORR_CELL, BCH_CORR_DOTX) // ptJet is modified after call
// root [12] double metEx = originalEx - correctionEx
// root [13] double metEy = originalEy - correctionEy
//      (if you want corrected jet directly, use ptReconstructedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX) )
//
// - low level functions of emulation: fcor = BCH_CORR_CELL+BCH_CORR_DOTX, fjet = BCH_CORR_JET
// root [14] est.fcor(40000., 0.5, 0.8) // pt, eta, phi
// root [15] est.fjet(40000., 0.5, 0.8)
//
//
//////// definitions /////////
//
// - pT(reconstructed) ... pT of jet by default reconstruction, which is already corrected
//                         by neighbor-average correction(BCH_CORR_CELL) and trigger-readout correction(BCH_CORR_DOTX)
// - pT(observed)      ... observed pT without any correction
// - pT(corrected)     ... pT of jet corrected by "jet profile method" (BCH_CORR_JET)
//
// - fake met estimator ... fakeCorrectedAndObserved = pT(corrected) - pT(observed)
// - for correction     ... fakeCorrectedAndReconstructed = pT(corrected) - pT(reconstructed)
//
//////// ChangeLog /////////
//
// v1.0 ( 5/ 6/2011) ... initial version
// v2.0 ( 6/ 6/2011) ... in the emulation mode, the shift of MET by the estimated fake is considered
// v3.0 (10/ 6/2011) ... in the emulation mode, the shift of jet pT by dead cells is considered
// v3.1 ( 1/ 7/2011) ... fix to release memory in deleting
// v4.0 (11/ 7/2011) ... add emulation func. to emulate scale shift of Jet/Met in given detector condition
//

#include <TFile.h>
#include <TFolder.h>
#include <TList.h>
#include <TH1D.h>
#include <TRandom2.h>

#include <iostream>
#include <cmath>
#include <cstdlib>

class EmulFakeMet {
 public:
  EmulFakeMet(std::string rootname): m_rootname(rootname) {
    // initialize
    m_fname[FCOR]="fcor";
    m_fname[FJET]="fjet";

    m_ptmax[0]=20.*1000;
    m_ptmax[1]=40.*1000;
    m_ptmax[2]=60.*1000;
    m_ptmax[3]=100.*1000;
    m_ptmax[4]=160.*1000;
    m_ptmax[5]=260.*1000;
    m_ptmax[6]=420.*1000;
    m_ptmax[7]=660.*1000;
    m_ptmax[8]=1120.*1000;
    m_ptmax[9]=9999.*1000;

    for(int i=0; i<NFTYPE; i++){
      cache_ftype[i]=0;
      cache_eta[i]=0;
      cache_phi[i]=0;
      cache_pt[i]=0;
      cache_f[i]=0;
      for(int j=0; j<NETABINS; j++){
        cache_feta[i][j]=0;
        for(int k=0; k<NPHIBINS; k++){
          cache_fphi[i][j][k]=0;
          for(int l=0; l<NPTBINS; l++){
            cache_hist[i][j][k][l]=0;
          }
        }
      }
    }
  }
  virtual ~EmulFakeMet(){
    for(int i=0; i<NFTYPE; i++){
      if(cache_ftype[i]) {
        // set owner to delete at onece
        cache_ftype[i]->SetOwner(1);
        TList* tl_eta = (TList*)cache_ftype[i]->GetListOfFolders();
        for(int j=0; j<tl_eta->GetSize(); j++) {
          TFolder* f_eta = (TFolder*)tl_eta->At(j);
          f_eta->SetOwner(1);
          TList* tl_phi = (TList*)f_eta->GetListOfFolders();
          for(int k=0; k<tl_phi->GetSize(); k++) {
            ((TFolder*)tl_phi->At(k))->SetOwner(1);
          }
        }
        delete cache_ftype[i];
      }
    }
  }

  enum FType { FCOR=0, FJET, NFTYPE };
  enum NBins { NETABINS=100, NPHIBINS=128, NPTBINS=10 };

  // get folder fcor or fjet
  TFolder* getFolder(FType itype){
    if(cache_ftype[itype]) return cache_ftype[itype];
    TFile tf(m_rootname.c_str());
    if(!tf.IsOpen()){
      std::cout << "file " << m_rootname << " not opened" << std::endl;
      abort();
    }
    cache_ftype[itype] = (TFolder*)tf.Get(m_fname[itype].c_str());
    if (cache_ftype[itype]==0){
      std::cout << "folder " << m_fname[itype] << " not found in " << m_rootname << std::endl;
      abort();
    }
    //cache_ftype[itype]->SetDirectory(0);
    return cache_ftype[itype];
  }

  // get folder fjet/eta%d
  TFolder* getFolder(FType itype, int ieta){
    if(cache_feta[itype][ieta]) return cache_feta[itype][ieta];
    TFolder* fold = getFolder(itype);
    sprintf(hname,"eta%d",ieta);
    cache_feta[itype][ieta] = (TFolder*)fold->FindObject(hname);
    if (cache_feta[itype][ieta]==0){
      std::cout << "folder " << hname << " not found in " << fold->GetName() << std::endl;
      abort();
    }
    return cache_feta[itype][ieta];
  }

  // get folder fjet/eta%d/phi%d
  TFolder* getFolder(FType itype, int ieta, int iphi){
    if(cache_fphi[itype][ieta][iphi]) return cache_fphi[itype][ieta][iphi];
    TFolder* fold = getFolder(itype,ieta);
    sprintf(hname,"phi%d",iphi);
    cache_fphi[itype][ieta][iphi] = (TFolder*)fold->FindObject(hname);
    if (cache_fphi[itype][ieta][iphi]==0){
      std::cout << "folder " << hname << " not found in " << fold->GetName() << std::endl;
      abort();
    }
    return cache_fphi[itype][ieta][iphi];
  }

  // get hist fjet/eta%d/phi%d/eta%d_phi%d_pt%d
  TH1D* getHist(FType itype, int ieta, int iphi, int ipt){
    if(cache_hist[itype][ieta][iphi][ipt]) return cache_hist[itype][ieta][iphi][ipt];
    TFolder* fold = getFolder(itype,ieta,iphi);
    sprintf(hname,"%s_eta%d_phi%d_pt%d",m_fname[itype].c_str(),ieta,iphi,ipt);
    cache_hist[itype][ieta][iphi][ipt] = (TH1D*)fold->FindObject(hname);
    if (cache_hist[itype][ieta][iphi][ipt]==0){
      std::cout << "folder " << hname << " not found in " << fold->GetName() << std::endl;
      abort();
    }
    return cache_hist[itype][ieta][iphi][ipt];
  }

  TH1D* getFest(FType itype, double eta, double phi, double pt, int minEntry=2){
    if(eta>=5.0) {
      std::cout << "eta out of range " << eta << " use " << 4.999 << std::endl;
      eta=4.999;
    }
    if(eta<-5.0) {
      std::cout << "eta out of range " << eta << " use " << -5.0 << std::endl;
      eta=-5.0;
    }
    if(phi>=3.2) {
      std::cout << "phi out of range " << phi << " use " << 3.199 << std::endl;
      phi=3.199;
    }
    if(phi<-3.2) {
      std::cout << "phi out of range " << phi << " use " << -3.2 << std::endl;
      phi=-3.2;
    }

    int ieta = static_cast<int>((eta+5.0)/(10./NETABINS));
    int iphi = static_cast<int>((phi+3.2)/(6.4/NPHIBINS));
    int ipt = 0;
    for(;ipt<NPTBINS;ipt++){
      if(pt<m_ptmax[ipt]) break;
    }
    for(int i=ipt; i>=0; i--){// check lower pt if less than minEntry
      TH1D* h = getHist(itype,ieta,iphi,i);
      if(h->Integral()>=minEntry || i==0) return h;
    } 
    return 0;//not reached
  }

  double emul(FType itype, double pt, double eta, double phi){
    if(eta==cache_eta[itype] && phi==cache_phi[itype] && pt==cache_pt[itype]) return cache_f[itype];

    TH1D* hist = getFest(itype,eta,phi,pt);

    double f = 0;
    if(hist->Integral()>0){
      // gurantee same number when same arguments, maybe not recomended usage but it's enough
      m_random.SetSeed(int(itype+eta*10000+phi*10000+pt));
      TRandom* backup = gRandom; 
      gRandom = &m_random;
      f = hist->GetRandom();
      gRandom = backup;
    }
    // 1 length cache for each type
    cache_eta[itype]=eta;
    cache_phi[itype]=phi;
    cache_pt[itype]=pt;
    cache_f[itype]=f;
    return f;
  }

 private:
  TRandom2 m_random;
  std::string m_rootname;
  std::string m_fname[NFTYPE];
  double m_ptmax[NPTBINS];

  // FindObject maybe expensive, cache histos 
  TFolder* cache_ftype[NFTYPE];
  TFolder* cache_feta[NFTYPE][NETABINS];
  TFolder* cache_fphi[NFTYPE][NETABINS][NPHIBINS];
  TH1D* cache_hist[NFTYPE][NETABINS][NPHIBINS][NPTBINS];

  // GetRandom maybe expensive, 1 length cache 
  double cache_eta[NFTYPE];
  double cache_phi[NFTYPE];
  double cache_pt[NFTYPE];
  double cache_f[NFTYPE];

  char hname[256];
};

class FakeMetEstimator {
 public:
  FakeMetEstimator(std::string rootname="fest_periodF_v1.root"): m_rootname(rootname), m_emul(0), m_fjetScale(1.){}
  virtual ~FakeMetEstimator(){
   if(m_emul) delete m_emul;
  }

  // This method is needed for Proof usage, added by Renaud
  void initialize(std::string rootname) {
    m_rootname = rootname;
    if(m_emul) delete m_emul;
    m_emul = new EmulFakeMet(m_rootname);
  }

   void setRootName(std::string rootname){ m_rootname = rootname; }
  void setFjetScale(double scale){ m_fjetScale = scale; }

  bool isBad(const std::vector<float>* ptJet,
	     const std::vector<float>* BCH_CORR_JET,
	     const std::vector<float>* BCH_CORR_CELL,
	     const std::vector<float>* BCH_CORR_DOTX,
	     const std::vector<float>* phiJet,
	     double etxMet,
	     double etyMet,
	     double fakeCosPhiThreshold=10.*1000,
	     double significanceCosPhiThreshold=10.,
	     double fakeThreshold=-1,
	     double significanceThreshold=-1){
    
    for(unsigned int i=0; i<ptJet->size(); i++){
      bool isbad = isBad(ptJet->at(i),BCH_CORR_JET->at(i),BCH_CORR_CELL->at(i),BCH_CORR_DOTX->at(i),phiJet->at(i),etxMet,etyMet,
			 fakeCosPhiThreshold,significanceCosPhiThreshold,fakeThreshold,significanceThreshold);
      if(isbad) return true;// bad event
    }
    return false;
  }

  bool isBadEmul(const std::vector<float>* ptJet,
             const std::vector<float>* etaJet,
             const std::vector<float>* phiJet,
             double etxMet,
             double etyMet,
	     const std::vector<float>* BCH_CORR_JET,
	     const std::vector<float>* BCH_CORR_CELL,
	     const std::vector<float>* BCH_CORR_DOTX,
             double fakeCosPhiThreshold=10.*1000,
             double significanceCosPhiThreshold=10.,
             double fakeThreshold=-1,
             double significanceThreshold=-1){

    for(unsigned int i=0; i<ptJet->size(); i++){
      bool isbad = isBadEmul(ptJet->at(i),etaJet->at(i),phiJet->at(i),etxMet,etyMet,BCH_CORR_JET->at(i),BCH_CORR_CELL->at(i),BCH_CORR_DOTX->at(i),
                         fakeCosPhiThreshold,significanceCosPhiThreshold,fakeThreshold,significanceThreshold);
      if(isbad) return true;// bad event
    }
    return false;
  }

  // emulate scale shift in detector conditions, gives ptJet vector modified after call, correctionEx,Ey is the propagation to MET
  double emulation(double& correctionEx,
                   double& correctionEy,
                   std::vector<float>* ptJet,
                   const std::vector<float>* etaJet,
                   const std::vector<float>* phiJet,
                   const std::vector<float>* BCH_CORR_JET, 
                   const std::vector<float>* BCH_CORR_CELL,
                   const std::vector<float>* BCH_CORR_DOTX){
    double fake=0;
    correctionEx=0;
    correctionEy=0;
    for(unsigned int i=0; i<ptJet->size(); i++){
      double fakeEx=0;
      double fakeEy=0;
      double ptJetEmul=ptJet->at(i);

      fake += emulation(fakeEx,fakeEy,ptJetEmul,etaJet->at(i),phiJet->at(i),BCH_CORR_JET->at(i),BCH_CORR_CELL->at(i),BCH_CORR_DOTX->at(i));

      correctionEx += fakeEx;
      correctionEy += fakeEy;
      (*ptJet)[i] = static_cast<float>(ptJetEmul);
    }
    return fake;
  }

  // correct MET by estimator
  double correction(double& correctionEx,
                    double& correctionEy,
		    const std::vector<float>* ptJet, 
		    const std::vector<float>* BCH_CORR_JET, 
		    const std::vector<float>* BCH_CORR_CELL,
		    const std::vector<float>* BCH_CORR_DOTX,
		    const std::vector<float>* phiJet){
    double fake=0;
    correctionEx=0;
    correctionEy=0;
    for(unsigned int i=0; i<ptJet->size(); i++){
      double fakeEx=0; 
      double fakeEy=0; 

      fake += correction(fakeEx,fakeEy,ptJet->at(i),BCH_CORR_JET->at(i),BCH_CORR_CELL->at(i),BCH_CORR_DOTX->at(i),phiJet->at(i)); 

      correctionEx += fakeEx;
      correctionEy += fakeEy;
    }
    return fake;
  }
  
  bool isBad(double ptReconstructed,
	     double BCH_CORR_JET,
	     double BCH_CORR_CELL, 
	     double BCH_CORR_DOTX,
	     double phiJet, 
	     double etxMet, 
	     double etyMet, 
	     double fakeCosPhiThreshold=10.*1000,
	     double significanceCosPhiThreshold=10.,
	     double fakeThreshold=-1, 
	     double significanceThreshold=-1){// negative for OFF, MeV unit for threshold
    if(fakeThreshold<0 && fakeCosPhiThreshold<0 && significanceThreshold<0 && significanceCosPhiThreshold<0) return false;

    double phiMet = (etxMet!=0)? atan2(etyMet,etxMet) : 0;
    double etMet = sqrt( pow(etxMet,2) + pow(etyMet,2) );

    if(fakeThreshold>=0){
      double fake = fakeCorrectedAndObserved(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
      if(fake<fakeThreshold) return false;
    }
    if(fakeCosPhiThreshold>=0){
      double fakeCosPhi = fakeCorrectedAndObservedCosPhi(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX,phiJet,phiMet);
      if(fakeCosPhi<fakeCosPhiThreshold) return false;
    }
    if(significanceThreshold>=0){
      double significance = significanceCorrectedAndObserved(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX,etMet);
      if(significance>significanceThreshold || significance<0.) return false;
    } 
    if(significanceCosPhiThreshold>=0){
      double significanceCosPhi = significanceCorrectedAndObservedCosPhi(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX,phiJet,phiMet,etMet);
      if(significanceCosPhi>significanceCosPhiThreshold || significanceCosPhi<0.) return false;
    }
    return true;// bad event
  }

  bool isBadEmul(double ptJet,
             double etaJet,
             double phiJet,
             double etxMet,
             double etyMet,
             double BCH_CORR_JET,
             double BCH_CORR_CELL,
             double BCH_CORR_DOTX,
             double fakeCosPhiThreshold=10.*1000,
             double significanceCosPhiThreshold=10.,
             double fakeThreshold=-1,
             double significanceThreshold=-1){// negative for OFF, MeV unit for threshold
    if(fakeThreshold<0 && fakeCosPhiThreshold<0 && significanceThreshold<0 && significanceCosPhiThreshold<0) return false;

    double phiMet = (etxMet!=0)? atan2(etyMet,etxMet) : 0;
    double etMet = sqrt( pow(etxMet,2) + pow(etyMet,2) );

    if(fakeThreshold>=0){
      double fake = fakeCorrectedAndObservedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
      if(fake<fakeThreshold) return false;
    }
    if(fakeCosPhiThreshold>=0){
      double fakeCosPhi = fakeCorrectedAndObservedCosPhiEmul(ptJet,etaJet,phiJet,phiMet,etMet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
      if(fakeCosPhi<fakeCosPhiThreshold) return false;
    }
    if(significanceThreshold>=0){
      double significance = significanceCorrectedAndObservedEmul(ptJet,etaJet,phiJet,phiMet,etMet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
      if(significance>significanceThreshold || significance<0.) return false;
    }
    if(significanceCosPhiThreshold>=0){
      double significanceCosPhi = significanceCorrectedAndObservedCosPhiEmul(ptJet,etaJet,phiJet,phiMet,etMet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
      if(significanceCosPhi>significanceCosPhiThreshold || significanceCosPhi<0.) return false;
    }
    return true;// bad event
  }

  // emulate scale shift in detector conditions
  double emulation(double& correctionEx,
                   double& correctionEy,
                   double& ptJet,
                   double etaJet,
                   double phiJet,
                   double BCH_CORR_JET,
                   double BCH_CORR_CELL,
                   double BCH_CORR_DOTX){
    double ptJetEmul = ptReconstructedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
    double fake = ptJetEmul - ptJet;
    correctionEx = fake * cos(phiJet);
    correctionEy = fake * sin(phiJet);
    ptJet = ptJetEmul;
    return fake;//absolute value of correction
  }
 
  // correct MET by estimator 
  double correction(double& correctionEx,
		    double& correctionEy,
		    double ptReconstructed, 
		    double BCH_CORR_JET, 
		    double BCH_CORR_CELL,
		    double BCH_CORR_DOTX,
		    double phiJet){
    double fake = fakeCorrectedAndReconstructed(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
    correctionEx = fake * cos(phiJet);
    correctionEy = fake * sin(phiJet);
    return fake;//absolute value of correction
  }

  // fcor/fjet
  double fcor(double BCH_CORR_CELL, double BCH_CORR_DOTX) { return BCH_CORR_CELL+BCH_CORR_DOTX; }
  double fjet(double BCH_CORR_JET) { return BCH_CORR_JET*m_fjetScale; }

  // emulation of fcor/fjet
  double femul(EmulFakeMet::FType itype, double pt, double eta, double phi) {
    if(m_emul==0){
      m_emul = new EmulFakeMet(m_rootname);
    }
    return m_emul->emul(itype,pt,eta,phi);
  }
  double fcor(double pt, double eta, double phi) { 
    double f=femul(EmulFakeMet::FCOR,pt,eta,phi);
    if(fabs(f)<0.01) return 0;
    else return f;
  }
  double fjet(double pt, double eta, double phi) {
    double f=femul(EmulFakeMet::FJET,pt,eta,phi);
    if(fabs(f)<0.01) return 0;
    else return f*m_fjetScale;
  }
  
  // pt(reconstructed) <emulation>
  double ptReconstructedEmul(double ptJet,
                         double etaJet,
                         double phiJet,
                         double BCH_CORR_JET,
                         double BCH_CORR_CELL,
                         double BCH_CORR_DOTX){
    double ptCorr = ptCorrected(ptJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
    return ptObservedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX)/(1-fcor(ptCorr,etaJet,phiJet));
  }

  // pt(observed) without any correction
  double ptObserved(double ptReconstructed,
                    double BCH_CORR_CELL,
                    double BCH_CORR_DOTX){
    return ptReconstructed*(1-fcor(BCH_CORR_CELL,BCH_CORR_DOTX));
  }

  // pt(observed) without any correction <emulation>
  double ptObservedEmul(double ptJet,
                        double etaJet,
                        double phiJet,
                        double BCH_CORR_JET,
                        double BCH_CORR_CELL,
                        double BCH_CORR_DOTX){
    double ptCorr = ptCorrected(ptJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
    return ptCorr*(1-fjet(ptCorr,etaJet,phiJet));
  }
 
 
  // pt(observed) corrected by jet profile method
  double ptCorrected(double ptReconstructed,
		     double BCH_CORR_JET, 
		     double BCH_CORR_CELL,
		     double BCH_CORR_DOTX){
    return ptObserved(ptReconstructed,BCH_CORR_CELL,BCH_CORR_DOTX)/(1-fjet(BCH_CORR_JET));
  }

  // pt(observed) corrected by jet profile method <emulation>
  double ptCorrectedEmul(double ptJet,
                         double BCH_CORR_JET,
                         double BCH_CORR_CELL,
                         double BCH_CORR_DOTX){
    return ptCorrected(ptJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
  }
 
 
  // fake_jet ... estimated fake pT by jet profile method
  double fakeCorrectedAndObserved(double ptReconstructed,
				  double BCH_CORR_JET,
				  double BCH_CORR_CELL,
				  double BCH_CORR_DOTX){
    return ptCorrected(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX)-ptObserved(ptReconstructed,BCH_CORR_CELL,BCH_CORR_DOTX);
  }
 
  // fake_jet ... estimated fake pT by jet profile method <emulation>
  double fakeCorrectedAndObservedEmul(double ptJet,
                                      double etaJet,
                                      double phiJet,
                                      double BCH_CORR_JET,
                                      double BCH_CORR_CELL,
                                      double BCH_CORR_DOTX){
    return ptCorrectedEmul(ptJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX)-ptObservedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
  }

 
  // fake_diff ... difference between jet profile method and default corrections
  double fakeCorrectedAndReconstructed(double ptReconstructed,
				       double BCH_CORR_JET,
				       double BCH_CORR_CELL,
				       double BCH_CORR_DOTX){
    return ptCorrected(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX)-ptReconstructed;
  }

  // fake_diff ... difference between jet profile method and default corrections <emulation>
  double fakeCorrectedAndReconstructedEmul(double ptJet,
                                           double etaJet,
                                           double phiJet,
                                           double BCH_CORR_JET,
                                           double BCH_CORR_CELL,
                                           double BCH_CORR_DOTX){
    return ptCorrectedEmul(ptJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX)-ptReconstructedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
  }

  // fake_diff ... the amount of default corrections
  double fakeReconstructedAndObserved(double ptReconstructed,
                                      double BCH_CORR_CELL,
                                      double BCH_CORR_DOTX){
    return ptReconstructed-ptObserved(ptReconstructed,BCH_CORR_CELL,BCH_CORR_DOTX);
  }

  // fake_diff ... the amount of default corrections <emulation>
  double fakeReconstructedAndObservedEmul(double ptJet,
                                          double etaJet,
                                          double phiJet,
                                          double BCH_CORR_JET,
                                          double BCH_CORR_CELL,
                                          double BCH_CORR_DOTX){
    return ptReconstructedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX)-ptObservedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
  }

 
  // Met / fake_jet
  double significanceCorrectedAndObserved(double ptReconstructed,
                                          double BCH_CORR_JET,
                                          double BCH_CORR_CELL,
                                          double BCH_CORR_DOTX,
                                          double etMet){
    double fake = fakeCorrectedAndObserved(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);
    if (fake==0)
      return LARGEVALUE;
    else
      return etMet/fake;
  }

  // Met / fake_jet <emulation>
  double significanceCorrectedAndObservedEmul(double ptJet,
                                              double etaJet,
                                              double phiJet,
                                              double phiMet,
                                              double etMet,
                                              double BCH_CORR_JET,
                                              double BCH_CORR_CELL,
                                              double BCH_CORR_DOTX){
    double fake = fakeCorrectedAndObservedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);

    // modification of MET
    double fakeOrg = fakeReconstructedAndObserved(ptJet,BCH_CORR_CELL,BCH_CORR_DOTX);
    addMet(phiMet,etMet,phiJet,fake-fakeOrg);

    if (fake==0)
      return LARGEVALUE;
    else
      return etMet/fake;
  }

  // fake_jet x cos (phi(Jet) - phi(Met)) ... projection to the Met direction (contribution on Met)
  double fakeCorrectedAndObservedCosPhi(double ptReconstructed,
                                        double BCH_CORR_JET,
                                        double BCH_CORR_CELL,
                                        double BCH_CORR_DOTX,
                                        double phiJet,
                                        double phiMet){
    return fakeCorrectedAndObserved(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX)*cos(phiJet-phiMet);
  }

 
  // fake_jet x cos (phi(Jet) - phi(Met)) <emulation>... projection to the Met direction (contribution on Met)
  double fakeCorrectedAndObservedCosPhiEmul(double ptJet,
					double etaJet,
					double phiJet,
					double phiMet,
					double etMet,
                                        double BCH_CORR_JET,
                                        double BCH_CORR_CELL,
                                        double BCH_CORR_DOTX){
    double fake = fakeCorrectedAndObservedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);

    // modification of MET
    double fakeOrg = fakeReconstructedAndObserved(ptJet,BCH_CORR_CELL,BCH_CORR_DOTX);
    addMet(phiMet,etMet,phiJet,fake-fakeOrg);

    return fake*cos(phiJet-phiMet);
  }
  
  // Met / ( fake_jet x cos(dphi) )
  double significanceCorrectedAndObservedCosPhi(double ptReconstructed,
						    double BCH_CORR_JET, 
						    double BCH_CORR_CELL, 
						    double BCH_CORR_DOTX,
						    double phiJet,
						    double phiMet,
						    double etMet){
    double fakeCosPhi = fakeCorrectedAndObservedCosPhi(ptReconstructed,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX,phiJet,phiMet);
    if (fakeCosPhi==0)
      return LARGEVALUE;
    else
      return etMet/fakeCosPhi;
  }
 
  // Met / ( fake_jet x cos(dphi) ) <emulation>
  double significanceCorrectedAndObservedCosPhiEmul(double ptJet,
                                                    double etaJet,
                                                    double phiJet,
                                                    double phiMet,
                                                    double etMet,
                                                    double BCH_CORR_JET,
                                                    double BCH_CORR_CELL,
                                                    double BCH_CORR_DOTX){
    double fake = fakeCorrectedAndObservedEmul(ptJet,etaJet,phiJet,BCH_CORR_JET,BCH_CORR_CELL,BCH_CORR_DOTX);

    // modification of MET
    double fakeOrg = fakeReconstructedAndObserved(ptJet,BCH_CORR_CELL,BCH_CORR_DOTX);
    addMet(phiMet,etMet,phiJet,fake-fakeOrg);

    double fakeCosPhi = fake*cos(phiJet-phiMet);

    if (fakeCosPhi==0)
      return LARGEVALUE;
    else
      return etMet/fakeCosPhi;
  }

  // helper function
  void addMet(double& phiMet, double& etMet, double phiAdd, double etAdd){
    double etxMet = etMet*cos(phiMet)+etAdd*cos(phiAdd);
    double etyMet = etMet*sin(phiMet)+etAdd*sin(phiAdd);
    phiMet = (etxMet!=0)? atan2(etyMet,etxMet) : 0; 
    etMet = sqrt(pow(etxMet,2)+pow(etyMet,2));
  }
 
  enum CValue {LARGEVALUE=9999};

 private:
  std::string m_rootname; 
  EmulFakeMet* m_emul; 
  double m_fjetScale;
};

#endif


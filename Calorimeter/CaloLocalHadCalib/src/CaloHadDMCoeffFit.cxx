/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloHadDMCoeffFit.cxx,v 1.1 2009/03/03 17:30:23 pospelov Exp $
//
// Description: see CaloHadDMCoeffFit.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------
#include "CaloLocalHadCalib/CaloHadDMCoeffFit.h"
#include "CaloLocalHadCalib/GetLCDefs.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffData.h"
#include "CaloLocalHadCalib/CaloLocalHadCoeffHelper.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloLocalHadCalib/GetLCSinglePionsPerf.h"
#include "AthenaKernel/Units.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <CLHEP/Vector/LorentzVector.h>

#include "TROOT.h"
#include "TStyle.h"
#include "TError.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH1F.h"
#include "TF1.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TText.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TProfile2D.h"


using CLHEP::HepLorentzVector;
using Athena::Units::MeV;
using Athena::Units::GeV;



CaloHadDMCoeffFit::CaloHadDMCoeffFit() :
  m_isTestbeam(false),
  m_energyMin(200*MeV), m_weightMax(5.0), m_NormalizationType("Lin"),
  m_NormalizationTypeNumber(0)
{
  m_data = 0;
  m_HadDMCoeff = 0;
  m_HadDMHelper = new CaloLocalHadCoeffHelper();
  m_distance_cut = 1.5;
}


CaloHadDMCoeffFit::~CaloHadDMCoeffFit()
{
  clear();
  delete m_HadDMHelper;
}


CaloLocalHadCoeff * CaloHadDMCoeffFit::process(CaloHadDMCoeffData *myData, CaloLocalHadCoeff *myHadDMCoeff, bool isSingleParticle, bool tbflag)
{
  m_isTestbeam = tbflag;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "--- CaloHadDMCoeffFit::process() --- " << std::endl;

  if(m_isTestbeam) std::cout << "Processing TESTBEAM data" << std::endl;

  if ( m_NormalizationType == "Lin" ) {
    std::cout << "Using weighting proportional to E_calib" << std::endl;
    m_NormalizationTypeNumber = GetLCDefs::LIN;
  } else if ( m_NormalizationType == "Log" ) {
    std::cout << "Using weighting proportional to log(E_calib)" << std::endl;
    m_NormalizationTypeNumber = GetLCDefs::LOG;
  } else if ( m_NormalizationType == "NClus" ) {
    std::cout << "Using weighting proportional to 1/N_Clus_E_calib>0" << std::endl;
    m_NormalizationTypeNumber = GetLCDefs::NCLUS;
  } else {
    std::cout << "Using constant weighting" << std::endl;
    m_NormalizationTypeNumber = GetLCDefs::CONST;
  }

  m_data = myData;
  m_HadDMCoeff = myHadDMCoeff;

  // loading only necessary branches
  m_data->fChain->SetBranchStatus("*",0);
  m_data->fChain->SetBranchStatus("mc_ener",1);
  m_data->fChain->SetBranchStatus("mc_eta",1);
  m_data->fChain->SetBranchStatus("mc_phi",1);
  m_data->fChain->SetBranchStatus("mc_pdg",1);
  m_data->fChain->SetBranchStatus("ncls",1);
  m_data->fChain->SetBranchStatus("cls_eta",1);
  m_data->fChain->SetBranchStatus("cls_phi",1);
  m_data->fChain->SetBranchStatus("cls_lambda",1);
  m_data->fChain->SetBranchStatus("cls_calib_emfrac",1);
  m_data->fChain->SetBranchStatus("cls_engcalib",1);
  m_data->fChain->SetBranchStatus("engClusSumCalib",1);
  m_data->fChain->SetBranchStatus("cls_ener_unw",1);
  m_data->fChain->SetBranchStatus("narea",1);
  m_data->fChain->SetBranchStatus("cls_eprep",1);
  m_data->fChain->SetBranchStatus("cls_dmener",1);
  //m_data->fChain->SetBranchStatus("cls_smpener_unw",1);
  //m_data->fChain->SetBranchStatus("cls_oocener",1);
  //m_data->fChain->SetBranchStatus("cls_engcalibpres",1);

  if( !m_data->GetEntries() ) {
    std::cout << "CaloHadDMCoeffFit::process -> Error! No entries in DeadMaterialTree." << std::endl;
    return 0;
  }

  m_data->GetEntry(0);
  if(m_data->m_narea != m_HadDMCoeff->getSizeAreaSet()) {
    std::cout << "CaloHadDMCoeffFit::process -> Error! Different numbers of areas for DM definition" << std::endl;
    std::cout << "m_data->m_narea:" << m_data->m_narea << " m_HadDMCoeff->getSizeAreaSet():" << m_HadDMCoeff->getSizeAreaSet() << std::endl;
    return 0;
  }

  clear();
  for(int i_size=0; i_size<m_HadDMCoeff->getSizeCoeffSet(); i_size++){
    m_engClus.push_back(new PrepData());
    m_engPrep.push_back(new PrepData());
    m_engDm.push_back(new PrepData());
    m_engDmOverClus.push_back(new PrepData());
  }


  // --------------------------------------------------------------------------
  // first run to calculate average and rms for histogram limits
  // --------------------------------------------------------------------------
  std::cout << "CaloHadDMCoeffFit::process() -> Info. Getting averages..." << std::endl;
  for(int i_ev=0; m_data->GetEntry(i_ev)>0;i_ev++) {
    if(i_ev%20000==0) std::cout << "    i_ev: " << i_ev << " '" << ((TChain *)m_data->fChain)->GetFile()->GetName() << "'" << std::endl;

    // checking event quality
    if(isSingleParticle) {
      bool GoodClusterFound(false);
      if( m_data->m_ncls )  {
        HepLorentzVector hlv_pion(1,0,0,1);
        hlv_pion.setREtaPhi(1./cosh(m_data->m_mc_eta), m_data->m_mc_eta, m_data->m_mc_phi);
        for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
          HepLorentzVector hlv_cls(1,0,0,1);
          hlv_cls.setREtaPhi(1./cosh( (*m_data->m_cls_eta)[i_cls] ), (*m_data->m_cls_eta)[i_cls], (*m_data->m_cls_phi)[i_cls]);
          double r = hlv_pion.angle(hlv_cls.vect());
          if(r < m_distance_cut*GetLCSinglePionsPerf::angle_mollier_factor(m_data->m_mc_eta) 
            && (*m_data->m_cls_engcalib)[i_cls] > 20.0*MeV 
            //&& (*m_data->m_cls_ener)[i_cls] > 0.01*m_data->m_mc_ener
            ) {
            GoodClusterFound = true;
            break;
          }
        } // i_cls
      }
      if(!GoodClusterFound) continue;
    }

    if(m_data->m_engClusSumCalib <=0.0) continue;
    for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
      if( (*m_data->m_cls_ener_unw)[i_cls] < m_energyMin) continue;

      double clus_weight = 1.0;
      if(m_NormalizationTypeNumber == GetLCDefs::LIN) {
        clus_weight = (*m_data->m_cls_engcalib)[i_cls]/m_data->m_engClusSumCalib;
      }
      if(clus_weight <= 0) continue;
      for(int i_dma=0; i_dma<m_data->m_narea; i_dma++){ // loop over DM areas
        std::vector<float> vars;
        m_data->PackClusterVars(i_cls, vars);
        int iBin = m_HadDMCoeff->getBin(i_dma, vars );

        if(iBin < 0) continue;
        if(iBin >= m_HadDMCoeff->getSizeCoeffSet()) {
          std::cout << "Panic'! iBin " << iBin << " " << m_HadDMCoeff->getSizeCoeffSet() << std::endl;
          continue;
        }
        m_engClus[iBin]->add( (*m_data->m_cls_ener_unw)[i_cls] );
        m_engPrep[iBin]->add( (*m_data->m_cls_eprep)[i_cls][i_dma] );
        m_engDm[iBin]->add( (*m_data->m_cls_dmener)[i_cls][i_dma] );
        if((*m_data->m_cls_ener_unw)[i_cls] <=0.0) {
          std::cout << "Panic! Zero cluster energy" << std::endl;
          continue;
        }
        double w = (*m_data->m_cls_dmener)[i_cls][i_dma]/(*m_data->m_cls_ener_unw)[i_cls];
        if(w>=0 && w<m_weightMax) {
          m_engDmOverClus[iBin]->add(w, clus_weight);
        }
      } // i_dma
    } // i_cls
  } // i_ev
  std::cout << "done" << std::endl;
  std::vector<double > engDmOverClusLim;
  engDmOverClusLim.resize(m_HadDMCoeff->getSizeCoeffSet(), 0.0);
  for(int i_size=0; i_size<m_HadDMCoeff->getSizeCoeffSet(); i_size++){
    engDmOverClusLim[i_size] = m_engDmOverClus[i_size]->m_aver + 3.0*sqrt(m_engDmOverClus[i_size]->m_rms);
  }

  // --------------------------------------------------------------------------
  // creation of histograms
  // --------------------------------------------------------------------------
  std::cout << "CaloHadDMCoeffFit::process() -> Info. Creation of histograms..." << std::endl;
  m_h2_DmVsPrep.resize(m_HadDMCoeff->getSizeCoeffSet(), 0);
  m_hp_DmVsPrep.resize(m_HadDMCoeff->getSizeCoeffSet(), 0);
  m_h1_engDmOverClus.resize(m_HadDMCoeff->getSizeCoeffSet(), 0);
  m_hp2_DmWeight.resize(m_HadDMCoeff->getSizeCoeffSet(), 0);

  char hname[1024];
  int nch_dms=40;
  // creation of profiles
  for(int i_size=0; i_size<m_HadDMCoeff->getSizeCoeffSet(); i_size++){
    const CaloLocalHadCoeff::LocalHadArea *area = m_HadDMCoeff->getAreaFromBin(i_size);
    if( area->getType() == CaloLocalHadDefs::AREA_DMFIT) {
      // eprep .vs. edmtrue
      sprintf(hname,"hp_DmVsPrep_%d",i_size);
//       float x_lim_edm =  m_engDm[i_size]->m_aver + 3.0*sqrt(m_engDm[i_size]->m_rms);
//       float x_lim_eprep =  m_engPrep[i_size]->m_aver + 3.0*sqrt(m_engPrep[i_size]->m_rms);
      float x_lim_edm =  2.0*m_engDm[i_size]->m_aver + 3.0*sqrt(m_engDm[i_size]->m_rms);
      float x_lim_eprep =  2.0*m_engPrep[i_size]->m_aver + 3.0*sqrt(m_engPrep[i_size]->m_rms);
      if(x_lim_edm <= 0.0) x_lim_edm = 1.0;
      if(x_lim_eprep <= 0.0) x_lim_eprep = 1.0;
      m_hp_DmVsPrep[i_size] = new TProfile(hname,hname,nch_dms,0., x_lim_edm);
      m_hp_DmVsPrep[i_size]->Sumw2();
      sprintf(hname,"h2_DmVsPrep_%d",i_size);
      m_h2_DmVsPrep[i_size] = new TH2F(hname,hname,nch_dms/2,0.0, x_lim_edm, nch_dms/2,0., x_lim_eprep);
    }
    if( area->getType() == CaloLocalHadDefs::AREA_DMLOOKUP) {
      sprintf(hname,"h1_engDmOverClus_%d",i_size);
//       float xlim = 2.0*m_engDmOverClus[i_size]->m_aver + 5.0*sqrt(m_engDmOverClus[i_size]->m_rms);
//       if(xlim < 1.5) xlim = 1.5;
      //m_h1_engDmOverClus[i_size] = new TH1F(hname,hname, nch_dms*2, -0.5, xlim );
      m_h1_engDmOverClus[i_size] = new TH1F(hname,hname, 150, -0.5, m_weightMax );

      // creation of histograms for inverted dm weight .vs. log10(ener), log10(lambda)
      int refbin = getFirstEnerLambdaBin(i_size);
      TProfile2D *hp2=0;
      if(refbin == i_size) {
         std::cout << " creating histos:" << hname << " refbin:" << refbin << std::endl;
        const CaloLocalHadCoeff::LocalHadDimension *dimEner = area->getDimension(CaloLocalHadCoeffHelper::DIM_ENER);
        const CaloLocalHadCoeff::LocalHadDimension *dimLambda = area->getDimension(CaloLocalHadCoeffHelper::DIM_LAMBDA);
        sprintf(hname,"m_hp2_DmWeight_%d",i_size);
        hp2 = new TProfile2D(hname, hname, dimEner->getNbins(), dimEner->getXmin(), dimEner->getXmax(), dimLambda->getNbins(), dimLambda->getXmin(), dimLambda->getXmax(), 0.0, m_weightMax);
      }else if (refbin >= 0) {
        hp2 = m_hp2_DmWeight[refbin];
      }
      m_hp2_DmWeight[i_size] = hp2;
    }
  }

  // --------------------------------------------------------------------------
  // Filling of histograms
  // --------------------------------------------------------------------------
  std::cout << "CaloHadDMCoeffFit::process() -> Info. Filling histograms..." << std::endl;
  for(int i_ev=0; m_data->GetEntry(i_ev)>0;i_ev++) {
    if(i_ev%20000==0) std::cout << "    i_ev: " << i_ev << " '" << ((TChain *)m_data->fChain)->GetFile()->GetName() << "'" << std::endl;

    if(isSingleParticle) {
      // checking event quality
      bool GoodClusterFound(false);
      if( m_data->m_ncls )  {
        HepLorentzVector hlv_pion(1,0,0,1);
        hlv_pion.setREtaPhi(1./cosh(m_data->m_mc_eta), m_data->m_mc_eta, m_data->m_mc_phi);
        for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
          HepLorentzVector hlv_cls(1,0,0,1);
          hlv_cls.setREtaPhi(1./cosh( (*m_data->m_cls_eta)[i_cls] ), (*m_data->m_cls_eta)[i_cls], (*m_data->m_cls_phi)[i_cls]);
          double r = hlv_pion.angle(hlv_cls.vect());
          if(r < m_distance_cut*GetLCSinglePionsPerf::angle_mollier_factor(m_data->m_mc_eta) 
            && (*m_data->m_cls_engcalib)[i_cls] > 20.0*MeV 
            //&& (*m_data->m_cls_ener)[i_cls] > 0.01*m_data->m_mc_ener
            ) {
            GoodClusterFound = true;
            break;
          }
        } // i_cls
      }
      if(!GoodClusterFound) continue;
    }

    //if(m_data->m_engClusSumCalib <=0.0) continue;
    for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
      if( (*m_data->m_cls_ener_unw)[i_cls] < m_energyMin) continue;

      double clus_weight = 1.0;
      if(m_NormalizationTypeNumber == GetLCDefs::LIN) {
        clus_weight = (*m_data->m_cls_engcalib)[i_cls]/m_data->m_engClusSumCalib;
      }
      if(clus_weight <= 0.0) continue;
      for(int i_dma=0; i_dma<m_data->m_narea; i_dma++){ // loop over DM areas
        std::vector<float> vars;
        m_data->PackClusterVars(i_cls, vars);
        int iBin = m_HadDMCoeff->getBin(i_dma, vars );

        if(iBin < 0) continue;
        if( (*m_data->m_cls_eprep)[i_cls][i_dma] > 0.0 && m_hp_DmVsPrep[iBin]){
          m_hp_DmVsPrep[iBin]->Fill( (*m_data->m_cls_dmener)[i_cls][i_dma], (*m_data->m_cls_eprep)[i_cls][i_dma], clus_weight );
          m_h2_DmVsPrep[iBin]->Fill( (*m_data->m_cls_dmener)[i_cls][i_dma], (*m_data->m_cls_eprep)[i_cls][i_dma], clus_weight );
        }
        double w = (*m_data->m_cls_dmener)[i_cls][i_dma]/(*m_data->m_cls_ener_unw)[i_cls];
        if(m_h1_engDmOverClus[iBin] && w>=0 && w<m_weightMax) {
          m_h1_engDmOverClus[iBin]->Fill(w, clus_weight);
        }
        if( m_hp2_DmWeight[iBin] ) {
          //double isol = (*m_data->m_cls_isol)[i_cls];
          m_hp2_DmWeight[iBin]->Fill(vars[CaloLocalHadCoeffHelper::DIM_ENER],vars[CaloLocalHadCoeffHelper::DIM_LAMBDA], w, clus_weight);
        }

      } // i_dma
    } // i_cls
  } // i_ev

  // --------------------------------------------------------------------------
  // Fitting histograms
  // --------------------------------------------------------------------------
  std::cout << "CaloHadDMCoeffFit::process() -> Info. Fitting histograms..." << std::endl;
  TF1 *fitFun = new TF1("fitFun","[0]+[1]*pow(x,[2])",1.,200000.);
  double start_pars[3] = {100.0, 1.0, 1.0};
  double start_errs[3] = {1.0, 1.0, 0.01};
  // fitting profiles in silent mode
  for(int i_size=0; i_size<m_HadDMCoeff->getSizeCoeffSet(); i_size++){
    const CaloLocalHadCoeff::LocalHadArea *area = m_HadDMCoeff->getAreaFromBin(i_size);
    if( area->getType() != CaloLocalHadDefs::AREA_DMFIT) continue;
    TProfile *hp= m_hp_DmVsPrep[i_size];
    if(hp->GetEntries() <25) continue;
    for(int ip=0; ip<fitFun->GetNpar(); ip++) fitFun->SetParameter(ip,start_pars[ip]);
    fitFun->SetParErrors(start_errs);
    fitFun->FixParameter(2,1.0);
    std::cout << "----------------------" << std::endl;
    double fitlim1 = hp->GetBinLowEdge(1);
    double fitlim2 = ProfileRefiner(hp);
    //std::cout << "area->getTitle():" << i_size << " " << area->getTitle() << " hlim:"<< hp->GetBinCenter(hp->GetNbinsX()) + hp->GetBinWidth(hp->GetNbinsX())/2. << std::endl;
    if(area->getTitle()=="ENG_CALIB_DEAD_TILEG3") {
      //std::cout << "  xxx ENG_CALIB_DEAD_TILEG3 " << std::endl;
      fitlim2 = ProfileRefiner(hp,0.85);
    }else{
      fitlim2 = ProfileRefiner(hp, 0.98);
    }
    //std::cout << "  xxx fitlim2:" << fitlim2<< std::endl;
    hp->Fit(fitFun,"0Q","",fitlim1,fitlim2); // silent fit (drawing option for fit results is switched off)
    TF1 *f=hp->GetFunction(fitFun->GetName());
    if(f) {
      f->ResetBit(TF1::kNotDraw);
      f->SetLineWidth(1);
      f->SetLineColor(2);
    } 
  } // i_size

  // --------------------------------------------------------------------------
  // Making output coefficients
  // --------------------------------------------------------------------------
  std::cout << "CaloHadDMCoeffFit::process() -> Info. Making new coefficients ... " << std::endl;
  CaloLocalHadCoeff *new_data = new CaloLocalHadCoeff(*m_HadDMCoeff);

  m_FitData.clear();
  m_FitData.resize(m_HadDMCoeff->getSizeCoeffSet(), 0);

  for(int i_size=0; i_size<(int)new_data->getSizeCoeffSet(); i_size++){
    const CaloLocalHadCoeff::LocalHadArea *dmArea = m_HadDMCoeff->getAreaFromBin(i_size);
    int n_area;
    for(n_area=0; n_area<m_HadDMCoeff->getSizeAreaSet(); n_area++){
      if(dmArea == m_HadDMCoeff->getArea(n_area)) {
        break;
      }
    }

    const CaloLocalHadCoeff::LocalHadCoeff *old_pars = m_HadDMCoeff->getCoeff(i_size);
    CaloLocalHadCoeff::LocalHadCoeff pars = *old_pars;

    std::vector<int > indexes; //  DIM_EMFRAC, DIM_SIDE, DIM_ETA, DIM_PHI, DIM_ENER, DIM_LAMBDA
    m_HadDMCoeff->bin2indexes(i_size, indexes);
    const CaloLocalHadCoeff::LocalHadDimension *dimEner = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ENER);
    const CaloLocalHadCoeff::LocalHadDimension *dimEta = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ETA);
    float ener = dimEner->getXmin() + dimEner->getDx()*(indexes[CaloLocalHadCoeffHelper::DIM_ENER]);
    float eta = dimEta->getXmin() + dimEta->getDx()*(indexes[CaloLocalHadCoeffHelper::DIM_ETA]+0.5);

    if( dmArea->getType() == CaloLocalHadDefs::AREA_DMFIT){
      TF1 *fitFun = m_hp_DmVsPrep[i_size]->GetFunction("fitFun");
      if(fitFun){
        float p0 = fitFun->GetParameter(0);
        float s0 = fitFun->GetParError(0);
        float p1 = fitFun->GetParameter(1);
        float s1 = fitFun->GetParError(1);
        m_FitData[i_size] = new FitData(p0, s0, p1, s1);
        // checking fit quality
        if(fitFun->GetNDF() >= 2 && fitFun->GetChisquare()/fitFun->GetNDF() < 100. && p1 !=0.0  && p1>0.2) {
        //if(fitFun->GetNDF() > 2 && fitFun->GetChisquare()/fitFun->GetNDF() < 15. && p1 !=0.0 && s1<fabs(p1) && p1>0.05) {
          m_FitData[i_size]->isOK = true;
          m_FitData[i_size]->descr = std::string("OK");
          pars[0] = -1.0*p0/p1;
          pars[1] = 1./p1;
          pars[2] = 1.0;
          std::cout << "i_size ok " << i_size << " " << m_FitData[i_size]->descr << std::endl;
        }else{
          m_FitData[i_size]->isOK = false;
          m_FitData[i_size]->descr = std::string("failed");
          if(fitFun->GetNDF() < 2 || fitFun->GetChisquare()/fitFun->GetNDF() > 100.) m_FitData[i_size]->descr += std::string(" NDFCHI");
          if(p1==0 || p1<0.2) m_FitData[i_size]->descr += std::string(" p1");
          //if(s1<fabs(p1)) m_FitData[i_size]->descr += std::string(" s1");
          std::cout << "i_size failed " <<  i_size << " " << m_FitData[i_size]->descr << std::endl;
        }
      }else{
        m_FitData[i_size] = new FitData();
        m_FitData[i_size]->isOK = false;
        m_FitData[i_size]->descr = "failed noFitFun";
        std::cout << "i_size nofitfun " <<  i_size << " " << m_FitData[i_size]->descr << std::endl;
      }

//       if(m_isTestbeam) {
//         // setting coefficients for neutral pions in between emec and hec equal to coefficients of charged pions
//         if(dmArea->getTitle() == "ENG_CALIB_DEAD_HEC0" && indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==1) {
//           std::vector<int > m_tmp = indexes;
//           m_tmp[CaloLocalHadCoeffHelper::DIM_EMFRAC]=0;
//           int n_area;
//           for(n_area=0; n_area<m_HadDMCoeff->getSizeAreaSet(); n_area++){
//             if(dmArea == m_HadDMCoeff->getArea(n_area)) {
//               break;
//             }
//           }
//           int iBin = m_HadDMCoeff->getBin( n_area, m_tmp);
//           const CaloLocalHadCoeff::LocalHadCoeff *had_pars = new_data->getCoeff(iBin);
//           pars[0] = (*had_pars)[0];
//           pars[1] = (*had_pars)[1];
//           pars[2] = (*had_pars)[2];
//         }
//       } // isTestbeam

        // neutral pions doesn't reach the material  between emec-hec and emb-tile, so normally the fit is screwed there
        // let's set fit coefficients for neutral pions in this DM area equal to the charged pion 
        if( indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==1 && (dmArea->getTitle() == "ENG_CALIB_DEAD_HEC0" || dmArea->getTitle() == "ENG_CALIB_DEAD_TILE0" ) ) {
          std::vector<int > tmp = indexes;
          tmp[CaloLocalHadCoeffHelper::DIM_EMFRAC]=0;
          int iBin = m_HadDMCoeff->getBin( n_area, tmp);
          const CaloLocalHadCoeff::LocalHadCoeff *had_pars = new_data->getCoeff(iBin);
          pars[0] = (*had_pars)[0];
          pars[1] = (*had_pars)[1];
          pars[2] = (*had_pars)[2];
          char str[128];
          sprintf(str, "Afrom %d",iBin);
          m_FitData[i_size]->descr += std::string(str);
          //std::cout << "xxx A i_size:" << i_size << " iBin:" << iBin  << " " << m_FitData[i_size]->descr << std::endl;
        }else if( indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==1 && (dmArea->getTitle() == "ENG_CALIB_DEAD_TILEG3") ) {
          // For material before tile scintillator things are weared, in some eta regions neutral pions have good correlation
          // between signal in scintillator and dead material energy, in some eta regions correlation is absent. So we select here
          // following approach: if fit failed, take coefficients from charged pion data and use them for neutral
          if(!m_FitData[i_size]->isOK) {
            std::vector<int > tmp = indexes;
            tmp[CaloLocalHadCoeffHelper::DIM_EMFRAC]=0;
            int iBin = m_HadDMCoeff->getBin( n_area, tmp);
            const CaloLocalHadCoeff::LocalHadCoeff *had_pars = new_data->getCoeff(iBin);
            pars[0] = (*had_pars)[0];
            pars[1] = (*had_pars)[1];
            pars[2] = (*had_pars)[2];
            char str[128];
            sprintf(str, "Afrom %d",iBin);
            m_FitData[i_size]->descr += std::string(str);
            std::cout << "xxx A i_size:" << i_size << " iBin:" << iBin  << " " << m_FitData[i_size]->descr << std::endl;
          }
        }else{
          // if fit went wrong, lets take fit results from neighboting eta bin
          if(!m_FitData[i_size]->isOK) {
            std::vector<int > tmp = indexes;
            if(tmp[CaloLocalHadCoeffHelper::DIM_ETA]>0) {
              tmp[CaloLocalHadCoeffHelper::DIM_ETA] -= 1;
            }else if(tmp[CaloLocalHadCoeffHelper::DIM_ETA]==0){
              tmp[CaloLocalHadCoeffHelper::DIM_ETA] += 1;
            }
            int iBin = m_HadDMCoeff->getBin( n_area, tmp);
            const CaloLocalHadCoeff::LocalHadCoeff *had_pars = new_data->getCoeff(iBin);
            pars[0] = (*had_pars)[0];
            pars[1] = (*had_pars)[1];
            pars[2] = (*had_pars)[2];
            char str[128];
            sprintf(str, "Bfrom %d",iBin);
            m_FitData[i_size]->descr += std::string(str);
            //std::cout << "xxx B i_size:" << i_size << " iBin:" << iBin << " " << m_FitData[i_size]->descr << std::endl;
          }
        }


    }else if(dmArea->getType() == CaloLocalHadDefs::AREA_DMLOOKUP){
      //if(dmArea->getTitle() == "ENG_CALIB_DEAD_LEAKAGE") {
        // right tail cut
        if( m_h1_engDmOverClus[i_size]->GetEntries() > 0) {
          double rcut = 0.92;
          if(m_isTestbeam){ // H6 testbeam
            rcut = 0.93;
            if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC] == 1)  rcut = 0.75;

            if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==0 && ener < 4.69 && fabs(eta)>2.8) {
              rcut = 0.85;
            }
            if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==1 && ener < 4.69 && fabs(eta)>3.0) {
              rcut = 0.85;
            }
          }else{ // atlas
            rcut = 0.95;
            if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC] == 1)  {
              rcut = 0.80;
              if(fabs(eta) > 1.44 && fabs(eta) < 1.56) {
                rcut = 0.60;
              }
            }
            if(fabs(eta) > 4.0) rcut = rcut*0.7;
          }
          double w = GetAverageWithoutRightTail(m_h1_engDmOverClus[i_size], rcut);
          pars[0] = 1.0+w;
        }else{
          pars[0] = 1.0;
        }
        pars[1] = float(m_h1_engDmOverClus[i_size]->GetEntries());
        pars[2] = float(m_engDm[i_size]->m_aver);

//       }else if (dmArea->getTitle() == "ENG_CALIB_DEAD_UNCLASS") {
//         if(m_isTestbeam) {
//           // right tail cut
//           if( m_h1_engDmOverClus[i_size]->GetEntries() > 0) {
//             //double rcut = 0.99;
//             double rcut = 0.93;
//             if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==0 && ener < 4.69 ) {
//               //rcut = 0.88;
//               rcut = 0.85;
//             }
//             if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC] == 1)  {
//               //rcut = 0.79; //clsw=lin
//               rcut = 0.85; //clsw=1.0
//             }
//             double w = GetAverageWithoutRightTail(m_h1_engDmOverClus[i_size], rcut);
//             pars[0] = 1.0+w;
//           }else{
//             pars[0] = 1.0;
//           }
//           pars[1] = float(m_h1_engDmOverClus[i_size]->GetEntries());
//           pars[2] = float(m_engDm[i_size]->m_aver);
// 
//         }else{ // Atlas
//           // simple average with weightMax rejection
//           if( m_hp2_DmWeight[i_size]->GetEntries() > 0) {
//             int binx = indexes[CaloLocalHadCoeffHelper::DIM_ENER] + 1;
//             int biny = indexes[CaloLocalHadCoeffHelper::DIM_LAMBDA] + 1;
//             double w = m_hp2_DmWeight[i_size]->GetBinContent(binx, biny);
//             // manual hack to decrease the level of correction
//             if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==1 && fabs(eta)>1.3 && fabs(eta)<1.7) {
//               if(ener < 4.5){
//                 w = w*0.78;
//               }else{
//                 w = w*0.92;
//               }
//             }
//             pars[0] = w + 1.0;
//             //pars[0] = w;
//             pars[1] = float(m_hp2_DmWeight[i_size]->GetBinEntries(m_hp2_DmWeight[i_size]->GetBin(binx, biny)));
//           }else{
//             pars[0] = 1.0;
//           }
//           pars[2] = float(m_engDm[i_size]->m_aver);
//         }
// 
//       }else{
//         std::cout << "Warning! Unknown lookup area " << dmArea->getTitle() << std::endl;
//       } 
    }
    new_data->setCoeff(i_size, pars);
  }

  return new_data;
}



/* ****************************************************************************
Making nice postscript report
**************************************************************************** */
void CaloHadDMCoeffFit::make_report(std::string &sreport)
{
  float etaRegions[][2] = { {0.0, 0.1}, {1.4, 1.7}, {2.0, 2.1}, {3.3, 3.4}, {4.0, 4.1} };
  std::cout << "CaloHadDMCoeffFit::make_report() -> Info. Making report..." << std::endl;
  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetTitleFillColor(10);
  int cc_xx = 768, cc_yy = 1024;
  char str[1024], hname[1024];
  TText tex;
  gROOT->SetBatch(kTRUE);
  gErrorIgnoreLevel=3; // root global variables to supress text output in ->Print() methods
  std::string sfname = sreport;
  TCanvas *ctmp = new TCanvas("ctmp","ctmp", cc_xx, cc_yy);
  sfname += "[";
  ctmp->Print(sfname.c_str());
  sfname = sreport;

  TCanvas *c1ps = new TCanvas("c1ps","c1ps", cc_xx, cc_yy);
  int npage=1;
  for(int i_dms=0; i_dms<(int)m_HadDMCoeff->getSizeAreaSet(); i_dms++) {
    const CaloLocalHadCoeff::LocalHadArea *dmArea = m_HadDMCoeff->getArea(i_dms);
    if( dmArea->getType() != CaloLocalHadDefs::AREA_DMFIT) continue;

    const CaloLocalHadCoeff::LocalHadDimension *dimFrac = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_EMFRAC);
    const CaloLocalHadCoeff::LocalHadDimension *dimSide = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_SIDE);
    const CaloLocalHadCoeff::LocalHadDimension *dimEner = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ENER);
    const CaloLocalHadCoeff::LocalHadDimension *dimLambda = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_LAMBDA);
    const CaloLocalHadCoeff::LocalHadDimension *dimEta = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ETA);
    const CaloLocalHadCoeff::LocalHadDimension *dimPhi = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_PHI);

    std::vector<int > v_indx;
    v_indx.resize(CaloLocalHadCoeffHelper::DIM_UNKNOWN, 0);
    for(int i_frac=0; i_frac<dimFrac->getNbins(); i_frac++) {
      for(int i_ener=0; i_ener<dimEner->getNbins(); i_ener++) {
        for(int i_lambda=0; i_lambda<dimLambda->getNbins(); i_lambda++) {
          for(int i_side=0; i_side<dimSide->getNbins(); i_side++) {
            for(int i_phi=0; i_phi<dimPhi->getNbins(); i_phi++) {
              v_indx[CaloLocalHadCoeffHelper::DIM_EMFRAC] = i_frac;
              v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = i_ener;
              v_indx[CaloLocalHadCoeffHelper::DIM_LAMBDA] = i_lambda;
              v_indx[CaloLocalHadCoeffHelper::DIM_PHI] = i_phi;
              v_indx[CaloLocalHadCoeffHelper::DIM_SIDE] = i_side;
              for(int k=0; k<2; k++){ // loop over TH2F and TProfile
                c1ps->Clear();
                TPad *pad1 = new TPad("p1ps","p1ps",0.0, 0.95, 1.0, 1.0); pad1->Draw();
                TPad *pad2 = new TPad("p2ps","p2ps",0.0, 0.0, 1.0, 0.95); pad2->Draw();
                // nice header with energy (in GeV) range on it
                pad1->cd();
                float ener1 = pow(10,dimEner->getXmin()+dimEner->getDx()*i_ener)/GeV;
                float ener2 = pow(10,dimEner->getXmin()+dimEner->getDx()*(i_ener+1))/GeV;
                sprintf(str,"%s em:%d ener:%d> %5.1f-%6.1f phi:%d s:%d",dmArea->getTitle().c_str(),i_frac,i_ener, ener1, ener2, i_phi, i_side);
                tex.SetTextSize(0.4);
                tex.SetTextColor(kBlue);
                tex.DrawTextNDC(0.05,0.1,str);
                // number of pad's divisions to display all eta-histograms on one page
                int ndiv = dimEta->getNbins();
                if(ndiv <=6){
                  pad2->Divide(2,3);
                }else if(ndiv <=9){
                  pad2->Divide(3,3);
                }else if(ndiv <=12){
                  pad2->Divide(3,4);
                }else if(ndiv <=20){
                  pad2->Divide(4,5);
                }else if(ndiv <=30){
                  pad2->Divide(5,6);
                } else {
                  pad2->Divide(6,6);
                }
                for(int i_eta=0; i_eta<dimEta->getNbins(); i_eta++){
                  v_indx[CaloLocalHadCoeffHelper::DIM_ETA] = i_eta;
                  int iBin = m_HadDMCoeff->getBin(i_dms, v_indx);
                  if(iBin == -1) {
                    std::cout << "Panic! iBin == -1, i_frac:" << i_frac << " i_ener:" << i_ener << " i_lambda:" << i_lambda << " i_eta:" << i_eta << std::endl;
                    return;
                  }
                  TH1 *hh = 0;
                  if(k==0) {
                    hh = m_h2_DmVsPrep[iBin];
                  }else{
                    hh = m_hp_DmVsPrep[iBin];
                  }
                  float eta1 = dimEta->getXmin() + dimEta->getDx()*i_eta;
                  float eta2 = dimEta->getXmin() + dimEta->getDx()*(i_eta+1);
                  pad2->cd(1+i_eta); gPad->SetGrid();
                  hh->GetXaxis()->SetNdivisions(508);
                  hh->GetYaxis()->SetNdivisions(508);
                  hh->GetXaxis()->SetTitle("edmtrue");
                  hh->GetYaxis()->SetTitle("eprep");
                  if(k==0) { // TH2F
                    gStyle->SetOptStat(111111);
                    m_h2_DmVsPrep[iBin]->Draw("box");
                  }else { // TProfile
                    gStyle->SetOptStat(1);
                    gStyle->SetOptFit();
                    //m_hp_DmVsPrep[iBin]->SetMaximum(m_engPrep[iBin]->m_aver + 3.0*sqrt(m_engPrep[iBin]->m_rms));
                    m_hp_DmVsPrep[iBin]->SetMaximum(  m_h2_DmVsPrep[iBin]->GetYaxis()->GetXmax() );
                    m_hp_DmVsPrep[iBin]->Draw();
                  }
                  tex.SetTextSize(0.095);
                  tex.SetTextColor(kBlue);
                  sprintf(str,"%4.2f-%4.2f",eta1,eta2);
                  tex.DrawTextNDC(0.2,0.8,str);
                  sprintf(str,"%d",iBin);
                  tex.DrawTextNDC(0.5,0.28,str);
                  // get parameters and draw values nicely
                  if(m_FitData[iBin]) {
                    tex.SetTextColor(kBlue);
                    float p0inv, s0inv, p1inv, s1inv;
                    m_FitData[iBin]->getInverted(p0inv, s0inv, p1inv, s1inv);
                    tex.SetTextColor(kBlue); tex.SetTextSize(0.07);
                    sprintf(str,"p  :%6.3f %6.3f",m_FitData[iBin]->p0, m_FitData[iBin]->p1);
                    tex.DrawTextNDC(0.2,0.7,str);
                    sprintf(str,"inv:%6.3f %6.3f",p0inv, p1inv);
                    tex.DrawTextNDC(0.2,0.7-0.1,str);
                    sprintf(str,"err:%6.3f %6.3f",s0inv, s1inv);
                    tex.DrawTextNDC(0.2,0.7-0.2,str);
                    if(!m_FitData[iBin]->isOK) {
                      tex.SetTextColor(kRed);
                    }
                    tex.DrawTextNDC(0.18,0.7-0.3,m_FitData[iBin]->descr.c_str());
                  }else{
                    sprintf(str,"Oops!");
                    tex.SetTextColor(kMagenta); tex.SetTextSize(0.095);
                    tex.DrawTextNDC(0.2,0.7,str);
                  }
                } // i_eta
                c1ps->Print(sfname.c_str());
                printf("page:%d  dmArea->m_title:%s  frac:%d  ener:%d phi:%d side:%d\n",
                      npage++, dmArea->getTitle().c_str(), i_frac, i_ener, i_phi, i_side);
              } // k
            } // i_phi
          } // i_side
        } // i_lambda
      } // i_ener

      /* **************************************************
      as a function of energy
      ************************************************** */
      for(int i_side=0; i_side<dimSide->getNbins(); i_side++){
        v_indx[CaloLocalHadCoeffHelper::DIM_SIDE] = i_side;
        for(int i_phi=0; i_phi<dimPhi->getNbins(); i_phi++){
          v_indx[CaloLocalHadCoeffHelper::DIM_PHI] = i_phi;
          for(int i_par=0; i_par<2; i_par++){
            c1ps->Clear();
            TPad *pad1 = new TPad("p1ps","p1ps",0.0, 0.95, 1.0, 1.0); pad1->Draw();
            TPad *pad2 = new TPad("p2ps","p2ps",0.0, 0.0, 1.0, 0.95); pad2->Draw();
            // nice header with energy (in GeV) range on it
            pad1->cd();
            sprintf(str,"Summary: %s par:%d frac:%d  phi:%d side:%d",dmArea->getTitle().c_str(),i_par,i_frac, i_phi, i_side);
            tex.SetTextSize(0.5); tex.SetTextColor(kBlue);
            tex.DrawTextNDC(0.05,0.1,str);
            // number of pad's divisions to display all eta-histograms on one page
            int ndiv = dimEta->getNbins();
            if(ndiv <=6){
              pad2->Divide(2,3);
            }else if(ndiv <=9){
              pad2->Divide(3,3);
            }else if(ndiv <=12){
              pad2->Divide(3,4);
            }else if(ndiv <=20){
              pad2->Divide(4,5);
            }else if(ndiv <=30){
              pad2->Divide(5,6);
            } else {
              pad2->Divide(6,6);
            }
            for(int i_eta=0; i_eta<dimEta->getNbins(); i_eta++){
              v_indx[CaloLocalHadCoeffHelper::DIM_ETA] = i_eta;
              TGraphErrors *gr = new TGraphErrors(dimEner->getNbins());
              for(int i_ener=0; i_ener<dimEner->getNbins(); i_ener++) {
                v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = i_ener;
                int iBin = m_HadDMCoeff->getBin(i_dms, v_indx);
                float y(0), ye(0);
                if(iBin >= 0 && m_FitData[iBin]) {
                  float p0inv, s0inv, p1inv, s1inv;
                  m_FitData[iBin]->getInverted(p0inv, s0inv, p1inv, s1inv);
                  if(i_par==0) {
                    y = p0inv;
                    ye = s0inv;
                  }else{
                    y = p1inv;
                    ye = s1inv;
                  }
                }
                gr->SetPoint(i_ener, dimEner->getXmin()+i_ener*dimEner->getDx(), y);
                gr->SetPointError(i_ener, 0.0, ye);
              } // i_ener
              pad2->cd(1+i_eta); gPad->SetGrid();
              gr->Draw("apl");
              float eta1 = dimEta->getXmin() + dimEta->getDx()*i_eta;
              float eta2 = dimEta->getXmin() + dimEta->getDx()*(i_eta+1);
              tex.SetTextSize(0.095);
              tex.SetTextColor(kBlue);
              sprintf(str,"%4.2f-%4.2f  phibin:%d",eta1,eta2,i_phi);
              tex.DrawTextNDC(0.2,0.8,str);
            } // i_eta
            c1ps->Print(sfname.c_str());
            printf("page:%d  dmArea->m_title:%s  frac:%d Energy summary\n",
                  npage++, dmArea->getTitle().c_str(), i_frac);
          } // i_par
        } // i_phi
      } // i_side
    } // i_frac
  } // i_dms

  /* *********************************************
  now making TH2F histograms which represents our look-up data
  ********************************************** */
  for(int i_dms=0; i_dms<int(m_HadDMCoeff->getSizeAreaSet()); i_dms++) {
    const CaloLocalHadCoeff::LocalHadArea *dmArea = m_HadDMCoeff->getArea(i_dms);
    if( dmArea->getType() != CaloLocalHadDefs::AREA_DMLOOKUP) continue;

    const CaloLocalHadCoeff::LocalHadDimension *dimFrac = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_EMFRAC);
    const CaloLocalHadCoeff::LocalHadDimension *dimSide = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_SIDE);
    const CaloLocalHadCoeff::LocalHadDimension *dimEner = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ENER);
    const CaloLocalHadCoeff::LocalHadDimension *dimLambda = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_LAMBDA);
    const CaloLocalHadCoeff::LocalHadDimension *dimEta = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ETA);
    const CaloLocalHadCoeff::LocalHadDimension *dimPhi = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_PHI);

    for(int i_frac=0; i_frac<dimFrac->getNbins(); i_frac++){
      for(int i_side=0; i_side<dimSide->getNbins(); i_side++){
        for(int i_phi=0; i_phi<dimPhi->getNbins(); i_phi++){
          for(int i_eta=0; i_eta<dimEta->getNbins(); i_eta++){
            // check if eta bin within region of interest
            bool ShowThisEta = false;
            for(int i=0; i<int(sizeof(etaRegions)/sizeof(float)/2); i++){
              float xeta = dimEta->getXmin()+i_eta*dimEta->getDx();
              if(xeta>=etaRegions[i][0] && xeta <= etaRegions[i][1]) {
                ShowThisEta = true;
                break;
              }
            }
            if(!ShowThisEta) continue;
            std::vector<int > v_indx;
            v_indx.resize(CaloLocalHadCoeffHelper::DIM_UNKNOWN, 0);
            v_indx[CaloLocalHadCoeffHelper::DIM_EMFRAC] = i_frac;
            v_indx[CaloLocalHadCoeffHelper::DIM_SIDE] = i_side;
            v_indx[CaloLocalHadCoeffHelper::DIM_ETA] = i_eta;
            v_indx[CaloLocalHadCoeffHelper::DIM_PHI] = i_phi;
            v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = 0;
            v_indx[CaloLocalHadCoeffHelper::DIM_LAMBDA] = 0;
            // lets find bins for axes
            Double_t xx[100];
            bzero(xx,100*sizeof(Double_t));
            for(int i_ener=0; i_ener<dimEner->getNbins()+1; i_ener++){
              xx[i_ener] = dimEner->getXmin() + dimEner->getDx()*i_ener;
            }
            Double_t yy[100];
            bzero(yy,100*sizeof(Double_t));
            for(int i_lambda=0; i_lambda<dimLambda->getNbins()+1; i_lambda++){
              yy[i_lambda] = dimLambda->getXmin() + dimLambda->getDx()*i_lambda;
            }
            int nbins = 0;
            int ibin_min = m_HadDMCoeff->getBin(i_dms, v_indx);
            v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = dimEner->getNbins() - 1;
            v_indx[CaloLocalHadCoeffHelper::DIM_LAMBDA] = dimLambda->getNbins() - 1;
            int ibin_max = m_HadDMCoeff->getBin(i_dms, v_indx);
            const int n_prof= 2;
            TH2F *hp2[n_prof];
            sprintf(hname,"%s dm:%d frac:%d eta:%d phi:%d indx:%d-%d<ecls>/<edm>",dmArea->getTitle().c_str(),i_dms, i_frac, i_eta, i_phi, ibin_min, ibin_max);
            hp2[0] = new TH2F(hname, hname, dimEner->getNbins(), xx, dimLambda->getNbins(), yy);
            sprintf(hname,"%s dm:%d frac:%d eta:%d phi:%d indx:%d-%d nev",dmArea->getTitle().c_str(),i_dms, i_frac, i_eta, i_phi, ibin_min, ibin_max);
            hp2[1] = new TH2F(hname, hname, dimEner->getNbins(), xx, dimLambda->getNbins(), yy);
            for(int i=0; i<n_prof; i++){
              hp2[i]->GetXaxis()->SetTitle("log10(E_{cls}(MeV))");
              hp2[i]->GetYaxis()->SetTitle("log10(#lambda)");
              hp2[i]->GetZaxis()->SetTitle("<E_{cls}>/<E_{dm}>");
            }
            // filling histograms
            float hp_aver[n_prof];
            bzero(hp_aver,n_prof*sizeof(float));
            std::vector<std::pair<int, int> > v_occupancy;
            for(int i_ener=0; i_ener<dimEner->getNbins(); i_ener++) {
              for(int i_lambda=0; i_lambda<dimLambda->getNbins(); i_lambda++) {
                v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = i_ener;
                v_indx[CaloLocalHadCoeffHelper::DIM_LAMBDA] = i_lambda;
                int iBin = m_HadDMCoeff->getBin(i_dms, v_indx);
                if(iBin == -1) {
                  std::cout << "Panic in pp3, Wrong iBin=-1" << std::endl;
                  exit(EXIT_FAILURE);
                }
                float x = m_engDmOverClus[iBin]->m_aver;
                hp2[0]->Fill(dimEner->getXmin() + dimEner->getDx()*(i_ener+0.5), dimLambda->getXmin() + dimLambda->getDx()*(i_lambda+0.5), x);
                hp_aver[0] += x;
                x = float(m_engDm[iBin]->size());
                hp2[1]->Fill(dimEner->getXmin() + dimEner->getDx()*(i_ener+0.5), dimLambda->getXmin() + dimLambda->getDx()*(i_lambda+0.5), x );
                hp_aver[1] += x;
                nbins ++;
                std::pair<int, int> pp;
                pp.first = int(m_engDmOverClus[iBin]->size());
                pp.second = iBin;
                v_occupancy.push_back(pp);
              }
            }
            TCanvas *cp2 = new TCanvas("cp2","cp2",cc_xx,cc_yy);
            cp2->cd();
            TPad *pad1 = new TPad("cp2_p1ps","cp2_p1ps",0.0, 0.95, 1.0, 1.0); pad1->Draw();
            TPad *pad2 = new TPad("cp2_p2ps","cp2_p2ps",0.0, 0.0, 1.0, 0.95); pad2->Draw();
            // nice header with energy (in GeV) range on it
            pad1->cd();
            sprintf(str,"Summary: %s frac:%d  phi:%d side:%d",dmArea->getTitle().c_str(),i_frac, i_phi, i_side);
            tex.SetTextSize(0.5); tex.SetTextColor(kBlue);
            tex.DrawTextNDC(0.05,0.1,str);
            pad2->Divide(2,3);
            pad2->cd(1);
            gPad->SetLogz();
            gPad->SetRightMargin(0.2);
            hp2[1]->Draw("colz");
            pad2->cd(2);
            if (ibin_min >= 0) {
              m_hp2_DmWeight[ibin_min]->SetStats(0);
              m_hp2_DmWeight[ibin_min]->SetMinimum(0.0);
              m_hp2_DmWeight[ibin_min]->SetMaximum(1.0);
              m_hp2_DmWeight[ibin_min]->Draw("colz");
            }
            // drawing bin number for histograms
            for(int i_ener=0; i_ener<dimEner->getNbins(); i_ener++) {
              v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = i_ener;
              int iBin = m_HadDMCoeff->getBin(i_dms, v_indx);
              sprintf(str,"%d",iBin);
              TLatex tex;
              tex.SetTextSize(0.03);
              tex.SetTextAngle(90.);
              tex.DrawLatex(dimEner->getXmin() + dimEner->getDx()*(i_ener+0.5), dimLambda->getXmax()-2.*dimLambda->getDx(), str);
            }

            // drawing spectras in selected cells
            std::sort(v_occupancy.begin(), v_occupancy.end());
            std::reverse(v_occupancy.begin(), v_occupancy.end());
            for(unsigned int i_spec=0; i_spec<v_occupancy.size(); i_spec++){
              int iBin = v_occupancy[i_spec].second;
              if(iBin == -1) {
                std::cout << "Panic in pp3, Wrong iBin=-1" << std::endl;
                exit(EXIT_FAILURE);
              }
              if(!m_h1_engDmOverClus[iBin]) {
                std::cout << "Undefined h1 for " << iBin << std::endl;
                continue;
              }
              std::vector<int > indexes;
              m_HadDMCoeff->bin2indexes(iBin, indexes);
              sprintf(str, "ibin:%d  frac:%d ener:%d lambda:%d eta:%d phi:%d",iBin,
                      indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC], indexes[CaloLocalHadCoeffHelper::DIM_ENER], 
                      indexes[CaloLocalHadCoeffHelper::DIM_LAMBDA], indexes[CaloLocalHadCoeffHelper::DIM_ETA], 
                      indexes[CaloLocalHadCoeffHelper::DIM_PHI]);
              pad2->cd(3+i_spec);
              m_h1_engDmOverClus[iBin]->SetStats(1);
              m_h1_engDmOverClus[iBin]->SetTitle(str);
              gStyle->SetOptStat(111111);
              m_h1_engDmOverClus[iBin]->Draw();
              double av = GetAverageWithoutRightTail(m_h1_engDmOverClus[iBin]);
              sprintf(str,"rmscu:%8.5f  evcu:%6.3f",m_engDmOverClus[iBin]->m_aver, av);
              tex.SetTextSize(0.05);
              tex.DrawTextNDC(0.2,0.7,str);
              if(i_spec >= 3) break;
            }
            cp2->Print(sfname.c_str());
            printf("page:%d  dmArea->m_title:%s  i_frac:%d  eta:%d phi:%d side:%d\n",
                  npage, dmArea->getTitle().c_str(), i_frac, i_eta, i_phi, i_side);
            npage++;
          }// i_eta
        }// i_phi
      } // i_side
    } // i_frac
  } // i_dms

  sfname = sreport;
  sfname += "]";
  ctmp->Print(sfname.c_str());
}


/* ****************************************************************************

**************************************************************************** */
void CaloHadDMCoeffFit::clear()
{
  for(std::vector<PrepData *>::iterator it = m_engClus.begin(); it!=m_engClus.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_engClus.clear();

  for(std::vector<PrepData *>::iterator it = m_engPrep.begin(); it!=m_engPrep.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_engPrep.clear();

  for(std::vector<PrepData *>::iterator it = m_engDm.begin(); it!=m_engDm.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_engDm.clear();

  for(std::vector<PrepData *>::iterator it = m_engDmOverClus.begin(); it!=m_engDmOverClus.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_engDmOverClus.clear();

  for(std::vector<TProfile *>::iterator it = m_hp_DmVsPrep.begin(); it!=m_hp_DmVsPrep.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_hp_DmVsPrep.clear();

  for(std::vector<TH2F *>::iterator it = m_h2_DmVsPrep.begin(); it!=m_h2_DmVsPrep.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_h2_DmVsPrep.clear();

  for(std::vector<TH1F *>::iterator it = m_h1_engDmOverClus.begin(); it!=m_h1_engDmOverClus.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_h1_engDmOverClus.clear();

  int i_size=0;
  for(std::vector<TProfile2D *>::iterator it = m_hp2_DmWeight.begin(); it!=m_hp2_DmWeight.end(); it++, i_size++){
    if( (*it) && i_size==getFirstEnerLambdaBin(i_size) ) delete (*it);
  }
  m_hp2_DmWeight.clear();

}



/* ****************************************************************************
- reset bin content if number of entries <2 (i.e. with zero errors)
- calculates interval on x-axis [0,xLimRight] which contains 92% of all entries
**************************************************************************** */
double CaloHadDMCoeffFit::ProfileRefiner(TProfile *pH, double ev_ratio)
{
  double xLimRight(0);
//   double e(0),cont(0),nent(0);
  double nevtot(0);
  for(int i=1; i<=pH->GetNbinsX(); i++){
    nevtot += pH->GetBinEntries(i);
  }
  //std::cout << "yyy entries:" << pH->GetEntries() << "bin0:" << pH->GetBinEntries(0) << " bin1:" << pH->GetBinEntries(pH->GetNbinsX()+1)
  //<< " nevtot: " << nevtot 
  //<< " ev_ratio:" << ev_ratio
  //<< std::endl;
  double nev_in(0);
  double inv_nevtot = nevtot ? 1. / nevtot : 1;
  for(int i=1; i<=pH->GetNbinsX(); i++){
    nev_in += pH->GetBinEntries(i);
    if(pH->GetBinEffectiveEntries(i)<2.0){
      pH->SetBinEntries(i,0.0);
      pH->SetBinContent(i,0.0);
    }
    //if(xLimRight==0.0 && nev_in/nevtot > ev_ratio) xLimRight=pH->GetBinCenter(i) + pH->GetBinWidth(i)/2.;
    //std::cout << "yyy i:" << i << " bin_content:" << pH->GetBinContent(i) << " entries:" << pH->GetBinEntries(i)
    //<< " nev_in:" << nev_in << " nevtot:" << nevtot << " nev_in/nevtot:" << nev_in/nevtot
    //<< std::endl;
    if(nev_in*inv_nevtot > ev_ratio) {
      xLimRight=pH->GetBinCenter(i) + pH->GetBinWidth(i)/2.;
      //std::cout << "yyy i:" << i << " xLimRight:" << xLimRight << std::endl;
      break;
    }
  }
  if(xLimRight==0) xLimRight=pH->GetBinCenter(pH->GetNbinsX()) + pH->GetBinWidth(pH->GetNbinsX())/2.;
  //std::cout << "yyy results xLimRight:" << xLimRight << std::endl;
  return xLimRight;
}



/* ****************************************************************************
- calculates average of TH1F histogram after rejection of events in the right tail
**************************************************************************** */
double CaloHadDMCoeffFit::GetAverageWithoutRightTail(TH1F *pH, double ev_ratio)
{
  double xLimRight=pH->GetBinCenter(pH->GetNbinsX()) + pH->GetBinWidth(pH->GetNbinsX())/2.;
  int nbinx = (int)pH->GetNbinsX();
//   double nevtot = pH->Integral()  - pH->GetBinContent(0) - pH->GetBinContent(nbinx+1);
  double nevtot = pH->Integral();
  double nev = 0;
  if(nevtot != 0.0) {
    const double inv_nevtot = 1. / nevtot;
    for(int i_bin = 0; i_bin <=nbinx; i_bin++){
      nev+= pH->GetBinContent(i_bin);
      
      if( nev*inv_nevtot >= ev_ratio){
        xLimRight = pH->GetBinCenter(i_bin) + pH->GetBinWidth(i_bin)/2.;
        break;
      }
    }
  }
  double new_aver = 0.0;
  double sum = 0.0;
  for(int i_bin = 1; i_bin <=nbinx; i_bin++){
    if(pH->GetBinCenter(i_bin) <= xLimRight) {
      new_aver += pH->GetBinContent(i_bin)*pH->GetBinCenter(i_bin);
      sum += pH->GetBinContent(i_bin);
    }
  }
  if(sum != 0.0) {
    new_aver /= sum;
    if(new_aver < 0.0) new_aver = 0.0;
  }else{
    new_aver = pH->GetMean();
  }
  return new_aver;
}


int CaloHadDMCoeffFit::getFirstEnerLambdaBin(int ibin)
{
  // DIM_EMFRAC, DIM_SIDE, DIM_ETA, DIM_PHI, DIM_ENER, DIM_LAMBDA
  const CaloLocalHadCoeff::LocalHadArea *dmArea = m_HadDMCoeff->getAreaFromBin(ibin);
  std::vector<int > indexes;
  m_HadDMCoeff->bin2indexes(ibin, indexes);
  indexes[CaloLocalHadCoeffHelper::DIM_ENER]=0;
  indexes[CaloLocalHadCoeffHelper::DIM_LAMBDA]=0;

  int n_area;
  for(n_area=0; n_area<m_HadDMCoeff->getSizeAreaSet(); n_area++){
    if(dmArea == m_HadDMCoeff->getArea(n_area)) {
      break;
    }
  }
  int refbin = m_HadDMCoeff->getBin(n_area, indexes);
  return refbin;
}



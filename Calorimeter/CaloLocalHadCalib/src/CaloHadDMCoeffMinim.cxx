/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloHadDMCoeffMinim.cxx,v 1.2 2009/03/06 14:43:23 pospelov Exp $
//
// Description: see CaloHadDMCoeffMinim.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------
#include "CaloLocalHadCalib/CaloHadDMCoeffMinim.h"
#include "AthenaKernel/Units.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffData.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffFit.h"
#include "CaloLocalHadCalib/CaloLocalHadCoeffHelper.h"
#include "CaloLocalHadCalib/GetLCDefs.h"
#include "CaloLocalHadCalib/GetLCSinglePionsPerf.h"
#include "boost/io/ios_state.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <CLHEP/Vector/LorentzVector.h>

#include "TROOT.h"
#include "TStyle.h"
#include "TError.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TText.h"
#include "TMinuit.h"
#include "TVirtualFitter.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TBenchmark.h"


using CLHEP::HepLorentzVector;
using Athena::Units::MeV;
using Athena::Units::GeV;


CaloHadDMCoeffMinim *CaloHadDMCoeffMinim::s_instance = nullptr;


CaloHadDMCoeffMinim::CaloHadDMCoeffMinim() :
  m_isTestbeam(false),
  m_nstep_fcn(0),
  m_iBinGlob(0),
  m_engClusMin(1.*GeV), 
  m_engBeamMin(1.*GeV),
  m_area_index(0),
  m_NormalizationType("Lin"),
  m_NormalizationTypeNumber(0)
{
  m_data = nullptr;
  m_HadDMCoeff = nullptr;
  m_HadDMHelper = new CaloLocalHadCoeffHelper();

  // list of parameters available for minimization
  m_minimPars.emplace_back("PreSamplerB", 1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("EMB1",        1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("EMB2",        1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("EMB3",        1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("PreSamplerE", 1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("EME1",        1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("EME2",        1.0, 0.5,  0.99, 2000.0 ); // 6
  m_minimPars.emplace_back("EME3",        1.0, 0.5,  0.99, 2000.0 );
  m_minimPars.emplace_back("HEC0",        1.0, 0.5,  0.99, 2000.0 );
  m_minimPars.emplace_back("HEC1",        1.0, 0.5,  0.99, 2000.0 );
  m_minimPars.emplace_back("HEC2",        1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("HEC3",        1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileBar0",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileBar1",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileBar2",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileGap1",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileGap2",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileGap3",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileExt0",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileExt1",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("TileExt2",    1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("FCAL0",       1.0, 0.01, 0.99, 3.0 );
  m_minimPars.emplace_back("FCAL1",       1.0, 0.01, 0.99, 3.0 );
  m_minimPars.emplace_back("FCAL2",       1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("MINIFCAL0",   1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("MINIFCAL1",   1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("MINIFCAL2",   1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("MINIFCAL3",   1.0, 0.5,  0.99, 20.0 );
  m_minimPars.emplace_back("CONST",       0.0, 10.,  0.0,  5000. );

  m_distance_cut = 1.5;

  s_instance = this;
}


CaloHadDMCoeffMinim::~CaloHadDMCoeffMinim()
{
  for(std::vector<MinimSample *>::iterator it=m_minimSample.begin(); it!=m_minimSample.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_minimSample.clear();
  delete m_HadDMHelper;
}


/* ****************************************************************************
Runs minimization to get new sampling weights for optimum reconstruction of
dead material before FCAL
**************************************************************************** */
CaloLocalHadCoeff * CaloHadDMCoeffMinim::process(CaloHadDMCoeffData *myData, CaloLocalHadCoeff *myHadDMCoeff, bool isSingleParticle, bool tbflag)
{
  m_isTestbeam = tbflag;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "--- CaloHadDMCoeffMinim::process() --- " << std::endl;

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

  m_data = myData; // pointer to TChain data
  m_HadDMCoeff = myHadDMCoeff; // pointer to the initial coefficients

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
  //m_data->fChain->SetBranchStatus("cls_eprep",1);
  m_data->fChain->SetBranchStatus("cls_dmener",1);
  m_data->fChain->SetBranchStatus("cls_smpener_unw",1);
  //m_data->fChain->SetBranchStatus("m_cls_oocener",1);
  //m_data->fChain->SetBranchStatus("cls_engcalibpres",1);

  if( !m_data->GetEntries() ) {
    std::cout << "CaloHadDMCoeffMinim::process -> Error! No entries in DeadMaterialTree." << std::endl;
    return nullptr;
  }

  m_data->GetEntry(0);
  if(m_data->m_narea != m_HadDMCoeff->getSizeAreaSet()) {
    std::cout << "CaloHadDMCoeffFit::process -> Error! Different numbers of areas for DM definition" << std::endl;
    std::cout << "m_data->m_narea:" << m_data->m_narea << " m_HadDMCoeff->getSizeAreaSet():" << m_HadDMCoeff->getSizeAreaSet() << std::endl;
    return nullptr;
  }

  // We are going to receive dead material correction coefficients only for one
  // dead material area corresponding 'ENG_CALIB_DEAD_FCAL' calibration moment
  const CaloLocalHadCoeff::LocalHadArea *dmArea = m_HadDMHelper->getAreaFromName(m_HadDMCoeff, "ENG_CALIB_DEAD_FCAL", m_area_index);
  std::cout << "CaloHadDMCoeffMinim::process() -> Info. Preparing for '" << dmArea->getTitle() << " index:" << m_area_index
      << " npars:" << dmArea->getNpars() << ", for minimization:" << m_validNames.size() << std::endl;
  if( (int)m_minimPars.size() != dmArea->getNpars()) {
    std::cout << "CaloHadDMCoeffMinim::process() -> FATAL! Number of parameters for dead material area is different from number of mininuit pars." << std::endl;
    exit(0);
  }

  /* ********************************************
  Let's fill sample with data to calculate chi2
  1. loop through data sample
  3. filling minimisation sample with clusters having appropriate iBin
  ********************************************* */
  std::cout << "CaloHadDMCoeffMinim::process() -> Info. Step 2 - making cluster set..." << std::endl;
  for(std::vector<MinimSample *>::iterator it=m_minimSample.begin(); it!=m_minimSample.end(); it++){
    if( (*it) ) delete (*it);
    (*it)=0;
  }
  m_minimSample.clear();
  m_sample_size.resize(dmArea->getLength(), 0);
  int nGoodEvents = 0;
  for(int i_ev=0; m_data->GetEntry(i_ev)>0;i_ev++) {

    //if(m_data->m_mc_ener < m_engBeamMin) continue;

    if(i_ev%20000==0) std::cout << "    i_ev: " << i_ev << " (" << nGoodEvents << ") '" << ((TChain *)m_data->fChain)->GetFile()->GetName() << "'" << std::endl;

    double EnergyResolution; // in GeV
    if( abs(m_data->m_mc_pdg) == 211) {
      EnergyResolution = sqrt((0.5*0.5/GeV)*m_data->m_mc_ener+pow((0.03/GeV*m_data->m_mc_ener),2));// in GeV
    } else {
      //EnergyResolution = sqrt(0.1*0.1*m_data->m_mc_ener/GeV+0.003*0.003+0.007*0.007*pow((m_data->m_mc_ener/GeV),2)); // in GeV
      EnergyResolution = sqrt((0.5*0.5/GeV)*m_data->m_mc_ener+pow((0.03/GeV*m_data->m_mc_ener),2));// in GeV
    }

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
      } // m_ncls
      if(!GoodClusterFound) continue;
    }

    if(m_data->m_engClusSumCalib <=0.0) continue;

    double engDmTrueSum = 0.0;
    for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
      engDmTrueSum += (*m_data->m_cls_dmener)[i_cls][m_area_index];
    }

    for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
      if( (*m_data->m_cls_ener_unw)[i_cls] < m_engClusMin
	  || (*m_data->m_cls_engcalib)[i_cls] < 20.0*MeV 
	  ) continue;
      std::vector<float> vars;
      m_data->PackClusterVars(i_cls, vars);
      int iBin = m_HadDMCoeff->getBin(m_area_index, vars );

      if(iBin >= dmArea->getOffset() && iBin < (dmArea->getOffset()+dmArea->getLength())  && m_data->m_engClusSumCalib > 0.0 && m_data->m_mc_ener > 0.0) {
        MinimSample *ev = new MinimSample();
        // we need only edmtrue and energy in cluster samplings to calculate fcn
        ev->ibin = iBin;
        ev->edmtrue = (*m_data->m_cls_dmener)[i_cls][m_area_index];
        if(ev->edmtrue<0) ev->edmtrue=0.0;
        ev->clsm_smp_energy_unw.resize((*m_data->m_cls_smpener_unw)[i_cls].size());
        for(unsigned int i_smp=0; i_smp<(*m_data->m_cls_smpener_unw)[i_cls].size(); i_smp++){
          ev->clsm_smp_energy_unw[i_smp] = (*m_data->m_cls_smpener_unw)[i_cls][i_smp];
        }
        // Calculation of cluster weight, which shows siginicance of it for chi2 calculation
        double clus_weight = 1.0;
        if(m_NormalizationTypeNumber == GetLCDefs::LIN) {
          clus_weight = (*m_data->m_cls_engcalib)[i_cls]/m_data->m_engClusSumCalib;
        }
        ev->weight = clus_weight;

        // error which will be used during chi2 calculation, currently it is simply the energy resolution
        ev->sigma2 = EnergyResolution*EnergyResolution;
        m_minimSample.push_back(ev);
        m_sample_size[iBin - dmArea->getOffset()]++;
      }
    } // i_cls
    
    nGoodEvents++;
  } // i_ev


  /* ********************************************
  run minimization
  ********************************************* */
  std::cout << "CaloHadDMCoeffMinim::process() -> Info. Starting minimization, m_minimSample.size(): " << m_minimSample.size() << "( " << float(m_minimSample.size())*(26.0/1e+06) << " Mb)"<< std::endl;
  for(int i_data=0; i_data<dmArea->getLength(); i_data++) {
    TBenchmark mb;
    mb.Start("minuitperf");
    m_iBinGlob = dmArea->getOffset() + i_data;
    m_minimSubSample.clear();
    for(std::vector<MinimSample *>::iterator it=m_minimSample.begin(); it!=m_minimSample.end(); it++) {
      MinimSample *event = (*it);
      if(event->ibin == m_iBinGlob) {
        m_minimSubSample.push_back(event);
        if(m_minimSubSample.size() > 100000) break;
      }
    }

    std::vector<int > indexes;
    m_HadDMCoeff->bin2indexes(m_iBinGlob, indexes);
    std::cout << "==============================================================================" << std::endl;
    std::cout << "run " << i_data 
        << " out of " << dmArea->getLength()
        << " m_iBinGlob:" << m_iBinGlob
        << " frac:" << indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]
        << " side:" << indexes[CaloLocalHadCoeffHelper::DIM_SIDE]
        << " eta:" << indexes[CaloLocalHadCoeffHelper::DIM_ETA]
        << " phi:" << indexes[CaloLocalHadCoeffHelper::DIM_PHI]
        << " ener:" << indexes[CaloLocalHadCoeffHelper::DIM_ENER]
        << " lambda:" << indexes[CaloLocalHadCoeffHelper::DIM_LAMBDA]
        << " size_of_subset:" << m_sample_size[i_data]
        << std::endl;

    // name of parameters to be minimized
    m_validNames.clear();
    if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC] == 0) {
      // list of samplings to minimise for charged pions
      m_validNames.emplace_back("EME2");
      m_validNames.emplace_back("EME3");
      m_validNames.emplace_back("HEC0");
      m_validNames.emplace_back("HEC1");
      m_validNames.emplace_back("FCAL0");
      m_validNames.emplace_back("FCAL1");
      m_validNames.emplace_back("CONST");
    }else{
      // list of samplings to minimise for neutral pions
      m_validNames.emplace_back("EME2");
      m_validNames.emplace_back("EME3");
      m_validNames.emplace_back("HEC0");
      m_validNames.emplace_back("FCAL0");
      m_validNames.emplace_back("CONST");
    }

    // setting up parameters which we are going to minimize
    for(std::vector<MinimPar >::iterator it_par=m_minimPars.begin(); it_par!= m_minimPars.end(); it_par++){
      bool fixIt = true;
      for(std::vector<std::string >::iterator it_name=m_validNames.begin(); it_name!=m_validNames.end(); it_name++){
        if((*it_name) == (*it_par).name) {
          fixIt = false;
          break;
        }
      }
      (*it_par).fixIt = fixIt;
    }

    for(unsigned int i_par=0; i_par<m_minimPars.size(); i_par++){
      m_minimPars[i_par].value = 1.0;
      m_minimPars[i_par].error = 0.0;
    }

    // skip minimization if size of minimization sample is too small (no clusters falling in given iBinGlob
    if(m_minimSubSample.size() > 80) {
      // preparing for minimization
      TVirtualFitter::SetDefaultFitter("Minuit");
      TVirtualFitter * minuit = TVirtualFitter::Fitter(nullptr, m_minimPars.size());
      for(unsigned int i_par=0; i_par<m_minimPars.size(); i_par++){
        MinimPar *par = &m_minimPars[i_par];
        minuit->SetParameter(i_par,par->name.c_str(), par->initVal, par->initErr, par->lowerLim, par->upperLim);
        if( par->fixIt )   minuit->FixParameter(i_par);
      }
      m_nstep_fcn = 0;
      minuit->SetFCN(CaloHadDMCoeffMinim::fcnWrapper);
      double arglist[100];
      arglist[0] = 0;
      // set print level
      minuit->ExecuteCommand("SET PRINT",arglist,2);
      // minimize
      arglist[0] = 2000; // number of function calls
      arglist[1] = 0.001; // tolerance
      // It is where minimization begins
      minuit->ExecuteCommand("MIGRAD",arglist,2);

      // Saving data
      std::cout << "Minuit results:" << std::endl;
      for(unsigned int i_par=0; i_par<m_minimPars.size(); i_par++){
        m_minimPars[i_par].value = minuit->GetParameter(i_par);
        m_minimPars[i_par].error = minuit->GetParError(i_par);
        std::cout << " i_par:" << i_par << " '" << m_minimPars[i_par].name << "' val:" << minuit->GetParameter(i_par) << " err:" << minuit->GetParError(i_par) << std::endl;
      }
      delete minuit;
    } // if minim sample is big enough
    // saving minimisation data
    m_minimResults[m_iBinGlob] = m_minimPars;
    mb.Stop("minuitperf");
    std::cout << "CPU: " << mb.GetCpuTime("minuitperf") << std::endl;
  } // loop over i_data


  /* *********************************************
  Making set with of new coefficients
  ********************************************* */
  std::cout << "CaloHadDMCoeffMinim::process() -> Info. Making output coefficients set " << std::endl;
  CaloLocalHadCoeff *newHadDMCoeff = new CaloLocalHadCoeff(*m_HadDMCoeff);
  for(std::map<int, std::vector<MinimPar > >::iterator it=m_minimResults.begin(); it!=m_minimResults.end(); it++) {
    int iBin = (*it).first;

    m_minimPars = (*it).second;
    CaloLocalHadCoeff::LocalHadCoeff pars;
    pars.resize(m_minimPars.size(),0.0);
    boost::io::ios_base_all_saver coutsave (std::cout);
    std::cout << std::fixed << std::setprecision(3) << iBin << " ";
    for(unsigned int i_par = 0; i_par<m_minimPars.size(); i_par++){
      pars[i_par] = m_minimPars[i_par].value;
      if(m_minimPars[i_par].fixIt == 1) continue;
      std::cout << std::fixed << std::setprecision(3) << "(" << m_minimPars[i_par].name << " " << m_minimPars[i_par].value << " " << m_minimPars[i_par].error << ") ";
    }
    std::cout << std::endl;

    if(m_isTestbeam) {
      std::vector<int > indexes;
      m_HadDMCoeff->bin2indexes(iBin, indexes);
      if(indexes[CaloLocalHadCoeffHelper::DIM_EMFRAC]==1) {
        const CaloLocalHadCoeff::LocalHadDimension *dimEta = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ETA);
        float eta = dimEta->getXmin() + dimEta->getDx()*(indexes[CaloLocalHadCoeffHelper::DIM_ETA]+0.5);
        if( (fabs(eta)>2.9 && fabs(eta)<3.15)) {
          //pars[CaloSampling::EME2] *= 0.96;
          //pars[CaloSampling::EME2] *= 0.97;
          pars[CaloSampling::EME2] *= 0.98;
        }
        if( fabs(eta) > 3.25) {
          //pars[CaloSampling::FCAL0] *= 0.93;
          //pars[CaloSampling::FCAL0] *= 0.94;
          pars[CaloSampling::FCAL0] *= 0.95;
        }
      }
    } // m_isTestbeam

    newHadDMCoeff->setCoeff(iBin, pars);
  }

  return newHadDMCoeff;
}



/* ****************************************************************************
Making nice postscript report
**************************************************************************** */
void CaloHadDMCoeffMinim::make_report(std::string &sreport)
{
  std::cout << "CaloHadDMCoeffMinim::make_report() -> Info. Making report..." << std::endl;
  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetTitleFillColor(10);
  int cc_xx = 768, cc_yy = 1024;
  char str[1024];
  char cname[256];
  gROOT->SetBatch(kTRUE);
  gErrorIgnoreLevel=3; // root global variables to supress text output in ->Print() methods
  std::string sfname = sreport;
  TCanvas *ctmp = new TCanvas("ctmp","ctmp", cc_xx, cc_yy);
  sfname += "[";
  ctmp->Print(sfname.c_str());
  sfname = sreport;
  // ---------------------
  int area_indx;
  const CaloLocalHadCoeff::LocalHadArea *dmArea = m_HadDMHelper->getAreaFromName(m_HadDMCoeff, "ENG_CALIB_DEAD_FCAL", area_indx);
  TLatex *tex = new TLatex(); tex->SetNDC();

  const CaloLocalHadCoeff::LocalHadDimension *dimFrac = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_EMFRAC);
  const CaloLocalHadCoeff::LocalHadDimension *dimSide = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_SIDE);
  const CaloLocalHadCoeff::LocalHadDimension *dimEner = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ENER);
  const CaloLocalHadCoeff::LocalHadDimension *dimLambda = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_LAMBDA);
  const CaloLocalHadCoeff::LocalHadDimension *dimEta = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_ETA);
  const CaloLocalHadCoeff::LocalHadDimension *dimPhi = dmArea->getDimension(CaloLocalHadCoeffHelper::DIM_PHI);

  for(int i_frac=0; i_frac<dimFrac->getNbins(); i_frac++){
    for(int i_ener=0; i_ener<dimEner->getNbins(); i_ener++){
      for(int i_lambda=0; i_lambda<dimLambda->getNbins(); i_lambda++){
        for(int i_side=0; i_side<dimSide->getNbins(); i_side++){
          for(int i_phi=0; i_phi<dimPhi->getNbins(); i_phi++){
            sprintf(cname,"c1_dmfcal_minuit_weights_frac%d_ener%d_lambda%d_phi%d_side%d",i_frac, i_ener, i_lambda, i_phi, i_side);
            TCanvas *c1_weights = new TCanvas(cname, cname, cc_xx, cc_yy);
            c1_weights->cd();
            TPad *pad1=nullptr, *pad2=nullptr;
            float y_edge = 0.85;
            pad1 = new TPad("p1ps","p1ps",0.0, y_edge, 1.0, 1.0); pad1->Draw();
            pad2 = new TPad("p2ps","p2ps",0.0, 0.0, 1.0, y_edge); pad2->Draw();
            // top pad
            pad1->cd(); gPad->SetGrid(); gPad->SetLeftMargin(0.07); gPad->SetTopMargin(0.05);  gPad->SetRightMargin(0.05);  gPad->SetBottomMargin(0.08);
            sprintf(str,"frac:%d  ener:%d  lambda:%d phi:%d side:%d",i_frac, i_ener, i_lambda, i_phi, i_side);
            tex->SetTextColor(1); tex->SetTextSize(0.20); tex->DrawLatex(0.1,0.4,str);
            pad2->Divide(3,3);
            int i_canvas = 0;
            // lets draw sample size
            pad2->cd(i_canvas+1);
            TGraph *gr = new TGraph(dimEta->getNbins());
            float smpsize=0.0;
            for(int i_eta=0; i_eta<dimEta->getNbins(); i_eta++){
              std::vector<int > v_indx;
              v_indx.resize(CaloLocalHadCoeffHelper::DIM_UNKNOWN, 0);
              v_indx[CaloLocalHadCoeffHelper::DIM_EMFRAC] = i_frac;
              v_indx[CaloLocalHadCoeffHelper::DIM_SIDE] = i_side;
              v_indx[CaloLocalHadCoeffHelper::DIM_ETA] = i_eta;
              v_indx[CaloLocalHadCoeffHelper::DIM_PHI] = i_phi;
              v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = i_ener;
              v_indx[CaloLocalHadCoeffHelper::DIM_LAMBDA] = i_lambda;
              int iBin = m_HadDMCoeff->getBin(area_indx, v_indx);
              float xx = dimEta->getXmin() + dimEta->getDx()*i_eta;
              float w = (float)m_sample_size[iBin-dmArea->getOffset()];
              smpsize += w;
              gr->SetPoint(i_eta, xx, w);
            }
            char str[256];
            sprintf(str,"Sample size %d",int(smpsize));
            gr->SetTitle(str);
            gr->Draw("apl");
            i_canvas++;
            for(unsigned int i_par=0; i_par<m_minimPars.size(); i_par++){
              std::vector<float> vx;
              std::vector<float> vy;
              std::vector<float> vye;
              for(int i_eta=0; i_eta<dimEta->getNbins(); i_eta++){
                std::vector<int > v_indx;
                v_indx.resize(CaloLocalHadCoeffHelper::DIM_UNKNOWN, 0);
                v_indx[CaloLocalHadCoeffHelper::DIM_EMFRAC] = i_frac;
                v_indx[CaloLocalHadCoeffHelper::DIM_SIDE] = i_side;
                v_indx[CaloLocalHadCoeffHelper::DIM_ETA] = i_eta;
                v_indx[CaloLocalHadCoeffHelper::DIM_PHI] = i_phi;
                v_indx[CaloLocalHadCoeffHelper::DIM_ENER] = i_ener;
                v_indx[CaloLocalHadCoeffHelper::DIM_LAMBDA] = i_lambda;
                int iBin = m_HadDMCoeff->getBin(area_indx, v_indx);
                if(!m_minimResults[iBin][i_par].fixIt){
                  vx.push_back(dimEta->getXmin() + dimEta->getDx()*i_eta);
                  vy.push_back(m_minimResults[iBin][i_par].value);
                  vye.push_back(m_minimResults[iBin][i_par].error);
                }
              } // i_eta
              if(!vx.empty()) {
                TGraphErrors *gr = new TGraphErrors(dimEta->getNbins());
                for(unsigned int i_p=0; i_p<vx.size(); i_p++){
                  gr->SetPoint(i_p, vx[i_p], vy[i_p]);
                  gr->SetPointError(i_p, 0.0, vye[i_p]);
                }
                pad2->cd(i_canvas+1);
                gPad->SetGrid();
                gr->SetMinimum(0.0);
                gr->SetLineColor(4);
                gr->SetTitle(m_minimPars[i_par].name.c_str());
                gr->Draw("apl");
                i_canvas++;
              }
            } // i_weight
            c1_weights->Print(sfname.c_str());
          } // i_phi
        } // i_side
      } // i_lambda
    } // i_ener
  } // i_frac

  sfname = sreport;
  sfname += "]";
  ctmp->Print(sfname.c_str());
}



/* ****************************************************************************

**************************************************************************** */
void CaloHadDMCoeffMinim::fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  if(gin && iflag){
    // to avoid warnings during compilation
  }
  double hi2 = 0.0;
  int nsel = 0;
  double sum_edm_rec = 0.0;
  double sum_edm_true = 0.0;
  for(std::vector<MinimSample *>::iterator it=m_minimSubSample.begin(); it!=m_minimSubSample.end(); it++) {
    MinimSample *event = (*it);
    double edm_rec = 0.0;
    double edm_true = event->edmtrue;
    for(int i_smp=0; i_smp<CaloSampling::Unknown; i_smp++){
      edm_rec += (par[i_smp] - 1.0) * (double)(event->clsm_smp_energy_unw[i_smp]);
    }
    edm_rec += par[CaloSampling::Unknown];

//     if(edm_rec < 0.0) edm_rec = 0.0;
//     if(edm_true < 0.0) edm_true = 0.0;
    edm_rec = edm_rec/GeV;
    edm_true = edm_true/GeV;

    sum_edm_rec += edm_rec/( event->sigma2 );
    sum_edm_true += edm_true/( event->sigma2 );
    hi2 += (event->weight*(edm_rec - edm_true)*(edm_rec - edm_true)/( event->sigma2 ) );
    nsel++;
  }

  if(nsel == 0) {
    std::cout << "nsel == 0 ! PANIC!" << std::endl;
    hi2=99999999999.;
  }else{
    if(m_nstep_fcn%20 == 0) {
      std::cout << ">>> step:" << m_nstep_fcn 
      << " size:(" << m_minimSample.size() << "," << nsel 
      << ") sum_edm_rec:" << sum_edm_rec/float(nsel) 
      << " sum_edm_true:" << sum_edm_true/float(nsel)
      << " hi2:" << hi2
      << " npar:" << npar
      << std::endl;
      for(unsigned int i_par=0; i_par<m_minimPars.size(); i_par++){
        if( !m_minimPars[i_par].fixIt ) std::cout << "( " << i_par << " " << m_minimPars[i_par].name << " " << par[i_par] << ")";
      }
      std::cout << std::endl;
    }
  }
  f = hi2;
  m_nstep_fcn++;
}



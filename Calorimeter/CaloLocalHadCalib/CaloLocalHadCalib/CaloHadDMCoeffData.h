/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_CALOHADDMCOEFFDATA_H
#define CALOLOCALHADCALIB_CALOHADDMCOEFFDATA_H

/**
 * @class CaloHadDMCoeffData
 * @version \$Id: CaloHadDMCoeffData.h,v 1.2 2009-05-18 20:31:52 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 23-Februar-2009
 * @brief Data to read from special DeadMaterialTree
 *
 * */

#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TBranch.h>
#include <TFile.h>
#include <TObject.h>

#include "CxxUtils/checker_macros.h"

class TTree;
class TBranch;

class ATLAS_NOT_THREAD_SAFE CaloHadDMCoeffData {
  public :

    TTree          *fChain;   //!pointer to the analyzed TTree or TChain
    Int_t           fCurrent; //!current Tree number in a TChain

    // Declaration of leaf types
    Int_t           m_ncls;
    Int_t           m_mc_pdg;
    Double_t        m_mc_ener;
    Double_t        m_mc_eta;
    Double_t        m_mc_phi;
    std::vector<double>  *m_cls_ener;
    std::vector<double>  *m_cls_ener_unw;
    std::vector<double>  *m_cls_lambda;
    std::vector<double>  *m_cls_eta;
    std::vector<double>  *m_cls_phi;
    std::vector<std::vector<double> > *m_cls_smpener;
    std::vector<std::vector<double> > *m_cls_smpener_unw;
    Int_t           m_narea;
    std::vector<std::vector<double> > *m_cls_eprep;
    std::vector<std::vector<double> > *m_cls_dmener;
    Double_t m_engClusSumCalib;
    std::vector<double>  *m_cls_engcalib;
    std::vector<int >    *m_cls_recostat;
    std::vector<double>  *m_cls_pi0prob;
    std::vector<double>  *m_cls_isol;
    std::vector<double>  *m_cls_oocener;
    std::vector<double>  *m_cls_calib_emfrac;
    std::vector<double>  *m_cls_engcalibpres;

    // List of branches
    TBranch        *b_ncls;   //!
    TBranch        *b_mc_pdg;   //!
    TBranch        *b_mc_ener;   //!
    TBranch        *b_mc_eta;   //!
    TBranch        *b_mc_phi;   //!
    TBranch        *b_cls_ener;   //!
    TBranch        *b_cls_ener_unw;   //!
    TBranch        *b_cls_lambda;   //!
    TBranch        *b_cls_eta;   //!
    TBranch        *b_cls_phi;   //!
    TBranch        *b_cls_smpener_unw;   //!
    TBranch        *b_m_narea;   //!
    TBranch        *b_cls_eprep;   //!
    TBranch        *b_cls_dmener;   //!
    TBranch        *b_engClusSumCalib; //!
    TBranch        *b_cls_engcalib; //!
    TBranch        *b_cls_recostat; //!
    TBranch        *b_cls_pi0prob; //!
    TBranch        *b_cls_isol; //!
    TBranch        *b_cls_oocener; //!
    TBranch        *b_cls_calib_emfrac; //!
    TBranch        *b_cls_engcalibpres; //!

    int m_MaxEventsPerFile;
    int m_classify_type;
    enum keys_cluster {kCLASSIFY_USE_PDG, kCLASSIFY_USE_PARTICLEID, kCLASSIFY};

    CaloHadDMCoeffData(TTree *tree);
    virtual ~CaloHadDMCoeffData();
    virtual Int_t    GetEntry(Long64_t entry);
    virtual Int_t    GetEntries();
    virtual void     Init(TTree *tree);

    int PackClusterVars(int iClus, std::vector<float> &vars);
    TTree *MakeTree(const char *treename);
    void clear();
    void SetClassificationType(int type) {m_classify_type = type;}
    void SetMaxEventsPerFile(int nev) {m_MaxEventsPerFile = nev;}
};

#endif


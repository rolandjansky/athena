/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/CaloMuonLikelihoodTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "PathResolver/PathResolver.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"
#include "TrkParameters/TrackParameters.h"

#include "TFile.h"
#include "TH1F.h"
#include <TString.h> // for Form

#include <string>
#include <iostream>
#include <cmath>
#include <map>

///////////////////////////////////////////////////////////////////////////////
// CaloMuonLikelihoodTool constructor
///////////////////////////////////////////////////////////////////////////////
CaloMuonLikelihoodTool::CaloMuonLikelihoodTool(const std::string& type, const std::string& name, const IInterface* parent) : 
    AthAlgTool(type,name,parent),
    m_fileNames() {
  declareInterface<ICaloMuonLikelihoodTool>(this);
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonLikelihoodTool::initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonLikelihoodTool::initialize() {
  ATH_MSG_INFO("Initializing " << name());
  ATH_CHECK(m_caloExtensionTool.retrieve());
  m_fileNames = {
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.A0.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.A1.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.A2.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.B0.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.B1.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.B2.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.C0.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.C1.root",m_calibRelease.value().c_str())),
    PathResolverFindCalibFile(Form("%s/CaloMuonLikelihood.PDF.C2.root",m_calibRelease.value().c_str()))
  };
  ATH_CHECK(retrieveHistograms());
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonLikelihoodTool::retrieveHistograms
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonLikelihoodTool::retrieveHistograms() {
  ATH_MSG_DEBUG("in CaloMuonLikelihoodTool::retrieveHistograms()");
  
  for ( int iFile = 0; iFile < 9; iFile++ ) {
    m_numKeys[iFile] = 0;
    const std::string& fileName = m_fileNames[iFile]; 
    
    for (int iHist=0; iHist<11; iHist++ ) {
      m_TH1F_sig[iFile][iHist] = 0;
      m_TH1F_bkg[iFile][iHist] = 0;
    }

    // --- Retrieving root files and list of keys ---
    
    TFile* rootFile = TFile::Open(fileName.c_str(), "READ");	
    if (!rootFile) {
      ATH_MSG_FATAL("Could not retrieve root file: " << fileName);
      return StatusCode::FAILURE;
    }
    TList* listOfKeys = rootFile->GetListOfKeys();
    if (!listOfKeys) {
      ATH_MSG_FATAL("Could not retrieve key list: " << fileName);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG(listOfKeys->GetSize() << "histogram keys found in " << fileName);
    if (listOfKeys->GetSize() > 22) {
      ATH_MSG_FATAL("This exceeds the maximum allowed number of histograms");
      return StatusCode::FAILURE;
    }

    int numKeysSignal = 0;
    int numKeysBkg    = 0;
    // --- Retrieving individual histograms ---
    for ( int iHist = 0; iHist < listOfKeys->GetSize(); iHist++ ) {
      const std::string histName = listOfKeys->At(iHist)->GetName();
      TH1F* hist = (TH1F*)(rootFile->Get(histName.c_str()));
      bool isSignal = false;
      if ( !hist ) {
        ATH_MSG_ERROR("cannot retrieve hist " << histName);
      }
      size_t endOfKey = histName.find("_signal", 0);
      if ( endOfKey!=std::string::npos ) {
        isSignal = true;
      }
      else {
        endOfKey = histName.find("_bgnd", 0);
        if ( endOfKey==std::string::npos ) {
          ATH_MSG_ERROR("Histogram with name: " << histName << " does not following the naming convention.");
          return StatusCode::FAILURE;
        }
      }
      const std::string key = histName.substr(0, endOfKey);
      ATH_MSG_DEBUG("Found histogram for " << (isSignal? "signal" : "background") << " with key " << key);
      if ( isSignal ) {
        m_TH1F_key[iFile][numKeysSignal] = key;        
        m_TH1F_sig[iFile][numKeysSignal] = hist;
        ATH_MSG_VERBOSE(m_TH1F_sig[iFile][numKeysSignal]->GetNbinsX());
        numKeysSignal++;
      }
      else {
        m_TH1F_bkg[iFile][numKeysBkg] = hist;
        numKeysBkg++;
      }
    }
    
    if ( numKeysSignal!=numKeysBkg ) {
      ATH_MSG_ERROR("The number of background histograms should be equal to the number of signal histograms.");
      return StatusCode::FAILURE;
    }
    m_numKeys[iFile] = numKeysSignal;

    ATH_MSG_DEBUG("Retrieved histograms from " << fileName);
    
  }
  
  m_cnt_warn = 0;

  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonLikelihoodTool::getLHR
///////////////////////////////////////////////////////////////////////////////
double CaloMuonLikelihoodTool::getLHR( const xAOD::TrackParticle* trk, const xAOD::CaloClusterContainer* ClusContainer, const double dR_CUT ) const {  
  ATH_MSG_DEBUG("in CaloMuonLikelihoodTool::getLHR()");

  if(trk && ClusContainer){
    double eta_trk = trk->eta();
    double p_trk = 0;
    double qOverP = trk->qOverP();
    if (qOverP!=0) p_trk = std::abs(1/qOverP);

    std::unique_ptr<Trk::CaloExtension> caloExt=m_caloExtensionTool->caloExtension(*trk);

    if(!caloExt) return 0;
    const Trk::TrackParameters* caloEntryInterSec = caloExt->caloEntryLayerIntersection();
    if(!caloEntryInterSec) {
      ATH_MSG_WARNING("getLHR() - caloEntryLayerIntersection is nullptr");
      return 0;
    }
    double eta_trkAtCalo = caloEntryInterSec->eta();
    double phi_trkAtCalo = caloEntryInterSec->parameters()[Trk::phi];

    double LR = getLHR( ClusContainer, eta_trk, p_trk, eta_trkAtCalo, phi_trkAtCalo, dR_CUT);
    return LR;
  }
    
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// CaloMuonLikelihoodTool::getLHR
///////////////////////////////////////////////////////////////////////////////
double CaloMuonLikelihoodTool::getLHR(const xAOD::CaloClusterContainer* ClusCollection, const double eta_trk, const double p_trk, const double eta_trkAtCalo, const double phi_trkAtCalo, const double dR_CUT) const {

  const double dR_CUT2=dR_CUT*dR_CUT;

  double etot_em = 0;
  double etot_hd = 0;
  double etot    = 0;

  double s[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  ATH_MSG_DEBUG("Loop over the CaloTopClusters...");
  for(xAOD::CaloClusterContainer::const_iterator iter = ClusCollection->begin(); iter != ClusCollection->end(); ++iter) {
    const xAOD::CaloCluster* theClus = *iter;
    double dphi = std::abs(theClus->phi() - phi_trkAtCalo);
    if(dphi > M_PI) dphi = 2*M_PI-dphi;
    const double deta = std::abs(theClus->eta() - eta_trkAtCalo);
    const double rij2=deta*deta + dphi*dphi;
    if (rij2>dR_CUT2) continue;

    double eemb0   = theClus->eSample(CaloSampling::PreSamplerB);
    double eemb1   = theClus->eSample(CaloSampling::EMB1);
    double eemb2   = theClus->eSample(CaloSampling::EMB2);
    double eemb3   = theClus->eSample(CaloSampling::EMB3);
    double eeme0   = theClus->eSample(CaloSampling::PreSamplerE);
    double eeme1   = theClus->eSample(CaloSampling::EME1);
    double eeme2   = theClus->eSample(CaloSampling::EME2);
    double eeme3   = theClus->eSample(CaloSampling::EME3);
    double etileg1 = theClus->eSample(CaloSampling::TileGap1);
    double etileg2 = theClus->eSample(CaloSampling::TileGap2);
    double etileg3 = theClus->eSample(CaloSampling::TileGap3);

    s[0]  += eemb0;
    s[1]  += eemb1;
    s[2]  += eemb2;
    s[3]  += eemb3;
    s[4]  += eeme0;
    s[5]  += eeme1;
    s[6]  += eeme2;
    s[7]  += eeme3;
    s[8]  += etileg1;
    s[9]  += etileg2;
    s[10] += etileg3;

    etot_em += eemb0+eemb1+eemb2+eemb3+
               eeme0+eeme1+eeme2+eeme3+
               etileg1+etileg2+etileg3;

    double etileb0 = theClus->eSample(CaloSampling::TileBar0);
    double etileb1 = theClus->eSample(CaloSampling::TileBar1);
    double etileb2 = theClus->eSample(CaloSampling::TileBar2);
    double etilee0 = theClus->eSample(CaloSampling::TileExt0);
    double etilee1 = theClus->eSample(CaloSampling::TileExt1);
    double etilee2 = theClus->eSample(CaloSampling::TileExt2);
    double ehec0   = theClus->eSample(CaloSampling::HEC0);
    double ehec1   = theClus->eSample(CaloSampling::HEC1);
    double ehec2   = theClus->eSample(CaloSampling::HEC2);
    double ehec3   = theClus->eSample(CaloSampling::HEC3);
    double efcal0  = theClus->eSample(CaloSampling::FCAL0);
    double efcal1  = theClus->eSample(CaloSampling::FCAL0);
    double efcal2  = theClus->eSample(CaloSampling::FCAL0);

    s[11] += etileb0;
    s[12] += etileb1;
    s[13] += etileb2;
    s[14] += etilee0;
    s[15] += etilee1;
    s[16] += etilee2;
    s[17] += ehec0;
    s[18] += ehec1;
    s[19] += ehec2;
    s[20] += ehec3;
    s[21] += efcal0;
    s[22] += efcal1;
    s[23] += efcal2;

    etot_hd += etileb0+etileb1+etileb2+
               etilee0+etilee1+etilee2+
               ehec0+ehec1+ehec2+ehec3+
               efcal0+efcal1+efcal2;

    etot    += eemb0+eemb1+eemb2+eemb3+
               eeme0+eeme1+eeme2+eeme3+
               etileg1+etileg2+etileg3+
               etileb0+etileb1+etileb2+
               etilee0+etilee1+etilee2+
               ehec0+ehec1+ehec2+ehec3+
               efcal0+efcal1+efcal2;
  }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Values extracted from the CaloTopoClusters for a cone of: " << dR_CUT << "\n"
                      << " - Energy in Calorimeter Total: "<< etot << "  EM: " << etot_em << "  HAD: "<< etot_hd
                      << "\n      eemb0: " << s[0]
                      << "\n      eemb1: " << s[1]
                      << "\n      eemb2: " << s[2]
                      << "\n      eemb3: " << s[3]
                      << "\n      eeme0: " << s[4]
                      << "\n      eeme1: " << s[5]
                      << "\n      eeme2: " << s[6]
                      << "\n      eeme3: " << s[7]
                      << "\n    etileg1: " << s[8]
                      << "\n    etileg2: " << s[9]
                      << "\n    etileg3: " << s[10]
                      << "\n    etileb0: " << s[11]
                      << "\n    etileb1: " << s[12]
                      << "\n    etileb2: " << s[13]
                      << "\n    etilee0: " << s[14]
                      << "\n    etilee1: " << s[15]
                      << "\n    etilee2: " << s[16]
                      << "\n      ehec0: " << s[17]
                      << "\n      ehec1: " << s[18]
                      << "\n      ehec2: " << s[19]
                      << "\n      ehec3: " << s[20]
                      << "\n     efcal0: " << s[21]
                      << "\n     efcal1: " << s[22]
                      << "\n     efcal2: " << s[23] << endmsg;
  }

  for(int i=0;i<24;++i) s[i] /= Gaudi::Units::GeV;
  etot_em /= Gaudi::Units::GeV;
  etot_hd /= Gaudi::Units::GeV;
  etot /= Gaudi::Units::GeV;

  double tmp_mx(-999), tmp_mxH(-999), tmp_mxE(-999);
  for (int i=0; i<24; ++i) {
    if (s[i]>tmp_mx          ) { tmp_mx =s[i]; }
    if (s[i]>tmp_mxE && i<11 ) { tmp_mxE=s[i]; }
    if (s[i]>tmp_mxH && i>=11) { tmp_mxH=s[i]; }
  }

  double emFr(999), mxFr(999), mxEM(999), mxHad(999);
  double EoverEtrk(999);
  double eemb1_wrtTotal(999), eemb2_wrtTotal(999), eemb3_wrtGroup(999), etileb0_wrtGroup(999), etileb1_wrtTotal(999), etileb2_wrtGroup(999), eeme1_wrtGroup(999), eeme1_wrtTotal(999), eeme2_wrtTotal(999), eeme3_wrtGroup(999), ehec0_wrtTotal(999);

  if(etot>0) {
    mxFr  = tmp_mx/etot;
    mxEM  = tmp_mxE/etot;
    mxHad = tmp_mxH/etot;
    emFr  = etot_em/etot;
    eemb1_wrtTotal   = s[1]/etot;
    eemb2_wrtTotal   = s[2]/etot;
    etileb1_wrtTotal = s[12]/etot;
    eeme1_wrtTotal   = s[5]/etot;
    eeme2_wrtTotal   = s[6]/etot;
    ehec0_wrtTotal   = s[17]/etot;
  }

  // p_trk in MeV
  if (p_trk)
    EoverEtrk = etot/(p_trk/Gaudi::Units::GeV);

  if(etot_em>0) {
    eemb3_wrtGroup = s[3]/etot_em;
    eeme1_wrtGroup = s[5]/etot_em;
    eeme3_wrtGroup = s[7]/etot_em;
  }

  if(etot_hd>0) {
    etileb0_wrtGroup = s[11]/etot_hd;
    etileb2_wrtGroup = s[13]/etot_hd;
  }

  // likelihood discriminant variables
  std::map<std::string, double> vars;
  vars["emFr"] = emFr;
  vars["EoverEtrk"] = EoverEtrk;
  vars["mxFr"] = mxFr;
  vars["mxEM"] = mxEM;
  vars["mxHad"] = mxHad;
  vars["eemb1_wrtTotal"] = eemb1_wrtTotal;
  vars["eemb2_wrtTotal"] = eemb2_wrtTotal;
  vars["eemb3_wrtGroup"] = eemb3_wrtGroup;
  vars["etileb0_wrtGroup"] = etileb0_wrtGroup;
  vars["etileb1_wrtTotal"] = etileb1_wrtTotal;
  vars["etileb2_wrtGroup"] = etileb2_wrtGroup;
  vars["eeme1_wrtGroup"] = eeme1_wrtGroup;
  vars["eeme1_wrtTotal"] = eeme1_wrtTotal;
  vars["eeme2_wrtTotal"] = eeme2_wrtTotal;
  vars["eeme3_wrtGroup"] = eeme3_wrtGroup;
  vars["ehec0_wrtTotal"] = ehec0_wrtTotal;

  ATH_MSG_DEBUG("likelihood discriminant variables values: " << dR_CUT);
  std::map<std::string,double>::iterator iter;
  for( iter = vars.begin(); iter != vars.end(); iter++ ) 
    ATH_MSG_DEBUG("  - " << iter->first << ": " << iter->second);
  
  double LR = 0;
  double ProbS(1), ProbB(1);

  int iFile = -1;
  if(std::abs(eta_trk)<1.4) {
    if(p_trk/Gaudi::Units::GeV<11.) iFile = 0;
    else if(p_trk/Gaudi::Units::GeV<51.) iFile = 1;
    else iFile = 2;
  }
  else if(std::abs(eta_trk)>=1.4&&std::abs(eta_trk)<=1.6) {
    if(p_trk/Gaudi::Units::GeV<11.) iFile = 3;
    else if(p_trk/Gaudi::Units::GeV<51.) iFile = 4;
    else iFile = 5;
  }
  else if(std::abs(eta_trk)>1.6&&std::abs(eta_trk)<2.5) {
    if(p_trk/Gaudi::Units::GeV<11.) iFile = 6;
    else if(p_trk/Gaudi::Units::GeV<51.) iFile = 7;
    else iFile = 8;
  }

  if(iFile<0) return LR;

  if(m_numKeys[iFile]==0) {
    ProbS = 0;
    ProbB = 1;
  } 
  else {
    for(int i=0;i<m_numKeys[iFile];i++) {
      std::map<std::string,double>::iterator it = vars.find(m_TH1F_key[iFile][i]);

      ATH_MSG_VERBOSE("getLHR " 
		      << ": m_TH1F_key["<< iFile << "][" << i << "(/" 
		      << m_numKeys[iFile] << ")] = " << m_TH1F_key[iFile][i] 
		      << ", " << ((it!=vars.end()) ? "found" : "not found"));  

      if(it != vars.end()) {
        int bin_sig = m_TH1F_sig[iFile][i]->GetXaxis()->FindFixBin(it->second);

      	ATH_MSG_VERBOSE("getLHR sig " 
      			<< ": it->first = "  << it->first 
      			<< ", it->second = " << it->second 
      			<< ", bin_sig = "    << bin_sig 
      			<< ", NbinsX = "     << m_TH1F_sig[iFile][i]->GetNbinsX());

        if(bin_sig>=1 && bin_sig<=m_TH1F_sig[iFile][i]->GetNbinsX()) {
          double SbinContent = m_TH1F_sig[iFile][i]->GetBinContent(bin_sig);
          ATH_MSG_DEBUG("m_TH1F_sig Bin Content for " << it->first << ": " << SbinContent);
          if(SbinContent)
            ProbS *= SbinContent; 
          else {
            ProbS *= 0.0000001;	    
            ATH_MSG_DEBUG("BinContent in m_TH1F_sig was 0!" << ProbS);
          }
        }  
        else {
          ATH_MSG_DEBUG("Sbin:" << bin_sig << " and NBinMax: " << m_TH1F_sig[iFile][i]->GetNbinsX());
          ProbS *= 1;
        }
        ATH_MSG_DEBUG("Temp ProbS : " << ProbS);
        int bin_bkg = m_TH1F_bkg[iFile][i]->GetXaxis()->FindFixBin(it->second);

      	ATH_MSG_VERBOSE("getLHR bkg " 
      			<< ": it->first = "  << it->first 
      			<< ", it->second = " << it->second 
      			<< ", bin_bkg = "    << bin_bkg 
      			<< ", NbinsX = "     << m_TH1F_bkg[iFile][i]->GetNbinsX());

        if(bin_bkg>=1 && bin_bkg<=m_TH1F_bkg[iFile][i]->GetNbinsX()) {
          double BbinContent = m_TH1F_bkg[iFile][i]->GetBinContent(bin_bkg);
          ATH_MSG_VERBOSE("m_TH1F_bkg Bin Content for " << it->first << ": " << BbinContent);
          if(BbinContent)
            ProbB *= BbinContent;
          else {
            ProbB *= 0.0000001;
            ATH_MSG_DEBUG("BinContent in m_TH1F_bkg was 0! " << ProbB);
          }
        } 
        else {
          ATH_MSG_DEBUG("Bbin:" << bin_bkg << " and NBinMax: " << m_TH1F_bkg[iFile][i]->GetNbinsX());
          ProbB *= 1;
        }
        ATH_MSG_DEBUG("      Temp ProbB: " << ProbB);
      } 
      else {
        if(m_cnt_warn<10) {
          ATH_MSG_WARNING("Histogram variable <" << m_TH1F_key[iFile][i] << "> in file <" << m_fileNames[iFile] << "> is not found in the calculated variable list");
          m_cnt_warn++;
        }
      }
    }
  }

  if(ProbS==0) LR = 0;
  else LR = ProbS/(ProbS+ProbB);

  ATH_MSG_DEBUG("ProbS : " << ProbS);
  ATH_MSG_DEBUG("ProbB : " << ProbB);
  ATH_MSG_DEBUG("LR    : " << LR);
  return LR;
}

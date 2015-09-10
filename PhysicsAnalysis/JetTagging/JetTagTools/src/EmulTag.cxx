/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/EmulTag.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include <CLHEP/Random/Randomize.h>

//#include "JetEvent/Jet.h"
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/AtlfInfo.h"

#include "TH1F.h"
#include "TH2F.h"

namespace Analysis {

  EmulTag::EmulTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p) {
    declareInterface<ITagTool>(this);
    declareProperty("Mode",     m_mode      = 2);
    declareProperty("ParamSet", m_atlfBNSet = 10);
    declareProperty("Tagger",   m_algoType);
    m_algoType.push_back("SV1IP3D");
    m_algoType.push_back("6d");
  }

  EmulTag::~EmulTag() { }

  StatusCode EmulTag::initialize() {

    m_imod = m_mode;
    if (m_imod != 1 && m_imod != 2) {
      ATH_MSG_ERROR("Unknown mode, use emulation instead !");
      m_imod = 2;
    }
    
    if (m_imod == 2) {
      // Checking that the property is consistent
      for (unsigned int i = 0; i < m_algoType.size(); i++) m_ialgoType.push_back(m_algoType[i]);
      if (m_ialgoType.size() != 2) {
	ATH_MSG_ERROR("Tagger should be a 2-vector of strings, use SV1+IP3D emulation !"); 
	m_ialgoType.clear();
	m_ialgoType.push_back("SV1IP3D");
	m_ialgoType.push_back("6d");
      }

      ATH_MSG_INFO("Initializing " << name() << " Tagger = " << m_ialgoType[0]);
      std::string type = m_ialgoType[1];
      StatusCode sc = initHistos(type);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("Unable to initialize weight histograms");
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_INFO("Initializing " << name() << " with parameterisation " << m_atlfBNSet);
      StatusCode sc = initParam(m_atlfBNSet);
      if (sc.isFailure()) {
	ATH_MSG_ERROR("Unable to initialize parameterisation");
	return StatusCode::FAILURE;
      }
    }
 
    return StatusCode::SUCCESS;
  }      


  StatusCode EmulTag::finalize() {
    return StatusCode::SUCCESS;
  }

  void EmulTag::tagJet(xAOD::Jet& jetToTag) {

    // Jet properties
    double ptjet  = jetToTag.p4().Et()/1.e3;
    double etajet = jetToTag.eta(); 
    int ip = findEt(ptjet);
    int ie = findEta(etajet);
    int pdg    = abs(jetToTag.pdgId());
    double dRb = 999., dRc = 999., dRt = 999.;
    const Analysis::AtlfInfo* ainfo = jetToTag.tagInfo<Analysis::AtlfInfo>("AtlfInfo");
    if (ainfo) {
      dRb = ainfo->deltaRMinTo("B");
      dRc = ainfo->deltaRMinTo("C");
      dRt = ainfo->deltaRMinTo("T");
    }
    //
    bool isB       = false;
    double epsilon = 0.6;
    double randnum = 0.; if (m_imod == 1) randnum = CLHEP::RandFlat::shoot();
    double w       = -20.;
    bool firstBin  = true;

    // This condition to be sure (but the tool should be called only if so...)
    if (fabs(etajet) <= 2.5) {
      if (pdg == 5) {
	if (m_imod == 2) {
	  if (m_hWb[ie][ip]) {
	    w = m_Wb[ie][ip]->GetRandom();
	    if (m_Wb[ie][ip]->GetXaxis()->FindBin(w) > 1) firstBin = false; 
	  }
	} else {
	  epsilon = m_EpsB->GetBinContent(ip+1,ie+1);
	}
      } else {
	int ifl = 0;
	int iso = 1;
	if (pdg == 0) {
	  if (dRb < 0.8 || dRc < 0.8 || dRt < 0.8) iso = 0;
	}
	if (pdg == 4)  {
	  ifl = 1;
	  if (dRb < 0.8) iso = 0;
	}
	if (pdg == 15) {
	  ifl = 2;
	  if (dRb < 0.8 || dRc < 0.8) iso = 0;
	}
	if (m_imod == 2) {
	  if (m_Wl[iso][ifl][ie][ip]) {
	    w = m_Wl[iso][ifl][ie][ip]->GetRandom();
	    if (m_Wl[iso][ifl][ie][ip]->GetXaxis()->FindBin(w) > 1) firstBin = false; 
	  }
	} else {
	  if (ifl == 0) {
	    if (iso == 1) epsilon = 1./m_RejLightNP->GetBinContent(ip+1,ie+1);
	    if (iso == 0) epsilon = 1./m_RejLightP->GetBinContent(ip+1,ie+1);
	  } else if (ifl == 4) { 
	    epsilon = 1./m_RejC->GetBinContent(ip+1,ie+1);
	  } else if (ifl == 15) {
	    epsilon = 1./m_RejTau->GetBinContent(ip+1,ie+1);
	  } 
	}
      }
    }

    if (m_imod == 2) {
      // Fill the info
      BaseTagInfo* info = new BaseTagInfo(m_ialgoType[0]);
      std::vector<double> like;
      like.push_back(1.);
      double pu = firstBin ? 1.e9 : exp(-w);
      like.push_back(pu);
      info->setTagLikelihood(like);
      info->makeValid();
      jetToTag.addInfo(info);
    } else {
      if (randnum < epsilon) isB = true;
      jetToTag.setCombinedLikelihood( std::vector<double>( 1, isB 
							  ? 1. 
							  : 0. ) );

    }
  }

  void EmulTag::finalizeHistos() { }

  StatusCode EmulTag::initHistos(std::string algN) {
    //
    StatusCode sc = StatusCode::SUCCESS;
    //
    ITHistSvc* histoSvc;
    sc = service("THistSvc", histoSvc);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("THistSvc not found!");
      return StatusCode::FAILURE;
    }
    //
    std::string fullPath = "/WeightFile/";
    std::string flav[4] = {"bw","uw","cw","tw"};
    for (int fl = 0; fl < 4; fl++) {
      for (int ie = 0; ie < m_etabin-1; ie++) {
        for (int ip = 0; ip < m_ptbin-1; ip++) {
          std::ostringstream oe, op;
          oe << ie;
          op << ip;
          std::string hName = fullPath + flav[fl] + algN + "e" + oe.str() + "p" + op.str();
	  if (fl == 0) {
	    sc = histoSvc->regHist(hName);
	    sc = histoSvc->getHist(hName,m_Wb[ie][ip]);
	  } else {
	    sc = histoSvc->regHist(hName+"i");
	    sc = histoSvc->getHist(hName+"i",m_Wl[1][fl-1][ie][ip]);
	    sc = histoSvc->regHist(hName+"ni");
	    sc = histoSvc->getHist(hName+"ni",m_Wl[0][fl-1][ie][ip]);
	  }
	  if (sc.isFailure()) {
	    ATH_MSG_FATAL("Missing weight histograms " << hName);
	    return StatusCode::FAILURE;
	  }
        }
      }
    }
    // Check
    for (int fl = 0; fl < 4; fl++) {
      for (int ie = 0; ie < m_etabin-1; ie++) {
        for (int ip = 0; ip < m_ptbin-1; ip++) {
          if (fl == 0) {
            m_hWb[ie][ip] = true;
            if (m_Wb[ie][ip]->Integral() <= 0.) {
              ATH_MSG_WARNING("histo " << flav[fl] << " " << algN << " " << ie << " " << ip << " empty");
              m_hWb[ie][ip] = false;
            }
          } else {
            m_hWl[0][fl-1][ie][ip] = true;
            m_hWl[1][fl-1][ie][ip] = true;
            if (m_Wl[0][fl-1][ie][ip]->Integral() <= 0.) {
              ATH_MSG_WARNING("histo " << flav[fl] << " "  << algN << " " << ie << " " << ip << " empty");
              m_hWl[0][fl-1][ie][ip] = false;
            }
            if (m_Wl[1][fl-1][ie][ip]->Integral() <= 0.) {
              ATH_MSG_WARNING("histo " << flav[fl] << " "  << algN << " " << ie << " " << ip << " empty");
              m_hWl[1][fl-1][ie][ip] = false;
            }
          }
        }
      }
    }
    return sc;
  }

  StatusCode EmulTag::initParam(unsigned int iset) {
    //
    StatusCode sc = StatusCode::SUCCESS;
    //
    double m_epsilonBjet = 0.6;
    unsigned int ialgo = iset;
    std::string algName = "IP2D"; 
    if ((ialgo > 7 && ialgo < 15) || (ialgo > 21 && ialgo < 29)) algName = "SV1+IP3D";
    if (ialgo == 100) {
      algName = "Canonical";
    }
    // The efficiency is defined by m_atlfBNSet (for m_atlfBNSet <= 14)
    if (m_atlfBNSet <= 14 && ialgo > 7) ialgo -= 7;
    m_epsilonBjet = 0.5 + (ialgo - 1)*0.05;
    msg(MSG::INFO) << "Running with " << algName;
    if (m_atlfBNSet < 15 || m_atlfBNSet == 100) msg(MSG::INFO) << " at eps_b = " << m_epsilonBjet << endreq;
    if (m_atlfBNSet > 14)                       msg(MSG::INFO) << " with a fixed cut" << endreq;
    
    // Histrogram loading
    ITHistSvc* histoSvc;
    sc = service("THistSvc", histoSvc);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("THistSvc not found!");
      return StatusCode::FAILURE;
    }
    //
    std::string fullPath = "/ParamFile/";
    std::ostringstream o;
    o << iset;
    std::string hisname = fullPath+"BEfficiency"+o.str();
    sc = histoSvc->regHist(hisname);
    sc = histoSvc->getHist(hisname,m_EpsB);
    hisname = fullPath+"RejectionFactorPU"+o.str();
    sc = histoSvc->regHist(hisname);
    sc = histoSvc->getHist(hisname,m_RejLightP);
    hisname = fullPath+"RejectionFactorNPU"+o.str();
    sc = histoSvc->regHist(hisname);
    sc = histoSvc->getHist(hisname,m_RejLightNP);
    hisname = fullPath+"RejectionFactorC"+o.str();
    sc = histoSvc->regHist(hisname);
    sc = histoSvc->getHist(hisname,m_RejC);
    hisname = fullPath+"RejectionFactorTau"+o.str();
    sc = histoSvc->regHist(hisname);
    sc = histoSvc->getHist(hisname,m_RejTau);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Missing parameterisation histograms ");
      return StatusCode::FAILURE;
    }
    //
    return sc;
  }

  int EmulTag::findEta(double eta) {
    if (fabs(eta) < m_ebin[1]  && fabs(eta) >= m_ebin[0]) return 0;
    if (fabs(eta) < m_ebin[2]  && fabs(eta) >= m_ebin[1]) return 1;
    if (fabs(eta) < m_ebin[3]  && fabs(eta) >= m_ebin[2]) return 2;
    if (fabs(eta) < m_ebin[4]  && fabs(eta) >= m_ebin[3]) return 3;
    if (fabs(eta) <= m_ebin[5] && fabs(eta) >= m_ebin[4]) return 4;
    return 0;
  }
  int EmulTag::findEt(double et) {
    if (fabs(et) < m_pbin[1]  && fabs(et) >= m_pbin[0]) return 0;
    if (fabs(et) < m_pbin[2]  && fabs(et) >= m_pbin[1]) return 1;
    if (fabs(et) < m_pbin[3]  && fabs(et) >= m_pbin[2]) return 2;
    if (fabs(et) < m_pbin[4]  && fabs(et) >= m_pbin[3]) return 3;
    if (fabs(et) < m_pbin[5]  && fabs(et) >= m_pbin[4]) return 4;
    if (fabs(et) < m_pbin[6]  && fabs(et) >= m_pbin[5]) return 5;
    if (fabs(et) < m_pbin[7]  && fabs(et) >= m_pbin[6]) return 6;
    if (fabs(et) < m_pbin[8]  && fabs(et) >= m_pbin[7]) return 7;
    if (fabs(et) <= m_pbin[9] && fabs(et) >= m_pbin[8]) return 8;
    return 0;
  }

}


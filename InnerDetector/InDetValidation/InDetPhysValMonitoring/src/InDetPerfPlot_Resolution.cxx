/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Resolution.cxx
 * @author nora pettersson
 **/

#include "InDetPerfPlot_Resolution.h"
#include <map>
#include <utility>
#include <cmath>
#include "logLinearBinning.h"

InDetPerfPlot_Resolution::InDetPerfPlot_Resolution(InDetPlotBase* pParent, const std::string& sDir)  : InDetPlotBase(pParent, sDir),
  m_meanWidthMethod(IDPVM::GetMeanWidth::iterRMS_convergence),
  m_primTrk(false),
  m_secdTrk(false),
  m_allTrk(false),

  m_trkP{},
  m_truetrkP{},
  m_trkErrP{},

  m_resP{},
  m_pullP{},
  m_sigP{},

  m_pull{},
  m_res{},
  m_sigma{},

  m_resHelpereta{},
  m_resHelperpt{},
  m_pullHelpereta{}, 
  m_pullHelperpt{}, 

  m_reswidth_vs_eta{},
  m_resmean_vs_eta{},
  m_reswidth_vs_pt{},
  m_resmean_vs_pt{},

  m_pullwidth_vs_eta{},
  m_pullmean_vs_eta{},
  m_pullwidth_vs_pt{},
  m_pullmean_vs_pt{},

  m_resHelpereta_pos{},
  m_resHelpereta_neg{},
  m_resHelperpt_pos{}, 
  m_resHelperpt_neg{},

  m_reswidth_vs_eta_pos{},
  m_resmean_vs_eta_pos{},
  m_reswidth_vs_pt_pos{},
  m_resmean_vs_pt_pos{},

  m_reswidth_vs_eta_neg{},
  m_resmean_vs_eta_neg{},
  m_reswidth_vs_pt_neg{},
  m_resmean_vs_pt_neg{},

  m_pullProjections_vs_pt{},
  m_pullProjections_vs_eta{},
  m_resProjections_vs_pt{},
  m_resProjections_vs_eta{},

  m_sigma_vs_eta{},
  m_sigma_vs_pt{}
  {
    
  TString tsDir = (TString) sDir;

  if (tsDir.Contains("Primary")) {
    m_primTrk = true; // control if sec/prim from init
  } else if (tsDir.Contains("Secondary")) {
    m_secdTrk = true;
  } else {
    m_allTrk = true;
  }

   // Using globally defined bin limits
  for (int ieta = 0; ieta <= m_nEtaBins; ieta++) {
    m_EtaBins[ieta] = m_etaMin + ((m_etaMax - m_etaMin) / m_nEtaBins) * ieta;
  }
  
   
  std::vector<double> ptBins = IDPVM::logLinearBinning(m_nPtBins, m_ptMin, m_ptMax, false);
  for (int ipt = 0; ipt <= m_nPtBins; ipt++) {
    m_PtBins[ipt] = (float) ptBins[ipt];
  }
  


}

void
InDetPerfPlot_Resolution::initializePlots() {
  
  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    //
    //1D distributions
    //

    if(iparam == PT) continue;
    book(m_pull[iparam], "pull_" + m_paramProp[iparam]);
    book(m_res[iparam],  "res_" + m_paramProp[iparam]);

    book(m_sigma[iparam], "sigma_" + m_paramProp[iparam]); //New One 
    //
    //2D Distributions to evaluate resolutions vs eta and pT
    //
    book(m_resHelpereta[iparam], "resHelpereta_" + m_paramProp[iparam]);
    book(m_resHelperpt[iparam], "resHelperpt_" + m_paramProp[iparam]);
    book(m_pullHelpereta[iparam], "pullHelpereta_" + m_paramProp[iparam]);
    book(m_pullHelperpt[iparam], "pullHelperpt_" + m_paramProp[iparam]);
    m_resHelperpt[iparam]->GetXaxis()->Set(m_nPtBins,m_PtBins);
    m_pullHelperpt[iparam]->GetXaxis()->Set(m_nPtBins,m_PtBins);
    //
    //1D Histograms for the final resolution and means
    //
    book(m_reswidth_vs_eta[iparam], "reswidth_vs_eta_" + m_paramProp[iparam]);
    book(m_resmean_vs_eta[iparam], "resmean_vs_eta_" + m_paramProp[iparam]);
    book(m_reswidth_vs_pt[iparam], "reswidth_vs_pt_" + m_paramProp[iparam]);
    book(m_resmean_vs_pt[iparam], "resmean_vs_pt_" + m_paramProp[iparam]);

    book(m_pullwidth_vs_eta[iparam], "pullwidth_vs_eta_" + m_paramProp[iparam]);
    book(m_pullmean_vs_eta[iparam], "pullmean_vs_eta_" + m_paramProp[iparam]);
    book(m_pullwidth_vs_pt[iparam], "pullwidth_vs_pt_" + m_paramProp[iparam]);
    book(m_pullmean_vs_pt[iparam], "pullmean_vs_pt_" + m_paramProp[iparam]);

    m_reswidth_vs_pt[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);
    m_resmean_vs_pt[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);
    m_pullwidth_vs_pt[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);
    m_pullmean_vs_pt[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);

    //
    //TProfiles of the sqrt(covii) 
    //
    book(m_sigma_vs_eta[iparam], "sigma_vs_eta_" + m_paramProp[iparam]);
    book(m_sigma_vs_pt[iparam], "sigma_vs_pt_" + m_paramProp[iparam]);

    //NORA
    //
    //Projections only done if high level of detail specified
    //To keep functionalies to produce projections for varied number of pt / eta bins
    //keep histogram to the old Book1D methods instead of xml definitions
    //
    //
    //Detailed histograms
    //
    if(m_iDetailLevel >= 200){
      book(m_resHelpereta_pos[iparam], "resHelpereta_pos" + m_paramProp[iparam]); 
      book(m_resHelpereta_neg[iparam], "resHelpereta_neg" + m_paramProp[iparam]);
      book(m_resHelperpt_pos[iparam],  "resHelperpt_pos" + m_paramProp[iparam]); 
      book(m_resHelperpt_neg[iparam],  "resHelperpt_neg" + m_paramProp[iparam]);

      //Add log binning
      m_resHelperpt_pos[iparam]->GetXaxis()->Set(m_nPtBins,m_PtBins);
      m_resHelperpt_neg[iparam]->GetXaxis()->Set(m_nPtBins,m_PtBins);

      //Resolution, Resolution Mean, Pull, Pull Mean
      book(m_reswidth_vs_eta_pos[iparam], "reswidth_vs_eta_" + m_paramProp[iparam] + "_pos");
      book(m_reswidth_vs_eta_neg[iparam], "reswidth_vs_eta_" + m_paramProp[iparam] + "_neg");
      book(m_resmean_vs_eta_pos[iparam], "resmean_vs_eta_" + m_paramProp[iparam] + "_pos");
      book(m_resmean_vs_eta_neg[iparam], "resmean_vs_eta_" + m_paramProp[iparam] + "_neg");

      book(m_reswidth_vs_pt_pos[iparam], "reswidth_vs_pt_" + m_paramProp[iparam] + "_pos");
      book(m_reswidth_vs_pt_neg[iparam], "reswidth_vs_pt_" + m_paramProp[iparam] + "_neg");
      book(m_resmean_vs_pt_pos[iparam], "resmean_vs_pt_" + m_paramProp[iparam] + "_pos");
      book(m_resmean_vs_pt_neg[iparam], "resmean_vs_pt_" + m_paramProp[iparam] + "_neg");
     
      m_reswidth_vs_pt_pos[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);
      m_resmean_vs_pt_pos[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);
      m_reswidth_vs_pt_neg[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);
      m_resmean_vs_pt_neg[iparam]->GetXaxis()->Set(m_nPtBins, m_PtBins);

      std::string tmpName, tmpTitle;
      float paramResBins[NPARAMS] = { 1000, 2000, 2000, 1000, 1000, 1000, 1000 }; 
      float paramRes[NPARAMS] =     { 1.5,  10.0, 2.5e-5, 0.01, 0.01, 100.0, 0.2 };

      //Projections
      for (unsigned int ibins = 0; ibins < m_nPtBins; ibins++) {
        tmpName = "PtProjections_pullProjection_" + m_paramProp[iparam] + std::to_string(ibins + 1);
        tmpTitle = tmpName + "; (" + m_paramProp[iparam] + "^{reco}-" + m_paramProp[iparam] +
                   "^{true})/#sigma_{" + m_paramProp[iparam] + "}";
        m_pullProjections_vs_pt[iparam][ibins] = Book1D(tmpName, tmpTitle, 200, -10.0, 10.0, false);
      }

      for (unsigned int ibins = 0; ibins < m_nEtaBins; ibins++) {
        tmpName = "EtaProjections_pullProjection_" + m_paramProp[iparam] + std::to_string(ibins + 1);
        tmpTitle = tmpName + "; (" + m_paramProp[iparam] + "^{reco}-" + m_paramProp[iparam] +
                   "^{true})/#sigma_{" + m_paramProp[iparam] + "}";
        m_pullProjections_vs_eta[iparam][ibins] = Book1D(tmpName, tmpTitle, 200, -10.0, 10.0, false);
      }
      for (unsigned int ibins = 0; ibins < m_nPtBins; ibins++) {
        tmpName = "PtProjections_resProjection_" + m_paramProp[iparam] + std::to_string(ibins + 1);
        tmpTitle = tmpName + "; " + m_paramProp[iparam] + "^{reco}-" + m_paramProp[iparam] +
                   "^{true} ";
        m_resProjections_vs_pt[iparam][ibins] =
        Book1D(tmpName, tmpTitle, paramResBins[iparam], -paramRes[iparam], paramRes[iparam], false);
     }
     for (unsigned int ibins = 0; ibins < m_nEtaBins; ibins++) {
       tmpName = "EtaProjections_resProjection_" + m_paramProp[iparam] + std::to_string(ibins + 1);
       tmpTitle = "resProjection_" + m_paramProp[iparam] + std::to_string(ibins + 1) + "; " +
                  m_paramProp[iparam] + "^{reco}-" + m_paramProp[iparam] + "^{true} ";
       m_resProjections_vs_eta[iparam][ibins] =
       Book1D(tmpName, tmpTitle, paramResBins[iparam], -paramRes[iparam], paramRes[iparam], false); 
     }
   }
   //
   //End of saving resolution and pull residual binnings
   //

  }

}

void
InDetPerfPlot_Resolution::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
  // Check whether the track is primary or secondary
  int trueBC = -9999;

  trueBC = truthprt.barcode();
  int isPrimTrk = 0;
  int isSecdTrk = 0;
  if ((trueBC > 0) && (trueBC < 200000)) {
    isPrimTrk = 1;
  }
  if ((trueBC > 200000)) {
    isSecdTrk = 1;
  }

  // Move on to the next track incase the wrong track category
  if (!isPrimTrk && !isSecdTrk) {
    return;
  }
  if (!isPrimTrk && m_primTrk) {
    return;
  }
  if (!isSecdTrk && m_secdTrk) {
    return;
  }

  // Get track paramters for truth and reco tracks for easy access
  // store all in track parameter maps trkP/truetrkP
  getTrackParameters(trkprt);
  getTrackParameters(truthprt);
  getPlotParameters();
  getPlots();

}

void
InDetPerfPlot_Resolution::getPlots() {
  float eta = -TMath::Log(TMath::Tan(m_truetrkP[THETA] / 2));

  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {    
    if(iparam == PT) continue;
    m_pull[iparam]->Fill(m_pullP[iparam]);
    m_res[iparam]->Fill(m_resP[iparam]);
    if(iparam == QOVERPT){
      m_sigma[iparam]->Fill(m_trkP[PT] * m_sigP[iparam]);
      m_sigma_vs_eta[iparam]->Fill(eta, m_trkP[PT] * m_sigP[iparam]);
      m_sigma_vs_pt[iparam]->Fill(m_truetrkP[PT], m_trkP[PT] * m_sigP[iparam]);
    } else {
      m_sigma[iparam]->Fill(m_sigP[iparam]);
      m_sigma_vs_eta[iparam]->Fill(eta, m_sigP[iparam]);
      m_sigma_vs_pt[iparam]->Fill(m_truetrkP[PT], m_sigP[iparam]);
    }
    m_resHelpereta[iparam]->Fill(eta, m_resP[iparam]);
    m_resHelperpt[iparam]->Fill(m_truetrkP[PT], m_resP[iparam]);
    m_pullHelperpt[iparam]->Fill(m_truetrkP[PT], m_pullP[iparam]);
    m_pullHelpereta[iparam]->Fill(eta, m_pullP[iparam]);
    
    if(m_iDetailLevel >= 200){

      if (m_trkP[QOVERPT] >= 0.) {
        m_resHelpereta_pos[iparam]->Fill(eta, m_resP[iparam]);
        m_resHelperpt_pos[iparam]->Fill(m_truetrkP[PT], m_resP[iparam]);
      }
      if (m_trkP[QOVERPT] < 0.) {
        m_resHelpereta_neg[iparam]->Fill(eta, m_resP[iparam]);
        m_resHelperpt_neg[iparam]->Fill(m_truetrkP[PT], m_resP[iparam]);
      }
    }

  }

}

void
InDetPerfPlot_Resolution::getPlotParameters() {
  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    m_resP[iparam] = m_trkP[iparam] - m_truetrkP[iparam];
    m_sigP[iparam] = m_trkErrP[iparam];
    (m_sigP[iparam] != 0) ? m_pullP[iparam] = m_resP[iparam] / m_sigP[iparam] : m_pullP[iparam] = -9999.;
  }
  m_resP[QOVERPT] = m_trkP[PT] * (m_trkP[QOVERPT] - m_truetrkP[QOVERPT]) * (1 / m_truetrkP[QOVERPT]);
  m_sigP[QOVERPT] *= m_trkP[PT];
}

void
InDetPerfPlot_Resolution::getTrackParameters(const xAOD::TrackParticle& trkprt) {

  m_trkP[D0] = trkprt.d0();
  m_trkP[Z0] = trkprt.z0();
  m_trkP[QOVERPT] = trkprt.qOverP() * (1 / std::sin(trkprt.theta()));
  m_trkP[THETA] = trkprt.theta();
  m_trkP[PHI] = trkprt.phi0();
  m_trkP[PT] = trkprt.pt() / Gaudi::Units::GeV;
  m_trkP[Z0SIN] = trkprt.z0() * std::sin(trkprt.theta());

  // Track fit errors
  m_trkErrP[D0] = std::sqrt(trkprt.definingParametersCovMatrix()(0, 0));
  m_trkErrP[Z0] = std::sqrt(trkprt.definingParametersCovMatrix()(1, 1));
  m_trkErrP[PHI] = std::sqrt(trkprt.definingParametersCovMatrix()(2, 2));
  m_trkErrP[THETA] = std::sqrt(trkprt.definingParametersCovMatrix()(3, 3));
  m_trkErrP[QOVERPT] = std::sqrt(trkprt.definingParametersCovMatrix()(4, 4)) * (1 / std::sin(trkprt.theta()));
  m_trkErrP[Z0SIN] = std::sqrt(std::pow(m_trkErrP[Z0] * std::sin(m_trkP[THETA]), 2) + 
                               std::pow(m_trkP[Z0] * m_trkErrP[THETA] * std::cos(m_trkP[THETA]), 2) + 
                               2 * m_trkP[Z0] * std::sin(m_trkP[THETA]) * std::cos(m_trkP[THETA]) * trkprt.definingParametersCovMatrix()(1, 3));  // Fixing incorrect formula for z0sin error


  // Get error on pT, taken from xAOD::TrackingHelpers.pTErr() but this function only works on a pointer input...
  if (trkprt.definingParametersCovMatrixVec().size() < 15) {
    throw std::runtime_error(
            "TrackParticle without covariance matrix for defining parameters or the covariance matrix is wrong dimensionality.");
  }
  if (std::abs(trkprt.qOverP()) < 0) {
    m_trkErrP[PT] = 0.0;
    throw std::runtime_error("q/p is zero");
  } else {
    double pt = trkprt.pt();
    double diff_qp = -pt / std::abs(trkprt.qOverP());
    double diff_theta = pt / tan(trkprt.theta());
    const std::vector<float>& cov = trkprt.definingParametersCovMatrixVec();
    double pt_err2 = diff_qp * (diff_qp * cov[14] + diff_theta * cov[13]) + diff_theta * diff_theta * cov[9];
    m_trkErrP[PT] = sqrt(pt_err2) / Gaudi::Units::GeV;
  }

}

void
InDetPerfPlot_Resolution::getTrackParameters(const xAOD::TruthParticle& truthprt) {
  // "d0", "z0", "phi", "theta" , "qOverP"
  // Perigee for truth particles are in aux container
  int nParams = 6;

  for (int iParams = 0; iParams < nParams; iParams++) {
    m_truetrkP[iParams] = -9999.;
    if (truthprt.isAvailable<float>(m_paramProp[iParams])) {
      m_truetrkP[iParams] = (truthprt.auxdata<float>(m_paramProp[iParams]));
    }
  }

  (truthprt.isAvailable<float>("d0")) ? m_truetrkP[D0] = truthprt.auxdata<float>("d0") : m_truetrkP[D0] = -9999.;
  (truthprt.isAvailable<float>("z0")) ? m_truetrkP[Z0] = truthprt.auxdata<float>("z0") : m_truetrkP[Z0] = -9999.;
  (truthprt.isAvailable<float>("theta")) ? m_truetrkP[THETA] = truthprt.auxdata<float>("theta") : m_truetrkP[THETA] =
                                                                 -9999.;
  (truthprt.isAvailable<float>("phi")) ? m_truetrkP[PHI] = truthprt.auxdata<float>("phi") : m_truetrkP[PHI] = -9999.;
  (truthprt.isAvailable<float>("theta") &&
   truthprt.isAvailable<float>("qOverP")) ? m_truetrkP[QOVERPT] = truthprt.auxdata<float>("qOverP") *
                                                                  (1 /
                                                                   std::sin(truthprt.auxdata<float>("theta"))) :
                                                                  m_truetrkP[QOVERPT] = -9999.;


  m_truetrkP[PT] = truthprt.pt() / Gaudi::Units::GeV;
  (truthprt.isAvailable<float>("z0") &&
   truthprt.isAvailable<float>("theta")) ? m_truetrkP[Z0SIN] = m_truetrkP[Z0] *
                                                               std::sin(m_truetrkP[THETA]) : m_truetrkP[Z0SIN] =
                                                                 -9999.;
}

void
InDetPerfPlot_Resolution::finalizePlots() {
    
  bool saveProjections = true;
  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {
    if(iparam == PT) continue;
    //
    //Only save vert detailed information if... high detail level
    //Reduces output for ART / PhysVal
    //
    if(m_iDetailLevel >= 200){
      makeResolutions(m_resHelpereta[iparam], m_reswidth_vs_eta[iparam], m_resmean_vs_eta[iparam],
                      m_resProjections_vs_eta[iparam], saveProjections);
      makeResolutions(m_resHelperpt[iparam], m_reswidth_vs_pt[iparam], m_resmean_vs_pt[iparam],
                      m_resProjections_vs_pt[iparam], saveProjections);
      makeResolutions(m_pullHelperpt[iparam], m_pullwidth_vs_pt[iparam], m_pullmean_vs_pt[iparam],
                      m_pullProjections_vs_pt[iparam], saveProjections);
      makeResolutions(m_pullHelpereta[iparam], m_pullwidth_vs_eta[iparam], m_pullmean_vs_eta[iparam],
                      m_pullProjections_vs_eta[iparam], saveProjections);
      makeResolutions(m_resHelperpt_pos[iparam], m_reswidth_vs_pt_pos[iparam], m_resmean_vs_pt_pos[iparam]);
      makeResolutions(m_resHelperpt_neg[iparam], m_reswidth_vs_pt_neg[iparam], m_resmean_vs_pt_neg[iparam]);
      makeResolutions(m_resHelpereta_pos[iparam], m_reswidth_vs_eta_pos[iparam], m_resmean_vs_eta_pos[iparam]);
      makeResolutions(m_resHelpereta_neg[iparam], m_reswidth_vs_eta_neg[iparam], m_resmean_vs_eta_neg[iparam]);
    } else {
      makeResolutions(m_resHelpereta[iparam], m_reswidth_vs_eta[iparam], m_resmean_vs_eta[iparam]);
      makeResolutions(m_resHelperpt[iparam], m_reswidth_vs_pt[iparam], m_resmean_vs_pt[iparam]);
      makeResolutions(m_pullHelperpt[iparam], m_pullwidth_vs_pt[iparam], m_pullmean_vs_pt[iparam]);
      makeResolutions(m_pullHelpereta[iparam], m_pullwidth_vs_eta[iparam], m_pullmean_vs_eta[iparam]);
    }

  }

}

void
InDetPerfPlot_Resolution::getMeanWidthResultsModUnits(TH1* p_input_hist, std::vector<float>& p_result,
						  IDPVM::GetMeanWidth::methods p_method) {
  // LM: reason for using this function:
  //   orignial code was set up to only have resolution resuls in um,
  //   while inputs are in mm. Do not want to do a substantial rewrite.
  double mm2um = 1000.;

  if (!p_result.empty()) {
    p_result.clear();
  }
  m_getMeanWidth.setResults(p_input_hist, p_method);
  for (auto it : m_getMeanWidth.getDebugs())
    ATH_MSG_DEBUG(it);
  for (auto it : m_getMeanWidth.getInfos())
    ATH_MSG_INFO(it);
  for (auto it : m_getMeanWidth.getWarnings())
    ATH_MSG_WARNING(it);
  for (auto it : m_getMeanWidth.getErrors())
    ATH_MSG_ERROR(it);
  p_result.push_back(m_getMeanWidth.getRMS());
  p_result.push_back(m_getMeanWidth.getRMSError());
  p_result.push_back(m_getMeanWidth.getMean());
  p_result.push_back(m_getMeanWidth.getMeanError());
  p_result.push_back(m_getMeanWidth.getFracOut());
  p_result.push_back(m_getMeanWidth.getFracOutUnc());

  TString vari = p_input_hist->GetName();
  if ( !vari.Contains("pull") &&
       (vari.Contains("d0") || vari.Contains("z0")) ) {
    // the  mean, meanerror, RMS and RMSerror
    // are conventionally first four elements of results vector
    p_result[0]*= mm2um;
    p_result[1]*= mm2um;
    p_result[2]*= mm2um;
    p_result[3]*= mm2um;
  }
}

void
InDetPerfPlot_Resolution::makeResolutions(TH2* h, TH1* hwidth, TH1* hmean) {

  // Should fix this in a better way
  TString hname = h->GetName();
  // warnings in case input histograms have large % events in under- and over- flow bins 
  std::vector< std::pair<unsigned int,double> > warnUOBinFrac;
   
  if (hname.Contains("Helpereta")) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      // Mapping of eta bins between TH2 and TH1 must be consistent, otherwise resolutions are asymmetric
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), ieta + 1, ieta + 1); //RESOLUTIONS VS ETA
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hwidth->SetBinContent(ieta + 1, result.at(0));
      hwidth->SetBinError(ieta + 1, result.at(1));
      hmean->SetBinContent(ieta + 1, result.at(2));
      hmean->SetBinError(ieta + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.) warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));
      result.clear();
      delete tmp;
    }    
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
    }
  }
  else if (hname.Contains("Helperpt")) {
    if (!warnUOBinFrac.empty())
      warnUOBinFrac.clear();
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), ipt + 1, ipt + 1);
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hwidth->SetBinContent(ipt + 1, result.at(0));
      hwidth->SetBinError(ipt + 1, result.at(1));
      hmean->SetBinContent(ipt + 1, result.at(2));
      hmean->SetBinError(ipt + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.) 
	warnUOBinFrac.push_back(std::make_pair(ipt + 1,result.at(6)));
      result.clear();
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
    }
  }
}

void
InDetPerfPlot_Resolution::makeResolutions(TH2* h, TH1* hwidth, TH1* hmean, TH1* hproj[m_nEtaBins], bool save) {
  TString hname = h->GetName();
  // warnings in case input histograms have large % events in under- and over- flow bins 
  std::vector< std::pair<unsigned int,double> > warnUOBinFrac;
  
  if (hname.Contains("Helpereta")) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_EtaBins[ieta]),
                                         h->GetXaxis()->FindBin(m_EtaBins[ieta + 1]-0.05));
      if (tmp->Integral() < 1) {
        continue;
      }
      if (save) {
        cloneHistogram(tmp, hproj[ieta]);
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hwidth->SetBinContent(ieta + 1, result.at(0));
      hwidth->SetBinError(ieta + 1, result.at(1));
      hmean->SetBinContent(ieta + 1, result.at(2));
      hmean->SetBinError(ieta + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.) 
	warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));      
      result.clear();
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
    }
  } else if (hname.Contains("Helperpt")) {
    if (!warnUOBinFrac.empty())
      warnUOBinFrac.clear();
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                         h->GetXaxis()->FindBin(m_PtBins[ipt + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      if (save) {
        cloneHistogram(tmp, hproj[ipt]);
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hwidth->SetBinContent(ipt + 1, result.at(0));
      hwidth->SetBinError(ipt + 1, result.at(1));
      hmean->SetBinContent(ipt + 1, result.at(2));
      hmean->SetBinError(ipt + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.)
	warnUOBinFrac.push_back(std::make_pair(ipt + 1,result.at(6)));
      result.clear();
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hwidth->GetName(),warnUOBinFrac));
    }
  }
}

void
InDetPerfPlot_Resolution::cloneHistogram(TH1D* h, TH1* hcopy) {
  unsigned int nbin = h->GetNbinsX();

  for (unsigned int ibin = 0; ibin < nbin; ibin++) {
    float binContent = h->GetBinContent(ibin + 1);
    float binError = h->GetBinError(ibin + 1);
    hcopy->SetBinContent(ibin + 1, binContent);
    hcopy->SetBinError(ibin + 1, binError);
  }
}

void
InDetPerfPlot_Resolution::makeResolutions(TH3* h, TH1* hres_eta[4][4], TH1* hres_pt[4][4]) {

  // warnings in case input histograms have large % events in under- and over- flow bins
  std::vector< std::pair<unsigned int,double> > warnUOBinFrac;
  
  float BinEta[5] = {
    0.0, 1.0, 1.5, 2.7, 5.0
  };
  float BinPt[5] = {
    0.0, 1.0, 2.0, 5.0, 500.0
  };

  for (unsigned int ieta = 0; ieta < 4; ieta++) {
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt + 1) +
                            std::string("pz_binEta") + std::to_string(ieta + 1);
      std::string tmpName1 = tmpName + std::string("_neg");
      std::string tmpName2 = tmpName + std::string("_pos");
      TH1D* tmp1 = (TH1D*) h->ProjectionZ(tmpName1.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                          h->GetXaxis()->FindBin(m_PtBins[ipt + 1]),
                                          h->GetYaxis()->FindBin(-BinEta[ieta + 1]),
                                          h->GetYaxis()->FindBin(-BinEta[ieta]));
      TH1D* tmp2 = (TH1D*) h->ProjectionZ(tmpName2.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                          h->GetXaxis()->FindBin(m_PtBins[ipt + 1]),
                                          h->GetYaxis()->FindBin(BinEta[ieta]),
                                          h->GetYaxis()->FindBin(BinEta[ieta + 1]));
      tmp1->Add(tmp2);
      if (tmp1->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp1, result, m_meanWidthMethod);
      hres_eta[ieta][0]->SetBinContent(ipt + 1, result.at(0));
      hres_eta[ieta][0]->SetBinError(ipt + 1, result.at(1));
      hres_eta[ieta][1]->SetBinContent(ipt + 1, result.at(2));
      hres_eta[ieta][1]->SetBinError(ipt + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.)
	warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));
      delete tmp1;
      delete tmp2;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_eta[ieta][0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_eta[ieta][1]->GetName(),warnUOBinFrac));
    }
  }

  for (unsigned int ipt = 0; ipt < 4; ipt++) {
    if (!warnUOBinFrac.empty())
      warnUOBinFrac.clear();
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt + 1) +
                            std::string("pz_binEta") + std::to_string(ieta + 1);
      TH1D* tmp =
        (TH1D*) h->ProjectionZ(tmpName.c_str(), h->GetXaxis()->FindBin(BinPt[ipt]), h->GetXaxis()->FindBin(
                                 BinPt[ipt + 1]), h->GetYaxis()->FindBin(m_EtaBins[ieta]),
                               h->GetYaxis()->FindBin(m_EtaBins[ieta + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result,m_meanWidthMethod);
      hres_pt[ipt][0]->SetBinContent(ieta + 1, result.at(0));
      hres_pt[ipt][0]->SetBinError(ieta + 1, result.at(1));
      hres_pt[ipt][1]->SetBinContent(ieta + 1, result.at(2));
      hres_pt[ipt][1]->SetBinError(ieta + 1, result.at(3));
      if (result.size()>6 && result.at(6)>0.)
	warnUOBinFrac.push_back(std::make_pair(ieta + 1,result.at(6)));
      delete tmp;
    }
    if (!warnUOBinFrac.empty()) {
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_pt[ipt][0]->GetName(),warnUOBinFrac));
      ATH_MSG_WARNING(m_getMeanWidth.reportUOBinVal(hres_pt[ipt][1]->GetName(),warnUOBinFrac));
    }
  }
}

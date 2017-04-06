#ifndef DELTASAGITTAFITTER_C
#define DELTASAGITTAFITTER_C
#include "deltaSagittaFitter.h" 

//////////////////////////////////////////////////////////
deltaSagittaFitter::deltaSagittaFitter() : m_printLevel (0),
					   m_NetaBins (0),
					   m_NphiBins (0),
					   m_NptBins (0),
					   m_fittedDeltaSagitta (0.),
					   m_CurrentStatus (true)
{
  m_outputFileName.Clear();
  m_outputFileName.Append("deltaSagittaFitter.pdf");

  h_PtPosMu_corr = NULL;
  h_PtNegMu_corr = NULL;
  h_PtPosMu_corr_aux = NULL;
  h_PtNegMu_corr_aux = NULL;
  h_Target = NULL;
  h_deltaSagittaMap = NULL;

  m_saveOutputGraphics = true;
  m_usedMethod = deltaSagittaFitter::CHISQUARE;
  //m_usedMethod = deltaSagittaFitter::CUMULATIVE;
  //m_usedMethod = deltaSagittaFitter::PTPEAK;
  m_thisPass = 0; // initialization to no pass done yet

  m_lastDeltaSagitta = -99.;
  m_callCount = 0;
  m_eventCount = 0;

  v_deltaSagitta_testValues.clear();
  v_h3DPtPosMu.clear();
  v_h3DPtNegMu.clear();


  return;
}

//////////////////////////////////////////////////////////
deltaSagittaFitter::~deltaSagittaFitter() 
{
}

//////////////////////////////////////////////////////////
void deltaSagittaFitter::Execute()
{
  if (m_printLevel >= 0) std::cout << " ** deltaSagittaFitter::Execute ** START ** " << std::endl;
  if (m_printLevel >= 0) std::cout << " ** deltaSagittaFitter::Execute ** Number of eta x phi cells = " << m_NetaBins << "x" << m_NphiBins  << std::endl;

  bool outputFileOpen = false;

  if (m_CurrentStatus) {
    // loop on eta-phi bins
    for (int etabin=1; etabin <= m_NetaBins; etabin++) {
      for (int phibin=1; phibin <= m_NphiBins; phibin++) {
	//if (etabin != 1 || phibin !=1) continue;
	if (m_printLevel >= 1) std::cout << std::endl << " ** deltaSagittaFitter ** Execute() ** Projecting (eta,phi) = (" << etabin <<", " << phibin << ") " << std::endl;
	this->FitEtaPhiCell (etabin, phibin);
	if (h_deltaSagittaMap != NULL) {
	  h_deltaSagittaMap->SetBinContent(etabin, phibin, m_fittedDeltaSagitta);
	}
      }
    }
  }
  else {
    // m_currentStaus == false
    std::cout << " ** deltaSagittaFitter::Execute ** WARNING ** no action as current status is BAD " << std::endl; 
  }
  
  if (m_printLevel >= 0) std::cout << " ** deltaSagittaFitter::Execute ** COMPLETED ** " << std::endl;
  return;
}

//////////////////////////////////////////////////////////
void deltaSagittaFitter::SetInputHistograms (TH3 * newth3pos, TH3 * newth3neg) 
{
  m_eventCount = 0;
  h_3DPtPosMu = newth3pos;
  h_3DPtNegMu = newth3neg;

  // make sure both histograms have the same number of bins
  if (h_3DPtPosMu != NULL && h_3DPtNegMu != NULL) {
    if (h_3DPtPosMu->GetXaxis()->GetNbins() == h_3DPtNegMu->GetXaxis()->GetNbins()) m_NetaBins = h_3DPtPosMu->GetXaxis()->GetNbins();
    if (h_3DPtPosMu->GetYaxis()->GetNbins() == h_3DPtNegMu->GetYaxis()->GetNbins()) m_NphiBins = h_3DPtPosMu->GetYaxis()->GetNbins();
    if (h_3DPtPosMu->GetZaxis()->GetNbins() == h_3DPtNegMu->GetZaxis()->GetNbins()) m_NptBins  = h_3DPtPosMu->GetZaxis()->GetNbins();
  }

  // if some failed --> bins == 0 --> flag status as bad
  if (m_NetaBins == 0 || m_NphiBins == 0 || m_NptBins == 0) {
    m_CurrentStatus = false;
  }

  if (m_CurrentStatus && h_deltaSagittaMap == NULL && false) {
    // build TH2 to store the deltaSagitta map
    h_deltaSagittaMap = new TH2F ("h_deltaSagittaMap", " #delta_{sagitta} map ", 
				  m_NetaBins, h_3DPtPosMu->GetXaxis()->GetXmin(), h_3DPtPosMu->GetXaxis()->GetXmax(),
				  m_NphiBins, h_3DPtPosMu->GetYaxis()->GetXmin(), h_3DPtPosMu->GetYaxis()->GetXmax());
    h_deltaSagittaMap->SetXTitle(h_3DPtPosMu->GetXaxis()->GetTitle());
    h_deltaSagittaMap->SetYTitle(h_3DPtPosMu->GetYaxis()->GetTitle());
    h_deltaSagittaMap->SetZTitle("#delta_{sagitta} [TeV^{-1}]");
    h_deltaSagittaMap->SetStats(kFALSE);
  }

  return;
}

//////////////////////////////////////////////////////////
void deltaSagittaFitter::Initialization (TH3 * sampleHist) 
{
  if (m_printLevel >= 0) std::cout << " ** deltaSagittaFitter::Initialization ** START **  Pass = " << m_thisPass << std::endl;
  
  this->PrepareIteration (0); // fill the deltaSagitta test values
  if (m_thisPass == 1) {
    m_CurrentStatus = true; // start flagging status as true
    
    //if (h_deltaSagittaMap != NULL ) h_deltaSagittaMap = NULL;
    

    if (sampleHist != NULL && m_thisPass == 1) {
      // vector with test delta sagitta values
      v_h3DPtPosMu.clear();
      v_h3DPtNegMu.clear();
      // fill values
      
      // now create many instances of the TH3 for positive and negative muon pt vs eta-phi
      TString hname;
      TString htitle;
      for (int i=0; i < (int) v_deltaSagitta_testValues.size(); i++) {
	// histogram for mu+
	hname.Clear();
	hname.Append("h_3DPtPosMu_");
	hname += i;
	htitle.Clear();
	htitle.Form("Test pT of #mu^{+} for #delta_{saggita} = %1.3f (%d)", v_deltaSagitta_testValues.at(i), i+1);
	TH3F* newth3ptpos = new TH3F (hname.Data(), htitle.Data(),
				      sampleHist->GetXaxis()->GetNbins(), sampleHist->GetXaxis()->GetXmin(), sampleHist->GetXaxis()->GetXmax(), 
				      sampleHist->GetYaxis()->GetNbins(), sampleHist->GetYaxis()->GetXmin(), sampleHist->GetYaxis()->GetXmax(), 
				      sampleHist->GetZaxis()->GetNbins(), sampleHist->GetZaxis()->GetXmin(), sampleHist->GetZaxis()->GetXmax());
	newth3ptpos->GetXaxis()->SetTitle(sampleHist->GetXaxis()->GetTitle());
	newth3ptpos->GetYaxis()->SetTitle(sampleHist->GetYaxis()->GetTitle());
	newth3ptpos->GetZaxis()->SetTitle(sampleHist->GetZaxis()->GetTitle());
	
	// histogram for mu-
	hname.Clear();
	hname.Append("h_3DPtNegMu_");
	hname += i;
	htitle.Clear();
	htitle.Form("Test pT of #mu^{-} for #delta_{saggita} = %1.3f (%d)", v_deltaSagitta_testValues.at(i), i+1);
	TH3F* newth3ptneg = new TH3F (hname.Data(), htitle.Data(),
				      sampleHist->GetXaxis()->GetNbins(), sampleHist->GetXaxis()->GetXmin(), sampleHist->GetXaxis()->GetXmax(), 
				      sampleHist->GetYaxis()->GetNbins(), sampleHist->GetYaxis()->GetXmin(), sampleHist->GetYaxis()->GetXmax(), 
				      sampleHist->GetZaxis()->GetNbins(), sampleHist->GetZaxis()->GetXmin(), sampleHist->GetZaxis()->GetXmax());
	newth3ptneg->GetXaxis()->SetTitle(sampleHist->GetXaxis()->GetTitle());
	newth3ptneg->GetYaxis()->SetTitle(sampleHist->GetYaxis()->GetTitle());
	newth3ptneg->GetZaxis()->SetTitle(sampleHist->GetZaxis()->GetTitle());
	
	// add the histograms to the vector list
	v_h3DPtPosMu.push_back(newth3ptpos);
	v_h3DPtNegMu.push_back(newth3ptneg);
	
	//
	if (m_printLevel >= 3) {
	  std::cout << "   -- mu+ -- histo  ** " << v_h3DPtPosMu.back() << " name: " << v_h3DPtPosMu.back()->GetName() 
		    << " with bins: " << v_h3DPtPosMu.back()->GetNbinsX() 
		    << "x" << v_h3DPtPosMu.back()->GetNbinsY() 
		    << "x" << v_h3DPtPosMu.back()->GetNbinsZ()
		    << std::endl;
	  std::cout << "   -- mu- -- histo  ** " << v_h3DPtNegMu.back() << v_h3DPtNegMu.back()->GetName() 
		    << " with bins: " << v_h3DPtNegMu.back()->GetNbinsX() 
		    << "x" << v_h3DPtNegMu.back()->GetNbinsY() 
		    << "x" << v_h3DPtNegMu.back()->GetNbinsZ()
		    << std::endl;
	}
      }
      
      // 
      m_NetaBins = sampleHist->GetXaxis()->GetNbins();
      m_NphiBins = sampleHist->GetYaxis()->GetNbins();
      m_NptBins = sampleHist->GetZaxis()->GetNbins();
      if (m_printLevel >= 0) {
	std::cout << " ** deltaSagittaFitter::Initialization ** m_NetaBins " << m_NetaBins << std::endl
		  << "                                          m_NPhiBins " << m_NphiBins << std::endl
		  << "                                          m_NptBins " << m_NptBins << std::endl;
      } 
    }
    else {
      // passed histogram is faulty
      m_CurrentStatus = false;
    }
  
    // if some failed --> bins == 0 --> flag status as bad
    if (m_NetaBins == 0 || m_NphiBins == 0 || m_NptBins == 0) {
      m_CurrentStatus = false;
    }
    
    if (m_CurrentStatus) {
      if (h_deltaSagittaMap == NULL) {
	// build TH2 to store the deltaSagitta map
	h_deltaSagittaMap = new TH2F ("h_deltaSagittaMap", " #delta_{sagitta} map ", 
				      m_NetaBins, sampleHist->GetXaxis()->GetXmin(), sampleHist->GetXaxis()->GetXmax(),
				      m_NphiBins, sampleHist->GetYaxis()->GetXmin(), sampleHist->GetYaxis()->GetXmax());
	h_deltaSagittaMap->SetXTitle(sampleHist->GetXaxis()->GetTitle());
	h_deltaSagittaMap->SetYTitle(sampleHist->GetYaxis()->GetTitle());
	h_deltaSagittaMap->SetZTitle("#delta_{sagitta} [TeV^{-1}]");
	h_deltaSagittaMap->SetStats(kFALSE);
      }
      else {
	std::cout << " ** deltaSagittaFitter::Initialization ** WARNING ** sagitta map TH2 already existing " << std::endl;
      }
      if (m_printLevel >=0 ) {
	std::cout << " -- h_deltaSagittaMap -- TH2 defined as: " << h_deltaSagittaMap->GetTitle() 
		  << " with bins: " << h_deltaSagittaMap->GetXaxis()->GetNbins() << "x" << h_deltaSagittaMap->GetYaxis()->GetNbins()
		  << std::endl;
      }
    }
    else {
      std::cout << " ** deltaSagittaFitter::Initialization ** WARNING ** due to current status being BAD the sagitta map TH2 was not built " << std::endl;
    }

    if (m_printLevel >= 3  && m_CurrentStatus) {
      std::cout << " ** deltaSagittaFitter::Initialization ** List of test values and histograms ** " << std::endl;
      for (int i=0; i < (int) v_deltaSagitta_testValues.size(); i++) {
	std::cout << "   i: " << i << "  ds: " << v_deltaSagitta_testValues.at(i) 
		  << " histo: " <<  v_h3DPtPosMu.at(i)
		  << "  hname: " << v_h3DPtPosMu.at(i)->GetName() 
		  << " binning: " << v_h3DPtPosMu.at(i)->GetXaxis()->GetNbins() << "x" << v_h3DPtPosMu.at(i)->GetYaxis()->GetNbins() << "x" << v_h3DPtPosMu.at(i)->GetZaxis()->GetNbins()
		  << "  hposmu: " << v_h3DPtPosMu.at(i)->GetTitle() << std::endl;
      }
    } 
  }

  if (m_printLevel >= 0) std::cout << " ** deltaSagittaFitter::Initialization ** COMPLETED **  Pass: "<< m_thisPass << std::endl;
  return;
}

//////////////////////////////////////////////////////////
void deltaSagittaFitter::PrepareIteration (int newiter) 
{
  m_thisPass++;
  if (m_printLevel >= 0) std::cout << " ** deltaSagittaFitter::PrepareIteration ** START **  newiter= " << newiter << "  Pass = " << m_thisPass << std::endl;

  v_deltaSagitta_testValues.clear();
  if (m_thisPass == 0 || m_thisPass == 1) { 
    double testValues[9] = {-4.0, -3.0, -2.0, -1.0, 0., 1.0, 2.0, 3.0, 4.0};
    //double testValues[9] = {-3.0, -2.0, -1.0, -0.5, 0., 0.5, 1.0, 2.0, 3.0};
    if (m_usedMethod == deltaSagittaFitter::CHISQUARE || m_usedMethod == deltaSagittaFitter::PTPEAK) {
      //double testValues[9] = {-4.0, -3.0, -2.0, -1.0, 0., 1.0, 2.0, 3.0, 4.0};
      // this method needs to scan on a wider delta sagitta range
      testValues[0] = -4.0;
      testValues[1] = -3.0;
      testValues[2] = -2.0;
      testValues[3] = -1.0;
      testValues[4] =  0.0;
      testValues[5] =  1.0;
      testValues[6] =  2.0;
      testValues[7] =  3.0;
      testValues[8] =  4.0;
    }
    for (int i=0; i < 9; i++) {
      v_deltaSagitta_testValues.push_back(testValues[i]);
    }
  }
  if (m_thisPass == 2) {
    double testValues[9] = {-0.500, -0.300, -0.150, -0.050, 0., 0.050, 0.150, 0.300, 0.500};
    //double testValues[9] = {-0.800, -0.500, -0.250, -0.100, 0., 0.100, 0.250, 0.500, 0.800};
    if (m_usedMethod == deltaSagittaFitter::PTPEAK) {
      // this method needs to scan on a wider delta sagitta range
      //double testValues[9] = {-0.800, -0.500, -0.250, -0.100, 0., 0.100, 0.250, 0.500, 0.800};
      testValues[0] = -0.800;
      testValues[1] = -0.500;
      testValues[2] = -0.250;
      testValues[3] = -0.100;
      testValues[4] =  0.000;
      testValues[5] =  0.100;
      testValues[6] =  0.250;
      testValues[7] =  0.500;
      testValues[8] =  0.800;
    }
    if (m_usedMethod == deltaSagittaFitter::CHISQUARE) {
      // this method needs to scan on a wider delta sagitta range
      //double testValues[9] = {-1.000, -0.800, -0.500, -0.200, 0., 0.200, 0.500, 0.800, 1.800};
      testValues[0] = -1.000;
      testValues[1] = -0.800;
      testValues[2] = -0.500;
      testValues[3] = -0.200;
      testValues[4] =  0.000;
      testValues[5] =  0.200;
      testValues[6] =  0.500;
      testValues[7] =  0.800;
      testValues[8] =  1.000;
    }
    for (int i=0; i < 9; i++) {
      v_deltaSagitta_testValues.push_back(testValues[i]);
    }
  }
  if (m_printLevel >= 0) {
    std::cout << " ** deltaSagittaFitter::PrepareIteration ** deltaSagitta test values: ";
    for (int i=0; i < (int) v_deltaSagitta_testValues.size(); i++) {
      std::cout << v_deltaSagitta_testValues.at(i) << "   ";
    }
    std::cout << endl;
  }

  if (m_thisPass ==2) {
    // reset TH3 histograms
    for (int i=0; i < (int) v_h3DPtPosMu.size(); i++) {
      v_h3DPtPosMu.at(i)->Reset();
      v_h3DPtNegMu.at(i)->Reset();
    }
  }

  if (m_printLevel >= 0) std::cout << " ** deltaSagittaFitter::PrepareIteration ** COMPLETED **  newiter= " << newiter << "  Pass = " << m_thisPass << std::endl;
    
}

//////////////////////////////////////////////////////////
 TH1* deltaSagittaFitter::ProjectPt (TH3* inputHisto, int etabin, int phibin, int charge, TString theOptions)
 { 
  if (m_printLevel >= 3) {
    std::cout << " ** deltaSagittaFitter::ProjectPt ** START ** " 
	      << " histogram: " << inputHisto->GetTitle()
	      << " (" << inputHisto->GetEntries() << " entries)"
	      << " etabin=" << etabin << "  phibin: " << phibin << "  charge: " << charge << std::endl;
  }

  TH1* h_projectz = NULL;

  if (charge > 1) charge = deltaSagittaFitter::QPOS;
  if (charge == 0) charge = deltaSagittaFitter::QPOS;
  if (charge <-1) charge = deltaSagittaFitter::QNEG;

  TString hname; hname.Clear();
  if (charge == deltaSagittaFitter::QPOS) {
    hname.Append(inputHisto->GetName()); hname.Append("_"); hname += etabin; hname.Append("_"); hname += phibin;
    h_projectz = inputHisto->ProjectionZ(hname.Data(), etabin, etabin, phibin, phibin, theOptions.Data()); // --> for errors,"e");
    h_projectz->SetLineColor(kBlue+1);
  }
  if (charge == deltaSagittaFitter::QNEG) {
    hname.Append(inputHisto->GetName()); hname.Append("_"); hname += etabin; hname.Append("_"); hname += phibin;
    h_projectz = inputHisto->ProjectionZ(hname.Data(), etabin, etabin, phibin, phibin, theOptions.Data()); // --> for errors,"e");
    h_projectz->SetLineColor(kRed+1);
  }

  TString htitle("p_{T} of the #mu  (#eta= ");
  TString etapart("all");
  if (etabin != 0) etapart.Form("%3.2f", h_3DPtPosMu->GetXaxis()->GetBinCenter(etabin));
  htitle.Append(etapart.Data());
  htitle.Append(",");
  TString phipart(" #phi = all)");
  if (phibin != 0) phipart.Form(" #phi = %3.2f)", h_3DPtPosMu->GetYaxis()->GetBinCenter(phibin));
  htitle.Append(phipart.Data());
  
  h_projectz->SetTitle(htitle.Data());
  h_projectz->SetStats(kFALSE);
  h_projectz->SetLineWidth(2);

  if (m_printLevel >= 5) {
    std::cout << " -- Input histogram " << inputHisto->GetName() << "  entries: " << inputHisto->GetEntries() << std::endl;
    std::cout << " -- projected histogram contents. " << std::endl;
    for (int bin=1; bin <= h_projectz->GetNbinsX(); bin++) {
      std::cout << "   bin: " << bin << "  pt: " << h_projectz->GetXaxis()->GetBinCenter(bin) << "   N entries: " << h_projectz->GetBinContent(bin) << std::endl;
    }
  }
  if (m_printLevel >= 3) {
    std::cout << " ** deltaSagittaFitter::ProjectPt ** COMPLETED ** etabin=" << etabin << "  phibin: " << phibin << "  charge: " << charge << std::endl;
  }

  return h_projectz; 
}

//////////////////////////////////////////////////////////
void deltaSagittaFitter::BuildHTarget ()
{ 
  if (m_printLevel >= 3) std::cout << " ** deltaSagittaFitter::BuildHTarget ** START " << std::endl;

  if (h_PtPosMu != NULL) {
    h_Target = (TH1*) h_PtPosMu->Clone();
    h_Target->SetTitle("Target: N_{#mu^{+}} - N_{#mu^{-}}  Vs p_{T}");
    h_Target->SetYTitle("N_{#mu^{+}} - N_{#mu^{-}}");
    h_Target->SetLineColor(kBlack);

    // set the target value
    float targetValue = 0.;
    
    // fill the target histogram with the target value and the error according to the stats
    for (int bin=1; bin <= m_NptBins; bin++) {
      h_Target->SetBinContent (bin, targetValue);
      h_Target->SetBinError (bin, sqrt( h_PtPosMu->GetBinContent(bin)+ h_PtNegMu->GetBinContent(bin)));
      if (m_printLevel >= 5) {
	std::cout << "  - h_Target - bin: " << bin 
		  << "  pt: " << h_Target->GetBinCenter (bin) 
		  << "  val: " << h_Target->GetBinContent (bin) 
		  << " +- " << h_Target->GetBinError (bin) 
		  << std::endl;
      }
    }
  } 
  else {
    // histogram is not ready
    m_CurrentStatus = false;
  }

  if (m_printLevel >= 3) std::cout << " ** deltaSagittaFitter::BuildHTarget ** COMPLETED " << std::endl;
  return;
}

//////////////////////////////////////////////////////////
TH1* deltaSagittaFitter::ComputeDifference ()
{ 
  if (m_printLevel >= 3) { 
    std::cout << " ** deltaSagittaFitter::ComputeDifference ** START ** " << std::endl
	      << "      - mu+ histo: " << h_PtPosMu->GetTitle() << std::endl
	      << "      - mu- histo: " << h_PtNegMu->GetTitle()
	      << std::endl;
  }
  
  TH1* h_difference = NULL;
  h_difference = (TH1*) h_PtPosMu->Clone();
  h_difference->Add(h_PtNegMu, -1);
  h_difference->SetTitle(""); //h_ratio->SetTitle("N_{#mu^{+}} - N_{#mu^{-}  Vs p_{T}}");
  h_difference->SetYTitle("N_{#mu^{+}} - N_{#mu^{-}}");
  h_difference->SetLineColor(kBlack);

  if (m_printLevel >= 5) {
    for (int bin=1; bin <= m_NptBins; bin++) {
      std::cout << "   - diff - bin: " << bin 
		<< "   pt: " << h_difference->GetBinCenter(bin)
		<< "   Nmu+: " << h_PtPosMu->GetBinContent(bin)
		<< "   Nmu-: " << h_PtNegMu->GetBinContent(bin)
		<< "   diff: " << h_difference->GetBinContent(bin)
		<< std::endl;
    }
  }

  if (m_printLevel >= 3) std::cout << " ** deltaSagittaFitter::ComputeDifference ** COMPLETED ** " << std::endl;
  return h_difference;
}

//////////////////////////////////////////////////////////
TH1* deltaSagittaFitter::ComputeDifference2 ()
{ 
  TH1* h_difference = NULL;
  h_difference = (TH1*) h_PtPosMu_corr->Clone();
  h_difference->Add(h_PtNegMu_corr, -1);
  h_difference->SetTitle(""); //h_ratio->SetTitle("N_{#mu^{+}} - N_{#mu^{-}  Vs p_{T}}");
  h_difference->SetYTitle("N_{#mu^{+}} - N_{#mu^{-}}");
  h_difference->SetLineColor(kBlack);

  if (m_printLevel >= 3) {
    for (int bin=1; bin <= m_NptBins; bin++) {
      std::cout << "   - diff - bin: " << bin 
		<< "   pt: " << h_difference->GetBinCenter(bin)
		<< "   Nmu+: " << h_PtPosMu_corr->GetBinContent(bin)
		<< "   Nmu-: " << h_PtNegMu_corr->GetBinContent(bin)
		<< "   diff: " << h_difference->GetBinContent(bin)
		<< std::endl;
    }
  }

  return h_difference;
}

//////////////////////////////////////////////////////////
TH1* deltaSagittaFitter::ComputeRatio (TH1 * hNumer, TH1* hDenom)
{ 
  TH1* hRatio = NULL;
  hRatio = (TH1*) hNumer->Clone();
  hRatio->Divide(hDenom);
  hRatio->SetTitle(""); //h_ratio->SetTitle("N_{#mu^{+}} - N_{#mu^{-}  Vs p_{T}}");
  hRatio->SetYTitle("N_{#mu^{+}} - N_{#mu^{-}}");
  hRatio->SetLineColor(kBlack);

  if (m_printLevel >= 5) {
    for (int bin=1; bin <= m_NptBins; bin++) {
      std::cout << "   - ratio - bin: " << bin 
		<< "   pt: " << hRatio->GetBinCenter(bin)
		<< "   Nmu+: " << hNumer->GetBinContent(bin)
		<< "   Nmu-: " << hDenom->GetBinContent(bin)
		<< "   ratio: " << hRatio->GetBinContent(bin)
		<< std::endl;
    }
  }

  
  float theMax = hRatio->GetMaximum();
  float theMin = 999999.; 
  for (int i=1; i <= hRatio->GetNbinsX(); i++) {
    if (hRatio->GetBinContent(i) > 0 && hRatio->GetBinContent(i) < theMin) theMin = hRatio->GetBinContent(i);  
  }


  float delta_plus = theMax - 1;
  float delta_minus = 1 - theMin;

  float good_delta = std::max(delta_plus, delta_minus);
  if (good_delta > 0.5) good_delta = 0.5;

  hRatio->SetMaximum(1+good_delta);
  hRatio->SetMinimum(1-good_delta);
  hRatio->SetStats(kFALSE);

  hRatio->GetXaxis()->SetLabelSize(0.08);
  hRatio->GetYaxis()->SetLabelSize(0.08);

  hRatio->GetXaxis()->SetTitleSize(0.08);
  hRatio->GetYaxis()->SetTitleOffset(0.50);
  hRatio->GetYaxis()->SetTitleSize(0.08);
  hRatio->GetXaxis()->SetTickSize(0.06);
 
  return hRatio;
}

/////////////////////////////////////////////
double deltaSagittaFitter::DiffVsDeltaSaggitaFunction(Double_t *v, Double_t *par)
{
  
  double theValue = 0;
  double thisDeltaSagitta = par[0];
  double thisPtVal = v[0];

  return theValue;
}

/////////////////////////////////////////////
void deltaSagittaFitter::Fill (float pxpos, float pypos, float pzpos, float mpos, float pxneg, float pyneg, float pzneg, float mneg)
{
  m_eventCount++;

  TLorentzVector mupos;
  TLorentzVector muneg;
  // Warning ZmumuValidationExample uses MeV as units 

  mupos.SetXYZM(pxpos, pypos, pzpos, mpos);
  muneg.SetXYZM(pxneg, pyneg, pzneg, mneg);

  if (m_printLevel >= 10 || m_eventCount == 1) {
    std::cout << " - deltaSagittaFitter - input data for event " << m_eventCount << " as follows" << std::endl
	      << "   mu+ (Pxyz,M) " << mupos.Px() << " " << mupos.Py() << " " << mupos.Pz() << " " << mupos.M() 
	      << "  Pt: " << mupos.Pt() << " Eta: " << mupos.Eta() << " Phi: " << mupos.Phi() << std::endl 
	      << "   mu- (Pxyz,M) " << muneg.Px() << " " << muneg.Py() << " " << muneg.Pz() << " " << muneg.M() 
	      << "  Pt: " << muneg.Pt() << " Eta: " << muneg.Eta() << " Phi: " << muneg.Phi() << std::endl; 
  }


  float scaleFactor = 0.000001; // TeV^{-1} to MeV^{-1} 
  // now loop over all deltaSagitta test values
  for (int i=0; i < (int) v_deltaSagitta_testValues.size(); i++) {
    float thisdeltasagitta = 0.;
    float centralDeltaSagitta = 0.;

    if (m_thisPass == 2) {
      // take as central value the deltaSagitta from the first pass
      if (h_deltaSagittaMap != NULL) {
	  centralDeltaSagitta = h_deltaSagittaMap->GetBinContent(h_deltaSagittaMap->GetXaxis()->FindBin(mupos.Eta()),
								 h_deltaSagittaMap->GetYaxis()->FindBin(mupos.Phi()));
      }
    }
    thisdeltasagitta = centralDeltaSagitta + v_deltaSagitta_testValues.at(i);

    float thisPt;
    float newPt; 
    int charge;
    // mu+
    charge = 1;
    thisPt = mupos.Pt();
    newPt = thisPt / (1 + charge * thisdeltasagitta * scaleFactor * thisPt);
    v_h3DPtPosMu.at(i)->Fill(mupos.Eta(), mupos.Phi(), newPt/1000); // histograms filled in GeV
    if (m_printLevel >= 5) {
      std::cout << "    -- mu+ -- ds: " << thisdeltasagitta << "  pt: " << thisPt << " -> " << newPt 
		<< "   filling histo: " << v_h3DPtPosMu.at(i)->GetName()
		<< "  (eta,phi): (" << mupos.Eta() <<", " << mupos.Phi() << ")"
		<< "  entries: " << v_h3DPtPosMu.at(i)->GetEntries() << std::endl
		<< "   cell: " <<  v_h3DPtPosMu.at(i)->GetXaxis()->FindBin(mupos.Eta()) << " - " << v_h3DPtPosMu.at(i)->GetYaxis()->FindBin(mupos.Phi())
		<< " of " <<  v_h3DPtPosMu.at(i)->GetXaxis()->GetNbins() << "x" <<v_h3DPtPosMu.at(i)->GetYaxis()->GetNbins() << "x" <<v_h3DPtPosMu.at(i)->GetZaxis()->GetNbins() 
		<< std::endl;  
    }

    // mu-
    if (m_thisPass == 2) {
      // take as central value the deltaSagitta from the first pass
      if (h_deltaSagittaMap != NULL) {
	  centralDeltaSagitta = h_deltaSagittaMap->GetBinContent(h_deltaSagittaMap->GetXaxis()->FindBin(muneg.Eta()),
								 h_deltaSagittaMap->GetYaxis()->FindBin(muneg.Phi()));
      }
    }
    thisdeltasagitta = centralDeltaSagitta + v_deltaSagitta_testValues.at(i);

    charge = -1;
    thisPt = muneg.Pt();
    newPt = thisPt / (1 + charge * thisdeltasagitta * scaleFactor * thisPt);
    v_h3DPtNegMu.at(i)->Fill(muneg.Eta(), muneg.Phi(), newPt/1000); // histograms filled in GeV
    if (m_printLevel >= 5) {
      std::cout << "    -- mu- -- ds: " << thisdeltasagitta << "  pt: " << thisPt << " -> " << newPt
		<< "   filling histo: " << v_h3DPtNegMu.at(i)->GetName()
		<< "  (eta,phi): (" << muneg.Eta() <<", " << muneg.Phi() << ")"
		<< "  entries: " << v_h3DPtNegMu.at(i)->GetEntries() << std::endl;
    }
    if (m_printLevel >=  5) {
      std::cout << "    cell: " <<  v_h3DPtPosMu.at(i)->GetXaxis()->FindBin(muneg.Eta()) << " - " << v_h3DPtPosMu.at(i)->GetYaxis()->FindBin(muneg.Phi())
		<< " of " <<  v_h3DPtNegMu.at(i)->GetXaxis()->GetNbins() << "x" <<v_h3DPtNegMu.at(i)->GetYaxis()->GetNbins() << "x" <<v_h3DPtNegMu.at(i)->GetZaxis()->GetNbins() 
		<< std::endl;  
    }
  }

  return;
}


/////////////////////////////////////////////
void deltaSagittaFitter::FitEtaPhiCell (int etabin, int phibin)
{ 
  
  if (m_printLevel >= 3) {
    std::cout << " ** deltaSagittaFitter::FitEtaPhiCell ** START **    eta: " << etabin << " phi: " << phibin << std::endl;
    std::cout << "                                         method: " <<   m_usedMethod << std::endl;
  }
  TGraphErrors * theChi2Points = new TGraphErrors ();
  TH1* h_PtPosMu_cum = NULL; 
  TH1* h_PtNegMu_cum = NULL; 

  TCanvas tempCanvas("tempcanvas","tempcanvas",900,600);
  TString canName;
  tempCanvas.Divide(2,1);

  TLatex plotText; plotText.SetTextColor(kGray+2);
  TString theText;

  float thisdeltasagitta = 0.;
  float centralDeltaSagitta = 0.;
  if (m_thisPass == 2) {
    // take as central value the deltaSagitta from the first pass
    if (h_deltaSagittaMap != NULL) centralDeltaSagitta = h_deltaSagittaMap->GetBinContent(etabin, phibin);
  }
  
  // loop on v_deltaSagitta_testValues
  for (int i=0; i < (int) v_deltaSagitta_testValues.size(); i++) {
    thisdeltasagitta = centralDeltaSagitta + v_deltaSagitta_testValues.at(i);

    if (m_printLevel >= 5) std::cout << " ** deltaSagittaFitter::FitEtaPhiCell ** eta: " << etabin << " phi: " << phibin 
				     << "  ds point: " << i+1 << "  ds= " << thisdeltasagitta
				     << std::endl;
    // first extract the mu+ and mu- pt distributions for this cell and this deltaSagitta test value
    h_PtPosMu = this->ProjectPt (v_h3DPtPosMu.at(i), etabin, phibin, deltaSagittaFitter::QPOS);
    h_PtNegMu = this->ProjectPt (v_h3DPtNegMu.at(i), etabin, phibin, deltaSagittaFitter::QNEG);

    // check number of entries and normalize to the one with maximum number of entries:
    double normEntries = max(h_PtPosMu->GetEntries(), h_PtNegMu->GetEntries());
    if (h_PtPosMu->GetEntries() < normEntries) {
      int ibin =  h_PtPosMu->GetMaximumBin();
      if (m_printLevel >= 10) std::cout << " ** deltaSagittaFitter::FitEtaPhiCell ** Normalizing h_PtPosMu from: " << h_PtPosMu->GetEntries() << " --> " << normEntries << std::endl; 
      h_PtPosMu->Scale(normEntries/h_PtPosMu->GetEntries());
    }
    if (h_PtNegMu->GetEntries() < normEntries) {
      if (m_printLevel >= 10) std::cout << " ** deltaSagittaFitter::FitEtaPhiCell ** Normalizing h_PtNegMu from: " << h_PtNegMu->GetEntries() << " --> " << normEntries << std::endl; 
      h_PtNegMu->Scale(normEntries/h_PtNegMu->GetEntries()); 
    }

    if (m_usedMethod == deltaSagittaFitter::CUMULATIVE) {
      h_PtPosMu_cum = h_PtPosMu->GetCumulative();
      h_PtNegMu_cum = h_PtNegMu->GetCumulative();
    }

    h_PosMinusNeg = this->ComputeDifference ();
    this->BuildHTarget ();
    
    float testValue = 0;
    float testValueError = 0;
    if (m_usedMethod == deltaSagittaFitter::CHISQUARE) {
      float theChi2 = 0.;
      for (int bin=1; bin <= m_NptBins; bin++) {
	if (h_Target->GetBinError(bin) > 1) { // to be sure there are some entries
	  theChi2 += pow((h_PosMinusNeg->GetBinContent(bin) - h_Target->GetBinContent(bin))/ h_Target->GetBinError(bin), 2);
	}
	if (m_printLevel >= 3) {
	  std::cout << "   - ds: " << thisdeltasagitta << "  bin: " << bin << "  pt: " << h_PosMinusNeg->GetBinCenter(bin) 
		    << "   Delta: " << h_PosMinusNeg->GetBinContent(bin)  << " - " <<   h_Target->GetBinContent(bin) 
		    << "   SumChi2: " << theChi2
		    << std::endl;
	}
      }
      testValue = theChi2;
      testValueError = 0.03*(theChi2 + m_NptBins);
    }

    if (m_usedMethod == deltaSagittaFitter::CUMULATIVE) {
      float theMaxDiff = -999.;
      int binMax = 0;
      theMaxDiff = 0;
      for (int bin=1; bin <= m_NptBins; bin++) {
	if (h_Target->GetBinError(bin) > 1) { // to be sure there are some entries
	  if (fabs(h_PosMinusNeg->GetBinContent(bin)) > theMaxDiff) {
	    theMaxDiff = fabs(h_PosMinusNeg->GetBinContent(bin) - h_Target->GetBinContent(bin));
	    binMax = bin;
	  }
	}
	if (m_printLevel >= 5) {
	  std::cout << "   - ds: " << thisdeltasagitta << "  bin: " << bin << "  pt: " << h_PosMinusNeg->GetBinCenter(bin) 
		    << "   Delta: " << h_PosMinusNeg->GetBinContent(bin)  << " - " <<   h_Target->GetBinContent(bin) 
		    << "   Cumulative " << fabs(h_PosMinusNeg->GetBinContent(bin) - h_Target->GetBinContent(bin)) 
		    << "   max: " << theMaxDiff
		    << std::endl;
	}
      }
      testValue = theMaxDiff;
      //testValueError = sqrt(h_PtPosMu_cum->GetBinContent(binMax)+h_PtNegMu_cum->GetBinContent(binMax));
      testValueError = sqrt(theMaxDiff);
    }


    if (m_usedMethod == deltaSagittaFitter::PTPEAK) {
      if (m_printLevel >=10) std::cout << "  ** deltaSagittaFitter::FitEtaPhiCell ** start PTPEAK method " << std::endl;
      // fit the peak to a Gaussian an compare where the distributions peak
      // first pass: fit the entire distribution: 
      h_PtPosMu->Fit("gaus","Q");
      double widthRange = fabs(h_PtPosMu->GetFunction("gaus")->GetParameter(2));
      if (widthRange < 10.) widthRange = 10.;
      int upperBin = h_PtPosMu->FindBin(h_PtPosMu->GetFunction("gaus")->GetParameter(1) + 0.8*widthRange);
      int lowerBin = h_PtPosMu->FindBin(h_PtPosMu->GetFunction("gaus")->GetParameter(1) - 1.2*widthRange);
      // make sure that minimum must be 5 bins to fit
      if (upperBin-lowerBin < 5) {
	upperBin++;
	lowerBin--;
      }

      if (m_printLevel >=10) std::cout << "  ** deltaSagittaFitter::FitEtaPhiCell ** fit range for mu+ pt: " 
				       << h_PtPosMu->GetBinCenter(lowerBin) << " (" << lowerBin << ")  --> " 
				       << h_PtPosMu->GetBinCenter(upperBin) << " (" << upperBin << ") " 
				       << std::endl;
      
      h_PtPosMu->Fit("gaus","Q","", h_PtPosMu->GetBinCenter(lowerBin), h_PtPosMu->GetBinCenter(upperBin));
      h_PtPosMu->GetFunction("gaus")->SetLineColor(h_PtPosMu->GetLineColor());

      // mu-
      h_PtNegMu->Fit("gaus","Q");
      widthRange = 1.1*fabs(h_PtNegMu->GetFunction("gaus")->GetParameter(2));
      if (widthRange < 10.) widthRange = 10.;
      upperBin = h_PtPosMu->FindBin(h_PtNegMu->GetFunction("gaus")->GetParameter(1) + 0.8*widthRange);
      lowerBin = h_PtPosMu->FindBin(h_PtNegMu->GetFunction("gaus")->GetParameter(1) - 1.2*widthRange);
      // make sure that minimum must be 5 bins to fit
      if (upperBin-lowerBin < 5) {
	upperBin++;
	lowerBin--;
      }

      h_PtNegMu->Fit("gaus","Q","", h_PtNegMu->GetBinCenter(lowerBin), h_PtNegMu->GetBinCenter(upperBin));       
      h_PtNegMu->GetFunction("gaus")->SetLineColor(h_PtNegMu->GetLineColor());

      // check the common area in +-1 sigma
      double lowerLimit = max(h_PtPosMu->GetFunction("gaus")->GetParameter(1) - 1.2*h_PtPosMu->GetFunction("gaus")->GetParameter(2), 
			      h_PtNegMu->GetFunction("gaus")->GetParameter(1) - 1.2*h_PtNegMu->GetFunction("gaus")->GetParameter(2));
      double upperLimit = max(h_PtPosMu->GetFunction("gaus")->GetParameter(1) + 0.8*h_PtPosMu->GetFunction("gaus")->GetParameter(2), 
			      h_PtNegMu->GetFunction("gaus")->GetParameter(1) + 0.8*h_PtNegMu->GetFunction("gaus")->GetParameter(2));
      
      h_PtPosMu_cum = h_PtPosMu->GetCumulative(); h_PtPosMu_cum->Reset();
      h_PtNegMu_cum = h_PtNegMu->GetCumulative(); h_PtNegMu_cum->Reset();

      testValue = 0.;
      testValueError = 0.;
      for (int bin=h_PtPosMu->FindBin(lowerLimit); bin <= h_PtPosMu->FindBin(upperLimit); bin++) {
	//double thisValue = min(h_PtPosMu->GetBinContent(bin),  h_PtNegMu->GetBinContent(bin));
	//double thisValue = min(h_PtPosMu->GetFunction("gaus")->Eval(h_PtPosMu->GetBinCenter(bin)),  
	//		       h_PtNegMu->GetFunction("gaus")->Eval(h_PtNegMu->GetBinCenter(bin)));
	double fValue = h_PtPosMu->GetFunction("gaus")->Integral(lowerLimit, h_PtPosMu->GetBinCenter(bin));
	h_PtPosMu_cum->SetBinContent(bin, fValue);

	fValue = h_PtNegMu->GetFunction("gaus")->Integral(lowerLimit, h_PtNegMu->GetBinCenter(bin));
	h_PtNegMu_cum->SetBinContent(bin, fValue);

	//float thisValue = fabs(h_PtPosMu->GetFunction("gaus")->Integral(lowerLimit, h_PtPosMu->GetBinCenter(bin)) - 
	//		       h_PtNegMu->GetFunction("gaus")->Integral(lowerLimit, h_PtNegMu->GetBinCenter(bin))); 
														  
	//testValue = std::max(testValue, thisValue);
	//testValueError += sqrt(thisValue/2);
      }
      float theMaxDiff = -999.;
      int binMax = 0;
      for (int bin=1; bin <= m_NptBins; bin++) {
	if (h_Target->GetBinError(bin) > 1) { // to be sure there are some entries
	  if (fabs(h_PosMinusNeg->GetBinContent(bin) - h_Target->GetBinContent(bin)) > theMaxDiff) {
	    theMaxDiff = fabs(h_PosMinusNeg->GetBinContent(bin) - h_Target->GetBinContent(bin));
	    binMax = bin;
	  }
	}
	if (m_printLevel >= 5) {
	  std::cout << "   - ds: " << thisdeltasagitta << "  bin: " << bin << "  pt: " << h_PosMinusNeg->GetBinCenter(bin) 
		    << "   Delta: " << h_PosMinusNeg->GetBinContent(bin)  << " - " <<   h_Target->GetBinContent(bin) 
		    << "   Cumulative " << fabs(h_PosMinusNeg->GetBinContent(bin) - h_Target->GetBinContent(bin)) 
		    << "   max: " << theMaxDiff
		    << std::endl;
	}
      }
      testValue = theMaxDiff;
      testValueError = 2*theMaxDiff / h_Target->GetBinError(binMax);

      // 


      //testValue = 0.;
      //testValue += pow(h_PtPosMu->GetFunction("gaus")->GetParameter(0)-h_PtNegMu->GetFunction("gaus")->GetParameter(0),2);
      //testValue += pow(h_PtPosMu->GetFunction("gaus")->GetParameter(1)-h_PtNegMu->GetFunction("gaus")->GetParameter(1),2);
      //testValue += pow(h_PtPosMu->GetFunction("gaus")->GetParameter(2)-h_PtNegMu->GetFunction("gaus")->GetParameter(2),2);
	
      //testValueError = 0.03*testValue;
    }

    //
    theChi2Points->SetPoint(i, thisdeltasagitta, testValue);
    if (m_thisPass == 1) theChi2Points->SetPointError(i, 0.010, testValueError); 
    if (m_thisPass == 2) theChi2Points->SetPointError(i, 0.001, testValueError); 

    // draw both distributions in the same canvas -> find maximum
    float theMax = h_PtPosMu->GetMaximum();
    if (h_PtNegMu->GetMaximum() > theMax) theMax = h_PtNegMu->GetMaximum();
    tempCanvas.cd(1);
    h_PtPosMu->SetMaximum(theMax);
    h_PtPosMu->Draw();
    h_PtNegMu->Draw("same");
    theText.Clear();
    theText.Form("#delta_{sagitta} = %1.3f", centralDeltaSagitta + v_deltaSagitta_testValues.at(i));
    plotText.DrawLatexNDC(0.50,0.85,theText.Data());
    if (m_usedMethod == deltaSagittaFitter::PTPEAK) {
      theText.Clear();
      theText.Form("#mu^{+}: #mu=%1.1f #sigma=%1.1f GeV",h_PtPosMu->GetFunction("gaus")->GetParameter(1), h_PtPosMu->GetFunction("gaus")->GetParameter(2));
      plotText.DrawLatexNDC(0.50,0.79, theText.Data());
      theText.Clear();
      theText.Form("#mu^{-}: #mu=%1.1f #sigma=%1.1f GeV",h_PtNegMu->GetFunction("gaus")->GetParameter(1), h_PtNegMu->GetFunction("gaus")->GetParameter(2));
      plotText.DrawLatexNDC(0.50,0.73, theText.Data());
      theText.Clear();
      theText.Form("#chi^{2}= %1.1f", testValue);
      plotText.DrawLatexNDC(0.50,0.67, theText.Data());
    }


    if (m_usedMethod == deltaSagittaFitter::CHISQUARE) {
      tempCanvas.cd(2);
      h_PosMinusNeg->Draw();
      theText.Clear();
      theText.Form("#chi^{2} = %1.1f", testValue);
      plotText.DrawLatexNDC(0.50,0.85,theText.Data());
    }
    if (m_usedMethod == deltaSagittaFitter::CUMULATIVE || m_usedMethod == deltaSagittaFitter::PTPEAK) {      
      h_PtPosMu_cum->SetMaximum(1.05*normEntries); 
      tempCanvas.cd(2);
      h_PtPosMu_cum->Draw();
      h_PtNegMu_cum->Draw("same");
      theText.Clear();
      theText.Form("#Delta(#int dp_{T}) = %1.1f", testValue);
      plotText.DrawLatexNDC(0.15,0.85,theText.Data());
    }

    canName.Clear();
    canName.Form("deltaSagittaFitter_pt_eta_%02d_phi_%02d_ds_%02d_pass_%d.png",etabin, phibin, i+1, m_thisPass);
    if (m_saveOutputGraphics) tempCanvas.Print(canName.Data());
  }
  
  if (m_printLevel >= 10) theChi2Points->Print();

  TCanvas canvasOfTheFit("canvasOfTheFit","canvasOfTheFit",900,600);
  canvasOfTheFit.cd();
  theChi2Points->Draw();
  theChi2Points->GetXaxis()->SetTitle("#delta_{sagitta} [TeV^{-1}]");
  theChi2Points->GetYaxis()->SetTitle("#chi^{2}");
  theChi2Points->Fit("pol2","Q");
  double theXoftheMin = 0.;
  if (theChi2Points->GetFunction("pol2") != NULL) {
    theXoftheMin = - theChi2Points->GetFunction("pol2")->GetParameter(1) / (2*theChi2Points->GetFunction("pol2")->GetParameter(2));
  }


  if (theChi2Points->GetFunction("pol2") != NULL) {
    TString theFitStatus(gMinuit->fCstatu.Data());
    if (m_printLevel>= 10) std::cout << " ** deltaSagittaFitter::FitEtaPhiCell ** fit status: " << gMinuit->fCstatu.Data() << std::endl;
    if (theFitStatus.Index("CONVERGED") == -1 || 
	theXoftheMin < (centralDeltaSagitta+v_deltaSagitta_testValues.front()) || 
		       theXoftheMin > (centralDeltaSagitta + v_deltaSagitta_testValues.back()) ||
      m_usedMethod == deltaSagittaFitter::PTPEAK) {
      if (m_printLevel >=1) std::cout << " ** ** deltaSagittaFitter::FitEtaPhiCell ** ** WARNING ** Fit failed " << std::endl;      
      // loop over points and take the minimum
      double theYmin = 99999.;
      double xatYmin = 99999.;
      for (int i=0; i < theChi2Points->GetN(); i++) {
	double theX, theY;
	theChi2Points->GetPoint(i, theX, theY);
	std::cout << "   - point i " << i << " --> " << theX << " - " << theY << std::endl;
	if (theY < theYmin) {
	  theYmin = theY;
	  xatYmin = theX;
	}
      }
      theXoftheMin = xatYmin;
    }
  }

  m_fittedDeltaSagitta = theXoftheMin;
  float deltaSagittaError = 0;
  if (theChi2Points->GetFunction("pol2") != NULL) {
    deltaSagittaError = sqrt(pow(theXoftheMin * theChi2Points->GetFunction("pol2")->GetParError(1) /theChi2Points->GetFunction("pol2")->GetParameter(1),2) +
			     pow(theXoftheMin * theChi2Points->GetFunction("pol2")->GetParError(2) /theChi2Points->GetFunction("pol2")->GetParameter(2),2) );
  }
  std::cout << " -- deltaSagitta -- Fit result -- eta: " << etabin 
	    << " phi: " << phibin
	    <<  "   deltaSagitta:  " << m_fittedDeltaSagitta 
	    << "   +- " << deltaSagittaError
	    << std::endl; 

  theText.Clear();
  theText.Form("#delta_{sagitta} = %1.3f #pm %1.3f", theXoftheMin, deltaSagittaError);
  plotText.DrawLatexNDC(0.40,0.85,theText.Data());
  
  canName.Clear();
  canName.Form("deltaSagittaFitter_fit_eta_%02d_phi_%02d_pass_%d.png",etabin, phibin, m_thisPass);
  if (m_saveOutputGraphics) canvasOfTheFit.Print(canName.Data());


  if (m_printLevel >= 3)std::cout << " ** deltaSagittaFitter::FitEtaPhiCell ** COMPLETED **    eta: " << etabin << " phi: " << phibin << std::endl;
  return;
}
#endif

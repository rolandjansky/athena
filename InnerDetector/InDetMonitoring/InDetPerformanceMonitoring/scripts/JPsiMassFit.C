#ifndef JPSIMASSFIT_C
#define JPSIMASSFIT_C
#include "JPsiMassFit.h" 
//#include <TF1Convolution.h>

/////////////
JPsiMassFit::JPsiMassFit() : m_printLevel (0), 
			     m_analysisMode(JPsiMassFit::JPSIMASS), 
			     m_theMuonCharge(1),
			     h_inv_mass_vs_etaphi(NULL)
{
  h_inv_mass_vs_etaphi_fitresult = NULL;
  h_mcmassbiasmap = NULL;
  m_Can_m_per_etaphi = NULL;
  m_Can_m_per_barrel = NULL;
  m_isMC = false;
  // m_JPsi1SMCbias = {0.};
  this->SetHumanReadableKeys();
  m_massError = 1; 
  m_fitSigma = 0.;
  m_chi2overndf = 1.;
  m_reference_mass = JPsi1SPDGMass;
  m_theMuonChargeSetByUser = false;
  m_saveFitsInOutputFile = true;
  m_outputFileName.Clear();
  m_outputFileName.Append("newoutput.pdf");
  m_histogramUserTitle.Clear();

  m_massLowerValueForFit = -99999999.; // default values -> no limit
  m_massUpperValueForFit =  99999999.;

  return;
}

/////////////
JPsiMassFit::~JPsiMassFit()
{
}

/////////////
void JPsiMassFit::SetHumanReadableKeys ()
{
  m_AnalysisModeKeyWord[0].Clear(); m_AnalysisModeKeyWord[0].Append("Z MASS ANALYSIS");
  m_AnalysisModeKeyWord[1].Clear(); m_AnalysisModeKeyWord[1].Append("JPSI MASS ANALYSIS");
  m_AnalysisModeKeyWord[2].Clear(); m_AnalysisModeKeyWord[2].Append("UPSILON MASS ANALYSIS");
  m_AnalysisModeKeyWord[3].Clear(); m_AnalysisModeKeyWord[2].Append("DIMUON MASS ANALYSIS");
  m_AnalysisModeKeyWord[4].Clear(); m_AnalysisModeKeyWord[4].Append("KSHORT MASS ANALYSIS");
  return;
}

//////////////////
void JPsiMassFit::Execute()
{
  if (m_printLevel >= 0) {std::cout << " ** JPsiMass_Fit ** Execute() ** start ** " << std::endl;}

  if (h_inv_mass_vs_etaphi != NULL) {
    if (m_printLevel >= 0) {
      std::cout << " ** JPsiMass_Fit ** Execute() ** analysis mode: " << m_AnalysisModeKeyWord[m_analysisMode] 
		<< " (" << m_analysisMode << ") " << std::endl;
    }

    if (m_printLevel >= 0) {
      std::cout << " ** JPsiMass_Fit ** Execute() ** Analysing \"" << h_inv_mass_vs_etaphi->GetTitle() 
		<< "\" histogram with " << h_inv_mass_vs_etaphi->GetEntries() << " entries "
		<< std::endl;
    }

    if (m_printLevel >= 0) {
      if (m_Can_m_per_barrel != NULL) m_Can_m_per_barrel = NULL;
      if (m_Can_m_per_etaphi == NULL) {
	m_Can_m_per_etaphi = new TCanvas("canmetaphi","Invariant mass fits",1000, 850);
	// --> Salva: do not divide store fit by fit in pdf // m_Can_m_per_etaphi->Divide( h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetNbins());
      }
      if (m_Can_m_per_barrel == NULL && m_analysisMode == JPsiMassFit::JPSIMASS) {
        m_Can_m_per_barrel = new TCanvas("m_Can_m_per_barrel","Invariant mass fit barrel",1000,850);
      }
    }
    
    int etabin = 1;
    int phibin = 1;
    double targetMass = JPsi1SPDGMass; // this is different from reference mass 
    // m_referenceMass is the PDG value that depends on the particle type
    // targetMass is the PDG value but corrected for possible MC biases

    bool fittingMethodKnown = false;
    m_reference_mass = JPsi1SPDGMass; // default
    if (m_analysisMode == JPsiMassFit::JPSIMASS) {fittingMethodKnown = true;}
    if (m_analysisMode == JPsiMassFit::UPSILONMASS) {m_reference_mass = Upsilon1SPDGMass; fittingMethodKnown = true;}
    if (m_analysisMode == JPsiMassFit::ZMASS) {m_reference_mass = ZPDGMass; fittingMethodKnown = true;}
    if (m_analysisMode == JPsiMassFit::DIMUONMASS) {m_reference_mass = ZPDGMass; fittingMethodKnown = false;}
    if (m_analysisMode == JPsiMassFit::KSHORTMASS) {m_reference_mass = KShortPDGMass; fittingMethodKnown = true;}
    
    // build TH2 to store the dimuon mass fit result (eta-phi map)
    h_inv_mass_vs_etaphi_fitresult = this->BuildHistogramForMassMap();
    h_inv_mass_vs_etaphi_fiterror = this->BuildHistogramForMassErrorMap();
    h_inv_mass_vs_etaphi_fitsigma = this->BuildHistogramForMassSigmaMap();
    h_inv_mass_vs_etaphi_chi2overndf = this->BuildHistogramForMassChi2();

    // build TH2 to store the invariant mass fit results compared with the PDG value
    h_inv_mass_vs_etaphi_fitresultVsPDG = this->BuildHistogramForMassMapVsPDG();
    h_inv_mass_vs_etaphi_fitresultVsPDGpc = this->BuildHistogramForMassMapVsPDGpc();

    // build TH2 too store the mass shift significance
    h_mass_shift_significance_vs_etaphi = this->BuildHistogramForMassShiftSignificance();
     
    // 
    bool outputFileOpen = false;

    this->SetOutputFileName();

    // Project the mass distribution bin-by-bin (in eta-phi)
    for ( etabin = 1; etabin <= h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(); etabin++) {
      for ( phibin = 1; phibin <= h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(); phibin++) {
	TString hname("h_invmass_etabin_"); hname += etabin;
	TString htitle;
	hname.Append("_phibin_"); hname += phibin; 
	if (m_printLevel >= 0) {std::cout << std::endl << " ** JPsiMass_Fit ** Execute() ** Projecting etabin " << etabin << "  phibin " << phibin << std::endl;}
	TH1D * h_this_etaphi_bin = h_inv_mass_vs_etaphi->ProjectionZ(hname.Data(), etabin, etabin, phibin, phibin,"e");
	htitle.Clear();
	if (m_histogramUserTitle.IsNull()) {
	  htitle.Append(h_this_etaphi_bin->GetTitle());
	}
	else {
	  htitle.Append(m_histogramUserTitle.Data());
	}
	htitle.Append(" eta bin: "); htitle+= etabin;
	htitle.Append(" phi bin: "); htitle+= phibin;
	h_this_etaphi_bin->SetTitle(htitle.Data());
	if (m_printLevel >= 0) {std::cout << "                                 entries: " <<  h_this_etaphi_bin->GetEntries() << std::endl;}
	
	if (m_Can_m_per_etaphi != NULL) {
	  m_Can_m_per_etaphi->cd();
	  // Salva --> do not divide: store fit by fit in a pdf file
	  // int pad_number =  h_inv_mass_vs_etaphi->GetXaxis()->GetNbins()*(h_inv_mass_vs_etaphi->GetYaxis()->GetNbins()-phibin) + etabin;
	  // m_Can_m_per_etaphi->cd(pad_number);
	  // h_this_etaphi_bin->DrawCopy();  
	}
	  
	// fit dimuon invariant mass distribution
	double thismass = m_reference_mass;
        double peakmass = m_reference_mass;
	targetMass = this->GetTargetMass(etabin, phibin);
	m_massError = 1; // default value
	m_fitSigma = 0; // default value
	m_chi2overndf = 1; //default value
	// but check there is enough entries
	if (h_this_etaphi_bin->GetEntries() > MinEntriesInHistoForFit && fittingMethodKnown) {
          thismass = this->FitInvMassDistribution(h_this_etaphi_bin);
          std::cout << " ** JPsiMass_Fit ** Execute() ** mass fit result **"
		    << " (eta, phi)= (" << etabin << ", " << phibin << ")" 
		    << "  entries: " << h_this_etaphi_bin->GetEntries() << std::endl
		    << "                 fitted mass: " << thismass << " GeV"
		    << " TargetMass(" << m_reference_mass 
		    << "+MCbias " << targetMass - m_reference_mass
		    <<" ): " << targetMass << " GeV"; 
	  if (fabs(thismass - targetMass) * JPsiMassFit::GeVtoMeV > 0.0009) {
	    std::cout << "   deltaM: " << (thismass - targetMass) * JPsiMassFit::GeVtoMeV << " MeV !!"; 
	    std::cout << "    Error: " << m_massError * JPsiMassFit::GeVtoMeV << " MeV" << std::endl;
          }
	  else {
	    std::cout << "   deltaM: " << (thismass - targetMass) * JPsiMassFit::GeVtoMeV * JPsiMassFit::MeVtokeV  << " !! keV !!";	      
	    std::cout << "    Error: " << m_massError * JPsiMassFit::GeVtoMeV * JPsiMassFit::MeVtokeV << " !! keV !!" << std::endl;
          }
          
	}
	else {
	  h_this_etaphi_bin->Draw(); // draw histogram even if it were not fitted
	}

	if (m_saveFitsInOutputFile) {
	  if (etabin == h_inv_mass_vs_etaphi->GetXaxis()->GetNbins() && phibin ==  h_inv_mass_vs_etaphi->GetYaxis()->GetNbins()) {
	    if (outputFileOpen) {
	      // this is the last --> close the file 
	      TString theOuputFileWithCloseClause(m_outputFileName.Data());  theOuputFileWithCloseClause.Append(")");
	      m_Can_m_per_etaphi->Print( theOuputFileWithCloseClause.Data(), "pdf");
	      if (m_printLevel >= 0) std::cout << " ** JPsiMassFit ** close output file: newoutput.pdf " << std::endl;
	    }
	  }
	  else {
	    m_Can_m_per_etaphi->Print(m_outputFileName.Data(),"pdf");	    
	    if (m_printLevel >= 0) std::cout << " ** JPsiMassFit ** canvas added to " << m_outputFileName.Data() << std::endl;
	  }
	  if (!outputFileOpen) { 
	    // output file is not yet open -> open the file
	    TString theOuputFileWithOpenClause(m_outputFileName.Data()); 
	    if (h_inv_mass_vs_etaphi->GetXaxis()->GetNbins() > 1 || 
		h_inv_mass_vs_etaphi->GetYaxis()->GetNbins() > 1) {
	      // file needs to be open for more plots
	      theOuputFileWithOpenClause.Append("(");
	    }
	    if (m_printLevel >= 0) std::cout << " ** JPsiMassFit ** open output file: " << m_outputFileName.Data() << std::endl;
	    m_Can_m_per_etaphi->Print(theOuputFileWithOpenClause,"pdf");
	    outputFileOpen = true;
	  }
	  
	  m_Can_m_per_etaphi->Print("fittest.png");
	  m_Can_m_per_etaphi->Print("fittest.pdf");
	}

	// fill histos with fit results
	h_inv_mass_vs_etaphi_fitresult->SetBinContent(etabin, phibin, thismass);
	h_inv_mass_vs_etaphi_fiterror->SetBinContent(etabin, phibin, m_massError*JPsiMassFit::GeVtoMeV);
	h_inv_mass_vs_etaphi_fitresultVsPDG->SetBinContent(etabin, phibin, m_theMuonCharge*(thismass - targetMass)*JPsiMassFit::GeVtoMeV);
	h_inv_mass_vs_etaphi_fitresultVsPDGpc->SetBinContent(etabin, phibin, m_theMuonCharge*(thismass - targetMass)/targetMass); // same as above but in %
	h_mass_shift_significance_vs_etaphi->SetBinContent(etabin, phibin, m_theMuonCharge*(thismass - targetMass)/m_massError);
	h_inv_mass_vs_etaphi_fitsigma->SetBinContent(etabin, phibin, m_fitSigma*JPsiMassFit::GeVtoMeV);
	h_inv_mass_vs_etaphi_chi2overndf->SetBinContent(etabin, phibin, m_chi2overndf);
	m_Can_m_per_etaphi->Print("test.png");
	
	if (m_printLevel >= 0 && m_analysisMode == JPsiMassFit::JPSIMASS && h_inv_mass_vs_etaphi->GetXaxis()->GetNbins() == 3 && etabin == 2) {
	  if (m_Can_m_per_barrel == NULL ) {m_Can_m_per_barrel = new TCanvas("canmbarrel","Invariant mass fit barrel",1000,850);}
	  else {
	    m_Can_m_per_barrel->cd();
	  }      
	}
	
      } // end of loop on phi bins
    } // end of loop on eta bins
      
    // simetrize the histogram with the fit result
    if (true) {
      double upper_gap = h_inv_mass_vs_etaphi_fitresult->GetMaximum() - targetMass; if (upper_gap < 0) upper_gap = -upper_gap;
      double lower_gap = h_inv_mass_vs_etaphi_fitresult->GetMinimum() - targetMass; if (lower_gap < 0) lower_gap = -lower_gap;
      double the_gap = upper_gap; if (lower_gap > the_gap) the_gap = lower_gap; the_gap = 1.05*the_gap; if (the_gap < 0.005) the_gap = 0.005;
      
      h_inv_mass_vs_etaphi_fitresult->SetMaximum(targetMass + the_gap);
      h_inv_mass_vs_etaphi_fitresult->SetMinimum(targetMass - the_gap);
      h_inv_mass_vs_etaphi_fitresultVsPDG->SetMaximum(the_gap * JPsiMassFit::GeVtoMeV);
      h_inv_mass_vs_etaphi_fitresultVsPDG->SetMinimum(-the_gap * JPsiMassFit::GeVtoMeV);
      h_inv_mass_vs_etaphi_fitresultVsPDGpc->SetMaximum(the_gap * JPsiMassFit::GeVtoMeV/targetMass);
      h_inv_mass_vs_etaphi_fitresultVsPDGpc->SetMinimum(-the_gap * JPsiMassFit::GeVtoMeV/targetMass);
      if (m_printLevel >= 10){ 
	std::cout << " ** JPsiMass_Fit ** Execute() ** range: " << the_gap 
		  << "  range: " <<  h_inv_mass_vs_etaphi_fitresult->GetMinimum() << " --> " <<  h_inv_mass_vs_etaphi_fitresult->GetMaximum() << std::endl;
      }
    }
  } 
  else {
      if (m_printLevel >= 0) {std::cout << " ** JPsiMass_Fit ** Execute() ** No input histogram defined ** " << std::endl;}
  }

  if (m_printLevel >= 0) {std::cout << " ** JPsiMass_Fit ** Execute() ** completed ** " << std::endl;}  
  return;
}

//////////////////
TH2 * JPsiMassFit::BuildHistogramForMassMap()
{
  TString hname (h_inv_mass_vs_etaphi->GetName());
  hname.Append("_fitresult");
  
  TString htitle (h_inv_mass_vs_etaphi->GetTitle());
  htitle.Append(" fit result");

  TH2 * h_mass_map = new TH2F (hname.Data(), htitle.Data(),
			       h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmax(),
			       h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmax());
  h_mass_map->SetXTitle(h_inv_mass_vs_etaphi->GetXaxis()->GetTitle());
  h_mass_map->SetYTitle(h_inv_mass_vs_etaphi->GetYaxis()->GetTitle());
  h_mass_map->SetZTitle(h_inv_mass_vs_etaphi->GetZaxis()->GetTitle());
  h_mass_map->SetStats(kFALSE);
  return h_mass_map;
}

//////////////////
TH2 * JPsiMassFit::BuildHistogramForMassErrorMap()
{
  TString hname (h_inv_mass_vs_etaphi->GetName());
  hname.Append("_fiterror");
  
  TString htitle (h_inv_mass_vs_etaphi->GetTitle());
  htitle.Append(" fit error");

  TH2 * h_mass_map = new TH2F (hname.Data(), htitle.Data(),
			       h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmax(),
			       h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmax());
  h_mass_map->SetXTitle(h_inv_mass_vs_etaphi->GetXaxis()->GetTitle());
  h_mass_map->SetYTitle(h_inv_mass_vs_etaphi->GetYaxis()->GetTitle());
  h_mass_map->SetZTitle("MeV");
  h_mass_map->SetStats(kFALSE);
  return h_mass_map;
}

//////////////////
TH2 * JPsiMassFit::BuildHistogramForMassSigmaMap()
{
  TString hname (h_inv_mass_vs_etaphi->GetName());
  hname.Append("_sigma");
  
  TString htitle (h_inv_mass_vs_etaphi->GetTitle());
  htitle.Append(" sigma ");

  TH2 * h_mass_map = new TH2F (hname.Data(), htitle.Data(),
			       h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmax(),
			       h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmax());
  h_mass_map->SetXTitle(h_inv_mass_vs_etaphi->GetXaxis()->GetTitle());
  h_mass_map->SetYTitle(h_inv_mass_vs_etaphi->GetYaxis()->GetTitle());
  h_mass_map->SetZTitle("#sigma_{M} [MeV]");
  h_mass_map->SetStats(kFALSE);
  return h_mass_map;
}

//////////////////
TH2 * JPsiMassFit::BuildHistogramForMassMapVsPDG()
{
  TString hname (h_inv_mass_vs_etaphi->GetName());
  hname.Append("_fitresultVsPDG");
  
  TString htitle (h_inv_mass_vs_etaphi->GetTitle());
  htitle.Append(": ");
  if (m_theMuonChargeSetByUser) htitle.Append(" q x ");
  htitle.Append("(M_{fit} - M_{PDG})");

  TH2 * h_mass_map = new TH2F (hname.Data(), htitle.Data(),
			       h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmax(),
			       h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmax());
  h_mass_map->SetXTitle(h_inv_mass_vs_etaphi->GetXaxis()->GetTitle());
  h_mass_map->SetYTitle(h_inv_mass_vs_etaphi->GetYaxis()->GetTitle());
  h_mass_map->SetZTitle("MeV");
  h_mass_map->SetStats(kFALSE);
  return h_mass_map;
}

//////////////////
TH2 * JPsiMassFit::BuildHistogramForMassMapVsPDGpc()
{
  TString hname (h_inv_mass_vs_etaphi->GetName());
  hname.Append("_fitresultVsPDGpc");
  
  TString htitle (h_inv_mass_vs_etaphi->GetTitle());
  htitle.Append(": ");
  if (m_theMuonChargeSetByUser) htitle.Append(" q x ");
  htitle.Append(":(M_{fit} - M_{PDG})/M_{PDG}");

  TH2 * h_mass_map = new TH2F (hname.Data(), htitle.Data(),
			       h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmax(),
			       h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmax());
  h_mass_map->SetXTitle(h_inv_mass_vs_etaphi->GetXaxis()->GetTitle());
  h_mass_map->SetYTitle(h_inv_mass_vs_etaphi->GetYaxis()->GetTitle());
  h_mass_map->SetStats(kFALSE);
  return h_mass_map;
}

//////////////////
TH2 * JPsiMassFit::BuildHistogramForMassShiftSignificance()
{
  TString hname (h_inv_mass_vs_etaphi->GetName());
  hname.Append("_MassShiftSignificance");
  
  TString htitle (h_inv_mass_vs_etaphi->GetTitle());
  htitle.Append(": (M_{fit} - M_{PDG}) / #sigma_{M}");

  TH2 * h_mass_map = new TH2F (hname.Data(), htitle.Data(),
			       h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmax(),
			       h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmax());
  h_mass_map->SetXTitle(h_inv_mass_vs_etaphi->GetXaxis()->GetTitle());
  h_mass_map->SetYTitle(h_inv_mass_vs_etaphi->GetYaxis()->GetTitle());
  h_mass_map->SetZTitle("Significance");
  h_mass_map->SetStats(kFALSE);
  h_mass_map->SetMaximum(5.);
  h_mass_map->SetMinimum(-5.);
  return h_mass_map;
}

//////////////////                                                                                                                                                                                           
TH2 * JPsiMassFit::BuildHistogramForMassChi2()
{
  TString hname (h_inv_mass_vs_etaphi->GetName());
  hname.Append("_Chi2overNdf");

  TString htitle (h_inv_mass_vs_etaphi->GetTitle());
  htitle.Append(": #chi^{2} / ndf");

  TH2 * h_mass_map = new TH2F (hname.Data(), htitle.Data(),
                               h_inv_mass_vs_etaphi->GetXaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetXaxis()->GetXmax(),
                               h_inv_mass_vs_etaphi->GetYaxis()->GetNbins(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmin(),  h_inv_mass_vs_etaphi->GetYaxis()->GetXmax());
  h_mass_map->SetXTitle(h_inv_mass_vs_etaphi->GetXaxis()->GetTitle());
  h_mass_map->SetYTitle(h_inv_mass_vs_etaphi->GetYaxis()->GetTitle());
  h_mass_map->SetZTitle("");
  h_mass_map->SetStats(kFALSE);
  return h_mass_map;
}

//////////////////
double JPsiMassFit::FitInvMassDistribution (TH1 * h_input)
{
  double thismass = 0.; // default value
  this->HistogramPreConditioning(h_input);
  //if (m_analysisMode == JPsiMassFit::ZMASS)    thismass = this->FitZMassDistribution(h_input);
  if (m_analysisMode == JPsiMassFit::ZMASS)    thismass = this->NEW_FitZMassDistribution(h_input);
  //if (m_analysisMode == JPsiMassFit::JPSIMASS) thismass = this->FitJPsiMassDistributionRooFit(h_input);
  if (m_analysisMode == JPsiMassFit::JPSIMASS) thismass = this->FitJPsiMassDistribution(h_input);
  if (m_analysisMode == JPsiMassFit::UPSILONMASS) thismass = this->FitUpsilonMassDistribution(h_input);
  if (m_analysisMode == JPsiMassFit::DIMUONMASS) thismass = this->FitJPsiMassDistribution(h_input);
  if (m_analysisMode == JPsiMassFit::KSHORTMASS) thismass = this->FitKShortMassDistribution(h_input);
  //if (m_analysisMode == JPsiMassFit::ZMASS) thismass = this->FindPeakValue(h_input);

  return thismass;
}

//////////////////
double JPsiMassFit::FitJPsiMassDistributionRooFit (TH1 * h_input)
{
  // Create a binned dataset that imports contents of TH1 and associates its contents to observable 'x'
  // RooDataHist dh_Mass_Pos("dh_Mass_Pos","dh JPsi Mass Pos",Mass_Pos,Import(*h_input)) ;

  double x_min = h_input->GetXaxis()->GetXmin(); if ( x_min < 2.) x_min = 2.;
  double x_max = h_input->GetXaxis()->GetXmax();

  RooRealVar  x("x", h_input->GetTitle(), x_min, x_max) ;
  RooDataHist inv_mass_data("inv_mass_data","dimuon invariant mass",x, h_input) ;
  inv_mass_data.weightError(RooAbsData::SumW2);

  //crosscheck to check entries
  for (int i=0;i<inv_mass_data.numEntries();i++){
    //inv_mass_data.get(i);
    cout << " Bin " << i << " x="<<inv_mass_data.get(i)->getRealValue("x")
	 << "  Weight="<<inv_mass_data.weight()
	 << "  Weight Error= "<<inv_mass_data.weightError(RooAbsData::SumW2)<<endl;
  }

  x.setRange("JPsi1SPeak",JPsi1SPDGMass-0.54,JPsi1SPDGMass+0.5) ;
  x.setRange("BackgroundLowMass", x_min, JPsi1SPDGMass-0.2) ; //was in argus model -0.5
  x.setRange("BackgroundHighMass", JPsi1SPDGMass+1., x_max) ;
  x.setRange("TotalRange",x_min,  x_max);

  // number of events in the jpsi peak and in the background
  RooRealVar nevts_jpsi1s("nevts_jpsi1s","# JPsi(1s) events", h_input->GetEntries()/2, 0.,3000000) ;
  RooRealVar nevts_jpsi2s("nevts_jpsi2s","# JPsi(2s) events", h_input->GetEntries()/20, 0.,3000000) ;
  RooRealVar nevts_jpsi1s_gauss_mc("nevts_jpsi1s_gauss_mc","# JPsi(1s) events for Gaussian", h_input->GetEntries()/100, 0.,3000000.) ;//5000.
  RooRealVar nevts_jpsi1s_cb("nevts_jpsi1s_cb","# JPsi(1s) events for Crystal Ball", h_input->GetEntries(), 0., 3000000.) ;
  RooRealVar nevts_bkg("nevts_bkg","# background events",h_input->GetEntries()/10 , 0.,3000000) ;
 
  // Gaussian for the J/Psi (1S) peak
  RooRealVar jpsi1s_mean("jpsi1s_mean","jpsi(1s) mass", JPsi1SPDGMass, JPsi1SPDGMass-1., JPsi1SPDGMass+0.2);
  RooRealVar jpsi1s_sigma("jpsi1s_sigma","jpsi(1s) sigma",1.0,-1.0,1.0) ;
  RooGaussian jpsi1s_gauss("jpsi1s_gauss","gauss(x,mean,sigma)", x ,jpsi1s_mean, jpsi1s_sigma);

  // Gaussian for the J/Psi (1S) peak in MC sum
  RooRealVar jpsi1s_mean_mc("jpsi1s_mean_mc","jpsi(1s) mass", JPsi1SPDGMass, JPsi1SPDGMass-1., JPsi1SPDGMass+0.2);
  RooRealVar jpsi1s_sigma_mc("jpsi1s_sigma_mc","jpsi(1s) sigma",0.5,-0.5,0.5) ;
  RooGaussian jpsi1s_gauss_mc("jpsi1s_gauss_mc","gauss(x,mean,sigma)", x ,jpsi1s_mean, jpsi1s_sigma_mc);


  // get an estimate of the sigma
  if (true) {
    jpsi1s_mean.setConstant(kTRUE);  // fix the mean and fit to get a reasonable estimate of the sigma
    jpsi1s_gauss.fitTo(inv_mass_data, RooFit::Range("JPsi1SPeak"));
    //jpsi1s_mean.setConstant(kTRUE);  // fix the mean and fit to get a reasonable estimate of the sigma
    //jpsi1s_gauss_mc.fitTo(inv_mass_data, RooFit::Range("JPsi1SPeak"));
 
  }

  // Gaussian for the J/Psi (2S) peak
  RooRealVar jpsi2s_mean("jpsi2s_mean","jpsi(2s) mass", JPsi2SPDGMass, JPsi2SPDGMass-0.5, JPsi2SPDGMass+0.5) ;
  //RooRealVar jpsi2s_sigma("jpsi2s_sigma","jpsi(2s) sigma",jpsi1s_sigma.getVal() , -1., 1.) ; // same initial value as the jpsi 1s sigma
  RooGaussian jpsi2s_gauss("jpsi2s_gauss","gauss(x,mean,sigma)", x ,jpsi2s_mean, jpsi1s_sigma); // use the same sigma as for the 1s

  if (!m_isMC) jpsi2s_mean.setConstant(kTRUE);  // for the time being, fix the psi 2s mass

  // polinomial shape for the background
  RooRealVar aa_Pos("aa_Pos","aa_Pos",1018., 1010., 1030.);//,1010.,1030.); //was 1010,1030
  RooRealVar bb_Pos("bb_Pos","bb_Pos",-221.77, -222.,-220.); //was -222, -220
  RooRealVar cc_Pos("cc_Pos","cc_Pos",0.99, 0.5, 8.);//,0.5,8) ; //was 1.5
  RooPolynomial bkgdpoly("bkgdpoly","2th order poly", x ,RooArgList(aa_Pos,bb_Pos,cc_Pos));
  if(!m_isMC) bkgdpoly.fitTo(inv_mass_data, RooFit::Range("BackgroundHighMass")); // initial fit to estimate the shape 
  
  //alternative model based in a crystall ball
  RooRealVar cb_alpha("cb_alpha","cb_alpha", 1.5, 1.0, 3.0) ; // in argus data was -1.5, -5 , -1
  RooRealVar cb_n("cb_n","cb_n", 2.5, 1.0, 10.) ; //in argus data was 10, 0.1, 25
  RooCBShape  crystalBall("CB", "Crystal Ball shape", x , jpsi1s_mean, jpsi1s_sigma, cb_alpha, cb_n);

  //alternative Novosibirsk model
  RooRealVar novsigma("novsigma","sigma",0.07,-1.0,1.0) ;
  RooRealVar novpeak("novpeak","peak", JPsi1SPDGMass, JPsi1SPDGMass-1.0, JPsi1SPDGMass+1.0) ;
  RooRealVar novtail("novtail","lambda parameter", -0.1, -0.3, 0.3) ;
  RooNovosibirsk  novosibirsk("novosibirsk", "Novosibirsk shape", x , jpsi1s_mean, novsigma, novtail);
 
  // MC: fit signal and Argus in low mass region to get an estimate of the parameters
  /*argpeak.setConstant(kTRUE);
  argus.fitTo(inv_mass_data, RooFit::Range("BackgroundLowMass"));
  //RooAddPdf argPlusSignal ("argPlusSignal","gaus+argus", RooArgList(jpsi1s_gauss, argus), RooArgList(nevts_jpsi1s, nevts_argus)) ;
  RooAddPdf argPlusSignal ("argPlusSignal","cb+argus", RooArgList(crystalBall, argus), RooArgList(nevts_jpsi1s, nevts_argus)) ;
  argPlusSignal.fitTo(inv_mass_data, RooFit::Range("BackgroundLowMass, JPsi1SPeak"));
  argpeak.setConstant(kFALSE);*/

  // Data: fit signal in low mass region to get an estimate of the parameters
  /*cb_alpha.setConstant(kTRUE);
  cb_n.setConstant(kTRUE);
  crystalBall.fitTo(inv_mass_data, RooFit::Range("BackgroundLowMass"));
  //RooAddPdf argPlusSignal ("argPlusSignal","gaus+argus", RooArgList(jpsi1s_gauss, argus), RooArgList(nevts_jpsi1s, nevts_argus)) ;
  RooAddPdf GausPlusSignal ("GausPlusSignal","cb+gaus", RooArgList(crystalBall, jpsi1s_gauss_mc), RooArgList(nevts_jpsi1s, nevts_jpsi1s_gauss_mc)) ;
  GausPlusSignal.fitTo(inv_mass_data, RooFit::Range("BackgroundLowMass,JPsi1SPeak"));
  cb_alpha.setConstant(kFALSE);
  cb_n.setConstant(kFALSE);*/
  
  // release the jpsi1s mass and fit with all params free
  if (true) jpsi1s_mean.setConstant(kFALSE);
  //jpsi1s_mean_mc.setConstant(kFALSE);
  // fix params
  //jpsi1s_mean.setConstant(kTRUE);
  //jpsi1s_sigma.setConstant(kFALSE);
  //cb_alpha.setConstant(kTRUE);
  //argpar.setConstant(kTRUE);
  //argpeak.setConstant(kTRUE);

  // Total pdf: signal peak + background
  RooAddPdf total_sum("total_sum","CB+gauss+background+gauss", RooArgList(crystalBall, jpsi2s_gauss, bkgdpoly, jpsi1s_gauss_mc),RooArgList(nevts_jpsi1s, nevts_jpsi2s, nevts_bkg, nevts_jpsi1s_gauss_mc)) ;
  RooAddPdf mc_sum("mc_sum", "CB  + gauss", RooArgList(crystalBall, jpsi1s_gauss_mc), RooArgList(nevts_jpsi1s_cb, nevts_jpsi1s_gauss_mc));

  // fit depends on data/MC
  if (m_isMC) {
    //jpsi1s_gauss.fitTo(inv_mass_data, RooFit::Range("JPsi1SPeak"));
    //Different kind of pdf fits
    //Chi2 Goodness of Fit
    //RooChi2Var Chi2Fit ("chi2","chi2", jpsi1s_gauss, inv_mass_data, RooFit::DataError(RooAbsData::SumW2));//, RooFit::Range("JPsi1SPeak"));
    //RooChi2Var Chi2Fit ("chi2","chi2", crystalBall, inv_mass_data, RooFit::DataError(RooAbsData::SumW2)); //, RooFit::Range("JPsi1SPeak"));
    //RooChi2Var Chi2Fit ("chi2","chi2", novosibirsk, inv_mass_data, RooFit::DataError(RooAbsData::SumW2));
    RooChi2Var Chi2Fit ("chi2","chi2", mc_sum, inv_mass_data, RooFit::DataError(RooAbsData::SumW2));
    RooMinuit mfitchi2(Chi2Fit);
    mfitchi2.simplex();
    mfitchi2.migrad();
    mfitchi2.improve();
    //mfitchi2.minos();
    RooFitResult* Result = mfitchi2.save();
    Result->Print();
    Result->correlationMatrix().Print();

    //Likelihood Fit
    //RooNLLVar nll("nll","nll",crystalBall,inv_mass_data) ;
    //RooMinuit mfitnll(nll);
    /*mfitnll.simplex();
    mfitnll.migrad();
    mfitnll.improve();
    mfitnll.minos();
    RooFitResult* Result = mfitnll.save();
    Result->Print();
    Result->correlationMatrix().Print();*/

  }
  else {// real data
    //total_sum.fitTo(inv_mass_data, RooFit::Range("TotalRange"));
    //Chi2  fitO
    RooChi2Var Chi2Fit ("chi2","chi2", total_sum, inv_mass_data, RooFit::DataError(RooAbsData::SumW2));
    RooMinuit mfitchi2(Chi2Fit);
    mfitchi2.simplex();
    mfitchi2.migrad();
    mfitchi2.improve();
    //mfitchi2.minos();
    RooFitResult* Result = mfitchi2.save();
    Result->Print();
    Result->correlationMatrix().Print();

    //Likelihood Fit
    //RooNLLVar nll("nll","nll",crystalBall,inv_mass_data) ;
    //RooMinuit mfitnll(nll);
    /*mfitnll.simplex();
    mfitnll.migrad();
    mfitnll.improve();
    mfitnll.minos();
    RooFitResult* Result = mfitnll.save();
    Result->Print();
    Result->correlationMatrix().Print();*/

  }

  // drawing
  RooPlot* Mass_frame = x.frame(RooFit::Title(h_input->GetTitle()));
  Mass_frame->SetXTitle(h_input->GetXaxis()->GetTitle());
  inv_mass_data.plotOn(Mass_frame, RooFit::DataError(RooAbsData::SumW2));

  if(m_isMC){
    //jpsi1s_gauss.plotOn(Mass_frame, RooFit::LineColor(kBlue+1));//, RooFit::Range("JPsi1SPeak"));
    //crystalBall.plotOn(Mass_frame, RooFit::LineColor(kRed+1));//, RooFit::Range("JPsi1SPeak"));
    //novosibirsk.plotOn(Mass_frame,  RooFit::LineColor(kGreen+1)) ;
    mc_sum.plotOn(Mass_frame, RooFit::Components(crystalBall), RooFit::LineColor(kRed+1));
    mc_sum.plotOn(Mass_frame, RooFit::Components(jpsi1s_gauss_mc), RooFit::LineColor(kBlue+1));
    //mc_sum.plotOn(Mass_frame, RooFit::Components(novosibirsk), RooFit::LineColor(kGreen+1));
    mc_sum.plotOn(Mass_frame, RooFit::LineColor(kBlack));
  }
  else{
    total_sum.plotOn(Mass_frame,RooFit::Components(bkgdpoly), RooFit::LineStyle(kDashed));
    total_sum.plotOn(Mass_frame,RooFit::Components(jpsi1s_gauss_mc), RooFit::LineColor(kRed));
    total_sum.plotOn(Mass_frame,RooFit::Components(crystalBall), RooFit::LineColor(kGreen+1));
    total_sum.plotOn(Mass_frame,RooFit::Components(jpsi2s_gauss), RooFit::LineColor(kOrange+1));
    total_sum.plotOn(Mass_frame);
    }
  Mass_frame->Draw();

  //gPad->SetLogy(1);
  gPad->Update();
  //std::cout << " cb_alpha: " << cb_alpha.getVal() << "  cb_n:" << cb_n.getVal() << "  product:" << cb_alpha.getVal() * cb_n.getVal() << std::endl;
  //char input;
  //cin>>input;

  // compute the signal to noise ratio
  float nSignal = nevts_jpsi1s.getVal() + nevts_jpsi1s_gauss_mc.getVal() + nevts_jpsi2s.getVal();
  float nBackground = nevts_bkg.getVal(); //+ nevts_argus.getVal();
  std::cout << " ** JPsi fit ** signal events = " << nSignal 
	    << "   Background events: " << nBackground 
	    << "   S/B: " << nSignal / nBackground 
	    << "   S/Sqrt(B): " << nSignal / sqrt(nBackground) 
	    << std::endl;

  //m_fitSigma = fabs(jpsi1s_sigma.getVal());
  if (m_isMC) { 
    double sigma1_2 = pow(jpsi1s_sigma.getVal(),2);
    double sigma2_2 = pow(jpsi1s_sigma_mc.getVal(),2);
    double weight1 = nevts_jpsi1s_cb.getVal() / (nevts_jpsi1s_cb.getVal() + nevts_jpsi1s_gauss_mc.getVal());
    double weight2 = nevts_jpsi1s_gauss_mc.getVal() / (nevts_jpsi1s_cb.getVal() + nevts_jpsi1s_gauss_mc.getVal());
    m_fitSigma = sqrt(  weight1 * sigma1_2 + weight2 * sigma2_2);
  }
  else {
    m_fitSigma = sqrt((jpsi1s_sigma.getVal()*jpsi1s_sigma.getVal())*(nevts_jpsi1s.getVal()*nevts_jpsi1s.getVal()) + (jpsi1s_sigma_mc.getVal()*jpsi1s_sigma_mc.getVal())*(nevts_jpsi1s_gauss_mc.getVal()*nevts_jpsi1s_gauss_mc.getVal()));
  }

  m_massError = jpsi1s_mean.getError();
  return jpsi1s_mean.getVal();
}

//////////////////
double JPsiMassFit::FitJPsiMassDistribution (TH1 * h_input)
{
  //if (!m_isMC) jpsi2s_mean.setConstant(kTRUE);  // for the time being, fix the psi 2s mass
  double x_min = h_input->GetXaxis()->GetXmin(); if ( x_min < 2.) x_min =  2.;
  double x_max = h_input->GetXaxis()->GetXmax(); if ( x_max >10.) x_max = 10.;
  h_input->SetMinimum(0);
  bool FitJPsiMass_isMC = m_isMC;
  // isMC set to true for the time being because missing Truth tree in the n-tuple
  FitJPsiMass_isMC = true;
  
  TF1* jpsi_pdf = new TF1("jpsi_pdf", JPsiFit, x_min, x_max, 12);
  jpsi_pdf->SetLineColor(kBlue);
  jpsi_pdf->SetLineWidth(2);
  jpsi_pdf->SetParName(0, "Nevts (1S) GaussN");    jpsi_pdf->SetParameter(0,  h_input->GetEntries()/20); 
  jpsi_pdf->SetParName(1, "Nevts (1S) GaussB"); jpsi_pdf->SetParameter(1,  h_input->GetEntries()/25); 
  jpsi_pdf->SetParName(2, "Nevts (2S) Gauss"); jpsi_pdf->SetParameter(2,  h_input->GetEntries()/250); 
  jpsi_pdf->SetParName(3, "J/psi(1S) mass");  jpsi_pdf->SetParameter(3,  JPsi1SPDGMass); 
  jpsi_pdf->SetParName(4, "J/psi(2S) mass");  jpsi_pdf->SetParameter(4,  JPsi2SPDGMass); 
  jpsi_pdf->SetParName(5, "SigmaBroad");      jpsi_pdf->SetParameter(5,  0.100); jpsi_pdf->SetParLimits(5, 0.01, 5.0);//0.100 
  jpsi_pdf->SetParName(6, "SigmaNarrow");     jpsi_pdf->SetParameter(6,  0.060); jpsi_pdf->SetParLimits(6, 0.01, 3.0); //0.060
  jpsi_pdf->SetParName(7, "cb_alpha");        jpsi_pdf->SetParameter(7,  0.5); jpsi_pdf->SetParLimits(7, -3.0,  3.0); 
  jpsi_pdf->SetParName(8, "cb_n");            jpsi_pdf->SetParameter(8,  2.0); jpsi_pdf->SetParLimits(8,  0.1, 20.0); 
  jpsi_pdf->SetParName(9, "bkgrnd c0");       
  jpsi_pdf->SetParName(10, "bkgrnd c1");      
  jpsi_pdf->SetParName(11, "bkgrnd c2");      
  //

  // Fix crystal ball parameters
  //jpsi_pdf->ReleaseParameter(7); jpsi_pdf->SetParLimits(7, -3.0,  3.0); jpsi_pdf->SetParError(7, 0.2);
  //jpsi_pdf->ReleaseParameter(8); jpsi_pdf->SetParLimits(8,  0.1, 20.0); jpsi_pdf->SetParError(8, 0.5);
  jpsi_pdf->FixParameter(7, 0.); // not used 
  jpsi_pdf->FixParameter(8, 20.); // not used
 
  if (FitJPsiMass_isMC) { // m_isMC 
    // in the MC distribution, there is no J/psi(2s)
    jpsi_pdf->FixParameter(2, 0.);
    jpsi_pdf->FixParameter(4, JPsi2SPDGMass);
    // there is no background
    jpsi_pdf->FixParameter(9,  0.); 
    jpsi_pdf->FixParameter(10, 0.); 
    jpsi_pdf->FixParameter(11, 0.);
  }
  else {
    // Real data fit
    double pol2bkg_xmin = 3.9;
    double pol2bkg_xmax = x_max;
    std::cout << " ** JPsiMassFit ** FitJPsiMassDistribution ** fitting pol2 background in real data. Range: " 
	      << pol2bkg_xmin << " --> " << pol2bkg_xmax << std::endl;
    // fit the background in the upper range
    if (false) {
      h_input->Fit("pol2","Q","",4.0, x_max);
      // use the fit result as input for the jpsi fit
      jpsi_pdf->SetParameter( 9, h_input->GetFunction("pol2")->GetParameter(0)); 
      jpsi_pdf->SetParameter(10, h_input->GetFunction("pol2")->GetParameter(1));
      jpsi_pdf->SetParameter(11, h_input->GetFunction("pol2")->GetParameter(2));
    }
    if (true) {
      // fit the tail of the background with a straight line
      h_input->Fit("pol1","Q","",4.0, x_max);
      // estimate the 2nd order coefficient
      // use the fit result as input for the jpsi fit
      double* coeffs = this->EstimatePol2Coeff_JPsiFit(h_input);
      jpsi_pdf->FixParameter( 9, coeffs[0]); 
      jpsi_pdf->FixParameter(10, coeffs[1]); 
      jpsi_pdf->FixParameter(11, coeffs[2]); 
    }
    // pre-fit
    // with fixed mass parameters and background
    jpsi_pdf->FixParameter(3,  JPsi1SPDGMass);
    jpsi_pdf->FixParameter(4,  JPsi2SPDGMass); 
    //
    h_input->Fit("jpsi_pdf","Q"); 
    //
    // Release masses and refit
    jpsi_pdf->ReleaseParameter(3);
    jpsi_pdf->ReleaseParameter(4);
    h_input->Fit("jpsi_pdf","Q");
    // fix the shape and release background
    jpsi_pdf->FixParameter(0,jpsi_pdf->GetParameter(0)); 
    jpsi_pdf->FixParameter(1,jpsi_pdf->GetParameter(1)); 
    jpsi_pdf->FixParameter(2,jpsi_pdf->GetParameter(2)); 
    jpsi_pdf->FixParameter(3,jpsi_pdf->GetParameter(3)); 
    jpsi_pdf->FixParameter(4,jpsi_pdf->GetParameter(4)); 
    jpsi_pdf->FixParameter(5,jpsi_pdf->GetParameter(5)); 
    jpsi_pdf->FixParameter(6,jpsi_pdf->GetParameter(6)); 
    jpsi_pdf->ReleaseParameter(9); 
    jpsi_pdf->ReleaseParameter(10); 
    jpsi_pdf->ReleaseParameter(11);    
    h_input->Fit("jpsi_pdf","Q");
    // 
    // release everything
    jpsi_pdf->ReleaseParameter(0);
    jpsi_pdf->ReleaseParameter(1);
    jpsi_pdf->ReleaseParameter(2);
    jpsi_pdf->ReleaseParameter(3);
    jpsi_pdf->ReleaseParameter(4);
    jpsi_pdf->ReleaseParameter(5); jpsi_pdf->SetParLimits(5, 0.01, 10.0);  
    jpsi_pdf->ReleaseParameter(6); jpsi_pdf->SetParLimits(6, 0.01, 10.0); 
  }


  std::cout << std::endl << " ** JPsiMassFit ** FitJPsiMassDistribution ** fitting J/psi mass distribution" << std::endl;
  h_input->Fit("jpsi_pdf","Q");
  
  // sanity check
  bool refit = false;
  bool fix_N2s = false;
  bool fix_swapsigmas = false;
  bool fix_N1s = false;
  bool bad_chi2 = false;
  bool big_masserror = false;

  int n_refit = 0;

  while (n_refit == 0 || (n_refit > 0 && refit && n_refit < 10)) {
    refit = false;
    n_refit++;

    if (jpsi_pdf->GetParError(3)> 0.200) {refit = true; big_masserror = true;}
    if (!FitJPsiMass_isMC && jpsi_pdf->GetParameter(2) < 0) {refit = true; fix_N2s= true;} //!m_isMC
    // if (!m_isMC && jpsi_pdf->GetParameter(2) < (jpsi_pdf->GetParameter(0)+jpsi_pdf->GetParameter(1))/50 ) {refit = true; fix_N2s = true;}
    // if (jpsi_pdf->GetParameter(6) > jpsi_pdf->GetParameter(5)) {refit = true; fix_swapsigmas = true;}
    // if (jpsi_pdf->GetParameter(0) > 20*jpsi_pdf->GetParameter(1) || jpsi_pdf->GetParameter(0) < 20*jpsi_pdf->GetParameter(1)) {refit = true; fix_N1s = true;}
    if (jpsi_pdf->GetChisquare() / jpsi_pdf->GetNDF() > 10) {refit = true; bad_chi2 = true;}
    //refit = false;
    std::cout << " ** JPsiMassFit ** FitJPsiMassDistribution **  n_refit = " << n_refit << "  refit = " << refit << std::endl;
    
    if (refit) {
      if (m_printLevel >= 0) std::cout << " ** JPsiMassFit ** FitJPsiMassDistribution ** J/psi mass distribution fit. Sanity check failed :( " << std::endl;
      // sanity check failed
      // first thing reset the masses
      jpsi_pdf->SetParameter(3,  JPsi1SPDGMass);
      jpsi_pdf->SetParameter(4,  JPsi2SPDGMass); 
      if (fix_N2s) jpsi_pdf->SetParameter(2,  h_input->GetEntries()/20);
      if (fix_swapsigmas) {
	float sigma1 = jpsi_pdf->GetParameter(5); 
	float sigma2 = jpsi_pdf->GetParameter(6); 
	jpsi_pdf->SetParameter(5, sigma2);
	jpsi_pdf->SetParameter(6, sigma1);
      }
      if (fix_N1s) {
	float newN1S = (jpsi_pdf->GetParameter(0) + jpsi_pdf->GetParameter(1))/2;
	jpsi_pdf->SetParameter(0,  newN1S);
	jpsi_pdf->SetParameter(1,  newN1S);
      }
      if (bad_chi2 || big_masserror) {
	// reset the sigma broad and narrow
	jpsi_pdf->SetParameter(5,  0.100); 
	jpsi_pdf->SetParameter(6,  0.060); 

	if (m_printLevel >= 1) {
	  std::cout << " ** JPsiMassFit::FitJPsiMassDistribution ** bad_chi2 = " << bad_chi2 << " big_masserror = " << big_masserror << " ** Press key to continue: " << std::endl;
	  std::cin.get();
	}
	// fix the masses
	jpsi_pdf->FixParameter(3,  JPsi1SPDGMass);
	jpsi_pdf->FixParameter(4,  JPsi2SPDGMass); 
	// test fit with fixed masses
	h_input->Fit("jpsi_pdf","Q");
	// release masses
	jpsi_pdf->ReleaseParameter(3);
	if (!FitJPsiMass_isMC) jpsi_pdf->ReleaseParameter(4);
	// and fix background
	jpsi_pdf->FixParameter( 9, jpsi_pdf->GetParameter(9));
	jpsi_pdf->FixParameter(10, jpsi_pdf->GetParameter(10));
	jpsi_pdf->FixParameter(11, jpsi_pdf->GetParameter(11));
      }
      
      // refit
      h_input->Fit("jpsi_pdf","Q");
      TString theFitStatus(gMinuit->fCstatu.Data());
      std::cout << " ** gMinuit ** " << gMinuit->fCstatu.Data() << std::endl;
      if (theFitStatus.Index("FAILED") > -1) {
	std::cout << " ** JPsiMassFit::FitJPsiMassDistribution ** WARNING ** 2nd Fit failed --> refit with seek " << std::endl;      
	//gMinuit->mnscan();
	//gMinuit->mnprin(3,jpsi_pdf->GetChisquare());
	gMinuit->mnseek();
	gMinuit->mnprin(3,jpsi_pdf->GetChisquare());
	gMinuit->Migrad();
	gMinuit->mnprin(3,jpsi_pdf->GetChisquare());
	gMinuit->mnmnos();
	gMinuit->mnprin(3,jpsi_pdf->GetChisquare());
	if (m_printLevel >= 1) {
	  std::cout << " ** JPsiMassFit::FitJPsiMassDistribution ** conditioning before refitting completed " << " ** Press key to continue: " << std::endl;
	  std::cin.get();
	}
	h_input->Fit("jpsi_pdf","Q");
      } 
      if (m_printLevel >= 1) {
	std::cout << " ** JPsiMassFit::FitJPsiMassDistribution ** check results and press key to continue: " << std::endl;
	std::cin.get();
      }
    }
  }
  
  // draw the components
  TF1* jpsiBkgrnd = new TF1("jpsiBkgrnd","pol2", x_min, x_max);
  jpsiBkgrnd->SetParameter(0, jpsi_pdf->GetParameter(9));
  jpsiBkgrnd->SetParameter(1, jpsi_pdf->GetParameter(10));
  jpsiBkgrnd->SetParameter(2, jpsi_pdf->GetParameter(11));
  jpsiBkgrnd->SetLineWidth(2);
  jpsiBkgrnd->SetLineColor(kAzure-4);
  jpsiBkgrnd->SetLineStyle(kDashed);
  jpsiBkgrnd->Draw("same");


  //TF1* jpsi1sCB = new TF1("jpsi1sCB","crystalball", x_min, x_max);
  TF1* jpsi1sNarrowGauss = new TF1("jpsi1sNG","gaus", x_min, x_max);
  jpsi1sNarrowGauss->SetParameter(0, jpsi_pdf->GetParameter(0));
  jpsi1sNarrowGauss->SetParameter(1, jpsi_pdf->GetParameter(3));
  jpsi1sNarrowGauss->SetParameter(2, jpsi_pdf->GetParameter(6));
  //jpsi1sCB->SetParameter(3, jpsi_pdf->GetParameter(7));
  //jpsi1sCB->SetParameter(4, jpsi_pdf->GetParameter(8));
  jpsi1sNarrowGauss->SetLineWidth(2);
  jpsi1sNarrowGauss->SetLineColor(kGreen+1);
  jpsi1sNarrowGauss->Draw("same");

  TF1* jpsi1sGauss = new TF1("jpsi1sGauss","gaus", x_min, x_max);
  jpsi1sGauss->SetParameter(0, jpsi_pdf->GetParameter(1));
  jpsi1sGauss->SetParameter(1, jpsi_pdf->GetParameter(3));
  jpsi1sGauss->SetParameter(2, jpsi_pdf->GetParameter(5));
  jpsi1sGauss->SetLineWidth(2);
  jpsi1sGauss->SetLineColor(kOrange+1);
  jpsi1sGauss->Draw("same");

  TF1* jpsi2s = new TF1("jpsi2s","gaus", x_min, x_max);
  jpsi2s->SetParameter(0, jpsi_pdf->GetParameter(2));
  jpsi2s->SetParameter(1, jpsi_pdf->GetParameter(4));
  jpsi2s->SetParameter(2, jpsi_pdf->GetParameter(5));
  jpsi2s->SetLineWidth(2);
  jpsi2s->SetLineColor(kOrange+8);
  jpsi2s->Draw("same");

  // redraw the fitted function
  jpsi_pdf->Draw("same");
  std::cout << " ** JPsiMassFit ** FitJPsiMassDistribution ** chi2:" << jpsi_pdf->GetChisquare() << std::endl;
  std::cout << " ** JPsiMassFit ** FitJPsiMassDistribution ** ndf :" << jpsi_pdf->GetNDF() << std::endl;

  // Add text with result
  float xText= 0.60;
  float yText= 0.72;
  float yGap = 0.040;
  TLatex textOnThePlot; 
  textOnThePlot.SetNDC(kTRUE);
  textOnThePlot.SetTextColor(kGray+2);
  textOnThePlot.SetTextSize(0.03);
  TString theResultText;
  
  theResultText.Clear();
  theResultText.Form("m_{J/#psi} = %5.1f #pm %5.1f MeV", JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParameter(3), JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParError(3));
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());

  if (!FitJPsiMass_isMC) {
  theResultText.Clear();
  theResultText.Form("m_{#psi 2s} = %5.1f #pm %5.1f MeV", JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParameter(4), JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParError(4));
  yText = yText - yGap;
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
  }

  theResultText.Clear();
  theResultText.Form("#sigma_{N} = %5.1f #pm %5.1f MeV", JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParameter(6), JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParError(6));
  yText = yText - yGap;
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());

  theResultText.Clear();
  theResultText.Form("#sigma_{B} = %5.1f #pm %5.1f MeV", JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParameter(5), JPsiMassFit::GeVtoMeV*jpsi_pdf->GetParError(5));
  yText = yText - yGap;
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
  
  theResultText.Clear();
  theResultText.Form("N_{N} = %5.0f #pm %1.0f", jpsi_pdf->GetParameter(0), jpsi_pdf->GetParError(0));
  yText = yText - yGap;
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());

  theResultText.Clear();
  theResultText.Form("N_{B} = %5.0f #pm %1.0f", jpsi_pdf->GetParameter(1), jpsi_pdf->GetParError(1));
  yText = yText - yGap;
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());

  if (!FitJPsiMass_isMC) {
  theResultText.Clear();
  theResultText.Form("N_{2s} = %5.0f #pm %1.0f", jpsi_pdf->GetParameter(2), jpsi_pdf->GetParError(2));
  yText = yText - yGap;
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
  }

  theResultText.Clear();
  theResultText.Form("#chi^{2}/ndof = %5.1f / %d = %5.2f", jpsi_pdf->GetChisquare(), jpsi_pdf->GetNDF(), jpsi_pdf->GetChisquare()/jpsi_pdf->GetNDF());
  yText = yText - yGap;
  textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
  

  gPad->Update();

  // chi2
  m_chi2overndf = jpsi_pdf->GetChisquare() / jpsi_pdf->GetNDF() ;
  
  // store fit values
  double thismass = jpsi_pdf->GetParameter(3);
  m_massError = jpsi_pdf->GetParError(3);
 
  //m_fitSigma = jpsi_pdf->GetParameter(5);
  double sigmaG_2 = pow(jpsi_pdf->GetParameter(5),2);
  double sigmaCB_2 = pow(jpsi_pdf->GetParameter(6),2);
  double weightG =  jpsi_pdf->GetParameter(1) / (jpsi_pdf->GetParameter(1) + jpsi_pdf->GetParameter(0)); 
  double weightCB = jpsi_pdf->GetParameter(0) / (jpsi_pdf->GetParameter(1) + jpsi_pdf->GetParameter(0)); 
  m_fitSigma = sqrt(  weightG * sigmaG_2 + weightCB * sigmaCB_2);


  return thismass;
}

//////////////////////////////////////////////
double JPsiMassFit::FitKShortMassDistribution (TH1 * h_input)
{
  // Create a binned dataset that imports contents of TH1 and associates its contents to observable 'x'
  // RooDataHist dh_Mass_Pos("dh_Mass_Pos","dh JPsi Mass Pos",Mass_Pos,Import(*h_input)) ;

  double x_min = h_input->GetXaxis()->GetXmin(); //if ( x_min < 2.) x_min = 2.;
  double x_max = h_input->GetXaxis()->GetXmax();

  RooRealVar  x("x", h_input->GetTitle(), x_min, x_max) ;
  RooDataHist inv_mass_data("inv_mass_data","dimuon invariant mass",x, h_input) ;
  inv_mass_data.weightError(RooAbsData::SumW2);

  //crosscheck to check entries
  for (int i=0;i<inv_mass_data.numEntries();i++){
    //inv_mass_data.get(i);
    cout << " Bin " << i << " x="<<inv_mass_data.get(i)->getRealValue("x")
   << "  Weight="<<inv_mass_data.weight()
   << "  Weight Error= "<<inv_mass_data.weightError(RooAbsData::SumW2)<<endl;
  }

  // x.setRange("KshortPeak",KshortPDGMass-0.54,KshortPDGMass+0.5) ;
  // x.setRange("BackgroundLowMass", x_min, KshortPDGMass-0.5) ;
  // x.setRange("BackgroundHighMass", KshortPDGMass+1., x_max) ;
  x.setRange("TotalRange",x_min,  x_max);



  RooRealVar mean("mean","mean",0.48,0.52);
  RooRealVar sigma1("sigma1","sigma1",0.001,0.1);
  RooRealVar sigma2("sigma2","sigma2",0.001,0.1);

  RooGaussian sig1("sig1","sig1",x,mean,sigma1);
  RooGaussian sig2("sig2","sig2",x,mean,sigma2);
  RooRealVar sig1_yield("sig1_yield","sig1_yield",h_input->GetEntries()/2,0.,10000000.);
  RooRealVar sig2_yield("sig2_yield","sig2_yield",h_input->GetEntries()/2,0.,10000000.);

  RooRealVar bkg_yield("bkg_yield","bkg_yield",h_input->GetEntries()/10,0.,10000000.);

  RooRealVar c("c","c",-10,100);
  RooExponential bkg("bkg","bkg",x,c);


  RooAddPdf total_sum("total_sum","gauss1+gauss2+exp",RooArgList(sig1,sig2,bkg),RooArgList(sig1_yield,sig2_yield,bkg_yield));

  // fit depends on data/MC
  if (m_isMC) {
    total_sum.fitTo(inv_mass_data);

  }
  else {// real data
    total_sum.fitTo(inv_mass_data);
  }

  // drawing
  RooPlot* Mass_frame = x.frame(RooFit::Title(h_input->GetTitle()));
  Mass_frame->SetXTitle(h_input->GetXaxis()->GetTitle());
  inv_mass_data.plotOn(Mass_frame, RooFit::DataError(RooAbsData::SumW2));

  if(m_isMC){
    total_sum.plotOn(Mass_frame);
  }
  else{
    total_sum.plotOn(Mass_frame,RooFit::Components(bkg), RooFit::LineStyle(kDashed));

    RooArgSet argset(sig1,sig2);
    total_sum.plotOn(Mass_frame,RooFit::Components(argset), RooFit::LineColor(kGreen+1));
    total_sum.plotOn(Mass_frame);
  }

  Mass_frame->Draw();

  gPad->Update();

  // compute the signal to noise ratio
  float nSignal = sig1_yield.getVal() + sig2_yield.getVal();
  float nBackground = bkg_yield.getVal();
  std::cout << " ** JPsi fit ** signal events = " << nSignal 
      << "   Background events: " << nBackground 
      << "   S/B: " << nSignal / nBackground 
      << "   S/Sqrt(B): " << nSignal / sqrt(nBackground) 
      << std::endl;

  m_fitSigma = fabs(sigma1.getVal()); 
  m_massError = mean.getError();
  return mean.getVal();
}

//////////////////
double JPsiMassFit::FitZMassDistribution (TH1 * h_input)
{
  // First attempt to fit Z mass distribution. To run this function, first load the RooEggePdf.cxx: 
  // .L RooEggePdf.cxx +
  // Then load run_ZmumuValidationExample.C and run it fixing the m_analysisMode to m_analysisMode(JPsiMassFit::ZMASS) using either a ZSAMPLE or JPSISAMPLE.
  
    if (m_printLevel >= 0) std::cout << " ** JPsiMass_Fit ** FitZMassdistribution ** input histogram: " << h_input->GetTitle() << std::endl; 
    double x_min = h_input->GetXaxis()->GetXmin(); if ( x_min < 70.) x_min = 70.;
    double x_max = h_input->GetXaxis()->GetXmax(); if ( x_max < 110.) x_max = 110.;
    
    RooRealVar  x("x", h_input->GetTitle(), x_min, x_max) ;
    RooDataHist inv_mass_data("inv_mass_data","dimuon invariant mass",x, h_input) ;
    
    x.setRange("ZPeakRange",ZPDGMass-20.0,ZPDGMass+20.0) ;
    //x.setRange("ZPeakRange",ZPDGMass-18.0,ZPDGMass+14.0) ;
    x.setRange("ZPeakRangeNarrow",ZPDGMass-8.0,ZPDGMass+8.0) ;
    x.setRange("TotalRange",x_min,  x_max);
    
    // Breit Wigner coefficients encoded in RooEggePdf.cxx
    RooRealVar A("A","A", 6.61); A.setConstant(kFALSE);
    RooRealVar B("B","B", -2.90); B.setConstant(kFALSE); // -3.05
    RooRealVar C("C","C", 1.); C.setConstant(kTRUE);
    
    // Mean and width used in RooEggePdf.cxx
    RooRealVar z_mass("z_mass", "Z mass (mean of RooEggePdf)" , ZPDGMass, ZPDGMass-5.0,ZPDGMass+5.0, "GeV");
    RooRealVar z_width("z_width","Z Width",  2.4952); //2.4952 this is the Z BreitWigner Gamma (width) from PDG
    
    //Function associated
    RooEggePdf z_theoshape( "z_theoshape", "Z theoretical shape",x, A, B, C, z_mass, z_width) ;
      
    // Gaussian to mimic the detector resolution effects around the Z peak  
    /*RooRealVar resol_bias("resol_bias", "Mu of Gaussian" , 0.) ; resol_bias.setConstant(kTRUE);
    RooRealVar resol_sigma("resol_sigma","Sigma of Gaussian", 1., -10., 10.) ;
    RooGaussian resol_gauss("resol_gauss", "gauss(x,resol_bias, resol_sigma)", x, resol_bias, resol_sigma) ;*/

    RooRealVar mu("mu", "Mu of Gaussian" , 0);//, -5., 5.) ;
    RooRealVar sigma("sigma","Sigma of Gaussian", 1.65, 0.5, 5., "GeV") ;
    RooGaussModel gauss("gauss", "gauss(x,mean,sigma)", x, mu, sigma) ;

    RooRealVar sigma2oversigma1("sigma2oversigma1","",2.);//
    RooFormulaVar sigma2("sigma2","Sigma of Gaussian", "sigma2oversigma1*sigma",RooArgList(sigma2oversigma1,sigma)); 
    RooGaussModel gauss2("gauss2", "gauss2(x,mean,sigma2)", x, mu, sigma2) ;

    RooRealVar gaussFrac("gaussFrac","Gauss Fraction", 0.65);//, 0., 1.);
    RooAddModel dGauss("dGauss","gauss+gauss2", RooArgList(gauss, gauss2), gaussFrac);

    // Convolute the signal with the resolution
    RooFFTConvPdf model("model","truth (X) gaussian", x, z_theoshape, dGauss);

    // linear background
    RooRealVar bkg_c1 ("bkg_c1","bkg_c1", -1.); bkg_c1.setConstant(kFALSE);
    RooPolynomial bkg_poly ("bkg", "background", x, RooArgList(bkg_c1) );
    RooRealVar nsig ("nsig","nsig",  0.99); nsig.setConstant(kFALSE);
    RooAddPdf theShape ("theShape", "Zlineshape + background", RooArgList(model, bkg_poly), nsig);

    // Convolute the signal with the resolution
    //RooFFTConvPdf model("model","truth (X) gaussian", x, z_theoshape, resol_gauss);
    //RooFFTConvPdf model("model","truth (X) gaussian", x, z_theoshape, tGauss);
    
    //fit to Z line shape
    //model.fitTo(inv_mass_data, RooFit::Range("ZPeakRange"));
    theShape.fitTo(inv_mass_data, RooFit::Range("ZPeakRange"));

   
    /*RooChi2Var Chi2Fit ("chi2","chi2", model, inv_mass_data, RooFit::Range("ZPeakRange"), RooFit::DataError(RooAbsData::SumW2));
    RooMinuit mfitchi2(Chi2Fit);
    mfitchi2.simplex();
    mfitchi2.migrad();
    //mfitchi2.improve();
    //mfitchi2.minos();
    RooFitResult* Result = mfitchi2.save();
    Result->Print();
    Result->correlationMatrix().Print();*/

    // drawing
    RooPlot* Mass_frame = x.frame(RooFit::Title(h_input->GetTitle()));
    Mass_frame->SetXTitle(h_input->GetXaxis()->GetTitle());
    inv_mass_data.plotOn(Mass_frame);
    model.plotOn(Mass_frame, RooFit::Components(dGauss), RooFit::LineColor(kGreen));
    model.plotOn(Mass_frame, RooFit::Components(z_theoshape), RooFit::LineColor(kRed));
    model.plotOn(Mass_frame);// RooFit::LineColor(kBlue)); 
    Mass_frame->Draw();
    
    if (true) {
      //std::cout << " cb_alpha: -- ZZZ --- " << std::endl;
      //char input;
      //cin>>input;
      
      // Add text with result
      float xText= 0.60;
      float yText= 0.72;
      float yGap = 0.040;
      TLatex textOnThePlot; 
      textOnThePlot.SetNDC(kTRUE);
      textOnThePlot.SetTextColor(kGray+2);
      textOnThePlot.SetTextSize(0.03);
      TString theResultText;
      
      theResultText.Clear();
      theResultText.Form("m_{Z} = %5.3f #pm %5.3f GeV", z_mass.getVal(), z_mass.getError());
      textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
      
      theResultText.Clear();
      theResultText.Form("#sigma_{R} = %5.3f #pm %5.3f GeV", sigma.getVal(), sigma.getError());
      yText = yText - yGap;
      textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
    }
    gPad->Update();

    m_massError = z_mass.getError();
    m_fitSigma = fabs(sigma.getVal());
    return z_mass.getVal();
}
//////////////////
double JPsiMassFit::NEW_FitZMassDistribution (TH1 * h_input, float inputSigma, float narrowGW, float sigmaWF, float theAzls, float theBzls)
{
  if (m_printLevel >= 0) std::cout << " ** JPsiMass_Fit ** FitZMassdistribution ** input histogram: " << h_input->GetTitle() << std::endl; 

  double x_min = h_input->GetXaxis()->GetXmin(); if ( x_min < 60.) x_min = 60.;
  double x_max = h_input->GetXaxis()->GetXmax(); if ( x_max < 120.) x_max = 120.;
  
  RooRealVar  x ("x", h_input->GetTitle(), x_min, x_max); // the variable 
  x.setRange("ZPeakRange", ZPDGMass-20.0, ZPDGMass+18.0); //x_min, x_max); //
  x.setRange("ZNarrowRange", ZPDGMass-4.0, ZPDGMass+4.0); 
  RooDataHist inv_mass_data("inv_mass_data", "dimuon invariant mass",x, h_input);


  // Z Lineshape coefficients as encoded in RooEggePdf.cxx
  RooRealVar lineshapeA ("lineshapeA", "A zls", theAzls, 0., 100.); lineshapeA.setConstant(kFALSE); // 6.61
  RooRealVar lineshapeB ("lineShapeB", "B zls", theBzls); lineshapeB.setConstant(kFALSE); // -3.05
  RooRealVar lineshapeC ("lineshapeC", "C zls", 1.00); lineshapeC.setConstant(kTRUE);

  // Z mass and Width (Mean and width) used in RooEggePdf.cxx
  RooRealVar z_mass ("z_mass", "Z mass", ZPDGMass, ZPDGMass - 4.0,ZPDGMass + 4.0, "GeV");
  RooRealVar z_width ("z_width", "Z Width",  2.4952); //2.4952 this is the Z BreitWigner Gamma (width) from PDG
  RooRealVar zpdgmass ("zpdgmass", "Z PDG mass", ZPDGMass);

  //probability density function for the lineshape
  RooEggePdf z_lineshape( "z_lineshape", "Z lineshape", x, lineshapeA, lineshapeB, lineshapeC, z_mass, z_width) ;

  // Gaussian resolution
  RooRealVar g_mean ("g_mean", "mass bias", 0., -100., 100.); 
  RooRealVar g_sigma ("g_sigma","sigma narrow", inputSigma, 0. ,8.); 
  RooGaussian gaussiana ("gaussiana","G(x,mu,sigma)", x, g_mean, g_sigma);
   
  RooRealVar sigma_wide_factor ("sigma_wide_factor", "sigma_wide_factor", sigmaWF, 1.0, 5.); sigma_wide_factor.setConstant(kTRUE);
  RooFormulaVar  g_sigma_wide ("sigma_wide","@0*@1", RooArgList(sigma_wide_factor, g_sigma));
  RooRealVar narrowGWeight ("narrowGWeight", "Narrow Gauss weight", narrowGW, 0.50, 0.99);
  RooGaussian broadGaussian ("broadGaussiana","broad G(x,mu,sigma)", x, g_mean, g_sigma_wide);
  RooAddModel  doubleGaussian ("doubleGaussian", "Double Gaussuian", RooArgList(gaussiana, broadGaussian), narrowGWeight);
  bool usingWeightOfGaussians = false;

  // Convolute the signal with the resolution
  RooFFTConvPdf model("model", "Z lineshape (X) gaussian", x, z_lineshape, gaussiana);
  //RooFFTConvPdf model("model", "Z lineshape (X) gaussian", x, z_lineshape, doubleGaussian); usingWeightOfGaussians = true;

  // linear background
  RooRealVar bkg_c1 ("bkg_c1", "bkg_c1", 0., -1000, 0.001); bkg_c1.setConstant(kFALSE);
  RooPolynomial bkg_poly ("bkg", "background", x, RooArgList(bkg_c1) );

  // Add a background
  RooRealVar nsig ("nsig","nsig",  0.95); nsig.setConstant(kFALSE);
  RooAddPdf theShape ("theShape", "Zlineshape + background", RooArgList(model, bkg_poly), nsig);

  // fit the peak to the gaussian --> extract first estimate of sigma
  g_mean = ZPDGMass;
  gaussiana.fitTo(inv_mass_data, RooFit::Range("ZNarrowRange"));
  z_mass = g_mean.getVal(); // use the result as input for the lineshape
  g_mean = 0; g_mean.setConstant(kTRUE); // reset the bias

  // Fit the distribution
  // first pass: fix sigma 
  g_sigma.setConstant(kTRUE);
  model.fitTo(inv_mass_data, RooFit::Range("ZPeakRange"));
  // second pass: release sigma
  g_sigma.setConstant(kFALSE);
  model.fitTo(inv_mass_data, RooFit::Range("ZPeakRange"));

  // freeze the lineshape and fit the background
  //lineshapeA.setConstant(kTRUE);
  //lineshapeB.setConstant(kTRUE);
  //theShape.fitTo(inv_mass_data, RooFit::Range("ZPeakRange"));

  // release all params and refit
  //lineshapeA.setConstant(kFALSE);
  //lineshapeB.setConstant(kFALSE);
  //theShape.fitTo(inv_mass_data, RooFit::Range("ZPeakRange"));

  /////////////////
  // plot results
  ////////////////
  RooPlot* Mass_frame = x.frame(RooFit::Title(h_input->GetTitle()));
  Mass_frame->SetXTitle(h_input->GetXaxis()->GetTitle());
  inv_mass_data.plotOn(Mass_frame);
  //z_lineshape.plotOn(Mass_frame,RooFit::LineColor(kRed+1));
  //theShape.plotOn(Mass_frame,RooFit::Components(model),RooFit::LineColor(kGreen+2), RooFit::NormRange("ZPeakRange"), RooFit::Range("full"));
  //theShape.plotOn(Mass_frame,RooFit::Components(bkg_poly),RooFit::LineColor(kRed+2), RooFit::NormRange("ZPeakRange"), RooFit::Range("full"));
  //theShape.plotOn(Mass_frame);
  //theShape.plotOn(Mass_frame, RooFit::NormRange("ZPeakRange"), RooFit::Range("full"));
  //model.plotOn(Mass_frame);
  model.plotOn(Mass_frame, RooFit::NormRange("ZPeakRange"), RooFit::Range("ZPeakRange"));

  //  w.pdf('model').plotOn(frame, RooFit.NormRange("full"),RooFit.Range("full"))

  //g_mean = ZPDGMass;
  //gaussiana.plotOn(Mass_frame,RooFit::LineColor(kRed+1));
  //gaussiana_pura.plotOn(Mass_frame,RooFit::LineColor(kGreen+3));
  //broadGaussian.plotOn(Mass_frame,RooFit::LineColor(kGray+2));
  //doubleGaussian.plotOn(Mass_frame,RooFit::LineColor(kOrange+1));
  Mass_frame->Draw();

  //////////////////
  // print values
  ///////////////////
  if (true) {
    //std::cout << " cb_alpha: -- ZZZ --- " << std::endl;
    //char input;
    //cin>>input;
    
    // Add text with result
    float xText= 0.60;
    float yText= 0.80;
    float yGap = 0.037;
    TLatex textOnThePlot; 
    textOnThePlot.SetNDC(kTRUE);
    textOnThePlot.SetTextColor(kGray+2);
    textOnThePlot.SetTextSize(0.03);
    TString theResultText;
    
    theResultText.Clear();
    theResultText.Form("m_{Z} = %5.3f #pm %5.3f GeV", z_mass.getVal(), z_mass.getError());
    textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
    
    theResultText.Clear();
    theResultText.Form("#sigma_{R} = %5.3f #pm %5.3f GeV", g_sigma.getVal(), g_sigma.getError());
    //theResultText.Form("#sigma_{0} = %5.3f #pm %5.3f GeV", fabs(linear_sigma_c0.getVal()), linear_sigma_c0.getError());
    yText = yText - yGap;
    textOnThePlot.DrawLatex( xText, yText,theResultText.Data());

    //theResultText.Form("#sigma_{1} = %5.3f #pm %5.3f", linear_sigma_c1.getVal(), linear_sigma_c1.getError());
    //yText = yText - yGap;
    //textOnThePlot.DrawLatex( xText, yText,theResultText.Data());

    if (usingWeightOfGaussians) {
      theResultText.Clear();
      theResultText.Form("W = %4.2f #pm %4.2f", narrowGWeight.getVal(), narrowGWeight.getError());
      yText = yText - yGap;
      textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
    }

    theResultText.Clear();
    theResultText.Form("A = %5.3f #pm %5.3f", lineshapeA.getVal(), lineshapeA.getError());
    yText = yText - yGap;
    textOnThePlot.DrawLatex( xText, yText,theResultText.Data());

    theResultText.Clear();
    theResultText.Form("B = %5.3f #pm %5.3f", lineshapeB.getVal(), lineshapeB.getError());
    yText = yText - yGap;
    textOnThePlot.DrawLatex( xText, yText,theResultText.Data());
  }
  gPad->Update();

  m_massError = z_mass.getError();
  //m_fitSigma = fabs(linear_sigma_c0.getVal());
  m_fitSigma = fabs(g_sigma.getVal());
  return z_mass.getVal();
}

//////////////////
double JPsiMassFit::FitUpsilonMassDistribution (TH1 * h_input)
{
  // Create a binned dataset that imports contents of TH1 and associates its contents to observable 'x'
  // RooDataHist dh_Mass_Pos("dh_Mass_Pos","dh JPsi Mass Pos",Mass_Pos,Import(*h_input)) ;

  double x_min = h_input->GetXaxis()->GetXmin(); if ( x_min < 5.) x_min =  5.;
  double x_max = h_input->GetXaxis()->GetXmax(); if ( x_max >14.) x_max = 14.;
      
  h_input->SetMinimum(0);
  // TF1* upsilon_pdf = new TF1("upsilon_pdf","[0]+[1]*x+[2]*x*x+[3]*exp(-0.5*((x-[4])/[5])*((x-[4])/[5]))+[6]*exp(-0.5*((x-[7])/[5])*((x-[7])/[5]))+[8]*exp(-0.5*((x-[9])/[5])*((x-[9])/[5]))",10);
  //TF1* upsilon_pdf = new TF1("upsilon_pdf", &JPsiMassFit::UpsilonFit, x_min, x_max, 10);
  TF1* upsilon_pdf = new TF1("upsilon_pdf", UpsilonFit, x_min, x_max, 10);
  upsilon_pdf->SetLineColor(kBlue);
  upsilon_pdf->SetParName(0, "bkgrnd c0");
  upsilon_pdf->SetParName(1, "bkgrnd c1");
  upsilon_pdf->SetParName(2, "bkgrnd c2");
  upsilon_pdf->SetParName(3, "Nevts Y(1S)"); upsilon_pdf->SetParLimits(3, 0., h_input->GetEntries()/2); 
  upsilon_pdf->SetParName(4, "Nevts Y(2S)"); upsilon_pdf->SetParLimits(4, 0., h_input->GetEntries()/2); 
  upsilon_pdf->SetParName(5, "Nevts Y(3S)"); upsilon_pdf->SetParLimits(5, 0., h_input->GetEntries()/2); 
  upsilon_pdf->SetParameter(6, Upsilon1SPDGMass); upsilon_pdf->SetParName(6, "Y(1S) mass"); 
  upsilon_pdf->SetParameter(7, Upsilon2SPDGMass); upsilon_pdf->SetParName(7, "Y(2S) mass"); 
  upsilon_pdf->SetParameter(8, Upsilon3SPDGMass); upsilon_pdf->SetParName(8, "Y(3S) mass"); 
  upsilon_pdf->SetParameter(9, 0.25); upsilon_pdf->SetParName(9, "Mass sigma"); 

  if (m_isMC) {
    // in the MC distribution, there is no J/psi(2s)
    upsilon_pdf->FixParameter(4, 0.);
    upsilon_pdf->FixParameter(5, 0.);
    upsilon_pdf->FixParameter(7, Upsilon2SPDGMass);
    upsilon_pdf->FixParameter(8, Upsilon3SPDGMass);

    // there is no background
    upsilon_pdf->FixParameter(0,  0.); 
    upsilon_pdf->FixParameter(1, 0.); 
    upsilon_pdf->FixParameter(2, 0.);
  }
  else {
    // first fit the polynomial background shape
    upsilon_pdf->FixParameter(6, Upsilon1SPDGMass);
    upsilon_pdf->FixParameter(7, Upsilon2SPDGMass);
    upsilon_pdf->FixParameter(8, Upsilon3SPDGMass);
    upsilon_pdf->FixParameter(9, 0.25);
    h_input->Fit("upsilon_pdf");
    
    // now release the mass parameters and fit
    upsilon_pdf->ReleaseParameter(6);
    upsilon_pdf->ReleaseParameter(7);
    upsilon_pdf->ReleaseParameter(8);
    upsilon_pdf->ReleaseParameter(9);
  }
  
  h_input->Fit("upsilon_pdf");

  //draw polynomial background 
  TF1* Bkgrnd = new TF1("Bkgrnd","pol2", x_min, x_max);
  Bkgrnd->SetParameter(0, upsilon_pdf->GetParameter(0));
  Bkgrnd->SetParameter(1, upsilon_pdf->GetParameter(1));
  Bkgrnd->SetParameter(2, upsilon_pdf->GetParameter(2));
  Bkgrnd->SetLineWidth(2);
  Bkgrnd->SetLineColor(kAzure-4);
  Bkgrnd->SetLineStyle(kDashed);
  Bkgrnd->Draw("same");

  // draw the 1s peak
  TF1* upsilonGauss = new TF1("upsilon1sGauss","gaus", x_min, x_max);
  upsilonGauss->SetParameter(0, upsilon_pdf->GetParameter(3));
  upsilonGauss->SetParameter(1, upsilon_pdf->GetParameter(6));
  upsilonGauss->SetParameter(2, upsilon_pdf->GetParameter(9));
  upsilonGauss->SetLineWidth(2);
  upsilonGauss->SetLineColor(kRed);
  upsilonGauss->Draw("same");

  //draw the 2s peak 
  TF1* upsilonGauss2s = new TF1("upsilon2sGauss","gaus", x_min, x_max);
  upsilonGauss2s->SetParameter(0, upsilon_pdf->GetParameter(4));
  upsilonGauss2s->SetParameter(1, upsilon_pdf->GetParameter(7));
  upsilonGauss2s->SetParameter(2, upsilon_pdf->GetParameter(9));
  upsilonGauss2s->SetLineWidth(2);
  upsilonGauss2s->SetLineColor(kGreen+1);
  upsilonGauss2s->Draw("same");

  //draw the 3s peak
  TF1* upsilonGauss3s = new TF1("upsilon3sGauss","gaus", x_min, x_max);
  upsilonGauss3s->SetParameter(0, upsilon_pdf->GetParameter(5));
  upsilonGauss3s->SetParameter(1, upsilon_pdf->GetParameter(8));
  upsilonGauss3s->SetParameter(2, upsilon_pdf->GetParameter(9));
  upsilonGauss3s->SetLineWidth(2);
  upsilonGauss3s->SetLineColor(kOrange+1);
  upsilonGauss3s->Draw("same");

  // redraw the fitted function
  upsilon_pdf->Draw("same");

  std::cout << " ** JPsiMassFit ** FitUpsilonMassDistribution ** chi2:" << upsilon_pdf->GetChisquare() << std::endl;
  std::cout << " ** JPsiMassFit ** FitUpsilonMassDistribution ** ndf :" << upsilon_pdf->GetNDF() << std::endl;

  

  // chi2
  m_chi2overndf = upsilon_pdf->GetChisquare() / upsilon_pdf->GetNDF() ;

  gPad->Update();
  
  

  double thismass = upsilon_pdf->GetParameter(6);
  m_massError = upsilon_pdf->GetParError(6);
  m_fitSigma = upsilon_pdf->GetParameter(9);

  return thismass;
}

//////////////////
/*double JPsiMassFit::FitZMassDistributionROOT (TH1 * h_input)
{
  double x_min = h_input->GetXaxis()->GetXmin(); if ( x_min < 70.) x_min =  70.;
  double x_max = h_input->GetXaxis()->GetXmax(); if ( x_max > 110.) x_max = 110.;
  h_input->SetMinimum(0);
 
  TF1* z_theoshape = new TF1("z_theoshape", RooEgge, x_min, x_max, 5);
  z_theoshape->SetLineColor(kBlack);
  z_theoshape->SetLineWidth(1);
  z_theoshape->SetParName(0, "A coefficient");    z_theoshape->SetParLimits(0,  6.61-10, 6.61+10); 
  z_theoshape->SetParName(1, "B coefficient"); z_theoshape->SetParLimits(1,  -3.05 -10, -3.05+10); 
  z_theoshape->SetParName(2, "C coefficient"); z_theoshape->SetParLimits(2,  1-10,1+10); 
  z_theoshape->SetParName(3, "Z mass");  z_theoshape->SetParLimits(3, ZPDGMass-10., ZPDGMass + 10.); 
  z_theoshape->SetParName(4, "Z PDG width");  z_theoshape->FixParameter(4, 2.4952); 

  
  TF1* z_resolgaus = new TF1("z_resolgaus", ZFit, x_min, x_max, 3);
  z_resolgaus->SetLineColor(kGreen);
  z_resolgaus->SetLineWidth(1);
  z_resolgaus->SetParName(0, "mu gaussian");    z_resolgaus->FixParameter(0, 0); 
  z_resolgaus->SetParName(1, "sigma gaussian "); z_resolgaus->SetParameter(1, 1);//-10, 10); 
  z_resolgaus->SetParName(2, "N evts gaussian"); z_resolgaus->FixParameter(2, 1);//h_input->GetEntries());

  TF1Convolution *f_conv = new TF1Convolution(z_theoshape,z_resolgaus,x_min,x_max,true); //not works for 20.7.3.8, but 20.7.5.7
  f_conv->SetRange(x_min,x_max);
  f_conv->SetNofPointsFFT(1000);
  TF1   *z_pdf = new TF1("z_pdf",*f_conv, x_min, x_max, f_conv->GetNpar());
  z_pdf->SetParName(0, "A coefficient");    z_pdf->SetParameter(0,  6.61); 
  z_pdf->SetParName(1, "B coefficient"); z_pdf->SetParameter(1,  -3.05); 
  z_pdf->SetParName(2, "C coefficient"); z_pdf->SetParameter(2,  1); 
  z_pdf->SetParName(3, "Z mass");  z_pdf->SetParLimits(3,  ZPDGMass-10., ZPDGMass+10.); 
  z_pdf->SetParName(4, "Z PDG width");  z_pdf->FixParameter(4, 2.4952); 
  z_pdf->SetParName(5, "mu gaussian");    z_pdf->FixParameter(5, 0); 
  z_pdf->SetParName(6, "sigma gaussian "); z_pdf->SetParameter(6, 1);//-10, 10); 
  z_pdf->SetParName(7, "N evts gaussian"); z_pdf->FixParameter(7, 1);//h_input->GetEntries());

  //fit
  std::cout << " ** JPsiMassFit ** FiZMassDistribution ** fitting Z mass distribution" << std::endl;
  //h_input->Fit("z_theoshape","","",ZPDGMass - 20., ZPDGMass + 20.);
  z_theoshape->Draw();
  z_resolgaus->Draw("same");
  z_pdf->Draw("same");
 
  std::cout << " ** JPsiMassFit ** FitZMassDistribution ** chi2:" << z_pdf->GetChisquare() << std::endl;
  std::cout << " ** JPsiMassFit ** FitZMassDistribution ** ndf :" << z_pdf->GetNDF() << std::endl;

  gPad->Update();

  // chi2
  m_chi2overndf = z_pdf->GetChisquare() / z_pdf->GetNDF() ;
  
  // store fit values
  double thismass = z_pdf->GetParameter(3);
  m_massError = z_pdf->GetParError(3);
 
  m_fitSigma = z_pdf->GetParameter(6);


  return thismass;
}*/

//////////////////
void JPsiMassFit::SetAnalysisMode (int newmode) 
{
  // first: take some precautions in the event the user set some value outside range
  if (newmode < JPsiMassFit::ZMASS) m_analysisMode = JPsiMassFit::ZMASS;
  if (newmode > JPsiMassFit::UPSILONMASS) m_analysisMode = JPsiMassFit::UPSILONMASS;

  // Now, set the new value
  m_analysisMode = newmode;

  if (m_printLevel >= 0) {
    std::cout << " ** JPsiMass_Fit ** SetAnalysisMode ** new mode: " << m_AnalysisModeKeyWord[m_analysisMode] 
	      << " (" << m_analysisMode << ") " << std::endl;
  }
  return;
}

//////////////////////
void JPsiMassFit::HistogramPreConditioning( TH1* h_input )
{
  for (int bin = 1 ; bin <= h_input->GetNbinsX(); bin++){
    if (h_input->GetBinError(bin) == 0) h_input->SetBinError(bin, 1.);
    //if (h_input->GetBinError(bin) < 512) h_input->SetBinError(bin, 512.);
  }
  return;
}

//////////////////////
//double JPsiMassFit::UpsilonFit(Double_t *v, Double_t *par)
double UpsilonFit(Double_t *v, Double_t *par)
{
  Double_t arg = 0;
  const double Upsilon1SPDGMass = 9.46030;
  const double Upsilon2SPDGMass = 10.02326;
  const double Upsilon3SPDGMass = 10.3552;

  Double_t fitval = 0.;
  
  double thismass = v[0];

  // fit parameters
  double n_1s = par[3];
  double n_2s = par[4];
  double n_3s = par[5];
  double m_1s = par[6];
  double m_2s = par[7];
  double m_3s = par[8];
  double sigma= par[9];

  double background = par[0] + par[1] * thismass + par[2]*pow(thismass,2);

  double gauss_1s = 0, gauss_2s = 0, gauss_3s = 0;
  if (sigma != 0) {
    arg = (thismass - m_1s)/sigma; 
    gauss_1s = n_1s * TMath::Exp(-0.5*arg*arg); 

    arg = (thismass - m_2s)/sigma; 
    gauss_2s = n_2s * TMath::Exp(-0.5*arg*arg); 

    arg = (thismass - m_3s)/sigma; 
    gauss_3s = n_3s * TMath::Exp(-0.5*arg*arg); 
  }

  fitval = background + gauss_1s + gauss_2s + gauss_3s; 
  
  // penalty terms to ensure the proper order of masses and a gap
  if (m_1s > (m_2s-0.100)) {
    fitval += (10*pow(m_2s-m_1s, 2)+10);    
  }
 
  if (m_2s > (m_3s-0.100)) {
    fitval += (10*pow(m_3s-m_2s, 2)+10);
  }

  /*
  // check the results are not crazy
  if (fabs(m_1s - Upsilon1SPDGMass) > 0.200) {
    fitval += 100000*pow(m_1s - Upsilon1SPDGMass, 2);
  } 
  if (fabs(m_2s - Upsilon2SPDGMass) > 0.200) {
    fitval += 100000*pow(m_2s - Upsilon2SPDGMass, 2);
  } 
  if (fabs(m_3s - Upsilon3SPDGMass) > 0.200) {
    fitval += 100000*pow(m_3s - Upsilon3SPDGMass, 2);
  } 
  */

  
  // number of events (n_1s > n_2s > n_3s)
  if (n_1s < n_2s) {
    fitval += pow(n_1s - n_2s,2);
  }
  if (n_1s < 0) {
    fitval += pow(n_1s,2);
  }
  if (n_2s < 0) {
    fitval += pow(n_2s,2);
  }
  if (n_3s < 0) {
    fitval += pow(n_3s,2);
  }
  
  return fitval;
}

/////////////////////////////////////////////
double JPsiFit(Double_t *v, Double_t *par)
{
  const Double_t JPsi1SPDGMass = 3.096916; //PDG 2010 in GeV in GeV
  const Double_t JPsi2SPDGMass = 3.686109;

  Double_t arg = 0;
 
  Double_t fitval = 0.;
  double thismass = v[0];
  static int callcount = 0;
  callcount++;
  
  if (false && callcount % 1000 == 0) std::cout << " ** JPsiFit ** call count: " << callcount << std::endl; 
  // fit parameters
  double n_gausN_1s  = par[0];
  double n_gausB_1s = par[1];
  double n_gauss_2s = par[2];
  double m_1s       = par[3];
  double m_2s       = par[4];
  double sigmaBroad = par[5];
  double sigmaNarrow= par[6]; 
  double cb_alpha   = par[7]; 
  double cb_n       = par[8]; 
  double pol_c0     = par[9];
  double pol_c1     = par[10];
  double pol_c2     = par[11];
  
  // polynomial background
  double background = pol_c0 + pol_c1 * thismass + pol_c2 * pow(thismass,2);

  // Broad Gaussian part for J/psi 1s and Gaussian 2s --> use the same sigma (sigmaBroad)
  double gauss_1s_narrow= 0, gauss_1s_broad= 0, gauss_2s= 0;

  if (sigmaNarrow != 0) {
    arg = (thismass - m_1s)/sigmaNarrow; 
    gauss_1s_narrow = n_gausN_1s * TMath::Exp(-0.5*arg*arg); 
  } 
  if (sigmaBroad != 0) {
    arg = (thismass - m_1s)/sigmaBroad; 
    gauss_1s_broad = n_gausB_1s * TMath::Exp(-0.5*arg*arg); 

    arg = (thismass - m_2s)/sigmaBroad; 
    gauss_2s = n_gauss_2s * TMath::Exp(-0.5*arg*arg); 
  }
  
  fitval = background + gauss_1s_narrow + gauss_1s_broad + gauss_2s;
  
  // penalty terms to ensure the proper order of masses and a gap
  if (m_1s > (m_2s-0.100)) {
    fitval += 1000*pow(m_2s-m_1s, 2);    
  }
  //  if (m_1s < (JPsi1SPDGMass - 0.500)) { // NEW -- SALVA -- 11/03/17
  //  fitval += 1000*pow(m_1s, 2);    
  //}
  if (fabs(m_1s-JPsi1SPDGMass) > 0.500) { // NEW -- SALVA -- 13/03/17
    fitval += 1000*pow(m_1s, 2);    
  }

  // check that m_2s is not that far away from the real value (at least not larger than the m1s and m2s mass difference form PDG)
  if (fabs(m_2s - JPsi2SPDGMass) > ( JPsi2SPDGMass - JPsi1SPDGMass)/2 && true) {
    fitval += pow(10*(m_2s-JPsi2SPDGMass), 2);    
  } 

  if (n_gausN_1s < 0.) fitval += pow(10*(fabs(n_gausN_1s)+1), 2);
  if (n_gausB_1s < 0.) fitval += pow(10*(fabs(n_gausB_1s)+1), 2);
  if (n_gauss_2s < 0.) fitval += pow(10*(fabs(n_gauss_2s)+1), 2);
  if (n_gausN_1s < 1.05*n_gausB_1s) fitval += pow(10*(fabs(n_gausB_1s)+1), 2); // NEW -- SALVA -- 11/03/17
  if (n_gausN_1s < 0.05*n_gausB_1s) fitval += pow(10*(fabs(n_gausB_1s)+1), 2);
  if (n_gausB_1s < 0.05*n_gausN_1s) fitval += pow(10*(fabs(n_gausN_1s)+1), 2);
  if (n_gauss_2s < 0.03*(n_gausB_1s + n_gausN_1s) ) pow(10*(fabs(n_gauss_2s)+1), 2);
  

  //if (n_gauss_2s > -0.1 && n_gauss_2s < (n_gausN_1s+n_gausB_1s)/100 && true ) fitval += pow(10*(fabs(n_gauss_2s)+1),2);
  //if (n_gauss_2s > n_gausN_1s && false ) fitval += pow(10*n_gauss_2s, 2);
  //if (n_gauss_2s > 2*n_gausB_1s && false ) fitval += pow(10*n_gauss_2s, 2);

  if (sigmaBroad < 1.05*(sigmaNarrow) && true) fitval += pow(100*(sigmaBroad+sigmaNarrow),2);
  if (sigmaBroad > 3*sigmaNarrow && true)  fitval += pow(100*(sigmaBroad+sigmaNarrow),2);
  //if (sigmaBroad < 0 && true) fitval += pow(100*(sigmaBroad),2);

  // penalty term is background is negative
  if (background < 0 && true ) fitval += pow(100*(fabs(background)+1),2);

  // penalty term for linear term of background positive 
  if (pol_c1 > 0 && true) fitval += pow(10*(fabs(pol_c1)+1),2);
  if (pol_c0 < 0 && true) fitval += pow(10*(fabs(pol_c0)+1),2);
  if (pol_c2 != 0) { // minimum of the background 
    if (pol_c2 < 0 && true) fitval += pow(10*(fabs(pol_c2)+1),2);
    if ( -pow(pol_c1,2)/(4*pol_c2) + pol_c0 < 5 && false) fitval += pow(10*(fabs(pol_c0)+1),2);
  }

  // near the peak (the signal must dominate)
  if (fabs(thismass-JPsi1SPDGMass) < 0.2 && false) {
    if (background > gauss_1s_narrow) fitval += pow(100*background,2);
  }

  if (fabs(thismass-JPsi2SPDGMass)<0.1 && false) {
    if (background > 2*gauss_2s) fitval += pow(100*background,2);
  }


  // penalty term to keep the masses within the range
  // if (m_1s < m_massLowerValueForFit || m_1s > m_massUpperValueForFit ) fitval = pow(fitval,2);  

  return fitval;
}

/////////////////////////////////////////////
double ZFit(Double_t *v, Double_t *par)
{
  Double_t arg = 0;
 
  Double_t fitval = 0.;
  double thismass = v[0];
  static int callcount = 0;
  callcount++;
  
  if (false && callcount % 1000 == 0) std::cout << " ** JPsiFit ** call count: " << callcount << std::endl; 
  // fit parameters
  double m_Z     = par[0];
  double sigma   = par[1];
  double n_gaus  = par[2];

  if (sigma != 0) {
    arg = (thismass - m_Z)/sigma; 
    fitval = n_gaus * TMath::Exp(-0.5*arg*arg); 
  } 
  
  
  // penalty terms to ensure the proper order of masses and a gap
  if (n_gaus < 0.) fitval += pow(10*n_gaus,2);

  //std::cout << "ZFIT FITVAL: " << fitval << std::endl;

  return fitval;
}

/////////////////////////////////////////////
double RooEgge(Double_t *v, Double_t *par)
{
  Double_t arg = 0;
 
  Double_t fitval = 0.;
  double thismass = v[0];
  static int callcount = 0;
  callcount++;

  if (false && callcount % 1000 == 0) std::cout << " ** ZFit ** call count: " << callcount << std::endl; 
  // fit parameters
  double A  = par[0];
  double B  = par[1];
  double C  = par[2];
  double m_Z = par[3];
  double width_Z = par[4];
  
  //if (thismass < 0) return 0;

  fitval += A/(thismass*thismass+1) ;
  fitval += B*(thismass*thismass-m_Z*m_Z)/((thismass*thismass-m_Z*m_Z)*(thismass*thismass-m_Z*m_Z)+width_Z*width_Z*m_Z*m_Z) ;
  fitval += C*thismass*thismass/((thismass*thismass-m_Z*m_Z)*(thismass*thismass-m_Z*m_Z)+width_Z*width_Z*m_Z*m_Z) ;
 
  //std::cout << "ROOEGGE FITVAL: " << fitval << std::endl;  
  return fitval;
}

//////////////////////
double JPsiMassFit::GetTargetMass(int etabin, int phibin)
{
  bool debug = false;
  if (debug) std::cout << " ** JPsiMassFit::GetTargetMass(" << etabin << ", " << phibin << ") " << std::endl;
  double tempMass = this->GetReferenceMass();
  double mcbias = 0;
  double targetMass = tempMass + mcbias; 
  
  if (h_mcmassbiasmap != NULL) {
    if (debug) std::cout << " ** JPsiMassFit::GetTargetMass ** checking h_mcmassbiasmap histogram " << h_mcmassbiasmap << std::endl;
    // check the map has the right dimensions
    bool goodEtaBin = false;
    bool goodPhiBin = false;
    if (0 < etabin && etabin <=h_mcmassbiasmap->GetNbinsX()) goodEtaBin = true;
    if (0 < phibin && phibin <=h_mcmassbiasmap->GetNbinsY()) goodPhiBin = true;
    if (!goodEtaBin or !goodPhiBin) {
      etabin = 1;
      phibin = 1;
    }
    mcbias = h_mcmassbiasmap->GetBinContent(etabin,phibin);
  }
  targetMass = tempMass + mcbias;
  if (debug) std::cout << " ** JPsiMassFit::GetTargetMass(" << etabin << ", " << phibin << ") = " << targetMass << std::endl;
  
  
 /* std::cout << " ** JPsiMassFit::GetTargetMass ** ref mass: " << tempMass 
	    << "  MC bias( " << etabin << ", " << phibin << "): " << mcbias 
	    << "  Target Mass: " << targetMass << std::endl;*/
  
  return targetMass;
}

//////////////////////
void JPsiMassFit::SetInputHistogram (TH3 * newth3) 
{
  h_inv_mass_vs_etaphi = newth3;

  // the mass is in the Z axis: extract the range from there
  m_massLowerValueForFit = h_inv_mass_vs_etaphi->GetZaxis()->GetXmin();
  m_massUpperValueForFit = h_inv_mass_vs_etaphi->GetZaxis()->GetXmax();

  std::cout << " ** JPsiMassFit::SetInputHistogram ** lower and upper mass fit range: " <<  m_massLowerValueForFit 
	    << " --> " <<  m_massUpperValueForFit
	    << std::endl;
}


//////////////////////
double* JPsiMassFit::EstimatePol2Coeff_JPsiFit(TH1* h_input)
{
  double theCoeff[3];

  // take the first 2 bins:
  double xval = (h_input->GetBinCenter(1)  + h_input->GetBinCenter(2))/2;
  double yval = (h_input->GetBinContent(1) + h_input->GetBinContent(2))/2;

  TGraphErrors * theTestPoints = new TGraphErrors ();
  int pointIndex=0;
  theTestPoints->SetPoint(0,xval,yval);
  theTestPoints->SetPointError(0, h_input->GetBinWidth(1)/2, h_input->GetBinError(1)); 
  for (int ibin=1; ibin < h_input->GetNbinsX(); ibin++) {
    if (h_input->GetBinCenter(ibin)> 4.0) {
      pointIndex++;
      theTestPoints->SetPoint(pointIndex, h_input->GetBinCenter(ibin), h_input->GetBinContent(ibin)); 
      theTestPoints->SetPointError(pointIndex, h_input->GetBinWidth(1)/2, h_input->GetBinError(ibin)); 
    }
  }

  theTestPoints->Fit("pol2","Q");
  theCoeff[0] = theTestPoints->GetFunction("pol2")->GetParameter(0);
  theCoeff[1] = theTestPoints->GetFunction("pol2")->GetParameter(1);
  theCoeff[2] = theTestPoints->GetFunction("pol2")->GetParameter(2);

  if (m_printLevel >= 0) std::cout << std::endl 
				   << " **  JPsiMassFit::EstimatePol2Coeff_JPsiFit ** c0= " << theCoeff[0]
				   << "  c1= " << theCoeff[1]
				   << "  c1= " << theCoeff[2]
				   << std::endl;

  delete theTestPoints;

  return theCoeff;
}

//////////////////////
double JPsiMassFit::FindPeakValue (TH1 * h_input) 
{

  // extract the range from the maximum bin
  //int maximumBin = h_input->GetMaximumBin();
  int maximumBin = h_input->FindBin(JPsiMassFit::ZPDGMass);
  int massLowerBin = maximumBin - 4;
  int massUpperBin = maximumBin + 4;

  double x_lower = h_input->GetBinCenter(massLowerBin);
  double x_upper = h_input->GetBinCenter(massUpperBin);  

  h_input->Fit("gaus","Q","",x_lower, x_upper);

  double peakmass = h_input->GetFunction("gaus")->GetParameter(1);
  m_massError = h_input->GetFunction("gaus")->GetParError(1);
  m_fitSigma = h_input->GetFunction("gaus")->GetParameter(2);

  TLine lineFitVal;
  lineFitVal.SetLineColor(kRed);
  lineFitVal.SetLineWidth(2);
  lineFitVal.DrawLine(peakmass, 0., peakmass, h_input->GetMaximum());
  lineFitVal.SetLineColor(kGreen+1);
  lineFitVal.DrawLine(JPsiMassFit::ZPDGMass, 0., JPsiMassFit::ZPDGMass, h_input->GetMaximum());

  std::cout << " ** JPsiMassFit::FindPeakValue ** lower and upper mass fit range: " <<  x_lower 
	    << " --> " <<  x_upper
	    << std::endl;

  return peakmass;
}

//////////////////////
void JPsiMassFit::SetOutputFileName () 
{
  switch (m_analysisMode) {
  case JPsiMassFit::ZMASS:
    m_outputFileName.Prepend("Z_");
    break;
  case JPsiMassFit::JPSIMASS:
    m_outputFileName.Prepend("JPsi_");
    break;
  case JPsiMassFit::UPSILONMASS:
    m_outputFileName.Prepend("Upsilon_");
    break;
  case JPsiMassFit::KSHORTMASS:
    m_outputFileName.Prepend("Kshort_");
    break;
  default:
    m_outputFileName.Prepend("dimuon_");
  }
 

  return;
}
#endif 

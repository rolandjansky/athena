/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "egammaAnalysisUtils/BosonPtReweightingTool.h"
#include <TString.h>
#include <TMath.h>
#include <TF1.h>
#include <TROOT.h>
#include <TFile.h>


#include <iostream>
#include <sstream>
#include <string>
#include <map>

const double BosonPtReweightingTool::m_MassCenters[m_nMassBins] =
  {23.81, 34.13, 44.35, 54.56, 64.88, 75.57, 87.72, 92.11, 106.26, 131.37, 169.05};

//===============================================================================================================
BosonPtReweightingTool::BosonPtReweightingTool(std::string sourcein, std::string destinationin, 
					       ePtWeightType ptweight, std::string weightsFileName)
{
  m_impose_physical_pol_constraint = false;
  m_PtWeight = ptweight;

  m_h_Ptweights_Wplus  = 0;
  m_h_Ptweights_Wminus = 0;
  m_h_Ptweights_Z      = 0;

  m_h_Rapidityweights_Wplus  = 0;
  m_h_Rapidityweights_Wminus = 0;
  m_h_Rapidityweights_Z      = 0;

  m_h_Ptweights_Z_massbinned.clear();
  m_PtzCenters.clear();

  m_f_Ptweights_Z = 0;
  m_param_Ptweights_Z.clear();

  m_h_aimom_source.clear();
  m_h_aimom_target.clear();

  TFile *weightsFile = TFile::Open(weightsFileName.c_str());
  
  if ( !weightsFile ) {
    std::cout << "ERROR: could not open file with weights: " << weightsFileName << std::endl;
    return;
  } else {
    std::cout << "Successfully opened file with weights: " << weightsFileName << std::endl;
  }

  std::string source = MapMCName(sourcein);
  std::string destination = MapMCName(destinationin);
  std::cout << "BosonPtReweightingTool will reweight: " << source << " => " << destination << std::endl;

  // Load Boson pt 1D histos
  if (m_PtWeight == PeakWeight) {
    TH1 *sss, *ddd;
    sss = (TH1*) weightsFile->Get((source+"/ptwplus").c_str());
    ddd = (TH1*) weightsFile->Get((destination+"/ptwplus").c_str());
    if (sss && ddd) {
      m_h_Ptweights_Wplus  = (TH1*) ddd->Clone((source+"_ptwplus").c_str());
      m_h_Ptweights_Wplus->Divide(sss);
      m_h_Ptweights_Wplus->SetDirectory(gROOT);
    }
    
    sss = (TH1*) weightsFile->Get((source+"/ptwminus").c_str());
    ddd = (TH1*) weightsFile->Get((destination+"/ptwminus").c_str());
    if (sss && ddd) {
      m_h_Ptweights_Wminus  = (TH1*) ddd->Clone((source+"_ptwminus").c_str());
      m_h_Ptweights_Wminus->Divide(sss);
      m_h_Ptweights_Wminus->SetDirectory(gROOT);
    }
    
    sss = (TH1*) weightsFile->Get((source+"/ptz").c_str());
    ddd = (TH1*) weightsFile->Get((destination+"/ptz").c_str());
    if (sss && ddd) {
      m_h_Ptweights_Z  = (TH1*) ddd->Clone((source+"_ptz").c_str());
      m_h_Ptweights_Z->Divide(sss);
      m_h_Ptweights_Z->SetDirectory(gROOT);
    } else {
      // legacy fall-back possiblity
      m_h_Ptweights_Z  = (TH1*) weightsFile->Get(("legacy_"+source+"_to_"+destination+"_weights_Z").c_str());
      if (m_h_Ptweights_Z)
	m_h_Ptweights_Z->SetDirectory(gROOT);
    }
    if ( !m_h_Ptweights_Wplus )
      std::cout << "WARNING: could not get pt weights for W+" << std::endl;
    if ( !m_h_Ptweights_Wminus )
      std::cout << "WARNING: could not get pt weights for W-" << std::endl;
    if ( !m_h_Ptweights_Z )
      std::cout << "WARNING: could not get pt weights for Z" << std::endl;


    // repeat same for boson rapidity!

    sss = (TH1*) weightsFile->Get((source+"/ywplus").c_str());
    ddd = (TH1*) weightsFile->Get((destination+"/ywplus").c_str());
    if (sss && ddd) {
      sss->Rebin(4); ddd->Rebin(4);
      m_h_Rapidityweights_Wplus  = (TH1*) ddd->Clone((source+"_ywplus").c_str());
      m_h_Rapidityweights_Wplus->Divide(sss);
      m_h_Rapidityweights_Wplus->SetDirectory(gROOT);
    }
    
    sss = (TH1*) weightsFile->Get((source+"/ywminus").c_str());
    ddd = (TH1*) weightsFile->Get((destination+"/ywminus").c_str());
    if (sss && ddd) {
      sss->Rebin(4); ddd->Rebin(4);
      m_h_Rapidityweights_Wminus  = (TH1*) ddd->Clone((source+"_ywminus").c_str());
      m_h_Rapidityweights_Wminus->Divide(sss);
      m_h_Rapidityweights_Wminus->SetDirectory(gROOT);
    }
    
    sss = (TH1*) weightsFile->Get((source+"/yz").c_str());
    ddd = (TH1*) weightsFile->Get((destination+"/yz").c_str());
    if (sss && ddd) {
      sss->Rebin(4); ddd->Rebin(4);
      m_h_Rapidityweights_Z  = (TH1*) ddd->Clone((source+"_yz").c_str());
      m_h_Rapidityweights_Z->Divide(sss);
      m_h_Rapidityweights_Z->SetDirectory(gROOT);
    }

    if ( !m_h_Rapidityweights_Wplus )
      std::cout << "WARNING: could not get boson rapidity weights for W+" << std::endl;
    if ( !m_h_Rapidityweights_Wminus )
      std::cout << "WARNING: could not get boson rapidity weights for W-" << std::endl;
    if ( !m_h_Rapidityweights_Z )
      std::cout << "WARNING: could not get boson rapidity weights for Z" << std::endl;
  }

  if (m_PtWeight == MassBinnedWeight) {
    TH1 *sss, *ddd;
    // read the elements in reverse order, as some of the low mass slices may not be populated
    // in that case the best we can do is to reuse a higher mass bin
    // we also rebin the input by factor 2 and smooth the ratio twice
    for (int i = m_nMassBins-1; i >= 0; i--) {
      sss = (TH1*) weightsFile->Get(TString::Format("%s/ptz_mllslice_%i", source.c_str(), i));
      ddd = (TH1*) weightsFile->Get(TString::Format("%s/ptz_mllslice_%i", destination.c_str(), i));
      if (sss && ddd) {
	sss->Rebin(2);
	if (sss != ddd) // condition true, in case the user requested source==destination! 
	  ddd->Rebin(2);
	if (sss->GetEntries() == 0) {
	  std::cout << "INFO: no information for mass bin " << i << " (" << m_MassCenters[i]
		    << " GeV) for Generator " << source << std::endl;
	}

	if (ddd->GetEntries() == 0) {
	  std::cout << "INFO: no information for mass bin " << i << " (" << m_MassCenters[i]
		    << " GeV) for Generator " << destination << std::endl;
	}

	TH1 *tmp;
	tmp  = (TH1*) ddd->Clone(TString::Format("%s_ptz_mllslice_%i", source.c_str(), i));
	tmp->Divide(sss);
	tmp->Smooth(2);

	if (sss->GetEntries() == 0 || ddd->GetEntries() == 0) {
	  std::cout << "INFO: force boson pt weight to 1. for mass bin " << i << " (" << m_MassCenters[i]
		    << " GeV)" << std::endl;
	  for (int jj = 0; jj <= tmp->GetNbinsX()+1; jj++)
	    tmp->SetBinContent(jj, 1.);
	}

	tmp->SetDirectory(gROOT);
	m_h_Ptweights_Z_massbinned.insert(m_h_Ptweights_Z_massbinned.begin(), tmp);
      } else {
	std::cout << "ERROR: could not get all mass sliced Z Pt weights" << std::endl;
	break;
      }
    }
    // create extra vector of ptz bin centers: just taking the logarithmic center of the bin
    // creating also something for the under+overflow
    double binratio = TMath::Sqrt(m_h_Ptweights_Z_massbinned[0]->GetXaxis()->GetBinLowEdge(2)
				  /m_h_Ptweights_Z_massbinned[0]->GetXaxis()->GetBinLowEdge(1));
    m_PtzCenters.push_back(m_h_Ptweights_Z_massbinned[0]->GetXaxis()->GetBinLowEdge(1)/binratio);
    for (int i = 0; i <= m_h_Ptweights_Z_massbinned[0]->GetNbinsX(); i++)
      m_PtzCenters.push_back(m_h_Ptweights_Z_massbinned[0]->GetXaxis()->GetBinLowEdge(i+1)
			     *binratio);
  }

  if (m_PtWeight == HighMassFit) {
    m_f_Ptweights_Z = (TF1*) weightsFile->Get(("weightfit/"+source+"_to_Pythia_ptz").c_str());
    //m_f_Ptweights_Z->SetDirectory(gROOT);
    int i = 0;
    while (weightsFile->Get(TString::Format("weightfit/%s_to_Pythia_p%i", source.c_str(), i)) != 0) {
      m_param_Ptweights_Z.push_back((TF1*) weightsFile->Get(TString::Format("weightfit/%s_to_Pythia_p%i", source.c_str(), i)));
      i++;
      //m_param_Ptweights_Z.back()->SetDirectory(gROOT);
    }
    if (m_f_Ptweights_Z == 0 || m_param_Ptweights_Z.size() == 0)
      std::cout << "ERROR: could not get the functions for Z Pt weights" << std::endl;
  }

  // now load polarisation histograms
  // hard-code histogram names
  // on-the-fly smoothing: disabled for now,
  // as this screws up the error calculation and one cannot identify the bad bins any more
  // also 2D smoothing in rapidity is somewhat problematic in smearing the correct dependance

  std::vector < std::string > polnames;
  polnames.push_back("wminus");
  polnames.push_back("wplus");
  polnames.push_back("zm46_66");
  polnames.push_back("zm66_76");
  polnames.push_back("zm76_91");
  polnames.push_back("zm91_106");
  polnames.push_back("zm106_116");
  polnames.push_back("zm116_150");

  for (unsigned int ibosons = 0; ibosons < polnames.size(); ibosons++) {
    for (int imom = 0; imom < 8; imom++) {
      std::stringstream ss;
      ss.clear();
      ss << imom;
      std::string momstr = ss.str();
      
      m_h_aimom_source.push_back((TH2*) weightsFile->Get((source+"/a"+momstr+polnames[ibosons]).c_str()));
      m_h_aimom_target.push_back((TH2*) weightsFile->Get((destination+"/a"+momstr+polnames[ibosons]).c_str()));

      if (m_h_aimom_source.back() != 0) {
	m_h_aimom_source.back()->SetDirectory(gROOT);
	//m_h_aimom_source.back()->Smooth(1, "k3a");
      }
      if (m_h_aimom_target.back() != 0) {
	m_h_aimom_target.back()->SetDirectory(gROOT);
	//m_h_aimom_target.back()->Smooth(1, "k3a");
      }
    }
  }
  
  if (m_h_aimom_source.size() == 8*polnames.size()
      && m_h_aimom_target.size() == 8*polnames.size()
      ) {
    std::cout << "Got W/Z polarisation histograms" << std::endl;
  } else {
    std::cout << "No or incomplete W/Z polarisation histograms" << std::endl;
    m_h_aimom_source.clear();
    m_h_aimom_target.clear();
  }

  weightsFile->Close();
  delete weightsFile;
}


//===============================================================================================================
BosonPtReweightingTool::~BosonPtReweightingTool()
{
  if ( m_h_Ptweights_Wplus )
    delete m_h_Ptweights_Wplus;
  if ( m_h_Ptweights_Wminus )
    delete m_h_Ptweights_Wminus;
  if ( m_h_Ptweights_Z )
    delete m_h_Ptweights_Z;
  
  if ( m_h_Rapidityweights_Wplus )
    delete m_h_Rapidityweights_Wplus;
  if ( m_h_Rapidityweights_Wminus )
    delete m_h_Rapidityweights_Wminus;
  if ( m_h_Rapidityweights_Z )
    delete m_h_Rapidityweights_Z;
  
  for (unsigned int i = 0; i < m_h_aimom_source.size(); i++)
    if (m_h_aimom_source[i] != 0)
      delete m_h_aimom_source[i];
  m_h_aimom_source.clear();
  for (unsigned int i = 0; i < m_h_aimom_target.size(); i++)
    if (m_h_aimom_target[i] != 0)
      delete m_h_aimom_target[i];
  m_h_aimom_target.clear();

  for (unsigned int i = 0; i < m_h_Ptweights_Z_massbinned.size(); i++)
    if (m_h_Ptweights_Z_massbinned[i] != 0)
      delete m_h_Ptweights_Z_massbinned[i];
  m_h_Ptweights_Z_massbinned.clear();
}

//===============================================================================================================
std::string BosonPtReweightingTool::MapMCName(const std::string &input)
{
  // provide alternative MC naming for sets with actual MC11 data set
  // we tag on data set number only for now!

  std::map<std::string, std::string> tagMC;

  tagMC["106043"] = "PythiaMC11";
  tagMC["106044"] = "PythiaMC11";
  tagMC["106046"] = "PythiaMC11";
  tagMC["106047"] = "PythiaMC11";
  tagMC["108319"] = "PythiaMC11";
  tagMC["108320"] = "PythiaMC11";

  tagMC["106081"] = "McAtNloMC11";
  tagMC["106082"] = "McAtNloMC11";
  tagMC["106083"] = "McAtNloMC11";
  tagMC["106084"] = "McAtNloMC11";
  tagMC["106087"] = "McAtNloMC11";
  tagMC["106088"] = "McAtNloMC11";
  tagMC["113712"] = "McAtNloMC11";
  tagMC["126594"] = "McAtNloMC11";
  tagMC["129913"] = "McAtNloMC11";
  tagMC["129914"] = "McAtNloMC11";
  tagMC["147215"] = "McAtNloMC11";
  tagMC["147215"] = "McAtNloMC11";
  tagMC["147216"] = "McAtNloMC11";
  tagMC["147217"] = "McAtNloMC11";
  tagMC["147218"] = "McAtNloMC11";

  tagMC["108297"] = "PowhegPythiaMC11";
  tagMC["108298"] = "PowhegPythiaMC11";
  tagMC["108300"] = "PowhegPythiaMC11";
  tagMC["108301"] = "PowhegPythiaMC11";
  tagMC["108303"] = "PowhegPythiaMC11";
  tagMC["108304"] = "PowhegPythiaMC11";
  tagMC["126967"] = "PowhegPythiaMC11";
  tagMC["126968"] = "PowhegPythiaMC11";
  tagMC["129806"] = "PowhegPythiaMC11";
  tagMC["129807"] = "PowhegPythiaMC11";

  tagMC["113184"] = "PowhegHerwigMC11";
  tagMC["113185"] = "PowhegHerwigMC11";
  tagMC["113186"] = "PowhegHerwigMC11";
  tagMC["113187"] = "PowhegHerwigMC11";
  tagMC["126006"] = "PowhegHerwigMC11";
  tagMC["126007"] = "PowhegHerwigMC11";
  tagMC["129808"] = "PowhegHerwigMC11";
  tagMC["129809"] = "PowhegHerwigMC11";
  tagMC["229808"] = "PowhegHerwigMC11";
  tagMC["147219"] = "PowhegHerwigMC11";
  tagMC["147220"] = "PowhegHerwigMC11";
  tagMC["147221"] = "PowhegHerwigMC11";
  tagMC["147222"] = "PowhegHerwigMC11";

  tagMC["107650"] = "AlpgenMC11_Np0";
  tagMC["107651"] = "AlpgenMC11_Np1";
  tagMC["107652"] = "AlpgenMC11_Np2";
  tagMC["107653"] = "AlpgenMC11_Np3";
  tagMC["107654"] = "AlpgenMC11_Np4";
  tagMC["107655"] = "AlpgenMC11_Np5";
  tagMC["107660"] = "AlpgenMC11_Np0";
  tagMC["107661"] = "AlpgenMC11_Np1";
  tagMC["107662"] = "AlpgenMC11_Np2";
  tagMC["107663"] = "AlpgenMC11_Np3";
  tagMC["107664"] = "AlpgenMC11_Np4";
  tagMC["107665"] = "AlpgenMC11_Np5";
  tagMC["107670"] = "AlpgenMC11_Np0";
  tagMC["107671"] = "AlpgenMC11_Np1";
  tagMC["107672"] = "AlpgenMC11_Np2";
  tagMC["107673"] = "AlpgenMC11_Np3";
  tagMC["107674"] = "AlpgenMC11_Np4";
  tagMC["107675"] = "AlpgenMC11_Np5";
  tagMC["107680"] = "AlpgenMC11_Np0";
  tagMC["107681"] = "AlpgenMC11_Np1";
  tagMC["107682"] = "AlpgenMC11_Np2";
  tagMC["107683"] = "AlpgenMC11_Np3";
  tagMC["107684"] = "AlpgenMC11_Np4";
  tagMC["107685"] = "AlpgenMC11_Np5";
  tagMC["107690"] = "AlpgenMC11_Np0";
  tagMC["107691"] = "AlpgenMC11_Np1";
  tagMC["107692"] = "AlpgenMC11_Np2";
  tagMC["107693"] = "AlpgenMC11_Np3";
  tagMC["107694"] = "AlpgenMC11_Np4";
  tagMC["107695"] = "AlpgenMC11_Np5";
  tagMC["107700"] = "AlpgenMC11_Np0";
  tagMC["107701"] = "AlpgenMC11_Np1";
  tagMC["107702"] = "AlpgenMC11_Np2";
  tagMC["107703"] = "AlpgenMC11_Np3";
  tagMC["107704"] = "AlpgenMC11_Np4";
  tagMC["107705"] = "AlpgenMC11_Np5";

  tagMC["147770"] = "Sherpa14MC11";
  tagMC["147771"] = "Sherpa14MC11";
  tagMC["147774"] = "Sherpa14MC11";
  tagMC["147775"] = "Sherpa14MC11";

  tagMC["147800"] = "PowhegPythia8MC11";
  tagMC["147801"] = "PowhegPythia8MC11";
  tagMC["147802"] = "PowhegPythia8MC11";
  tagMC["147803"] = "PowhegPythia8MC11";
  tagMC["147804"] = "PowhegPythia8MC11";
  tagMC["147805"] = "PowhegPythia8MC11";
  tagMC["147806"] = "PowhegPythia8MC11";
  tagMC["147807"] = "PowhegPythia8MC11";
  tagMC["129501"] = "PowhegPythia8MC11";
  tagMC["129521"] = "PowhegPythia8MC11";

  tagMC["126969"] = "PowhegPythiaMC11";//_MSWT2008";
  tagMC["126970"] = "PowhegPythiaMC11";//_MSWT2008";
  tagMC["126971"] = "PowhegPythiaMC11";//_MSWT2008";
  tagMC["126972"] = "PowhegPythiaMC11";//_MSWT2008";
  tagMC["126973"] = "PowhegPythiaMC11";//_MSWT2008";
  tagMC["126974"] = "PowhegPythiaMC11";//_MSWT2008";
  tagMC["126975"] = "PowhegPythiaMC11";//_HERAPDF15";
  tagMC["126976"] = "PowhegPythiaMC11";//_HERAPDF15";
  tagMC["126977"] = "PowhegPythiaMC11";//_HERAPDF15";
  tagMC["126978"] = "PowhegPythiaMC11";//_HERAPDF15";
  tagMC["126979"] = "PowhegPythiaMC11";//_HERAPDF15";
  tagMC["126980"] = "PowhegPythiaMC11";//_HERAPDF15";
  tagMC["126981"] = "PowhegPythiaMC11";//_NNPDF21";
  tagMC["126982"] = "PowhegPythiaMC11";//_NNPDF21";
  tagMC["126983"] = "PowhegPythiaMC11";//_NNPDF21";
  tagMC["126984"] = "PowhegPythiaMC11";//_NNPDF21";
  tagMC["126985"] = "PowhegPythiaMC11";//_NNPDF21";
  tagMC["126986"] = "PowhegPythiaMC11";//_NNPDF21";
  tagMC["147391"] = "PowhegPythiaMC11";//_NNPDF23";
  tagMC["147392"] = "PowhegPythiaMC11";//_NNPDF23";
  tagMC["147393"] = "PowhegPythiaMC11";//_NNPDF23";
  tagMC["147394"] = "PowhegPythiaMC11";//_NNPDF23";
  tagMC["147395"] = "PowhegPythiaMC11";//_ATLASWZ2010";
  tagMC["147396"] = "PowhegPythiaMC11";//_ATLASWZ2010";
  tagMC["147397"] = "PowhegPythiaMC11";//_ATLASWZ2010";
  tagMC["147398"] = "PowhegPythiaMC11";//_ATLASWZ2010";
  tagMC["147399"] = "PowhegPythiaMC11";//_ATLASWZ2010";
  tagMC["147501"] = "PowhegPythiaMC11";//_NNPDF23coll";
  tagMC["147502"] = "PowhegPythiaMC11";//_NNPDF23coll";
  tagMC["147503"] = "PowhegPythiaMC11";//_NNPDF23coll";
  tagMC["147504"] = "PowhegPythiaMC11";//_NNPDF23coll";

  std::map<std::string, std::string>::iterator it;
  for (it = tagMC.begin() ; it != tagMC.end(); it++) {
    // std::cout << (*it).first << " => " << (*it).second << std::endl;
    if (input.find((*it).first) != std::string::npos) {
      std::cout << "Found DS " << (*it).first << ", which is " << (*it).second << std::endl;
      return ((*it).second);
    }
  }

  return input;
}


//===============================================================================================================  
double BosonPtReweightingTool::GetPtWeightWplus ( double ptWGen ) const
{
  if (!m_h_Ptweights_Wplus)
    return 1.;

  int bin = m_h_Ptweights_Wplus->FindBin( ptWGen/1000 ); // weight histo is in GeV
  double weight = m_h_Ptweights_Wplus->GetBinContent(bin);
  return weight;
}

//===============================================================================================================
double BosonPtReweightingTool::GetPtWeightWminus( double ptWGen ) const
{
  if (!m_h_Ptweights_Wminus)
    return 1.;

  int bin = m_h_Ptweights_Wminus->FindBin( ptWGen/1000 ); // weight histo is in GeV
  double weight = m_h_Ptweights_Wminus->GetBinContent(bin);
  return weight;
}
//===============================================================================================================
double BosonPtReweightingTool::GetPtWeightZ( double ptZGen, double mZGen ) const
{
  double weight = 1.;

  if (m_PtWeight == PeakWeight) {
    if (!m_h_Ptweights_Z)
      return 1.;

    int bin = m_h_Ptweights_Z->FindBin( ptZGen/1000. ); // weight histo is in GeV
    weight = m_h_Ptweights_Z->GetBinContent(bin);

    // quick catch of zero or inf weights (can happen for ZData11_CenFiducial)
    if (weight <= 0 || weight > 100. || TMath::IsNaN(weight)) {
      std::cout << "Warning BosonPtReweightingTool::GetPtWeightZ(): weight " 
		<< weight << " at ptz = " << ptZGen << " GeV; force to 1.\n";
      return 1.;
    }

  } else if (m_PtWeight == MassBinnedWeight) {
    if (m_h_Ptweights_Z_massbinned.size() != m_nMassBins)
      return 1.;

    // find the right place on the regular 2D mass-ptz grid
    // inside the grid we do bi-linear interpolation in mass and log(ptz)
    // beyond the edges the values of the closest bins are returned
    ptZGen /= 1000.;  // weight histo is in GeV

    unsigned int ptzbinlow = m_h_Ptweights_Z_massbinned[0]->FindBin(ptZGen);
    unsigned int ptzbinup;
    if (ptZGen < m_PtzCenters[0]) {
      ptzbinup = ptzbinlow = 0;
    } else if (ptZGen > m_PtzCenters[m_PtzCenters.size()-1]) {
      ptzbinup = ptzbinlow = m_PtzCenters.size()-1;
    } else if (ptZGen < m_PtzCenters[ptzbinlow]) {
      ptzbinup = ptzbinlow;
      ptzbinlow--;
    } else {
      ptzbinup = ptzbinlow+1;
    }

    mZGen /= 1000.;

    unsigned int massbinlow, massbinup;
    if (mZGen < m_MassCenters[0]) {
      massbinlow = massbinup = 0.;
    } else if (mZGen >= m_MassCenters[m_nMassBins-1]) {
      massbinlow = massbinup = m_nMassBins-1;
    } else {
      for (massbinlow = 0; ; massbinlow++)
	if (m_MassCenters[massbinlow] <= mZGen && mZGen < m_MassCenters[massbinlow+1])
	  break;
      massbinup = massbinlow+1;
    }

//     std::cout << "ptzbinning " << ptZGen << " " << ptzbinlow << " " << ptzbinup << " " 
// 	      << m_PtzCenters[ptzbinlow] << " "
// 	      << m_PtzCenters[ptzbinup] << " "
// 	      << std::endl;

//     std::cout << "massbinning " << mZGen << " "
// 	      << massbinlow << " "
// 	      << massbinup << " "
// 	      << m_MassCenters[massbinlow] << " "
// 	      << m_MassCenters[massbinup] << " "
// 	      << std::endl;

    // bilinear interpolation (formulae from "Numerical Recipes" book)
    double weights[4];
    weights[0] = m_h_Ptweights_Z_massbinned[massbinlow]->GetBinContent(ptzbinlow);
    weights[1] = m_h_Ptweights_Z_massbinned[massbinlow]->GetBinContent(ptzbinup);
    weights[2] = m_h_Ptweights_Z_massbinned[massbinup]->GetBinContent(ptzbinup);
    weights[3] = m_h_Ptweights_Z_massbinned[massbinup]->GetBinContent(ptzbinlow);

    double t = 1., u = 1.;
    if (ptzbinlow != ptzbinup) {
      //t = (ptZGen-m_PtzCenters[ptzbinlow])/(m_PtzCenters[ptzbinup]-m_PtzCenters[ptzbinlow]);
      t = TMath::Log(ptZGen/m_PtzCenters[ptzbinlow])
	/TMath::Log(m_PtzCenters[ptzbinup]/m_PtzCenters[ptzbinlow]);
    }
    if (massbinlow != massbinup) {
      u = (mZGen-m_MassCenters[massbinlow])/(m_MassCenters[massbinup]-m_MassCenters[massbinlow]);
    }

    weight = (1-t)*(1-u)*weights[0]
      + t*(1-u)*weights[1]
      + t*u*weights[2]
      + (1-t)*u*weights[3];

    if (weight > 50 || weight < 0.02) {
      std::cout << "WARNING: hit unrealistic boson pt weight of " << weight << " at pt, m " 
		<< ptZGen << ", " << mZGen << " [GeV]. Return 1." << std::endl;
      weight = 1.;
    }


  } else if (m_PtWeight == HighMassFit) {
    if (m_f_Ptweights_Z != 0) {
      // setup parameters for function
      for (unsigned int i = 0; i < m_param_Ptweights_Z.size(); i++)
	m_f_Ptweights_Z->SetParameter(i, m_param_Ptweights_Z[i]->Eval(mZGen/1000.));
      // get weight
      weight = m_f_Ptweights_Z->Eval(ptZGen/1000.);
    }
  }
  return weight;
}

//===============================================================================================================  
double BosonPtReweightingTool::GetRapidityWeightWplus ( double yWGen ) const
{
  if (!m_h_Rapidityweights_Wplus)
    return 1.;

  int bin = m_h_Rapidityweights_Wplus->FindBin( yWGen );
  double weight = m_h_Rapidityweights_Wplus->GetBinContent(bin);
  int bin2 = m_h_Rapidityweights_Wplus->FindBin( -yWGen );
  weight += m_h_Rapidityweights_Wplus->GetBinContent(bin2);
  weight /= 2.0;

  if (weight > 2 || weight < 0.5)
    return 1.;
  return weight;
}

//===============================================================================================================
double BosonPtReweightingTool::GetRapidityWeightWminus( double yWGen ) const
{
  if (!m_h_Rapidityweights_Wminus)
    return 1.;

  int bin = m_h_Rapidityweights_Wminus->FindBin( yWGen );
  double weight = m_h_Rapidityweights_Wminus->GetBinContent(bin);
  int bin2 = m_h_Rapidityweights_Wminus->FindBin( -yWGen );
  weight += m_h_Rapidityweights_Wminus->GetBinContent(bin2);
  weight /= 2.0;

  if (weight > 2 || weight < 0.5)
    return 1.;
  return weight;
}
//===============================================================================================================
double BosonPtReweightingTool::GetRapidityWeightZ( double yZGen ) const
{
  if (!m_h_Rapidityweights_Z)
    return 1.;
  
  int bin = m_h_Rapidityweights_Z->FindBin( yZGen );
  double weight = m_h_Rapidityweights_Z->GetBinContent(bin);
  int bin2 = m_h_Rapidityweights_Z->FindBin( -yZGen );
  weight += m_h_Rapidityweights_Z->GetBinContent(bin2);
  weight /= 2.0;

  if (weight > 2 || weight < 0.5)
    return 1.;
  return weight;
}

double BosonPtReweightingTool::GetPolarisationWeight (double ptGen, double yGen, double mGen,
						      const std::vector<double> &aimom, int qgen) const
{
  if (m_h_aimom_source.size() == 0)
    return 1.;

  int boson = 0;
  if (qgen == -1)
    boson = 0; // W-
  else if (qgen == +1)
    boson = 1; // W+
  else if (qgen == 0) {
    // Z/gamma* with mass binning
    if (mGen < 66000.)
      boson = 2;
    else if (mGen < 76000.)
      boson = 3;
    else if (mGen < 91000.)
      boson = 4;
    else if (mGen < 106000.)
      boson = 5;
    else if (mGen < 116000.)
      boson = 6;
    else
      boson = 7;
  } else {
    // mistake: noisy exit
    std::cout << "Error in BosonPtReweightingTool::GetPolarisationWeight: Boson charge must be -1, 0 or +1.\n";
    return 1.;
  }
  int polbin = m_h_aimom_source[boson*8]->FindBin(ptGen/1000., TMath::Abs(yGen));
  static std::vector<double> ai_source(8, 0.);
  static std::vector<double> ai_target(8, 0.);

  bool largeError = false;

  for (unsigned int imom = 0; imom < 8; imom++) {
    ai_source[imom] = m_h_aimom_source[boson*8+imom]->GetBinContent(polbin);
    ai_target[imom] = m_h_aimom_target[boson*8+imom]->GetBinContent(polbin);
    if (m_h_aimom_source[boson*8+imom]->GetBinError(polbin) > 0.3
	|| m_h_aimom_target[boson*8+imom]->GetBinError(polbin) > 0.3
	|| m_h_aimom_source[boson*8+imom]->GetBinError(polbin) <= 0.0
	|| m_h_aimom_target[boson*8+imom]->GetBinError(polbin) <= 0.0) {
      largeError = true;
    }
  }

  if (largeError) {
    return 1.;
  }

  if (m_impose_physical_pol_constraint) {
    // fix up the "destination" polarisation to sensible values:
    // A0 should be larger than 0?!
    if (ai_target[0] < 0.)
      ai_target[0] = 0.;
  }

  double source = AiPolFunction(ai_source, aimom);

  double destination = AiPolFunction(ai_target, aimom);

  bool debug = false;

  // calculated cross section should in principle be positive!
  if (source < 0.) {
//     std::cout << "Negative source xsec " << source << std::endl;
//     debug = true;
    // cannot do much here... do not reweight
    return 1.;
  }
  if (destination < 0.) {
//     std::cout << "Negative target xsec " << destination << std::endl;
//     debug = true;
    // in case the source xsec is "significant", put weight = 0., otherwise don't reweight
    if (source > 0.01)
      return 0.;
    else
      return 1.;
  }

  double weight = destination / source;
  
  // cap weights at 0.1 or 10
  // sometimes negative weights appear, which we leave intact, though
  // this seems unreasonable and may need further debugging/understanding?!
  if (TMath::Abs(weight) < 0.1) {
    std::cout << "Small pol weight " << weight << std::endl;
    weight = 0.1*(weight>0 ? +1.0 : -1.0);
    debug = true;
  } else if (TMath::Abs(weight) > 10.) {
    std::cout << "Large pol weight " << weight << std::endl;
    weight = 10.*(weight>0 ? +1.0 : -1.0);
    debug = true;
  }

  if (debug) {
    std::cout << "boson pt, y, m " << ptGen << " " << yGen << " " << mGen << std::endl;
    std::cout << "Ai source ";
    for (unsigned int imom = 0; imom < 8; imom++)
      std::cout << ai_source[imom] << " ";
    std::cout << std::endl << "Ai target ";
    for (unsigned int imom = 0; imom < 8; imom++)
      std::cout << ai_target[imom] << " ";
    std::cout << std::endl << "coeff ";
    for (unsigned int imom = 0; imom < 8; imom++)
      std::cout << aimom[imom] << " ";
    std::cout << std::endl;
    std::cout << "source target xsec " << source << " " << destination << std::endl;
  }

  return weight;
}

double BosonPtReweightingTool::AiPolFunction(const std::vector<double> &ai,
					     const std::vector<double> &aimom)
{
  static const double norm = 3./(16.*TMath::Pi());

  // start with 1+cos^2theta term
  double xsec = 1+aimom[4]*aimom[4];
  // add up the other terms

  // ignore A5, A6 and A7, which are only noise
  //for (int imom = 0; imom < 8; imom++) {
  for (int imom = 0; imom < 5; imom++) {
    if (imom == 2)
      xsec += 0.5*ai[imom]*aimom[imom];
    else
      xsec += ai[imom]*aimom[imom];
  }

  return xsec*norm;
}


double BosonPtReweightingTool::CosThetaBeamSystem(const TLorentzVector & lep1, 
						  const TLorentzVector & lep2)
{
  double costheta = TMath::TanH(0.5*(lep1.Eta() - lep2.Eta()));
  if ((lep1.Pz() + lep2.Pz()) < 0)
    costheta *= -1.;
  return costheta;
}

double BosonPtReweightingTool::CosThetaRestSystem(const TLorentzVector & lep1,
						  const TLorentzVector & lep2)
{
  TLorentzVector boson = lep1 + lep2;
  TVector3 boostV = boson.BoostVector();
  TLorentzVector lep1_boosted = lep1;
  lep1_boosted.Boost(-boostV);
  return (TMath::Cos(lep1_boosted.Angle(boson.Vect())));
}

double BosonPtReweightingTool::PhiStar(const TLorentzVector & lep1, 
				       const TLorentzVector & lep2)
{
  double costheta = CosThetaBeamSystem(lep1, lep2);
  double sintheta = TMath::Sqrt(1. - costheta*costheta);
  double phiacop = TMath::Pi() - TVector2::Phi_mpi_pi(lep1.Phi() - lep2.Phi());
  return (TMath::Tan(phiacop/2)*sintheta);
}

void BosonPtReweightingTool::AimomRestSystem(const TLorentzVector & lep1, 
					     const TLorentzVector & lep2,
					     std::vector<double> & aimom)
{
  TLorentzVector boson = lep1+lep2;
  TVector3 boostV = boson.BoostVector();
  TLorentzVector lep1_boosted = lep1;
  lep1_boosted.Boost(-boostV);
  double phi = TVector2::Phi_mpi_pi(lep1_boosted.Phi()-boson.Phi());
  double theta = lep1_boosted.Angle(boson.Vect());

  double costheta = TMath::Cos(theta);
  double sintheta = TMath::Sin(theta);
  double sin2theta = TMath::Sin(2.0*theta);

  double cosphi = TMath::Cos(phi);
  double cos2phi = TMath::Cos(2.0*phi);
  double sinphi = TMath::Sin(phi);
  double sin2phi = TMath::Sin(2.0*phi);

  aimom[0] = 0.5 - 1.5*costheta*costheta;
  aimom[1] = sin2theta*cosphi;
  aimom[2] = sintheta*sintheta*cos2phi;
  aimom[3] = sintheta*cosphi;
  aimom[4] = costheta;
  aimom[5] = sintheta*sintheta*sin2phi;
  aimom[6] = sin2theta*sinphi;
  aimom[7] = sintheta*sinphi;
}

void BosonPtReweightingTool::AimomCollinsSopperSystem(const TLorentzVector & lep1, 
						      const TLorentzVector & lep2,
						      double ebeam,
						      std::vector<double> & aimom)
{
  TLorentzVector boson = lep1+lep2;
  double Lplus  = lep1.E()+lep1.Pz();
  double Lminus = lep1.E()-lep1.Pz();
  double Pplus  = lep2.E()+lep2.Pz();
  double Pminus = lep2.E()-lep2.Pz();

  double costheta  = (Lplus*Pminus - Lminus*Pplus);
  costheta *= TMath::Abs(boson.Pz());
  costheta /= (boson.Mag()*boson.Pz());
  costheta /= TMath::Sqrt(boson.Mag2() + boson.Pt()*boson.Pt());


  TVector3 boostV = -boson.BoostVector();
  TLorentzVector lep1_boosted = lep1;
  lep1_boosted.Boost(boostV);

  TVector3 CSAxis, xAxis, yAxis;
  TLorentzVector p1, p2;
  double sign = +1.;
  if (boson.Z() < 0)
    sign = -1.;
  p1.SetXYZM(0., 0., sign*ebeam, 0.938); // quark (?)
  p2.SetXYZM(0., 0., -sign*ebeam, 0.938); // antiquark (?)

  p1.Boost(boostV);
  p2.Boost(boostV);
  CSAxis = (p1.Vect().Unit()-p2.Vect().Unit()).Unit();
  yAxis = (p1.Vect().Unit()).Cross(p2.Vect().Unit());
  yAxis = yAxis.Unit();
  xAxis = yAxis.Cross(CSAxis);
  xAxis = xAxis.Unit();

  double phi = TMath::ATan2(lep1_boosted.Vect()*yAxis, lep1_boosted.Vect()*xAxis);
  double theta = TMath::ACos(costheta);

  double sintheta = TMath::Sin(theta);
  double sin2theta = TMath::Sin(2.0*theta);

  double cosphi = TMath::Cos(phi);
  double cos2phi = TMath::Cos(2.0*phi);
  double sinphi = TMath::Sin(phi);
  double sin2phi = TMath::Sin(2.0*phi);

  aimom[0] = 0.5 - 1.5*costheta*costheta;
  aimom[1] = sin2theta*cosphi;
  aimom[2] = sintheta*sintheta*cos2phi;
  aimom[3] = sintheta*cosphi;
  aimom[4] = costheta;
  aimom[5] = sintheta*sintheta*sin2phi;
  aimom[6] = sin2theta*sinphi;
  aimom[7] = sintheta*sinphi;
}


double BosonPtReweightingTool::WPolariationFunction(double *x, double *coef)
{
  // function that was used to fit the original cos theta* parabolas

  // x[0] = cos theta*
  // param 0 = normalisation (irrellevant)
  // param 1 = fL
  // param 2 = f0
  // fR is caculated from fL+fR+f0=1
  // param 3 = W charge (+-1)

  double cost = x[0];
  double one_m_qcost = 1-coef[3]*cost;
  double one_p_qcost = 1+coef[3]*cost;
  return (coef[0]*(coef[1]*one_m_qcost*one_m_qcost
 		   +(1-coef[1]-coef[2])*one_p_qcost*one_p_qcost
 		   +coef[2]*2*(1-cost*cost))
 	  );
}

//===============================================================================================================  
double BosonPtReweightingTool::LineShapeFix_Z_McAtNloGeV(double mass)
{
  // empirical function to fix observed generator deficit in ~60-65 GeV mass region
  // simple mass dependent weight seems ok
  // samples affected: mc11_7TeV.10608{7,8}.McAtNloZ*.e1096*

  const double param[4] = {1.73042e-01, 6.26010e+01, 4.20004e-01, 1.};

  if (mass < 60.)
    return 1.;
  if (mass > 75.)
    return 1.;

  return (param[0] * TMath::Exp(-(mass-param[1])*param[2]) + param[3]);
}

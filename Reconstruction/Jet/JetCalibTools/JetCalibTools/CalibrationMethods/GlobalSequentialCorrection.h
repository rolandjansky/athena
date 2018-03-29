/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_GLOBALSEQUENTIALCORRECTION_H
#define JETCALIBTOOLS_GLOBALSEQUENTIALCORRECTION_H 1

 /*
 *  Class definition of GlobalSequentialCorrection - see Root/GlobalSequentialCorrection.cxx for more details
 *  Joe Taenzer (joseph.taenzer@cern.ch), July 2014
 */

#include <TEnv.h>
#include <TAxis.h>
#include <TH2F.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

class GlobalSequentialCorrection 
  : virtual public ::IJetCalibrationTool,
    virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( GlobalSequentialCorrection, IJetCalibrationTool )

 public:
  //Some convenient typedefs
  typedef std::vector<TH2F*> VecTH2F;
  typedef std::vector<double> VecD;
  typedef std::vector<TString> StrV;
  typedef unsigned int uint;

  GlobalSequentialCorrection();
  GlobalSequentialCorrection(const std::string& name);
  GlobalSequentialCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev); //Apply the full GS calibration by default
  virtual ~GlobalSequentialCorrection();

  virtual StatusCode initializeTool(const std::string& name);

 protected:
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;

 private:
  double getTrackWIDTHResponse(double pT, uint etabin, double trackWIDTH) const;
  double getNTrkResponse(double pT, uint etabin, double nTrk) const;
  double getTile0Response(double pT, uint etabin, double Tile0) const;
  double getEM3Response(double pT, uint etabin, double EM3) const;
  double getChargedFractionResponse(double pT, uint etabin, double ChargedFraction) const;
  double getPunchThroughResponse(double E, double eta_det, int Nsegments) const;

  double getGSCCorrection(xAOD::JetFourMom_t jetP4, double eta,
			  double trackWIDTH, double nTrk, double Tile0, double EM3, int Nsegments, double ChargedFraction) const;

  double getJetPropertyMax(TString jetPropName, unsigned int etabin) {
    if ( jetPropName.Contains("EM3") && etabin < m_EM3MaxEtaBin ) return m_respFactorsEM3[etabin]->GetYaxis()->GetXmax();
    else if ( jetPropName.Contains("Tile0") && etabin < m_Tile0MaxEtaBin ) return m_respFactorsTile0[etabin]->GetYaxis()->GetXmax();
    else if ( jetPropName.Contains("nTrk") && etabin < m_nTrkMaxEtaBin ) return m_respFactorsnTrk[etabin]->GetYaxis()->GetXmax();
    else if ( jetPropName.Contains("trackWIDTH") && etabin < m_trackWIDTHMaxEtaBin ) return m_respFactorstrackWIDTH[etabin]->GetYaxis()->GetXmax();
    else return 1;
  }

  void setPunchThroughEtaBins(VecD etabins) { 
    if (etabins.size()==0) ATH_MSG_ERROR("Please check that the punch through eta binning is properly set in your config file");
    m_punchThroughEtaBins=etabins;
  }

  void setPunchThroughMinPt(double minPt) { 
    if (minPt < 0) ATH_MSG_ERROR("Error determining the punch through minimum pT");
    m_punchThroughMinPt = minPt;
  }

 private:

  double readPtJetPropertyHisto(double pT, double jetProperty,
				TH2F *respFactors) const;

 private:
  enum m_GSCSeq { ApplyChargedFraction = 1, ApplyTile0 = 2, ApplyEM3 = 4, ApplynTrk = 8, ApplytrackWIDTH = 16, ApplyPunchThrough = 32 };

  //Private members set in the constructor
  TEnv * m_config;
  TString m_jetAlgo, m_depthString, m_calibAreaTag;
  bool m_dev;

  //Private members set during initialization
  VecTH2F m_respFactorsEM3, m_respFactorsnTrk, m_respFactorstrackWIDTH, m_respFactorsTile0, m_respFactorsPunchThrough, m_respFactorsChargedFraction;
  double m_binSize;
  uint m_depth, m_trackWIDTHMaxEtaBin, m_nTrkMaxEtaBin, m_Tile0MaxEtaBin, m_EM3MaxEtaBin, m_chargedFractionMaxEtaBin;
  VecD m_punchThroughEtaBins;
  double m_punchThroughMinPt;
  bool m_turnOffTrackCorrections;
  bool m_PFlow;
  bool m_pTResponseRequirementOff;
  bool m_nTrkwTrk_4PFlow;
  double m_turnOffStartingpT, m_turnOffEndpT;

};

#endif

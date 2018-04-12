/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************************
 *     Class   : TTBarLeptonJetsBuilder_chi2
 *     Author  : LPC-ClermontFerrand team
 ************************************************************************/

#include "TopEventReconstructionTools/TtresChi2.h"
#include "TopEventReconstructionTools/TtresNeutrinoBuilder.h"

//_________________________________________________________________________________________________
TtresChi2::TtresChi2(std::string units){
  m_debug = 0;
  m_WReco = ROTATION;
  if     (units=="MeV")m_Units = 1000.;
  else if(units=="GeV")m_Units = 1.;
  else std::cerr << "WARNING in TtresChi2: units different from GeV or MeV" << std::endl;
  m_UsePtDiff = PTDIFF;
  m_Btag = AFFECTBTAG;
  m_RunMode = RUNDEFAULT;
  m_NeutrinoBuilder = new TtresNeutrinoBuilder(units);
  Init(DATA2012SUMMER2013PT100);
  res_chi2All  = -1.;
  res_chi2WH   = -1.;
  res_chi2TopH = -1.;
  res_chi2TopL = -1.;
  res_Mtl      = -1.;
  res_Mwh      = -1.;
  res_Mth      = -1.;
  res_Mtt      = -1.;
  m_WhChi2Value     = -1;
  m_ThWhChi2Value   = -1;
  m_TlChi2Value     = -1;
  m_PtDiffChi2Value = -1;
  m_category	= -1;
}

//_________________________________________________________________________________________________
TtresChi2::~TtresChi2(){
  if(m_debug>0) std::cout << "in destructor " << std::endl;
  if (m_NeutrinoBuilder) delete m_NeutrinoBuilder;
}

//_________________________________________________________________________________________________
void TtresChi2::Init(Chi2Version version, double highJetMass) {

  m_highJetMass = highJetMass;

  switch (version){
  
     case DATA2012SUMMER2013PT100:
  	//Rel17.2: values obtained for Z' 0.5TeV-2 TeV including njets>=5 events
	//         LC jets + Jet Area correction
	//	   Applying a cut at 100 GeV on the pT of the leading jet
  	MjjP=82.74*m_Units;
  	SMjjP=9.524*m_Units;
  	
  	m_TopMinusW_had_mean=89.99*m_Units;
  	m_TopMinusW_had_sigma=13.91*m_Units;
  	
  	m_Top_lep_mean=166.6*m_Units;
  	m_Top_lep_sigma=17.34*m_Units;
  	
  	m_PtDiff_mean=-1.903*m_Units;
  	m_PtDiff_sigma=47.24*m_Units;
  	
  	m_PtDiffRel_mean=0.002678;
  	m_PtDiffRel_sigma=0.07662;
	
  	m_PtDiffRelMass_mean=0.001315;
  	m_PtDiffRelMass_sigma=0.05092;
	
  	MTHJJ=171.8*m_Units;
  	STHJJ=16.04*m_Units;
	break;	
     	
     case DATA2012SUMMER2013:
  	//Rel17.2: values obtained for Z' 0.5TeV-2 TeV including njets>=5 events
	//         LC jets + Jet Area correction
  	MjjP=82.41*m_Units;
  	SMjjP=9.553*m_Units;
  	
  	m_TopMinusW_had_mean=89.01*m_Units;
  	m_TopMinusW_had_sigma=15.67*m_Units;
  	
  	m_Top_lep_mean=166*m_Units;
  	m_Top_lep_sigma=17.48*m_Units;
  	
  	m_PtDiff_mean=0.4305*m_Units;
  	m_PtDiff_sigma=46.07*m_Units;
  	
  	m_PtDiffRel_mean=0.00144;
  	m_PtDiffRel_sigma=0.07837;
	
  	m_PtDiffRelMass_mean=0.0004532;
  	m_PtDiffRelMass_sigma=0.05112;
	
  	MTHJJ=171.8*m_Units;
  	STHJJ=16.04*m_Units;
	break;
     case DATA2012AUTOMN2012:
  	//Rel17.2: values obtained for Z' 0.5TeV-2 TeV including njets>=5 events
	//         LC jets
  	MjjP=83.93*m_Units;
  	SMjjP=10.76*m_Units;

  	m_TopMinusW_had_mean=91.05*m_Units;
  	m_TopMinusW_had_sigma=14.23*m_Units;

  	m_Top_lep_mean=168.2*m_Units;
  	m_Top_lep_sigma=20.57*m_Units;

  	m_PtDiff_mean=-8.709*m_Units;
  	m_PtDiff_sigma=55.*m_Units;

  	MTHJJ=173.5*m_Units;
  	STHJJ=16.25*m_Units;
	break;

     case DATA2011SUMMER2012:
  	//Rel17.0: values obtained for Z' 1TeV-2 TeV including njets>=5 events
	//         EM+JES jets
  	MjjP=83.18*m_Units;
  	SMjjP=10.67*m_Units;

  	m_TopMinusW_had_mean=90.87*m_Units;
  	m_TopMinusW_had_sigma=12.78*m_Units;

  	m_Top_lep_mean=167.6*m_Units;
  	m_Top_lep_sigma=20.53*m_Units;

  	m_PtDiff_mean=-7.381*m_Units;
  	m_PtDiff_sigma=63.96*m_Units;

  	MTHJJ=172.2*m_Units;
  	STHJJ=16.8*m_Units;
	break;
     default:
  	std::cerr << "TtresChi2::Init   chi2 is not defined!!" << std::endl;
	break;
  }
  return;
}

//_________________________________________________________________________________________________
void TtresChi2::Reset(){
  res_chi2All  = -1.;
  res_chi2WH   = -1.;
  res_chi2TopH = -1.;
  res_chi2TopL = -1.;
  res_Mwh      = -1.;
  res_Mth      = -1.;
  res_Mtl      = -1.;
  res_Mtt      = -1.;
  m_WhChi2Value	    = -1;
  m_ThWhChi2Value   = -1;
  m_TlChi2Value     = -1;
  m_PtDiffChi2Value = -1;
  m_category	= -1;
  m_nChi2Values = 0;
  m_chi2Values.clear();
  m_chi2Wh_Values.clear();
  m_chi2Th_Values.clear();
  m_chi2ThWh_Values.clear();
  m_chi2Tl_Values.clear();
  m_i_Wq1.clear();
  m_i_Wq2.clear();
  m_i_Thb.clear();
  m_i_Tlb.clear();
  m_i_n.clear();
  m_chi2PtDiff_Values.clear();
  m_PtDiff_Values.clear();
  m_Wh_Values.clear();
  m_ThWh_Values.clear();
  m_Th_Values.clear();
  m_Tl_Values.clear();  
  m_chi2Values.reserve(10000);
  m_chi2Wh_Values.reserve(10000);
  m_chi2Th_Values.reserve(10000);
  m_chi2ThWh_Values.reserve(10000);
  m_chi2Tl_Values.reserve(10000);
  m_chi2PtDiff_Values.reserve(10000);
  m_i_Wq1.reserve(10000);
  m_i_Wq2.reserve(10000);
  m_i_Thb.reserve(10000);
  m_i_Tlb.reserve(10000);
  m_i_n.reserve(10000);
  m_PtDiff_Values.reserve(10000);
  m_Wh_Values.reserve(10000);
  m_ThWh_Values.reserve(10000);
  m_Th_Values.reserve(10000);
  m_Tl_Values.reserve(10000);  
  return;
}

//_________________________________________________________________________________________________
std::vector<TLorentzVector*> TtresChi2::GetNeutrinos(TLorentzVector* L, TLorentzVector*MET){
  std::vector<TLorentzVector*> neutrinoVector;
  if (m_WReco==ROTATION)    neutrinoVector = m_NeutrinoBuilder->candidatesFromWMass_Rotation(L, MET, false /*m_useSmallestPz*/);
  else if (m_WReco==REALPART)neutrinoVector = m_NeutrinoBuilder->candidatesFromWMass_RealPart(L, MET, false /*m_useSmallestPz*/);
  else if (m_WReco==DECREASING)   neutrinoVector = m_NeutrinoBuilder->candidatesFromWMass_Scaling(L, MET);
  return neutrinoVector;
}

//_________________________________________________________________________________________________
bool TtresChi2::findMinChiSquare_HMelseLM(TLorentzVector* L, const std::vector<TLorentzVector*>* jetVector,  const std::vector<bool>* isJetBtagged, TLorentzVector* MET, int& i_q1_W, int& i_q2_W, int& i_b_had, int& i_b_lep, int& ign1, double& chi2ming1, double& chi2ming1H, double& chi2ming1L){
  if(m_debug>0) std::cout << "entering TtresChi2::findMinChiSquare_HMelseLM()" << std::endl;
  bool status = false;

  status = this->findMinChiSquare_HighMass(L, jetVector, isJetBtagged, MET, i_q1_W, i_q2_W,          i_b_lep, ign1, chi2ming1, chi2ming1H, chi2ming1L);
  
  if (status && TMath::Log10(chi2ming1)<0.9) return status; // have found a good combination for HM
  
  // Otherwise try the LM chi2
  status = this->findMinChiSquare         (L, jetVector, isJetBtagged, MET, i_q1_W, i_q2_W, i_b_had, i_b_lep, ign1, chi2ming1, chi2ming1H, chi2ming1L);

  return status;
}


//_________________________________________________________________________________________________
bool TtresChi2::findMinChiSquare_LMelseHM(TLorentzVector* L, const std::vector<TLorentzVector*>* jetVector,  const std::vector<bool>* isJetBtagged, TLorentzVector* MET, int& i_q1_W, int& i_q2_W, int& i_b_had, int& i_b_lep, int& ign1, double& chi2ming1, double& chi2ming1H, double& chi2ming1L){
  if(m_debug>0) std::cout << "entering TtresChi2::findMinChiSquare_LMelseHM()" << std::endl;
  bool status = false;

  status = this->findMinChiSquare         (L, jetVector, isJetBtagged, MET, i_q1_W, i_q2_W, i_b_had, i_b_lep, ign1, chi2ming1, chi2ming1H, chi2ming1L);
  
  if (status && TMath::Log10(chi2ming1)<0.9) return status; // have found a good combination for LM
  
  // Otherwise try the HM chi2
  status = this->findMinChiSquare_HighMass(L, jetVector, isJetBtagged, MET, i_q1_W, i_q2_W,          i_b_lep, ign1, chi2ming1, chi2ming1H, chi2ming1L);

  return status;
}


//_________________________________________________________________________________________________
bool TtresChi2::findMinChiSquare_AllRanges(TLorentzVector* L, const std::vector<TLorentzVector*>* jetVector,  const std::vector<bool>* isJetBtagged, TLorentzVector* MET, int& i_q1_W, int& i_q2_W, int& i_b_had, int& i_b_lep, int& ign1, double& chi2ming1, double& chi2ming1H, double& chi2ming1L){
  if(m_debug>0) std::cout << "entering TtresChi2::findMinChiSquare_AllRanges()" << std::endl;
  bool status = false;
  float massMax = 0.;

  for (unsigned int i=0; i<jetVector->size(); ++i) if(jetVector->at(i)->M()>massMax)massMax= jetVector->at(i)->M();

  if(massMax>=m_highJetMass*m_Units)status = this->findMinChiSquare_HighMass(L, jetVector, isJetBtagged, MET, i_q1_W, i_q2_W,          i_b_lep, ign1, chi2ming1, chi2ming1H, chi2ming1L);
  else 		         status = this->findMinChiSquare         (L, jetVector, isJetBtagged, MET, i_q1_W, i_q2_W, i_b_had, i_b_lep, ign1, chi2ming1, chi2ming1H, chi2ming1L);

  return status;
}

//_________________________________________________________________________________________________
bool TtresChi2::findMinChiSquare(TLorentzVector* L, const std::vector<TLorentzVector*>* jetVector,  const std::vector<bool>* isJetBtagged, TLorentzVector* MET, int& i_q1_W, int& i_q2_W, int& i_b_had, int& i_b_lep, int& ign1, double& chi2ming1, double& chi2ming1H, double& chi2ming1L){
  if(m_debug>0) std::cout << "entering TtresChi2::findMinChiSquare()" << std::endl;
  //_________________________________________________________________________________________________
  i_q1_W=-1;
  i_q2_W=-1;
  i_b_had=-1;
  i_b_lep=-1;
  ign1=-1;
  chi2ming1 =1e7;
  chi2ming1H=1e7;
  chi2ming1L=1e7;
  double chi2ming1WH=1e7;
  int n_jets = jetVector->size();
  int n_bjets = 0;
  
  this->Reset();

  if (L == NULL) {
  	std::cerr << "ERROR : TtresChi2::findMinChiSquare: Lepton is NULL" << std::endl;
  	return false;
  }
  
  if(m_Btag==AFFECTBTAG){
  	for(unsigned int ib = 0; ib<(unsigned int)isJetBtagged->size(); ++ib){
		if(isJetBtagged->at(ib)) n_bjets++;
	}
  }
  
  //_________________________________________________________________________________________________
  std::vector<TLorentzVector*> neutrinoVector = GetNeutrinos(L, MET);

  //_________________________________________________________________________________________________
  for (unsigned int i=0; i<(unsigned int)n_jets; i++){
    const TLorentzVector* I = jetVector->at(i);
    for (unsigned int j=i+1; j<(unsigned int)n_jets; j++){
      const TLorentzVector* J = jetVector->at(j);
      for (unsigned int k=0; k< (unsigned int)n_jets; k++){
	if ( k!=i && k!=j) {
	  const TLorentzVector* K = jetVector->at(k);
	  TLorentzVector TopH = (*I) + (*J) + (*K);
	  TLorentzVector Whad = (*I) + (*J);
	  for (unsigned int n=0; n< neutrinoVector.size(); n++){

	    TLorentzVector* N = neutrinoVector[n];

	    TLorentzVector Wlv = (*N) + (*L);
	    for (unsigned int m=0; m< (unsigned int)n_jets; m++){
	      if (m!=i && m!=j && m!=k) {
	        const TLorentzVector* M = jetVector->at(m);
		TLorentzVector Tlv = Wlv + (*M);
		TLorentzVector Tt = Tlv + TopH;
		double chi2WH = pow((Whad.M()-MjjP)/SMjjP,2);
		double chi2H = chi2WH + pow((TopH.M()-Whad.M()-m_TopMinusW_had_mean)/m_TopMinusW_had_sigma,2);
		double chi2L =  pow((Tlv.M()-m_Top_lep_mean)/m_Top_lep_sigma,2);

		double chi2tempg1 = chi2H+chi2L;
		if (m_UsePtDiff==PTDIFF){  
			double chi2Diff = pow((TopH.Pt()-Tlv.Pt()-m_PtDiff_mean)/m_PtDiff_sigma,2);
			chi2tempg1+=chi2Diff; 
		}else if (m_UsePtDiff==PTDIFFREL){ 
			double chi2DiffRel = pow(((TopH.Pt()-Tlv.Pt())/(TopH.Pt()+Tlv.Pt())-m_PtDiffRel_mean)/m_PtDiffRel_sigma,2);
			chi2tempg1+=chi2DiffRel; 
		}else if (m_UsePtDiff==PTDIFFMASS){
			double chi2DiffRelMass = pow(((TopH.Pt()-Tlv.Pt())/(Tt.M())-m_PtDiffRelMass_mean)/m_PtDiffRelMass_sigma,2);
			chi2tempg1+=chi2DiffRelMass; 
		}
		
		// does this combination contain a b
		// k -> hadronic b
		// m -> leptonic b
		int n_bJetsComb = 0;
		if (isJetBtagged->at(k)){
		  n_bJetsComb++;
		}
		// second b
		if (isJetBtagged->at(m)){
		  n_bJetsComb++;
		}
		
		bool passBtag = false;
		if(m_Btag==STDBTAG || m_Btag==NO_BTAGHM){
		      if(n_bJetsComb>0){
		              passBtag = true;
		      }
		}else if(m_Btag==AFFECTBTAG){
		      if(n_bjets==0){
		      	      passBtag = true;
		      }else if(n_bjets==1){
		              if(n_bJetsComb==1){ passBtag = true; }
		      }else if(n_bjets>=2){
		              if(n_bJetsComb==2){ passBtag = true; }
		      }
		}else if(m_Btag==NO_BTAG){
			passBtag = true;
		}
		
		if(passBtag && m_RunMode==RUNSTUDY){
			m_chi2Values.push_back(chi2tempg1);
  			m_chi2Wh_Values.push_back(chi2WH);
  			m_chi2ThWh_Values.push_back(chi2H - chi2WH);
  			m_chi2Tl_Values.push_back(chi2L);
			m_chi2PtDiff_Values.push_back(chi2tempg1 - chi2H - chi2L);
  			m_i_Wq1.push_back(i);
  			m_i_Wq2.push_back(j);
  			m_i_Thb.push_back(k);
  			m_i_Tlb.push_back(m);
  			m_i_n.push_back(n);
			if(m_UsePtDiff==PTDIFF) m_PtDiff_Values.push_back((TopH.Pt()-Tlv.Pt())/m_Units);
			else if(m_UsePtDiff==PTDIFFMASS) m_PtDiff_Values.push_back((TopH.Pt()-Tlv.Pt())/(Tt.M()));
  			m_Wh_Values.push_back(Whad.M()/m_Units);
  			m_ThWh_Values.push_back((TopH.M()-Whad.M())/m_Units);
  			m_Tl_Values.push_back((Tlv.M())/m_Units);
			m_nChi2Values++;
		}
		
		//#################
		//Original chi2 method
		//#################
		if (chi2tempg1<chi2ming1){
		
		 if(passBtag){
			chi2ming1	= chi2tempg1;
                        chi2ming1H	= chi2H;
                        chi2ming1L	= chi2L;
  			m_WhChi2Value 	  = chi2WH;
			m_ThWhChi2Value   = chi2H - chi2WH;
			m_TlChi2Value 	  = chi2L;
			m_PtDiffChi2Value = chi2tempg1 - chi2H - chi2L;
			
                        i_q1_W=i;
                        i_q2_W=j;
                        i_b_had=k;
                        i_b_lep=m;
                        ign1=n;
			res_Mtl=Tlv.M();
			res_Mwh=Whad.M();
			res_Mth=TopH.M();
			res_Mtt=Tt.M();
			res_Tt = Tt;  
			
			//============================
			// bjet category splitting
			//============================
			if(n_bjets==0){
				m_category = 0;
			}else if(n_bjets==1){
				if(isJetBtagged->at(k)){//b-jet on the hadronic side
					m_category = 2;
				}else if(isJetBtagged->at(m)){//b-jet on the leptonic side
					m_category = 3;
				}else{
					std::cerr << "<!> In TtresChi2::findMinChiSquare : cannot find the corresponding category." << std::endl;
				}
			}else if(n_bjets>=2){
				if(isJetBtagged->at(k)){//b-jet on the hadronic side
					if(isJetBtagged->at(m)){//b-jet on the leptonic side too
						m_category = 1;
					}else{//b-jet only on the hadronic side
						m_category = 2;
					}
				}else if(isJetBtagged->at(m)){
					m_category = 3;
				}else{
					std::cerr << "<!> In TtresChi2::findMinChiSquare : cannot find the corresponding category." << std::endl;
				}
			}
		 }
		} // end of case a minimal chisquare was found
	      } // end of case unique combination
	    } //end of loop over jets
	  } // end of loop over all neutrino solutions
	} // end of case this is the electron channel
      } // end of loop over jets k
    } // end of loop over jets
  } // end of loop over jets

  bool status=false;
  if (  i_q1_W != -1 &&  i_q2_W != -1 &&  i_b_had != -1 &&  i_b_lep != -1 &&  ign1 != -1){
    status = true;
  }

  res_chi2All  = chi2ming1;
  res_chi2WH   = chi2ming1WH;
  res_chi2TopH = chi2ming1H;
  res_chi2TopL = chi2ming1L;

  for (unsigned int i =0; i< neutrinoVector.size(); i++){
    delete neutrinoVector[i];
  }
  neutrinoVector.clear();
  return status;
}


//_________________________________________________________________________________________________
bool TtresChi2::findMinChiSquare_HighMass(TLorentzVector* L, const std::vector<TLorentzVector*>* jetVector,  const std::vector<bool>* isJetBtagged, TLorentzVector* MET, int& i_q1_W, int& i_q2_W, int& i_b_lep, int& ign1, double& chi2ming1, double&
chi2ming1H, double& chi2ming1L){
  if(m_debug>0) std::cout << "entering TtresChi2::findMinChiSquare_HighMass()" << std::endl;
  //_________________________________________________________________________________________________
  i_q1_W=-1;
  i_q2_W=-1;
  i_b_lep=-1;
  ign1=-1;
  chi2ming1 =1e7;
  chi2ming1H=1e7;
  chi2ming1L=1e7;
  double HMtop = -1e6;
  int n_jets = jetVector->size();
  int n_bjets = 0;
  
  this->Reset();

  if (L == NULL) {
  	std::cerr << "ERROR : TtresChi2::findMinChiSquare: Lepton is NULL" << std::endl;
  	return false;
  }
  
  if(m_Btag==AFFECTBTAG){
  	for(unsigned int ib = 0; ib<(unsigned int)isJetBtagged->size(); ++ib){
		if(isJetBtagged->at(ib)) n_bjets++;
	}
  }
  
  //_________________________________________________________________________________________________
  std::vector<TLorentzVector*> neutrinoVector= GetNeutrinos(L, MET);

  //_________________________________________________________________________________________________
  for (unsigned int i=0; i<(unsigned int)n_jets; i++){
    const TLorentzVector* I = jetVector->at(i);
    for (unsigned int j=i+1; j<(unsigned int)n_jets; j++){
      const TLorentzVector* J = jetVector->at(j);
      if ( I->M()>m_highJetMass*m_Units || J->M()>m_highJetMass*m_Units ) {
	TLorentzVector TopH = (*I) + (*J);

	for (unsigned int n=0; n< neutrinoVector.size(); n++){

	  TLorentzVector* N = neutrinoVector[n];

	  TLorentzVector Wlv = (*N) + (*L);
	  for (unsigned int m=0; m<(unsigned int) n_jets; m++){
	    if (m!=i && m!=j) {
	      const TLorentzVector* M = jetVector->at(m);
	      TLorentzVector Tlv = Wlv + (*M);
	      TLorentzVector Tt = Tlv + TopH;

	      double HMtoptemp = I->M();
	      double chi2H = pow((TopH.M()-MTHJJ)/STHJJ,2);
	      double chi2L = pow((Tlv.M()-m_Top_lep_mean)/m_Top_lep_sigma,2);
	      
	      double chi2tempg1 = chi2H+chi2L;
	      if (m_UsePtDiff==PTDIFF){  
			double chi2Diff = pow((TopH.Pt()-Tlv.Pt()-m_PtDiff_mean)/m_PtDiff_sigma,2);
			chi2tempg1+=chi2Diff; 
	      }else if (m_UsePtDiff==PTDIFFREL){ 
			double chi2DiffRel = pow(((TopH.Pt()-Tlv.Pt())/(TopH.Pt()+Tlv.Pt())-m_PtDiffRel_mean)/m_PtDiffRel_sigma,2);
			chi2tempg1+=chi2DiffRel; 
	      }else if (m_UsePtDiff==PTDIFFMASS){
			double chi2DiffRelMass = pow(((TopH.Pt()-Tlv.Pt())/(Tt.M())-m_PtDiffRelMass_mean)/m_PtDiffRelMass_sigma,2);
			chi2tempg1+=chi2DiffRelMass; 
	      }
		
	      // does this combination contain a b
	      // k -> hadronic b
	      // m -> leptonic b
	      int n_bJetsComb = 0;
	      // check whether one of the taggers is passed
	      // first b
	      if (isJetBtagged->at(i) || isJetBtagged->at(j)){
	        n_bJetsComb++;
	      }
	      // second b
	      if (isJetBtagged->at(m)){
	        n_bJetsComb++;
	      }
	        		
	      bool passBtag = false;
	      if(m_Btag==STDBTAG){
	        if(n_bJetsComb>0){
	              passBtag = true;
	        }
	      }else if(m_Btag==AFFECTBTAG){
	      	if(n_bjets==0){
		      passBtag = true;
	        }else if(n_bjets==1){
	              if(n_bJetsComb==1){ passBtag = true; }
	        }else if(n_bjets>=2){
	              if(n_bJetsComb==2){ passBtag = true; }
	        }
	      }else if(m_Btag==NO_BTAGHM || m_Btag==NO_BTAG){
	        passBtag = true;
	      }
		
	      if(passBtag && m_RunMode==RUNSTUDY){
			m_chi2Values.push_back(chi2tempg1);
  			m_chi2Th_Values.push_back(chi2H);
  			m_chi2Tl_Values.push_back(chi2L);
			m_chi2PtDiff_Values.push_back(chi2tempg1 - chi2H - chi2L);
  			m_i_Wq1.push_back(i);
  			m_i_Wq2.push_back(j);
  			m_i_Tlb.push_back(m);
  			m_i_n.push_back(n);			
			if(m_UsePtDiff==PTDIFF) m_PtDiff_Values.push_back((TopH.Pt()-Tlv.Pt())/m_Units);
			else if(m_UsePtDiff==PTDIFFMASS) m_PtDiff_Values.push_back((TopH.Pt()-Tlv.Pt())/(Tt.M()));
  			m_Th_Values.push_back((TopH.M())/m_Units);
  			m_Tl_Values.push_back((Tlv.M())/m_Units);
			m_nChi2Values++;
	      }
	      
	      //#################
	      //Original chi2 method
	      //#################
	      if (chi2tempg1<chi2ming1 && HMtoptemp>=HMtop){
		if(passBtag){
			HMtop=HMtoptemp;
			chi2ming1=chi2tempg1;
			chi2ming1H=chi2H;
			chi2ming1L=chi2L;
			m_ThWhChi2Value = chi2H;
			m_TlChi2Value 	= chi2L;
			m_PtDiffChi2Value = chi2tempg1 - chi2H - chi2L;
			
			i_q1_W=i;
			i_q2_W=j;
			i_b_lep=m;
			ign1=n;
			res_Mtl=Tlv.M();
			//res_Mwh is not defined here
			res_Mth=TopH.M();
			res_Mtt=Tt.M();
			res_Tt = Tt;			
			
			//============================
			// bjet category splitting
			//============================
			if(n_bjets==0){
				m_category = 0;
			}else if(n_bjets==1){
				if(isJetBtagged->at(i) || isJetBtagged->at(j)){//b-jet on the hadronic side
					m_category = 2;
				}else if(isJetBtagged->at(m)){//b-jet on the leptonic side
					m_category = 3;
				}else{
					std::cerr << "<!> In TtresChi2::findMinChiSquare : cannot find the corresponding category." << std::endl;
				}
			}else if(n_bjets>=2){
				if(isJetBtagged->at(i) || isJetBtagged->at(m)){//b-jet on the hadronic side
					if(isJetBtagged->at(m)){//b-jet on the leptonic side too
						m_category = 1;
					}else{//b-jet only on the hadronic side
						m_category = 2;
					}
				}else if(isJetBtagged->at(m)){
					m_category = 3;
				}else{
					std::cerr << "<!> In TtresChi2::findMinChiSquare : cannot find the corresponding category." << std::endl;
				}
			}
			
		} //end of btag requirement
	      } // end of case a minimal chisquare was found
	    } // end of case unique combination
	  } //end of loop over jets
	} // end of loop over all neutrino solutions
      } // end of case this is the electron channel
    } // end of loop over jets
  } // end of loop over jets

  bool status=false;
  if (  i_q1_W != -1 &&  i_q2_W != -1 &&  i_b_lep != -1 &&  ign1 != -1){
    status = true;
  }

  res_chi2All  = chi2ming1;
  res_chi2TopH = chi2ming1H;
  res_chi2TopL = chi2ming1L;


  for (unsigned int i =0; i< neutrinoVector.size(); i++){
    delete neutrinoVector[i];
  }
  neutrinoVector.clear();
  return status;
}

//_________________________________________________________________________________________________
bool TtresChi2::findMinChiSquare_VeryHighMass(TLorentzVector* L, const std::vector<TLorentzVector*>* jetVector,  const std::vector<bool>* isJetBtagged, TLorentzVector* MET, int& i_q2_W, int& i_b_lep, int& ign1, double& chi2ming1, double& chi2ming1H, double& chi2ming1L){
  if(m_debug>0) std::cout << "entering TtresChi2::findMinChiSquare_VeryHighMass() " << std::endl;
  //_________________________________________________________________________________________________
  i_q2_W=-1;
  i_b_lep=-1;
  ign1=-1;
  chi2ming1 =1e7;
  chi2ming1H=1e7;
  chi2ming1L=1e7;
  double HMtop = -1e6;
  int n_jets = jetVector->size();
  this->Reset();

  if (L == NULL) {
  	std::cerr << "ERROR : TtresChi2::findMinChiSquare: Lepton is NULL" << std::endl;
  	return false;
  }

  std::vector<TLorentzVector*> neutrinoVector= GetNeutrinos(L, MET);

  //_________________________________________________________________________________________________
  for (unsigned int i=0; i<(unsigned int)n_jets; i++){
    const TLorentzVector* I = jetVector->at(i);
    TLorentzVector TopH = (*I);

    if (I->M()>150.*m_Units){
      for (unsigned int n=0; n< neutrinoVector.size(); n++){

	TLorentzVector* N = neutrinoVector[n];

	TLorentzVector Wlv = (*N) + (*L);
	for (unsigned int m=0; m< (unsigned int)n_jets; m++){
	  if (m!=i) {
	    const TLorentzVector* M = jetVector->at(m);
	    TLorentzVector Tlv = Wlv + (*M);
	    TLorentzVector Tt = Tlv + TopH;

	    double HMtoptemp = I->M();
	    double chi2H = 1.;
	    double chi2L = pow((Tlv.M()-m_Top_lep_mean)/m_Top_lep_sigma,2);
	    double chi2Diff = pow((TopH.Pt()-Tlv.Pt()-m_PtDiff_mean)/m_PtDiff_sigma,2);
	    double chi2tempg1 = chi2L;

	    if (m_UsePtDiff)  chi2tempg1+=chi2Diff;

	    //#################
	    //Original chi2 method
	    //#################
	    if (chi2tempg1<chi2ming1 && HMtoptemp>=HMtop){

	      // does this combination contain a b
	      // k -> hadronic b
	      // m -> leptonic b
	      int n_bJets = 0;
	      // check whether one of the taggers is passed
	      // first b
	      if (isJetBtagged->at(i)){
		n_bJets++;
	      }
	      // second b
	      if (isJetBtagged->at(m)){
		n_bJets++;
	      }

	      if (n_bJets >= 1){
		HMtop=HMtoptemp;
		chi2ming1=chi2tempg1;
		chi2ming1H=chi2H;
		chi2ming1L=chi2L;
		i_q2_W=i;
		i_b_lep=m;
		ign1=n;
		res_Mtl=Tlv.M();
		//res_Mwh is not defined here
		res_Mth=TopH.M();
		res_Mtt=Tt.M();
		res_Tt = Tt;
	      }		
	    } // end of case a minimal chisquare was found
	  } // end of case unique combination
	} //end of loop over jets
      } // end of loop over all neutrino solutions
    } // end of high mass
  } // end of loop over jets

  bool status = false;
  if (  i_q2_W != -1 &&  i_b_lep != -1 &&  ign1 != -1){
    status = true;
  }

  res_chi2All  = chi2ming1;
  res_chi2TopH = chi2ming1H;
  res_chi2TopL = chi2ming1L;

  for (unsigned int i =0; i< neutrinoVector.size(); i++){
    delete neutrinoVector[i];
  }
  neutrinoVector.clear();
  return status;
}



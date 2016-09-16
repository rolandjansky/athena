/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <cmath>
#include <algorithm>
#include "CxxUtils/make_unique.h"
#include <TF1.h>
#include <TFile.h>

#include <ElectronPhotonFourMomentumCorrection/GainTool.h>

using namespace std;

namespace egGain {
  
  GainTool::GainTool(string filenameTO, string filenameVar){
    m_TOFile = 0;
    m_varFile = 0;
    Init( filenameTO, filenameVar);
  }
    
  GainTool::~GainTool() {
    
    for (int i=0;i<m_NUM_ETA_BINS;++i){
      delete m_funcTO[i];

      for (int j=0;j< m_NUM_ENERGY_BINS;++j){
	delete m_funcG[j][i];
	delete m_conv_funcG[j][i];	
      }
      for (int j=0;j< m_NUM_UNCONV_ENERGY_BINS;++j){
	delete  m_unconv_funcG[j][i];
      }
    }
    
  }

  void GainTool::Init(string filenameTO, string filenameVar){

    m_TOFile = CxxUtils::make_unique<TFile>(filenameTO.c_str() );
    m_varFile = CxxUtils::make_unique<TFile>(filenameVar.c_str());

    for (int id = 0 ; id < m_NUM_ETA_BINS ; id++){
      string etabin;
      stringstream test;
      test << id;
      etabin = "finalFD"+test.str();
      m_funcTO[id] =(TF1*)m_TOFile->Get(etabin.c_str());

      string etabin1_P_elec;
      string etabin2_P_elec;
      string etabin3_P_elec;

      string etabin1_M_elec;
      string etabin2_M_elec;
      string etabin3_M_elec;

      string etabin1_P_conv;
      string etabin2_P_conv;
      string etabin3_P_conv;

      string etabin1_M_conv;
      string etabin2_M_conv;
      string etabin3_M_conv;

      string etabin1_P_unconv;
      string etabin2_P_unconv;
      string etabin3_P_unconv;
      string etabin4_P_unconv;

      string etabin1_M_unconv;
      string etabin2_M_unconv;
      string etabin3_M_unconv;
      string etabin4_M_unconv;

      stringstream testP;
      testP << id-14;
      stringstream testM;
      testM << 13-id;

      etabin1_P_elec = "elec_func1_"+testP.str();
      etabin2_P_elec = "elec_func2_"+testP.str();
      etabin3_P_elec = "elec_func3_"+testP.str();

      etabin1_M_elec = "elec_func1_"+testM.str();
      etabin2_M_elec= "elec_func2_"+testM.str();
      etabin3_M_elec= "elec_func3_"+testM.str();

      etabin1_P_conv = "phot_conv_func1_"+testP.str();
      etabin2_P_conv = "phot_conv_func2_"+testP.str();
      etabin3_P_conv = "phot_conv_func3_"+testP.str();

      etabin1_M_conv = "phot_conv_func1_"+testM.str();
      etabin2_M_conv = "phot_conv_func2_"+testM.str();
      etabin3_M_conv = "phot_conv_func3_"+testM.str();

      etabin1_P_unconv = "phot_unconv_func1_"+testP.str();
      etabin2_P_unconv = "phot_unconv_func2_"+testP.str();
      etabin3_P_unconv = "phot_unconv_func3_"+testP.str();
      etabin4_P_unconv = "phot_unconv_func4_"+testP.str();

      etabin1_M_unconv ="phot_unconv_func1_"+testM.str();
      etabin2_M_unconv= "phot_unconv_func2_"+testM.str();
      etabin3_M_unconv= "phot_unconv_func3_"+testM.str();
      etabin4_M_unconv= "phot_unconv_func4_"+testM.str();

      if(id<14){
	m_funcG[0][id]=(TF1*)m_varFile->Get(etabin1_M_elec.c_str());
	m_funcG[1][id]=(TF1*)m_varFile->Get(etabin2_M_elec.c_str());
	m_funcG[2][id]=(TF1*)m_varFile->Get(etabin3_M_elec.c_str());
	
	m_conv_funcG[0][id]=(TF1*)m_varFile->Get(etabin1_M_conv.c_str());
	m_conv_funcG[1][id]=(TF1*)m_varFile->Get(etabin2_M_conv.c_str());
	m_conv_funcG[2][id]=(TF1*)m_varFile->Get(etabin3_M_conv.c_str());
	
	m_unconv_funcG[0][id]=(TF1*)m_varFile->Get(etabin1_M_unconv.c_str());
	m_unconv_funcG[1][id]=(TF1*)m_varFile->Get(etabin2_M_unconv.c_str());
	m_unconv_funcG[2][id]=(TF1*)m_varFile->Get(etabin3_M_unconv.c_str());
	m_unconv_funcG[3][id]=(TF1*)m_varFile->Get(etabin4_M_unconv.c_str());	
      }

      else {
	m_funcG[0][id]=(TF1*)m_varFile->Get(etabin1_P_elec.c_str());
	m_funcG[1][id]=(TF1*)m_varFile->Get(etabin2_P_elec.c_str());
	m_funcG[2][id]=(TF1*)m_varFile->Get(etabin3_P_elec.c_str());
	
	m_conv_funcG[0][id]=(TF1*)m_varFile->Get(etabin1_P_conv.c_str());
	m_conv_funcG[1][id]=(TF1*)m_varFile->Get(etabin2_P_conv.c_str());
	m_conv_funcG[2][id]=(TF1*)m_varFile->Get(etabin3_P_conv.c_str());
	
	m_unconv_funcG[0][id]=(TF1*)m_varFile->Get(etabin1_P_unconv.c_str());
	m_unconv_funcG[1][id]=(TF1*)m_varFile->Get(etabin2_P_unconv.c_str());
	m_unconv_funcG[2][id]=(TF1*)m_varFile->Get(etabin3_P_unconv.c_str());
	m_unconv_funcG[3][id]=(TF1*)m_varFile->Get(etabin4_P_unconv.c_str());
      }
    }
  }

  double GainTool::CorrectionGainTool(double eta_input, double energy_input, double energy_layer2_input, PATCore::ParticleType::Type ptype){

    double eta_low[28] = {-2.4,-2.32,-2.22,-2.12,-2.02,-1.92,-1.82,-1.72,-1.62,-1.37,-1.2,-0.8,-0.6,-0.4,0.,0.4,0.6,0.8,1.2,1.52,1.62,1.72,1.82,1.92,2.02,2.12,2.22,2.32};
    double eta_high[28] = {-2.32,-2.22,-2.12,-2.02,-1.92,-1.82,-1.72,-1.62,-1.52,-1.2,-0.8,-0.6,-0.4,0,0.4,0.6,0.8,1.2,1.37,1.62,1.72,1.82,1.92,2.02,2.12,2.22,2.32,2.4};


    double diffES[28] = {-0.0763457,0.198092,-0.0288093,-0.0808452,0.0271571,-0.0975428,-0.0164521,-0.0737317,0.136447,0.0352632,-0.00197711,0.0244447,-0.0641183,0.0810265,
			   0.00735352,-0.013568,-0.0169185,-0.0142155,-0.0255637,0.0586014,-0.163098,0.0237162,-0.0690589,-0.0346536,-0.0886648,-0.0914096,0.0738988,-0.0376201};


    double corrHG[28] = {-0.484364,0.263687,-0.037742,-0.0553371,-0.0640682,-0.201265,-0.176052,-0.206764,0.0224639,0.00855262,-0.00583258,0.00196343,-0.108951,0.0708467,
			   -0.00438541,-0.0928867,-0.0487188,-0.0214743,-0.0619355,-0.055117,-0.297427,-0.0795861,-0.173311,-0.0905191,-0.297548,-0.147814,-0.0867268,-0.384337};

    double corrMG[28]={-0.0160707,0.199527,-0.000845413,-0.0611091,0.0877896,0.0479742,0.141414,0.639507,0.72873,0.21984,0.0643192,0.146518,0.0279768,0.140151,0.0682126,
			 0.167472,0.154887,0.122343,0.212282,0.657224,0.576652,0.135954,0.0798118,0.0167071,-0.0221686,-0.0398211,0.128146,-0.0226478};

    double range_energy[28]={195.,180.,175.,160.,140.,145.,155.,155.,145.,140.,120.,90.,90.,75.,75.,90.,90.,120.,140.,145.,155.,155.,145.,140.,160.,175.,180.,195.};
    
    double corrM_G, corrE;
    corrM_G =1;
    corrE=0;

    double energy_output;

    TF1* funcG_com[m_NUM_ENERGY_BINS][m_NUM_ETA_BINS]={};

    if (fabs(eta_input) > 2.4)
      return energy_input*1000.;

    int id_eta = -9999999;

    for(int i = 0 ; i < 28 ; i++){
      if( (eta_input >= eta_low[i]) && (eta_input <= eta_high[i]) ){
	id_eta = i;
	break;
      }
    }

    if (ptype == PATCore::ParticleType::UnconvertedPhoton){
      double norm_unconv=1.;
      if (id_eta<17 && id_eta>10) norm_unconv = m_unconv_funcG[0][id_eta]->Eval(range_energy[id_eta]);
      else if (id_eta<25 && id_eta>2) norm_unconv = m_unconv_funcG[1][id_eta]->Eval(range_energy[id_eta]);
      else norm_unconv = m_unconv_funcG[2][id_eta]->Eval(range_energy[id_eta]);

      if (energy_input<92) corrM_G = (m_unconv_funcG[0][id_eta]->Eval(energy_input))/(norm_unconv);
      else if (energy_input<160 && energy_input>=92) corrM_G = (m_unconv_funcG[1][id_eta]->Eval(energy_input))/(norm_unconv);
      else if (energy_input<400 && energy_input>=160) corrM_G = (m_unconv_funcG[2][id_eta]->Eval(energy_input))/(norm_unconv);
      else if (energy_input>=400) corrM_G = (m_unconv_funcG[3][id_eta]->Eval(energy_input))/(norm_unconv);
    }

    else if (ptype==PATCore::ParticleType::ConvertedPhoton){
      funcG_com[0][id_eta]= m_conv_funcG[0][id_eta];
      funcG_com[1][id_eta]= m_conv_funcG[1][id_eta];
      funcG_com[2][id_eta]= m_conv_funcG[2][id_eta];
    }
    else if (ptype==PATCore::ParticleType::Electron){
      funcG_com[0][id_eta]= m_funcG[0][id_eta];
      funcG_com[1][id_eta]= m_funcG[1][id_eta];
      funcG_com[2][id_eta]= m_funcG[2][id_eta];
    }


    if(ptype==PATCore::ParticleType::ConvertedPhoton || ptype==PATCore::ParticleType::Electron){
      if(id_eta==12 || id_eta == 13 || id_eta == 14 || id_eta==15){
	if (energy_input<105) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<400 && energy_input>=105) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=400) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else if(id_eta == 11 || id_eta ==16){
	if (energy_input<130) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<440 && energy_input>=130) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=440) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else if(id_eta == 10 || id_eta ==17){
	if (energy_input<190) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<400 && energy_input>=190) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=400) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else if(id_eta == 9 || id_eta ==18){
	if (energy_input<200) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<480 && energy_input>=200) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=480) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else if(id_eta==7 || id_eta == 8 || id_eta ==19 || id_eta==20){
	if (energy_input<250) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<450 && energy_input>=250) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=450) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else if(id_eta == 6 || id_eta ==21){
	if (energy_input<180) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<385 && energy_input>=180) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=385) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else if(id_eta == 5 || id_eta ==22){
	if (energy_input<165) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<460 && energy_input>=165) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=460) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else if(id_eta == 4 || id_eta ==23){
	if (energy_input<160) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<450 && energy_input>=160) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=450) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }

      else{
	if (energy_input<200) corrM_G = (funcG_com[0][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input<480 && energy_input>=200) corrM_G = (funcG_com[1][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
	else if (energy_input>=480) corrM_G = (funcG_com[2][id_eta]->Eval(energy_input))/(funcG_com[0][id_eta]->Eval(range_energy[id_eta]));
      }
    }
    
    double ets2 = energy_layer2_input/cosh(eta_input);
    double valTO = (m_funcTO[id_eta])->Eval(ets2);
    if (valTO < 0) {
      valTO=0;
    }

    corrE = -2 * energy_input* (((corrMG[id_eta]/91.2) - (diffES[id_eta]/91.2))*valTO*corrM_G + ((corrHG[id_eta]/91.2) - (diffES[id_eta]/91.2))*(1-valTO));
    energy_output = (energy_input+corrE)*1000.;
    return energy_output;
  }
}

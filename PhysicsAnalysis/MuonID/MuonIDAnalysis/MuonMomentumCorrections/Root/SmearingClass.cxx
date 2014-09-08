/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
//           SmearingClass.cxx -- ATLAS Experiment Software               //
////////////////////////////////////////////////////////////////////////////
///
/// class providing corrections to the simulated muon pT to match pT in data.
/// Resolution smearing and scale shift numbers are preliminary
///
/// responsible: atlas-perf-muons-conveners (at) cern.ch
///
#include <cstdlib>
#include <sstream>
#include <cassert>
#include <cmath>
#include "MuonMomentumCorrections/SmearingClass.h"
#include "MuonMomentumCorrections/BrandeisMomentumResolution.h"

using namespace MuonSmear;
using namespace std;

TRandom3* SmearingClass::m_rand = 0;

SmearingClass::SmearingClass(){
  if(!m_rand) {
    m_rand = new TRandom3();
  }
  useErrMatrix = false;

  DetRegions = NULL;
  m_nb_regions = 4;
  m_CallSetClass=false;
}

SmearingClass::SmearingClass(string Data,
			     string Algo,
			     string SmearType,
			     string Rel,
			     string Dir) {  
  if(!m_rand ) {
    m_rand = new TRandom3();
  }
  useErrMatrix = false;

  m_CallSetClass=true;
  m_nb_regions = 4;
  Initialize(Data,Algo,SmearType,Rel,Dir);
}

SmearingClass::SmearingClass(const SmearingClass& _rhs){
  const string SmearType = _rhs.Tsmear==SMEAR_QPT ? "q_pT" : "pT";
  useErrMatrix = false;
  m_nb_regions = 4;
  Initialize(_rhs.m_DataYear,_rhs.m_Algorithm,SmearType,_rhs.m_Release,_rhs.m_Fdir);
}

SmearingClass::~SmearingClass(){
    if( DetRegions != NULL) delete DetRegions;
    Finalize();
}

void SmearingClass::Finalize(){
    Clean();//Do we need this and the one before??
}

void SmearingClass::Initialize(string data,
			       string algo,
			       string smearType,
			       string rel,
			       string dir) {
  GeV=1000;
  m_pTmax=100000000;
  m_rand->SetSeed(0);
  useTan2 = true;
  useScale = true;
  useIDResoFunc   = false;
  useEnLossMSCorr = true;
  detRegion = 0;
  scaleRegion = 0;
  restrictCurvCorrSigma = 0.0;
  m_ScaleType = "";
  apply_to_data = false;
  useImprovedCombine = true;

  useNsigmaForICombine = 4;

  smearDeltaMS = 0;
  smearDeltaID = 0;
  smearDeltaCB = 0;
  ptms = 0;
  ptid = 0;
  ptcb = 0;
  eta = 0;
  charge = 0;
  if(m_CallSetClass){
    Tscale = SCALE_DEFAULT;
    SetAlgoSmearRelDir(data,algo,smearType,rel,dir);
  }
}

void SmearingClass::CleanScales(){
  apply_to_data = false;
  Tscale = SCALE_DEFAULT;
  m_scaleBins.clear();
  m_S1_ID.clear();
  m_S2_ID.clear();
  m_S1_MS.clear();
  m_S2_MS.clear();
  m_S1_CB.clear();
  m_S2_CB.clear();
  m_S1Corr_ID.clear();
  m_S2Corr_ID.clear();
  m_S1Corr_MS.clear();
  m_S2Corr_MS.clear();
  m_S1Corr_CB.clear();
  m_S2Corr_CB.clear();
  m_S1ACorr_ID.clear();
  m_S2ACorr_ID.clear();
  m_S1ACorr_MS.clear();
  m_S2ACorr_MS.clear();
  m_S1ACorr_CB.clear();
  m_S2ACorr_CB.clear();
}

void SmearingClass::Clean(){  
  m_scale_ID.clear();
  m_enLoss_MS.clear();
  m_scale_MS.clear();
  m_scale_CB.clear();
  m_scaleSyst_ID.clear();
  m_enLossSyst_MS.clear();
  m_scaleSyst_MS.clear();
  m_scaleSyst_CB.clear();

  m_scaleSystUp_ID.clear();
  m_enLossSystUp_MS.clear();
  m_scaleSystUp_MS.clear();
  m_scaleSystDw_ID.clear();
  m_enLossSystDw_MS.clear();
  m_scaleSystDw_MS.clear();

  CleanScales();
  m_p1_ID.clear();
  m_p2_ID.clear();
  m_p2_ID_TAN.clear();
  m_p0_MS.clear();
  m_p1_MS.clear();
  m_p2_MS.clear();

  m_E_p1_ID.clear();
  m_E_p2_ID.clear();
  m_E_p2_ID_TAN.clear();
  m_E_p0_MS.clear();
  m_E_p1_MS.clear();
  m_E_p2_MS.clear();

  m_S_p1_ID.clear();
  m_S_p2_ID.clear();
  m_S_p2_ID_TAN.clear();
  m_S_p0_MS.clear();
  m_S_p1_MS.clear();
  m_S_p2_MS.clear();
  m_SUp_p1_ID.clear();
  m_SUp_p2_ID.clear();
  m_SUp_p2_ID_TAN.clear();
  m_SUp_p0_MS.clear();
  m_SUp_p1_MS.clear();
  m_SUp_p2_MS.clear();
  m_SDw_p1_ID.clear();
  m_SDw_p2_ID.clear();
  m_SDw_p2_ID_TAN.clear();
  m_SDw_p0_MS.clear();
  m_SDw_p1_MS.clear();
  m_SDw_p2_MS.clear();

  m_MC_p1_ID.clear();
  m_MC_p2_ID.clear();
  m_MC_p2_ID_TAN.clear();
  m_MC_p0_MS.clear();
  m_MC_p1_MS.clear();
  m_MC_p2_MS.clear();

  m_CorrMatC0.clear();
  m_CorrMatC1.clear();
  m_CorrMatC2.clear();
  m_CorrMatC3.clear();
  m_CorrMatC4.clear();
  m_CorrMatC5.clear();
  m_CorrMatTanC0.clear();
  m_CorrMatTanC1.clear();
  m_CorrMatTanC2.clear();
  m_CorrMatTanC3.clear();
  m_CorrMatTanC4.clear();
  m_CorrMatTanC5.clear();
  InValues.clear(); 
}

void SmearingClass::SetAlgoSmearRelDir(string Data,
				      string Algo,
				      string SmearType,
				      string Rel,
				      string Dir) {  
  if(!m_CallSetClass){
    Initialize(Data,Algo,SmearType,Rel,Dir);
  }
  
  /*Checking DataType*/
  m_DataYear=Data;
  Tdata=SetData(m_DataYear);
  cout<<"SmearingClass INFO:: using " << m_DataYear <<endl;
  
  /*Checking algorithm*/
  if(Algo=="muid" || Algo=="staco" || Algo=="muons"){
    m_Algorithm=Algo;
    Talgo=SetAlgorithm(m_Algorithm);
    cout<<"SmearingClass INFO:: using " << m_Algorithm << " Algorithm "<<endl;
  }
  else{
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: wrong Algorithm in input "<<Algo<<endl
	<<"                      possible values are: "<<endl
	<<"                               muid or staco or muons (only 2012)"<<endl;
    return;
  }
  
  /*Checking smearing type*/
  if(SmearType=="pT") {
    Tsmear=SMEAR_PT;
    cout<<"SmearingClass INFO:: using " << SmearType << " Smearing "<<endl;
  }
  else if(SmearType=="q_pT"){
    Tsmear=SMEAR_QPT;
    cout<<"SmearingClass INFO:: using " << SmearType << " Smearing "<<endl;
  }
  else{
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: wrong Smear Type in input "<<SmearType<<endl
	<<"                      possible values are: "<<endl
	<<"                               pT ot q_pT "<<endl;
    return;
  }

  /*Checking reconstruction release*/
  if(Rel=="Rel16.6" || Rel=="Rel17" || Rel=="Rel17.2" || Rel == "Rel17.2Repro" || Rel == "Rel17.2Sum13"){
    m_Release=Rel;
    Trel=SetRelease(m_Release);
    cout<<"SmearingClass INFO:: using " << m_Release << " Release "<<endl;
  }
  else{
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: wrong Release in input "<<Rel<<endl
	<<"                      possible values are: "<<endl
	<<"                               Rel16.6, Rel17, Rel17.2, Rel17.2Repro, Rel17.2Sum13"<<endl;
    return;
  }
  
  /*Checking path*/
  if(Dir != ""){
      m_Fdir = Dir;
      cout<<"SmearingClass INFO:: Using user defined path:"<<endl
	  <<"                     "<<m_Fdir<<endl;
  } else {
      char *rootCoreArea = getenv("ROOTCOREBIN");
      char *rootCoreAreaOld = getenv("ROOTCOREDIR");
      char *testArea = getenv("TestArea");
      if(rootCoreArea != NULL){//First check RootCore as on grid the testArea is set also for rootcore
	  m_Fdir = string(rootCoreArea) + "/data/MuonMomentumCorrections/";
      } else if(rootCoreAreaOld != NULL){//old default for RootCore
	  m_Fdir = string(rootCoreAreaOld) + "/data/MuonMomentumCorrections/";
      } else if ( testArea != NULL ){//Athena running
	  m_Fdir = string(testArea) + "/InstallArea/share/";
      } else {
	  cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: "<<endl
	  <<" NO Path is set from ATHENA, ROOTCORE, or from the User! "<<endl;
	  return;
      }
  }

  if(m_Algorithm == "muons" && m_DataYear != "Data12"){
      cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: wrong settings:"<<endl
	  <<"                           'muons' algorithm available only for 2012 Data!"<<endl;
  }

  /*Filling Regions and setting specific defaults*/
  string regionsPath = m_Fdir;
  int regionMode =0; //simple Eta Bins
  if (Trel <= REL17) {
      regionsPath +="Regions.dat";
  } else if (Trel == REL17_2_REPRO) {
      regionsPath += "RegionsRepro.dat";//16 eta bins for ID and MS
  } else if (Trel == REL17_2_SUM13){
      regionsPath += "RegionsPhi18.dat";
      regionMode = 2;//MS: 48Bins L/S split plus 11_15 and Feet in the BARREL. ID has values from 16 eta bins 
      /////// Best options for this release /////// 
      useEnLossMSCorr = true;
      useNsigmaForICombine = 0;
      /////////////////////////////////////////////
  } else {
      cerr<<__FILE__<<" line:"<<__LINE__<<" SmearingClass ERROR: No Region file associated to Release="
	       <<Trel<<endl<<"Exit!"<<endl;
      exit(-1);
  }
  DetRegions = new Regions(regionsPath,regionMode);
  m_nb_regions = (int)DetRegions->GetNRegions();
  /*Filling Corrections Values*/
  FillValues();
}

void SmearingClass::FillValues(){
  /* Choose MuID or Staco Algorithm Values */
  
  Clean();
  double tmpval;
  int i=0;
  string tmpname;
  string scale_val = m_Fdir+"Scale_values_"+m_Algorithm+"_"+m_Release+".dat";
  if(m_DataYear == "Data11" && Trel >= REL17_2_SUM13) 
      scale_val = m_Fdir+"Data11_Scale_values_"+m_Algorithm+"_"+m_Release+".dat";
  string data_val  = m_Fdir+m_DataYear+"_values_"+m_Algorithm+"_"+m_Release+".dat";
  string mc_val    = m_Fdir+"MC_values_"+m_Algorithm+"_"+m_Release+".dat";
  string corr_val  = m_Fdir+m_DataYear+"_CorrMatrix_"+m_Algorithm+"_"+m_Release+".dat";
  
  /* filling scale vectors*/
  InValues.open(scale_val.c_str());
  i=0;
  if(!InValues.good()){
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: File "<<scale_val<<" NOT FOUND!!"<<endl;
    cout<<"                      Please provide a correct path!"<<endl;
    return;
  }
  else{
    while(InValues.good() && i<m_nb_regions){
      tmpval=0;
      if(i==0){
	getline(InValues,tmpname);
      }
      if(Trel >= REL17_2_SUM13) {
	  //These have a different order and Up/Dw split sys
	  InValues>>tmpval;
	  m_scale_ID.push_back(tmpval);
	  InValues>>tmpval;
      	  m_enLoss_MS.push_back(tmpval);
	  InValues>>tmpval;
	  m_scale_MS.push_back(tmpval);

      	  InValues>>tmpval;
	  m_scaleSystUp_ID.push_back(tmpval);
      	  InValues>>tmpval;
      	  m_enLossSystUp_MS.push_back(tmpval);
	  InValues>>tmpval;
	  m_scaleSystUp_MS.push_back(tmpval);
      	  InValues>>tmpval;
	  m_scaleSystDw_ID.push_back(tmpval);
      	  InValues>>tmpval;
      	  m_enLossSystDw_MS.push_back(tmpval);
	  InValues>>tmpval;
	  m_scaleSystDw_MS.push_back(tmpval);

      } else {
	  m_enLoss_MS.push_back(0.);
	  m_enLossSyst_MS.push_back(0.);

	  InValues>>tmpval;
	  m_scale_ID.push_back(tmpval);
	  InValues>>tmpval;
	  m_scaleSyst_ID.push_back(tmpval);
	  InValues>>tmpval;
      	  m_scale_MS.push_back(tmpval);
	  InValues>>tmpval;
	  m_scaleSyst_MS.push_back(tmpval);
      }
      //CB is always on the cfg file althoug rarely used
      InValues>>tmpval;
      m_scale_CB.push_back(tmpval);
      InValues>>tmpval;
      m_scaleSyst_CB.push_back(tmpval);

      i++;
    }
  }
  InValues.close();
  InValues.clear();
  
  /* filling data vectors*/
  InValues.open(data_val.c_str());
  i=0;
  if(!InValues.good()){
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: File "<<data_val<<" NOT FOUND!!"<<endl;
    cout<<"                      Please provide a correct path!"<<endl;
    return;
  } else{
      while(InValues.good() && i < m_nb_regions){
	  /*main values*/
	  tmpval=0;
	  if(i==0){
	      getline(InValues,tmpname);
	  }
	  InValues>>tmpval;
	  m_p1_ID.push_back(tmpval);
	  InValues>>tmpval;
	  m_p2_ID.push_back(tmpval);
	  InValues>>tmpval;
	  m_p2_ID_TAN.push_back(tmpval);
	  InValues>>tmpval;
	  if(Trel >= REL17_2_SUM13) {
	      m_p0_MS.push_back(tmpval);
	      InValues>>tmpval;
	  } else {
	      m_p0_MS.push_back(0);
	  }
	  m_p1_MS.push_back(tmpval);
	  InValues>>tmpval;
	  m_p2_MS.push_back(tmpval);
	  //Syst and stat uncertianties: they are split and symmetrized before Rel17.2Sum13, after
	  // that, the stat is added in ^2 and they are split in Up/Dw
	  if(Trel >= REL17_2_SUM13) {
	      InValues>>tmpval;
	      m_SUp_p1_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_SUp_p2_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_SUp_p2_ID_TAN.push_back(tmpval);
	      InValues>>tmpval;
	      m_SUp_p0_MS.push_back(tmpval);
	      InValues>>tmpval;
	      m_SUp_p1_MS.push_back(tmpval);
	      InValues>>tmpval;
	      m_SUp_p2_MS.push_back(tmpval);
	      InValues>>tmpval;
	      m_SDw_p1_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_SDw_p2_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_SDw_p2_ID_TAN.push_back(tmpval);
	      InValues>>tmpval;
	      m_SDw_p0_MS.push_back(tmpval);
	      InValues>>tmpval;
	      m_SDw_p1_MS.push_back(tmpval);
	      InValues>>tmpval;
	      m_SDw_p2_MS.push_back(tmpval);
	  } else {
	      m_E_p0_MS.push_back(0); m_S_p0_MS.push_back(0);
	      //Stat
	      InValues>>tmpval;
	      m_E_p1_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_E_p2_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_E_p2_ID_TAN.push_back(tmpval);
	      InValues>>tmpval;
	      m_E_p1_MS.push_back(tmpval);
	      InValues>>tmpval;
	      m_E_p2_MS.push_back(tmpval);
	      /*systematic*/
	      InValues>>tmpval;
	      m_S_p1_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_S_p2_ID.push_back(tmpval);
	      InValues>>tmpval;
	      m_S_p2_ID_TAN.push_back(tmpval);
	      InValues>>tmpval;
	      m_S_p1_MS.push_back(tmpval);
	      InValues>>tmpval;
	      m_S_p2_MS.push_back(tmpval);
	  }
      }
  }
  InValues.close();
  InValues.clear();
  
  /* filling mc vectors*/
  InValues.open(mc_val.c_str());
  i=0;
  if(!InValues.good()){
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: File "<<mc_val<<" NOT FOUND!!"<<endl;
    cout<<"                      Please provide a correct path!"<<endl;
    return;
  }
  else{
    while(InValues.good() && i<m_nb_regions){
      tmpval=0;
      if(i==0){
	getline(InValues,tmpname);
      }
      InValues>>tmpval;
      m_MC_p1_ID.push_back(tmpval);
      InValues>>tmpval;
      m_MC_p2_ID.push_back(tmpval);
      InValues>>tmpval;
      m_MC_p2_ID_TAN.push_back(tmpval);
      InValues>>tmpval;
      m_MC_p0_MS.push_back(tmpval);
      InValues>>tmpval;
      m_MC_p1_MS.push_back(tmpval);
      InValues>>tmpval;
      m_MC_p2_MS.push_back(tmpval);
    }
  }
  InValues.close();
  InValues.clear();
  
  /* filling correlation vectors*/
  InValues.open(corr_val.c_str());
  i=0;
  if(!InValues.good()){
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: File "<<corr_val<<" NOT FOUND!!"<<endl;
    cout<<"                      Please provide a correct path!"<<endl;
    return;
  }
  else{
    while(InValues.good() && i<m_nb_regions){
      tmpval=0;
      if(i==0){
	getline(InValues,tmpname);
      }
      InValues>>tmpval;
      m_CorrMatC0.push_back(tmpval);
      m_CorrMatTanC0.push_back(tmpval);
      
      InValues>>tmpval;
      m_CorrMatC1.push_back(tmpval);
      m_CorrMatTanC1.push_back(tmpval);
      
      InValues>>tmpval;
      m_CorrMatC2.push_back(tmpval);
      m_CorrMatTanC2.push_back(tmpval);
    
      InValues>>tmpval;
      m_CorrMatC3.push_back(tmpval);
      m_CorrMatTanC3.push_back(tmpval);
    
      InValues>>tmpval;
      m_CorrMatC4.push_back(tmpval);
      m_CorrMatTanC4.push_back(tmpval);
      
      InValues>>tmpval;
      m_CorrMatC5.push_back(tmpval);
      m_CorrMatTanC5.push_back(tmpval);
    }
  }
  InValues.close();
  InValues.clear();  
}

void SmearingClass::PrintValues(){  
  cout<<"SmearingClass INFO:: Checking if all variables are filled ... "<<endl;
  for(UInt_t i=0; i<m_scale_CB.size(); i++){
    cout<<endl;    
    cout<<" scale_ID["<<i<<"] "<<m_scale_ID[i]
	     <<"; enLoss_MS["<<i<<"] "<<m_enLoss_MS[i]
	     <<"; scale_MS["<<i<<"] "<<m_scale_MS[i]<<endl;
    if(Trel < REL17_2_SUM13){
    	cout<<" scaleSyst_ID["<<i<<"] "<<m_scaleSyst_ID[i]
    		 <<"; enLossSyst_MS["<<i<<"] "<<m_enLossSyst_MS[i]
    		 <<"; scaleSyst_MS["<<i<<"] "<<m_scaleSyst_MS[i]<<endl;
    } else {
    	cout<<" scaleSystUp_ID["<<i<<"] "<<m_scaleSystUp_ID[i]
    		 <<"; enLossSystUp_MS["<<i<<"] "<<m_enLossSystUp_MS[i]
    		 <<"; scaleSystUp_MS["<<i<<"] "<<m_scaleSystUp_MS[i]<<endl;
    	cout<<" scaleSystDw_ID["<<i<<"] "<<m_scaleSystDw_ID[i]
    		 <<"; enLossSystDw_MS["<<i<<"] "<<m_enLossSystDw_MS[i]
    		 <<"; scaleSystDw_MS["<<i<<"] "<<m_scaleSystDw_MS[i]<<endl;
    }
    cout<<" scale_CB["<<i<<"] "<<m_scale_CB[i]<<endl;
    cout<<" scaleSyst_CB["<<i<<"] "<<m_scaleSyst_CB[i]<<endl;
    
    cout<<" p1_ID["<<i<<"] "<<m_p1_ID[i]
	     <<";  p2_ID["<<i<<"] "<<m_p2_ID[i]
	     <<"; p2_ID_TAN["<<i<<"] "<<m_p2_ID_TAN[i]<<endl;
    cout<<" p0_MS["<<i<<"] " <<m_p0_MS[i]
	     <<"; p1_MS["<<i<<"] " <<m_p1_MS[i]
	     <<"; p2_MS["<<i<<"] "<<m_p2_MS[i]<<endl;

    if(Trel < REL17_2_SUM13){
    	cout<<" E_p1_ID["<<i<<"] "<<m_E_p1_ID[i]
    		 <<"; E_p2_ID["<<i<<"] "<<m_E_p2_ID[i]
    		 <<"; E_p2_ID_TAN["<<i<<"] "<<m_E_p2_ID_TAN[i]<<endl;
    	cout<<" E_p0_MS["<<i<<"] "<<m_E_p0_MS[i]
    		 <<"; E_p1_MS["<<i<<"] "<<m_E_p1_MS[i]
    		 <<"; E_p2_MS["<<i<<"] "<<m_E_p2_MS[i]<<endl;
	
    	cout<<"  S_p1_ID["<<i<<"] "<<m_S_p1_ID[i]
    	     <<"; S_p2_ID["<<i<<"] "<<m_S_p2_ID[i]
    		 <<"; S_p2_ID_TAN["<<i<<"] "<<m_S_p2_ID_TAN[i]<<endl;
    	cout<<" S_p0_MS["<<i<<"] "<<m_S_p0_MS[i]
    		 <<"; S_p1_MS["<<i<<"] "<<m_S_p1_MS[i]
    		 <<"; S_p2_MS["<<i<<"] "<<m_S_p2_MS[i]<<endl;
    } else {
    	cout<<"  SUp_p1_ID["<<i<<"] "<<m_SUp_p1_ID[i]
    		 <<"; SUp_p2_ID["<<i<<"] "<<m_SUp_p2_ID[i]
    		 <<"; SUp_p2_ID_TAN["<<i<<"] "<<m_SUp_p2_ID_TAN[i]<<endl;
    	cout<<" SUp_p0_MS["<<i<<"] "<<m_SUp_p0_MS[i]
    		 <<"; SUp_p1_MS["<<i<<"] "<<m_SUp_p1_MS[i]
    		 <<"; SUp_p2_MS["<<i<<"] "<<m_SUp_p2_MS[i]<<endl;
	
    	cout<<"  SDw_p1_ID["<<i<<"] "<<m_SDw_p1_ID[i]
    		 <<"; SDw_p2_ID["<<i<<"] "<<m_SDw_p2_ID[i]
    		 <<"; SDw_p2_ID_TAN["<<i<<"] "<<m_SDw_p2_ID_TAN[i]<<endl;
    	cout<<" SDw_p0_MS["<<i<<"] "<<m_SDw_p0_MS[i]
    		 <<"; SDw_p1_MS["<<i<<"] "<<m_SDw_p1_MS[i]
    		 <<"; SDw_p2_MS["<<i<<"] "<<m_SDw_p2_MS[i]<<endl;
    }

    cout<<" MC_p1_ID["<<i<<"] "<<m_MC_p1_ID[i]
	     <<"; MC_p2_ID["<<i<<"] "<<m_MC_p2_ID[i]
	     <<"; MC_p2_ID_TAN["<<i<<"] "<<m_MC_p2_ID_TAN[i] << endl;
    cout<<" MC_p0_MS["<<i<<"] "<<m_MC_p0_MS[i]
	     <<"; MC_p1_MS["<<i<<"] "<<m_MC_p1_MS[i]
	     <<"; MC_p2_MS["<<i<<"] "<<m_MC_p2_MS[i]<<endl;
    
    cout<<" CorrMatC0["<<i<<"] "<<m_CorrMatC0[i]
	     <<"; CorrMatC1["<<i<<"] "<<m_CorrMatC1[i]
	     <<"; CorrMatC2["<<i<<"] "<<m_CorrMatC2[i]
	     <<"; CorrMatC3["<<i<<"] "<<m_CorrMatC3[i]
	     <<"; CorrMatC4["<<i<<"] "<<m_CorrMatC4[i]
	     <<"; CorrMatC5["<<i<<"] "<<m_CorrMatC5[i]<<endl;

    cout<<" CorrMatTanC0["<<i<<"] "<<m_CorrMatTanC0[i]
	     <<"; CorrMatTanC1["<<i<<"] "<<m_CorrMatTanC1[i]
	     <<"; CorrMatTanC2["<<i<<"] "<<m_CorrMatTanC2[i]
	     <<"; CorrMatTanC3["<<i<<"] "<<m_CorrMatTanC3[i]
	     <<"; CorrMatTanC4["<<i<<"] "<<m_CorrMatTanC4[i]
	     <<"; CorrMatTanC5["<<i<<"] "<<m_CorrMatTanC5[i]<<endl;
  }
}

int SmearingClass::GetScaleRegion(){
  
  if(m_scaleBins.empty()) return -1;
  double _min = m_scaleBins[0];
  double _max = m_scaleBins[m_scaleBins.size()-1];
  if( eta<_min ) { return 0; }
  if( eta>=_max ) { return m_scaleBins.size() - 1; }
  vector<double>::iterator lb = lower_bound(m_scaleBins.begin(),m_scaleBins.end(),eta+1e-9); // [first,last)
  return( min( static_cast<int>( distance(m_scaleBins.begin(),lb) ) , static_cast<int>(m_scaleBins.size()-1) ) );
}

void SmearingClass::FillScales(string Scaletype, bool ApplyScalesToData){
  
  m_ScaleType=Scaletype;
  /*Checking momentum scaling type*/
  if(m_ScaleType==""     || m_ScaleType=="default" || 
     m_ScaleType=="kpkm" || m_ScaleType=="KC"      || 
     m_ScaleType=="K"    || m_ScaleType=="C" ){
    
    cout<<"SmearingClass INFO:: using " << m_ScaleType << " Scaling "<<endl;
  }
  else{
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: wrong Scale Type in input "<<m_ScaleType<<endl;
    cout<<"                      possible values are: {default,KC,K,C,kpkm} "<<endl;
    return;
  }
  /* Clean up old scale arrays */
  CleanScales();
  
  /* old non-charge-separated scales have already been loaded */
  if(m_ScaleType=="default" || m_ScaleType=="") return;
  /* Choose MuID or Staco Algorithm Values */
  string tmpname;
  /* correct charge-dependent momentum scale (k+ and k-) */
  string scale_val  = m_Fdir+m_DataYear+"_Scale_kpkm_values_"+m_Algorithm+"_"+m_Release+".dat";
  
  /* correct overall momentum scale and charge-dependent split (k and C) */
  if(m_ScaleType=="KC" || m_ScaleType=="K" || m_ScaleType=="C") {
    scale_val  = m_Fdir+m_DataYear+"_Scale_KC_values_"+m_Algorithm+"_"+m_Release+".dat";
  }
  cout<<"SmearingClass INFO:: loading scales from: " << scale_val << endl;
  apply_to_data = ApplyScalesToData;

  /* filling scale or shift vectors*/
  InValues.open(scale_val.c_str());
  if(!InValues.good()){
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: File "<<scale_val<<" NOT FOUND!!"<<endl;
    cout<<"                      Please provide a correct path!"<<endl;
    return;
  }
  if(m_ScaleType=="kpkm") Tscale=SCALE_KPKM;
  else if(m_ScaleType=="KC") Tscale=SCALE_KC;
  else if(m_ScaleType=="K") Tscale=SCALE_K;
  else if(m_ScaleType=="C") Tscale=SCALE_C;

  // skip information line
  getline(InValues,tmpname);

  // retrieve binning
  getline(InValues,tmpname);
  istringstream iss(tmpname);
  iss >> tmpname; // skip lower bound of first bin (since it won't be used)
  copy(istream_iterator<double>(iss), istream_iterator<double>(), back_inserter(m_scaleBins));
  cout << "Loading muon momentum scales in " << m_scaleBins.size() << " bins" << endl;

  string curname = "cmb";
  double v1,v2,v3,w1,w2,w3;
  int ibin=0;
  while( getline(InValues,tmpname) ) {
    if(tmpname=="cmb" || tmpname=="exms" || tmpname=="id") {
      curname=tmpname;
      continue;
    }
    istringstream iss(tmpname);
    iss >> v1 >> v2 >> v3 >> w1 >> w2 >> w3;
    const double ss = restrictCurvCorrSigma;
    if(curname=="cmb") {
      m_S1_CB.push_back(v1);
      m_S1Corr_CB.push_back(v2);
      m_S1ACorr_CB.push_back(v3);
      // special handling is user requested to restrict curvature
      // corrections to bins where they are significant
      if(Tscale>=SCALE_KC && ss>0) {
	m_S2_CB.push_back(fabs(w1)>=w2*ss ? w1 : 0.0);
      }
      else m_S2_CB.push_back(w1);
      m_S2Corr_CB.push_back(w2);
      m_S2ACorr_CB.push_back(w3);
    }
    if(curname=="id") {
      m_S1_ID.push_back(v1);
      m_S1Corr_ID.push_back(v2);
      m_S1ACorr_ID.push_back(v3);
      // special handling is user requested to restrict curvature
      // corrections to bins where they are significant
      if(Tscale>=SCALE_KC && ss>0) {
	m_S2_ID.push_back(fabs(w1)>=w2*ss ? w1 : 0.0);
      }
      else m_S2_ID.push_back(w1);
      m_S2Corr_ID.push_back(w2);
      m_S2ACorr_ID.push_back(w3);
    }
    if(curname=="exms") {
      m_S1_MS.push_back(v1);
      m_S1Corr_MS.push_back(v2);
      m_S1ACorr_MS.push_back(v3);
      // special handling is user requested to restrict curvature
      // corrections to bins where they are significant
      if(Tscale>=SCALE_KC && ss>0) {
	m_S2_MS.push_back(fabs(w1)>=w2*ss ? w1 : 0.0);
      }
      else m_S2_MS.push_back(w1);
      m_S2Corr_MS.push_back(w2);
      m_S2ACorr_MS.push_back(w3);
    }
    ibin++;
  }
  assert(m_S1_MS.size() == m_S1_ID.size());
  assert(m_S1_MS.size() == m_S1_CB.size());
  assert(m_S1_MS.size() == m_scaleBins.size());

  InValues.close();
  InValues.clear();  
}

void SmearingClass::SetSeed(int seed) {
  m_rand->SetSeed(seed);
}

void SmearingClass::SetSeed(int evtnum,
			    int muon_index,
			    int offset) {
  
  m_rand->SetSeed(offset + evtnum + muon_index*100);  
}

void SmearingClass::UseTan2(bool U) {
  /* use pT/tan(theta)^2 parameterization for CSC region */   
  useTan2=U;
}

void SmearingClass::UseGeV() {
  /* if input momenta are in GeV */  
  GeV=1;
}

void SmearingClass::UseScale(bool applyScale) {
  /* also use momentum scale, set as default */  
  useScale=applyScale;
}

void SmearingClass::RestrictCurvatureCorrections(double nsigma) {
  /* if "KC" or "C" scale corrections are used,
     the curvature shift correction is applied only in bins
     where the correction has a >2.5 sigma effect
  */

  assert(m_scaleBins.empty() && "ERROR: RestrictCurvatureCorrections() must be called BEFORE FillScales()");
  restrictCurvCorrSigma = nsigma;
}

void SmearingClass::UseImprovedCombine() {
  /* also use improved combine */  
  useImprovedCombine = true;
}

void SmearingClass::ApplyToData(bool U) {
  /* use pT/tan(theta)^2 parameterization for CSC region */ 
  
  apply_to_data = U;
}

void SmearingClass::Event(const TLorentzVector &p, 
			  string DetType,
                          double Charge) {
  Event(p.Pt(), p.Eta(), DetType, Charge, p.Phi());
  return;
}

void SmearingClass::Event(double Pt,
			  double Eta,
			  string DetType,
			  double Charge,
			  double Phi) {  
  /* MS or ID smearing only. DetType="MS","ID" */  
  ptms=0;
  ptid=0;
  ptcb=0;
  eta = Eta;  
  phi = Phi;
  charge = Charge;

  if(DetType=="MS"){ 
    Tdet = DET_MS;
    ptms = Pt/GeV;
  }
  else if(DetType=="ID"){
    Tdet = DET_ID;
    ptid = Pt/GeV;
  }
  else cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: wrong DetType in input "<<DetType<<endl;
  Event();
}


void SmearingClass::Event(const TLorentzVector & p_MS,
			  const TLorentzVector & p_ID,
			  const TLorentzVector & p_CB,
			  double Charge) {  
  Event(p_MS.Pt(),p_ID.Pt(), p_CB.Pt(), p_CB.Eta(), Charge, p_CB.Phi());
  return;  
}

void SmearingClass::Event(double PtMS,
			  double PtID,
			  double PtCB,
			  double Eta,
			  double Charge,
			  double Phi) {  
    /* For full MS ID and CB smearing */
    ptms = 0.;
    ptid = 0.;
    ptcb = 0.;
    eta = Eta;
    phi = Phi;
    charge = Charge;
    
    if(PtCB <= 0. ){
	cout<<__FILE__<<" line:"<<__LINE__<<" SmearingClass ERROR: pTCB="<<PtCB
	    <<"( <=0 ) ... CB smearing IS NOT POSSIBLE!"<<endl; 
	exit(-1);
    }
    if (PtID <= 0. && PtMS <= 0.){
	cout<<__FILE__<<" line:"<<__LINE__<<" SmearingClass ERROR: both pTID & pTMS <=0 ... smearing IS NOT POSSIBLE!"<<endl;
	exit(-1);
    }
    
    if(PtMS <= 0. || PtID <= 0.){
	cout<<__FILE__<<" line:"<<__LINE__<<" SmearingClass WARNING: pTMS="<<PtMS<<" or pTID="<<PtID
	    <<" are <= 0 ... may happen sometimes, CB smearing will proceed."<<endl;
    }
        
    Tdet = DET_CB;
    ptms = (PtMS > 0.) ? PtMS/GeV : 0.;
    ptid = (PtID > 0.) ? PtID/GeV : 0.;
    ptcb = PtCB/GeV;
    Event();
}

void SmearingClass::Event() {
  smearDeltaMS = 0.;
  smearDeltaID = 0.;
  smearDeltaCB = 0.;
  detRegion = -1;
  if(phi == defaultInit && Trel == REL17_2_SUM13) 
      cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: You need phi of the muon for after Rel17.2Sum13!"<<endl;

  /* Detector Region */
  if(Trel<REL17){
    detRegion = getRegionOld(eta);
  } else{
      if(phi == defaultInit && Trel < REL17_2_SUM13) {
	  //Lower release have just Eta bins, so usually the user doesn't input
	  //Phi and 0 is a good default for these regions	
  detRegion = DetRegions->GetRegion(eta,0);
      } else {
	  detRegion = DetRegions->GetRegion(eta,phi);
      }
  }
  if(detRegion ==-1){
      cerr<<__FILE__<<" "<<__LINE__<<" WARNING: can't find detRegion! Set smearing to 0."<<endl;
      return;
  }

  /* Scale Region */
  scaleRegion = GetScaleRegion();
  if(dbg) cout<<"Eta = "<<eta<<"; phi = "<<phi<<endl
		   <<" detRegion="<<detRegion<<" ("<<DetRegions->GetRegionName(detRegion)
		   <<"); scaleRegion="<<scaleRegion<<endl;
  
  /* If SmearingClass is applied to data, smearing makes no sense
     (there is no way to "unsmear" the data to make it match MC)
     However, the momentum scale or curvature corrections are still valid
     and will be applied accordingly in pTCB, pTMS, pTID methods.
   */
  if(apply_to_data) return;

  g0 = m_rand->Gaus(0,1);  
  g1 = m_rand->Gaus(0,1);
  g2 = m_rand->Gaus(0,1);
  g3 = m_rand->Gaus(0,1);
  g4 = m_rand->Gaus(0,1);

  /** Definition: deltaSmear = 1+smearDET **/
  smearDeltaMS = Smearing(DET_MS);
  smearDeltaID = Smearing(DET_ID);
  if(dbg) cout<<"g0="<<g0<<"; g1="<<g1<<"; g2="<<g2<<"; g3="<<g3<<"; g4="<<g4<<endl
		   <<"smearDeltaMS="<<smearDeltaMS<<"; smearDeltaID="<<smearDeltaID<<endl;
 
  //Calculate here the weights for the CB smearing
  if (Tdet==DET_CB){ 
      CalcCBWeights(); 
      smearDeltaCB = smearDeltaID*weightID + smearDeltaMS*weightMS;
  }

  if(useErrMatrix) ErrorMatrix();
}

double SmearingClass::Smearing(int DetType) {
    if (detRegion<0 || detRegion>=m_nb_regions) return 0; //++++++ HOW TO IMPROVE THIS CHECK?!
    double smear =0.;
  if (DetType==DET_MS){
      if(ptms==0){ 
	  return 0;
      } else {
	  smear =  m_p0_MS[detRegion]*g0/ptms + m_p1_MS[detRegion]*g1 + m_p2_MS[detRegion]*g2*ptms;
	  if(dbg) cout<<"m_p0_MS[detRegion]:"<<m_p0_MS[detRegion]<<"; m_p1_MS[detRegion]:"<<m_p1_MS[detRegion]<<"; m_p2_MS[detRegion]="<<m_p2_MS[detRegion]
			   <<"; ptms="<<ptms<<"; smear:"<<smear<<endl;
	  return smear;
      }
  } else if(DetType==DET_ID){
      if (useTan2 && m_p2_ID_TAN[detRegion] != 0.){
	  smear = m_p1_ID[detRegion]*g3 + m_p2_ID_TAN[detRegion]*g4*ptid*sinh(eta)*sinh(eta);
	  if(dbg) cout<<"m_p1_ID[detRegion]:"<<m_p1_ID[detRegion]<<"; m_p2_ID_TAN[detRegion]="<<m_p2_ID_TAN[detRegion]
			   <<"; ptid="<<ptid<<"; smearTAN:"<<smear<<endl;
	  return smear;
      } else{
	  smear = m_p1_ID[detRegion]*g3 + m_p2_ID[detRegion]*g4*ptid;
	  if(dbg) cout<<"m_p1_ID[detRegion]:"<<m_p1_ID[detRegion]<<"; m_p2_ID[detRegion]="<<m_p2_ID[detRegion]
			   <<"; ptid="<<ptid<<"; smear:"<<smear<<endl;
	  return smear;
      }
  } else{ 
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: DetType not defined "<<DetType<<endl;
  }
  return 0;
}

//Smearing with systematic variation not using the error matrix (no correlations)
double SmearingClass::SmearingSYSVar(int DetType, double var){
  
    if(var != 1. && var != -1 ){
	cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: sys variation not +1 not -1 "<<endl;
	return 0;
    }
    if (detRegion<0 || detRegion>=m_nb_regions) return 0; 
    
    double p0_MS_var=0., p1_MS_var=0., p2_MS_var=0., p1_ID_var=0., p2_ID_var=0., p2_ID_TAN_var=0.;   
    double newSmear = 0.;
    if (DetType==DET_MS){
	if(ptms==0){  return 0;
	} else {
	    if(Trel < REL17_2_SUM13){
		p0_MS_var = pow(m_E_p0_MS[detRegion]*m_E_p0_MS[detRegion]+m_S_p0_MS[detRegion]*m_S_p0_MS[detRegion],0.5);
		p1_MS_var = pow(m_E_p1_MS[detRegion]*m_E_p1_MS[detRegion]+m_S_p1_MS[detRegion]*m_S_p1_MS[detRegion],0.5);
		p2_MS_var = pow(m_E_p2_MS[detRegion]*m_E_p2_MS[detRegion]+m_S_p2_MS[detRegion]*m_S_p2_MS[detRegion],0.5);
	    } else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
		p0_MS_var = var > 0. ? m_SUp_p0_MS[detRegion] : m_SDw_p0_MS[detRegion];
		p1_MS_var = var > 0. ? m_SUp_p1_MS[detRegion] : m_SDw_p1_MS[detRegion];
		p2_MS_var = var > 0. ? m_SUp_p2_MS[detRegion] : m_SDw_p2_MS[detRegion];
	    }
	    
	    p0_MS_var = m_p0_MS[detRegion] + var*p0_MS_var;
	    p1_MS_var = m_p1_MS[detRegion] + var*p1_MS_var;
	    p2_MS_var = m_p2_MS[detRegion] + var*p2_MS_var;
	    
	    if(p0_MS_var < 0. ) p0_MS_var = 0.; //Truncation of unphysical variations
	    if(p1_MS_var < 0. ) p1_MS_var = 0.;
	    if(p2_MS_var < 0. ) p2_MS_var = 0.;
	    newSmear = (p0_MS_var*g0/ptms + p1_MS_var*g1 + p2_MS_var*g2*ptms );
	    if(dbg) cout<<"p0_MS_var="<<p0_MS_var<<"; p1_MS_var="<<p1_MS_var<<"; p2_MS_var="<<p2_MS_var
		      <<"; new MS Smear="<<newSmear<<endl;
	    return newSmear;
	}
    } else if(DetType==DET_ID){
	
	if(Trel < REL17_2_SUM13){
	    p1_ID_var     = pow(m_E_p1_ID[detRegion]*m_E_p1_ID[detRegion]+m_S_p1_ID[detRegion]*m_S_p1_ID[detRegion],0.5);
	    p2_ID_var     = pow(m_E_p2_ID[detRegion]*m_E_p2_ID[detRegion]+m_S_p2_ID[detRegion]*m_S_p2_ID[detRegion],0.5);
	    p2_ID_TAN_var = pow(m_E_p2_ID_TAN[detRegion]*m_E_p2_ID_TAN[detRegion]+m_S_p2_ID_TAN[detRegion]*m_S_p2_ID_TAN[detRegion],0.5);
	} else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
	    p1_ID_var     = var > 0. ? m_SUp_p1_ID[detRegion] : m_SDw_p1_ID[detRegion];
	    p2_ID_var     = var > 0. ? m_SUp_p2_ID[detRegion] : m_SDw_p2_ID[detRegion];
	    p2_ID_TAN_var = var > 0. ? m_SUp_p2_ID_TAN[detRegion] : m_SDw_p2_ID_TAN[detRegion];
	}
	
	p1_ID_var     = m_p1_ID[detRegion] + var*p1_ID_var;
	p2_ID_var     = m_p2_ID[detRegion] + var*p2_ID_var;
	p2_ID_TAN_var = m_p2_ID_TAN[detRegion] + var*p2_ID_TAN_var;
					    
	if(p1_ID_var < 0. ) p1_ID_var = 0.; //Truncation of unphysical variations
	if(p2_ID_var < 0. ) p2_ID_var = 0.;
	if(p2_ID_TAN_var < 0. ) p2_ID_TAN_var = 0.;
    
	if (useTan2 && m_p2_ID_TAN[detRegion]!=0){	    
	    newSmear = (p1_ID_var*g3 + p2_ID_TAN_var*g4*ptid*sinh(eta)*sinh(eta) );
	} else {
	    newSmear = (p1_ID_var*g3 + p2_ID_var*g4*ptid );
	}
	
	if(dbg) cout<<"p1_ID_var="<<p1_ID_var<<"; p2_ID_var="<<p2_ID_var<<"; p2_ID_TAN_var="<<p2_ID_TAN_var
		    <<"; sinh(eta)^2="<<sinh(eta)*sinh(eta)<<"; new ID Smear="<<newSmear<<endl;
	return newSmear;
    } else{ 
	cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: DetType not defined "<<DetType<<endl;
    }
    return 0;
}

double SmearingClass::ExpectedResolution(const string &DetType, const bool mc) const {  
  if(DetType=="MS"){ 
    return ExpectedResolution(DET_MS,mc);
  }
  else if(DetType=="ID"){
    return ExpectedResolution(DET_ID,mc);
  }
  else if(DetType=="CB"){
    return ExpectedResolution(DET_CB,mc);
  }
  else {
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: wrong DetType in input "<<DetType<<endl;
    return 0.;
  }
}

double SmearingClass::ExpectedResolution(const int DetType, const bool mc) const {
  
  /** do the average with the EXPECTED resolutions **/
  if (detRegion<0 || detRegion>=m_nb_regions) return 0; 
  double expRes = 0.;
  if (DetType == DET_MS) {
    if (ptms==0) cout<<__FILE__<<" line:"<<__LINE__<<" SmearingClass WARNING:: ptms==0"<<endl;
    double p0 = mc ? m_MC_p0_MS[detRegion] : (m_MC_p0_MS[detRegion] + m_p0_MS[detRegion]);
    double p1 = mc ? m_MC_p1_MS[detRegion] : (m_MC_p1_MS[detRegion] + m_p1_MS[detRegion]);
    double p2 = mc ? m_MC_p2_MS[detRegion] : (m_MC_p2_MS[detRegion] + m_p2_MS[detRegion]);
    expRes =  sqrt( pow(p0/ptms, 2) + pow(p1, 2) + pow(p2*ptms ,2));
    if(dbg) cout<<__LINE__<<" Std MS RES: "<<expRes<<endl;
    return expRes; //+++++No SYS!!!
  } else if (DetType == DET_ID) {
    if (ptid==0) cout<<__FILE__<<" line:"<<__LINE__<<" SmearingClass WARNING:: ptid==0"<<endl;

    if(useIDResoFunc){ 
	//Use the analytic ID resolution function developed by the Brandeis group: 
	// see BrandeisMomentumResolution.h for additional details
	expRes = Brandeis::GetMomentumResolutionID( ptid, eta, mc)/(ptid);
	if(dbg) cout<<__LINE__<<" Brandeis ID RES: "<<expRes<<endl;
	return expRes;//+++++No SYS!!!
    } else {//ID resolution parametrization extracted with gaussian fits on Muon MC and corrected with delta p1, delta_p2 terms
	double p1 = mc ? m_MC_p1_ID[detRegion] : (m_MC_p1_ID[detRegion] + m_p1_ID[detRegion]);
	double p2 = mc ? m_MC_p2_ID[detRegion] : (m_MC_p2_ID[detRegion] + m_p2_ID[detRegion]);
	if (m_MC_p2_ID_TAN[detRegion] != 0 && useTan2) {
	    p2 = mc ? m_MC_p2_ID_TAN[detRegion] : (m_MC_p2_ID_TAN[detRegion] + m_p2_ID_TAN[detRegion]);
	    p2 = p2*sinh(eta)*sinh(eta);
	}
	expRes = sqrt( pow(p1, 2) + pow( p2*ptid ,2));
	if(dbg) cout<<__LINE__<<" Std ID RES: "<<expRes<<endl;
	return expRes; //+++++No SYS!!!
    }
  } else if (DetType == DET_CB) {
      if (ptcb==0) cout<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass WARNING:: ptcb==0"<<endl;//+++++++
      // Due to complicated maths, the expected combined resolution
      // is given by this equation (note: all sigmas are fractional uncertainties):
      // sigma_CB = sqrt(2) * sigma_ID * sigma_MS * pTMS * pTID / {pTCB * sqrt({sigma_ID*pTID}^2 + {sigma_MS*pTMS}^2)}
      
      // Do a little recursive calling to make things easier to read
      // Turn these into *absolute* uncertainties to make life easier
      double sigmaID = ExpectedResolution(DET_ID, mc) * ptid;
      double sigmaMS = ExpectedResolution(DET_MS, mc) * ptms;
    
      double denominator = (ptcb) * sqrt(sigmaID*sigmaID + sigmaMS*sigmaMS);
      
      return denominator ? sqrt(2.) * sigmaID * sigmaMS / denominator : 0.;
  }
  else {
    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR: wrong DetType in input "<<DetType<<endl;
    return 0.;
  }
}
  
void SmearingClass::CalcCBWeights(){  
    //Calculate the weights of ID and MS used for CB. Two ways are implemented: 
    // - weight by ^2 of exp reso
    // - from equation pT_CB = R(pT_ID) + (1-R)(pT_MS)
    //(weights should be correctly normalized)

    weightID = 0.5; weightMS = 0.5;
    if (detRegion < 0 || detRegion >= m_nb_regions) return; 
    if (ptcb==0) { 
	cout<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass WARNING:: ptcb==0"<<endl;
	return; 
    }
    const bool doNotAddSmearing = true;
    double SigmaMS = ExpectedResolution(DET_MS, doNotAddSmearing); 
    double SigmaID = ExpectedResolution(DET_ID, doNotAddSmearing);
    if(dbg) cout<<"SigmaMS="<<SigmaMS<<"; SigmaID="<<SigmaID<<endl;
    double Nsigma = useNsigmaForICombine;

    if(useImprovedCombine){
	if(dbg) cout<<"fabs(ptcb-ptms)="<<fabs(ptcb-ptms)<<"; SigmaMS*ptcb="<<SigmaMS*ptcb
			 <<"; fabs(ptcb-ptid)="<<fabs(ptcb-ptid)<<"; SigmaID*ptid="<<SigmaID*ptid<<endl;
	if(fabs(ptcb-ptms) > Nsigma*SigmaMS*ptcb || fabs(ptcb-ptid) > Nsigma*SigmaID*ptcb) {
	    double R=1, Rplus;  
	    if(fabs(ptcb-ptms) == fabs(ptcb-ptid)) return;//This case returns weightID = weightMS = 0.5
	    if(fabs(ptcb-ptms) != 0 && fabs(ptcb-ptms) > fabs(ptcb-ptid)) {
		R = (ptid - ptcb)/(ptcb - ptms); /* R~wMS/wID */
		Rplus = 1 + R;
		if (Rplus != 0 && R > 0) {
		    weightID = 1/Rplus;    
		    weightMS = R/Rplus;    
		    return;
		}
	    }
	    if (fabs(ptcb-ptid) != 0 && fabs(ptcb-ptms) < fabs(ptcb-ptid)){
		R = (ptms - ptcb)/(ptcb - ptid); /* R~wID/wMS */
		Rplus = 1 + R;
		if (Rplus != 0 && R > 0) {
		    weightID = R/Rplus;
		    weightMS = 1/Rplus;
		    return;
		}
	    }
	}
    }

    //Standard combination with weighting of expected resolution squared: this is default if useImprovedCombine == false
    // and we are not in a pathological case
    double wMS = ptms/ptcb/pow(SigmaMS,2);
    double wID = ptid/ptcb/pow(SigmaID,2);
    weightID =  wID/(wMS + wID);
    weightMS =  wMS/(wMS + wID);

    if(dbg) cout<<__LINE__<<" wMS="<<wMS<<"; wID="<<wID<<endl;
    return;
}

void SmearingClass::ErrorMatrix() {
  //THIS FUNCTION NEED TO BE CAREFULLY REVIEWED, IT DOESN'T WORK AT THE MOMENT! +++++++++++
  if (detRegion<0 || detRegion>=m_nb_regions) return; 
    
  vms=0; vid=0; corr=0;
  double s1=0, s2=0, s3=0, s4=0;

  // tan2 parametrization only applies in CSC region
  if (!useTan2 || m_E_p2_ID_TAN[detRegion]==0) {
    if(ptms==0){
      vms=0;
    } else{
      s1 = pow(m_E_p1_MS[detRegion]*m_E_p1_MS[detRegion]+
	       m_S_p1_MS[detRegion]*m_S_p1_MS[detRegion],0.5);
      
      s2 = pow(m_E_p2_MS[detRegion]*m_E_p2_MS[detRegion]+
	       m_S_p2_MS[detRegion]*m_S_p2_MS[detRegion],0.5);
      
      vms = g1*g1*s1*s1+
	g2*g2*s2*s2*ptms*ptms+
	2.0*g1*g2*ptms*s1*s2*m_CorrMatC0[detRegion];
      
      vms = pow(fabs(vms),0.5);
    }
    
    s3 = pow(m_E_p1_ID[detRegion]*m_E_p1_ID[detRegion]+
	     m_S_p1_ID[detRegion]*m_S_p1_ID[detRegion],0.5);
    
    s4 = pow(m_E_p2_ID[detRegion]*m_E_p2_ID[detRegion]+
	     m_S_p2_ID[detRegion]*m_S_p2_ID[detRegion],0.5);
    
    vid = g3*g3*s3*s3+g4*g4*s4*s4*ptid*ptid+
      2.0*g3*g4*s3*s4*ptid*m_CorrMatC5[detRegion];
    
    vid = pow(fabs(vid),0.5);
    
    if (vms*vid!=0) {
      corr = (g1*s1*(g3*s3*m_CorrMatC1[detRegion]+
		     g4*s4*ptid*m_CorrMatC2[detRegion])+
	      g2*s2*ptms*(g3*s3*m_CorrMatC3[detRegion]+
			      g4*s4*ptid*m_CorrMatC4[detRegion]))/vms/vid;
    }
  } else {
    if(ptms==0){
      vms=0;
    } else{
      s1 = pow(m_E_p1_MS[detRegion]*m_E_p1_MS[detRegion]+
	       m_S_p1_MS[detRegion]*m_S_p1_MS[detRegion],0.5);
      
      s2 = pow(m_E_p2_MS[detRegion]*m_E_p2_MS[detRegion]+
	       m_S_p2_MS[detRegion]*m_S_p2_MS[detRegion],0.5);
      
      vms = g1*g1*s1*s1+
	g2*g2*s2*s2*ptms*ptms+
	2.0*g1*g2*ptms*s1*s2*m_CorrMatTanC0[detRegion];
      
      vms = pow(fabs(vms),0.5);
    }
    
    s3 = pow(m_E_p1_ID[detRegion]*m_E_p1_ID[detRegion]+
	     m_S_p1_ID[detRegion]*m_S_p1_ID[detRegion],0.5);
    
    s4 = pow(m_E_p2_ID_TAN[detRegion]*m_E_p2_ID_TAN[detRegion]+
	     m_S_p2_ID_TAN[detRegion]*m_S_p2_ID_TAN[detRegion],0.5);
    
    vid = g3*g3*s3*s3+g4*g4*s4*s4*ptid*ptid*sinh(eta)*sinh(eta)*sinh(eta)*sinh(eta)
      +2.0*g3*g4*s3*s4*ptid*sinh(eta)*sinh(eta)*m_CorrMatTanC5[detRegion];
    
    vid = pow(fabs(vid),0.5);
    
    if (vms*vid!=0) {
      corr = (g1*s1*(g3*s3*m_CorrMatTanC1[detRegion]+g4*s4*ptid*sinh(eta)*sinh(eta)*m_CorrMatTanC2[detRegion])+
	      g2*s2*ptms*(g3*s3*m_CorrMatTanC3[detRegion]+g4*s4*ptid*sinh(eta)*sinh(eta)*m_CorrMatTanC4[detRegion]))/vms/vid;
    }
  }
}

double SmearingClass::pTID() const{
  return GeV*pTCalc(DET_ID);
}
double SmearingClass::pTMS() const{
  return GeV*pTCalc(DET_MS);
}
double SmearingClass::pTCB() const{
    return GeV*pTCalc(DET_CB);
}

//This is used internally to calculate the smeared and scaled pT for ID, MS, CB.
// - inSmear (default is defaultInit): used to reply the calculation with the sys varied
// smearing for ID and MS: the new smearing is calculate outside and here applyed correctly.
// - scaleVar (default = 0.): +/- 1 is used for the the sys scale variation, the varied 
// scale is extracted here as it can happen in several flavours.
double SmearingClass::pTCalc(const int DetType, const double inSmearID, const double inSmearMS, const double scaleVar) const {   

    //////////////////////////////////////////////////////////////////////////////////////////
    //Scales have not been applied to the present smearings, get them before proceeding
    //////////////////////////////////////////////////////////////////////////////////////////
    double scaleID = 0., enLossCorrMS = 0., scaleMS = 0., scaleCB = 0.;//initialize all to 0
    //These are alternative scale corrections (KPKM,KC,K,C) they are != 0. if Tscale != SCALE_DEFAULT.
    double s1_ID = 0., s2_ID = 0., s1_MS = 0., s2_MS = 0., s1_CB = 0., s2_CB = 0.;//Description of these is in ApplyScale

    if(fabs(scaleVar) != 1. && scaleVar != 0.) 
	cerr<<__FILE__<<" "<<__LINE__<<": ERROR! Unpredicted scale variation of Delta "<<scaleVar<<" sigmas!"<<endl;

    if(Tscale == SCALE_DEFAULT){
	if(m_scale_ID[detRegion] != -1){
	    if(Trel >= REL17_2_SUM13) {
		scaleID = scaleVar > 0. ? m_scaleSystUp_ID[detRegion] : m_scaleSystDw_ID[detRegion];
	    } else {
		scaleID = m_scaleSyst_ID[detRegion];
	    }
	    scaleID = m_scale_ID[detRegion] + scaleVar*scaleID;
	} else {
	    scaleID = (Trel >= REL17_2_SUM13) ? 0. : 1.;
	}
	if(m_scale_MS[detRegion] != -1){
	    if(Trel >= REL17_2_SUM13) {
		scaleMS = scaleVar > 0. ? m_scaleSystUp_MS[detRegion] : m_scaleSystDw_MS[detRegion];
		enLossCorrMS = scaleVar > 0. ? m_enLossSystUp_MS[detRegion] : m_enLossSystDw_MS[detRegion];
	    } else {
		scaleMS = m_scaleSyst_MS[detRegion];
	    }

	    scaleMS =  m_scale_MS[detRegion] + scaleVar*scaleMS;
	    if(useEnLossMSCorr) enLossCorrMS = m_enLoss_MS[detRegion] + scaleVar*enLossCorrMS;
	} else {
	    scaleMS = (Trel >= REL17_2_SUM13) ? 0. : 1.;
	}
	if(Trel >= REL17_2_SUM13){//NB: REL17_2_SUM13 has scale parametrized around 0, not 1!!
	    scaleID += 1.; scaleMS += 1.;
	}

	if(m_scale_CB[detRegion] != -1){
	    scaleCB = m_scale_CB[detRegion] + scaleVar*m_scaleSyst_CB[detRegion];
	} else {
	    scaleCB = (scaleID*weightID + (scaleMS + enLossCorrMS/ptms)*weightMS);
	}
	if(dbg) 
	    cout<<__FILE__<<" "<<__LINE__<<": weightID="<<weightID<<"; weightMS="<<weightMS
		     <<"; enLossCorrMS="<<enLossCorrMS<<"; scaleID="<<scaleID<<"; scaleMS="<<scaleMS<<"; scaleCB="<<scaleCB<<endl;
    } else {
	if(unsigned(scaleRegion) >= m_S1_CB.size() || unsigned(scaleRegion) >= m_S2_CB.size()){
	    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: "<<scaleRegion<<" length of scaleRegion1: "<<m_S1_CB.size()<<endl;
	    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"SmearingClass ERROR:: "<<scaleRegion<<" length of scaleRegion2: "<<m_S2_CB.size()<<endl;
	    return -1;	
	}

	s1_ID = m_S1_ID[scaleRegion]; s2_ID = m_S2_ID[scaleRegion];
	s1_MS = m_S1_MS[scaleRegion]; s2_MS = m_S2_MS[scaleRegion];
	s1_CB = m_S1_CB[scaleRegion]; s2_CB = m_S2_CB[scaleRegion]; 
	//+++++ These variations were available for 2011 and should be checked again +++++++++++
	if(Tscale == SCALE_KC || Tscale == SCALE_K){
	    // here, we scale up or down the overall pT scale: called SCALE_K variation
	    s1_ID += scaleVar*m_S1Corr_ID[scaleRegion]; 
	    s1_MS += scaleVar*m_S1Corr_MS[scaleRegion]; 
	    s1_CB += scaleVar*m_S1Corr_CB[scaleRegion]; 
	    // here, we scale up or down the curvature offset: called SCALE_C variation
	    s2_ID += scaleVar*m_S2Corr_ID[scaleRegion];
	    s2_MS += scaleVar*m_S2Corr_MS[scaleRegion];
	    s2_CB += scaleVar*m_S2Corr_CB[scaleRegion];
	}
	if(Tscale == SCALE_KPKM ){
	    // m_S1Corr/m_S2Corr correspond to the component of kp/km error from Z mass fit: called SCALE_K variation
	    // m_S1ACorr/m_S2ACorr correspond to the component of kp/km error from R0 fit: called SCALE_C variation
	    s1_ID += scaleVar*m_S1ACorr_ID[scaleRegion] + scaleVar*m_S1Corr_ID[scaleRegion]; 
	    s2_ID += scaleVar*m_S2ACorr_ID[scaleRegion] - scaleVar*m_S2Corr_ID[scaleRegion];
	    s1_MS += scaleVar*m_S1ACorr_MS[scaleRegion] + scaleVar*m_S1Corr_MS[scaleRegion]; 
	    s2_MS += scaleVar*m_S2ACorr_MS[scaleRegion] - scaleVar*m_S2Corr_MS[scaleRegion];
	    s1_CB += scaleVar*m_S1ACorr_CB[scaleRegion] + scaleVar*m_S1Corr_CB[scaleRegion]; 
	    s2_CB += scaleVar*m_S2ACorr_CB[scaleRegion] - scaleVar*m_S2Corr_CB[scaleRegion];
	}
    }
    //////////////////////////////////////////////////////////////////////////////////////////


    double tmpDelta= 1.;
    double outPtID = ptid, outPtMS = ptms, outPtCB = ptcb;
    if(DetType == DET_ID){ 
	//Load the ID scale and smearing that you will use
	tmpDelta = (inSmearID == defaultInit) ? 1.+ smearDeltaID : 1.+ inSmearID;
	if(dbg)	cout<<__FILE__<<" "<<__LINE__
		    <<": tmpDelta="<<tmpDelta<<"; smearDeltaID="<<smearDeltaID<<"; scaleID="<<scaleID<<"; initial outPtID="<<outPtID<<endl;

	//In these releases the smearing was applied to the pt before the scale
	if( Trel < REL17_2_SUM13){
	    if(Tsmear == SMEAR_PT)  outPtID = outPtID*tmpDelta;
	    if(Tsmear == SMEAR_QPT) outPtID = (tmpDelta == 0) ? m_pTmax : outPtID/tmpDelta;
	}

	outPtID = ScaleApply(fabs(outPtID), s1_ID, s2_ID, scaleID);	

	if( Trel >= REL17_2_SUM13){
	    if(Tsmear == SMEAR_PT)  outPtID = outPtID*tmpDelta;
	    if(Tsmear == SMEAR_QPT) outPtID = (tmpDelta == 0) ? m_pTmax : outPtID/tmpDelta;	
	}
	return outPtID;
    }
    if(DetType == DET_MS){ 
	//Load the MS scale and smearing that you will use
	tmpDelta = (inSmearMS == defaultInit) ? 1.+ smearDeltaMS : 1.+ inSmearMS;
	if(dbg) cout<<__FILE__<<" "<<__LINE__
		    <<": tmpDelta="<<tmpDelta<<"; smearDeltaMS="<<smearDeltaMS<<"; scaleMS="<<scaleMS<<"; initial outPtMS="<<outPtMS<<endl;

	//In these releases the smearing was applied to the pt before the scale
	if( Trel < REL17_2_SUM13){
	    if(Tsmear == SMEAR_PT)  outPtMS = outPtMS*tmpDelta;
	    if(Tsmear == SMEAR_QPT) outPtMS = (tmpDelta == 0) ? m_pTmax : outPtMS/tmpDelta;
	}

	outPtMS = ScaleApply(fabs(outPtMS), s1_MS, s2_MS, scaleMS, enLossCorrMS);	    

	if( Trel >= REL17_2_SUM13){
	    if(Tsmear == SMEAR_PT)  outPtMS = outPtMS*tmpDelta;
	    if(Tsmear == SMEAR_QPT) outPtMS = (tmpDelta == 0) ? m_pTmax : outPtMS/tmpDelta;
	}
	return outPtMS;
    }

    if(DetType == DET_CB){
	if(inSmearID != defaultInit && inSmearMS != defaultInit) {
	    tmpDelta = 1. + inSmearID*weightID + inSmearMS*weightMS;
	} else if (inSmearID == defaultInit && inSmearMS == defaultInit) {
	    tmpDelta = 1. + smearDeltaCB;
	} else {
	    cout<<__FILE__<<" "<<__LINE__<<": ERROR! only one ID or MS var is defined for CB var!"<<endl;
	}
	if(dbg) cout<<__FILE__<<" "<<__LINE__
		    <<": tmpDelta="<<tmpDelta<<"; smearDeltaCB="<<smearDeltaCB<<"; scaleCB="<<scaleCB<<"; initial outPtCB="<<outPtCB<<endl;

	//In these releases the smearing was applied to the pt before the scale
	if( Trel < REL17_2_SUM13){
	    if(Tsmear == SMEAR_PT)  outPtCB = outPtCB*tmpDelta;
	    if(Tsmear == SMEAR_QPT) outPtCB = (tmpDelta == 0) ? m_pTmax : outPtCB/tmpDelta;
	}

	if(dbg) cout<<__FILE__<<" "<<__LINE__<<": tmpDelta="<<tmpDelta<<"; outPtCB="<<outPtCB<<endl;
	outPtCB = ScaleApply(fabs(outPtCB), s1_CB, s2_CB, scaleCB);

	if( Trel >= REL17_2_SUM13){
	    if(Tsmear == SMEAR_PT)  outPtCB = outPtCB*tmpDelta;
	    if(Tsmear == SMEAR_QPT) outPtCB = (tmpDelta == 0) ? m_pTmax : outPtCB/tmpDelta;
	}
	return outPtCB;
    }
    return 0.;
}


double SmearingClass::ScaleApply(const double pt, const double S1, const double S2, 
				 double S, const double S_enLoss) const{
    /* general-purpose function to apply scale/curvature corrections */
    if (detRegion<0 || detRegion>=m_nb_regions) return pt; 
    if(dbg) cout<<"detRegion="<<detRegion<<"; S="<<S<<"; S_EnLoss="<<S_enLoss<<"; S1="<<S1<<"; S2="<<S2<<endl;
    if(!useScale) return pt;
    // default momentum scale (derived form template fits)
    if(Tscale == SCALE_DEFAULT) {
	return apply_to_data ? pt/S : (pt*S + S_enLoss);
    }

    // charge-dependent scales: kp,km == pt(data)/pt(mc)
    if(Tscale == SCALE_KPKM) {
	if(charge == 0) {
	    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"ERROR: must provide muon charge via Event() to use advanced pT scaling" << endl;
	    return pt;
	}
	const double kpkm = (charge>0 ? S1 : S2)/100.0;
	return apply_to_data ? pt/kpkm : pt*kpkm;
    }
    // overall scale: K == pt(data)/pt(mc) and curvature split: 2*C == 1/pt+ - 1/pt-
    // both corrections can be applied one after another or one at a time
    if(Tscale >= SCALE_KC) {
	if(charge == 0) {
	    cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"ERROR: must provide muon charge via Event() to use advanced pT scaling" << endl;
	    return pt;
	}
	const double K = S1/100.0;
	const double C = S2/1e6;
	// scale pt (unless only curvature splitting is requested)
	const double ptsc = (Tscale==SCALE_C) ? pt : (apply_to_data ? pt/K : pt*K);
	// calculate curvature and apply a correction, if needed:
	double curv=1/ptsc;
	if(Tscale != SCALE_K) {
	    // if correcting a curvature offset in data:
	    if(apply_to_data) curv += charge<0 ? C  : -C;
	    // if putting a curvature offset into MC to better match the data
	    else curv += charge<0 ? -C  : +C;
	}
	return curv == 0 ? m_pTmax : fabs(1.0/curv);
    }
    return pt;
}


double SmearingClass::ChargeFlip(const double Smear) const{  
    if(Tsmear == SMEAR_PT){
	return 1;
    }
    else if (Tsmear == SMEAR_QPT){
	return Sign(1-Smear);
    }
    return 1;
}

double SmearingClass::ChargeFlipMS() const{  
  return ChargeFlip(smearDeltaMS);
}

double SmearingClass::ChargeFlipID() const{  
  return ChargeFlip(smearDeltaID);
}

double SmearingClass::ChargeFlipCB() const{  
  return ChargeFlip(smearDeltaCB);
}

void SmearingClass::MSUP(double &PtMS){
  double varSmearMS = 0.;
  if(useErrMatrix){
    varSmearMS = smearDeltaMS + Sign(smearDeltaMS)*vms;
  } else {
    varSmearMS =  SmearingSYSVar(DET_MS, 1.);
  }

  PtMS = GeV*pTCalc(DET_MS, defaultInit, varSmearMS);
}

void SmearingClass::MSUP(double &PtMS,
			 double &PtID,
			 double &PtCB) {
  double varSmearMS = 0.;    
  double varSmearID = 0.;
  if(useErrMatrix){
    varSmearMS = smearDeltaMS + Sign(smearDeltaMS)* vms;
    varSmearID = smearDeltaID + Sign(smearDeltaID)* vid*corr;
  } else {
    varSmearMS = SmearingSYSVar(DET_MS, 1.);
    varSmearID = smearDeltaID; //No correlation!! ++++++++TODO
  } 
  PtMS = GeV*pTCalc(DET_MS, defaultInit, varSmearMS);
  PtID = GeV*pTCalc(DET_ID, varSmearID);
  PtCB = GeV*pTCalc(DET_CB, varSmearID, varSmearMS);
}

void SmearingClass::MSLOW(double &PtMS) {
  double varSmearMS = 0.;
  if(useErrMatrix){
    varSmearMS = smearDeltaMS - Sign(smearDeltaMS)*vms;
  } else {
    varSmearMS = SmearingSYSVar(DET_MS, -1.);
  }
  PtMS = GeV*pTCalc(DET_MS, defaultInit, varSmearMS);
}

void SmearingClass::MSLOW(double &PtMS,
			  double &PtID,
			  double &PtCB) {
  double varSmearMS = 0.;    
  double varSmearID = 0.;
  if(useErrMatrix){
    varSmearMS = smearDeltaMS - Sign(smearDeltaMS)* vms;
    varSmearID = smearDeltaID - Sign(smearDeltaID)* vid*corr;
  } else {
    varSmearMS = SmearingSYSVar(DET_MS, -1.);
    varSmearID = smearDeltaID; //No correlation!! ++++++++TODO
  } 

  PtMS = GeV*pTCalc(DET_MS, defaultInit, varSmearMS);
  PtID = GeV*pTCalc(DET_ID, varSmearID);
  PtCB = GeV*pTCalc(DET_CB, varSmearID, varSmearMS);
}

void SmearingClass::IDUP(double &PtID) {
  double varSmearID = 0.;
  if(useErrMatrix){
    varSmearID = smearDeltaID + Sign(smearDeltaID)*vid;
  } else {
    varSmearID = SmearingSYSVar(DET_ID, 1.);
  }
  
  PtID = GeV*pTCalc(DET_ID, varSmearID);
}

void SmearingClass::IDUP(double &PtMS,
			 double &PtID,
			 double &PtCB) {
  double varSmearMS = 0.;    
  double varSmearID = 0.;
  if(useErrMatrix){
    varSmearMS = smearDeltaMS + Sign(smearDeltaMS)* vms*corr;
    varSmearID = smearDeltaID + Sign(smearDeltaID)* vid;
  } else {
    varSmearMS = smearDeltaMS; //No correlation!! ++++++++TODO
    varSmearID = SmearingSYSVar(DET_ID, 1.);
  } 

  PtMS = GeV*pTCalc(DET_MS, defaultInit, varSmearMS);
  PtID = GeV*pTCalc(DET_ID, varSmearID);
  PtCB = GeV*pTCalc(DET_CB, varSmearID, varSmearMS);
}

void SmearingClass::IDLOW(double &PtID) {
  double varSmearID = 0.;
  if(useErrMatrix){
    varSmearID = smearDeltaID - Sign(smearDeltaID)*vid;
  } else {
    varSmearID =  SmearingSYSVar(DET_ID, -1.);
  }
  PtID = GeV*pTCalc(DET_ID, varSmearID);
}

void SmearingClass::IDLOW(double &PtMS,
			  double &PtID,
			  double &PtCB) {
  double varSmearMS = 0.;    
  double varSmearID = 0.;
  if(useErrMatrix){
    varSmearMS = smearDeltaMS - Sign(smearDeltaMS)* vms*corr;
    varSmearID = smearDeltaID - Sign(smearDeltaID)* vid;
  } else {
    varSmearMS = smearDeltaMS; //No correlation!! ++++++++TODO
    varSmearID = SmearingSYSVar(DET_ID, -1.);
  } 

  PtMS = GeV*pTCalc(DET_MS, defaultInit, varSmearMS);
  PtID = GeV*pTCalc(DET_ID, varSmearID);
  PtCB = GeV*pTCalc(DET_CB, varSmearID, varSmearMS);
}

SYSTTYPE SmearingClass::SelectFun(string fun){
  
  if(fun=="IDUP") { Tfun=ID_UP;}
  else if(fun=="IDLOW") { Tfun=ID_LOW; }
  else if(fun=="MSUP") { Tfun=MS_UP; }
  else if(fun=="MSLOW") { Tfun=MS_LOW; }
  else if(fun=="SCALEUP") { Tfun=SCALE_UP; }
  else if(fun=="SCALELOW") { Tfun=SCALE_LOW; }
  return Tfun;
}

DATATYPE SmearingClass::SetData(string data){
  
  if(data=="Data10") { Tdata=DATA10; }
  else if(data=="Data11"){ Tdata=DATA11; }
  else if(data=="Data12"){ Tdata=DATA12; }
  return Tdata;
}

ALGOTYPE SmearingClass::SetAlgorithm(string algo){
  
  if(algo=="muid"){ Talgo=MUID; }
  else if(algo=="staco"){ Talgo=STACO; }
  else if(algo=="muons"){ Talgo=MUONS; }
  return Talgo;
}

RELTYPE SmearingClass::SetRelease(string rel){
  
  if(rel=="Rel16.6"){ Trel=REL16_6; }
  else if(rel=="Rel17"){ Trel=REL17; }
  else if(rel=="Rel17.2"){ Trel=REL17_2; }
  else if(rel=="Rel17.2Repro"){ Trel=REL17_2_REPRO; }
  else if(rel=="Rel17.2Sum13"){ Trel=REL17_2_SUM13; }
  return Trel;
}

void SmearingClass::PTVar(double &Pt,
			  string fun) {
  Tfun=SelectFun(fun);
  
  if (Tfun==ID_UP) { IDUP(Pt); }
  else if (Tfun==ID_LOW) { IDLOW(Pt); }
  else if (Tfun==MS_UP) { MSUP(Pt); }
  else if (Tfun==MS_LOW) { MSLOW(Pt); }
}

void SmearingClass::PTVar(double &PtMS,
			  double &PtID,
			  double &PtCB,
			  string fun) {  
    Tfun=SelectFun(fun);
    
    if (Tfun == ID_UP) {
	if(Tdet == DET_CB){ IDUP(PtMS,PtID,PtCB); return; }
	else if(Tdet == DET_ID){ IDUP(PtID); return; }
	
    } else if (Tfun == ID_LOW) { 
	if(Tdet == DET_CB){ IDLOW(PtMS,PtID,PtCB); return; }
	else if(Tdet == DET_ID){ IDLOW(PtID); return; }
	
    } else if (Tfun == MS_UP) { 
	if(Tdet == DET_CB){ MSUP(PtMS,PtID,PtCB); return; }
	else if(Tdet == DET_MS){ MSUP(PtMS); return; }
	
    } else if (Tfun == MS_LOW) { 
	if(Tdet == DET_CB){ MSLOW(PtMS,PtID,PtCB); return; }
	else if(Tdet == DET_MS){ MSLOW(PtMS); return; }
	
    } else if (Tfun == SCALE_UP || Tfun == SCALE_LOW) {
	
	const double sVar = (Tfun == SCALE_UP) ? 1.0 : -1.0;
	if(dbg) cout<<"sVar="<<sVar<<"; Tfun="<<Tfun<<"; Tscale="<<Tscale<<endl;
	PtID = GeV*pTCalc(DET_ID, defaultInit, defaultInit, sVar);
	PtMS = GeV*pTCalc(DET_MS, defaultInit, defaultInit, sVar);
	PtCB = GeV*pTCalc(DET_CB, defaultInit, defaultInit, sVar);
    } else {
	cerr<<__FILE__<<" "<<__LINE__<<": ERROR: not systematic variation associated to "<<fun<<endl;
	return;
    }
}

int SmearingClass::getRegionOld(const double eta) const{
  if (fabs(eta)<1.05) { return 0; }
  else if (fabs(eta)<1.7) { return 1; }
  else if (fabs(eta)<2.0) { return 2; }
  else if (fabs(eta)<2.7) { return 3; }
  else { return -1; }  
}

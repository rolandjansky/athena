/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#define _TESTBEAM_

// VARIABLES GLOBALES

bool NORMALISE=false;
bool PT=false;
bool SYMMETRY=false;

int TYPE;
enum Type{NOISE,PILEUP,VARIABLE};
TString VAR;

bool PILEUP_method1=false;
bool PILEUP_method2=true;

bool CASTOR=true;
//CASTOR=false;

  // FIT
  double m_SIGMA,m_errSIGMA,m_MEAN,m_errMEAN;
  int MINUIT_ROOT=2;
    //1 -> fit de MINUIT 
    TMinuit *gMinuit = new TMinuit(3);
    double x[100],y[100],errory[100];
    bool chi2=true;
    bool MINI=false;MINI=true;
    //2 -> fit de ROOT
    TF1 *m_f = new TF1("f","gaus");
  // BIN
  double m_max=1000000.;
  double m_min;
  int    m_Nbins=2000;
  double m_bin;

// HISTOGRAMMES
TH1F *m_H= new TH1F("H","H",m_Nbins,-m_max,m_max);
TH1F *H_pileup;

TTree *T_ID;
TTree *T_DB;
TTree *T_DATA;
int t666_id,t666_db,t666;

const int m_nCalos=4;
enum Calo{EMB,EMEC,HEC,FCAL};
long m_nCells[m_nCalos];
int m_nLayers[m_nCalos];

const int m_nDets=5;
int m_offset1[m_nDets];
int m_offset2[m_nDets];

const int m_nSamps=4;

const long m_NCELLSMAX=190000;
int NCELLS;

int NEVTSMAX[m_nDets];
int NEVTS;

TString m_store_file_name;
TString m_data_file_name[m_nCalos][4];
ofstream data_file[m_nCalos][8];
long m_ndata[m_nCalos][8];

#ifndef _TESTBEAM_
const int m_indexSymmMAX= 4000;
#else
const int m_indexSymmMAX= 2030;
#endif

float m_store_emin[m_nDets][m_indexSymmMAX];
float m_store_emax[m_nDets][m_indexSymmMAX];
TMatrix *m_store_e[m_nDets];

int m_indexSymmMax[m_nDets];
int m_indexContainer[m_nDets][m_indexSymmMAX];
int m_indexSymmContainer[m_NCELLSMAX];

#ifndef _TESTBEAM
const int m_nPtsMAX= 2600;//256*2 * 5
#else
const int m_nPtsMAX=16000;//16    * 1000
#endif

int m_nPts[m_nDets][m_indexSymmMAX];
int pos_neg=1;

float etmp[m_NCELLSMAX];

int DET,SAMP,REG,IETA,IPHI;

const int NOTDEF=-99;
const int OK=1;

typedef struct
{
  long  n[1]; 
  long  indice[m_NCELLSMAX];
  long  iphi[m_NCELLSMAX];
  long  ieta[m_NCELLSMAX];
  long  samp[m_NCELLSMAX];
  long  reg[m_NCELLSMAX];
  long  det[m_NCELLSMAX];
  float phi[m_NCELLSMAX];
  float eta[m_NCELLSMAX];
  float volume[m_NCELLSMAX];
} CELL;

typedef struct
{
  long  n[1]; 
  float elecnoise[m_NCELLSMAX];
  float pileupnoise[m_NCELLSMAX];
  float totalnoise[m_NCELLSMAX];
  float sigmaNoise_HG[m_NCELLSMAX];
  float sigmaNoise_MG[m_NCELLSMAX];
  float sigmaNoise_LG[m_NCELLSMAX];
  float RMSminbias[m_NCELLSMAX];
  float fSampl[m_NCELLSMAX];
  float ADC2MeV_HG[m_NCELLSMAX];
  float ADC2MeV_MG[m_NCELLSMAX];
  float ADC2MeV_LG[m_NCELLSMAX];
} DATABASE;

enum TYPECELL{CEL=0,RAW=1,HIT=2,nTYPECELL=3};
int TYPECELLS;
CELL ID;
DATABASE DB;
TString m_suffix[3];

////////////////////////////////////////////////////////////////////

void set(TString flag)
{
  if(flag=="NORMALISE") NORMALISE=true;
  if(flag=="PT")        PT=true;
  if(flag=="SYMMETRY")  SYMMETRY=true;
}

////////////////////////////////////////////////////////////////////

int calo(int detector)
{
  if(abs(detector)==1)      return EMB;
  else if(abs(detector)<=3) return EMEC;
  else if(abs(detector)==4) return HEC;
  else if(abs(detector)==5) return FCAL;
  std::abort();
}

////////////////////////////////////////////////////////////////////

void initialize()
{
  cout<<"- initialize ..."<<endl;
  m_suffix[CEL]="CEL";
  m_suffix[RAW]="RAW";
  m_suffix[HIT]="HIT";

  m_nLayers[EMB]=4;
  m_nLayers[EMEC]=4;
  m_nLayers[HEC]=4;
  m_nLayers[FCAL]=3;
  
  for(int iDet=0;iDet<m_nDets;++iDet)
  {
    m_offset1[iDet]=0;
    m_offset2[iDet]=0;
  }
  m_offset1[3-1]=1;
  m_offset2[5-1]=1;

  m_data_file_name[EMB][0] ="data_EMB0";
  m_data_file_name[EMB][1] ="data_EMB1";
  m_data_file_name[EMB][2] ="data_EMB2";
  m_data_file_name[EMB][3] ="data_EMB3";
  m_data_file_name[EMEC][0]="data_EMEC0";
  m_data_file_name[EMEC][1]="data_EMEC1";
  m_data_file_name[EMEC][2]="data_EMEC2";
  m_data_file_name[EMEC][3]="data_EMEC3";  
  m_data_file_name[HEC][0] ="data_HEC0";
  m_data_file_name[HEC][1] ="data_HEC1";
  m_data_file_name[HEC][2] ="data_HEC2";
  m_data_file_name[HEC][3] ="data_HEC3";
  m_data_file_name[FCAL][0]="data_FCAL1";
  m_data_file_name[FCAL][1]="data_FCAL2";
  m_data_file_name[FCAL][2]="data_FCAL3";
  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
    for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)     
      m_data_file_name[iCalo][iLayer]+=".txt";
    
  for(int icalo=0;icalo<m_nCalos;++icalo) 
    for(int nd=0;nd<m_nLayers[icalo];++nd) 
      m_ndata[icalo][nd]=0;

#ifndef _TESTBEAM_
  NEVTSMAX[0]=5;
  NEVTSMAX[1]=5;
  NEVTSMAX[2]=5;
  NEVTSMAX[3]=5;
  NEVTSMAX[4]=1000;// a changer
#else
  NEVTSMAX[0]=1000;
  NEVTSMAX[1]=1000;
  NEVTSMAX[2]=1000;
  NEVTSMAX[3]=1000;
  NEVTSMAX[4]=1000;
#endif
  

  for(int iDet=0;iDet<m_nDets;++iDet)
    m_store_e[iDet] = new TMatrix(m_indexSymmMAX,m_nPtsMAX);

  cout<<"- initialize OK"<<endl;
}

////////////////////////////////////////////////////////////////////

void noise(TString ntuple_name, 
	   TString file_out, 
	   TString type_cells)
{
  noise(ntuple_name,file_out,type_cells,5);
}

////////////////////////////////////////////////////////////////////

void noise(TString ntuple_name,  
	   TString file_out)
{
  noise(ntuple_name,file_out,"CEL",5);
}

////////////////////////////////////////////////////////////////////

void noise(TString ntuple_name,  
	   TString file_out, 
	   TString type_cells,
	   int Nevts)
{
  TYPE=NOISE;
  start(ntuple_name,file_out,type_cells,Nevts);
}

////////////////////////////////////////////////////////////////////

void pileup(TString ntuple_name,  
	    TString file_out,int z)
{
  pileup(ntuple_name,file_out,"CEL",z);
}

////////////////////////////////////////////////////////////////////

void pileup(TString ntuple_name,  
	    TString file_out, 
	    TString type_cells,int z)
{
  pos_neg=z;
  TYPE=PILEUP;
  start(ntuple_name,file_out,"CEL",1);
}

////////////////////////////////////////////////////////////////////

void var(TString ntuple_name,  
	 TString file_out,TString var)
{
  TYPE=VARIABLE;
  VAR=var;
  start(ntuple_name,file_out,"CEL",1);
}

////////////////////////////////////////////////////////////////////

void start(TString ntuple_name, 
	   TString file_out, 	    
	   TString type_cells,
	   int Nevts)
{
  if(TYPE==NOISE)    cout<<"NOISE"<<endl;
  if(TYPE==PILEUP)   cout<<"PILEUP"<<endl;
  if(TYPE==VARIABLE) cout<<"VARIABLE "<<VAR<<endl;
  if(type_cells=="CEL") TYPECELLS=CEL;
  if(type_cells=="RAW") TYPECELLS=RAW;
  if(type_cells=="HIT") TYPECELLS=HIT;
  NEVTS=Nevts;
  m_store_file_name=file_out;

  //-----------------------------------------------------
  // INITIALIZE
  initialize(); 

  //-----------------------------------------------------
  // ACCES NTUPLES
  bool G3=true;
  if(TYPE==PILEUP && CASTOR) 
    if(G3)    
      ntuple_name="rfio:/castor/cern.ch/user/l/lechowsk/Athena/batch750/his/ntuple_bruit_minbias750OK_simul_"+ntuple_name+".root";   
  /*
  ntuple_name="rfio:/castor/cern.ch/user/l/lechowsk/Athena/batch800/his/ntuple_bruit_800_pileup_lumi10_"+ntuple_name+".root";
  */
    else
      ntuple_name="rfio:/castor/cern.ch/user/l/lechowsk/Athena/batch820/his/ntuple_"+ntuple_name+".root";
 
  TFile *F;
  if(TYPE==PILEUP && CASTOR) F=TFile::Open(ntuple_name); 
  else                       F=new TFile(ntuple_name);  

  F->cd("CALO");

  t666_id=0;
  t666_db=0;
  t666   =0;
  TKey *key;
  TIter next(gDirectory->GetListOfKeys());
  while ((key = (TKey *) next())) 
  {   
    TString key_name= key->GetName();
         if(key_name.CompareTo("t666_id")==0) t666_id=OK;
    else if(key_name.CompareTo("t666_db")==0) t666_db=OK;
    else if(key_name.CompareTo("t666")==0)    t666   =OK;
    cout<<key_name<<" "<<t666_id<<" "<<t666_db<<" "<<t666<<endl;
  }
  cout<<"coucou1"<<endl;
  if(t666_id!=OK) return;
  cout<<"coucou2"<<endl;
  if(t666_id==OK) T_ID  =(TTree*) gDirectory->Get("t666_id");
  if(t666_db==OK) T_DB  =(TTree*) gDirectory->Get("t666_db");
  if(t666==OK)    T_DATA=(TTree*) gDirectory->Get("t666");

  //====
  if(TYPE==PILEUP)
  {
    F->cd("CBNT");
    TTree *Ttemp=(TTree*) gDirectory->Get("t3333");
    Ttemp->SetBranchStatus("*",0); //disable all branches
    Ttemp->SetBranchStatus("EtaGen",1);
    TTree *T3333 = Ttemp->CloneTree();
    if(t666==OK) T_DATA->AddFriend(T3333,"t3333");
  }
  //====

  cout<<"NTUPLE "<<ntuple_name<<" pour "<<type_cells<<"  - OK"<<endl;

  //-----------------------------------------------------
  // VARIABLES NTUPLE 
  if(t666_id==OK) 
  {
    T_ID->SetBranchAddress("n",&ID.n);
    T_ID->SetBranchAddress("iphi",ID.iphi);
    T_ID->SetBranchAddress("ieta",ID.ieta);
    T_ID->SetBranchAddress("samp",ID.samp);
    T_ID->SetBranchAddress("reg",ID.reg);
    T_ID->SetBranchAddress("det",ID.det);
    T_ID->SetBranchAddress("phi",ID.phi);
    T_ID->SetBranchAddress("eta",ID.eta);
    T_ID->SetBranchAddress("volume",ID.volume);

    T_ID->GetEntry(0);
    NCELLS=ID.n[0];  
    cout<<NCELLS<<endl;
  }
  if(t666_db==OK) 
  {
    T_DB->SetBranchAddress("elecnoise",DB.elecnoise);
    T_DB->SetBranchAddress("pileupnoise",DB.pileupnoise);
    T_DB->SetBranchAddress("totalnoise",DB.totalnoise);
    T_DB->SetBranchAddress("sigmaNoise_HG",DB.sigmaNoise_HG);
    T_DB->SetBranchAddress("sigmaNoise_MG",DB.sigmaNoise_MG);
    T_DB->SetBranchAddress("sigmaNoise_LG",DB.sigmaNoise_LG);
    T_DB->SetBranchAddress("RMSminbias",DB.RMSminbias);
    T_DB->SetBranchAddress("fSampl",DB.fSampl);
    T_DB->SetBranchAddress("ADC2MeV_HG",DB.ADC2MeV_HG);
    T_DB->SetBranchAddress("ADC2MeV_MG",DB.ADC2MeV_MG);
    T_DB->SetBranchAddress("ADC2MeV_LG",DB.ADC2MeV_LG);

    T_DB->GetEntry(0);
  }
  if(t666==OK) 
#ifndef _TESTBEAM_
    T_DATA->SetBranchAddress("e"+m_suffix[TYPECELLS],etmp);
#else
    T_DATA->SetBranchAddress("ADC0",etmp);
    //T_DATA->SetBranchAddress("ADC2",etmp);
#endif

  //-----------------------------------------------------
  for(int i=0;i<m_nCalos;++i) m_nCells[i]=0;  
  for(long nn=0;nn<NCELLS;++nn) ++m_nCells[calo(ID.det[nn])];  
  cout<<" EM:"  <<m_nCells[EMB]+m_nCells[EMEC]
      <<" HEC:" <<m_nCells[HEC]
      <<" FCAL:"<<m_nCells[FCAL]<<endl;

  //-----------------------------------------------------
  // FICHIERS 
  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
    for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)    
      (data_file[iCalo][iLayer]).open(m_data_file_name[iCalo][iLayer]);
    
  //-----------------------------------------------------
  // SYMETRIE
  symmetrize();   

  //-----------------------------------------------------
  // PROCESS PRINCIPAL 
  if(TYPE==NOISE)    extract_noise();
  if(TYPE==PILEUP)   extract_pileup();
  if(TYPE==VARIABLE) extract_variable(VAR);

  //-----------------------------------------------------
  // ECRITURE FICHIER	    	  
  //////////////////////// 
  cout<<"- ecriture fichier principal"<<endl;
  store();
  ////////////////////////   

 //-----------------------------------------------------
  if(TYPE==PILEUP && CASTOR) F->Close();

  cout<<"-FIN-"<<endl;
}

////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////

void symmetrize()
{
  cout<<"- symmetrisation ..."<<endl;

  for(int iDet=0;iDet<m_nDets;++iDet)
    m_indexSymmMax[iDet]=0;

  for(int index=0;index<NCELLS;++index)
  {     
    int iDet=ID.det[index];

    // z symmetry
    if(iDet<=0) continue;
    // phi symmetry    
#ifndef _TESTBEAM_  
    if(calo(iDet)!=FCAL && ID.iphi[index]!=0) continue;     
#endif         
      
    iDet=fabs(iDet)-1;
        
    //conversion iDet/indexSymm->index
    m_indexContainer[iDet][m_indexSymmMax[iDet]]=index;
    //conversion index -> indexSymm (iDet determine par ID.det[index])
    m_indexSymmContainer[index]=m_indexSymmMax[iDet];

    ++m_indexSymmMax[iDet];
  }//loop 

  cout<<"- symmetrisation OK"<<endl;
}

////////////////////////////////////////////////////////////////////

void store_energy()
{
  cout<<"- store energy"<<endl;
  float e;
  int iDet,iPt,indexSymm;
#ifndef _TESTBEAM_ 
  float Eevt=200.; 
#else
  float Eevt=2000.; 
#endif

  for(iDet=0;iDet<m_nDets;++iDet)
    for(indexSymm=0;indexSymm<m_indexSymmMax[iDet];++indexSymm)
    {
      m_nPts[iDet][indexSymm]=0;
      m_store_emin[iDet][indexSymm]= 1000000;
      m_store_emax[iDet][indexSymm]=-1000000;
    }

  cout<<NEVTS<<endl;
  for(int evt=0;evt<NEVTS;++evt)
  {
    T_DATA->GetEntry(evt);
    //cout<<" "<<evt<<endl;
    for(int index=0;index<NCELLS;++index) 
    {
      iDet=fabs(ID.det[index])-1;
      if(evt>=NEVTSMAX[iDet]) continue;
      indexSymm=m_indexSymmContainer[index];  
      e=etmp[index];
      if(NORMALISE) e/DB.elecnoise[index];
      iPt=m_nPts[iDet][indexSymm];
      (*m_store_e[iDet])[indexSymm][iPt]=e;
      if(e<m_store_emin[iDet][indexSymm])           
	m_store_emin[iDet][indexSymm]=e;
      if(e>m_store_emax[iDet][indexSymm] && e<Eevt) 
	m_store_emax[iDet][indexSymm]=e;

      ++m_nPts[iDet][indexSymm];
    }
  }
  
  cout<<"- store energy OK"<<endl; 
}

////////////////////////////////////////////////////////////////////

void extract_noise()
{
  store_energy();
  cout<<"- extract noise ..."<<endl; 

  double Emin,Emax,Width,Ecenter;
  int index;
  int nPts;

  for(int iDet=0;iDet<m_nDets;++iDet)
    for(int indexSymm=0;indexSymm<m_indexSymmMax[iDet];++indexSymm)
    {           
      index=m_indexContainer[iDet][indexSymm];

      //if(fmod(indexSymm,100)==0) 
	cout<<"   "<<iDet<<" "<<indexSymm<<" "<<index<<endl;

      // HISTOGRAMME 
      Emin= m_store_emin[iDet][indexSymm]; 
      Emax= m_store_emax[iDet][indexSymm]; 
      Width=Emax-Emin;
      Ecenter=0.; 
      Ecenter=Emin+Width/2;//?
      m_max=Ecenter+(Width/2)*3/2;
      m_min=Ecenter-(Width/2)*3/2;
      m_Nbins=15;
      m_bin=(m_max-m_min)/m_Nbins;
      m_H->SetBins(m_Nbins,m_min,m_max);
      //SetBins ne marche que AVANT Fill !!
      nPts=m_nPts[iDet][indexSymm];// nombre de points (entrees de H)
      cout<<"******"<<nPts<<" "<<Emin<<" "<<Emax<<" "<<Ecenter<<" "<<m_min<<" "<<m_max<<endl;
      for(int iPt=0;iPt<nPts;++iPt) 
      {
	m_H->Fill((*m_store_e[iDet])[indexSymm][iPt]);
	if(indexSymm<5) cout<<(*m_store_e[iDet])[indexSymm][iPt]<<" ";
      }

      // FIT
      if(nPts>50)      
	if(MINUIT_ROOT==2) fitROOT(); 
	else               fitMINUIT(m_H,m_bin,m_Nbins,m_max); 
      m_H->Reset();

      if(nPts>50)
	put_data_in_file(index);     
    }
  cout<<"- extract noise OK"<<endl; 
}

////////////////////////////////////////////////////////////////////

void extract_pileup()
{
  // on tourne deja sur des cellules types (symetrie)

  TProfile *profile=new TProfile("profile","profile",NCELLS+1,0,NCELLS);
  profile->SetErrorOption("s");//INDISPENSABLE !!!!!!!!

  TProfile *profile_pos=new TProfile("profile_pos","profile_pos",
				     NCELLS+1,0,NCELLS);
  profile_pos->SetErrorOption("s");

  TProfile *profile_neg=new TProfile("profile_neg","profile_neg",
				     NCELLS+1,0,NCELLS);
  profile_neg->SetErrorOption("s");

  //////////////////////////////
  T_DATA->AddFriend(T_ID);
  T_DATA->AddFriend(T_DB);

  //////////////////////////////

  if(PILEUP_method2)  
  {
    TString what_to_draw, what_to_draw1, what_to_draw2;  
                //what_to_draw =Form("e[%d]",index);
                  what_to_draw ="eCEL";
    if(NORMALISE) what_to_draw+="/pileupnoise";
    if(PT)        what_to_draw+="/cosh(eta)";
    what_to_draw+=":i>>profile";
    what_to_draw1=what_to_draw; what_to_draw2=what_to_draw;
    what_to_draw1+="_pos";      what_to_draw2+="_neg";

    TString cut_to_use, cut_to_use1, cut_to_use2;
    if(pos_neg<0) 
    {
      cut_to_use ="det<0";
      cut_to_use1=cut_to_use; cut_to_use2=cut_to_use;
    }
    if(pos_neg>0) 
    {
      cut_to_use ="det>0";
      cut_to_use1=cut_to_use; cut_to_use2=cut_to_use;
    }    
    if(NORMALISE) 
    {
      cut_to_use+=" && pileupnoise!=0 && abs(eta-t3333.EtaGen[0])>0.5";
      cut_to_use1=cut_to_use; cut_to_use2=cut_to_use;
    }

    if(pos_neg==0)
    {
      T_DATA->Draw(what_to_draw1,cut_to_use1,"goff");
      T_DATA->Draw(what_to_draw2,cut_to_use2,"goff");
    }
    else T_DATA->Draw(what_to_draw,cut_to_use,"goff");
  } 

  //////////////////////    
  for(int index=0;index<NCELLS;++index)
    //ici, on boucle sur des cellules deja symetrisees
  {    
    // METHODE 1
    if(PILEUP_method1)
    {
      what_to_draw=Form("eCEL[%d]>>h",index);
      T_DATA->Draw(what_to_draw,"","goff");
      
      H_pileup=(TH1F*)gDirectory->Get("h");
      m_SIGMA=H_pileup->GetRMS();
      m_errSIGMA=0.;
      m_MEAN=H_pileup->GetMean();	  
      m_errMEAN=0.;//m_errMEAN=m_SIGMA/sqrt(N);
    }

    // METHODE 2
    else if(PILEUP_method2)
    {
      long i_bin=index+1;
      if(pos_neg!=0)
      {
	m_SIGMA=profile->GetBinError(i_bin);
	m_errSIGMA=0.;
	m_MEAN=profile->GetBinContent(i_bin);	  
	m_errMEAN=0.;//m_errMEAN=m_SIGMA/sqrt(N);
      }
      else
      {
	float RMS1 =profile_pos->GetBinError(i_bin);
	float RMS2 =profile_neg->GetBinError(i_bin);
	float mean1=profile_pos->GetBinContent(i_bin);
	float mean2=profile_neg->GetBinContent(i_bin);
	if(RMS1!=RMS2)
	  m_SIGMA=(mean1-mean2)/sqrt(RMS1*RMS1+RMS2*RMS2);
	else
	  m_SIGMA=0;
	m_errSIGMA=0.;
	m_MEAN=0;	  
	m_errMEAN=0.;//m_errMEAN=m_SIGMA/sqrt(N);
      }
      if(G3) m_SIGMA*=1000.;
      if(G3) m_MEAN*=1000.;
    }

    put_data_in_file(index);
  }//loop

}

////////////////////////////////////////////////////////////////////

void extract_variable(TString variable)
{
  if(t666_db!=OK) return;

  int index;
  for(int iDet=0;iDet<m_nDets;++iDet)    
    for(int indexSymm=0;indexSymm<m_indexSymmMax[iDet];++indexSymm)
    {
      index=m_indexContainer[iDet][indexSymm];
      switch(variable) 
      {	
      case "elecnoise"    : m_SIGMA=DB.elecnoise[index];     break;
      case "pileupnoise"  : m_SIGMA=DB.pileupnoise[index];   break;
      case "totalnoise"   : m_SIGMA=DB.totalnoise[index];    break;
      case "sigmanoise_HG": m_SIGMA=DB.sigmanoise_HG[index]; break;
      case "sigmanoise_MG": m_SIGMA=DB.sigmanoise_MG[index]; break;
      case "sigmanoise_LG": m_SIGMA=DB.sigmanoise_LG[index]; break;
      case "RMSminbias"   : m_SIGMA=DB.RMSminbias[index];    break;
      case "fSampl"       : m_SIGMA=DB.fSampl[index];        break;
      case "ADC2MeV_HG"   : m_SIGMA=DB.ADC2MeV_HG[index];    break;
      case "ADC2MeV_MG"   : m_SIGMA=DB.ADC2MeV_MG[index];    break;
      case "ADC2MeV_LG"   : m_SIGMA=DB.ADC2MeV_LG[index];    break;
      }
      m_errSIGMA=0.;
      m_MEAN=0;	  
      m_errMEAN=0.;
      put_data_in_file(index);
    }
}

////////////////////////////////////////////////////////////////////

void put_data_in_file(int index)
{  
  int iDet =fabs(ID.det[index]);
  int iSamp=ID.samp[index];
 
  data_file[calo(iDet)][iSamp-m_offset2[iDet-1]]
    <<iDet<<"."
    <<iSamp+m_offset1[iDet-1]<<"."
    <<ID.reg[index]<<"."
    <<ID.ieta[index]<<"."
    <<ID.iphi[index]<<"\t"
    <<ID.eta[index]<<"\t"
    <<ID.phi[index]<<"\t"
    <<ID.volume[index]<<"\t"
    <<m_SIGMA<<"\t"
    <<m_errSIGMA<<"\t"
    <<m_MEAN<<"\t"
    <<m_errMEAN
    <<endl;		

  ++m_ndata[calo(iDet)][iSamp-m_offset2[iDet-1]];  	           
}

////////////////////////////////////////////////////////////////////

void store()
{
  double tmp1,tmp2,tmp3,tmp4,tmp5;  
  double ETA,SIGMA,MEAN,errSIGMA,errMEAN;

  ofstream store_file(m_store_file_name);       
  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
    store_file<<iCalo<<"\t"<<m_nCells[iCalo]<<endl; 
  store_file<<endl;

  char aLine[1024];
  std::string sLine;
  char dot;

  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
    if(m_nCells[iCalo]>0)    
      for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)
      {
	ifstream data_tmp(m_data_file_name[iCalo][iLayer]);
	do
        {       
	  data_tmp.getline(aLine,sizeof(aLine)-1);
	  store_file<<aLine<<endl;	    
	} 
	while(!data_tmp.eof());
	data_tmp.close(); 
	//remove(data_tmp);
      }
  store_file.close();
}

////////////////////////////////////////////////////////////////////

void fitROOT()
{	    
    //pas besoin d'initialiser les parametres avec une fonction predefinie !
    m_H->Fit("f","QI0");   
      //"Q" : Quiet mode (minimum printing)
      //"I" : Use integral of function in bin instead of value 
      //      at bin center (plus lent!)
      //"0" : Do not plot the result of the fit.
    m_SIGMA=m_f->GetParameter(2);
    m_errSIGMA=m_f->GetParError(2);
    //
    m_MEAN=m_f->GetParameter(1);
    m_errMEAN=m_f->GetParError(1);
    //
    cout<<"SIGMA="<<m_SIGMA<<endl;
}

////////////////////////////////////////////////////////////////////

void fitMINUIT()
{	
  //remplissage de x[],y[],errory[]
  double depart=-m_max;
  for (int ibin=0;ibin<m_Nbins;++ibin)
  {
    x[ibin]=depart+m_bin/2.; //valeur a la moitie du bin ibin
    y[ibin]=m_H->GetBinContent(ibin+1);
    errory[ibin]=sqrt(y[ibin]);
    depart=depart+m_bin;
  }	

  gMinuit->SetFCN(fcn);

  double arglist[10];
  int ierflg = 0;
  arglist[0] = -1;//no output
  gMinuit->mnexcm("SET PRI", arglist ,1,ierflg);
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
  arglist[0] = pow(10,-14);
  gMinuit->mnexcm("SET EPS", arglist ,1,ierflg);
  //Set starting values and step sizes for parameters
  double vstart[3] = {0.01,500,0};
  double step[3] = {0.01, 1, 0.01};
  gMinuit->mnparm(0, "sigma", vstart[0], step[0], 0,0,ierflg);
  gMinuit->mnparm(1, "A"    , vstart[1], step[1], 0,0,ierflg);
  gMinuit->mnparm(2, "M"    , vstart[2], step[2], 0,0,ierflg);  
  //Now ready for minimization step
  if(MINI)
  {
    arglist[0] = 100000;
    arglist[1] = 1.;
    gMinuit->mnexcm("MINI", arglist ,2,ierflg);
  }
  else
  {
    arglist[0] = 500;
    arglist[1] = 1.;
    gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
  } 
  //Print results
    /*
      double amin,edm,errdef;
      int nvpar,nparx,icstat;
      gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
      gMinuit->mnprin(3,amin);
    */
  TString chnam;
  double val, err, xlolim, xuplim;
  int iuint;
  //    
  gMinuit->mnpout(0,chnam,val,err,xlolim,xuplim,iuint);
  m_SIGMA=val;
  m_errSIGMA=err;
  //
  gMinuit->mnpout(2,chnam,val,err,xlolim,xuplim,iuint);    
  m_MEAN=val;
  m_errMEAN=err;
  //
}

////////////////////////////////////////////////////////////////////

void fcn(int &npar, double *gin, double &f, double *par, int iflag)
{
  if(chi2)
  {
    double chisq = 0;
    double delta;
    for (int i=0;i<m_Nbins; ++i) 
    {
      if(y[i]>pow(10,-6))
	delta  = (y[i]-Gaussienne(x[i],par))/errory[i];
      else delta=0;
      chisq += delta*delta;
    }
    f = chisq;
  }
}

////////////////////////////////////////////////////////////////////

double Gaussienne(double X, double *parametre)
  //=Gaussienne pour le Fit (fonction test)
{
  double sigma=parametre[0];
  double A=parametre[1];
  double M=parametre[2];
  double pi=2*asin(1);
  double FAC=1/sqrt(2*pi)*m_bin;
  double G=A*(FAC/sigma)*exp(-0.5*pow(((X-M)/sigma),2) );
  //double G=A * exp(-0.5*pow(((X-M)/sigma),2) );
  return G;
}

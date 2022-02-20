/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

bool PSEUDO=false;//PSEUDO=true;
//------------------
bool DIG=false;
bool DIG12=false;
bool RAW=false;
bool CEL=false;

const int m_nCalos=4;
enum Calo{EMB,EMEC,HEC,FCAL};
int m_nCells[m_nCalos];
int m_nLayers[m_nCalos];
const int m_nLayers_max=4;
double m_fac[m_nCalos][m_nLayers_max];

TString m_data_file_name[m_nCalos][m_nLayers_max];
int     m_MarkerStyle[m_nLayers_max];
int     m_MarkerSize[m_nLayers_max];
int     m_MarkerColor[m_nLayers_max];
float   m_maximum[m_nCalos];
int     m_offset[m_nCalos][m_nLayers_max];
TString m_legend[m_nCalos][m_nLayers_max];
TString m_legend_calo[m_nCalos];
TString m_name_canvas1D[m_nCalos];
TString m_name_canvas2D[m_nCalos][m_nLayers_max];
TString m_ps1D[m_nCalos];
TString m_ps2D[m_nCalos][m_nLayers_max];
//TString m_psFCAL[3];

const int NCELLSMAX=4000;
const int NBINSMAX=1000;
//int store_n[NCELLSMAX][NCELLSMAX];  
//float store_deta[NCELLSMAX],store_dphi[NCELLSMAX];
int index[NCELLSMAX];
int DET[NCELLSMAX],SAMP[NCELLSMAX],REG[NCELLSMAX];
int IETA[NCELLSMAX],IPHI[NCELLSMAX];
float VOLUME[NCELLSMAX];
float ETA[NCELLSMAX],PHI[NCELLSMAX],SIGMA[NCELLSMAX],MEAN[NCELLSMAX]; 
float errETA[NCELLSMAX],errSIGMA[NCELLSMAX],errMEAN[NCELLSMAX];
float binsX[NBINSMAX],binsY[NBINSMAX];
float DETA[NBINSMAX],DPHI[NBINSMAX],SIGMA_etendu[NBINSMAX][NBINSMAX];
float m_seuil[m_nCalos][m_nLayers_max];

const int m_nReg_max=5;
TH1F *Hnoise[m_nLayers_max][m_nReg_max];

enum axe{_ETA_,_PHI_};

TString m_suffixe;
bool SUFFIX=false;

int NDIMENSIONS[m_nCalos];
int NDIM;
#include <string>

///////////////////////////////////////////////////////////////////

int calo(int detector)
{
  if(abs(detector)==1)      return EMB;
  else if(abs(detector)<=3) return EMEC;
  else if(abs(detector)==4) return HEC;
  else if(abs(detector)==5) return FCAL;
  //else if(abs(detector)==6) return TILE;
  std::abort();
}

void initialize()
{
  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
    NDIMENSIONS[iCalo]=NDIM;
  NDIMENSIONS[FCAL]=2;

  m_nLayers[EMB]=4;
  m_nLayers[EMEC]=4;
  m_nLayers[HEC]=4;
  m_nLayers[FCAL]=3;

  m_data_file_name[EMB][0]="data_EMB0.txt";
  m_data_file_name[EMB][1]="data_EMB1.txt";
  m_data_file_name[EMB][2]="data_EMB2.txt";
  m_data_file_name[EMB][3]="data_EMB3.txt";
  m_data_file_name[EMEC][0]="data_EMEC0.txt";
  m_data_file_name[EMEC][1]="data_EMEC1.txt";
  m_data_file_name[EMEC][2]="data_EMEC2.txt";
  m_data_file_name[EMEC][3]="data_EMEC3.txt";  
  m_data_file_name[HEC][0]="data_HEC0.txt";
  m_data_file_name[HEC][1]="data_HEC1.txt";
  m_data_file_name[HEC][2]="data_HEC2.txt";
  m_data_file_name[HEC][3]="data_HEC3.txt";
  m_data_file_name[FCAL][0]="data_FCAL1.txt";
  m_data_file_name[FCAL][1]="data_FCAL2.txt";
  m_data_file_name[FCAL][2]="data_FCAL3.txt";

  m_MarkerStyle[0]=20; 
  m_MarkerStyle[1]=6;
  m_MarkerStyle[2]=21;
  m_MarkerStyle[3]=22;
  
  m_MarkerSize[0]=1; 
  m_MarkerSize[1]=3;
  m_MarkerSize[2]=1;
  m_MarkerSize[3]=1;

  m_MarkerColor[0]=1; 
  m_MarkerColor[1]=2;
  m_MarkerColor[2]=3;
  m_MarkerColor[3]=4;

  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
    for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)
      m_offset[iCalo][iLayer]=0;
  for(int iLayer=0;iLayer<m_nLayers[FCAL];++iLayer)
    m_offset[FCAL][iLayer]=1;

  m_legend[EMB][0]="S0 (PS)";
  m_legend[EMB][1]="S1 (Front)";
  m_legend[EMB][2]="S2 (Middle)";
  m_legend[EMB][3]="S3 (Back)";
  m_legend[EMEC][0]="S0 (PS)";
  m_legend[EMEC][1]="S1 (Front)";
  m_legend[EMEC][2]="S2 (Middle)";
  m_legend[EMEC][3]="S3 (Back)";
  m_legend[HEC][0]="S0";
  m_legend[HEC][1]="S1";
  m_legend[HEC][2]="S2";
  m_legend[HEC][3]="S3";
  m_legend[FCAL][0]="S1 ";
  m_legend[FCAL][1]="S2 ";
  m_legend[FCAL][2]="S3 ";
  m_legend_calo[EMB] ="EMB";
  m_legend_calo[EMEC]="EMEC";
  m_legend_calo[HEC] ="HEC";
  m_legend_calo[FCAL]="FCAL";    
  m_name_canvas1D[EMB] ="EMB";
  m_name_canvas1D[EMEC]="EMEC";
  m_name_canvas1D[HEC] ="HEC";
  m_name_canvas1D[FCAL]="FCAL";

  m_name_canvas2D[EMB][0] ="EMB0";
  m_name_canvas2D[EMB][1] ="EMB1";
  m_name_canvas2D[EMB][2] ="EMB2";
  m_name_canvas2D[EMB][3] ="EMB3";
  m_name_canvas2D[EMEC][0]="EMEC0";
  m_name_canvas2D[EMEC][1]="EMEC1";
  m_name_canvas2D[EMEC][2]="EMEC2";
  m_name_canvas2D[EMEC][3]="EMEC3";
  m_name_canvas2D[HEC][0] ="HEC0";
  m_name_canvas2D[HEC][1] ="HEC1"; 
  m_name_canvas2D[HEC][2] ="HEC2";
  m_name_canvas2D[HEC][3] ="HEC3";
  m_name_canvas2D[FCAL][0]="FCAL1";
  m_name_canvas2D[FCAL][1]="FCAL2";
  m_name_canvas2D[FCAL][2]="FCAL3";

  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
  {
    if(NDIMENSIONS[iCalo]==1)
    {
      m_ps1D[iCalo]="noise";
      m_ps1D[iCalo]+=m_name_canvas1D[iCalo]; 
      m_ps1D[iCalo]+="_";
      if(PSEUDO) m_ps1D[iCalo]+="PS";
      if(DIG)    m_ps1D[iCalo]+="DIG";
      if(RAW)    m_ps1D[iCalo]+="RAW";
      if(CEL)    m_ps1D[iCalo]+="CEL";
      if(SUFFIX) 
      {
	m_ps1D[iCalo]+="_";
	m_ps1D[iCalo]+=m_suffixe;
      }
      m_ps1D[iCalo]+=".ps";
    }
    else if(NDIMENSIONS[iCalo]==2)
    {
      //cout<<m_nLayers[iCalo]<<endl;
      for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)
      {
	m_ps2D[iCalo][iLayer]="noise";
	m_ps2D[iCalo][iLayer]+=m_name_canvas2D[iCalo][iLayer]; 
	m_ps2D[iCalo][iLayer]+="_";
	if(PSEUDO) m_ps2D[iCalo][iLayer]+="PS";
	if(DIG)    m_ps2D[iCalo][iLayer]+="DIG";
	if(RAW)    m_ps2D[iCalo][iLayer]+="RAW";
	if(CEL)    m_ps2D[iCalo][iLayer]+="CEL";
	if(SUFFIX) 
        {
	  m_ps2D[iCalo][iLayer]+="_";
	  m_ps2D[iCalo][iLayer]+=m_suffixe;
	}
	m_ps2D[iCalo][iLayer]+=".ps";
      }
    }
  }
  
  facteurs();

  /*
// RMS E minbias
  m_maximum[EMB]=2.5;
  m_maximum[EMEC]=27;
  m_maximum[HEC]=13;
  m_maximum[FCAL]=30;
  */
// mean E minbias
  /*
  m_maximum[EMB]=0.12;
  m_maximum[EMEC]=5;
  m_maximum[HEC]=2.3;
  m_maximum[FCAL]=8;
  */
// RMS/sqrt(mean) E minbias
  m_maximum[EMB]=100;
  m_maximum[EMEC]=70;
  m_maximum[HEC]=150;
  m_maximum[FCAL]=40;

  for(int iSamp=0;iSamp<m_nLayers_max;++iSamp)    
  {
    for(int iReg=0;iReg<m_nReg_max;++iReg)    
    {
      TString name=Form("Hnoise%d%d",iSamp,iReg);
      Hnoise[iSamp][iReg]=new TH1F(name,name,100,0,20);
    }
  }

  m_seuil[EMB][0]=12;
  m_seuil[EMB][1]=12;
  m_seuil[EMB][2]=12;
  m_seuil[EMB][3]=12;
}

///////////////////////////////////////////////////////////////////

void retrieve_files(TString store_file_name)
{
  cout<<"- retrieve files ..."<<endl;
  double tmp1,tmp2;  
  double ETA,SIGMA,MEAN,errSIGMA,errMEAN;

  char aLine[1024];
  std::string sLine;
  char dot;
  int DET,SAMP;

  ifstream store_file(store_file_name);

  for(int iCalo=0;iCalo<m_nCalos;++iCalo)
    store_file>>tmp1>>m_nCells[iCalo];

  ofstream data_tmp[m_nCalos][m_nLayers_max];
  for(int iCalo=0;iCalo<m_nCalos;++iCalo)  
    if(m_nCells[iCalo]>0)   
      for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)
	(data_tmp[iCalo][iLayer]).open(m_data_file_name[iCalo][iLayer]);

  do
  {       
    store_file.getline(aLine,sizeof(aLine)-1);
    sLine = std::string(aLine);
    std::istringstream es( sLine.c_str() );
    es>>DET>>dot>>SAMP;  
    //cout<<DET<<" "<<SAMP<<" "<<sLine<<endl;
    data_tmp[calo(DET)][SAMP]<<aLine<<endl;
  } 
  while(!store_file.eof());
  store_file.close(); 

  for(int iCalo=0;iCalo<m_nCalos;++iCalo)  
    if(m_nCells[iCalo]>0)   
      for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)
	(data_tmp[iCalo][iLayer]).close();

  cout<<"- retrieve files OK"<<endl;
}

///////////////////////////////////////////////////////////////////

void plot(TString store_file_name, int CELLULES, TString suffixe, int ND)
{
  SUFFIX=true;
  m_suffixe=suffixe;
  plot(store_file_name,CELLULES,ND);
}

///////////////////////////////////////////////////////////////////

void plot(TString store_file_name, int CELLULES, int ND)
{
  if(CELLULES==1)   DIG=true;
  if(CELLULES==112) DIG12=true;
  if(CELLULES==2)   RAW=true;
  if(CELLULES==3)   CEL=true;
  NDIM=ND;
  initialize();
  retrieve_files(store_file_name);    
  plot();  
}

///////////////////////////////////////////////////////////////////

void plot()
{
  bool ERRORS=false;
  bool MEANS=false;
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(0);
 //---------------------------------------
 
  TGraphErrors* graph[m_nCalos][m_nLayers_max];
  TMultiGraph *multi_graph[m_nCalos];  
  TH2F* graph2D[m_nCalos][m_nLayers_max];
  TCanvas *canvas1D[m_nCalos];
  TCanvas *canvas2D[m_nCalos][m_nLayers_max];
  TLegend *leg1D[m_nCalos];

  char aLine[1024];
  std::string sLine;
  char dot;

  int n,nx,ny;

  float seuil;

  for(int k=0;k<NCELLSMAX;++k) errETA[k]=0;
  ofstream debug("debug.txt");

  for(int iCalo=0;iCalo<m_nCalos;++iCalo)    
  {	
    if(m_nCells[iCalo]<=0) continue;

    if(NDIMENSIONS[iCalo]==1)
    {
      canvas1D[iCalo] = new TCanvas(m_name_canvas1D[iCalo],
				    m_name_canvas1D[iCalo],1);
      (canvas1D[iCalo])->SetBorderMode(0);
      (canvas1D[iCalo])->SetFillColor(10);
      multi_graph[iCalo] = new TMultiGraph();
      //leg1D[iCalo] = new TLegend(0.78,0.78,0.98,0.98);
      leg1D[iCalo] = new TLegend(0.82,0.87,1.,1.);
    }      

    for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer)
    {      
      if(iLayer>=2) continue;
      //if(iLayer<2) continue;
      cout<<"- plot calo "<<iCalo<<" layer "<<iLayer<<endl;
      debug<<"- plot calo "<<iCalo<<" layer "<<iLayer<<endl;

      //lecture donnees, stockage et numeration
      ifstream data;
      data.open(m_data_file_name[iCalo][iLayer]);
      if(data.bad()) cout<<m_data_file_name[iCalo][iLayer]<<" BAD !!"<<endl;

      //n=store_data(data);
      cout<<"store_data "<<endl;
      n=0;
      do 
      {	  	  
	char id[1024];
	if(data>>id)
        {
	  data>>ETA[n]>>PHI[n];
	  data>>VOLUME[n]>>SIGMA[n]>>errSIGMA[n]>>MEAN[n]>>errMEAN[n];
	  //cout<<n<<" "<<ETA[n]<<" "<<PHI[n]<<" "<<VOLUME[n]<<" "<<SIGMA[n]<<" "<<errSIGMA[n]<<" "<<MEAN[n]<<" "<<errMEAN[n]<<endl; 
	  if(!ERRORS) {errSIGMA[n]=0; errMEAN[n]=0;}  	  
	  read_id(std::string(id),n);	    	  
	  ++n;
	}	   
      }
      while(!data.eof());
      cout<<" -> OK"<<endl;

      data.close();     

      if(NDIMENSIONS[iCalo]==1)
      {
	//seuil
	for(int i=0;i<n;++i) SIGMA[i]=SIGMA[i]*m_fac[iCalo][iLayer];
	seuil=0.006;	
	//for(long i=0;i<n;++i) if(SIGMA[i]>seuil) SIGMA[i]=seuil; 

	if(!MEANS) 
	  graph[iCalo][iLayer]=new TGraphErrors(n,ETA,SIGMA,errETA,errSIGMA);
	else
	  graph[iCalo][iLayer]=new TGraphErrors(n,ETA,MEAN,errETA,errMEAN);
	graph[iCalo][iLayer]
	  ->SetMarkerStyle(m_MarkerStyle[iLayer+m_offset[iCalo][iLayer]]);
	graph[iCalo][iLayer]
	  ->SetMarkerSize(m_MarkerSize[iLayer+m_offset[iCalo][iLayer]]);
	graph[iCalo][iLayer]
	  ->SetMarkerColor(m_MarkerColor[iLayer+m_offset[iCalo][iLayer]]);
	multi_graph[iCalo]->Add(graph[iCalo][iLayer],"p");
	leg1D[iCalo]->AddEntry(graph[iCalo][iLayer],
			       m_legend[iCalo][iLayer],"p");
      }
      else if(NDIMENSIONS[iCalo]==2)
      {
	//ordonne les tableaux en eta, puis en phi pour chaque eta
	// et retourne le plus petit delta-phi
	float dphi_min=order_tabs(n);
	
	//remplit les SIGMA_etendu
	seuil=15;seuil=4;
	int nx=0;
	int ny=0;      
	for(int j=0;j<n;++j)
        {
	  int i=index[j];
	  float deta,dphi;
	  largeur_eta_phi(DET[i],SAMP[i],REG[i],deta,dphi);

	  binsX[nx]=ETA[i]-deta/2.;
	  //cout<<"=== "<<j<<" : "<<ETA[i]<<"->"<<binsX[nx]<<endl;
	  for(int iphi=0;iphi<int(dphi/dphi_min+0.5);++iphi)
	  {
	    binsY[ny]=PHI[i]-dphi/2. + iphi*dphi_min;
	    SIGMA_etendu[nx][ny]=SIGMA[i];
	    ++ny;
	  }
	  if(j+1!=n)//not the last iteration
	  {
	    if(ETA[i]!=ETA[index[j+1]])//eta changes
	    {	  
	      binsY[ny]=binsY[ny-1]+dphi_min;//bin sup
	      ++nx;
	      ny=0;//RAZ  
	    }
	  }
	  else//last iteration (so last eta)
	  {	    
	    ++nx;
	    binsX[nx]=binsX[nx-1]+deta;//bin sup 
	    binsY[ny]=binsY[ny-1]+dphi_min;//bin sup
	    //cout<<"last: "<<binsX[nx]<<" "<<binsY[ny]<<endl;	    
	  }
	}
	
	for(int i=0;i<nx+1;++i)
	  for(int j=0;j<ny+1;++j)
	    if(i<nx && j<ny) 
	      debug<<binsX[i]<<" "<<binsY[j]<<" "<<SIGMA_etendu[i][j]<<endl;
	    else
	      debug<<binsX[i]<<" "<<binsY[j]<<" ***"<<endl;
	
	//remplit le graphe
	canvas2D[iCalo][iLayer] = new TCanvas(m_name_canvas2D[iCalo][iLayer],
				              m_name_canvas2D[iCalo][iLayer],
					      1);
	(canvas2D[iCalo][iLayer])->SetBorderMode(0);
	(canvas2D[iCalo][iLayer])->SetFillColor(10);
	graph2D[iCalo][iLayer]=new TH2F(m_name_canvas2D[iCalo][iLayer],
					m_name_canvas2D[iCalo][iLayer],
					nx,binsX,ny,binsY);
	graph2D[iCalo][iLayer]->SetMinimum(0);
	graph2D[iCalo][iLayer]->SetMaximum(seuil);
	cout<<"new OK"<<endl;
	
	for(int i=0;i<nx;++i)
	  for(int j=0;j<ny;++j)
          {
	    graph2D[iCalo][iLayer]->SetBinContent(i+1,j+1,SIGMA_etendu[i][j]);
	  }
	cout<<"fill OK "<<graph2D[iCalo][iLayer]->GetEntries()<<endl;
	
	graph2D[iCalo][iLayer]->Draw("COLZ");
	canvas2D[iCalo][iLayer]->Print(m_ps2D[iCalo][iLayer]);
      }
    }

    if(NDIMENSIONS[iCalo]==1)
    {
      multi_graph[iCalo]->SetMinimum(0); 
      //multi_graph[iCalo]->SetMaximum(m_maximum[iCalo]);
      multi_graph[iCalo]->SetTitle(m_legend_calo[iCalo]);
      multi_graph[iCalo]->Draw("a");

      multi_graph[iCalo]->GetXaxis()->SetTitle("#eta");
      //multi_graph[iCalo]->GetXaxis()->SetTitleSize(.01);
      //multi_graph[iCalo]->GetXaxis()->SetTitleOffset(1.);
      multi_graph[iCalo]->GetXaxis()->SetTitleFont(62);
      multi_graph[iCalo]->GetXaxis()->SetLabelFont(62);
      multi_graph[iCalo]->GetYaxis()->SetTitle("Noise (MeV)");
      //multi_graph[iCalo]->GetYaxis()->SetTitleSize(.005);
      //multi_graph[iCalo]->GetYaxis()->SetTitleOffset(1.);
      multi_graph[iCalo]->GetYaxis()->SetTitleFont(62);
      multi_graph[iCalo]->GetYaxis()->SetLabelFont(62);

      //leg1D[iCalo]->SetHeader(m_legend_calo[iCalo]);
      //multi_graph[iCalo]->SetTitle(m_legend_calo[iCalo]);
      leg1D[iCalo]->SetHeader(0);
      leg1D[iCalo]->SetBorderSize(1);
      leg1D[iCalo]->SetFillColor(10);
      leg1D[iCalo]->SetTextSize(0.04);
      leg1D[iCalo]->SetTextFont(42);
      leg1D[iCalo]->Draw();      
     
      canvas1D[iCalo]->Print(m_ps1D[iCalo]);
      //canvas1D[iCalo]->Close();   
    }

  }//loop calos
  debug.close();


  for(int iSamp=0;iSamp<m_nLayers_max;++iSamp)      
    for(int iReg=0;iReg<m_nReg_max;++iReg)
    {
      if(Hnoise[iSamp][iReg]->GetEntries()>0)
      {
	cout<<iSamp<<" "<<iReg<<" : "
	    <<Hnoise[iSamp][iReg]->GetMean()<<" +- "
	    <<Hnoise[iSamp][iReg]->GetRMS()<<endl;
      }
    }
  
}

///////////////////////////////////////////////////////////////////

int store_data(ifstream data);
{
  /*
  cout<<"store_data ";
  int n=0;
  do 
  {	  	  
    char id[1024];
    if(data>>id)
    {
      data>>ETA[n]>>PHI[n];
      data>>VOLUME[n]>>SIGMA[n]>>errSIGMA[n]>>MEAN[n]>>errMEAN[n];	  
      if(!ERRORS) {errSIGMA[n]=0; errMEAN[n]=0;} 	  
      read_id(std::string(id),n);	    	  
      ++n;
    }	   
  }
  while(!data.eof());
  cout<<"OK"<<endl;
  return n;
  */
}
    
///////////////////////////////////////////////////////////////////

void facteurs()
{     
  for(int iCalo=0;iCalo<m_nCalos;++iCalo) 
    for(int iLayer=0;iLayer<m_nLayers[iCalo];++iLayer) 
    {
      if(DIG) m_fac[iCalo][iLayer]=25.;
      if(RAW) m_fac[iCalo][iLayer]=1.;
      if(CEL) m_fac[iCalo][iLayer]=1000.;
      m_fac[iCalo][iLayer]=1.;
    }
}

///////////////////////////////////////////////////////////////////

void largeur_eta_phi(int iDet, int iLayer, int iReg, float &weta, float &wphi)
{
  //cout<<"largeurs: ";
  float dphi=0.02454369;
  float deta=0.025;
  int iCalo=calo(iDet);
  switch(iCalo)
    {
    case EMB:
      if(iLayer==0)                       dphi*=4.;
      if(iLayer==1 && iReg==0)  {deta/=8.;dphi*=4.;}
      if(iLayer==2 && iReg==1)   deta*=3.;
      if(iLayer==3)              deta*=2.;
      break;
    case EMEC: 
      if(iCalo==1/*OW*/) 
      {
	if(iLayer>=2) /*dphi=dphi*/; else dphi*=4.;
	if(iLayer==1) 
	  if(iReg==0) deta*=2.;
	  else if(iReg==2) deta/=8.;
	  else if(iReg==2) deta/=6.;
	  else if(iReg==2) deta/=4.;
	if(iLayer==2 && iReg==0) deta*=2.;
	if(iLayer==3) deta*=2.;
      }
      if(iCalo==2/*IW*/) deta*=4.;
      break;
    case HEC: break;
    }
  weta=deta;
  wphi=dphi;
  //cout<<iDet<<" "<<iLayer<<" "<<iReg<<" "<<weta<<" "<<wphi<<endl;
}

///////////////////////////////////////////////////////////////////

void read_id(std::string id,int n)
{
  //cout<<"read_id: ";
  char dot;
  std::istringstream es( id.c_str() );
  es>>DET[n]>>dot>>SAMP[n]>>dot>>REG[n]>>dot>>IETA[n]>>dot>>IPHI[n];
  //cout<<" -> "<<id<<" / "<<DET[n]<<" "<<SAMP[n]<<" "<<REG[n]<<" "<<IETA[n]<<" "<<IPHI[n]<<endl;
}

///////////////////////////////////////////////////////////////////

float order_tabs(int n)
{
  cout<<"order_tabs ";

  //en fait, on n'ordonne pas les tableaux mais plutot un tableau d'indices !
  int itmp;

  //tableau initial de 0 a n-1
  for(int i=0;i<n;++i) 
  {
    index[i]=i;
    fill_Hnoise(i);
  }

  //ordonne en eta croissant
  for(int i=0;i<n;++i)     
    for(int j=i;j<n;++j)        
      if(ETA[index[i]]>ETA[index[j]])
      {
	itmp=index[j];
	index[j]=index[i];
	index[i]=itmp;	  	  
      }

  //ordonne en phi croissant pour chaque eta
  for(int i=0;i<n;++i)     
    for(int j=i;j<n;++j)        
      if(ETA[index[i]]==ETA[index[j]])
	if(PHI[index[i]]>PHI[index[j]])
        {
	  itmp=index[j];
	  index[j]=index[i];
	  index[i]=itmp;	  	  
	}
 
  //trouve le plus petit delta-phi
  float dphi_min=10000;
  for(int j=0;j<n;++j)   
  {
    int i=index[j];
    float deta,dphi;
    largeur_eta_phi(DET[i],SAMP[i],REG[i],deta,dphi);
    DETA[i]=deta;
    DPHI[i]=dphi;
    if(dphi<dphi_min) dphi_min=dphi;
  }

  cout<<"OK"<<endl;
  return dphi;
}

///////////////////////////////////////////////////////////////////

void fill_Hnoise(int indice)
{
  int i=indice;
  //if(ETA[i]<1.1 && ETA[i]>0.9 && PHI[i]<0.3 && PHI[i]>0.1) return;
  int iSamp=SAMP[i];
  int iReg =REG[i];

  // RUN1000185 / RUN1000297 / RUN1000298
  /*
  if(iSamp==2 && PHI[i]>0.2 && iReg==0) iReg=2;
  if(iSamp==2 && PHI[i]>0.2 && iReg==1) iReg=3;
  */

  // RUN1000473 / RUN1000474
  
  if(iSamp==2 && PHI[i]>0.2 && iReg==1) iReg=2;
  if(iSamp==2 && ETA[i]<0.8) iReg=3;  
  if(iSamp==1 && PHI[i]>0.2 && iReg==1) iReg=2;
  if(iSamp==1 && PHI[i]<0.2 && ETA[i]>1 && iReg==0) iReg=3;
  if(iSamp==3 && PHI[i]<0.2 && ETA[i]>0.8) iReg=1;
  

  Hnoise[iSamp][iReg]->Fill(SIGMA[i]);
}

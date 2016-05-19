// bug dans les ntuples que j ai fait avec 10.0.4: remplacer ZV[IVPRIMARY] par ZV[0] 

// Z -> e+ e- avec Jimmy :  abs(Type[ipart]) == 11 && Type[KMothNt[ipart]] == Type[ipart] && (GenStat[KMothNt[ipart]] == 123 || GenStat[KMothNt[ipart]] == 124)

// Top : W -> 2e


#define t3333_cxx
#include "t3333.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TROOT.h>
#include <TTree.h>
#include <TF1.h>
#include <TH1.h>

#include <TGraphErrors.h>
#include <TMath.h>
#include <TString.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLine.h>
#include <TMinuit.h>
#include "TObjArray.h"
#include "TFile.h"
#include "TKey.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TGaxis.h"
#include "TUnixSystem.h"
#include <math.h>
#include <iomanip>
#include <fstream>
#include <time.h>


#include <iostream>



string filePar;


int getPar()
{
 char line[999];
  FILE *f=gSystem->OpenPipe("grep RTTINFO CaloRec_RTT*log | cut -d= -f2 | tail -n1","r");
  if(f)
    {
      fgets(line,999,f);
      if (strlen(line)>0)
	line[strlen(line)-1] = 0;
      else 
	return -1;
    }
  
  if(-1 == fclose(f)){
    std::cerr<<"can not close file"<< std::endl;
  }

  string temp(line);
  string temp2="";
  for(int i=0;i<temp.size();i++)
    if (temp[i]!=' ')
      temp2+=temp[i];

  filePar=temp2+".par";
  string com="wget http://marwww.in2p3.fr/~kbernard/caloperf/"+filePar;
  gSystem->Exec(com.c_str());

  FILE *f2=fopen(filePar.c_str(),"r");
   if(!f2)
     {
      ofstream chkfile("Check2.txt",ios::app);
      chkfile<< "Problem to get " << filePar << endl;
      chkfile.close();
      return -1;
     }
   fclose(f2);
   return 1;
}

int mysign(double val)
{
  return (val<0 ? -1:1);
}



void saveHisto(TH1F *h1, char *opt)
{
  FILE *html= fopen("truth.html","a");

  FILE *txt= fopen("truth.txt","a");

  TString tmp=h1->GetTitle();
  TString name="";
  for(int i=0;i<tmp.Length();i++)
    {
      if (tmp[i]==' ')
	tmp[i]='_';
      if (tmp[i]=='(')
	tmp[i]='z';
      if (tmp[i]==')')
	tmp[i]='z';
      name+=tmp[i];
    }

  //  TString com="grep RTTParm_truth_"+name+ "  CaloRec_RTT*_log | cut -d= -f2 | tail -n1";

  TString com="grep RTTParm_truth_"+name+ " "+filePar +" | cut -d= -f2 | tail -n1";
  
  // std::cout << com << std::endl;

  char line[999];
  FILE *f=gSystem->OpenPipe(com.Data(),"r");
  if(f)
    {
      fgets(line,999,f);
      line[strlen(line)-1] = 0;
    }
  
  if(-1 == fclose(f)){
    std::cerr<<"can not close file"<< std::endl;
  }

  float lim=atof(line);

  std::cout << "Lim = " << lim << std::endl;

  //  gStyle->SetOptStat(kFALSE);
  TCanvas *can=new TCanvas();
  // only 3 digits
  TGaxis::SetMaxDigits(3);
 
  h1->SetLineColor(2);
  h1->SetLineStyle(2);
  h1->Draw(opt);

  fprintf(txt,"################################\n");
  fprintf(txt,"Name : %s\n",h1->GetTitle());
  fprintf(txt,"Mean = %lf\n",h1->GetMean());
  fprintf(txt,"RMS = %lf\n",h1->GetRMS());


  can->Modified();
  can->Update(); 

  TString epsname=name+".eps";
  TString gifname=name+".gif";

  can->SaveAs(epsname.Data());

  char comm1[500],comm2[500];
  sprintf(comm1,"%s %s","pstopnm -ppm -xborder 0 -yborder 0 -portrait",epsname.Data());
  sprintf(comm2,"%s %s%s %s","ppmtogif",epsname.Data(),"001.ppm >",gifname.Data());

  gSystem->Exec(comm1);
  gSystem->Exec(comm2);
 
  fprintf(html,"<CENTER><BR>");
  fprintf(html,"<BR>");
  fprintf(html,"<IMG SRC=\"%s\"><BR>",gifname.Data());
  fprintf(html,"<HR>");
  fclose(html);
  fclose(txt);

  delete can;

}

void saveHisto(TH1F *h1)
{
  FILE *html= fopen("truth.html","a");
  FILE *txt= fopen("truth.txt","a");


  TString tmp=h1->GetTitle();
  TString name="";
  for(int i=0;i<tmp.Length();i++)
    {
      if (tmp[i]==' ')
	tmp[i]='_';
      if (tmp[i]=='(')
	tmp[i]='z';
      if (tmp[i]==')')
	tmp[i]='z';
      name+=tmp[i];
    }

  //  TString com="grep \"RTTParm_truth_"+name+ " \"  CaloRec_RTT*_log | cut -d= -f2 | tail -n1";
  TString com="grep RTTParm_truth_"+name+ " "+filePar +" | cut -d= -f2 | tail -n1";
  
  char line[999];
  FILE *f=gSystem->OpenPipe(com.Data(),"r");
  if(f)
    {
      fgets(line,999,f);
      line[strlen(line)-1] = 0;
    }
  
  if(-1 == fclose(f)){
    std::cerr<<"Can not close file"<< std::endl;
  }

  float lim=atof(line);

  std::cout << "Lim = " << lim << std::endl;


  //  gStyle->SetOptStat(kFALSE);
  TCanvas *can=new TCanvas();
 
  // only 3 digits
  TGaxis::SetMaxDigits(3);
 

  h1->SetLineColor(2);
  h1->SetLineStyle(2);
  gStyle->SetOptStat(1111111);

  h1->Draw();

  fprintf(txt,"################################\n");
  fprintf(txt,"Name : %s\n",h1->GetTitle());
  fprintf(txt,"Mean = %lf\n",h1->GetMean());
  fprintf(txt,"RMS = %lf\n",h1->GetRMS());

  
  h1->Fit("gaus");
  TF1 *func = h1->GetFunction("gaus");
  Double_t parm[3];
  func->GetParameters(parm);
  fprintf(txt,"Fit Const = %lf\n",parm[0]);
  fprintf(txt,"Fit Mean = %lf\n",parm[1]);
  fprintf(txt,"Fit Sigma = %lf\n",parm[2]);
 
  int isOk=1;
  if ( parm[1] > lim )
    {
      isOk=0;
      fprintf(txt,"TEST FAILED (lim = %f)\n",lim);
    }
  else
    fprintf(txt,"TEST OK (lim = %f)\n",lim);


  can->Modified();
  can->Update(); 

  TString epsname=name+".eps";
  TString gifname=name+".gif";

  can->SaveAs(epsname.Data());

  char comm1[500],comm2[500];
  sprintf(comm1,"%s %s","pstopnm -ppm -xborder 0 -yborder 0 -portrait",epsname.Data());
  sprintf(comm2,"%s %s%s %s","ppmtogif",epsname.Data(),"001.ppm >",gifname.Data());

  gSystem->Exec(comm1);
  gSystem->Exec(comm2);
 
  fprintf(html,"<CENTER><BR>");
  fprintf(html,"<BR>");
  fprintf(html,"<IMG SRC=\"%s\"><BR>",gifname.Data());

  if ( isOk == 0 ) 
    {
      fprintf(html,"<BR>");
      fprintf(html,"<CENTER><font color=red size=10>TEST FAILED (lim = %f)</font></CENTER>",lim);
    }
  else
    {
      fprintf(html,"<BR>");
      fprintf(html,"<CENTER><font color=red size=10>TEST OK (lim = %f)</font></CENTER>",lim);
      fprintf(html,"<BR>");
    }

  fprintf(html,"<HR>");
  fclose(html);
  fclose(txt);

  delete can;

}





void t3333::Loop(int itype)
{
  //   In a ROOT session, you can do:
  //      Root > .L t3333.C
  //      Root > t3333 t
  //      Root > t.GetEntry(12); // Fill t data members with entry number 12
  //      Root > t.Show();       // Show values of entry 12
  //      Root > t.Show(16);     // Read and show values of entry 16
  //      Root > t.Loop();       // Loop on all entries
  //

  //     This is the loop skeleton where:
  //    jentry is the global entry number in the chain
  //    ientry is the entry number in the current Tree
  //  Note that the argument to GetEntry must be:
  //    jentry for TChain::GetEntry
  //    ientry for TTree::GetEntry and TBranch::GetEntry
  //
  //       To read only selected branches, Insert statements like:
  // METHOD1:
  //    fChain->SetBranchStatus("*",0);  // disable all branches
  //    fChain->SetBranchStatus("branchname",1);  // activate branchname
  // METHOD2: replace line
  //    fChain->GetEntry(jentry);       //read all branches
  //by  b_branchname->GetEntry(ientry); //read only this branch
  if (fChain == 0) return;


  // get par
int res=getPar();
  if (res<0)
return;

  ofstream foutput("truth.html",ios::out);
  foutput << "<HTML><HEAD><TITLE>TEST</TITLE></HEAD>"<< endl;
  foutput <<"<BODY>" <<endl;
  foutput.close();



  
  /////////////////

  TH1F *m_drmin  = new TH1F("m_drmin","drmin",100,-0.2,0.2); 
  
  

  ////////////////:

  TH1F *m_h1   = new TH1F("m_h1","Energy resolution",100,-0.25,0.25); 
  m_h1->SetXTitle("(E_{t}(reco)-E_{t}(true))/E_{t}(true)");

  TH1F *m_h2   = new TH1F("m_h2","Phi resolution",100,-0.01,0.01); 
  m_h2->SetXTitle("#Phi resolution (rad)");

  TH1F *m_h3   = new TH1F("m_h3","Eta resolution in the barrel",100,-0.01,0.01); m_h3->SetXTitle("#eta resolution");

  TH1F *m_h4   = new TH1F("m_h4","Eta resolution in the endcap",100,-0.01,0.01); m_h4->SetXTitle("#eta resolution");



  TH1F *m_h5   = new TH1F("m_h5","Efficiency vs eta",50,-3,3);   
  m_h5->SetXTitle("#eta");
  TH1F *m_tmp1   = new TH1F("m_tmp1","EtaGen[0]",50,-3,3); 
  TH1F *m_tmp2  = new TH1F("m_tmp2","cl_eta[0]",50,-3,3); 



  if ( itype == 1 ) //photons
    {

      Long64_t nentries = fChain->GetEntriesFast();

      Int_t nbytes = 0, nb = 0;
      for (Long64_t jentry=0; jentry<nentries;jentry++)
	{
	  Long64_t ientry = LoadTree(jentry);
	  if (ientry < 0) break;
	  nb = fChain->GetEntry(jentry);   nbytes += nb;
	  // if (Cut(ientry) < 0) continue;

	  b_IEvent->GetEntry(ientry);       
	  //std::cout<< "Event " << IEvent <<std::endl;

	  // resolution in energy
	  b_cl_et->GetEntry(ientry);
	  b_PtGen->GetEntry(ientry);
	  b_Type->GetEntry(ientry);
	   
	  //std::cout<< "pdg id " << Type[0] <<std::endl;


	  for (Long64_t i=0; i<cl_nc;i++)
	    {
	       
	      double val=(cl_et[i]-PtGen[0])/PtGen[0];
	      m_h1->Fill(val);
	    }
	   
	  // resolution in phi
	  b_cl_phi->GetEntry(ientry);
	  b_PhiGen->GetEntry(ientry);
	  for (Long64_t i=0; i<cl_nc;i++)
	    {
	      if ( cl_phi[i]-PhiGen[0] < 6 )
		{
		  double val=(cl_phi[i]-PhiGen[0]);
		  m_h2->Fill(val);
		}
	    }
	   
	   
	  // resolution in eta barrel corrected by the z vertex spread
	  b_cl_eta->GetEntry(ientry);
	  b_EtaGen->GetEntry(ientry);
	  b_IVPrimary->GetEntry(ientry);
	  b_ZV->GetEntry(ientry);
	   
	  for (Long64_t i=0; i<cl_nc;i++)
	    {
	      if ( fabs(EtaGen[0])<1.475 )
		{
		  double val=cl_eta[i]-asinh(sinh(EtaGen[0])+ZV[IVPrimary]/1600.);
		  m_h3->Fill(val);
		}
	    }
	   
	  // resolution in eta endcap corrected by the z vertex spread
	  b_cl_eta->GetEntry(ientry);
	  b_EtaGen->GetEntry(ientry);
	  b_IVPrimary->GetEntry(ientry);
	  b_ZV->GetEntry(ientry);
	   
	  for (Long64_t i=0; i<cl_nc;i++)
	    {
	      if ( fabs(EtaGen[0])>1.475 )
		{
		  double val=cl_eta[i]-asinh(sinh(EtaGen[0])/(1-mysign(EtaGen[0])*ZV[IVPrimary]/3800.));
 
		  m_h4->Fill(val);
		}
	    }


	  m_tmp2->Fill(EtaGen[0]);
	  m_tmp1->Fill(cl_eta[0]);
	}
 
    }
  else
    if (itype == 2 )  // single electrons -> BUGGY pour eta
      {

	Long64_t nentries = fChain->GetEntriesFast();

	Int_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++)
	  {
	    Long64_t ientry = LoadTree(jentry);
	    if (ientry < 0) break;
	    nb = fChain->GetEntry(jentry);   nbytes += nb;

	    b_IEvent->GetEntry(ientry);       
	    // std::cout<< "Event " << IEvent <<std::endl;

	  
	    b_NPar->GetEntry(ientry);  
	    b_Type->GetEntry(ientry);  
	    b_KMothNt->GetEntry(ientry); 

	    
	    int indEle[4];
	    int iele=0;
	    for (int ipart=0; ipart<NPar; ipart++)
	      if ( abs(Type[ipart])==11 && GenStat[ipart]==1 && KMothNt[ipart]==-1)
		{
		  indEle[iele]=ipart;
m_tmp2->Fill(EtaGen[indEle[iele]]);

		  iele=iele+1;
		  
		  if (iele>1)
		    {
		      std::cout<< "two many electrons " <<std::endl;
		      exit(-1);
		    }
		}

	    int nele=iele;

	    //    for (int j=0; j<nele;j++)
	    //   std::cout<< "Electron " << j << " is at "<<  indEle[j]  <<std::endl;


	    // a quel cluster correspond quel electron ?
	    // je tourne sur ts les clusters de l ev
	    for(int ic = 0; ic < cl_nc; ic++) 
	      {
		double etacl = cl_eta[ic];
		double phicl = cl_phi[ic];
		double etcl  = cl_et[ic];
		double etaclcor;
		double phiclcor;

		m_drmin->Fill(ZV[0]);
		double etae = EtaGen[indEle[0]];
		if ( fabs(cl_eta[ic])>1.475 )
		  {
		    etaclcor = asinh(sinh(cl_eta[ic])*(1-ZV[IVPrimary]/(mysign(cl_eta[ic])*3800.0)));
		    phiclcor = cl_phi[ic]+(0.3*3800*(-Type[indEle[0]]/11)*mysign(cl_eta[ic]))/(cl_et[ic]*sinh(cl_eta[ic]));

		    m_h4->Fill(etaclcor-etae);
		  }
		else
		  {
		    etaclcor = asinh(sinh(cl_eta[ic])-ZV[IVPrimary]/1600.0);
		   
		    m_h3->Fill(etaclcor-etae);
		    phiclcor = cl_phi[ic]+(0.3*1600.0*(-Type[indEle[0]]/11.0)/cl_et[ic]);
		  }
		  
		double phie = PhiGen[indEle[0]];
		double ete  = PtGen[indEle[0]];	  
		   	    
		m_h2->Fill(phiclcor-phie);
		m_h1->Fill((etcl-ete)/ete);
		  
		m_tmp1->Fill(etae);
		//	m_tmp2->Fill(etacl);
		  
	      }

	    /////////////////////////////////////////////////
	  }
  
      }


    else
      if (itype == 3 )  // H->ZZ, Z->ee, Z->mumu
	{
 	  Long64_t nentries = fChain->GetEntriesFast();

	  Int_t nbytes = 0, nb = 0;
	  for (Long64_t jentry=0; jentry<nentries;jentry++)
	    {
	      Long64_t ientry = LoadTree(jentry);
	      if (ientry < 0) break;
	      nb = fChain->GetEntry(jentry);   nbytes += nb;

	      b_IEvent->GetEntry(ientry);       
	      //std::cout<< "Event " << IEvent <<std::endl;

	      // find the 2 e-
	      b_NPar->GetEntry(ientry);  
	      b_Type->GetEntry(ientry);  
	      b_KMothNt->GetEntry(ientry); 

	    
	      int indEle[4];
	      int iele=0;
	      for (int ipart=0; ipart<NPar; ipart++)
		if ( abs(Type[ipart])==11 && GenStat[ipart]==1 && Type[KMothNt[ipart]]==23)
		  {
		    indEle[iele]=ipart;
m_tmp2->Fill(EtaGen[indEle[iele]]);

		    iele=iele+1;

		    if (iele>4)
		      {
			std::cout<< "two many electrons " <<std::endl;
			exit(-1);
		      }
		  }

	      int nele=iele;

	      //for (int j=0; j<nele;j++)
	      //	std::cout<< "Electron " << j << " is at "<<  indEle[j]  <<std::endl;


	      // a quel cluster correspond quel electron ?
	      // je tourne sur ts les clusters de l ev
	      for(int ic = 0; ic < cl_nc; ic++) 
		{
		  double drmin = 9999.;
		  int    im    = 0;

		  //pour un cluster donne je tourne sur tous les electrons primaires trouves precedemment et je minimise dr pour savoir celui qui est le plus pres du cluster 
		  for (int iele = 0; iele < nele; iele++) 
		    {
		      double deta = EtaGen[indEle[iele]] - cl_eta[ic];
		      double dphi = PhiGen[indEle[iele]] - cl_phi[ic];
		      if (dphi > M_PI) 
			double dphi = fabs(dphi) - 2.*M_PI;
		      double dr = sqrt(dphi*dphi + deta*deta);
		    
		      if (dr < drmin) 
			{
			  drmin = dr;
			  im    = iele;
			}
		    }
		  // // l'electron matchant le cluster a l'indice im
		  //      // resol et efficacite :
		  //  coupure |Eta|< 2.55 et Pt > 5Gev 
		  if (drmin < 0.2 &&  fabs(EtaGen[indEle[im]])<2.55 && PtGen[indEle[im]]>5000  ) 
		    {
		      m_drmin->Fill(drmin);

		      double etacl = cl_eta[ic];
		      double phicl = cl_phi[ic];
		      double etcl  = cl_et[ic];
		      double etaclcor;
		      double phiclcor;
		      double etae = EtaGen[indEle[im]];
		      if ( fabs(cl_eta[ic])>1.475 )
			{
			  etaclcor = asinh(sinh(cl_eta[ic])*(1-ZV[IVPrimary]/(mysign(cl_eta[ic])*3800.0)));
			  phiclcor = cl_phi[ic]+(0.3*3800*(-Type[indEle[im]]/11)*mysign(cl_eta[ic]))/(cl_et[ic]*sinh(cl_eta[ic]));
			  m_h4->Fill(etaclcor-etae);
			}
		      else
			{
			  etaclcor = asinh(sinh(cl_eta[ic])-ZV[IVPrimary]/1600.0);
			  phiclcor = cl_phi[ic]+(0.3*1600.0*(-Type[indEle[im]]/11.0)/cl_et[ic]);
			  m_h3->Fill(etaclcor-etae);
			}

			
		  
	
		      double phie = PhiGen[indEle[im]];
		      double ete  = PtGen[indEle[im]];

		      m_h2->Fill(phiclcor-phie);
		      m_h1->Fill((etcl-ete)/ete);
		      m_tmp1->Fill(etae);
		      //    m_tmp2->Fill(etacl);
		    }
		}

	      /////////////////////////////////////////////////
	    }

	}
      else
	if (itype == 4 )  // Z -> ee JIMMY (attention diff avec Pythia!) 
	  {

	    Long64_t nentries = fChain->GetEntriesFast();

	    Int_t nbytes = 0, nb = 0;
	    for (Long64_t jentry=0; jentry<nentries;jentry++)
	      {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		b_IEvent->GetEntry(ientry);       
		std::cout<< "Event " << IEvent <<std::endl;

		// find the 2 e-
		b_NPar->GetEntry(ientry);  
		b_Type->GetEntry(ientry);  
		b_KMothNt->GetEntry(ientry); 

	    
		int indEle[4];
		int iele=0;
		for (int ipart=0; ipart<NPar; ipart++)
	
		  if ( abs(Type[ipart]) == 11 && Type[KMothNt[ipart]] == Type[ipart] &&
		       (GenStat[KMothNt[ipart]] == 123 || GenStat[KMothNt[ipart]] == 124) )
		    {
		      indEle[iele]=ipart;
 m_tmp2->Fill(EtaGen[indEle[iele]]);

		      iele=iele+1;

		      if (iele>4)
			{
			  std::cout<< "two many electrons " <<std::endl;
			  exit(-1);
			}
		    }

		int nele=iele;

		//	for (int j=0; j<nele;j++)
		//	  std::cout<< "Electron " << j << " is at "<<  indEle[j]  <<std::endl;


		// a quel cluster correspond quel electron ?
		// je tourne sur ts les clusters de l ev
		for(int ic = 0; ic < cl_nc; ic++) 
		  {
		    double drmin = 9999.;
		    int    im    = 0;

		    //pour un cluster donne je tourne sur tous les electrons primaires trouves precedemment et je minimise dr pour savoir celui qui est le plus pres du cluster 
		    for (int iele = 0; iele < nele; iele++) 
		      {
		   

			double deta = EtaGen[indEle[iele]] - cl_eta[ic];
			double dphi = PhiGen[indEle[iele]] - cl_phi[ic];
			if (dphi > M_PI) 
			  double dphi = fabs(dphi) - 2.*M_PI;
			double dr = sqrt(dphi*dphi + deta*deta);
		    
			if (dr < drmin) 
			  {
			    drmin = dr;
			    im    = iele;
			  }
		      }
		    // // l'electron matchant le cluster a l'indice im
		 
		  
		    m_drmin->Fill(drmin);

		    if (drmin < 0.1  ) 
		      {
			double etacl = cl_eta[ic];
			double phicl = cl_phi[ic];
			double etcl  = cl_et[ic];
			double etaclcor;
			double phiclcor;
			double etae = EtaGen[indEle[im]];
			if ( fabs(cl_eta[ic])>1.475 )
			  {
			    etaclcor = asinh(sinh(cl_eta[ic])*(1-ZV[IVPrimary]/(mysign(cl_eta[ic])*3800.0)));
			    phiclcor = cl_phi[ic]+(0.3*3800*(-Type[indEle[im]]/11)*mysign(cl_eta[ic]))/(cl_et[ic]*sinh(cl_eta[ic]));
			    m_h4->Fill(etaclcor-etae);
			  }
			else
			  {
			    etaclcor = asinh(sinh(cl_eta[ic])-ZV[IVPrimary]/1600.0);
			    phiclcor = cl_phi[ic]+(0.3*1600.0*(-Type[indEle[im]]/11.0)/cl_et[ic]);
			    m_h3->Fill(etaclcor-etae);
			  }
			double phie = PhiGen[indEle[im]];
			double ete  = PtGen[indEle[im]];
	
			m_h2->Fill(phiclcor-phie);
			m_h1->Fill((etcl-ete)/ete);	 
			m_tmp1->Fill(etae);
			//	m_tmp2->Fill(etacl);

		      }
		  }

		/////////////////////////////////////////////////
	      }
 
	  }

else
	if (itype == 5 )  // Top 
	  {

	    Long64_t nentries = fChain->GetEntriesFast();

	    Int_t nbytes = 0, nb = 0;
	    for (Long64_t jentry=0; jentry<nentries;jentry++)
	      {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		b_IEvent->GetEntry(ientry);       
		std::cout<< "Event " << IEvent <<std::endl;

		// find the e-
		b_NPar->GetEntry(ientry);  
		b_Type->GetEntry(ientry);  
		b_KMothNt->GetEntry(ientry); 

		int indEle[4];
		int iele=0;
		for (int ipart=0; ipart<NPar; ipart++)
	
		  // if ( abs(Type[ipart]) == 11 && Type[KMothNt[ipart]] == Type[ipart] &&    (GenStat[KMothNt[ipart]] == 123 || GenStat[KMothNt[ipart]] == 124) )
		  //	  if (  abs(Type[ipart])==11 && GenStat[ipart]==1 && Type[KMothNt[ipart]]==24 )
		  if (  abs(Type[ipart])==11 && abs(Type[KMothNt[ipart]])==24 )
		    {
		      indEle[iele]=ipart;
   m_tmp2->Fill(EtaGen[indEle[iele]]);

		      iele=iele+1;

		      if (iele>4)
			{
			  std::cout<< "two many electrons " <<std::endl;
			  exit(-1);
			}
		    }

		int nele=iele;

		//	for (int j=0; j<nele;j++)
		//	  std::cout<< "Electron " << j << " is at "<<  indEle[j]  <<std::endl;


		// a quel cluster correspond quel electron ?
		// je tourne sur ts les clusters de l ev
		for(int ic = 0; ic < cl_nc; ic++) 
		  {
		    double drmin = 9999.;
		    int    im    = 0;

		    //pour un cluster donne je tourne sur tous les electrons primaires trouves precedemment et je minimise dr pour savoir celui qui est le plus pres du cluster 
		    for (int iele = 0; iele < nele; iele++) 
		      {
		 

			double deta = EtaGen[indEle[iele]] - cl_eta[ic];
			double dphi = PhiGen[indEle[iele]] - cl_phi[ic];
			if (dphi > M_PI) 
			  double dphi = fabs(dphi) - 2.*M_PI;
			double dr = sqrt(dphi*dphi + deta*deta);
		    
			if (dr < drmin) 
			  {
			    drmin = dr;
			    im    = iele;
			  }
		      }
		    // // l'electron matchant le cluster a l'indice im
		 
		  
		    m_drmin->Fill(drmin);

		    if (drmin < 0.1  ) 
		      {
			double etacl = cl_eta[ic];
			double phicl = cl_phi[ic];
			double etcl  = cl_et[ic];
			double etaclcor;
			double phiclcor;
			double etae = EtaGen[indEle[im]];
			if ( fabs(cl_eta[ic])>1.475 )
			  {
			    etaclcor = asinh(sinh(cl_eta[ic])*(1-ZV[IVPrimary]/(mysign(cl_eta[ic])*3800.0)));
			    phiclcor = cl_phi[ic]+(0.3*3800*(-Type[indEle[im]]/11)*mysign(cl_eta[ic]))/(cl_et[ic]*sinh(cl_eta[ic]));
			    m_h4->Fill(etaclcor-etae);
			  }
			else
			  {
			    etaclcor = asinh(sinh(cl_eta[ic])-ZV[IVPrimary]/1600.0);
			    phiclcor = cl_phi[ic]+(0.3*1600.0*(-Type[indEle[im]]/11.0)/cl_et[ic]);
			    m_h3->Fill(etaclcor-etae);
			  }
			double phie = PhiGen[indEle[im]];
			double ete  = PtGen[indEle[im]];
	
			m_h2->Fill(phiclcor-phie);
			m_h1->Fill((etcl-ete)/ete);	 
			m_tmp1->Fill(etae);
			//	m_tmp2->Fill(etacl);

		      }
		  }

		/////////////////////////////////////////////////
	      }
 
	  }




  gStyle->SetOptFit(1011);

 
  m_h1->Fit("gaus");
  m_h2->Fit("gaus");
  m_h3->Fit("gaus");
  m_h4->Fit("gaus");

  //saveHisto(m_drmin);
    		    
  saveHisto(m_h1);
  saveHisto(m_h2);
  saveHisto(m_h3);
  saveHisto(m_h4);

  // efficiency vs eta
  m_h5->Divide(m_tmp1,m_tmp2,1,1,"B");  
  saveHisto(m_h5,"E");


}


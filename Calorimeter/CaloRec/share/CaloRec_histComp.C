#include <iostream>
#include <algorithm>

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

gROOT->Reset();

int debug=0;

char *ROOTNtuple="ntuple.root";
char *histTest="Cluster.hist";


string filePar;

string findRef()
{
  char line[999] = {0};
  FILE *f=gSystem->OpenPipe("grep RTTINFO CaloRec_RTT*log | cut -d= -f2 | tail -n1","r");
  if(f)
    {
      fgets(line,999,f);
      if (strlen(line)>0)
	line[strlen(line)-1] = 0;
      else 
	return -1;
  
      if(-1 == fclose(f)){
        std::cerr<<"can not close file"<< std::endl;
      }
    }

  string temp(line);
  string temp2="";
  for(int i=0;i<temp.size();i++)
    if (temp[i]!=' ')
      temp2+=temp[i];

  return temp2;
}


int getPar()
{
  char line[999] = {0};
  FILE *f=gSystem->OpenPipe("grep RTTINFO CaloRec_RTT*log | cut -d= -f2 | tail -n1","r");
  if(f)
    {
      fgets(line,999,f);
      if (strlen(line)>0)
	line[strlen(line)-1] = 0;
      else 
	return -1;
  
      if(-1 == fclose(f)){
        std::cerr<<"can not close file"<< std::endl;
      }
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
      ofstream chkfile("Check.txt",ios::app);
      chkfile<< "Problem to get " << filePar << endl;
      chkfile.close();
      return -1;
     }
   fclose(f2);
   return 1;
}

void CaloRec_histComp()
{
  ofstream chkfile("Check.txt",ios::out);
  chkfile << "CaloRec_histComp : begin " << endl;

  

  // make histos
  int res=CaloRec_makeHist();
if (res<0)
return;

  chkfile << "CaloRec_histComp : makeHist done " << endl;
 

  // get par
res=getPar();
  if (res<0)
return;

  string tmp=findRef();

  REFHist="ref"+tmp+".hist";


  FILE *fich;
  if ( (fich=fopen(REFHist.c_str(),"r")) == NULL)
    {
      chkfile <<  REFHist << " not found !" << endl;
 
      chkfile.close();

      Draw(histTest);

	return;
    }
	else
	fclose(fich);


  chkfile << "Reference histo is : " << REFHist <<endl;


 // comparison
  ofstream foutput("HistoComparison.txt",ios::out);
  foutput <<"******************************************************************************" << endl;
  foutput<<"  offset  error_offset  Chi2_offset  Chi2_histo  Kolmogorov_test  histo"<<  endl;
  foutput << "  ------  ------------  -----------  ----------  ---------------  -----"<< endl;
  foutput.close();



  int res;

  res=histComparison(histTest,REFHist.c_str(),"Complete");
 
  res=histComparison(histTest,REFHist.c_str(),"identical");

  ofstream foutput("almostIdentical.html",ios::out);
  foutput << "<HTML><HEAD><TITLE>TEST</TITLE></HEAD>"<< endl;
  foutput <<"<BODY>" <<endl;
  foutput.close();
  res=histComparison(histTest,REFHist.c_str(),"almostIdentical");
  ofstream foutput("almostIdentical.html",ios::app);
  foutput << "</BODY>"<< endl;
  foutput <<"</HTML>" <<endl;

  chkfile.close(); 
}

// Make Histograms

void normH(TH1F* m_hist)
{
  ofstream chkfile("Check.txt",ios::app);
  if ( m_hist->GetEntries() != 0 )
    {
      double scale = 1/m_hist->GetEntries();
      m_hist->Scale(scale);
    }
  else
    {
      chkfile << "No entries in " << m_hist->GetTitle() << std::endl;
    }
  chkfile.close();
}

int makeHist (TString theFile)
{
  // change default style 
  // round filled marker

  gStyle->SetMarkerStyle(8);
  gStyle->SetStatW(0.4);
  //gStyle->SetStatFontSize(60);
  gStyle->SetStatFontSize(0.12);
  gStyle->SetLabelSize(0.06);
  gStyle->SetOptStat(111110);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(0);
  gStyle->SetStatColor(0);

  // only 3 digits
  TGaxis::SetMaxDigits(3);

FILE *fich;

if ((fich=fopen(theFile.Data(),"r"))==NULL)	
return -1;
else
fclose(fich);

  //open file
  TFile f(theFile);

  f.cd("CBNT") ;
  // this is needed only if pure numeric ttree identifier is used
  TTree *t3333=(TTree*) gDirectory->Get("t3333");

  //use default style
  t3333->UseCurrentStyle();

  TFile *hfile = new TFile("Cluster.hist","RECREATE","Clusters hist");

  TH1F *m_h1    = new TH1F("m_h1","Cluster E (GeV)",50,70.,110.);
  TH1F *m_h2    = new TH1F("m_h2","Cluster E (GeV) - barrel",50,70.,110.);
  TH1F *m_h3    = new TH1F("m_h3","Cluster E (GeV) - endcap",50,70.,110.);
  TH1F *m_h4    = new TH1F("m_h4","Cluster eta",40,-2.5,2.5);

  TH1F *m_h5    = new TH1F("m_h5","Cluster phi",25,-7.,7.);
  TH1F *m_h6    = new TH1F("m_h6","number of clusters",6,-.5,5.5);
 
  TH1F *m_h7    = new TH1F("m_h7","E frac. in Presampler - barrel",50,0.,1.);
  TH1F *m_h8   = new TH1F("m_h8","E frac. in 1st samp. - barrel",50,0.,1.);
  TH1F *m_h9   = new TH1F("m_h9","E frac. in 2nd samp. - barrel",50,0.,1.);
  TH1F *m_h10   = new TH1F("m_h10","E frac. in 3rd samp. - barrel",50,0.,.1);

  TH1F *m_h11   = new TH1F("m_h11","E frac. in Presampler - endcap",50,0.,1.);
  TH1F *m_h12   = new TH1F("m_h12","E frac. in 1st samp. - endcap",50,0.,1.);
  TH1F *m_h13   = new TH1F("m_h13","E frac. in 2nd samp. - endcap",50,0.,1.);
  TH1F *m_h14   = new TH1F("m_h14","E frac. in 3rd samp. - endcap",50,0.,0.1);

 
  TCanvas *myCanvas = new TCanvas("myCanvas","Cluster",1);

  myCanvas->Divide(2,8);
  myCanvas->cd(1);
  t3333->Draw("cl_et*cosh(cl_eta)/1000","");
  t3333->Draw("cl_et*cosh(cl_eta)/1000>>m_h1","");
  normH(m_h1);

  myCanvas->cd(2);
  t3333->Draw("cl_et*cosh(cl_eta)/1000","abs(cl_eta)<1.475");
  t3333->Draw("cl_et*cosh(cl_eta)/1000>>m_h2","abs(cl_eta)<1.475");
  normH(m_h2);

  myCanvas->cd(3);
  t3333->Draw("cl_et*cosh(cl_eta)/1000","abs(cl_eta)>=1.475");
  t3333->Draw("cl_et*cosh(cl_eta)/1000>>m_h3","abs(cl_eta)>=1.475");
  normH(m_h3);

  myCanvas->cd(4);
  t3333->Draw("cl_eta","");
  t3333->Draw("cl_eta>>m_h4","");
  normH(m_h4);

  myCanvas->cd(5);
  t3333->Draw("cl_phi","","");
  t3333->Draw("cl_phi>>m_h5","","");
  normH(m_h5);

  myCanvas->cd(6);
  t3333->Draw("cl_nc","","");
  t3333->Draw("cl_nc>>m_h6","","");
  normH(m_h6);

  myCanvas->cd(7);
  t3333->Draw("cl_eemb0/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)","abs(cl_eta)<1.475","");
  t3333->Draw("cl_eemb0/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h7","abs(cl_eta)<1.475","");
  normH(m_h7);

  myCanvas->cd(8);
  t3333->Draw("cl_eemb1/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h8","abs(cl_eta)<1.475","");
  t3333->Draw("cl_eemb1/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h8","abs(cl_eta)<1.475","");
  normH(m_h8);

  myCanvas->cd(9);
  t3333->Draw("cl_eemb2/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)","abs(cl_eta)<1.475","");
  t3333->Draw("cl_eemb2/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h9","abs(cl_eta)<1.475","");
  normH(m_h9);

  myCanvas->cd(10);
  t3333->Draw("cl_eemb3/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)","abs(cl_eta)<1.475","");
  t3333->Draw("cl_eemb3/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h10","abs(cl_eta)<1.475","");
  normH(m_h10);

  myCanvas->cd(11);     
  t3333->Draw("cl_eeme0/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)","abs(cl_eta)>=1.475 && abs(cl_eta)<=1.8","");
  t3333->Draw("cl_eeme0/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h11","abs(cl_eta)>=1.475 && abs(cl_eta)<=1.8","");
  normH(m_h11);

  myCanvas->cd(12);
  t3333->Draw("cl_eeme1/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)","abs(cl_eta)>=1.475","");
  t3333->Draw("cl_eeme1/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h12","abs(cl_eta)>=1.475","");
  normH(m_h12);

  myCanvas->cd(13);
  t3333->Draw("cl_eeme2/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)","abs(cl_eta)>=1.475","");
  t3333->Draw("cl_eeme2/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h13","abs(cl_eta)>=1.475","");
  normH(m_h13);

  myCanvas->cd(14);
  t3333->Draw("cl_eeme3/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)","abs(cl_eta)>=1.475","");
  t3333->Draw("cl_eeme3/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h14","abs(cl_eta)>=1.475","");
  normH(m_h14);

  //myCanvas->Print("Cluster.ps");
 
  hfile->Write();

  hfile->Close();
}

void CaloRec_makeHist()
{
  makeHist(ROOTNtuple);
}


//=========================================================================


// Comparison stuff



int calcSize(TObjArray* col){
  int count = 0;
  TIter iter = col->MakeIterator();
  while(iter.Next()){++count;}
  return count;
}


TObjArray* getHists(TFile * f){
  if (debug==1) 
    printf("getHists: start\n");
  TObjArray* hists = new TObjArray(0);
  int size = hists->GetEntries();
  if (debug==1)  
    printf("getHists: size on creation: %d\n",size);  

  TIter next(f->GetListOfKeys());
  TKey *key;
  while ((key=(TKey*)next())) {
    const char* name = key->GetName();
    if (debug==1) 
      printf("getHists: obj name %s\n", name);
    TObject* obj = f->Get(name);
    hists->Add(obj);
  }  
  
  size = calcSize(hists);
  return hists;
}

//=========================================================================

bool identical(TH1 *h1,TH1 *h2)
{
  FILE *log = fopen("identical.txt","a");
  if ( !log )
    {
      std::cout << "Can't open identical.txt !" << std::endl;
      exit(1);
    }

  int nbins1 = (int) h1->GetEntries();
  int  nbins2  = (int) h2->GetEntries();

  fprintf(log,"###################################\n"); 
  fprintf(log,"Name REF: %s\n",h1->GetTitle()); 
  fprintf(log,"Nbin1 : %d\n",nbins1); 
  fprintf(log,"Mean : %lf\n",h1->GetMean());
  fprintf(log,"RMS : %lf\n",h1->GetRMS());

  fprintf(log,"Name : %s\n",h2->GetTitle()); 
  fprintf(log,"Nbin2 : %d\n",nbins1); 
  fprintf(log,"Mean : %lf\n",h2->GetMean());
  fprintf(log,"RMS : %lf\n",h2->GetRMS());



  if (nbins1 != nbins2 || h1->GetMean()!=h2->GetMean() || h1->GetRMS()!=h2->GetRMS())
    { 
      fprintf(log,"Histogram number of entries, mean, or sigma differ\n");
      fprintf(log,"###################################\n"); 
      fclose(log);
      return false;
    }
  else
    {
      fprintf(log,"Histogram number of entries, mean, and sigma are identical\n");
      fprintf(log,"###################################\n"); 
      fclose(log);
      return true;
    }
	    
}

//=========================================================================

bool almostIdentical(TH1 *h1,TH1 *h2)
{
  FILE *html= fopen("almostIdentical.html","a");

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

  gStyle->SetOptStat(kFALSE);
  TCanvas *can=new TCanvas();
 
  h1->SetLineColor(2);
  h1->SetLineStyle(2);
  h1->Draw();

  h2->Draw((h1) ? "sames" : "");

  TLegend *legend=new TLegend(0.7,0.7,0.89,0.85);
  legend->SetTextFont(72);
  legend->SetTextSize(0.04);
  legend->AddEntry(h2,"REF","l");
  legend->AddEntry(h1,"NEW","l");
  legend->Draw();
 
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
 
  delete can;

  //////////////////
  FILE *log = fopen("almostIdentical.txt","a");
  if ( !log )
    {
      std::cout << "Can't open almostIdentical.txt !" << std::endl;
      exit(1);
    }

  fprintf(log,"###################################\n"); 
  fprintf(log,"Name : %s\n",h1->GetTitle());

  fprintf(html,"<CENTER><BR>");
  fprintf(html,"Name : %s<BR>",h1->GetTitle());
  fprintf(html,"<BR>"); 
  fprintf(html,"<TABLE border=1>");
  fprintf(html,"<TR>");
  fprintf(html,"<TD></TD>");
  fprintf(html,"<TD>REF</TD>");
  fprintf(html,"<TD>NEW</TD>");
  fprintf(html,"</TR>");

  fprintf(html,"<TR>");
  fprintf(html,"<TD>Entries</TD>");
  fprintf(html,"<TD>%d</TD>",h2->GetEntries());
  fprintf(html,"<TD>%d</TD>",h1->GetEntries());
  fprintf(html,"</TR>");

  fprintf(html,"<TR>");
  fprintf(html,"<TD>Mean</TD>");
  fprintf(html,"<TD>%f</TD>",h2->GetMean());
  fprintf(html,"<TD>%f</TD>",h1->GetMean());
  fprintf(html,"</TR>");

  fprintf(html,"<TR>");
  fprintf(html,"<TD>RMS</TD>");
  fprintf(html,"<TD>%f</TD>",h2->GetRMS());
  fprintf(html,"<TD>%f</TD>",h1->GetRMS());
  fprintf(html,"</TR>");

  fprintf(html,"</TABLE>");


  double dA = 0;
  double A = 0;

  TString com="grep \"RTTParm_almostIdentical_"+name+ " \"  "+TString(filePar)+ "| cut -d= -f2 | tail -n1";
  
  std::cout << com << std::endl;

  char line[999] = {0};
  FILE *f=gSystem->OpenPipe(com.Data(),"r");
  if(f)
    {
      fgets(line,999,f);
      line[strlen(line)-1] = 0;
  
      if(-1 == fclose(f)){
        std::cerr<<"Can not close file"<< std::endl;
      }
    }

  float cut=atof(line);

  std::cout << "cut = " << cut << std::endl;





  for (int k=0; k <= h1->GetNbinsX()+1; k++) {
    dA += fabs(h1->GetBinContent(k) - h2->GetBinContent(k));
    A += h1->GetBinContent(k);
  }

  if (A > 0) {
    dA = dA / A;
  }
  
  fprintf(log,"&Sigma;(|&Delta;A|)/&Sigma;A is %f (cut is %f)\n",dA,cut);
  fprintf(html,"<BR>"); 
  fprintf(html,"&Sigma;(|&Delta;A|)/&Sigma;A is %f (cut is %f)<BR>",dA,cut);
  fprintf(html,"<BR>");
  fprintf(html,"<IMG SRC=\"%s\"><BR>",gifname.Data());
  
  if ( dA < cut )
    {
      fprintf(log,"TEST OK\n");
      fprintf(log,"###################################\n"); 
      fprintf(html,"<font size=10 color=\"red\">TEST OK<BR></font>");
      fprintf(html,"<HR>"); 
      fprintf(html,"</CENTER><BR>");
      fclose(html);
      fclose(log);
      return true;
    }
  else
    {
      fprintf(log,"TEST FAILED\n");
      fprintf(log,"###################################\n"); 
      fprintf(html,"<font size=10 color=\"red\">TEST FAILED</font><BR>");
      fprintf(html,"<HR>"); 
      fprintf(html,"</CENTER><BR>");
      fclose(html);
      fclose(log);
      return false;
    }
	    
}

//=========================================================================

void drawHisto(TH1 *h1)
{
 FILE *html= fopen("DrawHistos.html","a");


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

  gStyle->SetOptStat(kFALSE);
  TCanvas *can=new TCanvas();
 
  h1->SetLineColor(2);
  h1->SetLineStyle(2);
  h1->Draw();

  TLegend *legend=new TLegend(0.7,0.7,0.89,0.85);
  legend->SetTextFont(72);
  legend->SetTextSize(0.04);
 
  legend->AddEntry(h1,"NEW","l");
  legend->Draw();
 
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
 
  delete can;

 
  fprintf(html,"<CENTER><BR>");
  fprintf(html,"Name : %s<BR>",h1->GetTitle());
  fprintf(html,"<BR>"); 
  fprintf(html,"<TABLE border=1>");
  

  fprintf(html,"<TR>");
  fprintf(html,"<TD>Entries</TD>");
  fprintf(html,"<TD>%d</TD>",h1->GetEntries());
  fprintf(html,"</TR>");

  fprintf(html,"<TR>");
  fprintf(html,"<TD>Mean</TD>");
  fprintf(html,"<TD>%f</TD>",h1->GetMean());
  fprintf(html,"</TR>");

  fprintf(html,"<TR>");
  fprintf(html,"<TD>RMS</TD>");
  fprintf(html,"<TD>%f</TD>",h1->GetRMS());
  fprintf(html,"</TR>");

  fprintf(html,"</TABLE>");   

fprintf(html,"<IMG SRC=\"%s\"><BR>",gifname.Data());
  

  fclose(html); 
}

//=========================================================================

bool binByBinComparison(TH1 *hist1,TH1 *hist2)
{
  FILE *log = fopen("HistoComparison.txt","w");
  if ( !log )
    {
      std::cerr<< "Can't open HistoComparison.res!" << std::endl;
      exit(1);
    }

  Int_t nbins1 = hist1->GetNbinsX();
  if (debug==1)
    { 
      printf("Name : %s\n",hist1->GetName());
      printf("Title : %s\n",hist1->GetTitle());
      printf("Nbin1 : %d\n",nbins1); 
      printf("Mean : %lf\n",hist1->GetMean());
      printf("RMS : %lf\n",hist1->GetRMS());

      fprintf(log,"Name : %s\n",hist1->GetName()); 
      fprintf(log,"Nbin1 : %d\n",nbins1); 
      fprintf(log,"Mean : %lf\n",hist1->GetMean());
      fprintf(log,"RMS : %lf\n",hist1->GetRMS());
    }

  Int_t nbins2 = hist2->GetNbinsX();
  if (debug==1) 
    { 
      printf("Name : %s\n",hist2->GetName()); 
      printf("Title : %s\n",hist1->GetTitle());
      printf("Nbin2 : %d\n",nbins2);  
      printf("Mean : %lf\n",hist2->GetMean());
      printf("RMS : %lf\n",hist2->GetRMS());

      fprintf(log,"Name : %s\n",hist2->GetName()); 
      fprintf(log,"Nbin1 : %d\n",nbins2); 
      fprintf(log,"Mean : %lf\n",hist2->GetMean());
      fprintf(log,"RMS : %lf\n",hist2->GetRMS());
    }
  fclose(log);

  if(nbins1 != nbins2){
    return false;
  }
  else{
    for(Int_t binNo=0;binNo < nbins1;binNo++){
      {
	if (debug==1)
	  printf("bin %d : content1=%lf content2=%lf\n",binNo,hist1->GetBinContent(binNo),hist2->GetBinContent(binNo));
	if(hist1->GetBinContent(binNo) != hist2->GetBinContent(binNo)){
	  return false;
	}
      }
    }
  }
  return true;
}


//=========================================================================

bool statisticalComparison(TH1 *hist1,TH1 *hist2){
  hist1->Sumw2();
  hist2->Sumw2();
  double resKolmo=hist1->KolmogorovTest(hist2,"D");
  if (debug==1) 
    printf("Kolmogorov res =%lf\n",resKolmo);
  //return true;
  float probValue=0.7;
  return ( resKolmo< probValue);
}

bool chi2Comparison(TH1 *hist1,TH1 *hist2){
  double resChi2=hist1->Chi2Test(hist2,"OUP",1);
  if (debug==1) 
    printf("Chi2 res =%lf\n",resChi2);
  //return true;
  float probValue=0.7;
  return ( resChi2< probValue);
}

bool completeComparison(TH1 *h_test,TH1 *h_ref)
{
  TLine *line_1;
  TLine *line;
  
  TCanvas *comparison = new TCanvas ("comparison","Comparison",1);
  comparison -> Divide(2,2);
  
  TCanvas *comparison_2 = new TCanvas ("comparison_2","Comparison 2",1);
  comparison_2 -> Divide(2,2);
    

  Double_t integ_h_test,integ_h_ref,scale_h_comp;
  Double_t ktest_cut,Kolmog_comp;
   
  const Int_t dim = 61;
   
  Double_t bins_test[dim],bins_ref[dim],bins_comp[dim];
  Double_t bin_x[dim],error_bins_test[dim],error_bins_ref[dim];
  Double_t error_norm_bins_ref[dim];
  Double_t bins_dif[dim],error_bins_dif[dim];
   
  Double_t x_step,Chi_2,Chi_2_red;
  Double_t p0[11],e0[11],Chi2_offset;
   
  //Double_t x_offset[11],err_x_offset[11];
   
  Int_t n_bin;
  Double_t range,range_start,range_end;
   
  gStyle -> SetOptFit(0000);
   
  char name[200],name2[200],histo[200],result[50];
   
  ofstream chkfile("Check.txt",ios::app);
  ofstream foutput("HistoComparison.txt",ios::app);
 
  //

  TString temp=h_ref->GetTitle();

  TString temp2="";
  for(int i=0;i<temp.Length();i++)
    {
      if (temp[i]==' ')
	temp[i]='_';
      if (temp[i]=='(')
	temp[i]='z';
      if (temp[i]==')')
	temp[i]='z';

      temp2=temp2+temp[i];
    }
  
  chkfile << "#############################################" << endl;


  //cout << temp2<< endl;
  sprintf(name,"comp_histos_%s.ps",temp2.Data());
  sprintf(name2,"comp2_histos_%s.ps",temp2.Data());
  sprintf(histo,"%s",h_ref->GetTitle());
  chkfile << histo << endl;

  n_bin = h_ref->GetNbinsX();
  TAxis *Ax=h_ref->GetXaxis();
  range_start = Ax->GetXmin();
  range_end = Ax->GetXmax();
  range = range_end-range_start;

  if (debug==1)
    {
      std::cout<< "name : " << name << std::endl;
      std::cout<< "histo : " << histo << std::endl;
      std::cout<< "n_bin  : " << n_bin << std::endl;
      std::cout<< "range_start : " << range_start << std::endl;
      std::cout<< "range_end : " << range_end << std::endl;
      std::cout<< "range : " << range << std::endl;
    }

  comparison -> cd(1);
  TH1 *h_test_1 = (TH1*)h_test->Clone("h_test_1");
  h_test_1 -> SetYTitle("events");
  integ_h_test = h_test_1 -> Integral();
  h_test_1 -> SetLineColor(4);
  h_test_1 -> Draw();
               
               
  comparison -> cd(2);
  TH1 *h_ref_1 = (TH1*)h_ref->Clone("h_ref_1");
  h_ref_1 -> SetYTitle("events");
  integ_h_ref = h_ref_1 -> Integral();
  h_ref_1 -> SetLineColor(2);
  h_ref_1 -> Draw();


  comparison -> cd(3);
  scale_h_comp = integ_h_test/integ_h_ref;
  // scale to the number of entries in the test histo
  TH1 *h_ref_2 = (TH1*)h_ref_1->Clone("h_ref_2");
  h_ref_2 -> Scale(scale_h_comp);
  h_ref_2 -> SetLineColor(2);
  h_ref_2 -> Draw();
  TH1 *h_test_2 = (TH1*)h_test_1->Clone("h_test_2");
  h_test_2 -> SetLineStyle(3);
  h_test_2 -> SetLineColor(4);
  h_test_2 -> Draw("same");
 
  
  
  TH1 *h_ref_5 = (TH1*)h_ref->Clone("h_ref_5");
  TH1 *h_ref_4 = (TH1*)h_ref->Clone("h_ref_4");
    
  TH1 *h_test_4 = (TH1*)h_test->Clone("h_ref_4");
  TH1 *h_ref_6 = (TH1*)h_ref->Clone("h_ref_6");
  
  x_step = range/(1.*n_bin);
  Chi_2 = 0.0;
  for (Int_t j = 1; j < n_bin+1; j++) {
    bins_test[j] =  h_test_2 -> GetBinContent(j);
    error_bins_test[j] = TMath::Sqrt(bins_test[j]);
    bins_ref[j] = h_ref_2 -> GetBinContent(j);
    error_bins_ref[j] = TMath::Sqrt(bins_ref[j]);
    error_norm_bins_ref[j] = error_bins_ref[j] * scale_h_comp;
    bins_dif[j] = bins_test[j] - bins_ref[j];
    error_bins_dif[j] = TMath::Sqrt(bins_test[j]+bins_ref[j]*scale_h_comp*scale_h_comp);
      
    if (bins_test[j] == 0 && bins_ref[j] == 0) {
      Chi_2 = Chi_2 + 0.;
      bins_comp[j] = 0.;
    }
      
    else {
      Chi_2 = Chi_2 + ((bins_dif[j]*bins_dif[j])/(error_bins_dif[j]*error_bins_dif[j]));
      bins_comp[j] = bins_dif[j]/error_bins_dif[j];
    }
      
    bin_x[j] = range_start + 1.0*j*x_step;
      
    h_ref_4 -> SetBinContent(j,bins_comp[j]);
   
    
    h_ref_5 -> SetBinContent(j,bins_comp[j]);
    h_ref_5 -> SetBinError(j,1.0);
      
    h_test_4 -> SetBinContent(j,bins_test[j]);
    h_test_4 -> SetBinError(j,error_bins_test[j]);
      
    h_ref_6 -> SetBinContent(j,bins_ref[j]);
    h_ref_6 -> SetBinError(j,error_norm_bins_ref[j]);      
  }
  
  chkfile<<setw(8)<<setprecision(4)<<" Chi2 of the difference between test y norm ref = "<<Chi_2<<endl;

  chkfile<<endl;
  Chi_2_red = Chi_2/(n_bin*1.0-1.0);
  chkfile<<setw(8)<<setprecision(4)<<" Chi2 reduced = "<<Chi_2_red<<endl;   
  chkfile<<endl;
  
  
  comparison -> cd(4);
  h_ref_4 -> SetMarkerStyle(21);
  h_ref_4 -> SetMarkerColor(3);
  h_ref_4 -> SetMarkerSize(0.5);
  h_ref_4 -> Draw("P");
  line_1 = new TLine(range_start,0.0,range_end,0.0);
  line_1 -> SetLineWidth(1);
  line_1 -> SetLineStyle(3);
  line_1 -> Draw();


  comparison_2 -> cd(1);
  h_test_4 -> SetTitle("test histo with errors");
  h_test_4 -> SetMarkerStyle(24);
  h_test_4 -> SetMarkerColor(2);
  h_test_4 -> SetLineColor(2);
  h_test_4 -> SetMarkerSize(0.5);
  h_test_4 -> Draw("AP");


  comparison_2 -> cd(2);
  h_ref_6 -> SetTitle("norm ref histo with errors");
  h_ref_6 -> SetMarkerStyle(21);
  h_ref_6 -> SetMarkerColor(4);
  h_ref_6 -> SetLineColor(4);
  h_ref_6 -> SetMarkerSize(0.5);
  h_ref_6 -> Draw("AP");


  comparison_2 -> cd(3);
  scale_h_comp = integ_h_test/integ_h_ref;
  // scale to the number of entries in the test histo
  TH1 *h_ref_3 = (TH1*)h_ref_6->Clone("h_ref_3");
  h_ref_3 -> SetTitle("superimposing both histos");
  h_ref_3 -> Scale(scale_h_comp);
  h_ref_3 -> SetMarkerStyle(21);
  h_ref_3 -> SetMarkerSize(0.5);
  h_ref_3 -> SetMarkerColor(4);
  h_ref_3 -> SetLineColor(4);
  h_ref_3 -> Draw("E1");
  TH1 *h_test_3 = (TH1*)h_test_4->Clone("h_test_3");
  h_test_3 -> SetMarkerStyle(24);
  h_test_3 -> SetMarkerSize(0.5);
  h_test_3 -> SetMarkerColor(2);
  h_test_3 -> SetLineColor(2);
  h_test_3 -> Draw("E1same");

  ktest_cut = 0.7;
  h_test_3 -> Sumw2();
  h_ref_3  -> Sumw2();
  Kolmog_comp = h_test_3 -> KolmogorovTest(h_ref_3,"D");
  if(Kolmog_comp < ktest_cut) 
    chkfile << " Kolmogorov test : Fail " << endl;
  else 
    chkfile << " Kolmogorov test : Pass " << endl;
  chkfile<<setw(8)<<setprecision(4)<<" Kolmogorov test = "<<Kolmog_comp<<endl;


  comparison_2 -> cd(4);
  h_ref_5 -> SetTitle("fitting the difference between histos");
  h_ref_5 -> SetLineColor(3);
  h_ref_5 -> SetMarkerStyle(21);
  h_ref_5 -> SetMarkerColor(3);
  h_ref_5 -> SetMarkerSize(0.5);
  h_ref_5 -> Draw("AP");
  h_ref_5 -> Fit("pol0");
  h_ref_5 -> GetFunction("pol0") -> SetLineColor(2);
  TF1 *myfit = (TF1*) h_ref_5 -> GetFunction("pol0");
  p0[0] = myfit->GetParameter(0);
  e0[0] = myfit ->GetParError(0);
  chkfile <<setw(8)<<setprecision(4)<< " offset = " << p0[0] << endl;
  chkfile <<setw(8)<<setprecision(4)<< " error offset = " << e0[0] << endl;
  Chi2_offset = (p0[0]*p0[0])/((e0[0]+1.)*(e0[0]+1.));
  chkfile <<setw(8)<<setprecision(4)<< " chi2 offset = " << Chi2_offset << endl;
  line = new TLine(range_start,0.0,range_end,0.0);
  line -> SetLineWidth(1);
  line -> SetLineColor(1);
  line -> SetLineStyle(3);
  line -> Draw();
    
  int res;
  if (p0[0]>1.0 || p0[0]<-1.0 || Chi_2_red>1.0 || Kolmog_comp<0.7)  {
    sprintf(result,"WARNING");
    res=0;
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    string mess=string(asctime(timeinfo))+": "+string(temp2)+" FAILED";
    //alert(mess);

  }
  else {
    res=1;
    sprintf(result,"OK");
  }
   
  foutput << p0[0] << "   " 
	  <<  e0[0]     << "   " 
	  <<   Chi2_offset    << "   " 
	  <<  Chi_2_red     << "   " 
	  <<  Kolmog_comp     << "   " 
	  <<   histo    << "   " 
	  <<    result   << "   " 
	  << endl;

 
  comparison->Print(name);
  chkfile << " saving file " << name << endl;
  comparison_2->Print(name2);
  chkfile << " saving file " << name2 << endl;

  foutput.close();
  chkfile.close(); 
  if (res==1)
    return true;
  else
    return false;
}




//=========================================================================

bool testHistogram(TH1* hist1, TH1* hist2, string comparisonType)
{
  if (comparisonType=="bin-by-bin")
    {
      return binByBinComparison(hist1,hist2);
    }
  else
    if (comparisonType=="Kolmo")
      { 
	//Kolmogorov test
	return statisticalComparison(hist1,hist2);
      }
    else
      if (comparisonType=="Chi2")
	{
	  return chi2Comparison(hist1,hist2);
	}

      else
	if (comparisonType=="Complete")
	  {	    
	    return completeComparison(hist1,hist2);
	  }
	else
	  if (comparisonType=="identical")
	    {
	      return identical(hist1,hist2);
	    }
	  else
	    if (comparisonType=="almostIdentical")
	      {
		return almostIdentical(hist1,hist2);
	      }

  return true;
}

//=========================================================================

int cleanup(TObjArray * array1, TObjArray * array2, TFile * file1, TFile * file2){

  array1->Delete();
  delete array1;

  array2->Delete();
  delete array2;
  
  delete file1;
  delete file2;

  return 0;
}


//=========================================================================

int histComparison(char* newHistFile, char* refHistsFile, char * compType){
  int successCode = 0;
  int errorCode = 1;

  ofstream chkfile("Check.txt",ios::app);

  //------------------------------------------------------------------
  // Open the 2 histogram files and extract the histograms into 2 lists
  //------------------------------------------------------------------
  TFile* hists = new TFile(newHistFile); 
  TFile* refHists = new TFile(refHistsFile); 
    
  TObjArray *listHists  = getHists(hists);
  TObjArray *listRhists = getHists(refHists);
  
  //------------------------------------------------------------------
  // Different number of histograms in the two histogram files -> EXIT
  //------------------------------------------------------------------
  if(calcSize(listHists) != calcSize(listRhists)){
    cleanup(listHists,listRhists,hists,refHists);
    chkfile << "histComparison.C return value: false info: the histogram files " << newHistFile << " and " << refHistsFile << " did not contain the same number of histograms" << std::endl;
    chkfile.close();
    return errorCode;
  }

  //------------------------------------------------------------------
  // No histograms in the newly created histogram file -> EXIT
  //------------------------------------------------------------------
  if(calcSize(listHists) == 0 ){
    cleanup(listHists,listRhists,hists,refHists);
    chkfile << "histComparison.C return value: false info: the newly created histogram file" << newHistFile << " contains no histograms!" << std::endl;
    chkfile.close();
    return errorCode;
  }
  
  //------------------------------------------------------------------
  // Preliminary checks over, now compare each histogram in the two files
  //------------------------------------------------------------------
  TIter new_iter(listHists);
  TIter ref_iter(listRhists);

  TObjArray* failed = new TObjArray(0);
  int sizeOnInit = failed->GetEntries();

  int n=0;

  while( newHist = (TH1*)new_iter.Next()){
    
    TH1 *refHist = (TH1*)ref_iter.Next();
    if (!testHistogram(newHist,refHist,compType)){
   
      failed->Add(new TObjString(newHist->GetTitle()));
    }
  }

  //------------------------------------------------------------------
  // Some histograms were different -> EXIT
  //------------------------------------------------------------------
  if (failed->GetEntries() > sizeOnInit){
    chkfile << "histComparison return value: false info: the following histograms were not identical in the two files " << std::endl;
    cleanup(listHists,listRhists,hists,refHists);
    failed->Delete();
    delete failed;
    chkfile.close();
    return errorCode;
  }
  
  //------------------------------------------------------------------
  // All OK
  //------------------------------------------------------------------
  chkfile  << "histComparison.C return value: true" << endl;
  chkfile.close();
  return successCode;
}


void alert(string mess)
{
  FILE *f=gSystem->OpenPipe("mail -s \"Validation problem\" bernardet@cppm.in2p3.fr","w");
  if(!f){
    std::cerr<<"can not send mail"<< std::endl;
    return;
  }

  fprintf(f,mess.c_str());

  if(-1 == fclose(f)){
    std::cerr<<"can not close file"<< std::endl;
  }
}

void Draw(char* HistFile)
{

  std::cout<< "Draw the histos" << std::endl;

  TFile* hists = new TFile(HistFile);     
  TObjArray *listHists  = getHists(hists);
  TIter iter(listHists);
  
  while( Hist = (TH1*)iter.Next())
    {
    
      drawHisto(Hist);
   
    }

}

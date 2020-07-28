#include "TFile.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TLatex.h"
#include "TF1.h"
#include "TLegend.h"

const int n_ptbin=16;
const int n_etabin=8;
const int n_b_bins=19;


Double_t get_vn_b (Double_t *x, Double_t *par);
Double_t get_vn_pt(Double_t *x, Double_t *par);
void reso();


void DrawPlots_MB(int implementation_type=0)
{
reso();

char name[100];
TFile *infile=new TFile("all.root");
TFile *outfile=new TFile("temp.root","recreate");
TProfile *prof1,*prof2,*prof3,*prof4;
TH1D     *hist1,*hist2;
TLatex Tex;
TLegend *Leg;

float pt_binvals[]={0,.25,.5,1,1.5,2,2.5,3,3.5,4,6,8,12,16,20,30,40};
float b_bin_vals[]={0.0     ,3.4955,4.9315 ,6.0375 ,6.9695 ,7.7895 ,8.5335 ,9.2135 ,9.8515 ,10.4485,11.0175,11.554 ,12.070 ,12.560 ,13.033 ,13.492 ,13.944 ,14.409 ,14.929,15.6425};
float b_bin_cent[]={2.33033,4.25428,5.50309,6.51463,7.38709,8.16715,8.87784,9.53606,10.1529,10.7355,11.2881,11.8139,12.3166,12.7985,13.2643,13.7197,14.1783,14.6705,15.2883};
TF1 *func1  = new TF1("func1" ,get_vn_b ,0,25,4);
TF1 *func2  = new TF1("func2" ,get_vn_pt,0,25,4);



hist1=new TH1D("Low_value" ,"Minimum vn",n_b_bins,-0.5,n_b_bins-0.5);hist1->SetLineColor(6);hist1->SetLineStyle(2);
hist2=new TH1D("High_value","Maximum vn",n_b_bins,-0.5,n_b_bins-0.5);hist2->SetLineColor(4);hist2->SetLineStyle(2);
for(int ihar=2;ihar<=6;ihar++){
  sprintf(name,"harmonic%d_b",ihar);
  TCanvas *C2=new TCanvas(name,name,1600,1200);
  C2->Divide(3,3);
  for(int ipt=0;ipt<9;ipt++){
    C2->cd(ipt+1);
    for(int ieta=0;ieta<5;ieta++){

      sprintf(name,"profile_b_dep_%d_pt%d_eta%d",ihar,ipt,ieta);
      prof1=(TProfile*)infile->Get(name);
      if(!prof1) {cout<<name<<" not found"<<endl;continue;}


      sprintf(name,"profile_b_dep_reco_%d_pt%d_eta%d",ihar,ipt,ieta);
      prof2=(TProfile*)infile->Get(name);
      if(!prof2) {cout<<name<<" not found"<<endl;continue;}

      if(ieta<5){
        if(ieta==0) {prof3= (TProfile*) prof1->Clone();}
        else        {prof3->Add(prof1);}

        if(ieta==0) {prof4= (TProfile*) prof2->Clone();}
        else        {prof4->Add(prof2);}
      }
    }
    for(int ib=0;ib<n_b_bins;ib++){
       func1->SetParameter(0,1.25); //mean eta
       func1->SetParameter(1,(pt_binvals[ipt]+pt_binvals[ipt+1])/2.0);
       func1->SetParameter(2,ihar);
       func1->SetParameter(3,implementation_type);
       hist1->SetBinContent(ib+1,func1->Eval(b_bin_vals[ib]));
       hist2->SetBinContent(ib+1,func1->Eval(b_bin_vals[ib+1]));
    }
    prof3->SetLineColor(2);
    prof3->SetMarkerColor(2);
    prof3->SetMarkerStyle(20);
    prof3->SetMarkerSize (0.3);
    prof3->SetStats(0);
    prof4->SetLineColor(4);
    prof4->SetMarkerColor(4);
    prof4->SetMarkerStyle(20);
    prof4->SetMarkerSize (0.3);
 
    prof3->Draw();
    prof4->Draw("same");
    hist1->DrawClone("same");
    hist2->DrawClone("same");



    sprintf(name,"pT(%.1f,%.1f)",pt_binvals[ipt],pt_binvals[ipt+1]);
    Tex.DrawTextNDC(0.5,0.7,name);

  }
  sprintf(name,"~/web/v%d_b.gif",ihar);
  C2->SaveAs(name);
  C2->Write();
}
prof3=0;prof4=0;




for(int ihar=2;ihar<=6;ihar++){
for(int ib_canvas=0;ib_canvas<2;ib_canvas++) {  //0-45% is canvas0,   45-90% is canvas1
  sprintf(name,"harmonic%d_cav%d_pt",ihar,ib_canvas);
  TCanvas *C2=new TCanvas(name,name,1600,1200);
  C2->Divide(3,3);
  for(int ib=ib_canvas*9;ib<(1+ib_canvas)*9;ib++){
    C2->cd(ib+1 -ib_canvas*9);
    for(int ieta=0;ieta<5;ieta++){

      sprintf(name,"profile_pt_dep_%d_eta%d_b%d",ihar,ieta,ib);
      prof1=(TProfile*)infile->Get(name);
      if(!prof1) {cout<<name<<" not found"<<endl;continue;}


      sprintf(name,"profile_pt_dep_reco_%d_eta%d_b%d",ihar,ieta,ib);
      prof2=(TProfile*)infile->Get(name);
      if(!prof2) {cout<<name<<" not found"<<endl;continue;}

      if(ieta<5){
        if(ieta==0) {prof3= (TProfile*) prof1->Clone();}
        else        {prof3->Add(prof1);}

        if(ieta==0) {prof4= (TProfile*) prof2->Clone();}
        else        {prof4->Add(prof2);}
      }
    }
    prof3->GetXaxis()->SetRangeUser(0,18);
    prof3->SetLineColor(2);
    prof3->SetMarkerColor(2);
    prof3->SetMarkerStyle(20);
    prof3->SetMarkerSize (0.3);
    prof3->SetStats(0);
    prof4->SetLineColor(4);
    prof4->SetMarkerColor(4);
    prof4->SetMarkerStyle(20);
    prof4->SetMarkerSize (0.3);
 
    prof3->GetYaxis()->SetRangeUser(0, prof4->GetMaximum()*1.05);
    prof3->Draw();
    prof4->Draw("same");

    func2->SetParameter(0,1.25);
    func2->SetParameter(1,b_bin_vals[ib]);
    func2->SetParameter(2,ihar);
    func2->SetParameter(3,implementation_type);
    func2->SetLineColor(3);
    func2->SetLineStyle(2);
    //func2->DrawClone("same");
    func2->SetParameter(1,b_bin_vals[ib+1]);
    func2->SetLineColor(6);
    //func2->DrawClone("same");

    func2->SetParameter(1,b_bin_cent[ib]);
    func2->SetLineStyle(1);
    func2->SetLineColor(1);
    func2->DrawClone("same");


    sprintf(name,"cent(%d,%d)",ib*5,ib*5+5);
    Tex.DrawTextNDC(0.5,0.7,name);
  }
  sprintf(name,"~/web/v%d_%d_pt.gif",ihar,+ib_canvas);
  C2->SaveAs(name);
  C2->Write();
}
}


}













Double_t get_vn_pt(Double_t *x, Double_t *par)
{

  float pt=x[0]*1000;
  float eta=par[0];
  float b=par[1];
  int ihar =(int) (par[2]);
  int implementation_type= (int) (par[3]);


  if(implementation_type==1 && ihar !=2 ) return 0;  // physical v2
  if(implementation_type==2 || implementation_type==6){    // constant vn or custom vn(which was choosen equal to constant vn)
    if(ihar==1) return 0.0000;
    if(ihar==2) return 0.0500;
    if(ihar==3) return 0.0280;
    if(ihar==4) return 0.0130;
    if(ihar==5) return 0.0045;
    if(ihar==6) return 0.0015;

  }
  if(implementation_type==3){                        // constant v2 only
    if (ihar==2) return 0.05;
    else         return 0.0 ;
  }
  if(implementation_type==4){
    if(ihar !=2) return 0;
    return 0.03968 * b
               * (1 - 2.1/(1 + exp(1.357*(pt/1000))))
               * exp(-(eta*eta)/(2*6.37*6.37));

  }
                                                     //Otherwise physical vn

  if (ihar==1 || ihar >6) return 0;
  pt=pt/1000.0; //convert to GeV

  float a1,a2,a3,a4;
  a1=0.4397*exp(-(b-4.526)*(b-4.526)/72.0) + 0.636;
  a2=1.916/(b+2) +0.1;
  a3=4.79*0.0001*(b-0.621)*(b-10.172)*(b-23)+1.2;   // this is >0 for b>0
  a4=0.135*exp(-0.5*(b-10.855)*(b-10.855)/4.607/4.607) +0.0120;

  float temp1 = pow(pt    , a1) / (1+exp( (pt-3.0)/a3));
  float temp2 = pow(pt+0.1,-a2) / (1+exp(-(pt-4.5)/a3));
  float temp3 =  0.01           / (1+exp(-(pt-4.5)/a3));

  float v2    = ( a4*(temp1+temp2) + temp3 )* exp(-0.5* eta*eta /6.27/6.27) ;

  if(ihar==2) return v2;

  if(ihar==3) {
    float fb=0.97 +1.06*exp(-0.5*b*b/3.2/3.2);
    return pow(fb*sqrt(v2),3);
  }
  float gb= 1.096 +1.36 *exp(-0.5*b*b/3.0/3.0);
  gb=gb*sqrt(v2);
  return pow(gb*sqrt(v2),ihar);


}


















Double_t get_vn_b(Double_t *x, Double_t *par)
{

  float pt=par[1]*1000;
  float eta=par[0];
  float b=x[0];
  int ihar =(int) (par[2]);
  int implementation_type= (int) (par[3]);


  if(implementation_type==1 && ihar !=2 ) return 0;  // physical v2
  if(implementation_type==2 || implementation_type==6){    // constant vn or custom vn(which was choosen equal to constant vn)
    if(ihar==1) return 0.0000;
    if(ihar==2) return 0.0500;
    if(ihar==3) return 0.0280;
    if(ihar==4) return 0.0130;
    if(ihar==5) return 0.0045;
    if(ihar==6) return 0.0015;

  }
  if(implementation_type==3){                        // constant v2 only
    if (ihar==2) return 0.05;
    else         return 0.0 ;
  }
  if(implementation_type==4){
    if(ihar !=2) return 0;
    return 0.03968 * b
               * (1 - 2.1/(1 + exp(1.357*(pt/1000))))
               * exp(-(eta*eta)/(2*6.37*6.37));

  }
                                                     //Otherwise physical vn

  if (ihar==1 || ihar >6) return 0;
  pt=pt/1000.0; //convert to GeV

  float a1,a2,a3,a4;
  a1=0.4397*exp(-(b-4.526)*(b-4.526)/72.0) + 0.636;
  a2=1.916/(b+2) +0.1;
  a3=4.79*0.0001*(b-0.621)*(b-10.172)*(b-23)+1.2;   // this is >0 for b>0
  a4=0.135*exp(-0.5*(b-10.855)*(b-10.855)/4.607/4.607) +0.0120;

  float temp1 = pow(pt    , a1) / (1+exp( (pt-3.0)/a3));
  float temp2 = pow(pt+0.1,-a2) / (1+exp(-(pt-4.5)/a3));
  float temp3 =  0.01           / (1+exp(-(pt-4.5)/a3));

  float v2    = ( a4*(temp1+temp2) + temp3 )* exp(-0.5* eta*eta /6.27/6.27) ;

  if(ihar==2) return v2;

  if(ihar==3) {
    float fb=0.97 +1.06*exp(-0.5*b*b/3.2/3.2);
    return pow(fb*sqrt(v2),3);
  }
  float gb= 1.096 +1.36 *exp(-0.5*b*b/3.0/3.0);
  gb=gb*sqrt(v2);
  //return pow(gb*sqrt(v2),ihar);
  return pow(gb,ihar);


}








//Correct the vn values about the reconstructed Reaction planes by the resolution
//This function must be called first, and then the other functions can be run
void reso(){


char name[100];
TProfile *profile_pt_dep_reco [6][n_b_bins][n_etabin],*profile_pt_dep [6][n_b_bins][n_etabin];
TProfile *profile_eta_dep_reco[6][n_b_bins][n_ptbin] ,*profile_eta_dep[6][n_b_bins][n_ptbin];
TProfile *profile_b_dep_reco  [6][n_ptbin] [n_etabin],*profile_b_dep  [6][n_ptbin] [n_etabin];
TH1D     *hist_b_dep_reco     [6][n_ptbin] [n_etabin];
TProfile *profile_resolution[6];
TH1D     *hist_resolution[6];

TFile *Infile =new TFile("all0.root");
TFile *Outfile=new TFile("all.root" ,"recreate");
Outfile->cd();

if(!Infile) {cout<<"Infile not found"<<endl;exit(0);}
for(int ihar=0;ihar<6;ihar++){

  sprintf(name,"profile_resolution_%d",ihar+1);
  profile_resolution[ihar]=(TProfile*)Infile->Get(name);

  sprintf(name,"profile_resolution_%d",ihar+1);
  hist_resolution   [ihar]=new TH1D (name,name,n_b_bins,-0.5,n_b_bins-0.5);

  if(!profile_resolution[ihar]) {cout<<name<<" dosesnt exist"<<endl;exit(0);}
  for(int ib_imp=0;ib_imp<n_b_bins;ib_imp++){
    for(int ieta=0;ieta<n_etabin;ieta++){
      sprintf(name,"profile_pt_dep_reco_%d_eta%d_b%d",ihar+1,ieta,ib_imp);  
      profile_pt_dep_reco [ihar][ib_imp][ieta]=(TProfile*)Infile->Get(name);
      if(!profile_pt_dep_reco [ihar][ib_imp][ieta]) {cout<<name<<" dosesnt exist"<<endl;exit(0);}
      sprintf(name,"profile_pt_dep_%d_eta%d_b%d",ihar+1,ieta,ib_imp);  
      profile_pt_dep      [ihar][ib_imp][ieta]=(TProfile*)Infile->Get(name);
      if(!profile_pt_dep      [ihar][ib_imp][ieta]) {cout<<name<<" dosesnt exist"<<endl;exit(0);}
    }
    for(int ipt=0;ipt<n_ptbin;ipt++){
      sprintf(name,"profile_eta_dep_reco_%d_pt%d_b%d",ihar+1,ipt,ib_imp);  
      profile_eta_dep_reco[ihar][ib_imp][ipt]=(TProfile*)Infile->Get(name);
      if(!profile_eta_dep_reco[ihar][ib_imp][ipt]) {cout<<name<<" dosesnt exist"<<endl;exit(0);}
      sprintf(name,"profile_eta_dep_%d_pt%d_b%d",ihar+1,ipt,ib_imp);  
      profile_eta_dep     [ihar][ib_imp][ipt]=(TProfile*)Infile->Get(name);
      if(!profile_eta_dep     [ihar][ib_imp][ipt]) {cout<<name<<" dosesnt exist"<<endl;exit(0);}
    }
  }
  for(int ipt=0;ipt<n_ptbin;ipt++){
    for(int ieta=0;ieta<n_etabin;ieta++){
      sprintf(name ,"profile_b_dep_reco_%d_pt%d_eta%d",ihar+1,ipt,ieta);
      profile_b_dep_reco[ihar][ipt][ieta]=(TProfile*)Infile->Get(name);
      if(!profile_b_dep_reco[ihar][ipt][ieta]) {cout<<name<<" dosesnt exist"<<endl;exit(0);}
      sprintf(name ,"profile_b_dep_%d_pt%d_eta%d",ihar+1,ipt,ieta);
      profile_b_dep     [ihar][ipt][ieta]=(TProfile*)Infile->Get(name);
      if(!profile_b_dep     [ihar][ipt][ieta]) {cout<<name<<" dosesnt exist"<<endl;exit(0);}


      sprintf(name ,"hist_b_dep_reco_%d_pt%d_eta%d",ihar+1,ipt,ieta);
      hist_b_dep_reco[ihar][ipt][ieta]=new TH1D (name,name,n_b_bins,-0.5,n_b_bins-0.5);
    }
  }
} 


for(int ihar=0;ihar<6;ihar++){
  for(int ib_imp=0;ib_imp<n_b_bins;ib_imp++){
    double reso=profile_resolution[ihar]->GetBinContent(ib_imp+1);
    if (reso >=0) reso= std::sqrt( reso);
    else          reso=-std::sqrt(-reso);

    for(int ieta=0;ieta<n_etabin;ieta++){
      profile_pt_dep_reco [ihar][ib_imp][ieta]->Scale(1.0/reso);
    }
    for(int ipt=0;ipt<n_ptbin;ipt++){
     profile_eta_dep_reco[ihar][ib_imp][ipt]->Scale(1.0/reso);
    }

    for(int ipt=0;ipt<n_ptbin;ipt++){
      for(int ieta=0;ieta<n_etabin;ieta++){
        float val=profile_b_dep_reco[ihar][ipt][ieta]->GetBinContent(ib_imp+1)/reso;
        float err=profile_b_dep_reco[ihar][ipt][ieta]->GetBinError  (ib_imp+1)/reso;
        
        hist_b_dep_reco[ihar][ipt][ieta]->SetBinContent(ib_imp+1,val);
        hist_b_dep_reco[ihar][ipt][ieta]->GetBinError  (ib_imp+1,err);

        Int_t  nentries=profile_b_dep_reco[ihar][ipt][ieta]->GetBinEntries(ib_imp+1);
        val=val*nentries; 
        profile_b_dep_reco[ihar][ipt][ieta]->SetBinContent(ib_imp+1,val);
        profile_b_dep_reco[ihar][ipt][ieta]->GetBinError  (ib_imp+1,err);
        profile_b_dep_reco[ihar][ipt][ieta]->SetBinEntries(ib_imp+1,nentries);
      }
    }
    hist_resolution[ihar]->SetBinContent(ib_imp+1,reso);
    float err=profile_resolution[ihar]->GetBinError(ib_imp+1);
    err=  (sqrt(reso*reso +err) - sqrt(reso*reso -err))/2.0;
    hist_resolution[ihar]->SetBinError  (ib_imp+1,err);
  }
} 


for(int ihar=0;ihar<6;ihar++){
  hist_resolution[ihar]->Write();
  for(int ib_imp=0;ib_imp<n_b_bins;ib_imp++){
    for(int ieta=0;ieta<n_etabin;ieta++){
      profile_pt_dep_reco [ihar][ib_imp][ieta]->Write();
      profile_pt_dep      [ihar][ib_imp][ieta]->Write();
    }
    for(int ipt=0;ipt<n_ptbin;ipt++){
     profile_eta_dep_reco[ihar][ib_imp][ipt]->Write();
     profile_eta_dep     [ihar][ib_imp][ipt]->Write();
    }
  }
  for(int ipt=0;ipt<n_ptbin;ipt++){
    for(int ieta=0;ieta<n_etabin;ieta++){
      hist_b_dep_reco[ihar][ipt][ieta]->Write();
      profile_b_dep_reco[ihar][ipt][ieta]->Write();
      profile_b_dep  [ihar][ipt][ieta]->Write();
    }
  }
} 




Outfile->Close(); 
Infile ->Close(); 
}

 

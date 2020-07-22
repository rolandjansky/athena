#include "TFile.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TLatex.h"
#include "TF1.h"
#include "TLegend.h"



Double_t get_vn(Double_t *x, Double_t *par);
void reso();


void DrawPlots(int implementation_type=0)
{
reso();

char name[100];
TFile *infile=new TFile("all.root");
TFile *outfile=new TFile("temp.root","recreate");
TProfile *prof1,*prof2,*prof3,*prof4;
TLatex Tex;
TLegend *Leg;

float pt_binvals[]={0,.25,.5,1,1.5,2,2.5,3,3.5,4,6,8,12,16,20,30,40};
TF1 *etadep;
if(implementation_type<7){
  etadep = new TF1("etadep","[0]*exp(-x*x*0.5/6.27/6.27)",-4,4);
}
else{
  etadep = new TF1("etadep","[0]",-4,4);
}
TF1 *func1  = new TF1("func1" ,get_vn,0,25,4);


TCanvas *C2=new TCanvas("C2","C2",1600,1200);
C2->Divide(3,3);
for(int ipt=1;ipt<9;ipt++){
  C2->cd(ipt);

  sprintf(name,"profile_eta_dep_2_pt%d",ipt);
  prof1=(TProfile*)infile->Get(name); 
  prof1->SetLineColor(2);  
  prof1->SetTitle("Using Truth #Psi");
  prof1->GetXaxis()->SetTitle("#eta");
  prof1->SetStats(0);
  prof1->Fit("etadep","QR");
  prof1->SetMarkerStyle(20);
  prof1->SetMarkerSize(.5);
  prof1->SetMarkerColor(2);
  prof1->SetMaximum (prof1->GetMaximum()*1.2);
  if(prof1->GetMinimum(0)>0) prof1->SetMinimum(0);
  prof1->Draw();

  sprintf(name,"profile_eta_dep_reco_2_pt%d",ipt);
  prof2=(TProfile*)infile->Get(name); 
  prof2->SetLineColor(4);  
  prof2->SetTitle("Using Reco #Psi");
  prof2->SetMarkerStyle(20);
  prof2->SetMarkerSize(.5);
  prof2->SetMarkerColor(4);
  if(implementation_type<7) prof2->Draw("same"); //Dont Draw for p-Pb as 2subevent resolution doesnto work


  sprintf(name,"(%.2f,%.2f) GeV",pt_binvals[ipt],pt_binvals[ipt+1]);
  Tex.SetNDC();
  Tex.DrawLatex(0.4,0.4,name);

  if(ipt==8){
    C2->cd(9);
    Leg=new TLegend(.3,.3,.9,.9);
    Leg->AddEntry(prof1,"Using Truth  #Psi");
    if(implementation_type<7) Leg->AddEntry(prof2,"Using Reco  #Psi"); //Dont Draw for p-Pb as 2subevent resolution doesnto work
    //Leg->AddEntry(etadep,"expected");
    Leg->AddEntry(etadep,"Fit to constant");
    Leg->Draw(); 
  }
 

}
C2->SaveAs("~/web/etadep_v2.gif");
C2->Write();


for(int ihar=2;ihar<=6;ihar++){
  sprintf(name,"harmonic%d",ihar);
  C2=new TCanvas(name,name,1600,1200);
  C2->Divide(3,3);
  for(int ieta=0;ieta<8;ieta++){
    C2->cd(ieta+1);

    sprintf(name,"profile_pt_dep_%d_eta%d",ihar,ieta);
    prof1=(TProfile*)infile->Get(name);
    if(!prof1) {cout<<name<<" not found"<<endl;continue;}
    prof1->SetLineColor(2);
    prof1->SetTitle("Using Truth #Psi");
    prof1->GetXaxis()->SetTitle("p_{T} (GeV)");
    prof1->GetXaxis()->SetRangeUser(0,18);
    prof1->SetStats(0);
    prof1->SetMarkerStyle(20);
    prof1->SetMarkerSize(.5);
    prof1->SetMarkerColor(2);
    prof1->Draw();

    sprintf(name,"profile_pt_dep_reco_%d_eta%d",ihar,ieta);
    prof2=(TProfile*)infile->Get(name);
    if(!prof2) {cout<<name<<" not found"<<endl;continue;}
    prof2->SetLineColor(4);  
    prof2->SetTitle("Using Reco #Psi");
    prof2->SetMarkerStyle(20);
    prof2->SetMarkerSize(.5);
    prof2->SetMarkerColor(4);
    if(implementation_type<7) prof2->Draw("same"); //Dont Draw for p-Pb as 2subevent resolution doesnto work

    func1->SetParameter(0,ieta*0.5 +0.25);
    func1->SetParameter(1,7);
    func1->SetParameter(2,ihar);
    func1->SetParameter(3,implementation_type);
    func1->DrawClone("same");
 
     

    Leg=new TLegend(.7,.7,.98,1);
    Leg->AddEntry(prof1,"Using Truth  #Psi");
    if(implementation_type<7) Leg->AddEntry(prof2,"Using Reco  #Psi");//Dont Draw for p-Pb
    Leg->AddEntry(func1,"expected");
    //Leg->Draw();

    sprintf(name,"|eta| #in (%.1f,%.1f)",ieta*0.5,ieta*0.5+0.5);
    Tex.DrawLatex(0.5,0.7,name);

    if(ieta<5){
      if(ieta==0) {prof3= (TProfile*) prof1->Clone();}//combined eta bin (-2.5-2.5)
      else        {prof3->Add(prof1);}

      if(ieta==0) {prof4= (TProfile*) prof2->Clone();}//combined eta bin (-2.5,2.5)
      else        {prof4->Add(prof2);}}
  }

  C2->cd(9);

      prof3->Draw(); 
      if(implementation_type<7) prof4->Draw("same");  //Dont Draw for p-Pb as 2subevent resolution doesnto work
      func1->SetParameter(0,1.25);
      func1->SetParameter(1,7);
      func1->SetParameter(2,ihar);
      func1->SetParameter(3,implementation_type);
      func1->DrawClone("same");

      Leg=new TLegend(.7,.7,.98,1);
      Leg->AddEntry(prof3,"Using Truth  #Psi");
      if(implementation_type<7) Leg->AddEntry(prof4,"Using Reco  #Psi");//Dont Draw for p-Pb
      Leg->AddEntry(func1,"expected");
      Leg->Draw();

      TLatex *Tex1=new TLatex();
      double low=0,high=2.5;
      sprintf(name,"eta(%.1f,%.1f)",low,high);
      Tex1->DrawTextNDC(0.5,0.7,name);

  sprintf(name,"~/web/v%d_pt.gif",ihar);
  C2->SaveAs(name);
  C2->Write();


}
}













Double_t get_vn(Double_t *x, Double_t *par)
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
  if(implementation_type==7) { //p_pb : b_imp and eta independent
    pt=pt/1000.0; //convert to GeV
    float an_val[4][3];
    an_val[0][0] = 0.1149;
    an_val[0][1] = 1.181;
    an_val[0][2] = 0.3767;

    an_val[1][0] = 0.0498;
    an_val[1][1] = 1.688;
    an_val[1][2] = 0.5046;

    an_val[2][0] = 0.02095;
    an_val[2][1] = 2.196;
    an_val[2][2] = 0.6259;

    an_val[3][0] = 0.00682*0.5;//added in 0.5 by hand
    an_val[3][1] = 4.938;
    an_val[3][2] = 1.237;

    if(ihar <2 || ihar>=6) return 0;
    return an_val[ihar-2][0]*pow(pt,an_val[ihar-2][1])*exp(-an_val[ihar-2][2]*pt);
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
    float fb=0.97 +1.06*std::exp(-0.5*b*b/3.2/3.2);
    return std::pow(fb*std::sqrt(v2),3);
  }
  float gb= 1.096 +1.36 *std::exp(-0.5*b*b/3.0/3.0);
  gb=gb*std::sqrt(v2);
  return std::pow(gb,ihar);
}






void reso(){

const int n_ptbin=16;
const int n_etabin=8;

char name[100];
TProfile *prof;
TFile *Infile =new TFile("all0.root");
TFile *Outfile=new TFile("all.root" ,"recreate");
Outfile->cd();

TProfile *profile_resolution= (TProfile*)Infile->Get("profile_resolution");
profile_resolution->Write();
  
  for(int ihar=0;ihar<6;ihar++){
    double reso=profile_resolution->GetBinContent(ihar+1);
    if (reso >=0) reso= std::sqrt( reso);
    else          reso=-std::sqrt(-reso);
    for(int ieta=0;ieta<n_etabin;ieta++){
      sprintf(name,"profile_pt_dep_reco_%d_eta%d",ihar+1,ieta);
      prof=(TProfile*)Infile->Get(name);
      prof->Scale(1.0/reso);
      prof->Write();

      sprintf(name,"profile_pt_dep_%d_eta%d",ihar+1,ieta);
      prof=(TProfile*)Infile->Get(name);
      prof->Write();
    }
    for(int ipt=0;ipt<n_ptbin;ipt++){
      sprintf(name,"profile_eta_dep_reco_%d_pt%d",ihar+1,ipt);
      prof=(TProfile*)Infile->Get(name);
      prof->Scale(1.0/reso);
      prof->Write();

      sprintf(name,"profile_eta_dep_%d_pt%d",ihar+1,ipt);
      prof=(TProfile*)Infile->Get(name);
      prof->Write();
    }
  }
  Outfile->Close(); 
  Infile->Close(); 
}

 

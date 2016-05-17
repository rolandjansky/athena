#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"

const int NFLOW=2;
const int NHAR=6;
const int NACC=2;

TFile *File[2];
TH1D* h_Vn[NFLOW][NHAR][NACC];
char name[1000];


#include <TLatex.h>
void myText2(float x, float y, Color_t color, string text,int size, int font,float angle=0){
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextColor(color);
  l.SetTextSize(size);
  l.SetTextFont(font);
  l.SetTextAngle(angle);
  l.DrawLatex(x,y,text.c_str());
}




void EbEPlots(){
  File[0] = new TFile("CheckFlow.hist.root");
  File[1] = new TFile("CheckFlow.EbE.hist.root");


  int col[]={kOrange-3,kAzure+10};
  double range[][NACC]={ {0.2,0.2},
                         {0.15,0.09}, 
                         {0.08,0.05}, 
                         {0.05,0.03}, {0.2,0.2},{0.2,0.2} };
  for(int ifile=0;ifile<NFLOW;ifile++){
    for(int ihar=0;ihar<NHAR;ihar++){
      for(int iacc=0;iacc<NACC;iacc++){
         sprintf(name,"hist_v%d_b%d_ebe_ID%d",ihar+1,2,iacc+1);
         h_Vn[ifile][ihar][iacc]=(TH1D*)File[ifile]->Get(name);
         TH1D* hist=h_Vn[ifile][ihar][iacc];
         hist->Rebin(5);
         hist->SetLineColor(col[ifile]);

         if(iacc==0) sprintf(name,"v_{%d} (p_{T}>0.5 GeV)",ihar+1);
         else        sprintf(name,"v_{%d} (p_{T}>0.0 GeV)",ihar+1);
         hist->GetXaxis()->SetTitle(name);
         hist->GetYaxis()->SetTitle("Events");
         hist->GetXaxis()->SetTitleSize(0.07);
         hist->GetYaxis()->SetTitleSize(0.07);
         hist->GetXaxis()->SetRangeUser(0,range[ihar][iacc]);
         hist->SetLineWidth(2);
      }
    }
  }

  for(int ihar=0;ihar<NHAR;ihar++){
    if(ihar>3 || ihar==0) continue;
    sprintf(name,"Can_v%d",ihar+1);
    TCanvas *C1=new TCanvas(name,name,1200,450);C1->Divide(2);
    for(int iacc=0;iacc<NACC;iacc++){
      C1->cd(iacc+1);
      for(int ifile=0;ifile<NFLOW;ifile++){
         TH1D* hist=h_Vn[ifile][ihar][iacc];
         if(ifile==0) {hist->Draw();}
         else         {hist->Draw("same");}
      }
    }
    C1->cd(1);
    float X1=0.60,Y1=0.90;
    myText2(X1     ,Y1    ,1,"ATLAS"   ,18,73);
    myText2(X1+0.15,Y1    ,1,"Internal",18,43);
    myText2(X1     ,Y1-.05,1,"Pb+Pb #sqrt{s_{NN}}=2.76 TeV",18,43);
    myText2(X1     ,Y1-.10,1,"Simulation (MC Truth)",18,43);

    sprintf(name,"Can_v%d.png",ihar+1);
    C1->SaveAs(name);
  }

}

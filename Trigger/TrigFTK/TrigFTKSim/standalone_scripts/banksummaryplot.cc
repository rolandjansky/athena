/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TFile.h>
#include <TH2.h>
#include <TH1D.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TText.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TLine.h>
#include <TGaxis.h>

#include <iostream>
#include <fstream>
#include <map>

#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

//TString g_scratch("/sps/atlas/l/lderamo/FTK_Framework");
TString g_scratch("/nfs/dust/atlas/user/sschmitt");
TString g_ftkip("ftksim");
TString g_dc("dc");

void rootlogon(void) {
  TStyle *square=new TStyle("square","square");
  gROOT->SetStyle("Plain");
  int font=42;
  gStyle->SetLabelFont(font,"XYZ");
  gStyle->SetLegendFont(font);
  gStyle->SetStatFont(font);
  gStyle->SetTextFont(font);
  gStyle->SetTitleFont(font,"XYZ");
  gStyle->Copy(*square);
  gROOT->SetStyle("square");
  
  gStyle->SetPadBorderSize(0.0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.28);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetHistLineWidth(2);
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(0.8);
  gStyle->SetTitleOffset(1.6,"y");
  gStyle->SetTitleOffset(1.2,"x");
  gStyle->SetTitleSize(0.8,"P");
  gStyle->SetTitleSize(0.075,"xy");
  gStyle->SetLabelSize(0.075,"xy");
  gStyle->SetLabelOffset(0.012,"xy");
  gStyle->SetNdivisions(505,"xy");
  double titleBorder=0.005;
  gStyle->SetTitleX(gStyle->GetPadLeftMargin());
  gStyle->SetTitleY(1.0-titleBorder);
  gStyle->SetTitleBorderSize(0.0);
  gStyle->SetTitleW(1.0-gStyle->GetPadLeftMargin()
                    -gStyle->GetPadRightMargin());
  gStyle->SetTitleH(gStyle->GetPadTopMargin()-2.*titleBorder);
  gStyle->SetGridColor(kGray);
}

int main(int argc, char const *argv[]) {
   rootlogon();
   for(int ibank=1;ibank<argc;ibank++) {
     glob_t globList;
     struct stat statBuf;
     memset(&globList,0,sizeof(glob_t));
     TString ftksimpath=g_scratch+"/"+g_ftkip+"/"+argv[ibank];
     int err=glob(ftksimpath+"*",GLOB_TILDE,0,&globList);
     if(err) {
       cout<<"===============================================================\n";
       cout<<" Error: can not stat "<<ftksimpath<<"\n";
       cout<<" Make sure that the path to your dc & ftksim files is well defined in Trigger/TrigFTK/TrigFTKSim/standalone_scripts/banksummaryplots.cc"<< "\n";
       cout<<"===============================================================\n";
       continue;
     }
     cout<<"===============================================================\n";
     cout<<"processing: "<<argv[ibank]<<"\n";
     map<int,TString> ftksimByMu;
     for(int ipath=0;ipath<globList.gl_pathc;ipath++) {
         if(stat(globList.gl_pathv[ipath],&statBuf)) {
            cout<<" Error: can not stat "<<globList.gl_pathv[ipath]<<"\n";
	    continue;
         }
         if(!S_ISDIR(statBuf.st_mode)) {
            cout<<" Not a directory, skipping: "
                <<globList.gl_pathv[ipath]<<"\n";
         }
         TString ftkip(globList.gl_pathv[ipath]);
         ftkip.ReplaceAll(ftksimpath+"_","");
         int mu=-1;
         if(!ftkip.BeginsWith("single")) {
            int i0=ftkip.Index("_mu")+3;
            int i1=ftkip.Index("_",1,i0,TString:: kExact);
            mu=TString(ftkip(i0,i1)).Atoi();
         }
         ftksimByMu[mu]=globList.gl_pathv[ipath];
      }
      globfree(&globList);
      if(!ftksimByMu.size()) {
         cout<<" Error: no ftksim file found\n";
         continue;
      }
      TString outputdir=g_scratch+"/dc/"+argv[ibank];
      TString bankpath=outputdir+"/*_reg*_sub0.pbank.root";
      map<int,TString> bankfiles;
      err=glob(bankpath,GLOB_TILDE,0,&globList);
      if(!err) {
         for(int ipath=0;ipath<globList.gl_pathc;ipath++) {
            if(stat(globList.gl_pathv[ipath],&statBuf)) {
               cout<<" Error: can not stat "<<globList.gl_pathv[ipath]<<"\n";
               continue;
            }
            if(!S_ISREG(statBuf.st_mode)) {
               cout<<" Not a regular file, skipping: "
                   <<globList.gl_pathv[ipath]<<"\n";
            }
            int reg=-1;
            char const *s=strstr(globList.gl_pathv[ipath],"_sub0.pbank.root");
            s=strstr(s-10,"_reg");
            if(s && sscanf(s,"_reg%d",&reg)==1) {
               bankfiles[reg]=globList.gl_pathv[ipath];
            }
         }
         globfree(&globList);
      }
      if(stat(outputdir,&statBuf)) {
         cout<<" DC bank directory does not exist, skipping: "<<outputdir<<"\n";
         continue;
      }
      if(!S_ISDIR(statBuf.st_mode)) {
         cout<<" not a directory, skipping: "<<outputdir<<"\n";
         continue;
      }
      TString rootFileName(outputdir+"/dataflow_and_efficiency.root");
      cout<<" writing output to root file: "<<rootFileName<<"\n";
      TFile *outputRoot= new TFile(rootFileName,"RECREATE");
      map<TString,TGraph *> graphs;
      vector<int> nPatternsTotal;
      vector<map<int,int> > dcConfig;
      for(auto ftksim=ftksimByMu.begin();ftksim!=ftksimByMu.end();ftksim++) {
         int mu=(*ftksim).first;
	 if(mu<0){
	   for(int i=0;i<2;i++) {
	     TString effFileName((*ftksim).second+"/"
				 +TString::Format("efficiency%d.root",i+1));
	     cout<<" reading (mu="<<mu<<"): "<<effFileName<<"\n";
	     TFile efficiencyRoot(effFileName);
	     outputRoot->cd();
	     TGraph *&effEta(graphs[TString::Format("efficiency%d_%d",i+1,mu)]);
	     efficiencyRoot.GetObject("greffeta",effEta);
	     if(!effEta) {
               cout<<"  error: efficiency graph ("
                   <<(i+1)<<".stage) not found\n";
	     }
	   }
	 }
         if(mu>=0) {
            TString dataflowFile((*ftksim).second+"/dataflow/output.root");
            cout<<" reading (mu="<<mu<<"): "<<dataflowFile<<"\n";
            TFile dataflowRoot(dataflowFile);
            outputRoot->cd();
            map<TString,TH2 *> histDataflow;
            dataflowRoot.GetObject("hist_dataflow_avg",histDataflow["avg"]);
            dataflowRoot.GetObject("hist_dataflow_min",histDataflow["min"]);
            dataflowRoot.GetObject("hist_dataflow_max",histDataflow["max"]);
	    dataflowRoot.GetObject("hist_dataflow_first_decile",histDataflow["first_decile"]);  
	    dataflowRoot.GetObject("hist_dataflow_last_decile",histDataflow["last_decile"]);
	
            int ntower;
	    
            for(auto obs=histDataflow.begin();obs!=histDataflow.end();obs++) {
               if(!(*obs).second) continue;
               for(int ivar=1;ivar<=(*obs).second->GetNbinsY();ivar++) {
                  TString label=(*obs).second->GetYaxis()->GetBinLabel(ivar);
                  if(!label.Length()) continue;
                  for(int endcap=0;endcap<2;endcap++) {
                     TString graphName
                        (label+"_"+(*obs).first+TString::Format("_%d",endcap));
                     TGraph *&ga(graphs[graphName+"_avg"]);
                     TGraph *&g1(graphs[graphName+"_min"]);
                     TGraph *&g2(graphs[graphName+"_max"]);
		     vector<double > x,dx,ya,dya,y1,dy1,y2,dy2;
                     if(ga) {
		       for(int i=0;i<ga->GetN();i++) {
                           x.push_back(ga->GetX()[i]);
                           dx.push_back(ga->GetErrorX(i));
                           ya.push_back(ga->GetY()[i]);
                           dya.push_back(ga->GetErrorY(i));
			   y1.push_back(g1->GetY()[i]);
                           dy1.push_back(g1->GetErrorY(i));
                           y2.push_back(g2->GetY()[i]);
                           dy2.push_back(g2->GetErrorY(i));		  
                        }
                        delete ga;
                        delete g1;
                        delete g2;			
                     }
                     x.push_back(mu);
                     dx.push_back(0.);
                     
                     ntower=(*obs).second->GetNbinsX();
                     double towerWeight=2./ntower;
		     double mean[3],rms[3],rms_old, rms_per_tow, rms_per_evt, mean_per_tow_sq, rms_evt;
		     rms_old = 0;
		     rms_per_tow = 0;
		     rms_per_evt = 0;
		     mean_per_tow_sq = 0;
		     rms_evt = 0;   
                     for(int i=0;i<3;i++) {		        
		       rms[i]=0.;
		        mean[i]=0.;
                     }		     
		     if((*obs).first != "first_decile" && (*obs).first != "last_decile") {
                     for(int it=0;it<ntower;it++) {		       
                        int towerIsEndcap=(it<ntower/4)||(it>=ntower*3/4);
                        if(endcap ^ towerIsEndcap)  continue;
                        double nevent=(*obs).second->GetBinContent(it+1,1);
                        double sum_w=(*obs).second->GetBinContent(it+1,ivar);
                        double sum_wsq_old=(*obs).second->GetBinError(it+1,ivar);
			double sum_wsq_per_evt=(*obs).second->GetBinError(it+1,ivar);
			double sum_wsq_per_tow=(*obs).second->GetBinContent(it+1,ivar);		
			sum_wsq_old *= sum_wsq_old;	
			  sum_wsq_old/=nevent; // (fi^2/nevt)
			sum_wsq_per_tow/=nevent;			
                        sum_wsq_per_tow *= sum_wsq_per_tow; // (fi/nevt)^2
			sum_wsq_per_evt *= sum_wsq_per_evt;
			sum_wsq_per_evt/=nevent; // (fi^2/nevt)
                        sum_w/=nevent;
                        if((!mean[1])||(sum_w<mean[1])) {
                           mean[1]=sum_w;
                           rms[1]=(sum_wsq_old-sum_w*sum_w);
                           if(rms[1]>0.) rms[1]=TMath::Sqrt(rms[1]);
                           else rms[1]=0.0;
                        }
                        if(sum_w>mean[2]) {
                           mean[2]=sum_w;
                           rms[2]=(sum_wsq_old-sum_w*sum_w);
                           if(rms[2]>0.) rms[2]=TMath::Sqrt(rms[2]);
                           else rms[2]=0.0;
                        }
			double oldmean=mean[0];
                        mean[0]+= sum_w*towerWeight;
			mean_per_tow_sq += sum_wsq_per_tow*towerWeight;
			rms_evt += sum_wsq_per_evt - sum_w*sum_w;
                        rms_old=TMath::Sqrt
			  (rms_old*rms_old+oldmean*oldmean // old sum(weight^2)
			   +sum_wsq_old*towerWeight // new sum(weight^2)
			   -mean[0]*mean[0]); // rew rms
		       
		       }
		     rms_per_tow = TMath::Sqrt(mean_per_tow_sq - mean[0]*mean[0]);
		     rms_per_evt = TMath::Sqrt(rms_evt*towerWeight);
		     rms[0]=rms_per_tow;
                     ya.push_back(mean[0]);
                     dya.push_back(rms[0]);
                     y1.push_back(mean[1]);
                     dy1.push_back(rms[1]);
                     y2.push_back(mean[2]);
                     dy2.push_back(rms[2]);
		     }
		     else{
		       for(int it=0;it<ntower;it++) {		       
                        int towerIsEndcap=(it<ntower/4)||(it>=ntower*3/4);
                        if(endcap ^ towerIsEndcap)  continue;
			double sum_w=(*obs).second->GetBinContent(it+1,ivar);
			double sum_wsq=(*obs).second->GetBinError(it+1,ivar);
			sum_wsq *= sum_wsq;
			 if((!mean[1])||(sum_w<mean[1])) {
                           mean[1]=sum_w;
                           rms[1]=(sum_wsq-sum_w*sum_w);
                           if(rms[1]>0.) rms[1]=TMath::Sqrt(rms[1]);
                           else rms[1]=0.0;
                        }
                        if(sum_w>mean[2]) {
                           mean[2]=sum_w;
                           rms[2]=(sum_wsq-sum_w*sum_w);
                           if(rms[2]>0.) rms[2]=TMath::Sqrt(rms[2]);
                           else rms[2]=0.0;
                        }
			mean_per_tow_sq += sum_wsq*towerWeight;
                        mean[0]+= sum_w*towerWeight;
		       }
		       rms_per_tow = TMath::Sqrt(mean_per_tow_sq - mean[0]*mean[0]);
		       rms[0] = rms_per_tow;
		       ya.push_back(mean[0]);
		       dya.push_back(rms[0]);
		       y1.push_back(mean[1]);
		       dy1.push_back(rms[1]);
		       y2.push_back(mean[2]);
		       dy2.push_back(rms[2]);
		     }
                     ga=new TGraphErrors(x.size(),x.data(),ya.data(),
                                         dx.data(),dya.data());
                     g1=new TGraphErrors(x.size(),x.data(),y1.data(),
                                         dx.data(),dy1.data());
                     g2=new TGraphErrors(x.size(),x.data(),y2.data(),
                                         dx.data(),dy2.data());
                  }
               }
            }
            // extract number of patterns, dc bits, etc
            if(!dcConfig.size()) {
               dcConfig.resize(ntower);
               nPatternsTotal.resize(ntower);
               for(int itower=0;itower<ntower;itower++) {
                  TString logfile=(*ftksim).second+
                     TString::Format("/log%d/log.FTKFullSimulation",itower);
                  TString dcConfigFile(TString::Format("dcconfig%d.txt",itower));
                  unlink(dcConfigFile);
                  TString command("awk '($2==\"TSP/DC\")&&($4==\"layer\")"
                                  "{if($8==\"bits\") nbit=$9; else nbit=$10;"
                                  "print \"DC\",substr($5,1,1),substr(nbit,1,1)}"
                                  "($5==\"number\")&&($7==\"distinct\")"
                                  "{gsub(\"=\",\" \",$14);print $14;gsub(\"=\",\" \",$15);print $15}' >"
                                  +dcConfigFile+" <"+logfile);
                  system(command);
                  ifstream configFile(dcConfigFile);
                  while(!configFile.eof()) {
                     string tag;
                     configFile>>tag;
                     if(tag=="DC") {
                        int plane,ndc;
                        configFile>>plane>>ndc;
                        dcConfig[itower][plane]=ndc;
                     } else if(tag=="patterns") {
                        configFile>>nPatternsTotal[itower];
                     } else {
                        //break;
                     }
                  }
                  unlink(dcConfigFile);
               }
            }
         }
      }
      outputRoot->cd();
      for(auto ig=graphs.begin();ig!=graphs.end();ig++) {
         if(!(*ig).second) continue;
         (*ig).second->SetName((*ig).first);
         (*ig).second->Write();
      }      
      // configuration string
      int nPatternMax=0;
      for(int itower=0;itower<nPatternsTotal.size();itower++) {
         if(nPatternsTotal[itower]>nPatternMax)
            nPatternMax=nPatternsTotal[itower];
      }
      int nPU=(nPatternMax+8*1024*1024-1)/(8*1024*1024)*nPatternsTotal.size();
      TString infoString=TString::Format
         ("nTower=%d    nPU=%d    DC=[",nPatternsTotal.size(),nPU);
      for(auto idc=dcConfig[0].begin();idc!=dcConfig[0].end();idc++) {
         infoString += TString::Format("%d",(*idc).second);
      }
      infoString+="]";
      int nb=-1,ne=-1;
      char const *s=strstr(argv[1],"-NB");
      if(s && sscanf(s,"-NB%d-NE%d_",&nb,&ne)==2) {
         infoString+=TString::Format(":Nb<%d:Ne<%d",nb+1,ne+1);
      }
      {
         TString summaryName=outputdir+"/dataflow_and_efficiency.summary.eps";
         int h_title=50;
         int h=300;
         
         TCanvas summary(argv[1],infoString,900,h+h_title);
         summary.Divide(3,1); //,0.0,0,0);
         TText *text=new TText();
         text->SetTextAlign(22);
         text->SetTextSize(0.8*h_title/(h+h_title));
         text->DrawText(0.5,
                        (h+0.5*h_title)/(h+h_title),infoString);  
	 TText *barrel=new TText();
	 barrel->SetTextAlign(22);
	 barrel->SetTextSize(0.45*h_title/(h+h_title));
	 barrel->SetTextColor(kBlue);
	 barrel->DrawText(0.185,(h+0.5*h_title)/(h+h_title)-0.8*h_title/(h+h_title),"Barrel");
	 barrel->DrawText(0.52,(h+0.5*h_title)/(h+h_title)-0.8*h_title/(h+h_title),"Barrel");
	 TText *slash=new TText();
	 slash->SetTextAlign(22);
	 slash->SetTextSize(0.45*h_title/(h+h_title));
	 slash->SetTextColor(kBlack);
	 slash->DrawText(0.22,(h+0.5*h_title)/(h+h_title)-0.8*h_title/(h+h_title),"/");
	 slash->DrawText(0.555,(h+0.5*h_title)/(h+h_title)-0.8*h_title/(h+h_title),"/");
	 TText *endcap=new TText();
	 endcap->SetTextAlign(22);
	 endcap->SetTextSize(0.45*h_title/(h+h_title));
	 endcap->SetTextColor(kRed);
	 endcap->DrawText(0.27,(h+0.5*h_title)/(h+h_title)-0.8*h_title/(h+h_title),"End-Cap");
	 endcap->DrawText(0.605,(h+0.5*h_title)/(h+h_title)-0.8*h_title/(h+h_title),"End-Cap");
         for(int ipad=1;ipad<=3;ipad++) {
            summary.GetPad(ipad)
               ->SetPad((ipad-1.)/3.,0.,ipad/3.,h/(double)(h+h_title));
         }
         static char const *dfPlotName[]={"nRoad","nFitI"};
	 int nroad_lim = 6000;
	 int nfit_lim = 30000;
	 if(nPU==64){
	    nroad_lim = 6000;
	    nfit_lim = 30000;}
	 if(nPU==128){
	    nroad_lim = 8000;
	    nfit_lim = 60000;}
	 static int const dfPlotLimit[]={nroad_lim,nfit_lim};
	 for(int dfplot=0;dfplot<2;dfplot++) {
            summary.cd(dfplot+1);
            bool first=true;
            TLegend *legend=new TLegend(0.3,0.65,0.8,0.9);
            legend->SetBorderSize(0);
            legend->SetFillStyle(0);
	    double xmin=0.,xmax=1.;
            double ymax=1.;
            for(int ec=0;ec<2;ec++) {
               TString type="barrel";
               if(ec) type="endcap";
	       TGraph *max=graphs[dfPlotName[dfplot]+
				  TString::Format("_avg_%d_max",ec)];
               if(max) {
		 xmin=max->GetX()[0];
		 xmax=max->GetX()[max->GetN()-1];
		 double dx=0.05*(xmax-xmin);
		 xmin -= dx;
		 xmax += dx;
		 for(int i=0;i<max->GetN();i++) {
		   double y=max->GetY()[i];
		   if(y>ymax) ymax=y;
		 }
	       }
            }
            TH1D *tmp=new TH1D
               (dfPlotName[dfplot],";#mu;"+
                TString(dfPlotName[dfplot])+" / tower",1,xmin,xmax);
	    TGaxis::SetMaxDigits(3);
            tmp->Draw("");
	    TObject *deciles[2];
	    	    
            for(int ec=0;ec<2;ec++) {
               TString type="barrel";
               if(ec) type="endcap";
               TGraph *avg=graphs[dfPlotName[dfplot]+
				  TString::Format("_avg_%d_avg",ec)];
	       TGraph *max=graphs[dfPlotName[dfplot]+
				  TString::Format("_avg_%d_max",ec)];
	       TGraph *first_decil=graphs[dfPlotName[dfplot]+
					  TString::Format("_first_decile_%d_avg",ec)];
	       TGraph *last_decil=graphs[dfPlotName[dfplot]+
					 TString::Format("_last_decile_%d_avg",ec)];
               int color=ec ? kRed : kBlue;
               if(avg && first_decil && last_decil && max) {
		 int NBin_last=avg->GetN()-1;
		 int NBin = NBin_last+1;
		 Double_t* x_avg = avg->GetX();
		 Double_t* y_avg = avg->GetY();
		 Double_t* y_first_decile = first_decil->GetY();
		 Double_t* y_last_decile = last_decil->GetY();
		 Double_t y_err_low[NBin], y_err_high[NBin];
		 for(int i=0; i<NBin; i++){
		   y_err_low[i] = y_avg[i]-y_first_decile[i];
		   y_err_high[i] = y_last_decile[i]-y_avg[i];
		 }
		 TGraphAsymmErrors *graph = new TGraphAsymmErrors(NBin,x_avg,y_avg,0,0, y_err_low,y_err_high);
		 graph->SetFillStyle(ec ? 3935 : 3953);
		 graph->SetFillColor(color-10);
		 graph->SetLineWidth(0);
		 graph->SetMarkerStyle(0);
		 deciles[ec]=graph->DrawClone("SAME 3");
		 
		 double x_pos,y_pos;
		 last_decil->GetPoint(NBin_last,x_pos,y_pos);
		 if(y_pos > ymax) ymax = y_pos;
		 if(dfPlotLimit[dfplot]>ymax) ymax = dfPlotLimit[dfplot];
		 tmp->GetYaxis()->SetRangeUser(0.,1.1*ymax);
               } else {
                  cout<<"graph not found\n";
               }
            }

	    
	    for(int ec=0;ec<2;ec++) {
	      TGraph *avg=graphs[dfPlotName[dfplot]+
				 TString::Format("_avg_%d_avg",ec)];
	      TGraph *max=graphs[dfPlotName[dfplot]+
				 TString::Format("_avg_%d_max",ec)];
	      int color=ec ? kRed : kBlue;
	      if(avg && max) {
		avg->SetLineColor(color);
		avg->SetFillStyle(0);
		avg->Draw("SAME XL");

		max->SetLineColor(color);
		max->SetLineStyle(2);
		max->SetFillStyle(0);
		max->Draw("SAME XL");
		}
	    }
	    for(int ec=0;ec<1;ec++) {
	      TGraph *avg_1= new TGraph();
	      TGraph *max_1= new TGraph();
	      TGraph *rms_1= new TGraph();
	      
	      rms_1->SetFillStyle(3935);
	      rms_1->SetLineWidth(0);
	      rms_1->SetMarkerStyle(0);
	      rms_1->SetFillColor(kMagenta+2);
	      
	      avg_1->SetLineColor(kMagenta+2);
	      avg_1->SetFillStyle(0);
		 
	      max_1->SetLineColor(kMagenta+2);
	      max_1->SetLineStyle(2);
	      max_1->SetFillStyle(0);
		 	      
	      legend->AddEntry(avg_1," avg ","l");
	      legend->AddEntry(max_1," avg max over tow. ","l");
	      legend->AddEntry(rms_1," 10/90 % percentile ","f");
	    }
	    
            if(dfPlotLimit[dfplot]) {
               TLine *limit=new TLine(xmin,dfPlotLimit[dfplot],xmax,dfPlotLimit[dfplot]);
               limit->SetLineStyle(3);
               limit->Draw();
               legend->AddEntry(limit,"hardware limit","l");
            }
	    legend->Draw();
	 }
	 summary.cd(3);
         TGraph *eff[2];
         eff[0]=graphs["efficiency1_-1"];
         eff[1]=graphs["efficiency2_-1"];
         int effAvg[2][4];
         for(int i=0;i<2;i++) {
            for(int j=0;j<4;j++) {
               effAvg[i][j]=0.;
               if(eff[i]) {
                  double sumw=0.;
                  double sume=0.;
                  for(int ig=0;ig<eff[i]->GetN();ig++) {
                     double x=eff[i]->GetX()[ig];
		     bool isBarrel= (x>=-1.1)&&(x<=1.1);
                     bool isEndcap= (x<=-1.6)||(x>=1.6);
                     if((j==0) && !isBarrel) continue; // x not barrel
                     if((j==1) && (isBarrel||isEndcap)) continue; // x is barrel of endcap
                     if((j==2) && !isEndcap) continue; // x not endcap   
                     double w=eff[i]->GetErrorY(ig);
                     if(w>0.0) w=1./w/w;		     
		     sumw+= w;
                     sume += w*eff[i]->GetY()[ig];
                  }
                  if(sume>0.) {
                     effAvg[i][j]=(int)((sume*100.+0.5)/sumw);
                  }
               }
            }
         }
         if(eff[0]) {
            eff[0]->SetTitle(";#eta;efficiency");
            eff[0]->SetLineColor(kBlack);
            eff[0]->SetMarkerColor(kBlack);
            eff[0]->GetYaxis()->SetRangeUser(0.2,1.0);
            eff[0]->Draw("");
            if(eff[1]) {
               eff[1]->SetLineColor(kGreen+2);
               eff[1]->SetMarkerColor(kGreen+2);
               eff[1]->Draw("SAME");
            }
            TLegend *legend=new TLegend(0.3,0.25,0.6,0.55,"single mu efficiency");
            legend->SetBorderSize(0);
            legend->SetFillStyle(0);
            legend->SetTextSize(0.05);
            for(int j=0;j<2;j++) {
               if(!eff[j]) continue;
               legend->AddEntry
                  (eff[j],TString::Format("%d.stage avg=%.2d%%",
                                          j+1,effAvg[j][3]),"lp");
               legend->AddEntry
                  ((TObject *)0,TString::Format("barr.=%.2d%% trans=%.2d%% ec=%.2d%%",
                                                effAvg[j][0],effAvg[j][1],effAvg[j][2]),"");
            }
            legend->Draw();
         }
         summary.SaveAs(summaryName);
      }
      delete outputRoot;
   }
   return 0;
}


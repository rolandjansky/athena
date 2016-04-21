#include <TCanvas.h>
#include <TBox.h>
#include <TH2F.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>


void plotRPClayerOld(Int_t inputLayer)
{
  TCanvas* cLayer = new TCanvas("","",900,700);
  //return;
  //Double_t M_PI=acos(-1.);
  Double_t hphimin = -TMath::Pi()-TMath::Pi()/10.;
  Double_t hphimax =  TMath::Pi()+TMath::Pi()/10.;
  TH2F* hLayer = new TH2F("hLayer","hLayer",100,-1.07,1.07,100,hphimin,hphimax);
  hLayer->Draw();
  TBox * t = new TBox();
  //t->SetLineStyle(1);
  t->SetLineWidth(2);
  t->SetLineColor(kBlack);
  //t->SetFillColor(kWhite);
  t->SetFillStyle(0);
    
  std::string STRING;
  stringstream ss;
  ss<<inputLayer;
  std::string laySTRING=ss.str();
  std::cout<<" Will use only lines with Layer = "<<laySTRING<<std::endl;
  ss.clear();
  ifstream infile("/afs/le.infn.it/user/s/spagnolo/atlas/rpcPerf/code/MuonSpectrometer/MuonGeoModelTest/share/rpc_current_panelID_R.07.01");
  int nPanels= 0;
  float etamin,etamax,phimin,phimax,zmin,zmax;
  std::string stName;
  std::string viewName;
  std::string stringId;
  int layerRPC,msIndex,stNameIndex,stEtaIndex,stPhiIndex,techIndex,dbrIndex,dbzIndex,dbpIndex,ggIndex,measphiIndex,stripIndex;
  unsigned long panelID;
  if (infile.is_open())                     //if the file is open
    {
	while(!infile.eof()) // To get you all the lines.
	{
	    ss.clear();
	    getline(infile,STRING); // Saves the line in STRING.
	    //std::cout<<" now reading <"<<STRING<<">"<<std::endl; // Prints our STRING.
	    if (STRING.substr(0,1)!=laySTRING) 
	    {
	      //std::cout<<"not a layer 1 panel; skipping"<<std::endl;
	      continue; // skip comments
	    }
	    else 
	    {
	      ++nPanels;
	      //STRING = "1 BML phi [7.2.-7.1.2.1.1.1.1.1.56] 1644724956 -1.0666 -1.0289 -0.1988 -0.0013";
	      //STRING = "1 BML phi [7.2.-7.1.2.1.1.1.1.1.56] 1644724956";
	      ss<<STRING;
	      //ss>>layerRPC>>stName>>viewName>>stringId;
	      std::cout<<" STRING = <"<<STRING<<">"<<std::endl;
	      
	      ss>>layerRPC>>stName>>viewName>>stringId>>panelID>>etamin>>etamax>>phimin>>phimax>>zmin>>zmax;
	      
	      if (nPanels<10) std::cout<<" Variables are <"<<layerRPC<<",  "<<stName<<", "<<viewName<<", "<<stringId<<", "<<panelID
	      <<" eta,phi range ["<<etamin<<", "<<etamax<<"]["<<phimin<<", "<<phimax<<"]"
	      <<std::endl;
	      
	      t->DrawBox(etamin,phimin,etamax,phimax);
	      //DrawRpcPanel(cLayer,etamin,etamax,phimin,phimax);
	    }
	    //	    if (nPanels>1) break;
	}
	
    }
  else
    {
      std::cout<<"a problem with input file"<<std::endl;
    }
  std::cout<<"Finished; n. of panels found is "<<nPanels<<std::endl;
  cLayer->SaveAs((std::string("RPClayer")+laySTRING+std::string(".png")).c_str());
  
}

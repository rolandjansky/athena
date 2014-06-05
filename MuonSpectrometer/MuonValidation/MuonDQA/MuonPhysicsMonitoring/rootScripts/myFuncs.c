/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

using namespace RooFit;

void fitHistogram(TH1F* hJpsiMass, int &nJpsi, double &mass, int &nJpsiErr, double &massErr)
{
  RooMsgService::instance().setSilentMode(true); 
  RooRealVar m("m","mass",2000,4000,"MeV");
  //Data
  RooDataHist data("data","data",m,hJpsiMass);
  //Parameters
  //RooRealVar b("b","slope b",-0.00005,-0.0001,0.0001,"MeV^-1");
  RooRealVar nsig("nsig","signal fraction",150., 1.,2000.);
  RooRealVar nbkg("nbkg","background fraction",500.,1.,40000.);
  RooRealVar mean("mean","JPsi mass mean",3100,2800,3300,"MeV");
  RooRealVar sigma("sigma","JPsi mass sigma",120,10,300,"MeV");
  //model
  RooGaussian sig("sig","signal",m,mean,sigma);  
  RooPolynomial bkg("bkg","background",m);
  RooAddPdf model("model","model",RooArgList(sig,bkg),RooArgList(nsig,nbkg));
  model.fitTo(data,PrintEvalErrors(-1));
  RooPlot* frame1 = m.frame();
  frame1->SetTitle("J/#psi Mass");
  data.plotOn(frame1);
  model.plotOn(frame1);
  model.paramOn(frame1,Parameters(RooArgList(nsig,mean,sigma)),Layout(0.6,0.95,0.93),Format("NEU"));  
  //Parameters printing in console///
  RooArgSet* params = model.getVariables();
  //params->Print("v");
  frame1->Draw();
  
  nJpsi = nsig.getVal();
  nJpsiErr = nsig.getError();
  mass = mean.getVal();
  massErr = mean.getError();
  
  return;
}

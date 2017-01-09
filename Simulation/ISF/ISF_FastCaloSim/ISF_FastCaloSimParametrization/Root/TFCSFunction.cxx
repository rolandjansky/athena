/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include "ISF_FastCaloSimParametrization/TFCSFunction.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegression.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegressionTF.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionHistogram.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TFile.h"

#include "TRandom1.h"

//=============================================
//======= TFCSFunction =========
//=============================================

TFCSFunction::TFCSFunction()
{
  
}


TFCS1DFunction* TFCSFunction::Create(TH1* hist,int skip_regression,int neurons_start, int neurons_end, double maxdev_regression, double maxdev_smartrebin, int ntoys)
{
 int verbose_level=1;
 
 // This function is called by the user when he wants a histogram to be transformed into a space efficient variant for the parametrization.
 // All code that decides whether a histogram should be transformed into a TFCS1DFunctionRegression or TFCS1DFunctionHistogram
 // should go here. 
 
 TRandom1* random=new TRandom1();
 random->SetSeed(0);
 int myrand=floor(random->Uniform()*1000000);
 stringstream ss;
 ss << myrand;
 string myrandstr = ss.str();
 
 string xmlweightfilename="regressionweights"+myrandstr;
 string outfilename="TMVAReg"+myrandstr+".root";
 float rangeval, startval;
 TFCS1DFunction* fct=new TFCS1DFunction(hist);
 TFCS1DFunctionRegression* freg=0;
 TFCS1DFunctionRegressionTF* fregTF=0;
 TFCS1DFunctionHistogram* fhis=0;
 int status=3;
 
 status=fct->testHisto(hist,xmlweightfilename,rangeval,startval,outfilename,skip_regression,neurons_start,neurons_end,maxdev_regression,ntoys);
 
 if(verbose_level==1) cout<<"--- testHisto status="<<status<<endl;
 if(status==1)
 {
 	if(verbose_level==1) cout<<"Regression"<<endl;
  freg=new TFCS1DFunctionRegression();
  freg->storeRegression(xmlweightfilename);
  remove(outfilename.c_str());
  remove(Form("dl/%s/TMVARegression_MLP.weights.xml",xmlweightfilename.c_str()));
  remove(Form("dl/%s",xmlweightfilename.c_str()));
  return freg;
 }
 if(status==2)
 {
 	if(verbose_level==1) cout<<"Regression and Transformation"<<endl;
  fregTF=new TFCS1DFunctionRegressionTF();
  fregTF->storeRegression(xmlweightfilename,rangeval,startval);
  remove(outfilename.c_str());
  remove(Form("dl/%s/TMVARegression_MLP.weights.xml",xmlweightfilename.c_str()));
  remove(Form("dl/%s",xmlweightfilename.c_str()));
  return fregTF;
 }
 if(status==3)
 {
  cout<<"xmlweightfilename: "<<xmlweightfilename<<endl;
  remove(outfilename.c_str());
  remove(Form("dl/%s/TMVARegression_MLP.weights.xml",xmlweightfilename.c_str()));
  remove(Form("dl/%s",xmlweightfilename.c_str()));
 	if(verbose_level==1) cout<<"Histogram"<<endl;
  fhis=new TFCS1DFunctionHistogram(hist, verbose_level,maxdev_smartrebin);
  return fhis;
 }
 if(status==0)
 {
 	cout<<"something went wrong :("<<endl;
  return 0;	
 }
 
 return 0;
 
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

//ClassImp(TFCSFunction)


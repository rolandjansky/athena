/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

using namespace std;
#include "ISF_FastCaloSimParametrization/TFCSFunction.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
//#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegression.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionRegressionTF.h"
#include "ISF_FastCaloSimParametrization/TFCS1DFunctionHistogram.h"
#include "TRandom1.h"
#include <sstream>  
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ftw.h>
#include <stdexcept>

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
 TFCS1DFunctionRegression* freg;
 TFCS1DFunctionRegressionTF* fregTF;
 TFCS1DFunctionHistogram* fhis;
 int status;
 //try
 //{
  status=fct->testHisto(hist,xmlweightfilename,rangeval,startval,outfilename,skip_regression,neurons_start,neurons_end,maxdev_regression,ntoys);
 //}
 //catch(std::runtime_error &e)
 //{
 // cout<<"An exception occured: "<<e.what()<<endl;
 // cout<<"Continuing anyway :P"<<endl;
 // status=3;
 //}
 
 if(verbose_level==1) cout<<"--- testHisto status="<<status<<endl;
 if(status==1)
 {
 	if(verbose_level==1) cout<<"Regression"<<endl;
  freg=new TFCS1DFunctionRegression();
  freg->storeRegression(xmlweightfilename);
  cout<<"xmlweightfilename: "<<xmlweightfilename<<endl;
  if (remove(outfilename.c_str()) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << outfilename.c_str() << "; exiting" << endl;
    return 0;
  }
  if (remove(Form("%s/TMVARegression_MLP.weights.xml",xmlweightfilename.c_str())) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << xmlweightfilename.c_str() << "/TMVARegression_MLP.weights.xml; exiting" << endl;
    return 0;
  }
  if (remove(Form("%s",xmlweightfilename.c_str())) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << xmlweightfilename.c_str() << "; exiting" << endl;
    return 0;
  }
  return freg;
 }
 if(status==2)
 {
 	if(verbose_level==1) cout<<"Regression and Transformation"<<endl;
  fregTF=new TFCS1DFunctionRegressionTF();
  fregTF->storeRegression(xmlweightfilename,rangeval,startval);
  cout<<"xmlweightfilename: "<<xmlweightfilename<<endl;
  if (remove(outfilename.c_str()) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << outfilename.c_str() << "; exiting" << endl;
    return 0;
  }
  if (remove(Form("%s/TMVARegression_MLP.weights.xml",xmlweightfilename.c_str())) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << xmlweightfilename.c_str() << "/TMVARegression_MLP.weights.xml; exiting" << endl;
    return 0;
  }
  if (remove(Form("%s",xmlweightfilename.c_str())) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << xmlweightfilename.c_str() << "; exiting" << endl;
    return 0;
  }
  return fregTF;
 }
 if(status==3)
 {
  cout<<"xmlweightfilename: "<<xmlweightfilename<<endl;
  if (remove(outfilename.c_str()) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << outfilename.c_str() << "; exiting" << endl;
    return 0;
  }
  if (remove(Form("%s/TMVARegression_MLP.weights.xml",xmlweightfilename.c_str())) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << xmlweightfilename.c_str() << "/TMVARegression_MLP.weights.xml; exiting" << endl;
    return 0;
  }
  if (remove(Form("%s",xmlweightfilename.c_str())) != 0) {
    cout << "Error in TFCSFunction: unable to remove " << xmlweightfilename.c_str() << "; exiting" << endl;
    return 0;
  }
 	if(verbose_level==1) cout<<"Histogram"<<endl;
  fhis=new TFCS1DFunctionHistogram(hist, verbose_level,maxdev_smartrebin);
  return fhis;
 }
 if(status==0)
 {
 	cout<<"something went wrong :("<<endl;
  return 0;	
 }
 
 //delete the weight file folder:
 
 return 0; 
 
}


//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSFunction)


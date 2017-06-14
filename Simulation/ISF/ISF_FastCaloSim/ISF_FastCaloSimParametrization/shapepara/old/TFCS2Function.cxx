/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

using namespace std;
#include "ISF_FastCaloSimParametrization/TFCS2Function.h"
#include "ISF_FastCaloSimParametrization/TFCS2DFunction.h"
#include "ISF_FastCaloSimParametrization/TFCS2DFunctionRegression.h"
#include "TRandom1.h"
#include <sstream>  
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ftw.h>

//=============================================
//======= TFCSFunction =========
//=============================================

TFCS2DFunction* TFCS2Function::Create(string inputfile, int verbose_level, string label, string labelout, int mycase, float calosize, bool doNNHit)
{
  // This function is called by the user when he wants a histogram to be transformed into a space efficient variant for the parametrization.
  // All code that decides whether a histogram should be transformed into a TFCS2DFunctionRegression
  // should go here. 

  TRandom1* random=new TRandom1();
  random->SetSeed(0);
  int myrand=floor(random->Uniform()*1000000);
  stringstream ss;
  ss << myrand;
  string myrandstr = ss.str();

  string xmlweightfilename="regressionweights"+myrandstr;
  string outfilename="TMVAReg"+myrandstr+".root";
  TFCS2DFunction* fct=new TFCS2DFunction(inputfile);
  int status=fct->fitShape(inputfile,xmlweightfilename, outfilename, label, labelout, mycase,calosize,doNNHit);
  if(verbose_level==1) cout<<"--- fitShape status="<<status<<endl;

  TFCS2DFunctionRegression* freg;
  if(status==1)
  {
    if(verbose_level==1) cout<<"Regression"<<endl;
    freg=new TFCS2DFunctionRegression();
    freg->storeRegression(xmlweightfilename,mycase);
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
    return 0;
  }
  if(status==0)
  {
    cout<<"something went wrong :("<<endl;
    return 0;	
  }

  return 0 ;  
}


//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS2Function)


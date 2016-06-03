/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterLikelihood.h
/// Package : BTagTools 
/// Author  : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
/// Created : March 2007
///
/// DESCRIPTION:
///
/// This class computes the likelihood value for being b-jet, c-jet or uds-jet for 
/// a single event. The RooFit toolkit for data modeling is used here.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "GaudiKernel/ITHistSvc.h"

#include <TMath.h>
#include <TH1.h>

#include <RooCategory.h>
#include <RooMappedCategory.h>
#include <RooRealVar.h>
#include <RooThresholdCategory.h>
#include <RooFormulaVar.h>
#include <RooArgList.h>
#include <RooProdPdf.h>
#include <RooAddPdf.h>
#include <RooSuperCategory.h>
#include <RooMappedCategory.h>
#include <RooSimultaneous.h>
#include <RooArgSet.h>
#include <RooParametricStepFunction.h>
#include <RooDataHist.h>
#include <RooHistPdf.h>
#include <RooAbsCollection.h>


#include "JetTagTools/IJetFitterClassifierTool.h"
#include "JetTagTools/JetFitterLikelihood.h"

#include "JetTagInfo/JetFitterTagInfo.h"
#include "JetTagInfo/IJetFitterTagInfo.h"

#include "JetTagCalibration/CalibrationBroker.h"

namespace Analysis {


JetFitterLikelihood::JetFitterLikelihood(const std::string& name,
					       const std::string& n, const IInterface* p):
        AthAlgTool(name, n,p),
        m_calibrationTool("BTagCalibrationBroker"),
        m_calibrationDirectory("JetFitter"),
        m_calibrationSubDirectory("RooFitLikelihood")
 {

//  THIS WILL BE DELETED NOW...
//  declareProperty("JetFitterCofficientFileNames",m_textFileNames);
//  declareProperty("JetFitterCoefficientFileKeys",m_keyToTextFileNames);

  //histo files... now from calibrationBroker
  declareProperty("CalibrationDirectory",m_calibrationDirectory);
  declareProperty("CalibrationSubDirectory",m_calibrationSubDirectory);
  declareProperty("calibrationTool",	m_calibrationTool);
  declareProperty("hypotheses", m_hypothesis); 
  declareInterface<IJetFitterClassifierTool>(this);
} 

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

JetFitterLikelihood::~JetFitterLikelihood() {

/*
  std::map<std::string,RooSimultaneous*>::iterator begin=m_likelihoods.begin();
  std::map<std::string,RooSimultaneous*>::iterator end=m_likelihoods.end();
  
  for(  std::map<std::string,RooSimultaneous*>::iterator iter=begin;iter!=end;++iter) {
    if ((*iter).second!=0) {
      delete (*iter).second;
      (*iter).second=0;
    }
  }

*/

}

StatusCode JetFitterLikelihood::initialize() {

  //  std::cout << " Size of coefficients: " << m_textFileNames.size() << " and " << 
  //    m_keyToTextFileNames.size() << std::endl;

  StatusCode sc = m_calibrationTool.retrieve();
  if (sc.isFailure())
  {
    ATH_MSG_FATAL(" Could not retrieve " << m_calibrationTool  << ". Aborting...");
    return sc;
  } else ATH_MSG_INFO(" Retrieved: " << m_calibrationTool);

  m_myRooVariables=new RooArgSet("JetFitterRooArgSet");

  RooCategory *cat_nVTX=new RooCategory("cat_nVTX","number of vertices category");
  cat_nVTX->defineType("NoVTX",0);
  cat_nVTX->defineType("1VTX",1);
  cat_nVTX->defineType("2VTX",2);
  cat_nVTX->defineType("3VTX",3);
  cat_nVTX->defineType("4VTX",4);
  cat_nVTX->defineType("5VTX",5);//I hope not to need more!!!
  cat_nVTX->defineType("6VTX",6);//I hope not to need more!!!
  cat_nVTX->defineType("7VTX",7);//I hope not to need more!!!
  cat_nVTX->defineType("8VTX",8);//I hope not to need more!!!
  cat_nVTX->defineType("9VTX",9);//I hope not to need more!!!
  cat_nVTX->defineType("10VTX",10);//I hope not to need more!!!
  m_myRooVariables->addOwned(*cat_nVTX);
  
  
  RooCategory* cat_nTracksAtVtx=new RooCategory("cat_nTracksAtVtx","number of tracks at Vertices");
  cat_nTracksAtVtx->defineType("NoTrackVTX",0);
  cat_nTracksAtVtx->defineType("2TrackVTX",2);
  cat_nTracksAtVtx->defineType("3TrackVTX",3);
  cat_nTracksAtVtx->defineType("4TrackVTX",4);
  cat_nTracksAtVtx->defineType("5TrackVTX",5);
  cat_nTracksAtVtx->defineType("6TrackVTX",6);
  cat_nTracksAtVtx->defineType("7TrackVTX",7);
  cat_nTracksAtVtx->defineType("8TrackVTX",8);
  cat_nTracksAtVtx->defineType("9TrackVTX",9);
  cat_nTracksAtVtx->defineType("10TrackVTX",10);
  cat_nTracksAtVtx->defineType("11TrackVTX",11);
  cat_nTracksAtVtx->defineType("12TrackVTX",12);
  cat_nTracksAtVtx->defineType("13TrackVTX",13);
  cat_nTracksAtVtx->defineType("14TrackVTX",14);
  cat_nTracksAtVtx->defineType("15TrackVTX",15);
  cat_nTracksAtVtx->defineType("16TrackVTX",16);
  cat_nTracksAtVtx->defineType("17TrackVTX",17);
  cat_nTracksAtVtx->defineType("18TrackVTX",18);
  cat_nTracksAtVtx->defineType("19TrackVTX",19);
  cat_nTracksAtVtx->defineType("20TrackVTX",20);
  cat_nTracksAtVtx->defineType("21TrackVTX",21);
  cat_nTracksAtVtx->defineType("22TrackVTX",22);
  cat_nTracksAtVtx->defineType("23TrackVTX",23);
  cat_nTracksAtVtx->defineType("24TrackVTX",24);
  cat_nTracksAtVtx->defineType("25TrackVTX",25);
  cat_nTracksAtVtx->defineType("26TrackVTX",26);
  cat_nTracksAtVtx->defineType("27TrackVTX",27);
  cat_nTracksAtVtx->defineType("28TrackVTX",28);
  cat_nTracksAtVtx->defineType("29TrackVTX",29);
  cat_nTracksAtVtx->defineType("30TrackVTX",30);
  cat_nTracksAtVtx->defineType("31TrackVTX",31);
  cat_nTracksAtVtx->defineType("32TrackVTX",32);
  cat_nTracksAtVtx->defineType("33TrackVTX",33);
  cat_nTracksAtVtx->defineType("34TrackVTX",34);
  cat_nTracksAtVtx->defineType("35TrackVTX",35);
  cat_nTracksAtVtx->defineType("36TrackVTX",36);
  cat_nTracksAtVtx->defineType("37TrackVTX",37);
  cat_nTracksAtVtx->defineType("38TrackVTX",38);
  cat_nTracksAtVtx->defineType("39TrackVTX",39);
  cat_nTracksAtVtx->defineType("40TrackVTX",40);
  cat_nTracksAtVtx->defineType("41TrackVTX",41);
  cat_nTracksAtVtx->defineType("42TrackVTX",42);
  cat_nTracksAtVtx->defineType("43TrackVTX",43);
  cat_nTracksAtVtx->defineType("44TrackVTX",44);
  cat_nTracksAtVtx->defineType("45TrackVTX",45);
  cat_nTracksAtVtx->defineType("46TrackVTX",46);
  cat_nTracksAtVtx->defineType("47TrackVTX",47);
  cat_nTracksAtVtx->defineType("48TrackVTX",48);
  cat_nTracksAtVtx->defineType("49TrackVTX",49);
  cat_nTracksAtVtx->defineType("50TrackVTX",50);


  m_myRooVariables->addOwned(*cat_nTracksAtVtx);

  RooCategory* cat_nSingleTracks=new RooCategory("cat_nSingleTracks","number of single significant tracks");
  cat_nSingleTracks->defineType("NoSingleTrack",0);
  cat_nSingleTracks->defineType("1SingleTrack",1);
  cat_nSingleTracks->defineType("2SingleTrack",2);
  cat_nSingleTracks->defineType("3SingleTrack",3);
  cat_nSingleTracks->defineType("4SingleTrack",4);
  cat_nSingleTracks->defineType("5SingleTrack",5);
  cat_nSingleTracks->defineType("6SingleTrack",6);
  cat_nSingleTracks->defineType("7SingleTrack",7);
  cat_nSingleTracks->defineType("8SingleTrack",8);
  cat_nSingleTracks->defineType("9SingleTrack",9);
  cat_nSingleTracks->defineType("10SingleTrack",10);
  cat_nSingleTracks->defineType("11SingleTrack",11);
  cat_nSingleTracks->defineType("12SingleTrack",12);
  cat_nSingleTracks->defineType("13SingleTrack",13);
  cat_nSingleTracks->defineType("14SingleTrack",14);
  cat_nSingleTracks->defineType("15SingleTrack",15);
  m_myRooVariables->addOwned(*cat_nSingleTracks);  

  RooMappedCategory* mycat_nVTX_nolvalue=new RooMappedCategory("mycat_nVTX_nolvalue","number of vertices categories",*cat_nVTX);//,"Undefined");
  mycat_nVTX_nolvalue->map("NoVTX","NoVTX");
  mycat_nVTX_nolvalue->map("1VTX","1VTX");
  mycat_nVTX_nolvalue->map("2VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("3VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("4VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("5VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("6VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("7VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("8VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("9VTX","2orMoreVTX");
  mycat_nVTX_nolvalue->map("10VTX","2orMoreVTX");
  m_myRooVariables->addOwned(*mycat_nVTX_nolvalue);

  RooCategory* mycat_nVTX=static_cast<RooCategory*>(mycat_nVTX_nolvalue->createFundamental("mycat_nVTX"));
  m_myRooVariables->addOwned(*mycat_nVTX);
  //  m_fictiousToRealCategory[mycat_nVTX_nolvalue]=mycat_nVTX;

  RooMappedCategory* mycat_nTracksAtVtx6Bin_nolvalue=
    new RooMappedCategory("mycat_nTracksAtVtx6Bin_nolvalue","number of tracks at vertices category",*cat_nTracksAtVtx);//,"Undefined");
  mycat_nTracksAtVtx6Bin_nolvalue->map("NoTrackVTX","NoTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("2TrackVTX","2TrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("3TrackVTX","3TrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("4TrackVTX","4TrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("5TrackVTX","5or6TrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("6TrackVTX","5or6TrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("7TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("8TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("9TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("10TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("11TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("12TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("13TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("14TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("15TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("16TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("17TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("18TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("19TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("20TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("21TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("22TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("23TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("24TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("25TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("26TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("27TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("28TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("29TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("30TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("31TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("32TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("33TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("34TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("35TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("36TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("37TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("38TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("39TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("40TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("41TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("42TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("43TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("44TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("45TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("46TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("47TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("48TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("49TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin_nolvalue->map("50TrackVTX","7orMoreTrackVTX");
  m_myRooVariables->addOwned(*mycat_nTracksAtVtx6Bin_nolvalue);
  
  RooCategory* mycat_nTracksAtVtx6Bin=static_cast<RooCategory*>(mycat_nTracksAtVtx6Bin_nolvalue->createFundamental("mycat_nTracksAtVtx6Bin"));
  m_myRooVariables->addOwned(*mycat_nTracksAtVtx6Bin);
  //  m_fictiousToRealCategory[mycat_nTracksAtVtx6Bin_nolvalue]=mycat_nTracksAtVtx6Bin;
  
  RooMappedCategory* mycat_nSingleTracks_nolvalue=
    new RooMappedCategory("mycat_nSingleTracks_nolvalue","number of single significant tracks category",*cat_nSingleTracks);//,"Undefined");
  mycat_nSingleTracks_nolvalue->map("NoSingleTrack","NoSingleTrack");
  mycat_nSingleTracks_nolvalue->map("1SingleTrack","1SingleTrack");
  mycat_nSingleTracks_nolvalue->map("2SingleTrack","2SingleTrack");
  mycat_nSingleTracks_nolvalue->map("3SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("4SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("5SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("6SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("7SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("8SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("9SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("10SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("11SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("12SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("13SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("14SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks_nolvalue->map("15SingleTrack","3orMoreSingleTrack");
  m_myRooVariables->addOwned(*mycat_nSingleTracks_nolvalue);

  RooCategory* mycat_nSingleTracks=static_cast<RooCategory*>(mycat_nSingleTracks_nolvalue->createFundamental("mycat_nSingleTracks"));
  m_myRooVariables->addOwned(*mycat_nSingleTracks);
  //  m_fictiousToRealCategory[mycat_nSingleTracks_nolvalue]=mycat_nSingleTracks;
  
  RooRealVar *energyFraction=new RooRealVar("energyFraction","energyFraction",0.5,0.,1.00001);
  RooRealVar *significance3d=new RooRealVar("significance3d","significance3d",5.,0.,100.);
  RooRealVar *mass=new RooRealVar("mass","mass",2000.,0.,10000.);
  m_myRooVariables->addOwned(*energyFraction);
  m_myRooVariables->addOwned(*significance3d);
  m_myRooVariables->addOwned(*mass);

 
  RooThresholdCategory* energyFractionCat_nolvalue=new RooThresholdCategory("energyFractionCat_nolvalue","energyFractionCat",
									    *energyFraction,"NoFrag");

  energyFractionCat_nolvalue->addThreshold(0.999,"Normal");    
  m_myRooVariables->addOwned(*energyFractionCat_nolvalue);

  RooCategory* energyFractionCat=static_cast<RooCategory*>(energyFractionCat_nolvalue->createFundamental("energyFractionCat"));
  m_myRooVariables->addOwned(*energyFractionCat);
  //  m_fictiousToRealCategory[energyFractionCat_nolvalue]=energyFractionCat;
    
  RooSuperCategory* allCategoriesCombined=new RooSuperCategory("allCategoriesCombined",
							       "allCategoriesCombined",
							       RooArgList(*mycat_nVTX,
									  *mycat_nSingleTracks,
									  *mycat_nTracksAtVtx6Bin,
									  *energyFractionCat));
  
  //  allCategoriesCombined->Print("v");
  m_myRooVariables->addOwned(*allCategoriesCombined);

  RooMappedCategory* allCategoriesCombined_reduced_nolvalue=new RooMappedCategory("allCategoriesCombined_reduced_nolvalue",
										  "allCategoriesCombined_reduced",
										  *allCategoriesCombined,
										  "Undefined");
  
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;NoSingleTrack;NoTrackVTX;NoFrag}","{NoVTX;NoSingleTrack}");
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;NoSingleTrack;NoTrackVTX;Normal}","{NoVTX;NoSingleTrack}");
  
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;1SingleTrack;NoTrackVTX;Normal}","{NoVTX;1SingleTrack;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;2SingleTrack;NoTrackVTX;Normal}","{NoVTX;2SingleTrack;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;3orMoreSingleTrack;NoTrackVTX;Normal}","{NoVTX;3orMoreSingleTrack;Normal}");
  
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;2TrackVTX;Normal}","{1VTX;NoSingleTrack;2TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;2TrackVTX;Normal}","{1VTX;1SingleTrack;2TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;2TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;2TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;2TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;2TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NotMapped;3TrackVTX;Normal}","{1VTX;NotMapped;3TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;3TrackVTX;Normal}","{1VTX;NoSingleTrack;3TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;3TrackVTX;Normal}","{1VTX;1SingleTrack;3TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;3TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;3TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;3TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;3TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;4TrackVTX;Normal}","{1VTX;NoSingleTrack;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;4TrackVTX;Normal}","{1VTX;1SingleTrack;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;4TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;4TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;5or6TrackVTX;Normal}","{1VTX;NoSingleTrack;5or6TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;5or6TrackVTX;Normal}","{1VTX;1SingleTrack;5or6TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;5or6TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;5or6TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;5or6TrackVTX;Normal}","{1VTX;2orMoreSingleTrack;5or6TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;7orMoreTrackVTX;Normal}","{1VTX;NoSingleTrack;7orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;7orMoreTrackVTX;Normal}","{1VTX;1SingleTrack;7orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;7orMoreTrackVTX;Normal}","{1VTX;2orMoreSingleTrack;7orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;7orMoreTrackVTX;Normal}","{1VTX;2orMoreSingleTrack;7orMoreTrackVTX;Normal}");
  
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;NoSingleTrack;4TrackVTX;Normal}","{2orMoreVTX;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;1SingleTrack;4TrackVTX;Normal}","{2orMoreVTX;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;2SingleTrack;4TrackVTX;Normal}","{2orMoreVTX;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;3orMoreSingleTrack;4TrackVTX;Normal}","{2orMoreVTX;4TrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;NoSingleTrack;5or6TrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;1SingleTrack;5or6TrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;2SingleTrack;5or6TrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;3orMoreSingleTrack;5or6TrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;NoSingleTrack;7orMoreTrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;1SingleTrack;7orMoreTrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;2SingleTrack;7orMoreTrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;3orMoreSingleTrack;7orMoreTrackVTX;Normal}","{2orMoreVTX;5orMoreTrackVTX;Normal}");
  
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;1SingleTrack;NoTrackVTX;NoFrag}","{NoVTX;1SingleTrack;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;2SingleTrack;NoTrackVTX;NoFrag}","{NoVTX;2SingleTrack;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{NoVTX;3orMoreSingleTrack;NoTrackVTX;NoFrag}","{NoVTX;3orMoreSingleTrack;NoFrag}");
  
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;2TrackVTX;NoFrag}","{1VTX;NoSingleTrack;2TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;2TrackVTX;NoFrag}","{1VTX;1SingleTrack;2TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;2TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;2TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;2TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;2TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NotMapped;3TrackVTX;NoFrag}","{1VTX;NotMapped;3TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;3TrackVTX;NoFrag}","{1VTX;NoSingleTrack;3TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;3TrackVTX;NoFrag}","{1VTX;1SingleTrack;3TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;3TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;3TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;3TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;3TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;4TrackVTX;NoFrag}","{1VTX;NoSingleTrack;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;4TrackVTX;NoFrag}","{1VTX;1SingleTrack;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;4TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;4TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;5or6TrackVTX;NoFrag}","{1VTX;NoSingleTrack;5or6TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;5or6TrackVTX;NoFrag}","{1VTX;1SingleTrack;5or6TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;5or6TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;5or6TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;5or6TrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;5or6TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;NoSingleTrack;7orMoreTrackVTX;NoFrag}","{1VTX;NoSingleTrack;7orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;1SingleTrack;7orMoreTrackVTX;NoFrag}","{1VTX;1SingleTrack;7orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;2SingleTrack;7orMoreTrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;7orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{1VTX;3orMoreSingleTrack;7orMoreTrackVTX;NoFrag}","{1VTX;2orMoreSingleTrack;7orMoreTrackVTX;NoFrag}");
  
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;NoSingleTrack;4TrackVTX;NoFrag}","{2orMoreVTX;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;1SingleTrack;4TrackVTX;NoFrag}","{2orMoreVTX;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;2SingleTrack;4TrackVTX;NoFrag}","{2orMoreVTX;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;3orMoreSingleTrack;4TrackVTX;NoFrag}","{2orMoreVTX;4TrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;NoSingleTrack;5or6TrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;1SingleTrack;5or6TrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;2SingleTrack;5or6TrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;3orMoreSingleTrack;5or6TrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;NoSingleTrack;7orMoreTrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;1SingleTrack;7orMoreTrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;2SingleTrack;7orMoreTrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  allCategoriesCombined_reduced_nolvalue->map("{2orMoreVTX;3orMoreSingleTrack;7orMoreTrackVTX;NoFrag}","{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
  m_myRooVariables->addOwned(*allCategoriesCombined_reduced_nolvalue);  

  RooCategory* allCategoriesCombined_reduced=static_cast<RooCategory*>(allCategoriesCombined_reduced_nolvalue->createFundamental("allCategoriesCombined_reduced"));
  m_myRooVariables->addOwned(*allCategoriesCombined_reduced);
  //  m_fictiousToRealCategory[allCategoriesCombined_reduced_nolvalue]=allCategoriesCombined_reduced;

  //now register all the needed histograms...

  if (m_hypothesis.size() == 0) { 
    m_hypothesis.push_back("bottom"); 
    m_hypothesis.push_back("light"); 
    m_hypothesis.push_back("charm"); 
  }
  for (std::vector<std::string>::const_iterator hyp_itr = m_hypothesis.begin(); 
       hyp_itr != m_hypothesis.end(); 
       hyp_itr++) { 
    initializeCalibrationFile(*hyp_itr);
  }

  ATH_MSG_INFO(" Initialization of JetFitterLikelihood succesfull");
  return StatusCode::SUCCESS;
}

StatusCode JetFitterLikelihood::finalize() {

  //here you should probably delete something :-)

  //(19-3-2008) Let's try...


  std::map<std::string, std::vector<RooDataHist*>* >::iterator histogramsBegin=m_likelihoodHistograms.begin();
  std::map<std::string, std::vector<RooDataHist*>* >::iterator histogramsEnd=m_likelihoodHistograms.end();
  
  for (std::map<std::string, std::vector<RooDataHist*>* >::iterator histogramsIter=histogramsBegin;
       histogramsIter!=histogramsEnd;histogramsIter++)
  {
    
    if ((*histogramsIter).second==0)
    {
      ATH_MSG_WARNING(" Found 0 pointer. This is a bug. ");
    }

    std::vector<RooDataHist*>::iterator dataBegin=(*histogramsIter).second->begin();
    std::vector<RooDataHist*>::iterator dataEnd=(*histogramsIter).second->end();

    for (std::vector<RooDataHist*>::iterator dataIter=dataBegin;dataIter!=dataEnd;++dataIter)
    {

      delete *dataIter;
      *dataIter=0;
    }

    delete (*histogramsIter).second;
    (*histogramsIter).second=0;

  }
  
  std::map<std::string, RooArgSet* >::iterator objectsBegin=m_likelihoodObjects.begin();
  std::map<std::string, RooArgSet* >::iterator objectsEnd=m_likelihoodObjects.end();
  
  for (std::map<std::string, RooArgSet* >::iterator objectsIter=objectsBegin;
       objectsIter!=objectsEnd;++objectsIter)
  {
    delete (*objectsIter).second;
    (*objectsIter).second=0;
  }
  
  delete m_myRooVariables;
  m_myRooVariables=0;
  
  ATH_MSG_INFO(" Finalization of JetFitterLikelihood succesfull");
  return StatusCode::SUCCESS;
}

  void JetFitterLikelihood::initializeCalibrationFile(const std::string & hypothesis)
  {
    
    TString directory(m_calibrationSubDirectory);
    directory+="/";

    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_noVTX_significance3d_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_noVTX_1SingleTrack_energyFraction_normal_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_noVTX_2orMoreSingleTrack_energyFraction_normal_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_1orMoreVTX_2TrackVTX_energyFraction_normal_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_1orMoreVTX_3TrackVTX_energyFraction_normal_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_1orMoreVTX_significance3d_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_1orMoreVTX_2TrackVTX_mass_histo")));
    m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_1orMoreVTX_3orMoreTrackVTX_mass_histo")));

    //here store the names to load for the coefficients...    
    std::vector<TString> coefficientNamesToLoad;
    coefficientNamesToLoad.push_back(TString("coef"));
    coefficientNamesToLoad.push_back(TString("e_1VTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_2TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_3TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_4TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_5or6TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1Vtx_1SingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_1Vtx_NoSingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_1orMoreVTX_2TrackVTX_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_1orMoreVTX_3TrackVTX_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_2orMoreVTX_4TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_NoVTX"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_1SingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_1SingleTrack_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_2SingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_2orMoreSingleTrack_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_NoSingleTrack"));
    
    //as a second step read the coefficients directly from the calibration histograms (1 histogram per coefficient)
    std::vector<TString>::const_iterator beginCoef=coefficientNamesToLoad.begin();
    std::vector<TString>::const_iterator endCoef=coefficientNamesToLoad.end();

    for (std::vector<TString>::const_iterator iterCoef=beginCoef;iterCoef!=endCoef;++iterCoef)
    {
     m_calibrationTool->registerHistogram(m_calibrationDirectory,std::string((const char*)(directory+hypothesis+"_"+*iterCoef)));
    }

    ATH_MSG_VERBOSE(" Initialized calibration file for: " << hypothesis << " tagger name: " << m_calibrationDirectory << " subdirectory: " << m_calibrationSubDirectory);

    m_calibrationTool->printStatus();

  }
  

  void JetFitterLikelihood::loadCalibration(const std::string & jetauthor,const std::string & hypothesis) {


    std::string refFileName(jetauthor);

    if (retrieveHistoFromFile(refFileName,hypothesis+"_noVTX_significance3d_histo")==0)
    {
      ATH_MSG_ERROR(" Cannot get calibration histograms. JetFitterTag and JetFitterCOMB taggers will produce nonsense");
      return;
    }

    bool deleteOldCalibration=false;
    

    if (m_likelihoods[jetauthor+hypothesis]!=0) 
    {
      if (isCalibrationUpdated(refFileName,hypothesis+"_noVTX_significance3d_histo"))
      {
        deleteOldCalibration=true;
      }
      else
      {
        return;
      }
    }
    
    if (deleteOldCalibration)
    {
      ATH_MSG_VERBOSE(" deleting calibration " );
      std::vector<RooDataHist*>* histogramsVector=m_likelihoodHistograms[jetauthor+hypothesis];
      
      if (histogramsVector==0)
      {
        ATH_MSG_ERROR("Should delete old calibration, but cannot find related histograms... This is a bug. ");
        return;
      }

      std::vector<RooDataHist*>::iterator dataBegin=histogramsVector->begin();
      std::vector<RooDataHist*>::iterator dataEnd=histogramsVector->end();
      
      for (std::vector<RooDataHist*>::iterator dataIter=dataBegin;dataIter!=dataEnd;++dataIter)
      {
        delete *dataIter;
        *dataIter=0;
      }

      delete histogramsVector;
      m_likelihoodHistograms[jetauthor+hypothesis]=0;

      RooArgSet* likelihoodObjects=m_likelihoodObjects[jetauthor+hypothesis];

      if (likelihoodObjects==0)
      {
        ATH_MSG_ERROR("Should delete old calibration, but cannot find related likelihood objects... This is a bug. ");
        return;
      }

      delete likelihoodObjects;
      m_likelihoodObjects[jetauthor+hypothesis]=0;

    }

    std::vector<RooDataHist*>* histogramsVector=new std::vector<RooDataHist*>();

    m_likelihoodHistograms[jetauthor+hypothesis]=histogramsVector;

    RooArgSet* likelihoodObjects=new RooArgSet();

    m_likelihoodObjects[jetauthor+hypothesis]=likelihoodObjects;
    
    
    //m_likelihoodObjects[jetauthor+hypothesis]=objectsRooArgSet;
    

    //    RooCategory* cat_nVTX=static_cast<RooCategory*>(m_myRooVariables->find("cat_nVTX"));
    //    RooCategory* cat_nTracksAtVtx=static_cast<RooCategory*>(m_myRooVariables->find("cat_nTracksAtVtx"));
    //    RooCategory* cat_nSingleTracks=static_cast<RooCategory*>(m_myRooVariables->find("cat_nSingleTracks"));

    //    RooCategory* mycat_nVTX=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nVTX"));
    //    RooCategory* mycat_nSingleTracks=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nSingleTracks"));
    
    RooRealVar *energyFraction=static_cast<RooRealVar*>(m_myRooVariables->find("energyFraction"));
    RooRealVar *significance3d=static_cast<RooRealVar*>(m_myRooVariables->find("significance3d"));
    RooRealVar *mass=static_cast<RooRealVar*>(m_myRooVariables->find("mass"));
    
    //    RooCategory* energyFractionCat=static_cast<RooCategory*>(m_myRooVariables->find("energyFractionCat"));

    RooRealVar* singleflavour_coef=new RooRealVar((hypothesis+"_"+jetauthor+"_coef").c_str(),(hypothesis+"_"+jetauthor+" jet component").c_str(),100000.,0.,1000000.);

    RooRealVar* singleflavour_e_NoVTX=new RooRealVar((hypothesis+"_"+jetauthor+"_e_NoVTX").c_str(),"no vertex coefficient",0.2,0.,1.);

    RooRealVar* singleflavour_e_1VTX=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1VTX").c_str(),"1 vertex coefficient",0.2,0.,1.);

    RooFormulaVar* singleflavour_e_2orMoreVTX=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_2orMoreVTX").c_str(),"2 vertex coefficient","1.-@0-@1",RooArgList(*singleflavour_e_NoVTX,
															    *singleflavour_e_1VTX));
    RooRealVar* singleflavour_e_1VTX_2TrackVTX=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1VTX_2TrackVTX").c_str(),
                                                              (hypothesis+"_"+jetauthor+"_e_1VTX_2TrackVTX").c_str(),0.2,0.,1.);
    RooRealVar* singleflavour_e_1VTX_3TrackVTX=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1VTX_3TrackVTX").c_str(),
						       (hypothesis+"_"+jetauthor+"_e_1VTX_3TrackVTX").c_str(),0.2,0.,1.);

    RooRealVar* singleflavour_e_1VTX_4TrackVTX=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1VTX_4TrackVTX").c_str(),
						       (hypothesis+"_"+jetauthor+"_e_1VTX_4TrackVTX").c_str(),0.2,0.,1.);

    RooRealVar* singleflavour_e_1VTX_5or6TrackVTX=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1VTX_5or6TrackVTX").c_str(),
							  (hypothesis+"_"+jetauthor+"_e_1VTX_5or6TrackVTX").c_str(),0.2,0.,1.);

    RooFormulaVar* singleflavour_e_1VTX_7orMoreTrackVTX =new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_1VTX_7orMoreTrackVTX").c_str(),
								    (hypothesis+"_"+jetauthor+"_e_1VTX_7orMoreTrackVTX").c_str(),
								    "1.-@0-@1-@2-@3",RooArgList(*singleflavour_e_1VTX_2TrackVTX,
												*singleflavour_e_1VTX_3TrackVTX,
												*singleflavour_e_1VTX_4TrackVTX,
												*singleflavour_e_1VTX_5or6TrackVTX));
    

    RooRealVar* singleflavour_e_1Vtx_NoSingleTrack=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1Vtx_NoSingleTrack").c_str(),
							   (hypothesis+"_"+jetauthor+"_e_1Vtx_NoSingleTrack").c_str(),
							   0.4,0.,1.);
 
   RooRealVar* singleflavour_e_1Vtx_1SingleTrack=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1Vtx_1SingleTrack").c_str(),
							  (hypothesis+"_"+jetauthor+"_e_1Vtx_1SingleTrack").c_str(),
							  0.4,0.,1.);
 
   RooFormulaVar* singleflavour_e_1Vtx_2orMoreSingleTrack=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_1Vtx_2orMoreSingleTrack").c_str(),
								      (hypothesis+"_"+jetauthor+"_e_1Vtx_2orMoreSingleTrack").c_str(),
								      "1.-@0-@1",RooArgList(*singleflavour_e_1Vtx_NoSingleTrack,
											    *singleflavour_e_1Vtx_1SingleTrack));
    
 
   RooRealVar* singleflavour_e_2orMoreVTX_4TrackVTX=new RooRealVar((hypothesis+"_"+jetauthor+"_e_2orMoreVTX_4TrackVTX").c_str(),
							     (hypothesis+"_"+jetauthor+"_e_2orMoreVTX_4TrackVTX").c_str(),0.2,0.,1.);
 
   RooFormulaVar* singleflavour_e_2orMoreVTX_5orMoreTrackVTX=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_2orMoreVTX_5orMoreTrackVTX").c_str(),
									 (hypothesis+"_"+jetauthor+"_e_2orMoreVTX_5orMoreTrackVTX").c_str(),
									 "1.-@0",RooArgList(*singleflavour_e_2orMoreVTX_4TrackVTX));
    
    
    //energyFraction normal...
    
 
   RooRealVar* singleflavour_e_NoVtx_1SingleTrack_energyFraction_normal=new RooRealVar((hypothesis+"_"+jetauthor+"_e_NoVtx_1SingleTrack_energyFraction_normal").c_str(),
										 (hypothesis+"_"+jetauthor+"_e_NoVtx_1SingleTrack_energyFraction_normal").c_str(),
										 0.8,0.,1.);
    
    
    
 
   RooFormulaVar* singleflavour_e_NoVtx_1SingleTrack_energyFraction_nofrag=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_NoVtx_1SingleTrack_energyFraction_nofrag").c_str(),
										       (hypothesis+"_"+jetauthor+"_e_NoVtx_1SingleTrack_energyFraction_nofrag").c_str(),
										       "1.-@0",RooArgList(*singleflavour_e_NoVtx_1SingleTrack_energyFraction_normal));
    
    

    RooRealVar* singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_normal=new RooRealVar((hypothesis+"_"+jetauthor+"_e_NoVtx_2orMoreSingleTrack_energyFraction_normal").c_str(),
										       (hypothesis+"_"+jetauthor+"_e_NoVtx_2orMoreSingleTrack_energyFraction_normal").c_str(),
										       0.8,0.,1.);
    
    
    
 
   RooFormulaVar* singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag").c_str(),
											     (hypothesis+"_"+jetauthor+"_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag").c_str(),
											     "1-@0",RooArgList(*singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_normal));
    
    
 
   RooRealVar* singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_normal=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1orMoreVTX_2TrackVTX_energyFraction_normal").c_str(),
										   (hypothesis+"_"+jetauthor+"_e_1orMoreVTX_2TrackVTX_energyFraction_normal").c_str(),
										   0.8,0.,1.);
    
    
    

    RooFormulaVar* singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag").c_str(),
											 (hypothesis+"_"+jetauthor+"_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag").c_str(),
											 "1-@0",RooArgList(*singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_normal));
    
    
 
   RooRealVar* singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_normal=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1orMoreVTX_3TrackVTX_energyFraction_normal").c_str(),
										   (hypothesis+"_"+jetauthor+"_e_1orMoreVTX_3TrackVTX_energyFraction_normal").c_str(),
										   0.8,0.,1.);
    
    
    
 
   RooFormulaVar* singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag").c_str(),
											 (hypothesis+"_"+jetauthor+"_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag").c_str(),
											 "1-@0",RooArgList(*singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_normal));
    
 
   RooRealVar* singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal=new RooRealVar((hypothesis+"_"+jetauthor+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal").c_str(),
											 (hypothesis+"_"+jetauthor+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal").c_str(),
											 0.8,0.,1.);
    

    RooFormulaVar* singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag").c_str(),
											       (hypothesis+"_"+jetauthor+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag").c_str(),
											       "1-@0",RooArgList(*singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
    
    //0ST
    
 
   RooFormulaVar* singleflavour_1VTX_0ST_2VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_normal").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_normal").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_NoSingleTrack,
												  *singleflavour_e_1VTX_2TrackVTX,
												  *singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_normal));
    
    
  
  RooFormulaVar* singleflavour_1VTX_0ST_3VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_normal").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_normal").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_NoSingleTrack,
												  *singleflavour_e_1VTX_3TrackVTX,
												  *singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_normal));
    
  
  RooFormulaVar* singleflavour_1VTX_0ST_4VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_normal").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_normal").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_NoSingleTrack,
												  *singleflavour_e_1VTX_4TrackVTX,
												  *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
  
  RooFormulaVar* singleflavour_1VTX_0ST_5or6VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_normal").c_str(),
									 (hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_normal").c_str(),
									 "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												     *singleflavour_e_1VTX,
												     *singleflavour_e_1Vtx_NoSingleTrack,
												     *singleflavour_e_1VTX_5or6TrackVTX,
												     *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
  
  RooFormulaVar* singleflavour_1VTX_0ST_7orMoreVTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_normal").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_normal").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_NoSingleTrack,
													*singleflavour_e_1VTX_7orMoreTrackVTX,
													*singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
    //1ST
    
 
   RooFormulaVar* singleflavour_1VTX_1ST_2VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_normal").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_normal").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_1SingleTrack,
												  *singleflavour_e_1VTX_2TrackVTX,
												  *singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_normal));
    
    

    RooFormulaVar* singleflavour_1VTX_1ST_3VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_normal").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_normal").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_1SingleTrack,
												  *singleflavour_e_1VTX_3TrackVTX,
												  *singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_normal));
    
 
   RooFormulaVar* singleflavour_1VTX_1ST_4VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_normal").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_normal").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_1SingleTrack,
												  *singleflavour_e_1VTX_4TrackVTX,
												  *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    

    RooFormulaVar* singleflavour_1VTX_1ST_5or6VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_normal").c_str(),
									 (hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_normal").c_str(),
									 "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												     *singleflavour_e_1VTX,
												     *singleflavour_e_1Vtx_1SingleTrack,
												     *singleflavour_e_1VTX_5or6TrackVTX,
												     *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
 
   RooFormulaVar* singleflavour_1VTX_1ST_7orMoreVTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_normal").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_normal").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_1SingleTrack,
													*singleflavour_e_1VTX_7orMoreTrackVTX,
													*singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
    
    //2orMoreST
    
 
   RooFormulaVar* singleflavour_1VTX_2orMoreST_2VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_normal").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_normal").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_2orMoreSingleTrack,
													*singleflavour_e_1VTX_2TrackVTX,
													*singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_normal));
    
    

    RooFormulaVar* singleflavour_1VTX_2orMoreST_3VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_normal").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_normal").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_2orMoreSingleTrack,
													*singleflavour_e_1VTX_3TrackVTX,
													*singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_normal));
    

    RooFormulaVar* singleflavour_1VTX_2orMoreST_4VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_normal").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_normal").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_2orMoreSingleTrack,
													*singleflavour_e_1VTX_4TrackVTX,
													*singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
 
   RooFormulaVar* singleflavour_1VTX_2orMoreST_5or6VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_normal").c_str(),
									       (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_normal").c_str(),
									       "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													   *singleflavour_e_1VTX,
													   *singleflavour_e_1Vtx_2orMoreSingleTrack,
													   *singleflavour_e_1VTX_5or6TrackVTX,
													   *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    

    RooFormulaVar* singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_normal").c_str(),
										  (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_normal").c_str(),
										  "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													      *singleflavour_e_1VTX,
													      *singleflavour_e_1Vtx_2orMoreSingleTrack,
													      *singleflavour_e_1VTX_7orMoreTrackVTX,
													      *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
    
    
    //2orMoreVTX
    
 
   RooFormulaVar* singleflavour_2orMoreVTX_4VTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_normal").c_str(),
									(hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_normal").c_str(),
									"@0*@1*@2*@3",RooArgList(*singleflavour_coef,
												 *singleflavour_e_2orMoreVTX,
												 *singleflavour_e_2orMoreVTX_4TrackVTX,
												 *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    

    RooFormulaVar* singleflavour_2orMoreVTX_5orMoreVTX_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_normal").c_str(),
									      (hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_normal").c_str(),
									      "@0*@1*@2*@3",RooArgList(*singleflavour_coef,
												       *singleflavour_e_2orMoreVTX,
												       *singleflavour_e_2orMoreVTX_5orMoreTrackVTX,
												       *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
    
    
    //
    

    RooRealVar* singleflavour_e_NoVtx_NoSingleTrack=new RooRealVar((hypothesis+"_"+jetauthor+"_e_NoVtx_NoSingleTrack").c_str(),
							    (hypothesis+"_"+jetauthor+"_e_NoVtx_NoSingleTrack").c_str(),
							    0.4,0.,1.);

    RooRealVar* singleflavour_e_NoVtx_1SingleTrack=new RooRealVar((hypothesis+"_"+jetauthor+"_e_NoVtx_1SingleTrack").c_str(),
							   (hypothesis+"_"+jetauthor+"_e_NoVtx_1SingleTrack").c_str(),
							   0.2,0.,1.);

    RooRealVar* singleflavour_e_NoVtx_2SingleTrack=new RooRealVar((hypothesis+"_"+jetauthor+"_e_NoVtx_2SingleTrack").c_str(),
							   (hypothesis+"_"+jetauthor+"_e_NoVtx_2SingleTrack").c_str(),
							   0.2,0.,1.);

    RooFormulaVar* singleflavour_e_NoVtx_3orMoreSingleTrack=new RooFormulaVar((hypothesis+"_"+jetauthor+"_e_NoVtx_3orMoreSingleTrack").c_str(),
								       (hypothesis+"_"+jetauthor+"_e_NoVtx_3orMoreSingleTrack").c_str(),
								       "1.-@0-@1-@2",RooArgList(*singleflavour_e_NoVtx_NoSingleTrack,
												*singleflavour_e_NoVtx_1SingleTrack,
												*singleflavour_e_NoVtx_2SingleTrack));
    
    //coef for noVTX case
    
 
   RooFormulaVar* singleflavour_noVTX_0ST_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_noVTX_0ST").c_str(),
							   (hypothesis+"_"+jetauthor+"_noVTX_0ST").c_str(),
							   "@0*@1*@2",RooArgList(*singleflavour_coef,
										 *singleflavour_e_NoVTX,
										 *singleflavour_e_NoVtx_NoSingleTrack));
    
 
   RooFormulaVar* singleflavour_noVTX_1ST_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_noVTX_1ST_normal").c_str(),
								  (hypothesis+"_"+jetauthor+"_noVTX_1ST_normal").c_str(),
								  "@0*@1*@2*@3",RooArgList(*singleflavour_coef,
											   *singleflavour_e_NoVTX,
											   *singleflavour_e_NoVtx_1SingleTrack,
											   *singleflavour_e_NoVtx_1SingleTrack_energyFraction_normal));
    

    RooFormulaVar* singleflavour_noVTX_2ST_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_noVTX_2ST_normal").c_str(),
								  (hypothesis+"_"+jetauthor+"_noVTX_2ST_normal").c_str(),
								  "@0*@1*@2*@3",RooArgList(*singleflavour_coef,
											   *singleflavour_e_NoVTX,
											   *singleflavour_e_NoVtx_2SingleTrack,
											   *singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_normal));
    

    RooFormulaVar* singleflavour_noVTX_3orMoreST_normal_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_normal").c_str(),
									(hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_normal").c_str(),
									"@0*@1*@2*@3",RooArgList(*singleflavour_coef,
												 *singleflavour_e_NoVTX,
												 *singleflavour_e_NoVtx_3orMoreSingleTrack,
												 *singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_normal));
    
    //ALL COEFFICIENTS FOR NOFRAG CASE AGAIN
    
    
    //0ST
    
 
   RooFormulaVar* singleflavour_1VTX_0ST_2VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_nofrag").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_nofrag").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_NoSingleTrack,
												  *singleflavour_e_1VTX_2TrackVTX,
												  *singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag));
    
    
  
   RooFormulaVar* singleflavour_1VTX_0ST_3VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_nofrag").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_nofrag").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_NoSingleTrack,
												  *singleflavour_e_1VTX_3TrackVTX,
												  *singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag));
    
  
   RooFormulaVar* singleflavour_1VTX_0ST_4VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_nofrag").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_nofrag").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_NoSingleTrack,
												  *singleflavour_e_1VTX_4TrackVTX,
												  *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
   
  RooFormulaVar* singleflavour_1VTX_0ST_5or6VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_nofrag").c_str(),
									 (hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_nofrag").c_str(),
									 "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												     *singleflavour_e_1VTX,
												     *singleflavour_e_1Vtx_NoSingleTrack,
												     *singleflavour_e_1VTX_5or6TrackVTX,
												     *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
    
 RooFormulaVar* singleflavour_1VTX_0ST_7orMoreVTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_nofrag").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_nofrag").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_NoSingleTrack,
													*singleflavour_e_1VTX_7orMoreTrackVTX,
													*singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
    //1ST
    
   
  RooFormulaVar* singleflavour_1VTX_1ST_2VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_nofrag").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_nofrag").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_1SingleTrack,
												  *singleflavour_e_1VTX_2TrackVTX,
												  *singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag));
    
    
  
   RooFormulaVar* singleflavour_1VTX_1ST_3VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_nofrag").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_nofrag").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_1SingleTrack,
												  *singleflavour_e_1VTX_3TrackVTX,
												  *singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag));
    
  
   RooFormulaVar* singleflavour_1VTX_1ST_4VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_nofrag").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_nofrag").c_str(),
								      "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												  *singleflavour_e_1VTX,
												  *singleflavour_e_1Vtx_1SingleTrack,
												  *singleflavour_e_1VTX_4TrackVTX,
												  *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
  
   RooFormulaVar* singleflavour_1VTX_1ST_5or6VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_nofrag").c_str(),
									 (hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_nofrag").c_str(),
									 "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
												     *singleflavour_e_1VTX,
												     *singleflavour_e_1Vtx_1SingleTrack,
												     *singleflavour_e_1VTX_5or6TrackVTX,
												     *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
 
    RooFormulaVar* singleflavour_1VTX_1ST_7orMoreVTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_nofrag").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_nofrag").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_1SingleTrack,
													*singleflavour_e_1VTX_7orMoreTrackVTX,
													*singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
    
    //2orMoreST
    
 
    RooFormulaVar* singleflavour_1VTX_2orMoreST_2VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_nofrag").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_nofrag").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_2orMoreSingleTrack,
													*singleflavour_e_1VTX_2TrackVTX,
													*singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag));
    
    
  
   RooFormulaVar* singleflavour_1VTX_2orMoreST_3VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_nofrag").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_nofrag").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_2orMoreSingleTrack,
													*singleflavour_e_1VTX_3TrackVTX,
													*singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag));
    
   
  RooFormulaVar* singleflavour_1VTX_2orMoreST_4VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_nofrag").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_nofrag").c_str(),
									    "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													*singleflavour_e_1VTX,
													*singleflavour_e_1Vtx_2orMoreSingleTrack,
													*singleflavour_e_1VTX_4TrackVTX,
													*singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
  
   RooFormulaVar* singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_nofrag").c_str(),
									       (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_nofrag").c_str(),
									       "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													   *singleflavour_e_1VTX,
													   *singleflavour_e_1Vtx_2orMoreSingleTrack,
													   *singleflavour_e_1VTX_5or6TrackVTX,
													   *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
   
  RooFormulaVar* singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_nofrag").c_str(),
										  (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_nofrag").c_str(),
										  "@0*@1*@2*@3*@4",RooArgList(*singleflavour_coef,
													      *singleflavour_e_1VTX,
													      *singleflavour_e_1Vtx_2orMoreSingleTrack,
													      *singleflavour_e_1VTX_7orMoreTrackVTX,
													      *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
    
    
    //2orMoreVTX
    
  
   RooFormulaVar* singleflavour_2orMoreVTX_4VTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_nofrag").c_str(),
									(hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_nofrag").c_str(),
									"@0*@1*@2*@3",RooArgList(*singleflavour_coef,
												 *singleflavour_e_2orMoreVTX,
												 *singleflavour_e_2orMoreVTX_4TrackVTX,
												 *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
 
    RooFormulaVar* singleflavour_2orMoreVTX_5orMoreVTX_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_nofrag").c_str(),
									      (hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_nofrag").c_str(),
									      "@0*@1*@2*@3",RooArgList(*singleflavour_coef,
												       *singleflavour_e_2orMoreVTX,
												       *singleflavour_e_2orMoreVTX_5orMoreTrackVTX,
												       *singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
    
    
    
    //coef for noVTX case
    
  
   RooFormulaVar* singleflavour_noVTX_1ST_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_noVTX_1ST_nofrag").c_str(),
								  (hypothesis+"_"+jetauthor+"_noVTX_1ST_nofrag").c_str(),
								  "@0*@1*@2*@3",RooArgList(*singleflavour_coef,
											   *singleflavour_e_NoVTX,
											   *singleflavour_e_NoVtx_1SingleTrack,
											   *singleflavour_e_NoVtx_1SingleTrack_energyFraction_nofrag));
    
  
   RooFormulaVar* singleflavour_noVTX_2ST_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_noVTX_2ST_nofrag").c_str(),
								  (hypothesis+"_"+jetauthor+"_noVTX_2ST_nofrag").c_str(),
								  "@0*@1*@2*@3",RooArgList(*singleflavour_coef,
											   *singleflavour_e_NoVTX,
											   *singleflavour_e_NoVtx_2SingleTrack,
											   *singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag));
    
 
    RooFormulaVar* singleflavour_noVTX_3orMoreST_nofrag_coef=new RooFormulaVar((hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_nofrag").c_str(),
									(hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_nofrag").c_str(),
									"@0*@1*@2*@3",RooArgList(*singleflavour_coef,
												 *singleflavour_e_NoVTX,
												 *singleflavour_e_NoVtx_3orMoreSingleTrack,
												 *singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag));
    
    
    //DUMMY PDF  
    Int_t singleflavour_noVTX_noSingleTrack_nbins(1);
    TArrayD singleflavour_noVTX_noSingleTrack_limits(singleflavour_noVTX_noSingleTrack_nbins+1);
    singleflavour_noVTX_noSingleTrack_limits[0]=0.;
    singleflavour_noVTX_noSingleTrack_limits[1]=1.00001;
    RooArgList* singleflavour_noVTX_noSingleTrack_list = new RooArgList((hypothesis+"_"+jetauthor+"_noVTX_noSingleTrack_list").c_str());
    RooParametricStepFunction* singleflavour_noVTX_noSingleTrack_pdf=new RooParametricStepFunction((hypothesis+"_"+jetauthor+"_noVTX_noSingleTrack_pdf").c_str(),
												   (hypothesis+"_"+jetauthor+"_noVTX_noSingleTrack_pdf").c_str(),
												   *significance3d,
												   *singleflavour_noVTX_noSingleTrack_list,
												   singleflavour_noVTX_noSingleTrack_limits,
												   singleflavour_noVTX_noSingleTrack_nbins);
    
  

    
    RooDataHist* singleflavour_noVTX_significance3d_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_noVTX_significance3d_histo").c_str(),
                                                                          (hypothesis+"_"+jetauthor+"_noVTX_significance3d_histo").c_str(),
                                                                          *significance3d,
                                                                          retrieveHistoFromFile(refFileName,hypothesis+"_noVTX_significance3d_histo"),
                                                                          1.0);
    
    histogramsVector->push_back(singleflavour_noVTX_significance3d_histo);
    
    RooHistPdf* singleflavour_noVTX_significance3d_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_noVTX_significance3d_pdf").c_str(),
                                                                      (hypothesis+"_"+jetauthor+"_noVTX_significance3d_pdf").c_str(),
                                                                      *significance3d,
                                                                      *singleflavour_noVTX_significance3d_histo);

    
    RooDataHist* singleflavour_noVTX_1SingleTrack_energyFraction_normal_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_noVTX_1SingleTrack_energyFraction_normal_histo").c_str(),
                                                                                              (hypothesis+"_"+jetauthor+"_noVTX_1SingleTrack_energyFraction_normal_histo").c_str(),
                                                                                              *energyFraction,
                                                                                              retrieveHistoFromFile(refFileName,hypothesis+"_noVTX_1SingleTrack_energyFraction_normal_histo"),
                                                                                              1.0);

    histogramsVector->push_back(singleflavour_noVTX_1SingleTrack_energyFraction_normal_histo);

    
    RooHistPdf* singleflavour_noVTX_1SingleTrack_energyFraction_normal_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_noVTX_1SingleTrack_energyFraction_normal_pdf").c_str(),
										   (hypothesis+"_"+jetauthor+"_noVTX_1SingleTrack_energyFraction_normal_pdf").c_str(),
										   *energyFraction,
										   *singleflavour_noVTX_1SingleTrack_energyFraction_normal_histo);

    
    
    RooDataHist* singleflavour_noVTX_2orMoreSingleTrack_energyFraction_normal_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_noVTX_2orMoreSingleTrack_energyFraction_normal_histo").c_str(),
											     (hypothesis+"_"+jetauthor+"_noVTX_2orMoreSingleTrack_energyFraction_normal_histo").c_str(),
											     *energyFraction,
											     retrieveHistoFromFile(refFileName,hypothesis+"_noVTX_2orMoreSingleTrack_energyFraction_normal_histo"),
											     1.0);

    histogramsVector->push_back(singleflavour_noVTX_2orMoreSingleTrack_energyFraction_normal_histo);
    
    
    
    RooHistPdf* singleflavour_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf").c_str(),
											 (hypothesis+"_"+jetauthor+"_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf").c_str(),
											 *energyFraction,
											 *singleflavour_noVTX_2orMoreSingleTrack_energyFraction_normal_histo);
    
    
    RooDataHist* singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_energyFraction_normal_histo").c_str(),
											 (hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_energyFraction_normal_histo").c_str(),
											 *energyFraction,
											 retrieveHistoFromFile(refFileName,hypothesis+"_1orMoreVTX_2TrackVTX_energyFraction_normal_histo"),
											 1.0);

    histogramsVector->push_back(singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_histo);
    

    RooHistPdf* singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf").c_str(),
										     (hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf").c_str(),
										     *energyFraction,
										     *singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_histo);
    
    
    RooDataHist* singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_1orMoreVTX_3TrackVTX_energyFraction_normal_histo").c_str(),
											 (hypothesis+"_"+jetauthor+"_1orMoreVTX_3TrackVTX_energyFraction_normal_histo").c_str(),
											 *energyFraction,
											 retrieveHistoFromFile(refFileName,hypothesis+"_1orMoreVTX_3TrackVTX_energyFraction_normal_histo"),
											 1.0);

    histogramsVector->push_back(singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_histo);

    
    RooHistPdf* singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf").c_str(),
										     (hypothesis+"_"+jetauthor+"_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf").c_str(),
										     *energyFraction,
										     *singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_histo);
    
    
    
    RooDataHist* singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo").c_str(),
											       (hypothesis+"_"+jetauthor+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo").c_str(),
											       *energyFraction,
											       retrieveHistoFromFile(refFileName,hypothesis+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo"),
											       1.0);

    histogramsVector->push_back(singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo);
    
    RooHistPdf* singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf").c_str(),
											   (hypothesis+"_"+jetauthor+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf").c_str(),
											   *energyFraction,
											   *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo);
    
    
    RooDataHist* singleflavour_1orMoreVTX_significance3d_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_1orMoreVTX_significance3d_histo").c_str(),
									(hypothesis+"_"+jetauthor+"_1orMoreVTX_significance3d_histo").c_str(),
									*significance3d,
									retrieveHistoFromFile(refFileName,hypothesis+"_1orMoreVTX_significance3d_histo"),
									1.0);

    histogramsVector->push_back(singleflavour_1orMoreVTX_significance3d_histo);

    
    RooHistPdf* singleflavour_1orMoreVTX_significance3d_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_1orMoreVTX_significance3d_pdf").c_str(),
								    (hypothesis+"_"+jetauthor+"_1orMoreVTX_significance3d_pdf").c_str(),
								    *significance3d,
								    *singleflavour_1orMoreVTX_significance3d_histo);
    
    
    RooDataHist* singleflavour_1orMoreVTX_2TrackVTX_mass_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_mass_histo").c_str(),
									(hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_mass_histo").c_str(),
									*mass,
									retrieveHistoFromFile(refFileName,hypothesis+"_1orMoreVTX_2TrackVTX_mass_histo"),
									1.0);

    histogramsVector->push_back(singleflavour_1orMoreVTX_2TrackVTX_mass_histo);
    
    
    RooHistPdf* singleflavour_1orMoreVTX_2TrackVTX_mass_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_mass_pdf").c_str(),
								    (hypothesis+"_"+jetauthor+"_1orMoreVTX_2TrackVTX_mass_pdf").c_str(),
								    *mass,
								    *singleflavour_1orMoreVTX_2TrackVTX_mass_histo);
    
    RooDataHist* singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_histo=new RooDataHist((hypothesis+"_"+jetauthor+"_1orMoreVTX_3orMoreTrackVTX_mass_histo").c_str(),
									      (hypothesis+"_"+jetauthor+"_1orMoreVTX_3orMoreTrackVTX_mass_histo").c_str(),
									      *mass,
									      retrieveHistoFromFile(refFileName,hypothesis+"_1orMoreVTX_3orMoreTrackVTX_mass_histo"),
									      1.0);
    
    histogramsVector->push_back(singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_histo);
    
    RooHistPdf* singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf=new RooHistPdf((hypothesis+"_"+jetauthor+"_1orMoreVTX_3orMoreTrackVTX_mass_pdf").c_str(),
									  (hypothesis+"_"+jetauthor+"_1orMoreVTX_3orMoreTrackVTX_mass_pdf").c_str(),
									  *mass,
									  *singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_histo);
    
    
    
    RooProdPdf* singleflavour_noVTX_0ST_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_noVTX_0ST_pdf").c_str(),
						    (hypothesis+"_"+jetauthor+"_noVTX_0ST_pdf").c_str(),
						    RooArgList(*singleflavour_noVTX_noSingleTrack_pdf));
    
    RooAddPdf* singleflavour_noVTX_0ST_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_noVTX_0ST_pdf_add").c_str(),
						      (hypothesis+"_"+jetauthor+"_noVTX_0ST_pdf_add").c_str(),
						      RooArgList(*singleflavour_noVTX_0ST_pdf),
						      RooArgList(*singleflavour_noVTX_0ST_coef));
    
    
    RooProdPdf* singleflavour_noVTX_1ST_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_noVTX_1ST_normal_pdf").c_str(),
							   (hypothesis+"_"+jetauthor+"_noVTX_1ST_normal_pdf").c_str(),
							   RooArgList(*singleflavour_noVTX_1SingleTrack_energyFraction_normal_pdf,
								      *singleflavour_noVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_noVTX_1ST_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_noVTX_1ST_normal_pdf_add").c_str(),
							     (hypothesis+"_"+jetauthor+"_noVTX_1ST_normal_pdf_add").c_str(),
							     RooArgList(*singleflavour_noVTX_1ST_normal_pdf),
							     RooArgList(*singleflavour_noVTX_1ST_normal_coef));
    
    RooProdPdf* singleflavour_noVTX_2ST_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_noVTX_2ST_normal_pdf").c_str(),
							   (hypothesis+"_"+jetauthor+"_noVTX_2ST_normal_pdf").c_str(),
							   RooArgList(*singleflavour_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf,
								      *singleflavour_noVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_noVTX_2ST_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_noVTX_2ST_normal_pdf_add").c_str(),
							     (hypothesis+"_"+jetauthor+"_noVTX_2ST_normal_pdf_add").c_str(),
							     RooArgList(*singleflavour_noVTX_2ST_normal_pdf),
							     RooArgList(*singleflavour_noVTX_2ST_normal_coef));
    
    RooProdPdf* singleflavour_noVTX_3orMoreST_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_normal_pdf").c_str(),
								 (hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_normal_pdf").c_str(),
								 RooArgList(*singleflavour_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf,
									    *singleflavour_noVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_noVTX_3orMoreST_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_normal_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_normal_pdf_add").c_str(),
								   RooArgList(*singleflavour_noVTX_3orMoreST_normal_pdf),
								   RooArgList(*singleflavour_noVTX_3orMoreST_normal_coef));
    
    
    //1
    
    RooProdPdf* singleflavour_1VTX_0ST_2VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_normal_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_normal_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_2TrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf,
									  *singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_2VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_normal_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_normal_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_0ST_2VTX_normal_pdf),
								 RooArgList(*singleflavour_1VTX_0ST_2VTX_normal_coef));
    
    RooProdPdf* singleflavour_1VTX_0ST_3VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_normal_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_normal_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf,
									  *singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_3VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_normal_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_normal_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_0ST_3VTX_normal_pdf),
								 RooArgList(*singleflavour_1VTX_0ST_3VTX_normal_coef));
    
    RooProdPdf* singleflavour_1VTX_0ST_4VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_normal_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_normal_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf,
									  *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_4VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_normal_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_normal_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_0ST_4VTX_normal_pdf),
								 RooArgList(*singleflavour_1VTX_0ST_4VTX_normal_coef));
    
    
    RooProdPdf* singleflavour_1VTX_0ST_5or6VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_normal_pdf").c_str(),
								  (hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_normal_pdf").c_str(),
								  RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									     *singleflavour_1orMoreVTX_significance3d_pdf,
									     *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_5or6VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_normal_pdf_add").c_str(),
								    (hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_normal_pdf_add").c_str(),
								    RooArgList(*singleflavour_1VTX_0ST_5or6VTX_normal_pdf),
								    RooArgList(*singleflavour_1VTX_0ST_5or6VTX_normal_coef));
  
    RooProdPdf* singleflavour_1VTX_0ST_7orMoreVTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_normal_pdf").c_str(),
								     (hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMore_pdf").c_str(),
								     RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										*singleflavour_1orMoreVTX_significance3d_pdf,
										*singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    
    RooAddPdf* singleflavour_1VTX_0ST_7orMoreVTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_normal_pdf_add").c_str(),
								       (hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_normal_pdf_add").c_str(),
								       RooArgList(*singleflavour_1VTX_0ST_7orMoreVTX_normal_pdf),
								       RooArgList(*singleflavour_1VTX_0ST_7orMoreVTX_normal_coef));
    
    
    
    //1ST
    
    RooProdPdf* singleflavour_1VTX_1ST_2VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_normal_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_normal_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_2TrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf,
									  *singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_2VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_normal_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_normal_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_1ST_2VTX_normal_pdf),
								 RooArgList(*singleflavour_1VTX_1ST_2VTX_normal_coef));
    
    RooProdPdf* singleflavour_1VTX_1ST_3VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_normal_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_normal_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf,
									  *singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_3VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_normal_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_normal_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_1ST_3VTX_normal_pdf),
								 RooArgList(*singleflavour_1VTX_1ST_3VTX_normal_coef));
  
    RooProdPdf* singleflavour_1VTX_1ST_4VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_normal_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_normal_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf,
									  *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_4VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_normal_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_normal_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_1ST_4VTX_normal_pdf),
								 RooArgList(*singleflavour_1VTX_1ST_4VTX_normal_coef));
    
    
    RooProdPdf* singleflavour_1VTX_1ST_5or6VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_normal_pdf").c_str(),
								  (hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_normal_pdf").c_str(),
								  RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									     *singleflavour_1orMoreVTX_significance3d_pdf,
									     *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_5or6VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_normal_pdf_add").c_str(),
								    (hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_normal_pdf_add").c_str(),
								    RooArgList(*singleflavour_1VTX_1ST_5or6VTX_normal_pdf),
								    RooArgList(*singleflavour_1VTX_1ST_5or6VTX_normal_coef));
    
    RooProdPdf* singleflavour_1VTX_1ST_7orMoreVTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_normal_pdf").c_str(),
								     (hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_normal_pdf").c_str(),
								     RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										*singleflavour_1orMoreVTX_significance3d_pdf,
										*singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    
    RooAddPdf* singleflavour_1VTX_1ST_7orMoreVTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_normal_pdf_add").c_str(),
								       (hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_normal_pdf_add").c_str(),
								       RooArgList(*singleflavour_1VTX_1ST_7orMoreVTX_normal_pdf),
								       RooArgList(*singleflavour_1VTX_1ST_7orMoreVTX_normal_coef));
    
    
    //2orMoreST
    
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_2VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_normal_pdf").c_str(),
								     (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_normal_pdf").c_str(),
								     RooArgList(*singleflavour_1orMoreVTX_2TrackVTX_mass_pdf,
										*singleflavour_1orMoreVTX_significance3d_pdf,
										*singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_2VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_normal_pdf_add").c_str(),
								       (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_normal_pdf_add").c_str(),
								       RooArgList(*singleflavour_1VTX_2orMoreST_2VTX_normal_pdf),
								       RooArgList(*singleflavour_1VTX_2orMoreST_2VTX_normal_coef));
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_3VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_normal_pdf").c_str(),
								     (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_normal_pdf").c_str(),
								     RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										*singleflavour_1orMoreVTX_significance3d_pdf,
										*singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_3VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_normal_pdf_add").c_str(),
								       (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_normal_pdf_add").c_str(),
								       RooArgList(*singleflavour_1VTX_2orMoreST_3VTX_normal_pdf),
								       RooArgList(*singleflavour_1VTX_2orMoreST_3VTX_normal_coef));
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_4VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_normal_pdf").c_str(),
								     (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_normal_pdf").c_str(),
								     RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										*singleflavour_1orMoreVTX_significance3d_pdf,
										*singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_4VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_normal_pdf_add").c_str(),
								       (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_normal_pdf_add").c_str(),
								       RooArgList(*singleflavour_1VTX_2orMoreST_4VTX_normal_pdf),
								       RooArgList(*singleflavour_1VTX_2orMoreST_4VTX_normal_coef));
    
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_5or6VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_normal_pdf").c_str(),
									(hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_normal_pdf").c_str(),
									RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										   *singleflavour_1orMoreVTX_significance3d_pdf,
										   *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_5or6VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_normal_pdf_add").c_str(),
									  (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_normal_pdf_add").c_str(),
									  RooArgList(*singleflavour_1VTX_2orMoreST_5or6VTX_normal_pdf),
									  RooArgList(*singleflavour_1VTX_2orMoreST_5or6VTX_normal_coef));
    
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_normal_pdf").c_str(),
									   (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_normal_pdf").c_str(),
									   RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										      *singleflavour_1orMoreVTX_significance3d_pdf,
										      *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_normal_pdf_add").c_str(),
									     (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_normal_pdf_add").c_str(),
									     RooArgList(*singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_pdf),
									     RooArgList(*singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_coef));
    
    
    
    //2orMore
    
    RooProdPdf* singleflavour_2orMoreVTX_4VTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_normal_pdf").c_str(),
								 (hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_normal_pdf").c_str(),
								 RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									    *singleflavour_1orMoreVTX_significance3d_pdf,
									    *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_2orMoreVTX_4VTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_normal_pdf_add").c_str(),
								   (hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_normal_pdf_add").c_str(),
								   RooArgList(*singleflavour_2orMoreVTX_4VTX_normal_pdf),
								   RooArgList(*singleflavour_2orMoreVTX_4VTX_normal_coef));
    
    
    
    RooProdPdf* singleflavour_2orMoreVTX_5orMoreVTX_normal_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_normal_pdf").c_str(),
								       (hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_normal_pdf").c_str(),
								       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										  *singleflavour_1orMoreVTX_significance3d_pdf,
										  *singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf));
    
    RooAddPdf* singleflavour_2orMoreVTX_5orMoreVTX_normal_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_normal_pdf_add").c_str(),
									 (hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_normal_pdf_add").c_str(),
									 RooArgList(*singleflavour_2orMoreVTX_5orMoreVTX_normal_pdf),
									 RooArgList(*singleflavour_2orMoreVTX_5orMoreVTX_normal_coef));
    
    //SAME FOR NOFRAG CASE...
    
    RooProdPdf* singleflavour_noVTX_1ST_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_noVTX_1ST_nofrag_pdf").c_str(),
							   (hypothesis+"_"+jetauthor+"_noVTX_1ST_nofrag_pdf").c_str(),
							   RooArgList(*singleflavour_noVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_noVTX_1ST_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_noVTX_1ST_nofrag_pdf_add").c_str(),
							     (hypothesis+"_"+jetauthor+"_noVTX_1ST_nofrag_pdf_add").c_str(),
							     RooArgList(*singleflavour_noVTX_1ST_nofrag_pdf),
							     RooArgList(*singleflavour_noVTX_1ST_nofrag_coef));
    
    RooProdPdf* singleflavour_noVTX_2ST_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_noVTX_2ST_nofrag_pdf").c_str(),
							   (hypothesis+"_"+jetauthor+"_noVTX_2ST_nofrag_pdf").c_str(),
							   RooArgList(*singleflavour_noVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_noVTX_2ST_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_noVTX_2ST_nofrag_pdf_add").c_str(),
							     (hypothesis+"_"+jetauthor+"_noVTX_2ST_nofrag_pdf_add").c_str(),
							     RooArgList(*singleflavour_noVTX_2ST_nofrag_pdf),
							     RooArgList(*singleflavour_noVTX_2ST_nofrag_coef));
    
    RooProdPdf* singleflavour_noVTX_3orMoreST_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_nofrag_pdf").c_str(),
								 (hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_nofrag_pdf").c_str(),
								 RooArgList(*singleflavour_noVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_noVTX_3orMoreST_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_nofrag_pdf_add").c_str(),
								   (hypothesis+"_"+jetauthor+"_noVTX_3orMoreST_nofrag_pdf_add").c_str(),
								   RooArgList(*singleflavour_noVTX_3orMoreST_nofrag_pdf),
								   RooArgList(*singleflavour_noVTX_3orMoreST_nofrag_coef));
    
    
    //1
    
    RooProdPdf* singleflavour_1VTX_0ST_2VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_nofrag_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_nofrag_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_2TrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_2VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_nofrag_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_0ST_2VTX_nofrag_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_0ST_2VTX_nofrag_pdf),
								 RooArgList(*singleflavour_1VTX_0ST_2VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_0ST_3VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_nofrag_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_nofrag_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_3VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_nofrag_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_0ST_3VTX_nofrag_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_0ST_3VTX_nofrag_pdf),
								 RooArgList(*singleflavour_1VTX_0ST_3VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_0ST_4VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_nofrag_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_nofrag_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_4VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_nofrag_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_0ST_4VTX_nofrag_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_0ST_4VTX_nofrag_pdf),
								 RooArgList(*singleflavour_1VTX_0ST_4VTX_nofrag_coef));
    
    
    RooProdPdf* singleflavour_1VTX_0ST_5or6VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_nofrag_pdf").c_str(),
								  (hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_nofrag_pdf").c_str(),
								  RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									     *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_0ST_5or6VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_nofrag_pdf_add").c_str(),
								    (hypothesis+"_"+jetauthor+"_1VTX_0ST_5or6VTX_nofrag_pdf_add").c_str(),
								    RooArgList(*singleflavour_1VTX_0ST_5or6VTX_nofrag_pdf),
								    RooArgList(*singleflavour_1VTX_0ST_5or6VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_0ST_7orMoreVTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_nofrag_pdf").c_str(),
								     (hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMore_pdf").c_str(),
								     RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										*singleflavour_1orMoreVTX_significance3d_pdf));
    
    
    RooAddPdf* singleflavour_1VTX_0ST_7orMoreVTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_nofrag_pdf_add").c_str(),
								       (hypothesis+"_"+jetauthor+"_1VTX_0ST_7orMoreVTX_nofrag_pdf_add").c_str(),
								       RooArgList(*singleflavour_1VTX_0ST_7orMoreVTX_nofrag_pdf),
								       RooArgList(*singleflavour_1VTX_0ST_7orMoreVTX_nofrag_coef));
    
    
    
    //1ST
    
    RooProdPdf* singleflavour_1VTX_1ST_2VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_nofrag_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_nofrag_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_2TrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_2VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_nofrag_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_1ST_2VTX_nofrag_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_1ST_2VTX_nofrag_pdf),
								 RooArgList(*singleflavour_1VTX_1ST_2VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_1ST_3VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_nofrag_pdf").c_str(),
							       (hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_nofrag_pdf").c_str(),
							       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
									  *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_3VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_nofrag_pdf_add").c_str(),
								 (hypothesis+"_"+jetauthor+"_1VTX_1ST_3VTX_nofrag_pdf_add").c_str(),
								 RooArgList(*singleflavour_1VTX_1ST_3VTX_nofrag_pdf),
								 RooArgList(*singleflavour_1VTX_1ST_3VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_1ST_4VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_nofrag_pdf").c_str(),
								      (hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_nofrag_pdf").c_str(),
								      RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										 *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_4VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_nofrag_pdf_add").c_str(),
									(hypothesis+"_"+jetauthor+"_1VTX_1ST_4VTX_nofrag_pdf_add").c_str(),
									RooArgList(*singleflavour_1VTX_1ST_4VTX_nofrag_pdf),
									RooArgList(*singleflavour_1VTX_1ST_4VTX_nofrag_coef));
    
    
    RooProdPdf* singleflavour_1VTX_1ST_5or6VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_nofrag_pdf").c_str(),
									 (hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_nofrag_pdf").c_str(),
									 RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										    *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_1ST_5or6VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_nofrag_pdf_add").c_str(),
									   (hypothesis+"_"+jetauthor+"_1VTX_1ST_5or6VTX_nofrag_pdf_add").c_str(),
									   RooArgList(*singleflavour_1VTX_1ST_5or6VTX_nofrag_pdf),
									   RooArgList(*singleflavour_1VTX_1ST_5or6VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_1ST_7orMoreVTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_nofrag_pdf").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_nofrag_pdf").c_str(),
									    RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										       *singleflavour_1orMoreVTX_significance3d_pdf));
    
    
    RooAddPdf* singleflavour_1VTX_1ST_7orMoreVTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_nofrag_pdf_add").c_str(),
									      (hypothesis+"_"+jetauthor+"_1VTX_1ST_7orMoreVTX_nofrag_pdf_add").c_str(),
									      RooArgList(*singleflavour_1VTX_1ST_7orMoreVTX_nofrag_pdf),
									      RooArgList(*singleflavour_1VTX_1ST_7orMoreVTX_nofrag_coef));
    
    
    //2orMoreST
    
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_2VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_nofrag_pdf").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_nofrag_pdf").c_str(),
									    RooArgList(*singleflavour_1orMoreVTX_2TrackVTX_mass_pdf,
										       *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_2VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_nofrag_pdf_add").c_str(),
									      (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_2VTX_nofrag_pdf_add").c_str(),
									      RooArgList(*singleflavour_1VTX_2orMoreST_2VTX_nofrag_pdf),
									      RooArgList(*singleflavour_1VTX_2orMoreST_2VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_3VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_nofrag_pdf").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_nofrag_pdf").c_str(),
									    RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										       *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_3VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_nofrag_pdf_add").c_str(),
									      (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_3VTX_nofrag_pdf_add").c_str(),
									      RooArgList(*singleflavour_1VTX_2orMoreST_3VTX_nofrag_pdf),
									      RooArgList(*singleflavour_1VTX_2orMoreST_3VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_4VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_nofrag_pdf").c_str(),
									    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_nofrag_pdf").c_str(),
									    RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										       *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_4VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_nofrag_pdf_add").c_str(),
									      (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_4VTX_nofrag_pdf_add").c_str(),
									      RooArgList(*singleflavour_1VTX_2orMoreST_4VTX_nofrag_pdf),
									      RooArgList(*singleflavour_1VTX_2orMoreST_4VTX_nofrag_coef));
    
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_nofrag_pdf").c_str(),
									       (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_nofrag_pdf").c_str(),
									       RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
											  *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_nofrag_pdf_add").c_str(),
										 (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_5or6VTX_nofrag_pdf_add").c_str(),
										 RooArgList(*singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_pdf),
										 RooArgList(*singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_coef));
    
    RooProdPdf* singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf").c_str(),
										  (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf").c_str(),
										  RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
											     *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf_add").c_str(),
										    (hypothesis+"_"+jetauthor+"_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf_add").c_str(),
										    RooArgList(*singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf),
										    RooArgList(*singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_coef));
    
    
    
    //2orMore
    
    RooProdPdf* singleflavour_2orMoreVTX_4VTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_nofrag_pdf").c_str(),
									(hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_nofrag_pdf").c_str(),
									RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
										   *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_2orMoreVTX_4VTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_nofrag_pdf_add").c_str(),
									  (hypothesis+"_"+jetauthor+"_2orMoreVTX_4VTX_nofrag_pdf_add").c_str(),
									  RooArgList(*singleflavour_2orMoreVTX_4VTX_nofrag_pdf),
									  RooArgList(*singleflavour_2orMoreVTX_4VTX_nofrag_coef));
    
    
    
    RooProdPdf* singleflavour_2orMoreVTX_5orMoreVTX_nofrag_pdf=new RooProdPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_nofrag_pdf").c_str(),
									      (hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_nofrag_pdf").c_str(),
									      RooArgList(*singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
											 *singleflavour_1orMoreVTX_significance3d_pdf));
    
    RooAddPdf* singleflavour_2orMoreVTX_5orMoreVTX_nofrag_pdf_add=new RooAddPdf((hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_nofrag_pdf_add").c_str(),
										(hypothesis+"_"+jetauthor+"_2orMoreVTX_5orMoreVTX_nofrag_pdf_add").c_str(),
										RooArgList(*singleflavour_2orMoreVTX_5orMoreVTX_nofrag_pdf),
										RooArgList(*singleflavour_2orMoreVTX_5orMoreVTX_nofrag_coef));
    
    
    
    

    //    RooSuperCategory* allCategoriesCombined=static_cast<RooSuperCategory*>(m_myRooVariables->find("allCategoriesCombined"));




    
    RooCategory* allCategoriesCombined_reduced=static_cast<RooCategory*>(m_myRooVariables->find("allCategoriesCombined_reduced"));

    
    RooSimultaneous* singleflavour_pdf=new RooSimultaneous((hypothesis+"_"+jetauthor+"_pdf").c_str(),
							   (hypothesis+"_"+jetauthor+"_pdf").c_str(),
							   *allCategoriesCombined_reduced);
    
    singleflavour_pdf->addPdf(*singleflavour_noVTX_0ST_pdf_add,"{NoVTX;NoSingleTrack}");
    singleflavour_pdf->addPdf(*singleflavour_noVTX_1ST_normal_pdf_add,"{NoVTX;1SingleTrack;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_noVTX_2ST_normal_pdf_add,"{NoVTX;2SingleTrack;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_noVTX_3orMoreST_normal_pdf_add,"{NoVTX;3orMoreSingleTrack;Normal}");
    
    
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_2VTX_normal_pdf_add,"{1VTX;NoSingleTrack;2TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_3VTX_normal_pdf_add,"{1VTX;NoSingleTrack;3TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_4VTX_normal_pdf_add,"{1VTX;NoSingleTrack;4TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_5or6VTX_normal_pdf_add,"{1VTX;NoSingleTrack;5or6TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_7orMoreVTX_normal_pdf_add,"{1VTX;NoSingleTrack;7orMoreTrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_2VTX_normal_pdf_add,"{1VTX;1SingleTrack;2TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_3VTX_normal_pdf_add,"{1VTX;1SingleTrack;3TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_4VTX_normal_pdf_add,"{1VTX;1SingleTrack;4TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_5or6VTX_normal_pdf_add,"{1VTX;1SingleTrack;5or6TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_7orMoreVTX_normal_pdf_add,"{1VTX;1SingleTrack;7orMoreTrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_2VTX_normal_pdf_add,"{1VTX;2orMoreSingleTrack;2TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_3VTX_normal_pdf_add,"{1VTX;2orMoreSingleTrack;3TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_4VTX_normal_pdf_add,"{1VTX;2orMoreSingleTrack;4TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_5or6VTX_normal_pdf_add,"{1VTX;2orMoreSingleTrack;5or6TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_pdf_add,"{1VTX;2orMoreSingleTrack;7orMoreTrackVTX;Normal}");
    
    singleflavour_pdf->addPdf(*singleflavour_2orMoreVTX_4VTX_normal_pdf_add,"{2orMoreVTX;4TrackVTX;Normal}");
    singleflavour_pdf->addPdf(*singleflavour_2orMoreVTX_5orMoreVTX_normal_pdf_add,"{2orMoreVTX;5orMoreTrackVTX;Normal}");
    
    
    singleflavour_pdf->addPdf(*singleflavour_noVTX_1ST_nofrag_pdf_add,"{NoVTX;1SingleTrack;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_noVTX_2ST_nofrag_pdf_add,"{NoVTX;2SingleTrack;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_noVTX_3orMoreST_nofrag_pdf_add,"{NoVTX;3orMoreSingleTrack;NoFrag}");
    
    
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_2VTX_nofrag_pdf_add,"{1VTX;NoSingleTrack;2TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_3VTX_nofrag_pdf_add,"{1VTX;NoSingleTrack;3TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_4VTX_nofrag_pdf_add,"{1VTX;NoSingleTrack;4TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_5or6VTX_nofrag_pdf_add,"{1VTX;NoSingleTrack;5or6TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_0ST_7orMoreVTX_nofrag_pdf_add,"{1VTX;NoSingleTrack;7orMoreTrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_2VTX_nofrag_pdf_add,"{1VTX;1SingleTrack;2TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_3VTX_nofrag_pdf_add,"{1VTX;1SingleTrack;3TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_4VTX_nofrag_pdf_add,"{1VTX;1SingleTrack;4TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_5or6VTX_nofrag_pdf_add,"{1VTX;1SingleTrack;5or6TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_1ST_7orMoreVTX_nofrag_pdf_add,"{1VTX;1SingleTrack;7orMoreTrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_2VTX_nofrag_pdf_add,"{1VTX;2orMoreSingleTrack;2TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_3VTX_nofrag_pdf_add,"{1VTX;2orMoreSingleTrack;3TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_4VTX_nofrag_pdf_add,"{1VTX;2orMoreSingleTrack;4TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_pdf_add,"{1VTX;2orMoreSingleTrack;5or6TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf_add,"{1VTX;2orMoreSingleTrack;7orMoreTrackVTX;NoFrag}");
    
    
    singleflavour_pdf->addPdf(*singleflavour_2orMoreVTX_4VTX_nofrag_pdf_add,"{2orMoreVTX;4TrackVTX;NoFrag}");
    singleflavour_pdf->addPdf(*singleflavour_2orMoreVTX_5orMoreVTX_nofrag_pdf_add,"{2orMoreVTX;5orMoreTrackVTX;NoFrag}");
    
    
    //    singleflavour_pdf->Print("v");


    RooArgSet* singleflavour_parameters=(RooArgSet*)singleflavour_pdf->getVariables();

    ATH_MSG_DEBUG(" Got pointer to the RooArgSet. print before " << singleflavour_parameters );
    

    if (msgLvl(MSG::VERBOSE))
    {
      ATH_MSG_DEBUG(" Initial coefficients set to ");
      singleflavour_parameters->Print("v");
    }
    
    //here store the names to load for the coefficients...    
    std::vector<TString> coefficientNamesToLoad;
    coefficientNamesToLoad.push_back(TString("coef"));
    coefficientNamesToLoad.push_back(TString("e_1VTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_2TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_3TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_4TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1VTX_5or6TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_1Vtx_1SingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_1Vtx_NoSingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_1orMoreVTX_2TrackVTX_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_1orMoreVTX_3TrackVTX_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_2orMoreVTX_4TrackVTX"));
    coefficientNamesToLoad.push_back(TString("e_NoVTX"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_1SingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_1SingleTrack_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_2SingleTrack"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_2orMoreSingleTrack_energyFraction_normal"));
    coefficientNamesToLoad.push_back(TString("e_NoVtx_NoSingleTrack"));
    
    //as a second step read the coefficients directly from the calibration histograms (1 histogram per coefficient)
    std::vector<TString>::const_iterator beginCoef=coefficientNamesToLoad.begin();
    std::vector<TString>::const_iterator endCoef=coefficientNamesToLoad.end();

    for (std::vector<TString>::const_iterator iterCoef=beginCoef;iterCoef!=endCoef;++iterCoef)
    {
      RooRealVar* myCoefficient=dynamic_cast<RooRealVar*>(singleflavour_parameters->find(hypothesis+"_"+jetauthor+"_"+*iterCoef));
      if (myCoefficient==0)
      {
        ATH_MSG_ERROR(" Cannot find coefficient" << hypothesis+"_"+jetauthor+"_"+*iterCoef);
      }
      else
      {
        std::string histoCoefName(hypothesis+"_"+*iterCoef);
        TH1* histo=(TH1*)retrieveHistoFromFile(refFileName,histoCoefName);
        if (histo!=0)
        {
          ATH_MSG_DEBUG(" Setting value of coefficient " << hypothesis+"_"+jetauthor+"_"+*iterCoef << " to " << histo->GetBinContent(1));
          myCoefficient->setVal(histo->GetBinContent(1));
        }
        else
        {
          ATH_MSG_WARNING("Failed setting value of coefficients. The first one is: " << hypothesis+"_"+jetauthor+"_"+*iterCoef);
//          ATH_MSG_ERROR("Coefficient information NOT in database. Abort loading calibration...");
//          return;
        }
      }
    }

    if (msgLvl(MSG::VERBOSE))
    {
      ATH_MSG_DEBUG(" After reading from COOL, coefficients set to ");
      singleflavour_parameters->Print("v");
    }


    RooRealVar* singleflavourcoef=(RooRealVar*)singleflavour_parameters->find((hypothesis+"_"+jetauthor+"_coef").c_str());
    singleflavourcoef->setVal(1);


    m_likelihoods[jetauthor+hypothesis]=singleflavour_pdf;
    ATH_MSG_DEBUG(" stored likelihood for string " << jetauthor+hypothesis);


    ATH_MSG_VERBOSE("deliting parameters ");
    delete singleflavour_parameters;
    ATH_MSG_VERBOSE("deleted");
    
//    likelihoodObjects->addOwned(* singleflavourcoef ); NO
//    likelihoodObjects->addOwned(* singleflavour_parameters ); //MAYBE...
    likelihoodObjects->addOwned(* singleflavour_pdf );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_5orMoreVTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_5orMoreVTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_4VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_4VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_4VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_4VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_3VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_3VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_2VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_2VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_7orMoreVTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_7orMoreVTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_5or6VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_5or6VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_4VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_4VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_3VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_3VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_2VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_2VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_7orMoreVTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_7orMoreVTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_5or6VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_5or6VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_4VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_4VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_3VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_3VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_2VTX_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_2VTX_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_3orMoreST_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_noVTX_3orMoreST_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_2ST_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_noVTX_2ST_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_1ST_nofrag_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_noVTX_1ST_nofrag_pdf );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_5orMoreVTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_5orMoreVTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_4VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_4VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_5or6VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_5or6VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_4VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_4VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_3VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_3VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_2VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_2VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_7orMoreVTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_7orMoreVTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_5or6VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_5or6VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_4VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_4VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_3VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_3VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_2VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_2VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_7orMoreVTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_7orMoreVTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_5or6VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_5or6VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_4VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_4VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_3VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_3VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_2VTX_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_2VTX_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_3orMoreST_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_noVTX_3orMoreST_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_2ST_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_noVTX_2ST_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_1ST_normal_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_noVTX_1ST_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_0ST_pdf_add );
    likelihoodObjects->addOwned(* singleflavour_noVTX_0ST_pdf );
    likelihoodObjects->addOwned(* singleflavour_1orMoreVTX_3orMoreTrackVTX_mass_pdf );
    likelihoodObjects->addOwned(* singleflavour_1orMoreVTX_2TrackVTX_mass_pdf );
    likelihoodObjects->addOwned(* singleflavour_1orMoreVTX_significance3d_pdf );
    likelihoodObjects->addOwned(* singleflavour_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_1SingleTrack_energyFraction_normal_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_significance3d_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_noSingleTrack_pdf );
    likelihoodObjects->addOwned(* singleflavour_noVTX_noSingleTrack_list  );
    likelihoodObjects->addOwned(* singleflavour_noVTX_3orMoreST_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_noVTX_2ST_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_noVTX_1ST_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_5orMoreVTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_4VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_7orMoreVTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_5or6VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_4VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_3VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_2VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_7orMoreVTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_5or6VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_4VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_3VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_2VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_7orMoreVTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_5or6VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_4VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_3VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_2VTX_nofrag_coef );
    likelihoodObjects->addOwned(* singleflavour_noVTX_3orMoreST_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_noVTX_2ST_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_noVTX_1ST_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_noVTX_0ST_coef );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_3orMoreSingleTrack );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_2SingleTrack );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_1SingleTrack );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_NoSingleTrack );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_5orMoreVTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_2orMoreVTX_4VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_7orMoreVTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_5or6VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_4VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_3VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_2orMoreST_2VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_7orMoreVTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_5or6VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_4VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_3VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_1ST_2VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_7orMoreVTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_5or6VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_4VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_3VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_1VTX_0ST_2VTX_normal_coef );
    likelihoodObjects->addOwned(* singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag );
    likelihoodObjects->addOwned(* singleflavour_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal );
    likelihoodObjects->addOwned(* singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag );
    likelihoodObjects->addOwned(* singleflavour_e_1orMoreVTX_3TrackVTX_energyFraction_normal );
    likelihoodObjects->addOwned(* singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag );
    likelihoodObjects->addOwned(* singleflavour_e_1orMoreVTX_2TrackVTX_energyFraction_normal );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_2orMoreSingleTrack_energyFraction_normal );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_1SingleTrack_energyFraction_nofrag );
    likelihoodObjects->addOwned(* singleflavour_e_NoVtx_1SingleTrack_energyFraction_normal );
    likelihoodObjects->addOwned(* singleflavour_e_2orMoreVTX_5orMoreTrackVTX );
    likelihoodObjects->addOwned(* singleflavour_e_2orMoreVTX_4TrackVTX );
    likelihoodObjects->addOwned(* singleflavour_e_1Vtx_2orMoreSingleTrack );
    likelihoodObjects->addOwned(* singleflavour_e_1Vtx_1SingleTrack );
    likelihoodObjects->addOwned(* singleflavour_e_1Vtx_NoSingleTrack );
    likelihoodObjects->addOwned(* singleflavour_e_1VTX_7orMoreTrackVTX  );
    likelihoodObjects->addOwned(* singleflavour_e_1VTX_5or6TrackVTX );
    likelihoodObjects->addOwned(* singleflavour_e_1VTX_4TrackVTX );
    likelihoodObjects->addOwned(* singleflavour_e_1VTX_3TrackVTX );
    likelihoodObjects->addOwned(* singleflavour_e_1VTX_2TrackVTX );
    likelihoodObjects->addOwned(* singleflavour_e_2orMoreVTX );
    likelihoodObjects->addOwned(* singleflavour_e_1VTX );
    likelihoodObjects->addOwned(* singleflavour_e_NoVTX );
    likelihoodObjects->addOwned(* singleflavour_coef );


  }

  bool JetFitterLikelihood::isCalibrationUpdated(const std::string& jetauthor,
                                                 const std::string& histoName)
  {
    TString directory(m_calibrationSubDirectory);
    directory+="/";
    
    TString nameOfHisto(directory);
    nameOfHisto+=histoName;
    
    ATH_MSG_DEBUG(" Trying to retrieve histogram with name: " << nameOfHisto << 
        " to check if the calibration is updated ");
    
    std::pair<TH1*, bool> histo=m_calibrationTool->retrieveHistogram(m_calibrationDirectory, 
                                                                     jetauthor, 
                                                                     std::string((const char*)(nameOfHisto)));

    if (histo.first==0)
    {
      ATH_MSG_ERROR(" Cannot retrieve histogram for update check: " << std::string((const char*)(nameOfHisto)));
    }

    return histo.second;
  }
  
  

  TH1* JetFitterLikelihood::retrieveHistoFromFile(const std::string& jetauthor, 
						 const std::string& histoName) {
    TString directory(m_calibrationSubDirectory);
    directory+="/";

    TString nameOfHisto(directory);
    nameOfHisto+=histoName;

    ATH_MSG_DEBUG(" Trying to retrieve histogram with name: " << nameOfHisto);

    std::pair<TH1*, bool> histo=m_calibrationTool->retrieveHistogram(m_calibrationDirectory, 
                                                                     jetauthor, 
                                                                     std::string((const char*)(nameOfHisto)));

    m_calibrationTool->updateHistogramStatus(m_calibrationDirectory, 
                                             jetauthor, 
                                             std::string((const char*)(nameOfHisto)),
                                             false);
    
    return histo.first;

  }


  void JetFitterLikelihood
  ::fillLikelihoodValues(IJetFitterTagInfo & myTagInfo,
			 const std::string & jetauthor,
			 double /* jetpT */,
			 double /* jeteta */,
			 double /* IP3dlike=-5000 */) { 
    
    for (std::vector<std::string>::const_iterator 
	   hyp_itr = m_hypothesis.begin(); 
       hyp_itr != m_hypothesis.end(); 
       hyp_itr++) { 
      fillLikelihoodValues(myTagInfo, jetauthor, *hyp_itr);
    }

  }


  void JetFitterLikelihood
  ::fillLikelihoodValues(xAOD::BTagging* BTag,
			 const std::string & jetauthor,
			 double /* jetpT */,
			 double /* jeteta */,
			 double /* IP3dlike=-5000 */) { 
    
    for (std::vector<std::string>::const_iterator 
	   hyp_itr = m_hypothesis.begin(); 
       hyp_itr != m_hypothesis.end(); 
       hyp_itr++) { 
      fillLikelihoodValues(BTag, jetauthor, *hyp_itr);
    }

  }


void JetFitterLikelihood::fillLikelihoodValues(IJetFitterTagInfo & myTagInfo,
					       const std::string & jetauthor,
					       const std::string & hypothesis) {
 
  if (hypothesis==""||jetauthor=="") {
    ATH_MSG_WARNING(" Hypothesis or jetauthor is empty. No likelihood value given back. " );
  }

  loadCalibration(jetauthor,hypothesis);

  //first get the right instance of the fit
  RooSimultaneous* myLikelihood=m_likelihoods[jetauthor+hypothesis];
  
  ATH_MSG_DEBUG(" Trying to read back likelihood with string " << jetauthor+hypothesis << " gives pointer " << myLikelihood );

  if (myLikelihood==0) {
    ATH_MSG_WARNING(" No Likelihood defined for the string " << jetauthor+hypothesis);
    return;
  }

  //  std::cout << " the pointer is : " << myLikelihood << " for string " << jetauthor+hypothesis );

  //  myLikelihood->Print("v");


  //get the variables you need
  RooCategory *cat_nVTX=static_cast<RooCategory*>(m_myRooVariables->find("cat_nVTX"));
  RooCategory *cat_nTracksAtVtx=static_cast<RooCategory*>(m_myRooVariables->find("cat_nTracksAtVtx"));
  RooCategory *cat_nSingleTracks=static_cast<RooCategory*>(m_myRooVariables->find("cat_nSingleTracks"));

  RooAbsCategory* mycat_nVTX_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("mycat_nVTX_nolvalue"));
  RooCategory* mycat_nVTX=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nVTX"));

  RooAbsCategory* mycat_nTracksAtVtx6Bin_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("mycat_nTracksAtVtx6Bin_nolvalue"));
  RooCategory* mycat_nTracksAtVtx6Bin=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nTracksAtVtx6Bin"));

  RooAbsCategory* mycat_nSingleTracks_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("mycat_nSingleTracks_nolvalue"));
  RooCategory* mycat_nSingleTracks=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nSingleTracks"));

  RooRealVar* energyFraction=static_cast<RooRealVar*>(m_myRooVariables->find("energyFraction"));
  RooRealVar* significance3d=static_cast<RooRealVar*>(m_myRooVariables->find("significance3d"));
  RooRealVar* mass=static_cast<RooRealVar*>(m_myRooVariables->find("mass"));

  RooAbsCategory* energyFractionCat_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("energyFractionCat_nolvalue"));
  RooCategory* energyFractionCat=static_cast<RooCategory*>(m_myRooVariables->find("energyFractionCat"));

  //  RooSuperCategory* allCategoriesCombined=static_cast<RooSuperCategory*>(m_myRooVariables->find("allCategoriesCombined"));
  
  RooAbsCategory* allCategoriesCombined_reduced_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("allCategoriesCombined_reduced_nolvalue"));
  RooCategory* allCategoriesCombined_reduced=static_cast<RooCategory*>(m_myRooVariables->find("allCategoriesCombined_reduced"));
  
  //set them to the right values
  cat_nVTX->setIndex(myTagInfo.nVTX());

  cat_nTracksAtVtx->setIndex(myTagInfo.nTracksAtVtx());

  cat_nSingleTracks->setIndex(myTagInfo.nSingleTracks());

  mycat_nVTX->setIndex(mycat_nVTX_nolvalue->getIndex());

  mycat_nTracksAtVtx6Bin->setIndex(mycat_nTracksAtVtx6Bin_nolvalue->getIndex());

  mycat_nSingleTracks->setIndex(mycat_nSingleTracks_nolvalue->getIndex());
  
  energyFraction->setVal(myTagInfo.energyFraction());

  significance3d->setVal(myTagInfo.significance3d());

  mass->setVal(myTagInfo.mass());
  /*
  std::cout << "cat_nVTX " << cat_nVTX->getLabel() << " " << cat_nVTX->getIndex() << " cat_nTracksAtVtx " << 
    cat_nTracksAtVtx->getLabel() << " " << cat_nTracksAtVtx->getIndex() << " " << 
    " cat_nSingleTracks " << cat_nSingleTracks->getLabel() << " " << cat_nSingleTracks->getIndex() << 
    " mycat_nVTX_nolvalue " <<     mycat_nVTX_nolvalue->getLabel() << " " << mycat_nVTX_nolvalue->getIndex() << 
    " mycat_nVTX " <<     mycat_nVTX->getLabel() << " " << mycat_nVTX->getIndex() << 
    " mycat_nTracksAtVtx6Bin_nolvalue " <<     mycat_nTracksAtVtx6Bin_nolvalue->getLabel() << " " << mycat_nTracksAtVtx6Bin_nolvalue->getIndex() << 
    " mycat_nTracksAtVtx6Bin " <<     mycat_nTracksAtVtx6Bin->getLabel() << " " << mycat_nTracksAtVtx6Bin->getIndex() << 
    " mycat_nSingleTracks_nolvalue " <<     mycat_nSingleTracks_nolvalue->getLabel() << " " << mycat_nSingleTracks_nolvalue->getIndex() << 
    " mycat_nSingleTracks " <<     mycat_nSingleTracks->getLabel() << " " << mycat_nSingleTracks->getIndex() << std::endl;
  */

  if (msgLvl(MSG::VERBOSE))
  {
    ATH_MSG_VERBOSE("nVTX: " << cat_nVTX->getIndex() << " nTracksAtVtx: " << cat_nTracksAtVtx->getIndex() << " nSingleTracks: " << cat_nSingleTracks->getIndex());
    ATH_MSG_VERBOSE("energyFraction " << energyFraction->getVal());
    ATH_MSG_VERBOSE("significance3d " << significance3d->getVal());
    ATH_MSG_VERBOSE("mass " << mass->getVal());
  }
  

  energyFractionCat->setIndex(energyFractionCat_nolvalue->getIndex());
  //  energyFractionCat->Print("v");

  /*
  std::cout << " nolvalue : " << energyFractionCat_nolvalue->getIndex() << " " << energyFractionCat_nolvalue->getLabel() << 
    " energyFractionCat " << " " << energyFractionCat->getIndex() << " " << energyFractionCat->getLabel() << std::endl;
  */  
  allCategoriesCombined_reduced->setIndex(allCategoriesCombined_reduced_nolvalue->getIndex());
  //  allCategoriesCombined_reduced->Print("v");
  /*
  std::cout << " nolvalue : " << allCategoriesCombined_reduced_nolvalue->getIndex() << " " << allCategoriesCombined_reduced_nolvalue->getLabel() << 
    " allCategoriesCombined_reduced " << " " << allCategoriesCombined_reduced->getIndex() << " " << allCategoriesCombined_reduced->getLabel() << std::endl;
  */
  //obtain the likelihood value (weight)
  
  double expectedEvents=myLikelihood->expectedEvents(RooArgSet(*significance3d,
                                                               *mass,
                                                               *energyFraction));

  double val=myLikelihood->getVal();

  double weight=expectedEvents*val;


  /*
  ATH_MSG_DEBUG( "Expected fraction: " << myLikelihood->expectedEvents(RooArgSet(*significance3d,
										       *mass,
										       *energyFraction))
       << " expected weight: " << myLikelihood->getVal() << " product: " << weight);

  std::cout<< "Expected fraction: " << myLikelihood->expectedEvents(RooArgSet(*significance3d,
									      *mass,
									      *energyFraction))
	   << " expected weight: " << myLikelihood->getVal() << " product: " << weight << std::endl;
  */

  ATH_MSG_DEBUG( "val: " << val << " expected Events: " << expectedEvents << " weight " << weight);

  std::vector<double> weights=myTagInfo.tagLikelihood();
  
  if (weights.size()!=3) {
    if (weights.size()==0) {
      weights.push_back(0);
      weights.push_back(1);
      weights.push_back(2);
    } else {
      ATH_MSG_WARNING("Strange: weight size is between 0 and 2...");
    }
  }
  
  //store it in the JetFitterTagInfo object (hypothesis [hypothesis])
  if (hypothesis=="bottom") {
    weights[0]=weight;
  } else if (hypothesis=="light") {
    weights[1]=weight;
  } else if (hypothesis=="charm") {
    weights[2]=weight;
  } else {
    ATH_MSG_WARNING("Hypothesis different than Sig and Bkg not yet supported" );
  }
  
  myTagInfo.setTagLikelihood(weights);
  myTagInfo.makeValid(); 
}


void JetFitterLikelihood::fillLikelihoodValues(xAOD::BTagging* BTag,
					       const std::string & jetauthor,
					       const std::string & hypothesis) {
 
  if (hypothesis==""||jetauthor=="") {
    ATH_MSG_WARNING(" Hypothesis or jetauthor is empty. No likelihood value given back. " );
  }

  loadCalibration(jetauthor,hypothesis);

  //first get the right instance of the fit
  RooSimultaneous* myLikelihood=m_likelihoods[jetauthor+hypothesis];
  
  ATH_MSG_DEBUG(" Trying to read back likelihood with string " << jetauthor+hypothesis << " gives pointer " << myLikelihood );

  if (myLikelihood==0) {
    ATH_MSG_WARNING(" No Likelihood defined for the string " << jetauthor+hypothesis);
    return;
  }

  //  std::cout << " the pointer is : " << myLikelihood << " for string " << jetauthor+hypothesis );

  //  myLikelihood->Print("v");


  //get the variables you need
  RooCategory *cat_nVTX=static_cast<RooCategory*>(m_myRooVariables->find("cat_nVTX"));
  RooCategory *cat_nTracksAtVtx=static_cast<RooCategory*>(m_myRooVariables->find("cat_nTracksAtVtx"));
  RooCategory *cat_nSingleTracks=static_cast<RooCategory*>(m_myRooVariables->find("cat_nSingleTracks"));

  RooAbsCategory* mycat_nVTX_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("mycat_nVTX_nolvalue"));
  RooCategory* mycat_nVTX=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nVTX"));

  RooAbsCategory* mycat_nTracksAtVtx6Bin_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("mycat_nTracksAtVtx6Bin_nolvalue"));
  RooCategory* mycat_nTracksAtVtx6Bin=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nTracksAtVtx6Bin"));

  RooAbsCategory* mycat_nSingleTracks_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("mycat_nSingleTracks_nolvalue"));
  RooCategory* mycat_nSingleTracks=static_cast<RooCategory*>(m_myRooVariables->find("mycat_nSingleTracks"));

  RooRealVar* energyFraction=static_cast<RooRealVar*>(m_myRooVariables->find("energyFraction"));
  RooRealVar* significance3d=static_cast<RooRealVar*>(m_myRooVariables->find("significance3d"));
  RooRealVar* mass=static_cast<RooRealVar*>(m_myRooVariables->find("mass"));

  RooAbsCategory* energyFractionCat_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("energyFractionCat_nolvalue"));
  RooCategory* energyFractionCat=static_cast<RooCategory*>(m_myRooVariables->find("energyFractionCat"));

  //  RooSuperCategory* allCategoriesCombined=static_cast<RooSuperCategory*>(m_myRooVariables->find("allCategoriesCombined"));
  
  RooAbsCategory* allCategoriesCombined_reduced_nolvalue=static_cast<RooAbsCategory*>(m_myRooVariables->find("allCategoriesCombined_reduced_nolvalue"));
  RooCategory* allCategoriesCombined_reduced=static_cast<RooCategory*>(m_myRooVariables->find("allCategoriesCombined_reduced"));

  int i_nVTX, i_nTracksAtVtx, i_nSingleTracks;
  float f_energyFraction, f_mass, f_significance3d;
  
  bool status = true;

  status &= BTag->taggerInfo(i_nVTX, xAOD::BTagInfo::JetFitter_nVTX);
  status &= BTag->taggerInfo(i_nTracksAtVtx, xAOD::BTagInfo::JetFitter_nTracksAtVtx);
  status &= BTag->taggerInfo(i_nSingleTracks, xAOD::BTagInfo::JetFitter_nSingleTracks);
  status &= BTag->taggerInfo(f_energyFraction, xAOD::BTagInfo::JetFitter_energyFraction);
  status &= BTag->taggerInfo(f_mass, xAOD::BTagInfo::JetFitter_mass);
  status &= BTag->taggerInfo(f_significance3d, xAOD::BTagInfo::JetFitter_significance3d);

  if (!status) {
    ATH_MSG_WARNING("Error retrieving input variables; calculations will be incorrect!");
  }

  //set them to the right values
  cat_nVTX->setIndex(i_nVTX);

  cat_nTracksAtVtx->setIndex(i_nTracksAtVtx);

  cat_nSingleTracks->setIndex(i_nSingleTracks);

  mycat_nVTX->setIndex(mycat_nVTX_nolvalue->getIndex());

  mycat_nTracksAtVtx6Bin->setIndex(mycat_nTracksAtVtx6Bin_nolvalue->getIndex());

  mycat_nSingleTracks->setIndex(mycat_nSingleTracks_nolvalue->getIndex());
  
  energyFraction->setVal(f_energyFraction);

  significance3d->setVal(f_significance3d);

  mass->setVal(f_mass);
  /*
  std::cout << "cat_nVTX " << cat_nVTX->getLabel() << " " << cat_nVTX->getIndex() << " cat_nTracksAtVtx " << 
    cat_nTracksAtVtx->getLabel() << " " << cat_nTracksAtVtx->getIndex() << " " << 
    " cat_nSingleTracks " << cat_nSingleTracks->getLabel() << " " << cat_nSingleTracks->getIndex() << 
    " mycat_nVTX_nolvalue " <<     mycat_nVTX_nolvalue->getLabel() << " " << mycat_nVTX_nolvalue->getIndex() << 
    " mycat_nVTX " <<     mycat_nVTX->getLabel() << " " << mycat_nVTX->getIndex() << 
    " mycat_nTracksAtVtx6Bin_nolvalue " <<     mycat_nTracksAtVtx6Bin_nolvalue->getLabel() << " " << mycat_nTracksAtVtx6Bin_nolvalue->getIndex() << 
    " mycat_nTracksAtVtx6Bin " <<     mycat_nTracksAtVtx6Bin->getLabel() << " " << mycat_nTracksAtVtx6Bin->getIndex() << 
    " mycat_nSingleTracks_nolvalue " <<     mycat_nSingleTracks_nolvalue->getLabel() << " " << mycat_nSingleTracks_nolvalue->getIndex() << 
    " mycat_nSingleTracks " <<     mycat_nSingleTracks->getLabel() << " " << mycat_nSingleTracks->getIndex() << std::endl;
  */

  if (msgLvl(MSG::VERBOSE))
  {
    ATH_MSG_VERBOSE("nVTX: " << cat_nVTX->getIndex() << " nTracksAtVtx: " << cat_nTracksAtVtx->getIndex() << " nSingleTracks: " << cat_nSingleTracks->getIndex());
    ATH_MSG_VERBOSE("energyFraction " << energyFraction->getVal());
    ATH_MSG_VERBOSE("significance3d " << significance3d->getVal());
    ATH_MSG_VERBOSE("mass " << mass->getVal());
  }
  

  energyFractionCat->setIndex(energyFractionCat_nolvalue->getIndex());
  //  energyFractionCat->Print("v");

  /*
  std::cout << " nolvalue : " << energyFractionCat_nolvalue->getIndex() << " " << energyFractionCat_nolvalue->getLabel() << 
    " energyFractionCat " << " " << energyFractionCat->getIndex() << " " << energyFractionCat->getLabel() << std::endl;
  */  
  allCategoriesCombined_reduced->setIndex(allCategoriesCombined_reduced_nolvalue->getIndex());
  //  allCategoriesCombined_reduced->Print("v");
  /*
  std::cout << " nolvalue : " << allCategoriesCombined_reduced_nolvalue->getIndex() << " " << allCategoriesCombined_reduced_nolvalue->getLabel() << 
    " allCategoriesCombined_reduced " << " " << allCategoriesCombined_reduced->getIndex() << " " << allCategoriesCombined_reduced->getLabel() << std::endl;
  */
  //obtain the likelihood value (weight)
  
  double expectedEvents=myLikelihood->expectedEvents(RooArgSet(*significance3d,
                                                               *mass,
                                                               *energyFraction));

  double val=myLikelihood->getVal();

  double weight=expectedEvents*val;


  /*
  ATH_MSG_DEBUG( "Expected fraction: " << myLikelihood->expectedEvents(RooArgSet(*significance3d,
										       *mass,
										       *energyFraction))
       << " expected weight: " << myLikelihood->getVal() << " product: " << weight);

  std::cout<< "Expected fraction: " << myLikelihood->expectedEvents(RooArgSet(*significance3d,
									      *mass,
									      *energyFraction))
	   << " expected weight: " << myLikelihood->getVal() << " product: " << weight << std::endl;
  */

  ATH_MSG_DEBUG( "val: " << val << " expected Events: " << expectedEvents << " weight " << weight);

  if (hypothesis == "bottom")
  {
    BTag->setJetFitter_pb(weight);
  }
  else if (hypothesis == "light")
  {
    BTag->setJetFitter_pu(weight);
  }
  else if (hypothesis == "charm")
  {
    BTag->setJetFitter_pc(weight);
  }
}


}//end Analysis namespace

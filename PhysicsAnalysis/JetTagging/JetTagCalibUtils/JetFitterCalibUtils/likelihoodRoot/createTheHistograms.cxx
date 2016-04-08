/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "createTheHistograms.h"

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
#include <RooDataSet.h>
#include <RooHistPdf.h>
#include <RooNDKeysPdf.h>
#include <RooBinning.h>
#include <RooCmdArg.h>
#include <RooGlobalFunc.h>
#include <Roo1DTable.h>
#include <RooPlot.h>

#include <TH1.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TCanvas.h>

using namespace RooFit;
using namespace std;

TH1F* makeHisto(RooNDKeysPdf* myKeysPdf,const char* name,int numtot,double begin,double end,RooRealVar* dep) {

  TH1F* newhisto=new TH1F(name,name,numtot,begin,end);

  int nbins=newhisto->GetNbinsX();

  for (int ss=1;ss<nbins+1;++ss) {
    double center=newhisto->GetBinCenter(ss);
    double width=newhisto->GetBinWidth(ss);
//    std::cout << "Center is " << center << std::endl;
    dep->setRange("selection",center-width/2.,center+width/2.) ;
    RooAbsReal* fracInt = myKeysPdf->createIntegral(*dep,RooFit::Range("selection")) ;
//    std::cout << " Evaluated to " << fracInt->getVal() << std::endl;
    newhisto->Fill(center,fracInt->getVal());
    delete fracInt;
  }
  return newhisto;
}
//RooAbsPdf* createTheFit() {
void createTheHistograms(
                         TString collectionName,
                         int flavourType)
{

  TString flavourString;

  if (flavourType==5)
  {
    flavourString=TString("bottom");
  }
  if (flavourType==4)
  {
    flavourString=TString("charm");
  }
  if (flavourType==1)
  {
    flavourString=TString("light");
  }

  TString identificationFile("../trainingResultsLikelihood/plots/");
  identificationFile+=collectionName;
  identificationFile+="_";
  identificationFile+=flavourString;

  RooCategory *cat_flavour=new RooCategory("cat_flavour","jet flavour");
  cat_flavour->defineType("b",5);
  cat_flavour->defineType("c",4);
  cat_flavour->defineType("l",1);
  

  RooCategory *cat_nVTX=new RooCategory("nVTX","number of vertices category");
  cat_nVTX->defineType("NoVTX",0);
  cat_nVTX->defineType("1VTX",1);
  cat_nVTX->defineType("2VTX",2);
  cat_nVTX->defineType("3VTX",3);
  cat_nVTX->defineType("4VTX",4);
  cat_nVTX->defineType("5VTX",5);//I hope not to need more!!!

  RooCategory* cat_nTracksAtVtx=new RooCategory("nTracksAtVtx","number of tracks at Vertices");
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


  RooCategory* cat_nSingleTracks=new RooCategory("nSingleTracks","number of single significant tracks");
  cat_nSingleTracks->defineType("NoSingleTrack",0);
  cat_nSingleTracks->defineType("1SingleTrack",1);
  cat_nSingleTracks->defineType("2SingleTrack",2);
  cat_nSingleTracks->defineType("3SingleTrack",3);
  cat_nSingleTracks->defineType("4SingleTrack",4);
  cat_nSingleTracks->defineType("5SingleTrack",5);
  cat_nSingleTracks->defineType("6SingleTrack",6);
  cat_nSingleTracks->defineType("7SingleTrack",7);
  
  RooMappedCategory* mycat_nVTX=new RooMappedCategory("mycat_nVTX","number of vertices categories",*cat_nVTX);//,"Undefined");
  mycat_nVTX->map("NoVTX","NoVTX");
  mycat_nVTX->map("1VTX","1VTX");
  mycat_nVTX->map("2VTX","2orMoreVTX");
  mycat_nVTX->map("3VTX","2orMoreVTX");
  mycat_nVTX->map("4VTX","2orMoreVTX");
  mycat_nVTX->map("5VTX","2orMoreVTX");
  

  RooMappedCategory* mycat_nTracksAtVtx6Bin=
    new RooMappedCategory("mycat_nTracksAtVtx6Bin","number of tracks at vertices category",*cat_nTracksAtVtx);//,"Undefined");
  mycat_nTracksAtVtx6Bin->map("NoTrackVTX","NoTrackVTX");
  mycat_nTracksAtVtx6Bin->map("2TrackVTX","2TrackVTX");
  mycat_nTracksAtVtx6Bin->map("3TrackVTX","3TrackVTX");
  mycat_nTracksAtVtx6Bin->map("4TrackVTX","4TrackVTX");
  mycat_nTracksAtVtx6Bin->map("5TrackVTX","5or6TrackVTX");
  mycat_nTracksAtVtx6Bin->map("6TrackVTX","5or6TrackVTX");
  mycat_nTracksAtVtx6Bin->map("7TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("8TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("9TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("10TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("11TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("12TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("13TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("14TrackVTX","7orMoreTrackVTX");
  mycat_nTracksAtVtx6Bin->map("15TrackVTX","7orMoreTrackVTX");

   RooMappedCategory* mycat_nTracksAtVtx4Bin=
     new RooMappedCategory("mycat_nTracksAtVtx4Bin","number of tracks at vertices category",*cat_nTracksAtVtx);//,"Undefined");
  mycat_nTracksAtVtx4Bin->map("NoTrackVTX","NoTrackVTX");
  //mycat_nTracksAtVtx4Bin->map("NoTrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("2TrackVTX","2TrackVTX");
  mycat_nTracksAtVtx4Bin->map("3TrackVTX","3TrackVTX");
  mycat_nTracksAtVtx4Bin->map("4TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("5TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("6TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("7TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("8TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("9TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("10TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("11TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("12TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("13TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("14TrackVTX","4orMoreTrackVTX");
  mycat_nTracksAtVtx4Bin->map("15TrackVTX","4orMoreTrackVTX");

  RooMappedCategory* mycat_nTracksAtVtx3Bin=
    new RooMappedCategory("mycat_nTracksAtVtx3Bin","number of tracks at vertices category",*cat_nTracksAtVtx);//,"Undefined");
  mycat_nTracksAtVtx3Bin->map("NoTrackVTX","NoTrackVTX");
  mycat_nTracksAtVtx3Bin->map("2TrackVTX","2TrackVTX");
  mycat_nTracksAtVtx3Bin->map("3TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("4TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("5TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("6TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("7TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("8TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("9TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("10TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("11TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("12TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("13TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("14TrackVTX","3orMoreTrackVTX");
  mycat_nTracksAtVtx3Bin->map("15TrackVTX","3orMoreTrackVTX");


  RooMappedCategory* mycat_nSingleTracks=
    new RooMappedCategory("mycat_nSingleTracks","number of single significant tracks category",*cat_nSingleTracks);//,"Undefined");
  mycat_nSingleTracks->map("NoSingleTrack","NoSingleTrack");
  mycat_nSingleTracks->map("1SingleTrack","1SingleTrack");
  mycat_nSingleTracks->map("2SingleTrack","2SingleTrack");
  mycat_nSingleTracks->map("3SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks->map("4SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks->map("5SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks->map("6SingleTrack","3orMoreSingleTrack");
  mycat_nSingleTracks->map("7SingleTrack","3orMoreSingleTrack");

  /*
  RooMappedCategory* mycat_nSingleTracksMain=
    new RooMappedCategoryMain("mycat_nSingleTracksMain","number of single significant tracks category only two bins",*cat_nSingleTracks,"Undefined");
  mycat_nSingleTracks->map("NoSingleTrack","NoSingleTrack");
  mycat_nSingleTracks->map("1SingleTrack","1orMoreSingleTrack");
  mycat_nSingleTracks->map("2SingleTrack","1orMoreSingleTrack");
  mycat_nSingleTracks->map("3SingleTrack","1orMoreSingleTrack");
  mycat_nSingleTracks->map("4SingleTrack","1orMoreSingleTrack");
  mycat_nSingleTracks->map("5SingleTrack","1orMoreSingleTrack");
  mycat_nSingleTracks->map("6SingleTrack","1orMoreSingleTrack");
  mycat_nSingleTracks->map("7SingleTrack","1orMoreSingleTrack");
  */
  
  RooRealVar *energyFraction=new RooRealVar("energyFraction","energyFraction",0.5,0.,1.00001);
  RooRealVar *significance3d=new RooRealVar("significance3d","significance3d",5.,0.,100.);
  RooRealVar *mass=new RooRealVar("mass","mass",2000.,0.,10000.);
  //  RooRealVar* nVTX=new RooRealVar("nVTX","nVTX",1,-0.5,5.5);
  RooRealVar* nTracksAtVtx=new RooRealVar("nTracksAtVtx2","nTracksAtVtx2",2,-0.5,15.5);
  RooRealVar* nSingleTracks=new RooRealVar("nSingleTracks2","nSingleTracks2",0,-0.5,7.5);

  RooThresholdCategory* energyFractionCat=new RooThresholdCategory("energyFractionCat","energyFractionCat",
								   *energyFraction,"NoFrag");
  energyFractionCat->addThreshold(0.999,"Normal");



  Double_t significance3d_bins[50];

  for (int jj=0;jj<49;jj++) {
//    std::cout << " bin " << jj << std::endl;
    significance3d_bins[jj]=((Double_t)jj)*(40./48.);
//    std::cout << " significance 3d " << significance3d_bins[jj] << std::endl;
  }
  significance3d_bins[49]=1000.;
    
  std::cout << " before roobinning " << std::endl;
  RooBinning* significance3d_binning=new RooBinning(49,significance3d_bins,"significance3d_binning");
  significance3d_binning->Print("v");
  
  Double_t mass_bins[50];
  
  for (int ss=0;ss<49;ss++) {
    mass_bins[ss]=((Double_t)ss)*(5000./48);
//    std::cout << " mass " << mass_bins[ss] << std::endl;

  }
  mass_bins[49]=20000.;
    
  RooBinning* mass_binning=new RooBinning(49,mass_bins,"mass_binning");
  mass_binning->Print("v");

  Double_t energyFraction_bins[50];
  
  for (int ii=0;ii<49;ii++) {
    energyFraction_bins[ii]=(Double_t(ii))*(1./48);
//    std::cout << " energyFraction " << energyFraction_bins[ii] << std::endl;
  }
  energyFraction_bins[49]=1.000001;
    
  RooBinning* energyFraction_binning=new RooBinning(49,energyFraction_bins,"energyFraction_binning");
  energyFraction_binning->Print("v");

  
//  std::cout << " here " << std::endl;
  
  TString fileName("../../../reduceddatasets/reduceddataset_");
  fileName+=collectionName;
  fileName+=".root";
  
  TFile *file=new TFile(fileName);
  TTree* tree=(TTree*)file->Get("SVTree");

  TString outputFileName("../trainingResultsLikelihood/smoothedhistos_");
  outputFileName+=collectionName;
  outputFileName+="_";
  outputFileName+=flavourString;
  outputFileName+=".root";
  TFile *filehistos=new TFile(outputFileName,"recreate");

  TString eventListString("cat_flavour==");
  eventListString+=flavourType;
  

  tree->Draw(">>eventlist",eventListString);
  
//  TEventList* eventlist=(TEventList*)gDirectory->Get("eventlist");
  
//  tree->SetEventList(eventlist);


  //  mass->setBinning(*mass_binning);
  //  energyFraction->setBinning(*energyFraction_binning);
  //  significance3d->setBinning(*significance3d_binning);


  mass->Print("v");
  energyFraction->Print("v");
  significance3d->Print("v");

  RooArgSet before(*cat_nVTX,*cat_nTracksAtVtx,
                   *cat_nSingleTracks,*energyFraction,
                   *mass,*significance3d,*cat_flavour);
  

  before.add(*cat_flavour);

  
  before.Print("v");
  cout << "  Reading in dataset " << endl;

  TString set("MC");
  set+=collectionName;
  if (flavourType==1)
  {
    set+="1";
  }
  else if (flavourType==4)
  {
    set+="4";
  }
  else if (flavourType==5)
  {
    set+="5";
  }
  
  

  RooDataSet* dataset2=new RooDataSet(set,set,before);


  int var_nVTX;
  int var_nTracksAtVtx;
  int var_nSingleTracks;
  double var_energyFraction;
  double var_mass;
  double var_significance3d;
  int var_cat_flavour;

  tree->SetBranchAddress("nVTX",&var_nVTX);
  tree->SetBranchAddress("nTracksAtVtx",&var_nTracksAtVtx);
  tree->SetBranchAddress("nSingleTracks",&var_nSingleTracks);
  tree->SetBranchAddress("energyFraction",&var_energyFraction);
  tree->SetBranchAddress("mass",&var_mass);
  tree->SetBranchAddress("significance3d",&var_significance3d);
  tree->SetBranchAddress("cat_flavour",&var_cat_flavour);
  
  for (int i=0;i<tree->GetEntries();i++)
  {
    tree->GetEntry(i);


    cat_nVTX->setIndex(var_nVTX);
    cat_nTracksAtVtx->setIndex(var_nTracksAtVtx);
    cat_nSingleTracks->setIndex(var_nSingleTracks);
    energyFraction->setVal(var_energyFraction);
    mass->setVal(var_mass);
    significance3d->setVal(var_significance3d);
    cat_flavour->setIndex(var_cat_flavour);
  
    dataset2->add(RooArgSet(*cat_nVTX,
                            *cat_nTracksAtVtx,
                            *cat_nSingleTracks,
                            *energyFraction,
                            *mass,
                            *significance3d,
                            *cat_flavour));

  }



//  RooDataSet* dataset2=new RooDataSet("MC","MC",tree,before);
  cout << "  Reading in dataset FINISHED " << endl;
  
  RooDataSet* dataset=(RooDataSet*)dataset2->reduce(eventListString);

  dataset->addColumn(*energyFractionCat);
  dataset->addColumn(*mycat_nVTX);
  dataset->addColumn(*mycat_nSingleTracks);
  dataset->addColumn(*mycat_nTracksAtVtx3Bin);
  dataset->addColumn(*mycat_nTracksAtVtx4Bin);
  dataset->addColumn(*mycat_nTracksAtVtx6Bin);
  
  const RooArgSet* obsindata2  = dataset->get(0);
  RooCategory* cat_energyFractionCat = static_cast<RooCategory*>(obsindata2->find("energyFractionCat"));


  /*
  RooDataHist* datahist=new RooDataHist("MC_binned","MC_binned",RooArgSet(*cat_nVTX,*cat_nTracksAtVtx,
									  *cat_nSingleTracks,*energyFraction,
									  *mass,*significance3d),*dataset);
  datahist->Print("v");
  cout << " datahist created " << endl;

  
  datahist->addColumn(*mycat_nVTX);
  //  datahist->addColumn(*mycat_nSingleTracks);
  //  datahist->addColumn(*mycat_nTracksAtVtx3Bin);
  //  datahist->addColumn(*mycat_nTracksAtVtx4Bin);
  datahist->addColumn(*mycat_nTracksAtVtx6Bin);

  //  datahist->Print("v");

  cout << "datahist created DONE" << endl;
  */
  //jetzt die PDF...
  
  dataset->Print("v");

  dataset->table(*mycat_nVTX)->Print();
  dataset->table(*mycat_nSingleTracks)->Print();
  dataset->table(*mycat_nTracksAtVtx3Bin)->Print();
  dataset->table(*mycat_nTracksAtVtx4Bin)->Print();

  
  RooRealVar* bottom_coef=new RooRealVar(flavourString+"_coef","bottom jet component",100000.,0.,1000000.);
  //  RooRealVar* light_coef=new RooRealVar("light_coef","light jet component",0.2,0.,1.);
  //  RooFormulaVar* charm_coef=new RooFormulaVar("charm_coef","charm jet component","1.-@0-@1",RooArgList(*light_coef,*bottom_coef));

  //////NNNNNNOOOOOOOOOWWWWWWWW    BBBBBBOOOOOOOOTTTTTTTTOOOOOOOOMMMMMMMMM

  RooRealVar* bottom_e_NoVTX=new RooRealVar(flavourString+"_e_NoVTX","no vertex coefficient",0.2,0.,1.);
  RooRealVar* bottom_e_1VTX=new RooRealVar(flavourString+"_e_1VTX","1 vertex coefficient",0.2,0.,1.);
  //RooFormulaVar* bottom_e_2orMoreVTX=new RooFormulaVar(flavourString+"_e_2orMoreVTX","2 vertex coefficient","1.-@0-@1",RooArgList(*bottom_e_NoVTX,*bottom_e_1VTX));
  RooFormulaVar* bottom_e_2orMoreVTX=new RooFormulaVar(flavourString+"_e_2orMoreVTX","2 vertex coefficient","1.-@0-@1",RooArgList(*bottom_e_NoVTX,
															  *bottom_e_1VTX));
  

  RooRealVar* bottom_e_1VTX_2TrackVTX=new RooRealVar(flavourString+"_e_1VTX_2TrackVTX",
						     flavourString+"_e_1VTX_2TrackVTX",0.2,0.,1.);
  RooRealVar* bottom_e_1VTX_3TrackVTX=new RooRealVar(flavourString+"_e_1VTX_3TrackVTX",
						     flavourString+"_e_1VTX_3TrackVTX",0.2,0.,1.);
  RooRealVar* bottom_e_1VTX_4TrackVTX=new RooRealVar(flavourString+"_e_1VTX_4TrackVTX",
						     flavourString+"_e_1VTX_4TrackVTX",0.2,0.,1.);
  RooRealVar* bottom_e_1VTX_5or6TrackVTX=new RooRealVar(flavourString+"_e_1VTX_5or6TrackVTX",
							flavourString+"_e_1VTX_5or6TrackVTX",0.2,0.,1.);
  RooFormulaVar* bottom_e_1VTX_7orMoreTrackVTX =new RooFormulaVar(flavourString+"_e_1VTX_7orMoreTrackVTX",
								  flavourString+"_e_1VTX_7orMoreTrackVTX",
								  "1.-@0-@1-@2-@3",RooArgList(*bottom_e_1VTX_2TrackVTX,
											      *bottom_e_1VTX_3TrackVTX,
											      *bottom_e_1VTX_4TrackVTX,
											      *bottom_e_1VTX_5or6TrackVTX));


   RooRealVar* bottom_e_1Vtx_NoSingleTrack=new RooRealVar(flavourString+"_e_1Vtx_NoSingleTrack",
							  flavourString+"_e_1Vtx_NoSingleTrack",
							  0.4,0.,1.);
   RooRealVar* bottom_e_1Vtx_1SingleTrack=new RooRealVar(flavourString+"_e_1Vtx_1SingleTrack",
							  flavourString+"_e_1Vtx_1SingleTrack",
							  0.4,0.,1.);
   RooFormulaVar* bottom_e_1Vtx_2orMoreSingleTrack=new RooFormulaVar(flavourString+"_e_1Vtx_2orMoreSingleTrack",
								     flavourString+"_e_1Vtx_2orMoreSingleTrack",
								     "1.-@0-@1",RooArgList(*bottom_e_1Vtx_NoSingleTrack,
											   *bottom_e_1Vtx_1SingleTrack));
     

   RooRealVar* bottom_e_2orMoreVTX_4TrackVTX=new RooRealVar(flavourString+"_e_2orMoreVTX_4TrackVTX",
							    flavourString+"_e_2orMoreVTX_4TrackVTX",0.2,0.,1.);
   RooFormulaVar* bottom_e_2orMoreVTX_5orMoreTrackVTX=new RooFormulaVar(flavourString+"_e_2orMoreVTX_5orMoreTrackVTX",
								       flavourString+"_e_2orMoreVTX_5orMoreTrackVTX",
								       "1.-@0",RooArgList(*bottom_e_2orMoreVTX_4TrackVTX));


   //energyFraction normal...

   RooRealVar* bottom_e_NoVtx_1SingleTrack_energyFraction_normal=new RooRealVar(flavourString+"_e_NoVtx_1SingleTrack_energyFraction_normal",
										flavourString+"_e_NoVtx_1SingleTrack_energyFraction_normal",
										0.8,0.,1.);
  

  
   RooFormulaVar* bottom_e_NoVtx_1SingleTrack_energyFraction_nofrag=new RooFormulaVar(flavourString+"_e_NoVtx_1SingleTrack_energyFraction_nofrag",
										      flavourString+"_e_NoVtx_1SingleTrack_energyFraction_nofrag",
										      "1.-@0",RooArgList(*bottom_e_NoVtx_1SingleTrack_energyFraction_normal));
  
  
   RooRealVar* bottom_e_NoVtx_2orMoreSingleTrack_energyFraction_normal=new RooRealVar(flavourString+"_e_NoVtx_2orMoreSingleTrack_energyFraction_normal",
										      flavourString+"_e_NoVtx_2orMoreSingleTrack_energyFraction_normal",
										      0.8,0.,1.);
  
  
  
   RooFormulaVar* bottom_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag=new RooFormulaVar(flavourString+"_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag",
											    flavourString+"_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag",
											    "1-@0",RooArgList(*bottom_e_NoVtx_2orMoreSingleTrack_energyFraction_normal));


   RooRealVar* bottom_e_1orMoreVTX_2TrackVTX_energyFraction_normal=new RooRealVar(flavourString+"_e_1orMoreVTX_2TrackVTX_energyFraction_normal",
										  flavourString+"_e_1orMoreVTX_2TrackVTX_energyFraction_normal",
										  0.8,0.,1.);
  
  
  
  RooFormulaVar* bottom_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag=new RooFormulaVar(flavourString+"_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag",
										       flavourString+"_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag",
										       "1-@0",RooArgList(*bottom_e_1orMoreVTX_2TrackVTX_energyFraction_normal));
  
  
  RooRealVar* bottom_e_1orMoreVTX_3TrackVTX_energyFraction_normal=new RooRealVar(flavourString+"_e_1orMoreVTX_3TrackVTX_energyFraction_normal",
										 flavourString+"_e_1orMoreVTX_3TrackVTX_energyFraction_normal",
										 0.8,0.,1.);
  
  
  
  RooFormulaVar* bottom_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag=new RooFormulaVar(flavourString+"_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag",
										       flavourString+"_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag",
										       "1-@0",RooArgList(*bottom_e_1orMoreVTX_3TrackVTX_energyFraction_normal));
  
  RooRealVar* bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal=new RooRealVar(flavourString+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal",
										       flavourString+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal",
										       0.8,0.,1.);
  
  RooFormulaVar* bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag=new RooFormulaVar(flavourString+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag",
											     flavourString+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag",
											     "1-@0",RooArgList(*bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  


  //0ST

  RooFormulaVar* bottom_1VTX_0ST_2VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_2VTX_normal",
								    flavourString+"_1VTX_0ST_2VTX_normal",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_NoSingleTrack,
												*bottom_e_1VTX_2TrackVTX,
												*bottom_e_1orMoreVTX_2TrackVTX_energyFraction_normal));


  RooFormulaVar* bottom_1VTX_0ST_3VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_3VTX_normal",
								    flavourString+"_1VTX_0ST_3VTX_normal",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_NoSingleTrack,
												*bottom_e_1VTX_3TrackVTX,
												*bottom_e_1orMoreVTX_3TrackVTX_energyFraction_normal));

  RooFormulaVar* bottom_1VTX_0ST_4VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_4VTX_normal",
								    flavourString+"_1VTX_0ST_4VTX_normal",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_NoSingleTrack,
												*bottom_e_1VTX_4TrackVTX,
												*bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));

  RooFormulaVar* bottom_1VTX_0ST_5or6VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_5or6VTX_normal",
								       flavourString+"_1VTX_0ST_5or6VTX_normal",
								       "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												   *bottom_e_1VTX,
												   *bottom_e_1Vtx_NoSingleTrack,
												   *bottom_e_1VTX_5or6TrackVTX,
												   *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));

  RooFormulaVar* bottom_1VTX_0ST_7orMoreVTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_7orMoreVTX_normal",
									  flavourString+"_1VTX_0ST_7orMoreVTX_normal",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_NoSingleTrack,
												      *bottom_e_1VTX_7orMoreTrackVTX,
												      *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  
  //1ST
  
  RooFormulaVar* bottom_1VTX_1ST_2VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_2VTX_normal",
								    flavourString+"_1VTX_1ST_2VTX_normal",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_1SingleTrack,
												*bottom_e_1VTX_2TrackVTX,
												*bottom_e_1orMoreVTX_2TrackVTX_energyFraction_normal));
  
  
  RooFormulaVar* bottom_1VTX_1ST_3VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_3VTX_normal",
								    flavourString+"_1VTX_1ST_3VTX_normal",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_1SingleTrack,
												*bottom_e_1VTX_3TrackVTX,
												*bottom_e_1orMoreVTX_3TrackVTX_energyFraction_normal));
  
  RooFormulaVar* bottom_1VTX_1ST_4VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_4VTX_normal",
								    flavourString+"_1VTX_1ST_4VTX_normal",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_1SingleTrack,
												*bottom_e_1VTX_4TrackVTX,
												*bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  
  RooFormulaVar* bottom_1VTX_1ST_5or6VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_5or6VTX_normal",
								       flavourString+"_1VTX_1ST_5or6VTX_normal",
								       "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												   *bottom_e_1VTX,
												   *bottom_e_1Vtx_1SingleTrack,
												   *bottom_e_1VTX_5or6TrackVTX,
												   *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  
  RooFormulaVar* bottom_1VTX_1ST_7orMoreVTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_7orMoreVTX_normal",
									  flavourString+"_1VTX_1ST_7orMoreVTX_normal",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_1SingleTrack,
												      *bottom_e_1VTX_7orMoreTrackVTX,
												      *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  
  
  //2orMoreST
  
  RooFormulaVar* bottom_1VTX_2orMoreST_2VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_2VTX_normal",
									  flavourString+"_1VTX_2orMoreST_2VTX_normal",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_2orMoreSingleTrack,
												      *bottom_e_1VTX_2TrackVTX,
												      *bottom_e_1orMoreVTX_2TrackVTX_energyFraction_normal));
  
  
  RooFormulaVar* bottom_1VTX_2orMoreST_3VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_3VTX_normal",
									  flavourString+"_1VTX_2orMoreST_3VTX_normal",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_2orMoreSingleTrack,
												      *bottom_e_1VTX_3TrackVTX,
												      *bottom_e_1orMoreVTX_3TrackVTX_energyFraction_normal));
  
  RooFormulaVar* bottom_1VTX_2orMoreST_4VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_4VTX_normal",
									  flavourString+"_1VTX_2orMoreST_4VTX_normal",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_2orMoreSingleTrack,
												      *bottom_e_1VTX_4TrackVTX,
												      *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));

  RooFormulaVar* bottom_1VTX_2orMoreST_5or6VTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_5or6VTX_normal",
									     flavourString+"_1VTX_2orMoreST_5or6VTX_normal",
									     "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
													 *bottom_e_1VTX,
													 *bottom_e_1Vtx_2orMoreSingleTrack,
													 *bottom_e_1VTX_5or6TrackVTX,
													 *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  
  RooFormulaVar* bottom_1VTX_2orMoreST_7orMoreVTX_normal_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_7orMoreVTX_normal",
										flavourString+"_1VTX_2orMoreST_7orMoreVTX_normal",
										"@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
													    *bottom_e_1VTX,
													    *bottom_e_1Vtx_2orMoreSingleTrack,
													    *bottom_e_1VTX_7orMoreTrackVTX,
													    *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  

  
  //2orMoreVTX
  
  RooFormulaVar* bottom_2orMoreVTX_4VTX_normal_coef=new RooFormulaVar(flavourString+"_2orMoreVTX_4VTX_normal",
								      flavourString+"_2orMoreVTX_4VTX_normal",
								      "@0*@1*@2*@3",RooArgList(*bottom_coef,
											       *bottom_e_2orMoreVTX,
											       *bottom_e_2orMoreVTX_4TrackVTX,
											       *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));
  
  RooFormulaVar* bottom_2orMoreVTX_5orMoreVTX_normal_coef=new RooFormulaVar(flavourString+"_2orMoreVTX_5orMoreVTX_normal",
									    flavourString+"_2orMoreVTX_5orMoreVTX_normal",
									    "@0*@1*@2*@3",RooArgList(*bottom_coef,
												     *bottom_e_2orMoreVTX,
												     *bottom_e_2orMoreVTX_5orMoreTrackVTX,
												     *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal));

												       
  //

  RooRealVar* bottom_e_NoVtx_NoSingleTrack=new RooRealVar(flavourString+"_e_NoVtx_NoSingleTrack",
							  flavourString+"_e_NoVtx_NoSingleTrack",
							  0.4,0.,1.);
  RooRealVar* bottom_e_NoVtx_1SingleTrack=new RooRealVar(flavourString+"_e_NoVtx_1SingleTrack",
							 flavourString+"_e_NoVtx_1SingleTrack",
							 0.2,0.,1.);
  RooRealVar* bottom_e_NoVtx_2SingleTrack=new RooRealVar(flavourString+"_e_NoVtx_2SingleTrack",
							 flavourString+"_e_NoVtx_2SingleTrack",
							 0.2,0.,1.);
  RooFormulaVar* bottom_e_NoVtx_3orMoreSingleTrack=new RooFormulaVar(flavourString+"_e_NoVtx_3orMoreSingleTrack",
								     flavourString+"_e_NoVtx_3orMoreSingleTrack",
								     "1.-@0-@1-@2",RooArgList(*bottom_e_NoVtx_NoSingleTrack,
											      *bottom_e_NoVtx_1SingleTrack,
											      *bottom_e_NoVtx_2SingleTrack));
  
  //coef for noVTX case
  
  RooFormulaVar* bottom_noVTX_0ST_coef=new RooFormulaVar(flavourString+"_noVTX_0ST",
							 flavourString+"_noVTX_0ST",
							 "@0*@1*@2",RooArgList(*bottom_coef,
									       *bottom_e_NoVTX,
									       *bottom_e_NoVtx_NoSingleTrack));
  
  RooFormulaVar* bottom_noVTX_1ST_normal_coef=new RooFormulaVar(flavourString+"_noVTX_1ST_normal",
								flavourString+"_noVTX_1ST_normal",
								"@0*@1*@2*@3",RooArgList(*bottom_coef,
											 *bottom_e_NoVTX,
											 *bottom_e_NoVtx_1SingleTrack,
											 *bottom_e_NoVtx_1SingleTrack_energyFraction_normal));

  RooFormulaVar* bottom_noVTX_2ST_normal_coef=new RooFormulaVar(flavourString+"_noVTX_2ST_normal",
								flavourString+"_noVTX_2ST_normal",
								"@0*@1*@2*@3",RooArgList(*bottom_coef,
											 *bottom_e_NoVTX,
											 *bottom_e_NoVtx_2SingleTrack,
											 *bottom_e_NoVtx_2orMoreSingleTrack_energyFraction_normal));

  RooFormulaVar* bottom_noVTX_3orMoreST_normal_coef=new RooFormulaVar(flavourString+"_noVTX_3orMoreST_normal",
								      flavourString+"_noVTX_3orMoreST_normal",
								      "@0*@1*@2*@3",RooArgList(*bottom_coef,
											       *bottom_e_NoVTX,
											       *bottom_e_NoVtx_3orMoreSingleTrack,
											       *bottom_e_NoVtx_2orMoreSingleTrack_energyFraction_normal));

  //ALL COEFFICIENTS FOR NOFRAG CASE AGAIN


  //0ST

  RooFormulaVar* bottom_1VTX_0ST_2VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_2VTX_nofrag",
								    flavourString+"_1VTX_0ST_2VTX_nofrag",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_NoSingleTrack,
												*bottom_e_1VTX_2TrackVTX,
												*bottom_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag));


  RooFormulaVar* bottom_1VTX_0ST_3VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_3VTX_nofrag",
								    flavourString+"_1VTX_0ST_3VTX_nofrag",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_NoSingleTrack,
												*bottom_e_1VTX_3TrackVTX,
												*bottom_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_0ST_4VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_4VTX_nofrag",
								    flavourString+"_1VTX_0ST_4VTX_nofrag",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_NoSingleTrack,
												*bottom_e_1VTX_4TrackVTX,
												*bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_0ST_5or6VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_5or6VTX_nofrag",
								       flavourString+"_1VTX_0ST_5or6VTX_nofrag",
								       "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												   *bottom_e_1VTX,
												   *bottom_e_1Vtx_NoSingleTrack,
												   *bottom_e_1VTX_5or6TrackVTX,
												   *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_0ST_7orMoreVTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_0ST_7orMoreVTX_nofrag",
									  flavourString+"_1VTX_0ST_7orMoreVTX_nofrag",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_NoSingleTrack,
												      *bottom_e_1VTX_7orMoreTrackVTX,
												      *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
  
  //1ST

  RooFormulaVar* bottom_1VTX_1ST_2VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_2VTX_nofrag",
								    flavourString+"_1VTX_1ST_2VTX_nofrag",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_1SingleTrack,
												*bottom_e_1VTX_2TrackVTX,
												*bottom_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag));
  
  
  RooFormulaVar* bottom_1VTX_1ST_3VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_3VTX_nofrag",
								    flavourString+"_1VTX_1ST_3VTX_nofrag",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_1SingleTrack,
												*bottom_e_1VTX_3TrackVTX,
												*bottom_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_1ST_4VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_4VTX_nofrag",
								    flavourString+"_1VTX_1ST_4VTX_nofrag",
								    "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												*bottom_e_1VTX,
												*bottom_e_1Vtx_1SingleTrack,
												*bottom_e_1VTX_4TrackVTX,
												*bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_1ST_5or6VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_5or6VTX_nofrag",
								       flavourString+"_1VTX_1ST_5or6VTX_nofrag",
								       "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												   *bottom_e_1VTX,
												   *bottom_e_1Vtx_1SingleTrack,
												   *bottom_e_1VTX_5or6TrackVTX,
												   *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_1ST_7orMoreVTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_1ST_7orMoreVTX_nofrag",
									  flavourString+"_1VTX_1ST_7orMoreVTX_nofrag",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_1SingleTrack,
												      *bottom_e_1VTX_7orMoreTrackVTX,
												      *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));


  //2orMoreST

  RooFormulaVar* bottom_1VTX_2orMoreST_2VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_2VTX_nofrag",
									  flavourString+"_1VTX_2orMoreST_2VTX_nofrag",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_2orMoreSingleTrack,
												      *bottom_e_1VTX_2TrackVTX,
												      *bottom_e_1orMoreVTX_2TrackVTX_energyFraction_nofrag));
  
  
  RooFormulaVar* bottom_1VTX_2orMoreST_3VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_3VTX_nofrag",
									  flavourString+"_1VTX_2orMoreST_3VTX_nofrag",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_2orMoreSingleTrack,
												      *bottom_e_1VTX_3TrackVTX,
												      *bottom_e_1orMoreVTX_3TrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_2orMoreST_4VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_4VTX_nofrag",
									  flavourString+"_1VTX_2orMoreST_4VTX_nofrag",
									  "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
												      *bottom_e_1VTX,
												      *bottom_e_1Vtx_2orMoreSingleTrack,
												      *bottom_e_1VTX_4TrackVTX,
												      *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));

  RooFormulaVar* bottom_1VTX_2orMoreST_5or6VTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_5or6VTX_nofrag",
									     flavourString+"_1VTX_2orMoreST_5or6VTX_nofrag",
									     "@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
													 *bottom_e_1VTX,
													 *bottom_e_1Vtx_2orMoreSingleTrack,
													 *bottom_e_1VTX_5or6TrackVTX,
													 *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
  
  RooFormulaVar* bottom_1VTX_2orMoreST_7orMoreVTX_nofrag_coef=new RooFormulaVar(flavourString+"_1VTX_2orMoreST_7orMoreVTX_nofrag",
										flavourString+"_1VTX_2orMoreST_7orMoreVTX_nofrag",
										"@0*@1*@2*@3*@4",RooArgList(*bottom_coef,
													    *bottom_e_1VTX,
													    *bottom_e_1Vtx_2orMoreSingleTrack,
													    *bottom_e_1VTX_7orMoreTrackVTX,
													    *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
  


  //2orMoreVTX

  RooFormulaVar* bottom_2orMoreVTX_4VTX_nofrag_coef=new RooFormulaVar(flavourString+"_2orMoreVTX_4VTX_nofrag",
								      flavourString+"_2orMoreVTX_4VTX_nofrag",
								      "@0*@1*@2*@3",RooArgList(*bottom_coef,
											       *bottom_e_2orMoreVTX,
											       *bottom_e_2orMoreVTX_4TrackVTX,
											       *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));
  
  RooFormulaVar* bottom_2orMoreVTX_5orMoreVTX_nofrag_coef=new RooFormulaVar(flavourString+"_2orMoreVTX_5orMoreVTX_nofrag",
									    flavourString+"_2orMoreVTX_5orMoreVTX_nofrag",
									    "@0*@1*@2*@3",RooArgList(*bottom_coef,
												     *bottom_e_2orMoreVTX,
												     *bottom_e_2orMoreVTX_5orMoreTrackVTX,
												     *bottom_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_nofrag));



  //coef for noVTX case
  
  RooFormulaVar* bottom_noVTX_1ST_nofrag_coef=new RooFormulaVar(flavourString+"_noVTX_1ST_nofrag",
								flavourString+"_noVTX_1ST_nofrag",
								"@0*@1*@2*@3",RooArgList(*bottom_coef,
											 *bottom_e_NoVTX,
											 *bottom_e_NoVtx_1SingleTrack,
											 *bottom_e_NoVtx_1SingleTrack_energyFraction_nofrag));

  RooFormulaVar* bottom_noVTX_2ST_nofrag_coef=new RooFormulaVar(flavourString+"_noVTX_2ST_nofrag",
								flavourString+"_noVTX_2ST_nofrag",
								"@0*@1*@2*@3",RooArgList(*bottom_coef,
											 *bottom_e_NoVTX,
											 *bottom_e_NoVtx_2SingleTrack,
											 *bottom_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag));

  RooFormulaVar* bottom_noVTX_3orMoreST_nofrag_coef=new RooFormulaVar(flavourString+"_noVTX_3orMoreST_nofrag",
								      flavourString+"_noVTX_3orMoreST_nofrag",
								      "@0*@1*@2*@3",RooArgList(*bottom_coef,
											       *bottom_e_NoVTX,
											       *bottom_e_NoVtx_3orMoreSingleTrack,
											       *bottom_e_NoVtx_2orMoreSingleTrack_energyFraction_nofrag));


  //

  /*
  RooRealVar* bottom_e_NoVTX_noSingleTrack=new RooRealVar(flavourString+"_e_NoVTX_noSingleTrack",flavourString+"_e_NoVTX_noTracks",0.2,0.,1.);
  RooRealVar* bottom_e_NoVTX_1SingleTrack=new RooRealVar(flavourString+"_e_NoVTX_1SingleTrack",flavourString+"_e_NoVTX_1SingleTrack",0.2,0.,1.);
  RooRealVar* bottom_e_NoVTX_2SingleTracks=new RooRealVar(flavourString+"_e_NoVTX_2SingleTracks",flavourString+"_e_NoVTX_2SingleTracks",0.1,0.,1.);
  RooFormulaVar* bottom_e_NoVTX_3orMoreSingleTracks=new RooFormulaVar(flavourString+"_e_NoVTX_2orMoreSingleTracks",
								      flavourString+"_e_NoVTX_2orMoreSingleTracks",
								      "1.-@0-@1-@2",RooArgList(*bottom_e_NoVTX_noSingleTrack,
											       *bottom_e_NoVTX_1SingleTrack,
											       *bottom_e_NoVTX_2SingleTracks));

  */
								      /*
								      "1.-@0-@1",RooArgList(*bottom_e_NoVTX_1SingleTrack,
											    *bottom_e_NoVTX_2SingleTracks));
									*/
  
  //DUMMY PDF  
  Int_t bottom_noVTX_noSingleTrack_nbins(1);
  TArrayD bottom_noVTX_noSingleTrack_limits(bottom_noVTX_noSingleTrack_nbins+1);
  bottom_noVTX_noSingleTrack_limits[0]=-0.5;
  bottom_noVTX_noSingleTrack_limits[1]=7.5;
  RooArgList* bottom_noVTX_noSingleTrack_list = new RooArgList(flavourString+"_noVTX_noSingleTrack_list");
  RooParametricStepFunction* bottom_noVTX_noSingleTrack_pdf=new RooParametricStepFunction(flavourString+"_noVTX_noSingleTrack_pdf",
											  flavourString+"_noVTX_noSingleTrack_pdf",
											  *nSingleTracks,
											  *bottom_noVTX_noSingleTrack_list,
											  bottom_noVTX_noSingleTrack_limits,
											  bottom_noVTX_noSingleTrack_nbins);
  
  
  
  std::cout << "First smoothing " << std::endl;
  
  
  RooDataSet* data_noVTX=(RooDataSet*)dataset->reduce("nVTX==0&&nSingleTracks>0");
  RooNDKeysPdf* bottom_noVTX_significance3d_pdf=new RooNDKeysPdf(flavourString+"_noVTX_significance3d_pdf",
                                                                 flavourString+"_noVTX_significance3d_pdf",
                                                                 *significance3d,
                                                                 *data_noVTX,
                                                                 RooNDKeysPdf::NoMirror,1);
  
  TH1F* bottom_noVTX_significance3d_histo=makeHisto(bottom_noVTX_significance3d_pdf,
						    flavourString+"_noVTX_significance3d_histo",
						    500,0,100,
						    significance3d);

  /*
    RooDataHist* bottom_noVTX_significance3d_histo2=new RooDataHist(flavourString+"_noVTX_significance3d_histo2",
    flavourString+"_noVTX_significance3d_histo2",
    *significance3d,
    bottom_noVTX_significance3d_histo,
    1.0);
    
    std::cout << " 1 " << std::endl;
    
    RooHistPdf* bottom_noVTX_significance3d_pdf2=new RooHistPdf(flavourString+"_noVTX_significance3d_pdf2",
    flavourString+"_noVTX_significance3d_pdf2",
    *significance3d,
    *bottom_noVTX_significance3d_histo2);
    */
  
  TCanvas c1("c1","c1");						
  RooPlot* bottom_noVTX_significance3d_pdf_plot=significance3d->frame();
  data_noVTX->plotOn(bottom_noVTX_significance3d_pdf_plot);
  bottom_noVTX_significance3d_pdf->plotOn(bottom_noVTX_significance3d_pdf_plot);
  bottom_noVTX_significance3d_pdf_plot->Draw();


  c1.Update();
  c1.SaveAs(identificationFile+"_noVTX_significance3d_pdf.eps");
  

  std::cout << "new smoothing " << std::endl;
  
  RooDataSet* data_noVTX_1SingleTrack=(RooDataSet*)dataset->reduce("nVTX==0&&nSingleTracks==1&&energyFractionCat==energyFractionCat::Normal");

  RooNDKeysPdf* bottom_noVTX_1SingleTrack_energyFraction_normal_pdf=new RooNDKeysPdf(flavourString+"_noVTX_1SingleTrack_energyFraction_normal_pdf",
										 flavourString+"_noVTX_1SingleTrack_energyFraction_normal_pdf",
										 *energyFraction,
										 *data_noVTX_1SingleTrack,
                                                                                 RooNDKeysPdf::NoMirror,1);

  TH1F* bottom_noVTX_1SingleTrack_energyFraction_normal_histo=makeHisto(bottom_noVTX_1SingleTrack_energyFraction_normal_pdf,
									flavourString+"_noVTX_1SingleTrack_energyFraction_normal_histo",
									500,0,1.00001,
									energyFraction);


  Int_t bottom_dummy1_nbins(1);
  TArrayD bottom_dummy1_limits(bottom_dummy1_nbins+1);
  bottom_dummy1_limits[0]=0.;
  bottom_dummy1_limits[1]=1.00001;
  RooArgList* bottom_dummy1_list = new RooArgList(flavourString+"_dummy1_list");
  RooParametricStepFunction* bottom_dummy1_pdf=new RooParametricStepFunction(flavourString+"_dummy1_pdf",
									     flavourString+"_dummy1_pdf",
									     *energyFraction,
									     *bottom_dummy1_list,
									     bottom_dummy1_limits,
									     bottom_dummy1_nbins);


  
  
  
  TCanvas c2("c2","c2");						
  RooPlot* bottom_noVTX_1SingleTrack_energyFraction_pdf_plot=energyFraction->frame();
  data_noVTX_1SingleTrack->plotOn(bottom_noVTX_1SingleTrack_energyFraction_pdf_plot);
  bottom_noVTX_1SingleTrack_energyFraction_normal_pdf->plotOn(bottom_noVTX_1SingleTrack_energyFraction_pdf_plot);
  bottom_noVTX_1SingleTrack_energyFraction_pdf_plot->Draw();
  c2.Update();
  c2.SaveAs(identificationFile+"_noVTX_1SingleTrack_energyFraction_pdf.eps");
  




  std::cout << "new smoothing " << std::endl;
  RooDataSet* data_noVTX_2orMoreSingleTrack=(RooDataSet*)dataset->reduce("nVTX==0&&nSingleTracks>1&&energyFractionCat==energyFractionCat::Normal");
  RooNDKeysPdf* bottom_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf=new RooNDKeysPdf(flavourString+"_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf",
										       flavourString+"_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf",
										       *energyFraction,
										       *data_noVTX_2orMoreSingleTrack,
                                                                                       RooNDKeysPdf::NoMirror,1);

  
  TH1F* bottom_noVTX_2orMoreSingleTrack_energyFraction_normal_histo=makeHisto(bottom_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf,
									      flavourString+"_noVTX_2orMoreSingleTrack_energyFraction_normal_histo",
									      500,0,1.00001,
									      energyFraction);
  
  

  TCanvas c3("c3","c3");						
  RooPlot* bottom_noVTX_2orMoreSingleTrack_energyFraction_pdf_plot=energyFraction->frame();
  data_noVTX_2orMoreSingleTrack->plotOn(bottom_noVTX_2orMoreSingleTrack_energyFraction_pdf_plot);
  bottom_noVTX_2orMoreSingleTrack_energyFraction_normal_pdf->plotOn(bottom_noVTX_2orMoreSingleTrack_energyFraction_pdf_plot);
  bottom_noVTX_2orMoreSingleTrack_energyFraction_pdf_plot->Draw();
  c3.Update();
  c3.SaveAs(identificationFile+"_noVTX_2orMoreSingleTrack_energyFraction_pdf.eps");
  



  /*
  Int_t bottom_1VTX_nSingleTracks_nbins(4);
  TArrayD bottom_1VTX_nSingleTracks_limits(bottom_1VTX_nSingleTracks_nbins+1);
  bottom_1VTX_nSingleTracks_limits[0]=-0.5;
  bottom_1VTX_nSingleTracks_limits[1]=0.5;
  bottom_1VTX_nSingleTracks_limits[2]=1.5;
  bottom_1VTX_nSingleTracks_limits[3]=2.5;
  bottom_1VTX_nSingleTracks_limits[4]=7.5;
  
  RooArgList* bottom_1VTX_nSingleTracks_list = new RooArgList(flavourString+"_1VTX_nSingleTracks_list");
  RooRealVar* bottom_1VTX_nSingleTracks_noTracks=new RooRealVar(flavourString+"_1VTX_nSingleTracks_noTracks","bin 0 value",
								0.1,0.0,20.0);
  RooRealVar* bottom_1VTX_nSingleTracks_1Track=new RooRealVar(flavourString+"_1VTX_nSingleTracks_1Track","bin 1 value",
							      0.1,0.0,20.0);
  RooRealVar* bottom_1VTX_nSingleTracks_2Track=new RooRealVar(flavourString+"_1VTX_nSingleTracks_2Track","bin 2 value",
							      0.1,0.0,20.0);

  bottom_1VTX_nSingleTracks_list->add(*bottom_1VTX_nSingleTracks_noTracks);
  bottom_1VTX_nSingleTracks_list->add(*bottom_1VTX_nSingleTracks_1Track);
  bottom_1VTX_nSingleTracks_list->add(*bottom_1VTX_nSingleTracks_2Track);

  RhhBinnedPdf* bottom_1VTX_nSingleTracks_pdf=new RhhBinnedPdf(flavourString+"_1VTX_nSingleTracks_pdf",
							       flavourString+"_1VTX_nSingleTracks_pdf",
							       *nSingleTracks,
							       *bottom_1VTX_nSingleTracks_list,
							       bottom_1VTX_nSingleTracks_limits);
  */

 


  Int_t bottom_1VTX_nTracksAtVtx_nbins(5);
  TArrayD bottom_1VTX_nTracksAtVtx_limits(bottom_1VTX_nTracksAtVtx_nbins+1);
  // bottom_1VTX_nTracksAtVtx_limits[0]=1.5;
  bottom_1VTX_nTracksAtVtx_limits[0]=-.5;
  bottom_1VTX_nTracksAtVtx_limits[1]=2.5;
  bottom_1VTX_nTracksAtVtx_limits[2]=3.5;
  bottom_1VTX_nTracksAtVtx_limits[3]=4.5;
  bottom_1VTX_nTracksAtVtx_limits[4]=6.5;
  bottom_1VTX_nTracksAtVtx_limits[5]=15.5;
  
  
  RooArgList* bottom_1VTX_nTracksAtVtx_list = new RooArgList(flavourString+"_1VTX_nTracksAtVtx_list");
  RooRealVar* bottom_1VTX_nTracksAtVtx_2Tracks=new RooRealVar(flavourString+"_1VTX_nTracksAtVtx_2Tracks","bin 0 value",
							      0.1,0.0,1.0);
  RooRealVar* bottom_1VTX_nTracksAtVtx_3Tracks=new RooRealVar(flavourString+"_1VTX_nTracksAtVtx_3Tracks","bin 1 value",
							     0.1,0.0,1.0);
  RooRealVar* bottom_1VTX_nTracksAtVtx_4Tracks=new RooRealVar(flavourString+"_1VTX_nTracksAtVtx_4Tracks","bin 2 value",
							     0.1,0.0,1.0);
  RooRealVar* bottom_1VTX_nTracksAtVtx_56Tracks=new RooRealVar(flavourString+"_1VTX_nTracksAtVtx_56Tracks","bin 3 value",
							       0.1,0.0,1.0);
  bottom_1VTX_nTracksAtVtx_list->add(*bottom_1VTX_nTracksAtVtx_2Tracks);
  bottom_1VTX_nTracksAtVtx_list->add(*bottom_1VTX_nTracksAtVtx_3Tracks);
  bottom_1VTX_nTracksAtVtx_list->add(*bottom_1VTX_nTracksAtVtx_4Tracks);
  bottom_1VTX_nTracksAtVtx_list->add(*bottom_1VTX_nTracksAtVtx_56Tracks);
  bottom_1VTX_nTracksAtVtx_list->add(*bottom_1VTX_nTracksAtVtx_2Tracks);
  

  RooParametricStepFunction* bottom_1VTX_nTracksAtVtx_pdf=new RooParametricStepFunction(flavourString+"_1VTX_nTracksAtVtx_pdf",
											flavourString+"_1VTX_nTracksAtVtx_pdf",
											*nTracksAtVtx,
											*bottom_1VTX_nTracksAtVtx_list,
											bottom_1VTX_nTracksAtVtx_limits,
											bottom_1VTX_nTracksAtVtx_nbins);

  Int_t bottom_2orMoreVTX_nTracksAtVtx_nbins(2);
  TArrayD bottom_2orMoreVTX_nTracksAtVtx_limits(bottom_2orMoreVTX_nTracksAtVtx_nbins+1);
  bottom_2orMoreVTX_nTracksAtVtx_limits[0]=-0.5;
  //  bottom_2orMoreVTX_nTracksAtVtx_limits[0]=3.5;
  bottom_2orMoreVTX_nTracksAtVtx_limits[1]=4.5;
  bottom_2orMoreVTX_nTracksAtVtx_limits[2]=15.5;
  
  RooArgList* bottom_2orMoreVTX_nTracksAtVtx_list = new RooArgList(flavourString+"_2orMoreVTX_nTracksAtVtx_list");
  RooRealVar* bottom_2orMoreVTX_nTracksAtVtx_4Tracks=new RooRealVar(flavourString+"_2orMoreVTX_nTracksAtVtx_4Tracks","bin 0 value",
								    0.4,0.0,1.0);
  bottom_2orMoreVTX_nTracksAtVtx_list->add(*bottom_2orMoreVTX_nTracksAtVtx_4Tracks);

  RooParametricStepFunction* bottom_2orMoreVTX_nTracksAtVtx_pdf=new RooParametricStepFunction(flavourString+"_2orMoreVTX_nTracksAtVtx_normal_pdf",
											      flavourString+"_2orMoreVTX_nTracksAtVtx_normal_pdf",
											      *nTracksAtVtx,
											      *bottom_2orMoreVTX_nTracksAtVtx_list,
											      bottom_2orMoreVTX_nTracksAtVtx_limits,
											      bottom_2orMoreVTX_nTracksAtVtx_nbins);
  

  
  std::cout << "new smoothing " << std::endl;
  RooDataSet* data_1orMoreVTX_2TrackVTX=(RooDataSet*)dataset->reduce("nVTX>0&&nTracksAtVtx==2&&energyFractionCat==energyFractionCat::Normal"); 
  RooNDKeysPdf* bottom_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf=new RooNDKeysPdf(flavourString+"_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf",
									    flavourString+"_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf",
									    *energyFraction,
									    *data_1orMoreVTX_2TrackVTX,
                                                             RooNDKeysPdf::NoMirror,1.);

  TH1F* bottom_1orMoreVTX_2TrackVTX_energyFraction_normal_histo=makeHisto(bottom_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf,
									  flavourString+"_1orMoreVTX_2TrackVTX_energyFraction_normal_histo",
									  500,0,1.00001,
									  energyFraction);


  Int_t bottom_dummy3_nbins(1);
  TArrayD bottom_dummy3_limits(bottom_dummy1_nbins+1);
  bottom_dummy3_limits[0]=0.;
  bottom_dummy3_limits[1]=1.00001;
  RooArgList* bottom_dummy3_list = new RooArgList(flavourString+"_dummy3_list");
  RooParametricStepFunction* bottom_dummy3_pdf=new RooParametricStepFunction(flavourString+"_dummy3_pdf",
									     flavourString+"_dummy3_pdf",
									     *energyFraction,
									     *bottom_dummy3_list,
									     bottom_dummy3_limits,
									     bottom_dummy3_nbins);





  TCanvas c4("c4","c4");						
  RooPlot* bottom_1orMoreVTX_2TrackVTX_energyFraction_pdf_plot=energyFraction->frame();
  data_1orMoreVTX_2TrackVTX->plotOn(bottom_1orMoreVTX_2TrackVTX_energyFraction_pdf_plot);
  bottom_1orMoreVTX_2TrackVTX_energyFraction_normal_pdf->plotOn(bottom_1orMoreVTX_2TrackVTX_energyFraction_pdf_plot);
  bottom_1orMoreVTX_2TrackVTX_energyFraction_pdf_plot->Draw();
  c4.Update();
  c4.SaveAs(identificationFile+"_1orMoreVTX_2TrackVTX_energyFraction_pdf.eps");
  
  

  std::cout << "new smoothing " << std::endl;
  RooDataSet* data_1orMoreVTX_3TrackVTX=(RooDataSet*)dataset->reduce("nVTX>0&&(nTracksAtVtx+nSingleTracks)==3&&energyFractionCat==energyFractionCat::Normal"); 
  RooNDKeysPdf* bottom_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf=new RooNDKeysPdf(flavourString+"_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf",
										   flavourString+"_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf",
										   *energyFraction,
										   *data_1orMoreVTX_3TrackVTX,
                                                             RooNDKeysPdf::NoMirror,1);
  
  TH1F* bottom_1orMoreVTX_3TrackVTX_energyFraction_normal_histo=makeHisto(bottom_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf,
									  flavourString+"_1orMoreVTX_3TrackVTX_energyFraction_normal_histo",
									  500,0,1.00001,
									  energyFraction);

  

  Int_t bottom_dummy4_nbins(1);
  TArrayD bottom_dummy4_limits(bottom_dummy1_nbins+1);
  bottom_dummy4_limits[0]=0.;
  bottom_dummy4_limits[1]=1.00001;
  RooArgList* bottom_dummy4_list = new RooArgList(flavourString+"_dummy4_list");
  RooParametricStepFunction* bottom_dummy4_pdf=new RooParametricStepFunction(flavourString+"_dummy4_pdf",
									     flavourString+"_dummy4_pdf",
									     *energyFraction,
									     *bottom_dummy4_list,
									     bottom_dummy4_limits,
									     bottom_dummy4_nbins);




  TCanvas c5("c5","c5");						
  RooPlot* bottom_1orMoreVTX_3TrackVTX_energyFraction_pdf_plot=energyFraction->frame();
  data_1orMoreVTX_3TrackVTX->plotOn(bottom_1orMoreVTX_3TrackVTX_energyFraction_pdf_plot);
  bottom_1orMoreVTX_3TrackVTX_energyFraction_normal_pdf->plotOn(bottom_1orMoreVTX_3TrackVTX_energyFraction_pdf_plot);
  bottom_1orMoreVTX_3TrackVTX_energyFraction_pdf_plot->Draw();
  c5.Update();
  c5.SaveAs(identificationFile+"_1orMoreVTX_3TrackVTX_energyFraction_pdf.eps");
  
 


  std::cout << "new smoothing " << std::endl;
  RooDataSet* data_1orMoreVTX_4orMoreTrackVTX=(RooDataSet*)dataset->reduce("nVTX>0&&(nTracksAtVtx+nSingleTracks)>3&&energyFractionCat==energyFractionCat::Normal"); 
  RooNDKeysPdf* bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf=new RooNDKeysPdf(flavourString+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf",
											 flavourString+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf",
											 *energyFraction,
											 *data_1orMoreVTX_4orMoreTrackVTX,
                                                             RooNDKeysPdf::NoMirror,1);

  TH1F* bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo=makeHisto(bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf,
										flavourString+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo",
										500,0,1.00001,
										energyFraction);

  

  
  
  Int_t bottom_dummy5_nbins(1);
  TArrayD bottom_dummy5_limits(bottom_dummy1_nbins+1);
  bottom_dummy5_limits[0]=0.;
  bottom_dummy5_limits[1]=1.00001;
  RooArgList* bottom_dummy5_list = new RooArgList(flavourString+"_dummy5_list");
  RooParametricStepFunction* bottom_dummy5_pdf=new RooParametricStepFunction(flavourString+"_dummy5_pdf",
									     flavourString+"_dummy5_pdf",
									     *energyFraction,
									     *bottom_dummy5_list,
									     bottom_dummy5_limits,
									     bottom_dummy5_nbins);




  TCanvas c6("c6","c6");						
  RooPlot* bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_pdf_plot=energyFraction->frame();
  data_1orMoreVTX_4orMoreTrackVTX->plotOn(bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_pdf_plot);
  bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_pdf->plotOn(bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_pdf_plot);
  bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_pdf_plot->Draw();
  c6.Update();
  c6.SaveAs(identificationFile+"_1orMoreVTX_4orMoreTrackVTX_energyFraction_pdf.eps");
  
  

  
  std::cout << "new smoothing " << std::endl;
  RooDataSet* data_1orMoreVTX=(RooDataSet*)dataset->reduce("nVTX>0"); 
  RooNDKeysPdf* bottom_1orMoreVTX_significance3d_pdf=new RooNDKeysPdf(flavourString+"_1orMoreVTX_significance3d_pdf",
								  flavourString+"_1orMoreVTX_significance3d_pdf",
								  *significance3d,
								  *data_1orMoreVTX,
                                                             RooNDKeysPdf::NoMirror,1);

  TH1F* bottom_1orMoreVTX_significance3d_histo=makeHisto(bottom_1orMoreVTX_significance3d_pdf,
						    flavourString+"_1orMoreVTX_significance3d_histo",
						    500,0,100,
						    significance3d);

 

  TCanvas c7("c7","c7");						
  RooPlot* bottom_1orMoreVTX_significance3d_pdf_plot=significance3d->frame();
  data_1orMoreVTX->plotOn(bottom_1orMoreVTX_significance3d_pdf_plot);
  bottom_1orMoreVTX_significance3d_pdf->plotOn(bottom_1orMoreVTX_significance3d_pdf_plot);
  bottom_1orMoreVTX_significance3d_pdf_plot->Draw();
  c7.Update();
  c7.SaveAs(identificationFile+"_1orMoreVTX_significance3d_pdf_plot.eps");
  





  std::cout << "new smoothing " << std::endl;
  //RooDataSet* data_1orMoreVTX_2TrackVTX=dataset->reduce("cat_nVTX>0&&cat_nTracksAtVtx==2"); 
  RooNDKeysPdf* bottom_1orMoreVTX_2TrackVTX_mass_pdf=new RooNDKeysPdf(flavourString+"_1orMoreVTX_2TrackVTX_mass_pdf",
								  flavourString+"_1orMoreVTX_2TrackVTX_mass_pdf",
								  *mass,
								  *data_1orMoreVTX_2TrackVTX,
                                                             RooNDKeysPdf::NoMirror,1);

  TH1F* bottom_1orMoreVTX_2TrackVTX_mass_histo=makeHisto(bottom_1orMoreVTX_2TrackVTX_mass_pdf,
						    flavourString+"_1orMoreVTX_2TrackVTX_mass_histo",
						    500,0,10000,
						    mass);


  //,

  TCanvas c8("c8","c8");						
  RooPlot* bottom_1orMoreVTX_2TrackVTX_mass_pdf_plot=mass->frame();
  data_1orMoreVTX_2TrackVTX->plotOn(bottom_1orMoreVTX_2TrackVTX_mass_pdf_plot);
  bottom_1orMoreVTX_2TrackVTX_mass_pdf->plotOn(bottom_1orMoreVTX_2TrackVTX_mass_pdf_plot);
  bottom_1orMoreVTX_2TrackVTX_mass_pdf_plot->Draw();
  c8.Update();
  c8.SaveAs(identificationFile+"_1orMoreVTX_2TrackVTX_mass_pdf.eps");
  


  //							      bottom_1orMoreVTX_2TrackVTX_mass_nbins);
  //  RooNumIntConfig* cfg = RooAbsReal::defaultIntegratorConfig();
  //  cfg->setEpsAbs(1E-4);
  //  cfg->setEpsRel(1E-4);
  //    cfg->method1D().setLabel("RooIntegrator1D");
  //  cfg->method1D()->Print("v");
  //  cfg->getConfigSection("RooIntegrator1D")->setRealValue("maxSteps",100);
  //  bottom_1orMoreVTX_2TrackVTX_mass_pdf->setIntegratorConfig(*cfg);
  
  //bottom_1orMoreVTX_2TrackVTX_mass_pdf->->setIntegratorConfig(*cfg);

  
  std::cout << "new smoothing " << std::endl;
  RooDataSet* data_1orMoreVTX_3orMoreTrackVTX=(RooDataSet*)dataset->reduce("nVTX>0&&(nTracksAtVtx+nSingleTracks)>2"); 
  RooNDKeysPdf* bottom_1orMoreVTX_3orMoreTrackVTX_mass_pdf=new RooNDKeysPdf(flavourString+"_1orMoreVTX_3orMoreTrackVTX_mass_pdf",
									flavourString+"_1orMoreVTX_3orMoreTrackVTX_mass_pdf",
									*mass,
									*data_1orMoreVTX_3orMoreTrackVTX,
                                                             RooNDKeysPdf::NoMirror,1.);

  
  TH1F* bottom_1orMoreVTX_3orMoreTrackVTX_mass_histo=makeHisto(bottom_1orMoreVTX_3orMoreTrackVTX_mass_pdf,
						    flavourString+"_1orMoreVTX_3orMoreTrackVTX_mass_histo",
						    500,0,10000,
						    mass);

  TCanvas c9("c9","c9");						
  RooPlot* bottom_1orMoreVTX_3orMoreTrackVTX_mass_pdf_plot=mass->frame();
  data_1orMoreVTX_3orMoreTrackVTX->plotOn(bottom_1orMoreVTX_3orMoreTrackVTX_mass_pdf_plot);
  bottom_1orMoreVTX_3orMoreTrackVTX_mass_pdf->plotOn(bottom_1orMoreVTX_3orMoreTrackVTX_mass_pdf_plot);
  bottom_1orMoreVTX_3orMoreTrackVTX_mass_pdf_plot->Draw();
  c9.Update();
  c9.SaveAs(identificationFile+"_1orMoreVTX_3orMoreTrackVTX_mass_pdf_plot.eps");
  

  bottom_noVTX_significance3d_histo->Write();
  bottom_noVTX_1SingleTrack_energyFraction_normal_histo->Write();
  bottom_noVTX_2orMoreSingleTrack_energyFraction_normal_histo->Write();
  bottom_1orMoreVTX_2TrackVTX_energyFraction_normal_histo->Write();
  bottom_1orMoreVTX_3TrackVTX_energyFraction_normal_histo->Write();
  bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo->Write();
  bottom_1orMoreVTX_significance3d_histo->Write();
  bottom_1orMoreVTX_2TrackVTX_mass_histo->Write();
  bottom_1orMoreVTX_3orMoreTrackVTX_mass_histo->Write();
  filehistos->Write();
  filehistos->Close();
  

}


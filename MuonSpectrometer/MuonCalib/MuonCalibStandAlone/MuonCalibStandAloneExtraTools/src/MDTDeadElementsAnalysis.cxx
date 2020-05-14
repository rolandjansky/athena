/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneExtraTools/MDTDeadElementsAnalysis.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <cmath>
#include <fstream>  
#include <sstream>
#include <iostream>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h> 
#include <TFile.h> 
#include <TKey.h> 
#include <TCanvas.h>
#include <TStyle.h>

class ToString {
public:
  template< class T >
  std::string operator()( const T &i ) {
    std::ostringstream os;
    os << i;
    return os.str();
  }
};

MDTDeadElementsAnalysis::MDTDeadElementsAnalysis() :
  polfunc(NULL), deadTubesMap(NULL), geo(NULL), ndeadmezz(-1), ndeadtubes(-1),
  deadMezzDone(false), deadTubesDone(false), deadMultilayerDone(false),
  nlayers(-1), nlayers_per_ml(-1), ntubes_per_mezz(-1), chi2poly(-1.),
  average_entry(-1.), n_entry(-1.), nholes(-1) {
  verbose=false;
  drawPlots=false;
  writeResultsToAsciFile=false;
}

// ==============================================================================
//
// MDTDeadElementsAnalysis::histogramScanCalibCenters
//
// ==============================================================================

void MDTDeadElementsAnalysis::histogramScanCalibCenters(TFile *rootfile) {

    int nregionsanalysed=0;
    int nsectorsanalysed=0;

  if(rootfile->IsZombie()) {
    return;
  }

  int n_processed_chambers=0;

  std::string sector;
  std::string chamber;

  TH1F* hadccut;
  TH1F* hgeom; 
  TH1F* hdeadmap;
  TH2F* hdeadchannels;
  TH2F* hsummary;

  for (int iregion=1; iregion<=4; iregion++) {
    std::string region_name;
    if (iregion==1) region_name="Barrel_A";
    if (iregion==2) region_name="Barrel_C";
    if (iregion==3) region_name="Endcap_A";
    if (iregion==4) region_name="Endcap_C";
    std::string regiondir_name="/MDT/"+region_name;
    
    TDirectory* regiondir=(TDirectory*)rootfile->Get(regiondir_name.c_str());
    if(!regiondir) {
      continue;
    }
    
    hsummary = (TH2F*) regiondir->Get("DeadElements");
    if(hsummary) hsummary->Reset();
    
    nregionsanalysed++;

    for (int isector=1; isector<=16; isector++) {

      TString sector_name=(isector<10)?("Sector0"):("Sector");
      sector_name+=isector;
      TDirectory* sectordir=(TDirectory*)regiondir->Get((const char*)sector_name);

       nsectorsanalysed++;

       TIter next(sectordir->GetListOfKeys());
       TKey *key;
       while ( (key = (TKey*)next()) ) {
 	std::string chamber_name=key->GetName();
 	if (chamber_name=="OVERVIEW") continue;

 	TDirectory* chamberdir=(TDirectory*)sectordir->Get(chamber_name.c_str());
 	TDirectory* deadstatusdir=(TDirectory*)sectordir->Get((chamber_name+"/DeadStatus").c_str());
 	TDirectory* expertdir=(TDirectory*)sectordir->Get((chamber_name+"/Expert").c_str());

 	if(!chamberdir) {
	  continue;
	}
	
 	if(!deadstatusdir) {
	  continue;
	}
	
 	if(!expertdir) {
	  continue;
	}
       
 	hadccut = (TH1F*) chamberdir->Get("a_HitsPerTubeAdcCut");
 	hgeom = (TH1F*) expertdir->Get("DeadTubeRefMap");
 	hdeadmap = (TH1F*) expertdir->Get("DeadTubeMap");

        // RESET hdeadmap and make it a copy of hgeom :
        //
        int nbinshref=hgeom->GetNbinsX();
        int nbinshres=hdeadmap->GetNbinsX();
        if (nbinshref!=nbinshres) {
	  continue;
        }
        hdeadmap->Reset();
        for (int bin=1;bin<=nbinshref;bin++){
	  hdeadmap->SetBinContent(bin,hgeom->GetBinContent(bin));
        }
       
        // Clone a_HitsPerTubeAdcCut 
        //
	TH1F *HhitsCopy = (TH1F*)hadccut->Clone();
	getBasicGeometry(hgeom,chamber_name);
	deadElementsAnalysis(HhitsCopy,hgeom,hdeadmap,4,chamber_name);

	HhitsCopy->Delete();
	n_processed_chambers++;
      
	// report here the results in the 2D chamber view histogram:
	TH1F *hdeadtubestatus=(TH1F*)chamberdir->FindObjectAny("b_DeadTubeStatus");
	if(hdeadtubestatus) hdeadtubestatus->Reset();    

	TH2F *hdeadtubes=(TH2F*) deadstatusdir->FindObjectAny("ChamberDeadTubes");
	hdeadtubes->Reset(); 

	hdeadchannels = (TH2F*) expertdir->Get("ChamberDeadChannels");

	hdeadchannels->Reset();
	hdeadchannels->SetMinimum(0);
	hdeadchannels->SetMaximum(10);
	int totBins = hdeadmap->GetNbinsX();
	
	std::vector<int> ntubes_per_layer; 
	for(int j=1; j<=2;j++){
	  ntubes_per_layer.push_back(nbins_per_layer[j-1]);
	}
	
	if( chamber_name =="BIR1A11") ntubes_per_layer[0] = 30;
	if( chamber_name =="BIR4A11") ntubes_per_layer[0] = 30;
	if( chamber_name =="BIR1A15") ntubes_per_layer[0] = 30;
	if( chamber_name =="BIR4A15") ntubes_per_layer[0] = 30;


	for(int jML=1; jML<=2;jML++){

	  for(int jLay=1; jLay<=nlayers_per_ml;jLay++){

	    TString deadlayername="DeadTubes_ML";       
	    deadlayername+=jML;
	    deadlayername+="_L";
	    deadlayername+=jLay;
	    
	    TH1F *hdeadperlayer =(TH1F*)deadstatusdir->FindObjectAny((const char*)deadlayername);
	    if(hdeadperlayer) hdeadperlayer->Reset();
	    
	    
	    int bincounter=0;

	    //	    for(int jbin=1; jbin<=nbins_per_layer[jML-1];jbin++){
	    for(int jbin=1; jbin<=ntubes_per_layer[jML-1];jbin++){

	      int alltubes = (jML-1)*(nlayers_per_ml)*(ntubes_per_layer[jML-1]) + (jLay-1)*(ntubes_per_layer[jML-1])+jbin;

	      int ires = (int)hdeadmap->GetBinContent(alltubes);

	      if (ires==0) continue;
// 	      int iML = 1;
// 	      if (ires>0) iML = 2;
// 	      int iLy = (jbin-1)/(int)ntubes_per_layer[jML-1] + 1;
// 	      if (iML==2) iLy = iLy+1;
// 	      int iTube = (jbin-1)%ntubes_per_layer[jML-1] + 1;
	      int icode = ires%10;

	      if (icode<0) icode=-icode;
	      if (icode==0) icode=10;

	      double contr=0;

	      if(icode==0){ 
	      } else if(icode==10){ 
		contr=1;
	      } else if(icode==7 ){ 
	      } else{ 
	      }
	      double contr2=contr;
	      if(icode!=0 && contr==0) contr2=0.1;

	      bincounter++;
	      if(hdeadperlayer) hdeadperlayer->SetBinContent(bincounter,contr2);
	    }
	  }
	}
	
 	for (int bin=1;bin<=totBins;bin++){
	  int ires = (int)hdeadmap->GetBinContent(bin);
	  if (ires==0) continue;
	  int iML = 1;
	  if (ires>0) iML = 2;
	  //	    int iLy = (bin-1)/(int)nbins_per_layer[iML-1] + 1;
	  int iLy = (bin-1)/(int)ntubes_per_layer[iML-1] + 1;
	  if (iML==2) iLy = iLy+1;
	  //	    int iTube = (bin-1)%nbins_per_layer[iML-1] + 1;
	  int iTube = (bin-1)%ntubes_per_layer[iML-1] + 1;
	  int icode = ires%10;

	  if (icode<0) icode=-icode;
	  if (icode==0) icode=10;
	  hdeadchannels->SetBinContent(iTube,iLy,(float)icode);      

	  int Lay_offset = 0;
	  if(nlayers_per_ml ==3) Lay_offset = 1;
	  iLy = iLy + Lay_offset;

	  double contr=0;

	  if(icode==0){ 
	    hdeadtubes->SetBinContent(iTube,iLy,1); //white
	  } else if(icode==10){ 
	    contr=1;
	    hdeadtubes->SetBinContent(iTube,iLy,2); //green
	  } else if(icode==7 ){ 
	    hdeadtubes->SetBinContent(iTube,iLy,3); // yellow
	  } else { 
	    hdeadtubes->SetBinContent(iTube,iLy,4); // red
	  }
	  double contr2=contr;
	  if(icode!=0 && contr==0) contr2=0.1;

	  if(hdeadtubestatus) hdeadtubestatus->SetBinContent(bin,contr2);
	   
	}
	
	if (hsummary){
	  hsummary->Fill(isector,2, (float) deadChamber.size() );
	  hsummary->Fill(isector,3, (float) deadMultilayer.size() );
	  hsummary->Fill(isector,4, (float) deadLayer.size() );
	  hsummary->Fill(isector,5, (float) deadMezz.size() );
	  hsummary->Fill(isector,6, (float) deadTube.size() );
	}
      } // end loop on chambers       
    } // end loop over sectors
  } // end loop over regions BA/BC/EA/EC
}  //end MDTDeadElementsAnalysis::histogramScanCalibCenters

// ==============================================================================
//
// MDTDeadElementsAnalysis::histogramScanGnam
//
// ==============================================================================

void MDTDeadElementsAnalysis::histogramScanGnam(std::string rootfile){
 
  TFile f(rootfile.c_str());

  if(f.IsZombie()) {
    return;
  } 

  MDTDeadElementsAnalysis *mdtTubeAna = new MDTDeadElementsAnalysis;

  f.cd("MDT");
  TDirectory *dir = gDirectory;
  TIter next(dir->GetListOfKeys());
  TKey *key;
  int n_processed_chambers=0;

  while ((key=(TKey*)next())) {
    TString chambname=key->GetName();
    if(chambname!="Overview"){
      TString path1 = "BARREL_A" + chambname +  "/HitsPerTube_gt_100_ADC_counts";
      TString path2 = "DEBUG/MDT/" + chambname +  "/HitsPerTube<AdcCut";
      n_processed_chambers++;
    }//if !OVERVIEW
  }//key while 


  mdtTubeAna->printSummary();

  // --------- 
  f.Close();

  delete mdtTubeAna; mdtTubeAna=0;
  return;
}  //end MDTDeadElementsAnalysis::histogramScanGnam

// ==============================================================================
//
// MDTDeadElementsAnalysis::WriteAsciFile
//
// ==============================================================================
void MDTDeadElementsAnalysis::WriteAsciFile(){
  writeResultsToAsciFile = true;
} // WriteAsciFile

// ==============================================================================
//
// MDTDeadElementsAnalysis::getChamberName()
//
// ==============================================================================
std::string MDTDeadElementsAnalysis::getChamberName(){
  return chamberName;
} // getChamberName

// ==============================================================================
//
// MDTDeadElementsAnalysis::setChamberName(string chambname)
//
// ==============================================================================
void MDTDeadElementsAnalysis::setChamberName(std::string chambname){
  chamberName = chambname;
} // setChamberName

// ==============================================================================
//
// MDTDeadElementsAnalysis::setVerbose
//
// ==============================================================================
void MDTDeadElementsAnalysis::setVerbose(){
  verbose = true;
} // setVerbose

// ==============================================================================
//
// MDTDeadElementsAnalysis::setNoVerbose
//
// ==============================================================================
void MDTDeadElementsAnalysis::setNoVerbose(){
  verbose = false;
} // setNoVerbose

// ==============================================================================
//
// MDTDeadElementsAnalysis::deadElementsAnalysis
//
// ==============================================================================
void MDTDeadElementsAnalysis::deadElementsAnalysis(TH1F *idh1, TH1F *HRef, TH1F *deadTubesMap, int /*nlayers_ml*/, std::string chambname) {

  getBasicGeometry(HRef,chambname);
  // Initialization

  ClearList();
  int total_bins= (int) idh1->GetNbinsX();
  Clear(total_bins);
  setChamberName(chambname);
  // Processing
  chamberName=chambname;
  deadChambers(idh1,HRef, deadTubesMap, chambname);
  if (deadChamber.size()==0) deadMultilayers(idh1,HRef, deadTubesMap, chambname);
  if (deadChamber.size()==0) deadLayers(idh1,HRef, deadTubesMap, chambname);
  if (deadChamber.size()==0) deadMezzanines(idh1,HRef, deadTubesMap, chambname);
  if (deadChamber.size()==0) deadTubes(idh1,HRef, deadTubesMap, chambname);

  // Finalize
  if(verbose) {
    printSummary();
    if (drawPlots) {
      TCanvas *c1 = new TCanvas();
      c1->cd();
      deadTubesMap->Draw();
    }
  }    
  if (writeResultsToAsciFile){
    printCompactSummaryToAsciFile();
  }

  return;
}// deadElementsAnalysis

// ==============================================================================
//
// MDTDeadElementsAnalysis::deadElementsAnalysis
//
// ==============================================================================
void MDTDeadElementsAnalysis::deadElementsAnalysis(TH1F* idh1, TH1F* HRef, TH1F* deadTubesMap, 
						   int /*nlayers_ml*/, std::string chambname, int convert_mezzanine[20]){

  getBasicGeometry(HRef,chambname);
  // Initialization
  ClearList();
  int total_bins= (int) idh1->GetNbinsX();
  Clear(total_bins);
  setChamberName(chambname);
  // Processing
  chamberName=chambname;
  deadChambers(idh1,HRef, deadTubesMap, chambname);
  if (deadChamber.size()==0) deadMultilayers(idh1,HRef, deadTubesMap, chambname);
  if (deadChamber.size()==0) deadLayers(idh1,HRef, deadTubesMap, chambname);
  if (deadChamber.size()==0) deadMezzanines(idh1,HRef, deadTubesMap, chambname, convert_mezzanine);
  if (deadChamber.size()==0) deadTubes(idh1,HRef, deadTubesMap, chambname);

  // Finalize
  if(verbose) {
    printSummary();
    if (drawPlots) {
      TCanvas * c1 = new TCanvas();
      c1->cd();
      deadTubesMap->Draw();
    }
  }    
  if (writeResultsToAsciFile){
    printSummaryToAsciFile();
  }
 
  return;
}// deadElementsAnalysis

// ==============================================================================
//
// MDTDeadElementsAnalysis::deadChambers
//
// ==============================================================================
void MDTDeadElementsAnalysis::deadChambers(TH1F *idh1, TH1F */*HRef*/, TH1F */*deadTubesMap*/, std::string chambname){
  if (idh1->GetEntries() == 0.) deadChamber.push_back(chambname);
}// deadChambers


// ==============================================================================
//
// MDTDeadElementsAnalysis::deadLayers
//
// ==============================================================================
void MDTDeadElementsAnalysis::deadLayers(TH1F* idh1,TH1F* HRef, TH1F* deadTubesMap, std::string chambname){
  double layer_content[8];
  int layer_non_zero_bins[8];
  for(int ii=0; ii<8; ii++){
     layer_content[ii] = 0.;
     layer_non_zero_bins[ii] = 0;
  }
  int binstart = 1;
  int binend = nbins_per_layer[0];
  int indlayer=0;
  for(int ml=1;ml<3;ml++){
    // protection if only 1 ml
    if (nbins_per_layer[ml-1]==0) continue;
    for(int l=1;l<=nlayers_per_ml;l++){
      layer_content[indlayer]=0.;
      layer_non_zero_bins[indlayer-1]=0;
      for(int bin=binstart;bin<=binend;bin++){
	double c=idh1->GetBinContent(bin);
	layer_content[indlayer-1]=layer_content[indlayer-1]+c;
	if(c>0.)layer_non_zero_bins[indlayer-1]++;
      }
      binstart=binstart+nbins_per_layer[0];
      binend=binend+nbins_per_layer[0];
      indlayer++;
    } // loop layers of 1 ml
  } // loop 2 multilayers

  // analysis now: 1) minimum & average

  int minimum_position=1;
  int minimum_content=layer_non_zero_bins[0];
  double average_non_zero_bins=0.;
  for(int l=1;l<indlayer-1;l++){
    if(layer_non_zero_bins[l-1]<minimum_content){
      minimum_position=l;
      minimum_content=layer_non_zero_bins[l-1];
    }
    average_non_zero_bins = average_non_zero_bins + 
      double(layer_non_zero_bins[l-1]);
  }
  average_non_zero_bins = average_non_zero_bins - minimum_content;
  average_non_zero_bins = average_non_zero_bins / double(indlayer-1-1);

  // analysis 2) rms 

  double rms_non_zero_bins=0.;
  for(int l=1;l<indlayer;l++){
    if(l!=minimum_position){
      rms_non_zero_bins = std::pow(average_non_zero_bins - double(layer_non_zero_bins[l-1]),2)
	+ rms_non_zero_bins;
    }
  }
  rms_non_zero_bins = std::pow(rms_non_zero_bins/double(indlayer-1),0.5);
  if(rms_non_zero_bins<1.) rms_non_zero_bins=1.;

  // analysis 3) spot dead layer when average-3*rms/(layers-1)>content of minimum && rms!=1
  // analysys 4) average bin content < 20.
  float average_bin_content = (float)layer_content[minimum_position-1]/(float)nbins_per_layer[0];
  bool test = average_non_zero_bins-8.*rms_non_zero_bins/std::pow(double(indlayer-1),0.5)>minimum_content
    &&rms_non_zero_bins!=1.&&average_bin_content<10.;
  if(test) {
    int tubestart = (minimum_position-1)*nbins_per_layer[0]+1;
    int tubeend = minimum_position*nbins_per_layer[0];
    deadLayer.push_back(minimum_position);
    deadLayerChamber.push_back(chambname);
    for(int j=tubestart;j<=tubeend;j++){
      Double_t cgeo = HRef->GetBinContent(j);
      Double_t signML = cgeo/std::abs(cgeo);
      int igeo = (int) cgeo;
      Int_t refquantity = igeo%10;
      if(std::abs(cgeo)!=0.&& refquantity == 0){
	deadTubesMap->Fill(j,4.*signML);
      }
    }
  }
  //  verbose=false;
}// deadLayers

// ==============================================================================
//
// MDTDeadElementsAnalysis::getBasicGeometry
//
// ==============================================================================

void MDTDeadElementsAnalysis::getBasicGeometry(TH1F* HRef, int nlayers_ml, std::string chambname){
  int nbins=HRef->GetNbinsX();
  schamber = chambname;
  chtype = schamber.substr(1,1);
  nlayers_per_ml = nlayers_ml;
  int firstbinML1=1;
  int firstbinML2=10000;
  int lastbinML1=-1;
  int lastbinML2=-1;
  for(int bin=1;bin<=nbins;bin++){
    float content = HRef->GetBinContent(bin);
    if(content<0.) {
      if(bin>lastbinML1) lastbinML1 = bin;
    } 
    if(content>0.) {
      if(bin<firstbinML2) firstbinML2 = bin;
      if(bin>lastbinML2) lastbinML2 = bin;
    }
  }

  firstbinML[0]=firstbinML1;
  firstbinML[1]=firstbinML2;
  lastbinML[0]=lastbinML1;
  lastbinML[1]=lastbinML2;

  nbins_per_layer[0] = (lastbinML[0]-firstbinML[0]+1)/nlayers_ml;
  nbins_per_layer[1] = (lastbinML[1]-firstbinML[1]+1)/nlayers_ml;
  ntubes_per_mezz=6;
  if(chtype=="O" || chtype=="M") {
    nlayers=6;
    ntubes_per_mezz=8;
  }
  for(int MultiLayer=0;MultiLayer<2;MultiLayer++){
    nmezz[MultiLayer]=nbins_per_layer[MultiLayer]/ntubes_per_mezz;
    // Algos parameters
    ntubes[MultiLayer]=15;
    tstep[MultiLayer]=5.;
    if(nbins_per_layer[MultiLayer]==36){
      ntubes[MultiLayer]=18;
      tstep[MultiLayer]=6.;
    }
    if(nbins_per_layer[MultiLayer]==72){
      ntubes[MultiLayer]=12;
      tstep[MultiLayer]=6.;
    }
    if(nbins_per_layer[MultiLayer]==64){
      ntubes[MultiLayer]=16;
      tstep[MultiLayer]=8.;
    }
    if(nbins_per_layer[MultiLayer]==48){
      ntubes[MultiLayer]=16;
      tstep[MultiLayer]=8.;
    }
    if(nbins_per_layer[MultiLayer]==56){
      ntubes[MultiLayer]=14;
      tstep[MultiLayer]=7.;
    }
  }
} // getBasicGeometry

  // ==============================================================================
  //
  // MDTDeadElementsAnalysis::getBasicGeometry
  //
  // ==============================================================================

void MDTDeadElementsAnalysis::getBasicGeometry(TH1F* HRef, std::string chambname){
  int nbins=HRef->GetNbinsX();
  schamber = chambname;
  chtype = schamber.substr(1,1);
  int firstbinML1=10000;
  int firstbinML2=10000;
  int lastbinML1=-1;
  int lastbinML2=-1;
  int nlayers_ml1=0;
  int nlayers_ml2=0;
  for(int bin=1;bin<=nbins;bin++){
    float content = HRef->GetBinContent(bin);
    if(content<0.) {
      float f_level=std::abs(content)/10.;
      int   i_level= (int) f_level;
      if(i_level==(int) f_level && i_level>nlayers_ml1) {
	nlayers_ml1=i_level;
      } 
      if(i_level<(int) f_level && i_level+1>nlayers_ml1) {
	nlayers_ml1=i_level+1;
      }
      if(bin<firstbinML1) firstbinML1 = bin;
      if(bin>lastbinML1) lastbinML1 = bin;
    } 
    if(content>0.) {
      float f_level=std::abs(content)/10.;
      int   i_level= (int) f_level;
      if(i_level==(int) f_level && i_level>nlayers_ml2) {
	nlayers_ml2=i_level;
      } 
      if(i_level<(int) f_level && i_level+1>nlayers_ml2) {
	nlayers_ml2=i_level+1;
      }
      if(bin<firstbinML2) firstbinML2 = bin;
      if(bin>lastbinML2) lastbinML2 = bin;
    }
  }
  if(nlayers_ml1>0&&nlayers_ml2>0){
    // two real multilayers
    if(lastbinML1+1!=firstbinML2){
      // cut out multilayer
      lastbinML1=nbins/2;
      firstbinML2=lastbinML1+1;
    }
  }
  firstbinML[0]=firstbinML1;
  firstbinML[1]=firstbinML2;
  lastbinML[0]=lastbinML1;
  lastbinML[1]=lastbinML2;
  nbins_per_layer[0]=0;
  nbins_per_layer[1]=0;
  if(nlayers_ml1>0){
    nbins_per_layer[0] = (lastbinML[0]-firstbinML[0]+1)/nlayers_ml1;
  }
  if(nlayers_ml2>0){
    nbins_per_layer[1] = (lastbinML[1]-firstbinML[1]+1)/nlayers_ml2;
  }
  ntubes_per_mezz=6;
  nlayers_per_ml=nlayers_ml1;
  if(chtype=="O" || chtype=="M") {
    nlayers=6;
    ntubes_per_mezz=8;
  }
  for(int MultiLayer=0;MultiLayer<2;MultiLayer++){
    nmezz[MultiLayer]=nbins_per_layer[MultiLayer]/ntubes_per_mezz;
    // Algos parameters
    ntubes[MultiLayer]=15;
    tstep[MultiLayer]=5.;
    if(nbins_per_layer[MultiLayer]==36){
      ntubes[MultiLayer]=18;
      tstep[MultiLayer]=6.;
    }
    if(nbins_per_layer[MultiLayer]==72){
      ntubes[MultiLayer]=12;
      tstep[MultiLayer]=6.;
    }
    if(nbins_per_layer[MultiLayer]==64){
      ntubes[MultiLayer]=16;
      tstep[MultiLayer]=8.;
    }
    if(nbins_per_layer[MultiLayer]==56){
      ntubes[MultiLayer]=14;
      tstep[MultiLayer]=7.;
    }
  }

} // getBasicGeometry

  // ==============================================================================
  //
  // MDTDeadElementsAnalysis::printSummary
  //
  // ==============================================================================

void MDTDeadElementsAnalysis::printSummary(){

}// printSummary
// ==============================================================================
//
// MDTDeadElementsAnalysis::printSummaryToAsciFile
//
// ==============================================================================

void MDTDeadElementsAnalysis::printSummaryToAsciFile(){
  std::ofstream deadAna;
  deadAna.open("deadana.dat",std::ios::app);

  unsigned im=0;
  if(noisyChamber.size()+deadMultilayer.size()+deadLayer.size()+deadMezz.size()+deadTube.size()>0)
    deadAna<<"Examine chamber "<<chamberName<<std::endl;
  
  if(noisyChamber.size()>0){
    deadAna<<"number of noisy chambers "<<noisyChamber.size()<<std::endl;
    for(im=0;im<noisyChamber.size();im++){
      deadAna<<" "<<noisyChamber[im]<<std::endl;
    }
  }

  if(deadMultilayer.size()>0){
    deadAna<<"number of dead MultiLayers "<<deadMultilayer.size()<<std::endl;
    for(im=0;im<deadMultilayer.size();im++){
      deadAna<<" dead MultiLayer "<<deadMultilayer[im]<<" found in chamber "<<deadMLChamber[im]<<std::endl;
    }
  }

  if(deadLayer.size()>0){
    deadAna<<"number of dead Layers "<<deadLayer.size()<<std::endl;
    for(im=0;im<deadLayer.size();im++){
      deadAna<<" dead Layer "<<deadLayer[im]<<" found in chamber "<<deadLayerChamber[im]<<std::endl;
    }
  }

  if(deadMezz.size()>0){
    deadAna<<"number of dead mezz "<<deadMezz.size()<<std::endl;
    for(im=0;im<deadMezz.size();im++){
      deadAna <<" chamber "<<deadMezzChamber[im]<<" mezzanine "<<deadMezz[im]<<
	" reduced numbering mezzanine "<<deadMezz_red[im]<<
	" csm mezzanine "<<deadMezz_csm[im]<<
	" dead in layer "<<deadMezzML[im]<<std::endl;
    }
  }

  if(deadTube.size()>0){
    deadAna<<"number of dead tubes "<<deadTube.size()<<std::endl;
    for(im=0;im<deadTube.size();im++){
      deadAna<<" "<<deadTubeChamber[im]<<" "<<deadTube[im]<<" "<<deadTubeML[im]<<" "<<deadTubeLay[im]<<std::endl;
    }
  }

}// printSummaryToAsciFile

// ==============================================================================
//
// MDTDeadElementsAnalysis::printCompactSummaryToAsciFile
//
// ==============================================================================


void MDTDeadElementsAnalysis::printCompactSummaryToAsciFile(){

  std::ofstream deadAna;
  deadAna.open("deadana.dat",std::ios::app);

  unsigned im=0;

  if(deadChamber.size()>0){
    deadAna<<chamberName<< " dead_CHAMBER "<<std::endl;
  }

  if(deadMultilayer.size()>0){
    for(im=0;im<deadMultilayer.size();im++){
      deadAna<<deadMLChamber[im]<< " dead_MULTILAYER "<<deadMultilayer[im]<<std::endl;
    }
  }

  if(deadLayer.size()>0){
    for(im=0;im<deadLayer.size();im++){
      int lay=deadLayer[im];
      int ml=1;
      if (lay>nlayers_per_ml) {
	lay=lay-nlayers_per_ml; 
	ml=2;
      }
      deadAna<<deadLayerChamber[im]<< " dead_LAYER "<<ml<<" "<<lay<<std::endl;
    }
  }

  if(deadMezz.size()>0){
    for(im=0;im<deadMezz.size();im++){
      deadAna <<deadMezzChamber[im]<<" dead_MEZZANINE "<<deadMezz[im]<<
	" reduced numbering mezzanine "<<deadMezz_red[im]<<
	" csm mezzanine "<<deadMezz_csm[im]<<
	" dead in MultiLayer "<<deadMezzML[im]<<std::endl;
    }
  }

  if(deadTube.size()>0){
    for(im=0;im<deadTube.size();im++){
      deadAna<<deadTubeChamber[im]<<" dead_TUBE "<<deadTubeML[im]<<" "<<deadTubeLay[im]<<" "<<deadTube[im]<<std::endl;
    }
  }

}// printCompactSummaryToAsciFile
// ==============================================================================
//
// MDTDeadElementsAnalysis::NoisyChamber
//
// ==============================================================================

bool MDTDeadElementsAnalysis::NoisyChamber(TH1F* idh1, std::string chambname){

  double binsbelow2=0;
  double binsabove1=0;
  int nbins=idh1->GetNbinsX();
  int Entries=0;
  for(int bin=1;bin<=nbins;bin++){
    Entries = Entries + (int) idh1->GetBinContent(bin);
  }
  double bins = (double) nbins;
  for (int nb=1;nb<=nbins;nb++){
    if (idh1->GetBinContent(nb)<2.) {
      binsbelow2++;
    } else {
      binsabove1++;
    }
  }    

  double ratio=0.;

  if (Entries>0) {
    ratio= binsbelow2 / bins;}
  bool Noise=false;
  if (ratio>0.5) {
    Noise=true;
    noisyChamber.push_back(chambname);
  }  

  return Noise;

}// NoisyChamber

// ==============================================================================
//
// MDTDeadElementsAnalysis::ClearList
//
// ==============================================================================

void MDTDeadElementsAnalysis::ClearList(){

  noisyChamber.clear();
  deadChamber.clear();
  deadLayer.clear();
  deadLayerChamber.clear();
  deadMLChamber.clear();
  deadMLCounts.clear();
  deadMLLevel.clear();
  deadMultilayer.clear();
  deadMezzChamber.clear();
  deadMezz.clear();
  deadMezzML.clear();
  deadMezz_csm.clear();
  deadMezz_red.clear();
  deadMezzEffi.clear();
  deadMezzErrEffi.clear();
  deadMezzChi2.clear();
  deadTube.clear();
  deadTubeId.clear();
  deadTubeHistoBin.clear();
  deadTubeChamber.clear();
  deadTubeLay.clear();
  deadTubeML.clear();
  deadTubeChi2.clear();
  deadTubeEffi.clear();
  deadTubeErrEffi.clear();

} // ClearList

  // ==============================================================================
  //
  // MDTDeadElementsAnalysis::Clear
  //
  // ==============================================================================

void MDTDeadElementsAnalysis::Clear(int ntubes){
           
  chamberName="NULL"; 
  deadMezzDone=false;
  deadTubesDone=false;
  ndeadmezz=0;
  for(int i=0;i<nmmax;i++){
    deadmezz[i]=0;
    deadmezzMLay[i]=0;
  }
  ndeadtubes=0;
  if(ntubes>ntmax || ntubes<0) ntubes=ntmax;
  for (int i=0;i<ntubes;i++){
    deadtube[i]=0;     
    deadtubeseed[i]=0.;
    deadtubelen[i]=0.;
    deadtubeLay[i]=0;
    deadtubeML[i]=0;
    deadtubechi2[i]=0;
    deadtubeeffi[i]=0;
    deadtubeerr[i]=0;
    for(int j=0;j<3;j++) deadtubestat[i][j]=0;
  }

}// Clear       

// ==============================================================================
//
// MDTDeadElementsAnalysis::deadTubes
//
// ==============================================================================

void MDTDeadElementsAnalysis::deadTubes(TH1F* idh, TH1F* HRef, TH1F* deadTubesMap, std::string chambname){

  std::string codepart="MDTDeadElementsAnalysis";
  std::string option="Q";

  TH1F * idh1 = (TH1F*)idh->Clone();

  int tottubes= (int) idh1->GetNbinsX();
  for (int nb=1;nb<=tottubes;nb++){
    if(idh1->GetBinContent(nb)==0.) {
      // test test test
      float cref=HRef->GetBinContent(nb);
      if(cref!=0.) {
	idh1->SetBinError(nb,2.3);
      }
    }
  }

  MDTDeadElementsAnalysis::deadTubesDone=true;
 
  int nbins=idh1->GetNbinsX();
  int Entries=0;
  for(int bin=1;bin<=nbins;bin++){
    Entries = Entries + (int) idh1->GetBinContent(bin);
  }
  float averageEntries = float(Entries) / float(nbins);
  if(Entries>4000 || averageEntries > 10.){

    double chi2cut=30.;
    if(Entries>50000.) {
      chi2cut=60.;
    }

    std::vector<double> parf(7, 0.0);
    polfunc = new TF1("fit0",polnml3,1.,(double)2.*ntubes[0],4);
    TF1 *fitfunc = new TF1("fit1",completefunc,1.,(double)2.*ntubes[0],7);

    // loop over ML's
    for(int ML=0;ML<2;ML++){
      if (nbins_per_layer[ML]==0) continue;
      /*
	Make sure not to loop over a dead ML
      */
      bool MLdead = false;
      for(unsigned int im=0;im<deadMultilayer.size();im++){
	if(deadMultilayer[im]==ML+1) MLdead = true;
      }
      if(MLdead) {
	break;
      }
      double tubestart=1.+ML*(firstbinML[1]-1);
      fitfunc->SetParameters(-51.,180.,-10.,0.,9.,1.,0.5);

      for (int nlay=1; nlay<=nlayers_per_ml;nlay++){
	double tube0 = (nlay-1)*nbins_per_layer[ML]+1+ML*nbins_per_layer[1]*nlayers_per_ml;
	double tube1 = tube0 + nbins_per_layer[ML]-1;

	for (tubestart = tube0; tubestart <=tube1-ntubes[ML]+1; tubestart=tubestart+tstep[ML]) {
	  // here the scan starts; you have a sliding fit interval which goes from
	  // t0start to t0start + ntubes[0]

	  // if a polnml3 fit gives a good chi2 -> no dead tube
	  //

	  option="Q";
	  if(verbose) {
	    option="";
	  }
	  if (verbose){
	    idh1->SetMinimum(0.);
	    idh1->SetAxisRange(tubestart-2.5,tubestart+ntubes[ML]+1.5);
	    idh1->Fit(polfunc,option.c_str(),"",(double)tubestart,(double)tubestart+ntubes[ML]-1);
	    if (drawPlots) { 
	      TCanvas *c1 = new TCanvas();
	      c1->WaitPrimitive();
	      int inutile;
	      std::cin>>inutile;
            }
	  } else {
	    idh1->Fit(polfunc,option.c_str(),"",(double)tubestart,(double)tubestart+ntubes[ML]-1);
	  }
	  double chi2pol3=polfunc->GetChisquare();
          for (int n=0;n<=3;n++) polyparf[n]=polfunc->GetParameter(n);
	  if(chi2pol3<chi2cut) continue;

	  double chi2min=99999999.;
	  double xbinmin;
	  int ibinmin = 0;

	  average_entry=0.;
	  n_entry=0.;

	  int firsttube = int (tubestart);
	  int mtubes = int(ntubes[ML]);
	  getSpikesInInterval(idh1,firsttube,firsttube+mtubes-1,polfunc);
	  getHolesInInterval(idh1,firsttube,firsttube+mtubes-1,mtubes, polfunc);
	  getHolesInInterval_strategy5(idh1,firsttube,firsttube+mtubes-1,mtubes, polfunc);

	  std::string oldoption=option;
	  //	  setVerbose();
	  if(average_entry==0) continue;

	  double err[10];
	  // remove all holes from the fit!
	  for(int nh=0;nh<nholes;nh++){
	    // put error=0.-> bin does not count in fit
	    err[nh]=idh1->GetBinError(holes[nh]);
	    idh1->SetBinError(holes[nh],0.);
	  }
	  // loop over all holes !

	  for(int nh=0;nh<nholes;nh++){
	    option="Q";
	    if(verbose) {
	      option="";
	    }
	    // stop analysis if eff_hole is > .4
	    if(eff_hole[nh]>0.4) continue;
	    for(int i=0;i<4;i++){
	      fitfunc->SetParameter(i,polyparf[i]);
	    }
	    ibinmin = holes[nh];
	    xbinmin=idh1->GetBinCenter(ibinmin);
	    if(err[nh]>0.) idh1->SetBinError(ibinmin,err[nh]);
	    if(err[nh]==0.) idh1->SetBinError(ibinmin,2.3);
	    idh1->SetBinError(ibinmin,2.3);
	    fitfunc->SetParameter(6,1.);
	    fitfunc->SetParameter(4,xbinmin);
	    fitfunc->SetParameter(5,1.);
	    fitfunc->SetParLimits(4,0.,36.);
	    fitfunc->SetParLimits(6,-2.,2.);    // TEST TEST 
	    chi2min = 99999999999999.;
	    double lenmin;
	    double effimin;
	    double errmin;
	    double binminfit;
	    double len=1.;
	    // prepare complete fit
	    for(int ipar=0;ipar<4;ipar++){
	      fitfunc->SetParameter(ipar,polyparf[ipar]);
	    }
	    fitfunc->SetParameter(4,xbinmin);
	    fitfunc->FixParameter(4,xbinmin);
	    fitfunc->SetParameter(5,len);
	    fitfunc->FixParameter(5,len);
	    fitfunc->SetParameter(6,eff_hole[nh]);
	    option = option+"B";
	    idh1->Fit("fit1",option.c_str(),"",(double)tubestart-0.5,(double)tubestart+ntubes[0]-0.5);
	    if(verbose){
	      idh1->SetMinimum(0.);
	    }
	    double chi2 = fitfunc->GetChisquare();
	    double deltachi2=chi2pol3-chi2;
	    if((chi2<chi2cut&&fitfunc->GetParError(6)>.1)||deltachi2/chi2pol3>.3){
	      option = option+"E";        // TEST TEST
	      if(average_entry<50.) option=option+"L";
	      //	if error on hole efficiency if > 5% refine fit using option 'E'

	      if(verbose){
		idh1->Fit("fit1",option.c_str(),"",(double)tubestart-0.5,(double)tubestart+ntubes[ML]-0.5);
	      } else {
		idh1->Fit("fit1",option.c_str(),"",(double)tubestart-0.5,(double)tubestart+ntubes[ML]-0.5);
	      }
	      option = oldoption;
	    }

	    chi2min=chi2;
	    if(average_entry<50.) chi2min=-chi2min;
	    lenmin=len;
	    effimin=fitfunc->GetParameter(6);
	    errmin=fitfunc->GetParError(6);
	    binminfit=fitfunc->GetParameter(4);

	    // the command below (->Wait) is very usefull
	    // corresponds to the 'wait' of PAW

	    if(verbose&&effimin<0.9){
	      idh1->SetAxisRange(tubestart-2.5,tubestart+ntubes[ML]+1.5);
	      if (drawPlots) { 
		TCanvas *c1 = new TCanvas();
		idh1->Draw();
		c1->WaitPrimitive();
		int inutile;
		std::cin>>inutile;
              }
	    }

	    // every fit gives a dead tube by definition 
	    // the selection of real dead tubes is done afterward

	    // count dead tubes candidates & fill vectors

	    int ib=(int) (binminfit);//round it to next integer
	    binminfit=float(ib)+0.5;//put it in the middle of the bin!
	    ndeadtubes++;
	    deadtubeseed[ndeadtubes-1]=ibinmin;
	    deadtube[ndeadtubes-1]=binminfit;
	    deadtubelen[ndeadtubes-1]=lenmin;
	    deadtubechi2[ndeadtubes-1]=chi2min;
	    deadtubeeffi[ndeadtubes-1]=effimin;
	    deadtubeerr[ndeadtubes-1]=errmin;

	    // compute now difference between polynomial at the position of the dead tube
	    // and compute [poly-hist(dead-tube)]/stat-error

	    // double mrtest=0.;

	    double mrtest=0.;

	    for(double mr=deadtube[ndeadtubes-1];mr<deadtube[ndeadtubes-1]+deadtubelen[ndeadtubes-1];mr++){
	      double emr=idh1->GetBinError(int(mr));
	      if(emr==0.) emr=2.3;
	      double cmr=idh1->GetBinContent(int(mr));
	      double pol=parf[0]
		+mr*parf[1]
		+mr*mr*parf[2]
		+mr*mr*mr*parf[3];
	      double diff = pol-cmr;
	      if(diff>0.)mrtest=diff;
	      if(emr!=0.){mrtest=diff/emr;}
	      int index=(int) (mr-deadtube[ndeadtubes-1]);
	      deadtubestat[ndeadtubes-1][index]=mrtest;
	    }//for mr=

	    // put back stat error of the hole to 0
	    idh1->SetBinError(ibinmin,0.);
	  } // end loop holes
	    // put back error!
	  for(int nh=0;nh<nholes;nh++){
	    idh1->SetBinError(ibinmin,err[nh]);
	  }

	  // end complete fit ==================

	}// loop tubestart
      } //loop nlayers

    } // end loop over ML's

      //gabri
    validateDeadTubes(chi2cut,HRef,deadTubesMap,chambname);
  } //if(Entries>0)
  delete idh1;
}// deadTubes

// ==============================================================================
//
// MDTDeadElementsAnalysis::validateDeadTubes
//
// ==============================================================================

std::string MDTDeadElementsAnalysis::validateDeadTubes(double chi2cut, TH1F *HRef, TH1F *deadTubesMap, std::string chambname){
  for (int nd=0;nd<ndeadtubes;nd++){
    Double_t cgeo = HRef->GetBinContent(int(deadtube[nd]));
    bool stat_significance = ((1.- deadtubeeffi[nd])/deadtubeerr[nd] > 7.)||
      (deadtubestat[nd][0]>5.);
    if (deadtubeerr[nd]==0.) continue;
    double effective_cut=chi2cut;
    if(deadtubechi2[nd])effective_cut=chi2cut*6.;
    if (std::abs(deadtubechi2[nd])< effective_cut && stat_significance &&deadtubeeffi[nd]<=0.2
	&& cgeo !=0.){
      for(int nt=0;nt<1;nt++){
	int nlay = (int)(((int) deadtube[nd]-1)/nbins_per_layer[0])+1;
        int ML=(int)((nlay-1)/nlayers_per_ml)+1;
	int tube_in_layer=((int) deadtube[nd])-(nlay-1)*nbins_per_layer[0];
	if(nlay>nlayers_per_ml) nlay=nlay-nlayers_per_ml;
	bool alreadyFound = false;
	Double_t cdead = deadTubesMap->GetBinContent(int(deadtube[nd]));
	// already found in deadTubesMap
	int idead = (int) cdead;
	Int_t deadquantity = idead%10;
	if (deadTubeHistoBin.size()>0) {
	  if ( deadtube[nd] == deadTubeHistoBin.back() ){
	    alreadyFound = true;
	  }
	}
	if (!alreadyFound&& deadquantity==0) {
	  deadTubeHistoBin.push_back(deadtube[nd]);
	  deadTube.push_back(tube_in_layer);
	  deadTubeId.push_back(ML*1000+nlay*100+tube_in_layer);
	  deadTubeChamber.push_back(chambname);   
	  deadTubeLay.push_back(nlay);
	  deadTubeML.push_back(ML); 
	  deadTubeEffi.push_back(deadtubeeffi[nd]); 
	  deadTubeErrEffi.push_back(deadtubeerr[nd]); 
	  deadTubeChi2.push_back(deadtubechi2[nd]);  
	  Double_t c = (Double_t) deadtube[nd];
	  Double_t cgeo = HRef->GetBinContent(int(c));
	  Double_t signML = cgeo/std::abs(cgeo);
	  int igeo = (int) cgeo;
	  Int_t refquantity = igeo%10;
	  if(std::abs(cgeo)!=0.&& refquantity ==0){
	    deadTubesMap->Fill(int(c),2.*signML);
	  }
	}

      } //for mt = bin
    }// end first test ==================
  } // for nd=0, dead tubes
  return "end processing";
}//deadTubes

// ==============================================================================
//
// MDTDeadElementsAnalysis::getHolesInInterval_strategy5
//
// ==============================================================================

void MDTDeadElementsAnalysis::getHolesInInterval_strategy5(TH1F* idh1, int firsttube, int lasttube, int /*ntubes*/, TF1 * /*polfunc*/){
  TH1F * distr_of_entries = new TH1F("","",1000,0.,1000.);
  for(int ib=firsttube; ib<=lasttube;ib++){
    float cont = idh1->GetBinContent(ib);
    distr_of_entries->Fill(cont);
}
  MsgStream log(Athena::getMessageSvc(),"MDTDeadElementsAnalysis");
  TF1 *f1 = new TF1("f1", "gaus", 1, 1000);
  distr_of_entries->Fit("f1", "RLQ0");
  double mean = f1->GetParameter(2);
  double sigma = f1->GetParameter(3);
  for(int ib=1;ib<=mean-3.*sigma;ib++){
    double cont = distr_of_entries->GetBinContent(ib);
    if(cont!=0) {
      for(int ib2=firsttube; ib2<=lasttube;ib2++){
        float cont2 = idh1->GetBinContent(ib2);
        if(cont2==float(ib-1)) {
          log<<MSG::WARNING<<"bin "<<ib2<<"is dead or inefficient"<<endmsg;
        }
      }
    }
  }

} // getHolesInInterval
// ==============================================================================
//
// MDTDeadElementsAnalysis::getHolesInInterval
//
// ==============================================================================

void MDTDeadElementsAnalysis::getHolesInInterval(TH1F* idh1, int firsttube, int lasttube, int ntubes, TF1 *polfunc){
  double better_average=average_entry;
  double better_entry =n_entry;
  std::vector<double> parf(7, 0.0);
  std::string codepart="GetHolesInInterval";
  double bestchi2 = 1000000.;
  chi2poly=0.;
  polfunc->SetParameter(0,polyparf[0]);
  polfunc->SetParameter(1,polyparf[1]);
  polfunc->SetParameter(2,polyparf[2]);
  polfunc->SetParameter(3,polyparf[3]);
  idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
  polyparf[0]=polfunc->GetParameter(0);
  polyparf[1]=polfunc->GetParameter(1);
  polyparf[2]=polfunc->GetParameter(2);
  polyparf[3]=polfunc->GetParameter(3);
  double chi2_pol=polfunc->GetChisquare();
  nholes=0;

  //  ================== start strategy average > 100 && chi2_pol<100.
  // first try comparing wrt average entry

  if(average_entry>100.&& chi2_pol<100.){

    polyparf[0]=average_entry;
    polyparf[1]=0.;
    polyparf[2]=0.;
    polyparf[3]=0.;
    double olderror[10];
    for(int j=0;j<10;j++){holes[j]=0;}
    for(int ibin=firsttube;ibin<=lasttube;ibin++){
      double xbin=double(ibin);
      double c=idh1->GetBinContent(ibin);
      double cerr=idh1->GetBinError(ibin);
      double polinomial=polyparf[0]
	+xbin*polyparf[1]
	+xbin*xbin*polyparf[2]
	+xbin*xbin*xbin*polyparf[3];
      double residual = c-average_entry;
      // new def!! Use pol fit with all points unless chi2 is very bad
      residual=c-polinomial;
      if(residual<-3.*cerr&&nholes<10&&c/average_entry<0.9){
	if(nholes<10) nholes++;
	olderror[nholes-1]=cerr;
	holes[nholes-1]=(int) xbin;
	eff_hole[nholes-1]=c/average_entry;
	idh1->SetBinError(ibin,0.);
      }
    } //for(ibin
    for(int ih=0;ih<nholes;ih++){
      int holepos=holes[ih];
      double cerr=olderror[ih];
      idh1->SetBinError(holepos,cerr);
    }

  }

    //  ================== end strategy average > 100 

    //  ================== refine strategy average > 100 if chi2>50

  if(average_entry>100.&& chi2_pol>50.){
    nholes=0;
    for(int j=0;j<10;j++){holes[j]=0;}
    idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
    chi2poly = polfunc->GetChisquare();
    bestchi2 = chi2poly;
    int bestposition = 0;
    double besteff = 1.;
    for(int ibin=firsttube;ibin<=lasttube;ibin++){
      double xbin=double(ibin);
      double olderr = idh1->GetBinError(ibin);
      idh1->SetBinError(ibin,0.);
      idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
      chi2_pol = polfunc->GetChisquare();
      if(bestchi2>chi2_pol){ 
	bestchi2 = chi2_pol;
	bestposition = ibin;
	parf[0]=polfunc->GetParameter(0);
	parf[1]=polfunc->GetParameter(1);
	parf[2]=polfunc->GetParameter(2);
	parf[3]=polfunc->GetParameter(3);
	double polinomial=parf[0]
	  +xbin*parf[1]
	  +xbin*xbin*parf[2]
	  +xbin*xbin*xbin*parf[3];
	double c=idh1->GetBinContent(ibin);
	besteff=c/polinomial;
	for(int i=0;i<4;i++){polyparf[i]=parf[i];}
      } //if(bestchi2 ...
      idh1->SetBinError(ibin,olderr);
    } //for (int bin
    if(bestchi2<350.){
      nholes=1;
      holes[0]=bestposition;
      eff_hole[0]=besteff;
    } //if(effi ...
  } 

    //
    //  INSERT HERE NEW PATCH FOR TWO TUBES !!!!
    // 
  // if still bad try removing two bins ...

  //  if(average_entry>100.&& chi2_pol>50.){
  if(average_entry>100.&& bestchi2>50.){
    nholes=0;
    int bestposition1 = 0;
    double besteff1 = 1.;
    int bestposition2 = 0;
    double besteff2 = 1.;

    for(int j=0;j<10;j++){holes[j]=0;}
    idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
    chi2poly = polfunc->GetChisquare();
    bestchi2 = chi2poly;
    for(int ibin=firsttube;ibin<lasttube;ibin++){
      double xbin1=double(ibin);
      double olderr1 = idh1->GetBinError(ibin);
      idh1->SetBinError(ibin,0.);
      for(int lbin=ibin+1;lbin<=lasttube;lbin++){
	double xbin2=double(lbin);
	double olderr2 = idh1->GetBinError(lbin);
	idh1->SetBinError(lbin,0.);

	idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
	chi2_pol = polfunc->GetChisquare();
	if(bestchi2>chi2_pol){ 
	  bestchi2 = chi2_pol;
	  bestposition1 = ibin;
	  bestposition2 = lbin;
	  parf[0]=polfunc->GetParameter(0);
	  parf[1]=polfunc->GetParameter(1);
	  parf[2]=polfunc->GetParameter(2);
	  parf[3]=polfunc->GetParameter(3);
	  double polinomial=parf[0]
	    +xbin1*parf[1]
	    +xbin1*xbin1*parf[2]
	    +xbin1*xbin1*xbin1*parf[3];
	  double c=idh1->GetBinContent(ibin);
	  besteff1=c/polinomial;
	  polinomial=parf[0]
	    +xbin2*parf[1]
	    +xbin2*xbin2*parf[2]
	    +xbin2*xbin2*xbin2*parf[3];
	  c=idh1->GetBinContent(lbin);
	  besteff2=c/polinomial;
	  for(int i=0;i<4;i++){polyparf[i]=parf[i];}
	}
	idh1->SetBinError(lbin,olderr2);
      }
      if(bestchi2<50.){
	nholes=2;
	holes[0]=bestposition1;
	eff_hole[0]=besteff1;
	holes[1]=bestposition2;
	eff_hole[1]=besteff2;
      }
      idh1->SetBinError(ibin,olderr1);
    }
  } //if(average>100.&&chi2poly>50.)



    //  ================== end refine strategy average > 100 if chi2>50
    //
    //  INSERT HERE NEW PATCH FOR THREE TUBES IN SEQUENCE !!!!
    // 
  // if still bad try removing three bins ...
  if(average_entry>100.&& bestchi2>50.){
    //==================================================
    nholes=0;
    int bestposition1 = 0;
    double besteff1 = 1.;
    int bestposition2 = 0;
    double besteff2 = 1.;
    int bestposition3 = 0;
    double besteff3 = 1.;
    for(int j=0;j<10;j++){holes[j]=0;}
    idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
    chi2poly = polfunc->GetChisquare();
    bestchi2 = chi2poly;
    double initial_pol_chi2= chi2poly;
    double olderr1(0.0);
    double olderr2(0.0);
    double olderr3(0.0);
    for(int ibin=firsttube;ibin<lasttube-3;ibin++){
      double xbin1=double(ibin);
      olderr1 = idh1->GetBinError(ibin);
      double xbin2=double(ibin+1);
      olderr2 = idh1->GetBinError(ibin+1);
      double xbin3=double(ibin+2);
      olderr3 = idh1->GetBinError(ibin+2);
      idh1->SetBinError(ibin,0.);
      idh1->SetBinError(ibin+1,0.);
      idh1->SetBinError(ibin+2,0.);
      idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
      chi2_pol = polfunc->GetChisquare();
      if(bestchi2>chi2_pol){ 
	bestchi2 = chi2_pol;
	bestposition1 = ibin;
	bestposition2 = ibin+1;
	bestposition3 = ibin+2;
	parf[0]=polfunc->GetParameter(0);
	parf[1]=polfunc->GetParameter(1);
	parf[2]=polfunc->GetParameter(2);
	parf[3]=polfunc->GetParameter(3);

	double polinomial=parf[0]
	  +xbin1*parf[1]
	  +xbin1*xbin1*parf[2]
	  +xbin1*xbin1*xbin1*parf[3];
	double c=idh1->GetBinContent(ibin);
	besteff1=c/polinomial;
	polinomial=parf[0]
	  +xbin2*parf[1]
	  +xbin2*xbin2*parf[2]
	  +xbin2*xbin2*xbin2*parf[3];
	c=idh1->GetBinContent(ibin+1);
	besteff2=c/polinomial;
	c=idh1->GetBinContent(ibin);
	besteff1=c/polinomial;
	polinomial=parf[0]
	  +xbin3*parf[1]
	  +xbin3*xbin3*parf[2]
	  +xbin3*xbin3*xbin3*parf[3];
	c=idh1->GetBinContent(ibin+2);
	besteff3=c/polinomial;
	for(int i=0;i<4;i++){polyparf[i]=parf[i];}
      }
      if(bestchi2<50.){
	if(besteff1<0.2&&besteff2<0.2&&besteff3<0.2){
	  nholes=3;
	  holes[0]=bestposition1;
	  eff_hole[0]=besteff1;
	  holes[1]=bestposition2;
	  eff_hole[1]=besteff2;
	  holes[2]=bestposition3;
	  eff_hole[2]=besteff3;
	}
      }
      idh1->SetBinError(ibin,olderr1);
      idh1->SetBinError(ibin+1,olderr2);
      idh1->SetBinError(ibin+2,olderr3);
    } //for (int ibin
    //==================================================
    double gain = bestchi2/initial_pol_chi2;
    if(gain<0.3) {      
      int ibin1=bestposition1;
      idh1->SetBinError(ibin1,0.);
      idh1->SetBinError(ibin1+1,0.);
      idh1->SetBinError(ibin1+2,0.);

    for(int ibin=firsttube;ibin<lasttube;ibin++){
      if(ibin<ibin1||ibin>ibin1+2){
      idh1->Fit(polfunc,"Q","",(double)firsttube,(double)lasttube);
      chi2_pol = polfunc->GetChisquare();
      if(bestchi2>chi2_pol){ 
	bestchi2 = chi2_pol;
	parf[0]=polfunc->GetParameter(0);
	parf[1]=polfunc->GetParameter(1);
	parf[2]=polfunc->GetParameter(2);
	parf[3]=polfunc->GetParameter(3);
	double xbin1=double(ibin);
	double polinomial=parf[0]
	  +xbin1*parf[1]
	  +xbin1*xbin1*parf[2]
	  +xbin1*xbin1*xbin1*parf[3];
	double c=idh1->GetBinContent(ibin);
	besteff1=c/polinomial;

	for(int i=0;i<4;i++){polyparf[i]=parf[i];}
      }
      idh1->SetBinError(ibin,olderr1);
      idh1->SetBinError(ibin+1,olderr2);
      idh1->SetBinError(ibin+2,olderr3);
      }//if(ibin<ibin1 ...
    } //for (int ibin

    }
  } //if(average>100.&&chi2poly>50.)



    //  ================== end refine strategy average > 100 if chi2>50 for 3 tubes in sequence


    //  ================== start strategy average < 100

  if(average_entry<100.){
    nholes=0;
    for(int j=0;j<10;j++){holes[j]=0;}
    better_average=0.;
    better_entry =0.;
    for(int nb=firsttube;nb<=lasttube;nb++){
      double c=idh1->GetBinContent(nb);
      double cerr=idh1->GetBinError(nb);
      /*
	tag 'holes' candidates using criterium c deviates from average by more than 4stat-errors
	remember not to use bins of dead mezzanines tagged using SetBinError(nb,0.) in deadMezz
      */
      if((average_entry-c)>3.5*std::sqrt(average_entry)&&n_entry>double(ntubes)-3.){
	position_holes[nb]=1;
	if(nholes<10){
	  // this is one dead tube candidate!
	  // remember eff_hole, to cut on it
	  nholes++;
	  holes[nholes-1]=nb;
	  eff_hole[nholes-1]=c/average_entry;
	} 
      }else {
	if(cerr>0.){
	  better_average=better_average+c;
	  better_entry++;
	}
      }
    } // for int=nb..
    if(better_entry>0.) better_average = better_average/better_entry;
    polyparf[0]=better_average;
    for(int i=1;i<4;i++){polyparf[i]=0.;}
  } // else if average_entry<100.

    //  ================== end strategy average < 100
  average_entry=better_average;
} // getHolesInInterval

  // ==============================================================================
  //
  // MDTDeadElementsAnalysis::getAverageInInterval
  //
  // ==============================================================================

void MDTDeadElementsAnalysis::getAverageLevelInInterval(TH1F* idh1, int firsttube, int lasttube){
  average_entry=0.;
  n_entry=0.;
  for(int nb=firsttube;nb<=lasttube;nb++){
    average_entry=average_entry+idh1->GetBinContent(nb);
    n_entry++;
  }
  average_entry=average_entry/n_entry;
}
// ==============================================================================
//
// MDTDeadElementsAnalysis::getSpikesInInterval
//
// ==============================================================================

void MDTDeadElementsAnalysis::getSpikesInInterval(TH1F* idh1, int firsttube, int lasttube, TF1  * /*polfunc*/){

  average_entry=0.;
  n_entry=0.;
  for(int nb=firsttube;nb<=lasttube;nb++){
    average_entry=average_entry+idh1->GetBinContent(nb);
    n_entry++;
  }

  average_entry=average_entry/n_entry;
  // look for max entries

  //	int position_spikes[1000];
  for(int index=0; index<ntmax; index++){position_spikes[index]=0;}
  double old_average=average_entry;

  while(1) {
    // remove bins 'well' above average until average gets stable
    // if a bin has been removed already position_spikes[nb]=1
    double largest_num_entries=0.;
    int pos_largest_num_entries=0;
    for(int nb=firsttube;nb<=lasttube;nb++){
      double c=idh1->GetBinContent(nb);
      if(c>largest_num_entries&&position_spikes[nb]!=1){
	largest_num_entries=c;
	pos_largest_num_entries=nb;
      }
    }
    // check average when bin with max entries is removed!
    // if largest bin accounts for more than 80% of total -> spike

    if(largest_num_entries>1.5*average_entry) {
      position_spikes[pos_largest_num_entries]=1;
      average_entry=(average_entry*n_entry-largest_num_entries)/(n_entry-1.);
      n_entry--;
      // if error of one bin is put to zero-> bin not used in the fit
      idh1->SetBinError(pos_largest_num_entries,0.);
    }
    double ratio=99999.;
    if(old_average>0.) ratio=average_entry/old_average;
    if( ratio>0.8 || n_entry<=6 ) break;  

    old_average=average_entry;

  }//end while
}

// ==============================================================================
//
// Polnml3
//
// ==============================================================================
Double_t polnml3(Double_t * x, Double_t * parf) {

  Double_t poly=parf[0]
    +x[0]*parf[1]
    +x[0]*x[0]*parf[2]
    +x[0]*x[0]*x[0]*parf[3];

  return poly;
}//polnml3

// ==============================================================================
//
// completefunc
//
// ==============================================================================

double completefunc(double * x, double * parf) {

  double poly=parf[0]
    +x[0]*parf[1]
    +x[0]*x[0]*parf[2]
    +x[0]*x[0]*x[0]*parf[3];

  double eff=1.;
  int tubefit = (int) parf[4];
  double par4=tubefit;
  if(x[0]>par4-0.5 && x[0]<par4+std::abs(parf[5])-0.5) {
    eff=parf[6];
  }
  double value=poly*eff;

  return value;
}//completefunc

// ==============================================================================
//
// MDTDeadElementsAnalysis::deadMezzanines
//
// ==============================================================================

void MDTDeadElementsAnalysis::deadMezzanines(TH1F* idh, TH1F* HRef, TH1F* deadTubesMap, std::string chambname){

  TH1F * idh1 = (TH1F*)idh->Clone();

  MDTDeadElementsAnalysis::deadMezzDone=true;

  int Entries=0;
  int nbins=idh1->GetNbinsX();
  for(int bin=1;bin<=nbins;bin++){
    Entries = Entries + (int) idh1->GetBinContent(bin);
  }

  std::ostringstream deadAna;
  double cmax = -1.;
  int pos_maximum = -1;
  for(int ib=1; ib<=nbins; ib++){
    double cont=idh1->GetBinContent(ib);
    if(cont>cmax) {
      cmax=cont;
      pos_maximum = ib;
    }
  }

  if(Entries-cmax>1000) {
    TH1F * hmezz = new TH1F("hmezz","hmezz",
			    nbins_per_layer[0]+nbins_per_layer[1],0.5,
			    nbins_per_layer[0]+nbins_per_layer[1]+0.5);
    TH1F * hmezzRef = new TH1F("hmezzRef","hmezzRef",
			       nbins_per_layer[0]+nbins_per_layer[1],0.5,
			       nbins_per_layer[0]+nbins_per_layer[1]+0.5);
    hmezz->SetTitle(chambname.c_str());
    hmezzRef->SetTitle(chambname.c_str());
    for(int MultiLayer=0;MultiLayer<2;MultiLayer++){
      if (nbins_per_layer[MultiLayer]==0) continue;
      int ibstart = (MultiLayer)*nbins_per_layer[0]*nlayers_per_ml+1;
      int ibend = (MultiLayer)*nbins_per_layer[0]*nlayers_per_ml
	+nbins_per_layer[MultiLayer]*nlayers_per_ml;
      for(int ib=ibstart; ib<=ibend; ib++){
	int nlay = (ib-firstbinML[MultiLayer])/nbins_per_layer[MultiLayer]+1;
	int ML=MultiLayer+1;
	int tube_in_layer=ib-(nlay-1)*nbins_per_layer[MultiLayer]-firstbinML[MultiLayer];
	if(nlay>nlayers_per_ml) nlay=nlay-nlayers_per_ml; 
	double cont=idh1->GetBinContent(ib);
	double contRef=HRef->GetBinContent(ib);
	hmezz->Fill(tube_in_layer+(ML-1)*nbins_per_layer[MultiLayer]+1,cont);
	hmezzRef->Fill(tube_in_layer+(ML-1)*nbins_per_layer[MultiLayer]+1,contRef);
      }
    }
    // treat correctly zeros & compute average
    // remember bins with zero entries (I should rather say zero error!) are NOT included in a chi2 fit
    // thus to take them into account you need to give them a non zero error

    average_entry=0.;
    n_entry=0.;
    for (int nb=1;nb<=nbins_per_layer[0]*2;nb++){
      average_entry=average_entry+hmezz->GetBinContent(nb);
      n_entry++;
      double contRef=std::abs(hmezzRef->GetBinContent(nb));
      if (hmezz->GetBinContent(nb)==0.&&contRef!=nlayers_per_ml&&contRef!=0.) {
	hmezz->SetBinError(nb,2.3);
      }
    }             
    average_entry=average_entry/n_entry;
    // look for max entries

    for(int index=0; index<ntmax; index++){position_spikes[index]=0;}
    double old_average=average_entry;

    while(1) {
      // remove bins 'well' above average until average gets stable
      // if a bin has been removed already position_spikes[nb]=1
      double largest_num_entries=0.;
      int pos_largest_num_entries=0;
      for (int nb=1;nb<=nbins_per_layer[0]*2;nb++){
	double c=hmezz->GetBinContent(nb);
	if (c>largest_num_entries&&position_spikes[nb]!=1){
	  largest_num_entries=c;
	  pos_largest_num_entries=nb;
	}
      }
      // check average when bin with max entries is removed!
      // if largest bin accounts for more than 80% of total -> spike

      if(largest_num_entries>1.5*average_entry) {
	position_spikes[pos_largest_num_entries]=1;
	average_entry=(average_entry*n_entry-largest_num_entries)/(n_entry-1.);
	n_entry--;
	// if error of one bin is put to zero-> bin not used in the fit
	hmezz->SetBinError(pos_largest_num_entries,0.);
      }
      double ratio=99999.;
      if(old_average>0.) ratio=average_entry/old_average;
      if( ratio>0.8 || n_entry<=6 ) break;  

      old_average=average_entry;

    }//end while

    double chi2cut=30.;
    if(Entries>50000.) chi2cut=60.;
    TF1 *fitfunc = new TF1("fit1",completefunc,1.,(double)2.*ntubes[0],7);
    polfunc = new TF1("fit0",polnml3,1.,(double)2.*ntubes[0],4);
    fitfunc->SetParameters(-51.,180.,-10.,0.,9.,1.,0.5);
    for(int ml=1;ml<=2;ml++){
      if (nbins_per_layer[ml-1]==0) continue;
      double fit_start=1.+(ml-1)*nbins_per_layer[1];
      double fit_end=nbins_per_layer[0]+(ml-1)*nbins_per_layer[1];
      if(verbose){
	hmezz->Fit(polfunc,"","",(double)fit_start,(double)fit_end);
      } else {
	hmezz->Fit(polfunc,"Q","",(double)fit_start,(double)fit_end);
      }
      hmezz->SetMinimum(0.);
      hmezz->SetMaximum(500.);
      double chi2 = polfunc->GetChisquare();
      double ndf= nbins_per_layer[0]-4;
      if(chi2/ndf > 10 )	{
	hmezz->SetBinError(pos_maximum,0.);
	if(verbose) {
	  hmezz->Fit(polfunc,"","",(double)fit_start,(double)fit_end);
	} else {
	  hmezz->Fit(polfunc,"Q","",(double)fit_start,(double)fit_end);
	}

	double olderr[8];
        for(int ii=0; ii<8; ii++) olderr[ii] = 0.;
	for (int imezz=1+(ml-1)*nmezz[1];imezz<=nmezz[0]+(ml-1)*nmezz[1];imezz++){
	  for (int ic=(imezz-1)*ntubes_per_mezz+1+(ml-1)*nbins_per_layer[1];
	       ic<=(imezz)*ntubes_per_mezz+(ml-1)*nbins_per_layer[1];ic++){
	    int index = (ic-1)%8;
	    olderr[index]=hmezz->GetBinError(ic);
	    hmezz->SetBinError(ic,0.);
	  }
	  hmezz->Fit(polfunc,"Q","",(double)fit_start,(double)fit_end);
	  if(verbose){
            if (drawPlots) { 
	      TCanvas *c1 = new TCanvas();
	      hmezz->Draw();
	      c1->WaitPrimitive();
	      int inutile;
	      std::cin>>inutile;
            }
	  }
	  // chi2 = poly->GetChisquare();
	  chi2 = polfunc->GetChisquare();
	  fitfunc->SetParameter(0,polfunc->GetParameter(0));
	  fitfunc->SetParameter(1,polfunc->GetParameter(1));
	  fitfunc->SetParameter(2,polfunc->GetParameter(2));
	  fitfunc->SetParameter(3,polfunc->GetParameter(3));
	  fitfunc->FixParameter(4,(imezz-1)*ntubes_per_mezz+1.);
	  fitfunc->FixParameter(5,ntubes_per_mezz);
	  fitfunc->SetParameter(6,.1);
	  fitfunc->SetParLimits(6,-2.,2.);

	  for(int ic=(imezz-1)*ntubes_per_mezz+1+(ml-1)*nbins_per_layer[0]
		;ic<=(imezz)*ntubes_per_mezz+(ml-1)*nbins_per_layer[0];ic++){
	    int index = (ic-1)%8;
	    hmezz->SetBinError(ic,olderr[index]);
	  }

	  if(!verbose){
	    hmezz->Fit("fit1","BQ","",(double)fit_start,(double)fit_end);
	  } else {
	    hmezz->Fit("fit1","B","",(double)fit_start,(double)fit_end);
            if (drawPlots) { 
	      TCanvas *c1 = new TCanvas();
	      hmezz->Draw();
	      c1->WaitPrimitive();
	      int inutile;
	      std::cin>>inutile;
            }
	  }
	  double chi2 = fitfunc->GetChisquare();
	  double efficiency = fitfunc->GetParameter(6);
	  double err_efficiency = fitfunc->GetParError(6);
	  if(err_efficiency>0.05) {
	    if(verbose){
	      hmezz->Fit("fit1","BEL","",(double)fit_start,(double)fit_end);
            } else {
	      hmezz->Fit("fit1","BQEL","",(double)fit_start,(double)fit_end);
            }
	    efficiency = fitfunc->GetParameter(6);
	    err_efficiency = fitfunc->GetParError(6); 
	  }
	  if(chi2/ndf<chi2cut&&(1.-efficiency)/err_efficiency>7&&efficiency<0.3){


	    deadMezz.push_back(imezz);
	    int imezz_red=imezz%(nmezz[ml-1]+1);
	    deadMezz_red.push_back(imezz_red);
	    deadMezzChamber.push_back(chambname);
	    deadMezzML.push_back(ml);
	    deadMezzEffi.push_back(efficiency);
	    deadMezzErrEffi.push_back(err_efficiency);
	    deadMezzChi2.push_back(chi2);
	    deadMezz_csm.push_back(-1);
	    int layer_start=(ml-1)*nlayers_per_ml+1; 
	    int layer_end=nlayers_per_ml*ml;
	    for(int l=layer_start;l<=layer_end;l++){
	      int tube_start=1+(imezz-1)*ntubes_per_mezz+(l-1)*nbins_per_layer[0];
	      int tube_end=tube_start+ntubes_per_mezz-1;
	      for(int t=tube_start;t<=tube_end;t++){
		Double_t cgeo = HRef->GetBinContent(t);
		Double_t signML = cgeo/std::abs(cgeo);
		int igeo = (int) cgeo;
		Int_t refquantity = igeo%10;
		if(std::abs(cgeo)!=0. && refquantity==0){
		  idh1->SetBinError(t,0.);

		  deadTubesMap->Fill(t,3.*signML);
		}
	      }
	    }
	  }
	} // for mezz
      } // if(chi2)
    } // for ml
    hmezz->Delete();
    hmezzRef->Delete();
  } // if(Entries..

  delete idh1;

  return;

}//deadMezzanines


// ==============================================================================
//
// MDTDeadElementsAnalysis::deadMezzanines
//
// ==============================================================================

void MDTDeadElementsAnalysis::deadMezzanines(TH1F* idh, TH1F* HRef, TH1F* deadTubesMap, 
					     std::string chambname, int /*convert_mezzanine*/[20]){

  TH1F * idh1 = (TH1F*)idh->Clone();

  MDTDeadElementsAnalysis::deadMezzDone=true;

  int Entries=0;
  int nbins=idh1->GetNbinsX();
  for(int bin=1;bin<=nbins;bin++){
    Entries = Entries + (int) idh1->GetBinContent(bin);
  }

  std::ostringstream deadAna;
  double cmax = -1.;
  int pos_maximum = -1;
  for(int ib=1; ib<=nbins; ib++){
    double cont=idh1->GetBinContent(ib);
    if(cont>cmax) {
      cmax=cont;
      pos_maximum = ib;
    }
  }

  if(Entries-cmax>1000) {
    TH1F * hmezz = new TH1F("hmezz","hmezz",
			    nbins_per_layer[0]+nbins_per_layer[1],0.5,
			    nbins_per_layer[0]+nbins_per_layer[1]+0.5);
    TH1F * hmezzRef = new TH1F("hmezzRef","hmezzRef",
			       nbins_per_layer[0]+nbins_per_layer[1],0.5,
			       nbins_per_layer[0]+nbins_per_layer[1]+0.5);
    hmezz->SetTitle(chambname.c_str());
    hmezzRef->SetTitle(chambname.c_str());
    for(int MultiLayer=0;MultiLayer<2;MultiLayer++){
      if (nbins_per_layer[MultiLayer]==0) continue;
      int ibstart = (MultiLayer)*nbins_per_layer[0]*nlayers_per_ml+1;
      int ibend = (MultiLayer)*nbins_per_layer[0]*nlayers_per_ml
	+nbins_per_layer[MultiLayer]*nlayers_per_ml;
      for(int ib=ibstart; ib<=ibend; ib++){
	int nlay = (ib-firstbinML[MultiLayer])/nbins_per_layer[MultiLayer]+1;
	int ML=MultiLayer+1;
	int tube_in_layer=ib-(nlay-1)*nbins_per_layer[MultiLayer]-firstbinML[MultiLayer];
	if(nlay>nlayers_per_ml) nlay=nlay-nlayers_per_ml; 
	double cont=idh1->GetBinContent(ib);
	double contRef=HRef->GetBinContent(ib);
	hmezz->Fill(tube_in_layer+(ML-1)*nbins_per_layer[MultiLayer]+1,cont);
	hmezzRef->Fill(tube_in_layer+(ML-1)*nbins_per_layer[MultiLayer]+1,contRef);
      }
    }
    // treat correctly zeros & compute average
    // remember bins with zero entries (I should rather say zero error!) are NOT included in a chi2 fit
    // thus to take them into account you need to give them a non zero error

    average_entry=0.;
    n_entry=0.;
    for (int nb=1;nb<=nbins_per_layer[0]*2;nb++){
      average_entry=average_entry+hmezz->GetBinContent(nb);
      n_entry++;
      double contRef=std::abs(hmezzRef->GetBinContent(nb));
      if (hmezz->GetBinContent(nb)==0.&&contRef!=nlayers_per_ml&&contRef!=0.) {
	hmezz->SetBinError(nb,2.3);
      }
    }             
    average_entry=average_entry/n_entry;
    // look for max entries

    for(int index=0; index<ntmax; index++){position_spikes[index]=0;}
    double old_average=average_entry;

    while(1) {
      // remove bins 'well' above average until average gets stable
      // if a bin has been removed already position_spikes[nb]=1
      double largest_num_entries=0.;
      int pos_largest_num_entries=0;
      for (int nb=1;nb<=nbins_per_layer[0]*2;nb++){
	double c=hmezz->GetBinContent(nb);
	if (c>largest_num_entries&&position_spikes[nb]!=1){
	  largest_num_entries=c;
	  pos_largest_num_entries=nb;
	}
      }
      // check average when bin with max entries is removed!
      // if largest bin accounts for more than 80% of total -> spike

      if(largest_num_entries>1.5*average_entry) {
	position_spikes[pos_largest_num_entries]=1;
	average_entry=(average_entry*n_entry-largest_num_entries)/(n_entry-1.);
	n_entry--;
	// if error of one bin is put to zero-> bin not used in the fit
	hmezz->SetBinError(pos_largest_num_entries,0.);
      }
      double ratio=99999.;
      if(old_average>0.) ratio=average_entry/old_average;
      if( ratio>0.8 || n_entry<=6 ) break;  

      old_average=average_entry;

    }//end while
    double chi2cut=30.;
    if(Entries>50000.) chi2cut=60.;

    polfunc = new TF1("fit0",polnml3,1.,(double)2.*ntubes[0],4);
    TF1 *fitfunc = new TF1("fit1",completefunc,1.,(double)2.*ntubes[0],7);
    fitfunc->SetParameters(-51.,180.,-10.,0.,9.,1.,0.5);
    for(int ml=1;ml<=2;ml++){
      if (nbins_per_layer[ml-1]==0) continue;
      double fit_start=1.+(ml-1)*nbins_per_layer[1];
      double fit_end=nbins_per_layer[0]+(ml-1)*nbins_per_layer[1];
      if(verbose){
	hmezz->Fit(polfunc,"","",(double)fit_start,(double)fit_end);
      } else {
	hmezz->Fit(polfunc,"Q","",(double)fit_start,(double)fit_end);
      }
      hmezz->SetMinimum(0.);
      hmezz->SetMaximum(500.);
      double chi2 = polfunc->GetChisquare();
      double ndf= nbins_per_layer[0]-4;
      if(chi2/ndf > 10 )	{
	hmezz->SetBinError(pos_maximum,0.);
	if(verbose) {
	  hmezz->Fit(polfunc,"","",(double)fit_start,(double)fit_end);
	} else {
	  hmezz->Fit(polfunc,"Q","",(double)fit_start,(double)fit_end);
	}

	double olderr[8];
        for(int ii=0; ii<8; ii++) olderr[ii] = 0.;
	for (int imezz=1+(ml-1)*nmezz[1];imezz<=nmezz[0]+(ml-1)*nmezz[1];imezz++){
	  for (int ic=(imezz-1)*ntubes_per_mezz+1+(ml-1)*nbins_per_layer[1];
	       ic<=(imezz)*ntubes_per_mezz+(ml-1)*nbins_per_layer[1];ic++){
	    int index = (ic-1)%8;
	    olderr[index]=hmezz->GetBinError(ic);
	    hmezz->SetBinError(ic,0.);
	  }
	  hmezz->Fit(polfunc,"Q","",(double)fit_start,(double)fit_end);
	  if(verbose){
            if (drawPlots) {
	      TCanvas *c1 = new TCanvas();
	      hmezz->Draw();
	      c1->WaitPrimitive();
	    }
	  }
	  chi2 = polfunc->GetChisquare();
	  fitfunc->SetParameter(0,polfunc->GetParameter(0));
	  fitfunc->SetParameter(1,polfunc->GetParameter(1));
	  fitfunc->SetParameter(2,polfunc->GetParameter(2));
	  fitfunc->SetParameter(3,polfunc->GetParameter(3));
	  fitfunc->FixParameter(4,(imezz-1)*ntubes_per_mezz+1.);
	  fitfunc->FixParameter(5,ntubes_per_mezz);
	  fitfunc->SetParameter(6,.1);
	  fitfunc->SetParLimits(6,-2.,2.);

	  for(int ic=(imezz-1)*ntubes_per_mezz+1+(ml-1)*nbins_per_layer[0]
		;ic<=(imezz)*ntubes_per_mezz+(ml-1)*nbins_per_layer[0];ic++){
	    int index = (ic-1)%8;
	    hmezz->SetBinError(ic,olderr[index]);
	  }

	  if(!verbose){
	    hmezz->Fit("fit1","BQ","",(double)fit_start,(double)fit_end);
	  } else {
	    hmezz->Fit("fit1","B","",(double)fit_start,(double)fit_end);
            if (drawPlots) {
	      TCanvas *c1 = new TCanvas();
	      hmezz->Draw();
	      c1->WaitPrimitive();
            }
	  }
	  double chi2 = fitfunc->GetChisquare();
	  double efficiency = fitfunc->GetParameter(6);
	  double err_efficiency = fitfunc->GetParError(6);
	  if(err_efficiency>0.05) {
	    if(verbose){
	      hmezz->Fit("fit1","BEL","",(double)fit_start,(double)fit_end);
            } else {
	      hmezz->Fit("fit1","BQEL","",(double)fit_start,(double)fit_end);
            }
	    efficiency = fitfunc->GetParameter(6);
	    err_efficiency = fitfunc->GetParError(6); 
	  }
	  if(chi2/ndf<chi2cut&&(1.-efficiency)/err_efficiency>7&&efficiency<0.3){


	    deadMezz.push_back(imezz);
	    int imezz_red=imezz%(nmezz[ml-1]+1);
	    deadMezz_red.push_back(imezz_red);
	    deadMezzChamber.push_back(chambname);
	    deadMezzML.push_back(ml);
	    deadMezzEffi.push_back(efficiency);
	    deadMezzErrEffi.push_back(err_efficiency);
	    deadMezzChi2.push_back(chi2);
	    deadMezz_csm.push_back(-1);
	    int layer_start=(ml-1)*nlayers_per_ml+1; 
	    int layer_end=nlayers_per_ml*ml;
	    for(int l=layer_start;l<=layer_end;l++){
	      int tube_start=1+(imezz-1)*ntubes_per_mezz+(l-1)*nbins_per_layer[0];
	      int tube_end=tube_start+ntubes_per_mezz-1;
	      for(int t=tube_start;t<=tube_end;t++){
		Double_t cgeo = HRef->GetBinContent(t);
		Double_t signML = cgeo/std::abs(cgeo);
		int igeo = (int) cgeo;
		Int_t refquantity = igeo%10;
		if(std::abs(cgeo)!=0. && refquantity==0){
		  idh1->SetBinError(t,0.);

		  deadTubesMap->Fill(t,3.*signML);
		}
	      }
	    }
	  }
	} // for mezz
      } // if(chi2)
    } // for ml
    hmezz->Delete();
    hmezzRef->Delete();
  } // if(Entries..

  delete idh1;

  return;

}//deadMezzanines

// ==============================================================================
//
// MDTDeadElementsAnalysis::deadMultilayers
//
// ==============================================================================

void MDTDeadElementsAnalysis::deadMultilayers(TH1F *idh1,  TH1F *HRef, TH1F *deadTubesMap, std::string chambname){

  MDTDeadElementsAnalysis::deadMultilayerDone=true;
  std::ostringstream deadAna;
  int nbins=idh1->GetNbinsX();

  int nML = 2;
  if ( nbins_per_layer[1]==0 ) nML=1;
  int binsML1 = nbins/nML;
  int binsML2 = binsML1;
  if ( nML==1 ) {
    deadAna << "DeadMultilayers Analysis SKIPPED since chamber "<<chambname<<" has 1 ML only "<<std::endl;
    return ;
  }

  polfunc = new TF1("fit0",polnml3,1.,(double)2.*ntubes[0],4);
  getSpikesInInterval(idh1,1,binsML1,polfunc);
  float isolatedCountsML1 = 0.;
  float nonisolatedCountsML1 = 0.;
  float averageLevelML1 = average_entry;
  for(int j=2;j<binsML1;j++){
    if(position_spikes[j]!=1) {
      if(idh1->GetBinContent(j-1)==0&&idh1->GetBinContent(j+1)==0)isolatedCountsML1++;
      if(idh1->GetBinContent(j-1)>0&&idh1->GetBinContent(j+1)>0)nonisolatedCountsML1++;
    }
  }

  getSpikesInInterval(idh1,binsML1+1,binsML1+binsML2,polfunc);
  float isolatedCountsML2 = 0.;
  float nonisolatedCountsML2 = 0.;
  float averageLevelML2 = average_entry;
  for(int j=binsML1+2;j<binsML1+binsML2;j++){
    if(position_spikes[j]!=1) {
      if(idh1->GetBinContent(j-1)==0&&idh1->GetBinContent(j+1)==0)isolatedCountsML2++;
      if(idh1->GetBinContent(j-1)>0&&idh1->GetBinContent(j+1)>0)nonisolatedCountsML2++;
    }
  }
  // ML1

  if(float(nonisolatedCountsML1)/float(binsML1)<.2||
     std::abs(averageLevelML1/averageLevelML2)<0.1) {
    deadAna <<" dead MultiLayer 1 found in chamber "<<chambname<<std::endl;
    deadMultilayer.push_back(1);
    deadMLChamber.push_back(chambname);
    deadMLCounts.push_back(nonisolatedCountsML1);
    deadMLLevel.push_back(averageLevelML1);
    for(int j=1;j<=binsML1;j++){
      Double_t cgeo = HRef->GetBinContent(j);
      Double_t signML = cgeo/std::abs(cgeo);
      int igeo = (int) cgeo;
      Int_t refquantity = igeo%10;
      if(std::abs(cgeo)!=0.&& refquantity==0){
	deadTubesMap->Fill(j,5.*signML);
      }
    }

  } else {
    deadAna << " MultiLayer 1 in chamber is alive"<<std::endl;
  }
  // ML2

  if(float(nonisolatedCountsML2)/float(binsML2)<.2||
     std::abs(averageLevelML2/averageLevelML1)<0.1) {
    deadAna <<" dead MultiLayer 2 found in chamber "<<chambname<<std::endl;
    deadMultilayer.push_back(2);
    deadMLChamber.push_back(chambname);
    deadMLCounts.push_back(nonisolatedCountsML2);
    deadMLLevel.push_back(averageLevelML2);
    for(int j=binsML1+1;j<=binsML1+binsML2;j++){
      Double_t cgeo = HRef->GetBinContent(j);
      Double_t signML = cgeo/std::abs(cgeo);
      int igeo = (int) cgeo;
      Int_t refquantity = igeo%10;
      if(std::abs(cgeo)!=0.&& refquantity==0){
	deadTubesMap->Fill(j,5.*signML);
      }
    }
  } else {
    deadAna << " MultiLayer 2 in chamber is alive"<<std::endl;
  }
  return;
}//deadMultilayers

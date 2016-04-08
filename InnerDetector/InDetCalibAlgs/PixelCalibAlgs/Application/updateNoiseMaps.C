/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<vector>
#include<cmath>
#include<map>
#include<sstream>
#include<string>
#include<iostream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TKey.h"
#include "Riostream.h"

#include "PixelCalibAlgs/PixelConvert.h"
#include "PixelConditionsData/SpecialPixelMap.h"

double ComputeMuChip(TH2D* modhisto,int chip) {
  static const int nchip = 16; 
  static const int ncol  = 18; 
  static const int nrow  = 160; // not encoded in ModuleSpecialPixelMap
  static const int nlongcol=2;
  static const int ngangedrow=4;
  static const int totalpixels = nchip*ncol*nrow;
  static const double fr10 = (ncol-nlongcol)*(nrow-ngangedrow)/double(totalpixels); // fraction of normal pixels
  static const double fr15 = nlongcol*(nrow-ngangedrow)/double(totalpixels);        // fraction of long pixels
  static const double fr20 = (ncol-nlongcol)*ngangedrow/double(totalpixels);        // fraction of ganged pixels
  static const double fr30 = nlongcol*ngangedrow/double(totalpixels);               // fraction of long-ganged pixels
  double totalhits=0.;
  int rowmin=0, colmin=0; 
  if (chip<(nchip/2)) {
    rowmin=nrow+8; 
    colmin = (nchip/2-chip-1)*ncol;
  } else {
    colmin = (chip-nchip/2)*ncol;
  }
  double rowmax=rowmin+nrow;
  double colmax=colmin+ncol;
  for (int col=colmin; col<colmax; col++) {
    for (int row=rowmin; row<rowmax; row++) {
      totalhits+=(modhisto->GetBinContent(col+1,row+1));
    }
  }
  double mu = totalhits/double(totalpixels);
  return mu/(fr10+1.5*fr15+2.*fr20+3.0*fr30);
}

double ComputeMuAve(TH2D* modhisto) {
  double muave = 0.;
  int ngoodchip=0;
  for (int i=0; i<16; i++) {
    double muchip = ComputeMuChip(modhisto,i);
    if ( muchip>0. ) {
      muave+=muchip;
      ngoodchip++;
    }
  }
  if ( ngoodchip>0 ) muave/=ngoodchip;
  return muave;
}

int ComputePoisson(double probcut, double mu) {
  // Given a Poisson distrubution with average mu, 
  // compute the smallest value of n such that 
  // Probability(x>=n)<probcut;
  double term = exp(-mu);
  int n = 0;
  double prob=1.;
  while (prob>probcut) {
    prob-=term;
    n++;
    term*=(mu/n);
  }
  return n;
}


int main(int argc, char* argv[]){

  std::string option;

  if( argc >= 2 ){
    option = argv[1];
  }

  if( !(argc == 3 ||
	(argc == 4 && option == "--online")) ){
   
    std::cout << "Usage: updateNoiseMaps [--online] <inputfile> <outputfile>" << std::endl;
    return 0;
  }

  bool optionOnline = false;

  if( option == "--online" ){
    optionOnline = true;
  }

  TFile* hitMapFile = new TFile(argv[argc - 2], "READ");
  
  if( !hitMapFile || !hitMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open input file." << std::endl;
    return 1;
  }

  TFile* noiseMapFile = new TFile(argv[argc - 1], "RECREATE");

  if( !noiseMapFile || !noiseMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open output file." << std::endl;
    return 1;
  }

  std::vector<std::string> components;
  components.push_back("Disk1A");
  components.push_back("Disk2A");
  components.push_back("Disk3A");
  components.push_back("Disk1C");
  components.push_back("Disk2C");
  components.push_back("Disk3C");
  components.push_back("B-layer");
  components.push_back("Layer1");
  components.push_back("Layer2");

  std::vector<std::string> types;
  types.push_back("Normal");
  types.push_back("Ganged");
  types.push_back("InterGanged");
  types.push_back("Long");

  std::map<unsigned int, TH2D*> hitMaps;
  std::map<unsigned int, TH2C*> noiseMaps;
  std::map<unsigned int, TH1D*> lumiMaps;

  std::map<unsigned int, TH2C*> noiseMapsOnline;

  TH1D* nEventsHistogram = 0;

  hitMapFile->GetObject("NEvents", nEventsHistogram);

  double nEvents = nEventsHistogram->GetBinContent(1);

  TH2F* disablePlotB0 = 0;
  TH2F* disablePlotB1 = 0;
  TH2F* disablePlotB2 = 0;
  TH2F* disablePlotEC = 0;

  TH1D* globalOccupancy = 0;

  TH1D* disabledModules = 0;
  
  TH1D* maskedPlot = 0;

  std::vector< TH1D*> h_occupancy;

  double minOccupancy = 0;

  double nHitsBeforeMask = 0.;
  double nHitsAfterMask = 0.;
  
  hitMapFile->GetObject("disablePlotB0", disablePlotB0);
  hitMapFile->GetObject("disablePlotB1", disablePlotB1);
  hitMapFile->GetObject("disablePlotB2", disablePlotB2);
  hitMapFile->GetObject("disablePlotEC", disablePlotEC);

  hitMapFile->GetObject("DisabledModules", disabledModules);
    
  if( !optionOnline ){
  
    nEventsHistogram->SetDirectory(noiseMapFile);
    
    disablePlotB0->SetDirectory(noiseMapFile);
    disablePlotB1->SetDirectory(noiseMapFile);
    disablePlotB2->SetDirectory(noiseMapFile);
    disablePlotEC->SetDirectory(noiseMapFile);

    disabledModules->SetDirectory(noiseMapFile);

    int minLogOccupancy = 8;
    minOccupancy = std::pow(10.,-minLogOccupancy);

    globalOccupancy = new TH1D("occupancy","Pixel occupancy",minLogOccupancy*10,-minLogOccupancy,0.);
    globalOccupancy->SetDirectory(noiseMapFile);

    h_occupancy.resize( components.size() + types.size() );
    
    for( std::vector<std::string>::const_iterator component = components.begin();
	 component != components.end(); ++component){
      
      h_occupancy[ component - components.begin() ] = 
	new TH1D(("occupancy" + (*component)).c_str(), ("Pixel occupancy " + (*component)).c_str(),
		 minLogOccupancy*10,-minLogOccupancy,0.);
      
      h_occupancy[ component - components.begin() ]->SetDirectory(noiseMapFile);
    }
    
    for( std::vector<std::string>::const_iterator type = types.begin();
	 type != types.end(); ++type){
      
      h_occupancy[ type - types.begin() + components.size() ] = 
	new TH1D(("occupancy" + (*type)).c_str(), ("Pixel occupancy " + (*type)).c_str(),
		 minLogOccupancy*10,-minLogOccupancy,0.);
      
      h_occupancy[ type - types.begin() + components.size() ]->SetDirectory(noiseMapFile);
    }

    maskedPlot = new TH1D("maskedPlot","Disabled pixels per module",50,0.5,50.5);
    maskedPlot->SetDirectory(noiseMapFile);
  }

  // read hit maps from input file

  // endcaps 

  std::vector<std::string> hemispheres;
  hemispheres.push_back("A");
  hemispheres.push_back("C");

  for(std::vector<std::string>::const_iterator hemisphere = hemispheres.begin(); 
      hemisphere != hemispheres.end(); ++hemisphere){
    
    std::string hitMapsDirName = std::string("hitMaps_endcap") + (*hemisphere);
    std::string noiseMapsDirName = std::string("noiseMaps_endcap") + (*hemisphere);
    std::string lumiMapsDirName = std::string("LBdep_endcap") + (*hemisphere);
    
    TDirectory* hitMapDir = 0;
    TDirectory* noiseMapDir = 0;
    TDirectory* lumiMapDir = 0;
    
    if( !optionOnline ){
      hitMapDir = noiseMapFile->mkdir(hitMapsDirName.c_str());
      noiseMapDir = noiseMapFile->mkdir(noiseMapsDirName.c_str());
      lumiMapDir = noiseMapFile->mkdir(lumiMapsDirName.c_str());
    }
    
    for(int k = 1; k <= 3; k ++){
      
      std::ostringstream component, hitMapsPath, lumiMapsPath;
      
      component << "Disk" << k << (*hemisphere);
      hitMapsPath << "hitMaps_endcap" << (*hemisphere) << "/Disk" << k;
      lumiMapsPath << "LBdep_endcap" << (*hemisphere) << "/Disk" << k;
      
      TDirectory* hitMapSubDir = 0;
      TDirectory* noiseMapSubDir = 0;
      TDirectory* lumiMapSubDir = 0;
      
      if( !optionOnline ){
	hitMapSubDir = hitMapDir->mkdir(component.str().substr(0,5).c_str());
	noiseMapSubDir = noiseMapDir->mkdir(component.str().substr(0,5).c_str());
	lumiMapSubDir = lumiMapDir->mkdir(component.str().substr(0,5).c_str());
      }

      for(int j = 0; j < 48; j++){
	std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))->GetListOfKeys()
					     ->At(j)))->GetName());

	int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));
	hitMaps[i] = 
	  static_cast<TH2D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
						 ->GetListOfKeys()->At(j)))->ReadObj());
	lumiMaps[i] = 
	  static_cast<TH1D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lumiMapsPath.str().c_str())))
						 ->GetListOfKeys()->At(j)))->ReadObj());
	
	if( optionOnline ){
	  
	  std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));

	  noiseMapsOnline[i] = new TH2C(DCSID.c_str(), ("occupancy: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);
	  TDirectory* noiseMapDir = noiseMapFile->mkdir(DCSID.c_str());
	  noiseMapsOnline[i]->SetDirectory( noiseMapDir );
	}
	else{
	  hitMaps[i]->SetName(name.c_str());
	  hitMaps[i]->SetDirectory(hitMapSubDir);
	  lumiMaps[i]->SetName(name.c_str());
	  lumiMaps[i]->SetDirectory(lumiMapSubDir);
	  
	  noiseMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
	  noiseMaps[i]->SetDirectory(noiseMapSubDir);
	}	
      }
    }
  }

  
  // barrel
  
  std::vector<int> staves;
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  layers.push_back("B-layer");
  layers.push_back("Layer1");
  layers.push_back("Layer2");
  

  std::string hitMapsDirName("hitMaps_barrel");
  std::string noiseMapsDirName("noiseMaps_barrel");
  std::string lumiMapsDirName("LBdep_barrel");
  
  TDirectory* hitMapDir = 0;
  TDirectory* noiseMapDir = 0;
  TDirectory* lumiMapDir = 0;

  if( !optionOnline ){
    hitMapDir = noiseMapFile->mkdir(hitMapsDirName.c_str());
    noiseMapDir = noiseMapFile->mkdir(noiseMapsDirName.c_str());
    lumiMapDir = noiseMapFile->mkdir(lumiMapsDirName.c_str());
  }

  for(unsigned int k = 0; k < staves.size(); k++){
    
    std::ostringstream hitMapsPath;
    
    hitMapsPath << "hitMaps_barrel/" << layers[k];

    std::ostringstream lumiMapsPath;
    
    lumiMapsPath << "LBdep_barrel/" << layers[k];
    
    TDirectory* hitMapSubDir = 0;
    TDirectory* noiseMapSubDir = 0;
    TDirectory* lumiMapSubDir = 0;
    
    if( !optionOnline ){
      hitMapSubDir = hitMapDir->mkdir(layers[k].c_str());
      noiseMapSubDir = noiseMapDir->mkdir(layers[k].c_str());
      lumiMapSubDir = lumiMapDir->mkdir(layers[k].c_str());
    }

    for(int j = 0; j < (staves[k] * 13); j++){
      
      std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))->GetListOfKeys()
					   ->At(j)))->GetName());

      int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));

      hitMaps[i] = 
	static_cast<TH2D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
					       ->GetListOfKeys()->At(j)))->ReadObj());
      lumiMaps[i] = 
	static_cast<TH1D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lumiMapsPath.str().c_str())))
					       ->GetListOfKeys()->At(j)))->ReadObj());
      if( optionOnline ){
	std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));
	
	noiseMapsOnline[i] = new TH2C(DCSID.c_str(), ("occupancy: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);
	TDirectory* noiseMapDir = noiseMapFile->mkdir(DCSID.c_str());
	noiseMapsOnline[i]->SetDirectory( noiseMapDir );

      }
      else{
	hitMaps[i]->SetName(name.c_str());
	hitMaps[i]->SetDirectory(hitMapSubDir);
	lumiMaps[i]->SetName(name.c_str());
	lumiMaps[i]->SetDirectory(lumiMapSubDir);

	noiseMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
	noiseMaps[i]->SetDirectory(noiseMapSubDir);
      }
    }
  }
  

  // print cut configuration

  std::map<std::string, double> cuts;

  double gangedPixelMultiplier = 2.;
  double longPixelMultiplier   = 1.5;
  double longgangedPixelMultiplier = 3.;
  std::string text;

  std::string occupancyPerBC( "true" );
  bool optionOccupancyPerBC = false;

  unsigned int nBCReadout = 1;

  double occucut = 1.E-7;
  std::cout << std::endl 
	    << "Using probability for a normal pixel to be flagged as noisy: "
	    << std::endl;

  cuts["Disk1A"] = occucut;
  cuts["Disk2A"] = occucut;
  cuts["Disk3A"] = occucut;
  cuts["Disk1C"] = occucut;
  cuts["Disk2C"] = occucut;
  cuts["Disk3C"] = occucut;  
  cuts["B-layer"]= occucut;
  cuts["Layer1"] = occucut;
  cuts["Layer2"] = occucut;
    

  std::cout << "Disk1A " << cuts["Disk1A"] << std::endl;
  std::cout << "Disk2A " << cuts["Disk2A"] << std::endl;
  std::cout << "Disk3A " << cuts["Disk3A"] << std::endl << std::endl;

  std::cout << "Disk1C " << cuts["Disk1C"] << std::endl;
  std::cout << "Disk2C " << cuts["Disk2C"] << std::endl;
  std::cout << "Disk3C " << cuts["Disk3C"] << std::endl << std::endl;

  std::cout << "BLayer " << cuts["B-layer"] << std::endl;
  std::cout << "Layer1 " << cuts["Layer1"] << std::endl;
  std::cout << "Layer2 " << cuts["Layer2"] << std::endl << std::endl;

  std::cout << "Multiplier for ganged pixels " << gangedPixelMultiplier << std::endl;
  std::cout << "Multiplier for long pixels " << longPixelMultiplier << std::endl << std::endl;
  std::cout << "Multiplier for long-ganged pixels " << longgangedPixelMultiplier << std::endl << std::endl;
  
  if( occupancyPerBC == "true" || occupancyPerBC == "True" || occupancyPerBC == "TRUE" ){
    optionOccupancyPerBC = true;
    std::cout << "Occupancy calculated per BC with " << nBCReadout << " BC readout" 
	      << std::endl << std::endl;
  }
  else{
    std::cout << "Occupancy calculated per event" << std::endl << std::endl;
  }
  

  // calculate noise maps

  for(std::map<unsigned int, TH2D*>::const_iterator module = hitMaps.begin(); module != hitMaps.end(); ++module){

    unsigned int moduleID = (*module).first;

    TH2C* noiseMap = 0;

    if( optionOnline ){
      noiseMap = noiseMapsOnline[moduleID];
    }
    else{
      noiseMap = noiseMaps[moduleID];
    }
    
    int barrel = ((moduleID >> 25) & 3);
    barrel = 2*(barrel-1);
    int layer = ((moduleID >> 23) & 3);
    int module_phi = ((moduleID >> 17) & 63);
    int module_eta = ((moduleID >> 13) & 15);
    module_eta -= 6;
    
    unsigned int component_index = 0;
    
    if( barrel == 2 ){
      component_index = layer;
    }
    else if ( barrel == -2 ){
      component_index = layer + 3;
    }
    else if ( barrel == 0 ){
      component_index = layer + 6;
    }

    TH2F* disablePlot = 0;
    
    if( !optionOnline ){

      if( barrel!=0 ) disablePlot = disablePlotEC;
      else{
	if(layer == 0) disablePlot = disablePlotB0;
	else if(layer == 1) disablePlot = disablePlotB1;
	else if(layer == 2) disablePlot = disablePlotB2;
      } 
    }

    int thisModuleCut=0;

    double nHitsAfterMaskThisModule = 0.;
    
    double cut = cuts[components[ component_index ] ];
    double thisPixelCut = 0.;

    double muave = ComputeMuAve(module->second);
    double normalpixelcut = ComputePoisson(cut,muave);
    double longpixelcut = ComputePoisson(cut,muave*longPixelMultiplier);
    double gangedpixelcut = ComputePoisson(cut,muave*gangedPixelMultiplier);
    double longgangedpixelcut = ComputePoisson(cut,muave*longgangedPixelMultiplier);
    // std::cout << module->second->GetTitle() << "  mu=" << muave << " cut at: " << normalpixelcut << std::endl;
    for(int pixel_eta = 0; pixel_eta < 144; pixel_eta++){
      for(int pixel_phi = 0; pixel_phi < 328; pixel_phi++){
	
	double nHits = module->second->GetBinContent( pixel_eta + 1, pixel_phi + 1 );

	
	unsigned int type = 0;
	unsigned int pixelType = ModuleSpecialPixelMap::
	  pixelType( pixel_eta % 18, (pixel_phi <= 163) ? pixel_phi : 327 - pixel_phi );
	
	switch(pixelType) {
	case 0:
        case 5:
	  type = 0; // normal pixel
	  thisPixelCut = normalpixelcut;
	  break;
	case 1:
        case 6:
	  type = 1; // long pixel
	  thisPixelCut = longpixelcut;
	  break;
	case 2:
	  type = 2; // ganged pixel
	  thisPixelCut = gangedpixelcut;
	  break;
	case 3:
	  type = 3; // long-ganged pixel
	  thisPixelCut = longgangedpixelcut;
	  break;
	default:
	  type = 4;
	  break;
	}
	  
	if( type != 4 ){

	  double occupancy = 0.;
	  
	  if( optionOccupancyPerBC ){
	    occupancy = nHits / nEvents / nBCReadout;
	  }
	  else{
	    occupancy = nHits / nEvents;
	  }

	  if( nHits >= thisPixelCut ){
	    
	    if( optionOnline ){
	      
	      unsigned int column = 0, row = 0;
	      
	      if(barrel == 0){                            //barrel
		
		if( pixel_phi > 167 ){
		  row = 327 - pixel_phi;
		}
		else{
		  row = 319 - pixel_phi;
		}
		
		column = 143 - pixel_eta;
	      }
	      else if(barrel == -2  || barrel == 2){      //disks
		if(module_phi % 2 == 0){                  //even modules
		  row = pixel_phi;
		  
		  if( pixel_phi > 167 ){
		    row -= 8;
		  } 
		  
		  column = 143 - pixel_eta;
		}
		else if(module_phi % 2 == 1){             //odd modules
		  
		  if( pixel_phi > 167 ){
		    row = 327 - pixel_phi;
		  }
		  else{
		    row = 319 - pixel_phi;
		  }
		  
		  column = 143 - pixel_eta;
		}
	      }
	      noiseMap->Fill(column, row);
	    }
	    else{
	      noiseMap->Fill(pixel_eta, pixel_phi);
	    }

	    thisModuleCut++;
	  }
	  
	  if( !optionOnline ){
	    if ( occupancy < minOccupancy ) occupancy = minOccupancy;
	  
	    double logOccupancy = std::log10(occupancy);

	    globalOccupancy->Fill( logOccupancy );
	    h_occupancy[ component_index ]->Fill( logOccupancy );
	    h_occupancy[ type + components.size() ]->Fill( logOccupancy );
	  }
	}
	  
	if( !optionOnline ){
	  if( noiseMap->GetBinContent(pixel_eta + 1, pixel_phi + 1) == 0 ){
	    
	    nHitsAfterMaskThisModule += nHits;
	  }
	}
      }
    }

    nHitsBeforeMask += module->second->GetEntries();
    nHitsAfterMask += nHitsAfterMaskThisModule;

    if( !optionOnline ){
      
      if(thisModuleCut > 0.) maskedPlot->Fill(thisModuleCut);
      
      if( barrel==0 ){
	if( disablePlot->GetBinContent(module_eta+7, module_phi+1) < 0. ){
	  disablePlot->SetBinContent(module_eta+7, module_phi+1, -1.);
	}
	else{
	  disablePlot->SetBinContent(module_eta+7, module_phi+1, thisModuleCut);
	}
      }
      else if( barrel>0 ){
	if( disablePlot->GetBinContent(layer+5, module_phi+1) < 0. ){
	  disablePlot->SetBinContent(layer+5, module_phi+1, -1.);
	}
	else{
	  disablePlot->SetBinContent(layer+5, module_phi+1, thisModuleCut);
	}
      }
      else{
	if( disablePlot->GetBinContent(3-layer, module_phi+1) < 0. ){
	  disablePlot->SetBinContent(3-layer, module_phi+1, -1.);
	}
	else{
	  disablePlot->SetBinContent(3-layer, module_phi+1, thisModuleCut);
	}
      }
    }
  }

  if( !optionOnline ){
    double nPixels = 80363520.;
    double nModules = 1744.;

    double nDisabledModules = 0.;

    for( int i = 1; i < nModules + 1; i++ ){
      nDisabledModules += disabledModules->GetBinContent(i);
    }

    nDisabledModules /= nEvents;

    double nPixelsOnDisabledModules = nDisabledModules * nPixels / nModules;
    
    std::cout << std::endl;
    
    std::cout << "Taking into account an average of " 
	      << nDisabledModules << " disabled modules." << std::endl;
    std::cout << "Occupancy before offline noise mask " 
	      << nHitsBeforeMask / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << " +- "
	      << std::sqrt(nHitsBeforeMask) / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << std::endl;
    std::cout << "Occupancy after offline noise mask " 
	      << nHitsAfterMask / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << " +- "
	      << std::sqrt(nHitsAfterMask) / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << std::endl << std::endl;
  }
  
  hitMapFile->Close();
  delete hitMapFile;

  noiseMapFile->Write();
  noiseMapFile->Close();
  delete noiseMapFile;

  return 0;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<vector>
#include<cmath>
#include<map>
#include<sstream>
#include<string>
#include<iostream>
#include<fstream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TKey.h"
#include "Riostream.h"

#include "PixelCalibAlgs/PixelConvert.h"
#include "PixelConditionsData/SpecialPixelMap.h"

int minEventNumber = 2000000;
int minAveragePixelHitNumber = 13;

int nPixelsPerModule = 46080;
double nModules = 1744.;
double nPixels = nModules*nPixelsPerModule;

using namespace std;

int main(int argc, char* argv[]){

  //-----------------------------------
  //Usage configuration of the program
  //------------------------------------
  std::string option;
  std::string finname;
  std::string foutname;

  bool optionOnline = false;

  if( argc == 3 ){
    finname=argv[argc - 2];
    foutname=argv[argc - 1];
  }
  else if( argc == 4 ){
    finname=argv[argc - 2];
    foutname=argv[argc - 1];
    option = argv[1];
    if( option == "--online" ) optionOnline = true;
  }
  else{
    std::cout << "Usage: updateDeadMaps [--online] <inputfile> <outputfile> " << std::endl;
    return 0;
  }	
	
  //-----------------------------
  //Open input and output files
  //------------------------------
  TFile* hitMapFile = new TFile(finname.c_str(), "READ");  
  if( !hitMapFile || !hitMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open input file." << std::endl;
    return 1;
  }

	ofstream logfile;
  	logfile.open("DeadPixels.txt");

  TH1D* nEventsHistogram = 0;
  hitMapFile->GetObject("NEvents", nEventsHistogram);
  double nEvents = nEventsHistogram->GetBinContent(1);
		
  if(nEvents < minEventNumber) {
    std::cerr << "WARNING!" << std::endl;
    std::cerr << "Number of events in the file is "<< nEvents <<std::endl;
    std::cerr << "Minimum required event number to have an accurate estimate of dead pixels is "<< minEventNumber <<std::endl;
  }


  TFile* deadMapFile = new TFile(foutname.c_str(), "RECREATE");
  if( !deadMapFile || !deadMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open output file." << std::endl;
    return 1;
  }

  //-------------------------------
  //Read configuration file
  //-------------------------------

  std::string text;
  std::ifstream cutConfigFile;
  cutConfigFile.open("updateDeadMapsConfig.txt");

  if( cutConfigFile.is_open() ){

    std::cout << std::endl 
	      << "Configuration read from file updateDeadMapsConfig.txt:" 
	      << std::endl << std::endl;

    cutConfigFile >> text >> minEventNumber;
    cutConfigFile >> text >> minAveragePixelHitNumber;

  }
  else{
    std::cout << std::endl 
              << "File updateDeadMapsConfig.txt not in current directory, using default occupancy cuts:" 
              << std::endl << std::endl;
    
  }
  std::cout<<"Minimum event number required: "<<minEventNumber<<std::endl;
  std::cout<<"Minimum average hits/pixl required per module: "
	   <<minAveragePixelHitNumber<<std::endl;
  std::cout<<std::endl<<std::endl;

  //-------------------------------
  // Initialization 
  //-------------------------------
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

  std::map<unsigned int, TH2D*> hitMaps;
  std::map<unsigned int, TH2C*> deadMaps;
  std::map<unsigned int, TH2C*> deadMapsOnline;
  std::map<unsigned int, TH2C*> deadRefMaps;
  std::map<unsigned int, TH2C*> deadRefMapsOnline;

  TH1D* disabledModules = 0;
  TH1D* deadPlot = 0;

  TH1D* deadPixel_layer = 0;
  TH1D* deadPixel_layer_all = 0;

  std::vector< TH1D*> h_deadPixel;

  double ndeadPixel[9]={0.};
  double ndeadPixel_total=0;
  double ndeadPixel_noDisabled[9]={0.};

  hitMapFile->GetObject("DisabledModules", disabledModules);
    
  if( !optionOnline ){
  
    nEventsHistogram->SetDirectory(deadMapFile);

    disabledModules->SetDirectory(deadMapFile);

    deadPlot = new TH1D("deadPlot","Zero hit pixels per module",100,-0.5,99.5);
    deadPlot->SetDirectory(deadMapFile);

    deadPixel_layer = new TH1D("deadPixel_layer","Zero hit pixels in each layer (No include dead module)",9,0,9);
    deadPixel_layer->SetDirectory(deadMapFile);
    deadPixel_layer->SetLabelSize  ( 0.05,"x");
    deadPixel_layer->GetXaxis()->SetBinLabel(1,"Disk1A");
    deadPixel_layer->GetXaxis()->SetBinLabel(2,"Disk2A");
    deadPixel_layer->GetXaxis()->SetBinLabel(3,"Disk3A");
    deadPixel_layer->GetXaxis()->SetBinLabel(4,"Disk1C");
    deadPixel_layer->GetXaxis()->SetBinLabel(5,"Disk2C");
    deadPixel_layer->GetXaxis()->SetBinLabel(6,"Disk3C");
    deadPixel_layer->GetXaxis()->SetBinLabel(7,"B-Layer");
    deadPixel_layer->GetXaxis()->SetBinLabel(8,"Layer1");
    deadPixel_layer->GetXaxis()->SetBinLabel(9,"Layer2");

    deadPixel_layer_all = new TH1D("deadPixel_layer_all","Zero hit pixels in each layer (include dead module)",9,0,9);
    deadPixel_layer_all->SetDirectory(deadMapFile);
    deadPixel_layer_all->SetLabelSize  ( 0.05,"x");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(1,"Disk1A");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(2,"Disk2A");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(3,"Disk3A");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(4,"Disk1C");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(5,"Disk2C");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(6,"Disk3C");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(7,"B-Layer");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(8,"Layer1");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(9,"Layer2");


    h_deadPixel.resize( components.size() );
    for( std::vector<std::string>::const_iterator component = components.begin();
         component != components.end(); ++component){

      h_deadPixel[ component - components.begin() ] =
        new TH1D(("deadPixel" + (*component)).c_str(), ("Zero hit pixel per module " + (*component)).c_str(),
                 100,-0.5,99.5);

      h_deadPixel[ component - components.begin() ]->SetDirectory(deadMapFile);    
    }

  }

  // Open file to save python fragment with disabled modules
  ofstream pyFile("DisabledModulesAndFE.py"); 

  //------------------------------------
  // read hit maps from input file
  //
  // endcaps 
  // 48 modules each disk
  // phi_index [0,327]
  // eta_index [0,143]
  //------------------------------------
  std::vector<std::string> hemispheres;
  hemispheres.push_back("A");
  hemispheres.push_back("C");

  for(std::vector<std::string>::const_iterator hemisphere = hemispheres.begin(); 
      hemisphere != hemispheres.end(); ++hemisphere){
    
    std::string hitMapsDirName = std::string("hitMaps_endcap") + (*hemisphere);
    std::string deadMapsDirName = std::string("deadMaps_endcap") + (*hemisphere);
    
    TDirectory* hitMapDir = 0;
    TDirectory* deadmapDir = 0;
    
    if( !optionOnline ){
      hitMapDir = deadMapFile->mkdir(hitMapsDirName.c_str());
      deadmapDir = deadMapFile->mkdir(deadMapsDirName.c_str());
    }
    
    for(int k = 1; k <= 3; k ++){
      
      std::ostringstream component, hitMapsPath;
      
      component << "Disk" << k << (*hemisphere);
      hitMapsPath << "hitMaps_endcap" << (*hemisphere) << "/Disk" << k;
      
      TDirectory* hitMapSubDir = 0;
      TDirectory* deadmapSubDir = 0;
      
      if( !optionOnline ){
	hitMapSubDir = hitMapDir->mkdir(component.str().substr(0,5).c_str());
	deadmapSubDir = deadmapDir->mkdir(component.str().substr(0,5).c_str());
      }

      for(int j = 0; j < 48; j++){
	std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
					     ->GetListOfKeys()->At(j)))->GetName());

	int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));
	//int i = PixelConvert::HashID((name));

	hitMaps[i] = 
	  static_cast<TH2D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
						 ->GetListOfKeys()->At(j)))->ReadObj());
	
	if( optionOnline ){
	  
	  std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));
	  deadMapsOnline[i] = new TH2C(DCSID.c_str(), ("Dead: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);
	  
	  deadmapDir = deadMapFile->mkdir(DCSID.c_str());
	  deadMapsOnline[i]->SetDirectory( deadmapDir );
	  

	}
	else{
	  hitMaps[i]->SetName(name.c_str());
	  hitMaps[i]->SetDirectory(hitMapSubDir);
	  
	  deadMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
	  deadMaps[i]->SetDirectory(deadmapSubDir);

	}	
      }
    }
  }

  
  //-----------------------------------------------------------
  // read hit maps from input file
  //
  // barrel
  // 22, 38, 52 slaves for layer 0, 1 and 2 
  // 13 modules per stave
  // phi_index [0,319] for layer 0, [0,327] for layer 1 & 2
  // eta_index [0,143] for layer 0, 1 & 2
  //-----------------------------------------------------------
  
  std::vector<int> staves;
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  layers.push_back("B-layer");
  layers.push_back("Layer1");
  layers.push_back("Layer2");
  

  std::string hitMapsDirName("hitMaps_barrel");
  std::string deadMapsDirName("deadMaps_barrel");
  
  TDirectory* hitMapDir = 0;
  TDirectory* deadmapDir = 0;

  if( !optionOnline ){
    hitMapDir = deadMapFile->mkdir(hitMapsDirName.c_str());
    deadmapDir = deadMapFile->mkdir(deadMapsDirName.c_str());
  }

  for(unsigned int k = 0; k < staves.size(); k++){
    
    std::ostringstream hitMapsPath;
    
    hitMapsPath << "hitMaps_barrel/" << layers[k];
    
    TDirectory* hitMapSubDir = 0;
    TDirectory* deadmapSubDir = 0;
    
    if( !optionOnline ){
      hitMapSubDir = hitMapDir->mkdir(layers[k].c_str());
      deadmapSubDir = deadmapDir->mkdir(layers[k].c_str());
    }

    for(int j = 0; j < (staves[k] * 13); j++){
      
      std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
					   ->GetListOfKeys()->At(j)))->GetName());

      int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));
      //int i = PixelConvert::HashID((name));

      hitMaps[i] = 
	static_cast<TH2D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
					       ->GetListOfKeys()->At(j)))->ReadObj());
      if( optionOnline ){
	std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));
	
	deadMapsOnline[i] = new TH2C(DCSID.c_str(), ("Dead: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);
	TDirectory* deadmapDir = deadMapFile->mkdir(DCSID.c_str());
	deadMapsOnline[i]->SetDirectory( deadmapDir );

      }
      else{
	hitMaps[i]->SetName(name.c_str());
	hitMaps[i]->SetDirectory(hitMapSubDir);

	deadMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
	deadMaps[i]->SetDirectory(deadmapSubDir);

      }
    }
  }
  

  //----------------------------------
  // calculate dead maps
  //----------------------------------
	
	int alternativeNDisabledModules = 0;
	
  for(std::map<unsigned int, TH2D*>::const_iterator module = hitMaps.begin(); module != hitMaps.end(); ++module){

    unsigned int moduleID = (*module).first;
		std::string onlineID = PixelConvert::OnlineID(module->first);
		std::string offlineID = PixelConvert::OfflineID(module->first);
		std::string DCSID = PixelConvert::DCSID(onlineID);
    
    TH2C* deadmap = 0;
 
    if( optionOnline ){
      deadmap = deadMapsOnline[moduleID];
     }
    else{
      deadmap = deadMaps[moduleID];
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

    double averageHitNumber=module->second->GetEntries()/nPixelsPerModule;
    if(averageHitNumber<minAveragePixelHitNumber && averageHitNumber>0.001){
      std::cerr<<"WARNING!" << std::endl;
      std::cerr<<"Average Occ/pix= "<<averageHitNumber
	       <<" in module " << module->second->GetTitle() << std::endl;
    }
     
    int thisModuleCut=0;
    bool isModuleActive=false;
    const int nchip=16;
    const int nrow =160;
    const int ncol =18;
    bool isFEActive[nchip];

    for (int chip=0; chip<nchip; chip++) {
      isFEActive[chip]=false;
      int rowmin=0, colmin=0; 
      if (chip<(nchip/2)) {
	rowmin=nrow+8; 
	colmin = (nchip/2-chip-1)*ncol;
      } else {
	colmin = (chip-nchip/2)*ncol;
      }
      double rowmax=rowmin+nrow;
      double colmax=colmin+ncol;
      for(int pixel_eta = colmin; pixel_eta < colmax; pixel_eta++){
	for(int pixel_phi = rowmin; pixel_phi < rowmax; pixel_phi++){
	  double nHits = module->second->GetBinContent( pixel_eta + 1, pixel_phi + 1 );

	  unsigned int pixelType = ModuleSpecialPixelMap::
	    pixelType( pixel_eta % 18, (pixel_phi <= 163) ? pixel_phi : 327 - pixel_phi );

	  if(pixelType != 0 && pixelType !=1 && pixelType !=2 &&
	     pixelType != 3 && pixelType !=5 && pixelType !=6)  continue;
	  //pixelType:
	  //0: normal, 1: long, 2: ganged, 3: long ganged, 5: inter ganged, 
	  //6: long inter ganged, 8: invalid 	              

	  if(nHits < 0.5){
	  
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


	      deadmap->Fill(column, row);
	    
	    }
	    else{

	      deadmap->Fill(pixel_eta, pixel_phi);
	    
	    }
	  
	    thisModuleCut++;
	  } else {
	    isModuleActive=true;
	    isFEActive[chip]=true;
	  }
	
	}
      
      }
    }
    if (!isModuleActive) {
      std::cout << "Dead module: "  << module->second->GetTitle() << std::endl;
      pyFile << "job.PixMapOverlayWriter.ModuleIDFragments += [ \"" 
	     <<  module->second->GetTitle() << "\" ]" << std::endl;
    } else {
      bool hasDeadFE = false;
      for (int chip=0; chip<nchip; chip++) {
	if (!isFEActive[chip]) { hasDeadFE=true; break; }
      }
      if (hasDeadFE) {
	std::cout << "Dead FE in module: "  << module->second->GetTitle();
	pyFile << "job.PixMapOverlayWriter.Frontends += { \"" 
	       <<  module->second->GetTitle() << "\": ";
	char sep='[';
	for (int chip=0; chip<nchip; chip++) {
	  if (!isFEActive[chip]) { 
	    std::cout << ", " << chip;
            pyFile << sep << chip;
	    sep = ',';
	  }
	}
	std::cout << std::endl;
        pyFile << "] }" << std::endl;
      } 
    }


    if( !optionOnline ){
      
      ndeadPixel[component_index]+=thisModuleCut;
      ndeadPixel_total+=thisModuleCut;

      if(thisModuleCut<nPixelsPerModule){
	ndeadPixel_noDisabled[component_index]+=thisModuleCut;
      } else {
	deadmap->Reset();
	
	alternativeNDisabledModules++;
	//std::cout << "Check moduli" << onlineID << std::endl;
      }
      
      h_deadPixel[component_index]->Fill(thisModuleCut);
      deadPlot->Fill(thisModuleCut);
      
    
	
    }
		
		//std::cout << "Module " << onlineID << " " << offlineID << " " << DCSID << " dead pixels " << thisModuleCut << std::endl; 
		logfile << "Module " << DCSID << " dead pixels " << thisModuleCut <<std::endl; 
    
  }

  if( !optionOnline ){
    std::cout<<std::endl;
    std::cout<<"---------------------------------------------------------"<<std::endl;
    std::cout<<"Summary of the pixel with zero hit:"<<std::endl;
    std::cout<<"---------------------------------------------------------"<<std::endl;

    double nDisabledModules = 0.;
    int nFullDisabledModules = 0;

    for( int i = 1; i < nModules + 1; i++ ){
		//cout << disabledModules->GetBinContent(i) << endl;
      nDisabledModules += disabledModules->GetBinContent(i);
      if ( disabledModules->GetBinContent(i) == nEvents ) 
	nFullDisabledModules++;
    }

    nDisabledModules /= nEvents;

    // double nPixelsOnDisabledModules = nDisabledModules * nPixels / nModules;
    double nPixelsOnDisabledModules = nFullDisabledModules * nPixelsPerModule ;
 		double alternativeNPixelsNoDisabledModules = 0 ;
   	double alternativeNDeadPixelTotal=0; 
		
		for( std::vector<std::string>::const_iterator component = components.begin();
         component != components.end(); ++component) {
				 		alternativeNDeadPixelTotal += ndeadPixel[ component - components.begin() ]; 
						alternativeNPixelsNoDisabledModules += ndeadPixel_noDisabled[ component - components.begin() ];
    }
		
	 
    std::cout << std::endl;
    std::cout << "Number of events in this run " 
	      << nEvents << std::endl;

    std::cout << "Number of disabled modules " 
	      << nFullDisabledModules << std::endl;
			
		std::cout << "Alternative disabled modules "
				<< alternativeNDisabledModules << std::endl;
		

    std::cout << "Number of pixel from the disabled modules " 
	      << nPixelsOnDisabledModules << std::endl;
		
		std::cout << "Alternative number of pixel from the disabled modules  " 
	      << alternativeNDeadPixelTotal - alternativeNPixelsNoDisabledModules << std::endl;		
		

    std::cout << "Number of pixel with zero hit (total) " 
	      << ndeadPixel_total << std::endl;

    std::cout << "Number of pixel with zero hit (not including disabled modules) " 
	      << ndeadPixel_total - nPixelsOnDisabledModules << std::endl;
				
		std::cout << "Alternative number of pixel with zero hit (not including disabled modules) " 
	      << alternativeNPixelsNoDisabledModules << std::endl;				

    std::cout << std::endl;
    std::cout << "Number of pixel with zero hit in each layer: (not including disabled module)" 
	      << std::endl;
    std::cout << std::endl;

    for( std::vector<std::string>::const_iterator component = components.begin();
         component != components.end(); ++component){

      std::cout <<component->c_str()<<" : "
		<<ndeadPixel[ component - components.begin() ]<< " ("
		<<ndeadPixel_noDisabled[ component - components.begin() ]
		<<")"<<std::endl;

      deadPixel_layer_all->SetBinContent(component - components.begin() + 1,
				     ndeadPixel[ component - components.begin() ]);
      deadPixel_layer->SetBinContent(component - components.begin() + 1,
				     ndeadPixel_noDisabled[ component - components.begin() ]);
    }

    
  }
  
  hitMapFile->Close();
  delete hitMapFile;

  deadMapFile->Write();
  deadMapFile->Close();
  delete deadMapFile;


  pyFile.close();

  return 0;
}


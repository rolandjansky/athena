#include<vector>
#include<cmath>
#include<map>
#include<sstream>
#include<string>
#include<iostream>
#include<fstream>
#include<utility>
#include<string>
#include<filesystem>


#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TKey.h"
#include "Riostream.h"


std::vector< std::pair< std::string, std::vector<int> > > pixelMapping;
std::vector< std::pair< int, std::vector<int> > > hashMapping;
std::vector< std::pair< std::string, std::vector<int> > > channelMapping;
void setPixelMapping(bool isIBL);
std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);
int getHashFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);
std::vector<int> getPositionFromDCSID (std::string module_name);
std::vector<int> getChannelFromHashID (int hashid);
void checkInactiveModule(bool isIBL, TFile* hitMapFile, int &npush_back, std::vector<std::string> &vsmodule, std::vector<int> &vLB_start, std::vector<int> &vLB_end);
void checkInactiveFEs(bool isIBL, TFile* hitMapFile, int &npush_backFE, std::vector<std::string> &vsFE, std::vector<std::string> &FEcode);
void make_txt(std::string srun, int npush_back, int npush_backFE, std::vector<std::string> vsFE, std::vector<std::string> vsmodule, std::vector<int> vLB_start, std::vector<int> vLB_end, std::vector<std::string> FEcode);


std::vector<std::string> &splitter(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> splitter(const std::string &s, char delim) {
  std::vector<std::string> elems;
  splitter(s, delim, elems); 
  std::cout<<s<<" "<<delim<<" "<<std::endl;
  return elems;
}

bool is_file_exist(const char *fileName)
{
  std::ifstream infile;
  infile.open(fileName);
  return infile.good();
}

using namespace std;

int main(int argc, char* argv[]){
  bool isIBL = true;
  std::string stag_name = "PixMapOverlay-RUN2-DATA-UPD4-04";

  //-----------------------------------
  //Usage configuration of the program
  //------------------------------------
  std::string runNumber = "0";
  std::string finname = "";
  std::string sflag_first = "";

  bool flag_first = false;
  if( argc == 2 ){
    sflag_first = argv[1];
    if( sflag_first.find("first") != std::string::npos ) flag_first = true;
  }else if( argc == 3 ){
    runNumber = argv[1];
    finname = argv[2];
  }else{
    std::cout << "For initialization: " << std::endl;
    std::cout << ">makeInactiveModuleList.exe first" << std::endl;
    std::cout << "Then, PixOverlay_run0.py is created to make new database tag." << std::endl;
    std::cout << std::endl;
    std::cout << "To make inactive module list: " << std::endl;
    std::cout << ">makeInactiveModuleList.exe [run #] [ROOT file]" << std::endl;
    std::cout << "Then, PixOverlay_run[run#].py is created." << std::endl;
    return 0;
  }

  //-----------------------------
  //Read pixel mapping
  //------------------------------
  setPixelMapping(isIBL);

  //-----------------------------
  //Open input and output files; define variables
  //------------------------------
  int npush_back = 0;
  int npush_backFE = 0;
  std::vector<std::string> vsmodule;
  std::vector<std::string> vsFE;
  std::vector<std::string> FEcode;
  std::vector<int> vLB_start;
  std::vector<int> vLB_end;

  if(flag_first != true){
    TFile* hitMapFile = new TFile(finname.c_str(), "READ");
    if( !hitMapFile || !hitMapFile->IsOpen() ){
      std::cerr << "Error: Unable to open input file." << std::endl;
      return 1;
    
    }
    checkInactiveFEs(isIBL, hitMapFile, npush_backFE, vsFE, FEcode);
    checkInactiveModule(isIBL, hitMapFile, npush_back, vsmodule, vLB_start, vLB_end);
    
    
    hitMapFile->Close();
    delete hitMapFile;
  }

  //-----------------------------
  // Open file to save txt fragment with disabled modules
  //-----------------------------
  std::cout<<"create txt file"<<std::endl;
  make_txt(runNumber, npush_back,npush_backFE, vsFE,  vsmodule, vLB_start, vLB_end, FEcode);

  return 0;
}

//-----------------------------
// Function to read infos from Lumi-block information (IBL for each FE, rest for each module)
//-----------------------------
void checkInactiveModule(bool isIBL, TFile* hitMapFile, int &npush_back, std::vector<std::string> &vsmodule, std::vector<int> &vLB_start, std::vector<int> &vLB_end){
  //-----------------------------
  // Everything adopted to new noise maps
  //-----------------------------
  TH1* nEventsLBHistogram = 0;
  hitMapFile->GetObject("NEventsLB", nEventsLBHistogram);
  
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
  if (isIBL) components.push_back("IBL");
  components.push_back("B0");
  components.push_back("B1");
  components.push_back("B2");

  std::map<std::string, TH1D*> lbdep;
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
    std::string lbdepDirName = std::string("OccupancyLb");

    for(int k = 1; k <= 3; k ++){
      std::ostringstream component, lbdepPath;
      component << "Disk" << k << (*hemisphere);
      lbdepPath << lbdepDirName << "/Disk" << k <<(*hemisphere);

      for(int phi = 0; phi < 48; phi++){
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))->GetListOfKeys()->At(phi)))->GetName());
        lbdep[name] = static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))->GetListOfKeys())->At(phi)))->ReadObj());
        lbdep[name]->SetName(name.c_str());
      }
    } // loop over k
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
  if(isIBL) staves.push_back(14);
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  if(isIBL) layers.push_back("IBL");
  layers.push_back("B0");
  layers.push_back("B1");
  layers.push_back("B2");

  std::string lbdepDirName("lbdep_barrel");
  for(unsigned int layer = 0; layer < staves.size(); layer++){
    std::ostringstream lbdepPath;
    lbdepPath << "OccupancyLb/" << layers[layer];

    int nModulesPerStave = 13;
    if (isIBL && layer == 0) nModulesPerStave = 32; // --- IBL --- //
      for(int module = 0; module < staves[layer] * nModulesPerStave; module++){ // loop on modules
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))->GetListOfKeys()->At(module)))->GetName());
        lbdep[name] = static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get( lbdepPath.str().c_str() ) ))->GetListOfKeys() )->At(module) ))->ReadObj());
        lbdep[name]->SetName(name.c_str());
      }
  }
  
  for(std::map<std::string, TH1D*>::const_iterator module = lbdep.begin(); module != lbdep.end(); ++module){
    std::string moduleID = (*module).first;
    int LB_start = 0;
    int LB_end = 0;
    int flag_start = 0;
    int nbin =nEventsLBHistogram->GetNbinsX();

    //-----------------------------
    // Writing list of modules (or FEs in IBL modules) that do not work during a LB
    //-----------------------------
    for (int LB = 1; LB <= nbin; LB++) {
   
      if(nEventsLBHistogram->GetBinContent(LB) < 80.) continue; // #events in LB >= 80
      if(lbdep[moduleID]->GetBinContent(LB) ==0) { // (#module hits in LB) / (#events in LB) < 1
        
	if(flag_start == 0){
	  flag_start = 1;
	  LB_start = LB;
	  LB_end = LB;
	}else{
	  LB_end = LB;
	}
      }else{// the module have hits
	if(flag_start == 1){
	  flag_start = 0;
	  vsmodule.push_back(moduleID);
	  vLB_start.push_back(LB_start);
	  vLB_end.push_back(LB_end);
	  npush_back++;
	}
      }//if(lbdep[moduleID]->GetBinContent(LB + 1) < 1.)
    } //for (int LB = 1; LB < 2000; LB++)
    if(flag_start == 1){
      vsmodule.push_back(moduleID);
      vLB_start.push_back(LB_start);
      vLB_end.push_back(LB_end);
      npush_back++;
    }

  }//for(std::map<std::string, TH1D*>::const_iterator module = lbdep.begin(); module != lbdep.end(); ++module)
}

void checkInactiveFEs(bool isIBL, TFile* hitMapFile, int &npush_backFE, std::vector<std::string> &vsFE, std::vector<std::string> &FEcode){ //Function for FE masking
  //
  //-------------------------------
  // Everything adopted to new noise maps
  // Initialization
  //-------------------------------
  std::vector<std::string> components;
  components.push_back("Disk1A");
  components.push_back("Disk2A");
  components.push_back("Disk3A");
  components.push_back("Disk1C");
  components.push_back("Disk2C");
  components.push_back("Disk3C");
  if (isIBL) components.push_back("IBL");
  components.push_back("B0");
  components.push_back("B1");
  components.push_back("B2");

  std::map<std::string, TH1D*> hitMaps;
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
      std::string hitMapsDirName = std::string("Occupancy2d");

    for(int k = 1; k <= 3; k ++){
      std::ostringstream component, hitMapsPath;
      component << "Disk" << k << (*hemisphere);
      hitMapsPath << hitMapsDirName << "/Disk" << k <<(*hemisphere);

      
      for(int phi = 0; phi < 48; phi++){
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))->GetListOfKeys()->At(phi)))->GetName());
        hitMaps[name] = static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))->GetListOfKeys())->At(phi)))->ReadObj());
        hitMaps[name]->SetName(name.c_str());
      }
    } // loop over k
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
  if(isIBL) staves.push_back(14);
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  if(isIBL) layers.push_back("IBL");
  layers.push_back("B0");
  layers.push_back("B1");
  layers.push_back("B2");

  std::string hitMapsDirName("hitMaps_barrel");
  for(unsigned int layer = 0; layer < staves.size(); layer++){
    std::ostringstream hitMapsPath;
    hitMapsPath <<"Occupancy2d/" << layers[layer];
    int nModulesPerStave = 13;
    if (isIBL && layer == 0) nModulesPerStave = 32; // --- IBL --- //
    if (layer !=0){						//IBL ignored,because treated in function for LB information
      for(int module = 0; module < staves[layer] * nModulesPerStave; module++){ // loop on modules
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))->GetListOfKeys()->At(module)))->GetName());
        hitMaps[name] = static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get( hitMapsPath.str().c_str() ) ))->GetListOfKeys() )->At(module) ))->ReadObj());
        hitMaps[name]->SetName(name.c_str());
      }
    }
  }
  //---------------------------------------
  // Writing list of FE that shall be masked
  //---------------------------------------
  std::cout<<"hitMaps.size(): "<<hitMaps.size()<<endl;
  int a=0; //counting variable
  for(std::map<std::string, TH1D*>::const_iterator module = hitMaps.begin(); module != hitMaps.end(); ++module){
    std::string moduleID = (*module).first;
    //---------------------------------------
    // Further variables for counting
    //---------------------------------------
    int nbinx =hitMaps[moduleID]->GetNbinsX();
    int nbiny =hitMaps[moduleID]->GetNbinsY();
    double FE0=0;
    double FE1=0;
    double FE2=0;
    double FE3=0;
    double FE4=0;
    double FE5=0;
    double FE6=0;
    double FE7=0;
    double FE8=0;
    double FE9=0;
    double FE10=0;
    double FE11=0;
    double FE12=0;
    double FE13=0;
    double FE14=0;
    double FE15=0;
    //---------------------------------------
    // Variables to identify modules
    //---------------------------------------
    std::string D1A ("D1A");
    std::string D2A ("D2A");
    std::string D3A ("D3A");
    std::string D1C ("D1C");
    std::string D2C ("D2C");
    std::string D3C ("D3C");
    std::string M1 ("M1");
    std::string M2 ("M2");
    std::string M3 ("M3");
    std::string M4 ("M4");
    std::string M5 ("M5");
    std::string M6 ("M6");
    std::size_t disk1A = moduleID.find(D1A);
    std::size_t disk2A = moduleID.find(D2A);
    std::size_t disk3A = moduleID.find(D3A);
    std::size_t disk1C = moduleID.find(D1C);
    std::size_t disk2C = moduleID.find(D2C);
    std::size_t disk3C = moduleID.find(D3C);
    std::size_t M1_find = moduleID.find(M1);
    std::size_t M2_find = moduleID.find(M2);
    std::size_t M3_find = moduleID.find(M3);
    std::size_t M4_find = moduleID.find(M4);
    std::size_t M5_find = moduleID.find(M5);
    std::size_t M6_find = moduleID.find(M6);
    //---------------------------------------
    // Check for hits in FEs
    //---------------------------------------
    for(int k=1; k<=nbinx; k++){
      for(int l=1; l<=nbiny; l++){
	if(k<=18){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE7=FE7+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE8=FE8+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE8=FE8+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE7=FE7+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }
        }
        if(k>=19 and k<=36){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE6=FE6+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE9=FE9+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE9=FE9+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE6=FE6+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }
        }
        if(k>=37 and k<=54){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE5=FE5+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE10=FE10+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE10=FE10+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE5=FE5+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }
        }
        if(k>=55 and k<=72){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE4=FE4+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE11=FE11+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE11=FE11+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE4=FE4+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }
        }
        if(k>=73 and k<=90){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE3=FE3+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE12=FE12+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE12=FE12+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE3=FE3+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }
        }
        if(k>=91 and k<=108){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE2=FE2+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE13=FE13+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE13=FE13+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE2=FE2+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }
        }
        if(k>=109 and k<=126){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE1=FE1+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE14=FE14+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE14=FE14+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE1=FE1+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }  
        }
        if(k>=127){
          if(l<=164){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE0=FE0+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE15=FE15+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else if(l>=165){
            if(((!disk1A or !disk2A or !disk3A) and (M1_find==11 or M2_find==11 or M3_find==11)) or  ((!disk1C or !disk2C or !disk3C) and (M4_find==11 or M5_find==11 or M6_find==11))){
              FE15=FE15+hitMaps[moduleID]->GetBinContent(k,l);
            }
            else{
              FE0=FE0+hitMaps[moduleID]->GetBinContent(k,l);
            }
          }
          else{
            std::cout<<"ERROR"<<" "<<k<<" "<<l<<std::endl;
          }
        }
      }
    }// Hits in FEs checked 
    
    //---------------------------------------
    // Variables for threshold
    //---------------------------------------
    int sum=0;
    int sum_FE=0;
    int denom=16;
    if(FE0==0){denom=denom-1;}
    if(FE1==0){denom=denom-1;}
    if(FE2==0){denom=denom-1;}
    if(FE3==0){denom=denom-1;}
    if(FE4==0){denom=denom-1;}
    if(FE5==0){denom=denom-1;}
    if(FE6==0){denom=denom-1;}
    if(FE7==0){denom=denom-1;}
    if(FE8==0){denom=denom-1;}
    if(FE9==0){denom=denom-1;}
    if(FE10==0){denom=denom-1;}
    if(FE11==0){denom=denom-1;}
    if(FE12==0){denom=denom-1;}
    if(FE13==0){denom=denom-1;}
    if(FE14==0){denom=denom-1;}
    if(FE15==0){denom=denom-1;} 
    
    //---------------------------------------
    // Making list of masked FE
    //---------------------------------------
    int average_hit=0;
    if(denom!=0){ 
     average_hit =(FE0+FE1+FE2+FE3+FE4+FE5+FE6+FE7+FE8+FE9+FE10+FE11+FE12+FE13+FE14+FE15)/16;
    }
    if(FE0<=0.0*average_hit){//value multiplied with average_hit is threshold
      sum=sum+std::pow(2,0);//calculating number to know which FEs in the module shall be masked
      sum_FE++;
    }
    if(FE1<=0.0*average_hit){
      sum=sum+std::pow(2,1);
      sum_FE++;
    }
    if(FE2<=0.0*average_hit){
      sum=sum+std::pow(2,2);
      sum_FE++;
    }
    if(FE3<=0.0*average_hit){
      sum=sum+std::pow(2,3);
      sum_FE++;
    }
    if(FE4<=0.0*average_hit){
      sum=sum+std::pow(2,4);
      sum_FE++;
    }
    if(FE5<=0.0*average_hit){
      sum=sum+std::pow(2,5);
      sum_FE++;
    }
    if(FE6<=0.0*average_hit){
      sum=sum+std::pow(2,6);
      sum_FE++;
    }
    if(FE7<=0.0*average_hit){
      sum=sum+std::pow(2,7);
      sum_FE++;
    }
    if(FE8<=0.0*average_hit){
      sum=sum+std::pow(2,8);
      sum_FE++;
    }
    if(FE9<=0.0*average_hit){
      sum=sum+std::pow(2,9);
      sum_FE++;
    }
    if(FE10<=0.0*average_hit){
      sum=sum+std::pow(2,10);
      sum_FE++;
    }
    if(FE11<=0.0*average_hit){
      sum=sum+std::pow(2,11);
      sum_FE++;
    }
    if(FE12<=0.0*average_hit){
      sum=sum+std::pow(2,12);
      sum_FE++;
    }
    if(FE13<=0.0*average_hit){
      sum=sum+std::pow(2,13);
      sum_FE++;
    }
    if(FE14<=0.0*average_hit){
      sum=sum+std::pow(2,14);
      sum_FE++;
    }
    if(FE15<=0.0*average_hit){
      sum=sum+std::pow(2,15);
      sum_FE++;
    }
    if (sum!=0){
      a++;
      FEcode.push_back(std::to_string(sum));
      vsFE.push_back(moduleID);
      npush_backFE++;
    }
  }
  
}


void setPixelMapping(bool isIBL){
  std::string cmtpath = std::getenv("DATAPATH");
  std::vector<std::string> paths = splitter(cmtpath, ':');
  std::ifstream ifs;
  std::ifstream ifs2;
  std::ifstream ifs3;

  for(const auto& x : paths){
    if(is_file_exist((x + "/PixelMapping_Run2.dat").c_str())){
      std::cout << "datapath: " << x <<" "<<std::endl;
      if(isIBL){
        ifs.open(x + "/PixelMapping_Run2.dat");
        ifs2.open(x + "/HashVSdcsID.dat");
      }else{
        ifs.open(x + "/PixelMapping_May08.dat");
      }
      int tmp_barrel_ec; int tmp_layer; int tmp_module_phi; int tmp_module_eta; std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(4);

      
      while(ifs >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp_module_name) {
        
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        pixelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
      }
      int tmp_hash; int tmp0; int tmp1; int tmp2; int tmp3; int tmp4; std::string tmp_id;
      while(ifs2 >> tmp0 >> tmp_hash >> tmp1 >> tmp2 >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp3 >> tmp4 >> tmp_id) {

        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        hashMapping.push_back(std::make_pair(tmp_hash, tmp_position));
      }
    }
    if(is_file_exist((x + "/table_Run2.txt").c_str())){
      std::cout << "datapath: " << x << std::endl;
      ifs3.open(x + "/table_Run2.txt");
      std::string str;
      std::string tmp_word1, tmp_word2, tmp_word3;
      int tmp_channel;
      int tmp_hashid = 0;
      std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(2);

      int flag_start = 0;
      while(getline(ifs3, str)){
	if(flag_start == 1){
	  int pos = 0;
	  pos = str.find("INFO") + 6;
	  if(str.find("INFO") != std::string::npos) {tmp_channel = atoi( (str.substr(pos, 4)).c_str() );}
	  pos = str.find("[") - 10;
	  if(str.find("[") != std::string::npos) {tmp_hashid = atoi( (str.substr(pos, 8)).c_str() );}
	  pos = str.find("[") + 19;
	  if(str.find("[") != std::string::npos) {tmp_module_name = str.substr(pos, 19);}
	  tmp_position[0] = tmp_channel;
	  tmp_position[1] = tmp_hashid;
	  channelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
	}
	if(str.find("PixMapOverlayWr") != std::string::npos) flag_start = 1;
     }// while(getline(ifs3, str))
     std::cout<<channelMapping.size()<<std::endl;
    }
  }
}

std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta){
  for(unsigned int ii = 0; ii < pixelMapping.size(); ii++) {
    if (pixelMapping[ii].second.size() != 4) {
      std::cout << "getDCSIDFromPosition: Vector size is not 4!" << std::endl;
      return std::string("Error!");
    }
    if (pixelMapping[ii].second[0] != barrel_ec) continue;
    if (pixelMapping[ii].second[1] != layer) continue;
    if (pixelMapping[ii].second[2] != module_phi) continue;
    if (pixelMapping[ii].second[3] != module_eta) continue;
    return pixelMapping[ii].first;
  }
  std::cout << "[getDCSIDFromPosition] Not found!" << std::endl;
  return std::string("Error!");
}

int getHashFromPosition (int barrel_ec, int layer, int module_phi, int module_eta){
  for(unsigned int ii = 0; ii < hashMapping.size(); ii++) {
    if (hashMapping[ii].second.size() != 4) {
      std::cout << "getDCSIDFromPosition: Vector size is not 4!" << std::endl;
      return 0;
    }
    if (hashMapping[ii].second[0] != barrel_ec) continue;
    if (hashMapping[ii].second[1] != layer) continue;
    if (hashMapping[ii].second[2] != module_phi) continue;
    if (hashMapping[ii].second[3] != module_eta) continue;
    return hashMapping[ii].first;
  }
  std::cout << "[getHashFromPosition] Not found!" << std::endl;
  return 0;
}

std::vector<int> getPositionFromDCSID (std::string module_name){
  std::string number1 = "7";
  std::string number2 = "8";
  std::string character = "I";
  if(module_name[13]!=number1 and module_name[13]!=number2 and module_name[1]==character){
    module_name.erase(14,2);
  }
  for(unsigned int ii = 0; ii < pixelMapping.size(); ii++) {
    if (pixelMapping[ii].first == module_name)
      return pixelMapping[ii].second;
  }
  std::cout << "[getPositionFromDCSID] Not found!" << std::endl;
  return pixelMapping[0].second;
}

std::vector<int> getChannelFromHashID (int hashid){
  for(unsigned int ii = 0; ii < channelMapping.size(); ii++) {
    if (channelMapping[ii].second[1] == hashid)
      return channelMapping[ii].second;
  }
  std::cout << "[getChannelFromHashID] Not found!" << std::endl;
  return channelMapping[1].second;
}
//---------------------------------------
// Make a txt file out of masked modules/FEs
//---------------------------------------
void make_txt(std::string srun, int npush_back, int npush_backFE, std::vector<std::string> vsFE, std::vector<std::string> vsmodule, std::vector<int> vLB_start, std::vector<int> vLB_end, std::vector<std::string> FEcode){
  std::string spyfilename = "./PixMapOverlay_run" + srun;
  spyfilename += ".txt";
  std::ofstream txtFile;
  txtFile.open(spyfilename.c_str(),std::ofstream::out);
  std::cout <<"spyfilename: "<<spyfilename << std::endl;
  
  //---------------------------------------
  // List of masked FE
  //---------------------------------------
  std::vector<std::vector<int>> channel_FEcode(npush_backFE+1, std::vector<int>(2));
  std::cout<<npush_backFE<<std::endl;
  if(npush_backFE > 0){
    for(int i=1; i<=npush_backFE; i++){
      
      std::vector<int> position = getPositionFromDCSID(vsFE[i-1]);
      int barrel = position[0];
      int layer = position[1];
      int module_phi = position[2];
      int module_eta = position[3];
      int module_hash = getHashFromPosition(barrel, layer, module_phi, module_eta);
      std::vector<int> channel_set = getChannelFromHashID(module_hash);
      int channel = channel_set[0];
      int FECode =0;
      stringstream FECODE(FEcode[i-1]);
      FECODE >> FECode;
      channel_FEcode[i][0]=channel;
      channel_FEcode[i][1]=FECode;
      for(int l=1; l<i;l++){// Calculating/getting values for list
        if (channel_FEcode[i][0]>channel_FEcode[l][0]){
          continue;
        }
        else{
          for(int k=0; k<i-l; k++){
            channel_FEcode[i-k][0]=channel_FEcode[i-1-k][0];
            channel_FEcode[i-k][1]=channel_FEcode[i-1-k][1];
         }
        }
        channel_FEcode[l][0]=channel;
        channel_FEcode[l][1]=FECode;
        break;
      }
    }
  }
  
  std::vector <std::string>::iterator it_smodule = vsmodule.begin();
  std::vector <int>::iterator it_LBstart = vLB_start.begin();
  std::vector <int>::iterator it_LBend = vLB_end.begin();
  std::cout<<npush_back<<std::endl;
  
  //---------------------------------------
  //List of masked modules and FEs(IBL) per LB
  //---------------------------------------
  std::vector<std::vector<int>> channel_Modulecode(npush_back+1, std::vector<int>(4));
  if(npush_back > 0){
      for(int i=1; i<=npush_back; i++){
        std::vector<int> position = getPositionFromDCSID(*it_smodule);
        std::string module_name = *it_smodule;
        int barrel = position[0];
        int layer = position[1];
        int module_phi = position[2];
        int module_eta = position[3];
        int module_hash = getHashFromPosition(barrel, layer, module_phi, module_eta);
        std::vector<int> channel_set = getChannelFromHashID(module_hash);
        int channel = channel_set[0];       
        unsigned long sRun = std::stoi (srun);
        unsigned long iov_start = (sRun << 32)+*it_LBstart;
        unsigned long iov_end = (sRun << 32)+*it_LBend;
        channel_Modulecode[i][0]=channel;
        channel_Modulecode[i][1]=65535;
        channel_Modulecode[i][2]=iov_start;
        channel_Modulecode[i][3]=iov_end;
        for(int l=1; l<=i;l++){//Calculating/getting values for list
          if (channel_Modulecode[i][0]>channel_Modulecode[l][0]){
            continue;
          }
          else{
            for(int k=0; k<i-l; k++){
              channel_Modulecode[i-k][0]=channel_Modulecode[i-1-k][0];
              channel_Modulecode[i-k][1]=channel_Modulecode[i-1-k][1];
              channel_Modulecode[i-k][2]=channel_Modulecode[i-1-k][2];
              channel_Modulecode[i-k][3]=channel_Modulecode[i-1-k][3];
            }
          }
          channel_Modulecode[l][0]=channel;
          channel_Modulecode[l][1]=65535;
          std::string number1 = "7";
          std::string number2 = "8";
          std::string number3 = "1";
          std::string number4 = "2";
          std::string character = "I"; 
          if( module_name[1]==character and (module_name[13]==number1 or module_name[13]==number2)){
           channel_Modulecode[l][1]=0;
          }
          else if(module_name[13]!=number1 and module_name[13]!=number2 and module_name[1]==character){
           if(module_name[15]==number3){ 
             channel_Modulecode[l][1]=1;
           }
           else if(module_name[15]==number4){
             channel_Modulecode[l][1]=2;
           }
          }

          channel_Modulecode[l][2]=iov_start;
          channel_Modulecode[l][3]=iov_end;
          break;
        }
        
        *it_smodule++;
        *it_LBstart++;
        *it_LBend++;
      }  
  }
  //-----------------------------------
  // Combine both lists
  //-----------------------------------
  std::vector<std::vector<int>> combine(npush_backFE+npush_back+1, std::vector<int>(4));
  std::string module_info;
  for(int i=1; i<=npush_backFE+npush_back; i++){

    if (i<=npush_backFE){
      combine[i][0]=channel_FEcode[i][0];
      combine[i][1]=channel_FEcode[i][1];
      combine[i][2]=0;
      combine[i][3]=0;
      if(combine[i][1]==65535)
        continue;     
    }
    else{
      combine[i][0]=channel_Modulecode[i-npush_backFE][0];
      combine[i][1]=channel_Modulecode[i-npush_backFE][1];
      combine[i][2]=channel_Modulecode[i-npush_backFE][2];
      combine[i][3]=channel_Modulecode[i-npush_backFE][3];
    }

    if(combine[i][1]==65535){
     combine[i][1]=0;
     module_info =std::to_string(combine[i][0])+" "+std::to_string(combine[i][1])+" "+std::to_string(combine[i][2])+" "+std::to_string(combine[i][3])+"\n";
    }
    else{
     module_info =std::to_string(combine[i][0])+" "+std::to_string(combine[i][1])+" "+std::to_string(combine[i][2])+" "+std::to_string(combine[i][3])+"\n";
    }

    txtFile << module_info;
  }
  
  txtFile.close();
}

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
#include<utility>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TKey.h"
#include "Riostream.h"

#include "PixelCalibAlgs/PixelConvert.h"
#include "PixelConditionsData/SpecialPixelMap.h"

std::vector< std::pair< std::string, std::vector<int> > > m_pixelMapping;
std::vector< std::pair< int, std::vector<int> > > m_hashMapping;
std::vector< std::pair< std::string, std::vector<int> > > m_channelMapping;
void setPixelMapping(bool m_isIBL);
std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);
int getHashFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);
std::vector<int> getPositionFromDCSID (std::string module_name);
std::vector<int> getChannelFromHashID (int hashid);
void checkInactiveModule(bool m_isIBL, TFile* hitMapFile, int &npush_back, std::vector<std::string> &vsmodule, std::vector<int> &vLB_start, std::vector<int> &vLB_end);
void make_pyfile(std::string srun, std::string stag_name, int npush_back, std::vector<std::string> vsmodule, std::vector<int> vLB_start, std::vector<int> vLB_end, bool flag_first);


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
  bool m_isIBL = true;
  std::string stag_name = "PixMapOverlay-RUN2-DATA-UPD4-02";

  //-----------------------------------
  //Usage configuration of the program
  //------------------------------------
  std::string runNumber = "0";
  std::string finname = "";
  std::string sflag_first = "";
  bool optionOnline = false;

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
  setPixelMapping(m_isIBL);

  //-----------------------------
  //Open input and output files
  //------------------------------
  int npush_back = 0;
  std::vector<std::string> vsmodule;
  std::vector<int> vLB_start;
  std::vector<int> vLB_end;
  if(flag_first != true){
    TFile* hitMapFile = new TFile(finname.c_str(), "READ");
    if( !hitMapFile || !hitMapFile->IsOpen() ){
      std::cerr << "Error: Unable to open input file." << std::endl;
      return 1;
    }
    checkInactiveModule(m_isIBL, hitMapFile, npush_back, vsmodule, vLB_start, vLB_end);
    
    hitMapFile->Close();
    delete hitMapFile;
  }

  //-----------------------------
  // Open file to save python fragment with disabled modules
  //-----------------------------
  make_pyfile(runNumber, stag_name, npush_back, vsmodule, vLB_start, vLB_end, flag_first);

  return 0;
}

void checkInactiveModule(bool m_isIBL, TFile* hitMapFile, int &npush_back, std::vector<std::string> &vsmodule, std::vector<int> &vLB_start, std::vector<int> &vLB_end){


  TH1D* nEventsHistogram = 0;
  TH1D* nEventsLBHistogram = 0;
  hitMapFile->GetObject("NEvents", nEventsHistogram);
  hitMapFile->GetObject("NEventsLB", nEventsLBHistogram);
  //  double nEvents = nEventsHistogram->GetBinContent(1);

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
  if (m_isIBL) components.push_back("IBL");     // kazuki
  components.push_back("B-layer");
  components.push_back("Layer1");
  components.push_back("Layer2");

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
    std::string lbdepDirName = std::string("LBdep_endcap") + (*hemisphere);
    TDirectory* lbdepDir = 0;

    for(int k = 1; k <= 3; k ++){
      std::ostringstream component, lbdepPath;
      component << "Disk" << k << (*hemisphere);
      lbdepPath << lbdepDirName << "/Disk" << k;

      for(int phi = 0; phi < 48; phi++){
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))->GetListOfKeys()->At(phi)))->GetName());
        lbdep[name] = static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))->GetListOfKeys())->At(phi)))->ReadObj());
        lbdep[name]->SetName(name.c_str());
        //lbdep[name]->Divide(nEventsLBHistogram);
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
  if(m_isIBL) staves.push_back(14);
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  if(m_isIBL) layers.push_back("IBL");
  layers.push_back("B-layer");
  layers.push_back("Layer1");
  layers.push_back("Layer2");

  std::string lbdepDirName("lbdep_barrel");
  TDirectory* lbdepDir = 0;
  for(unsigned int layer = 0; layer < staves.size(); layer++){
    std::ostringstream lbdepPath;
    lbdepPath << "LBdep_barrel/" << layers[layer];

    int nModulesPerStave = 13;
    if (m_isIBL && layer == 0) nModulesPerStave = 20; // --- IBL --- //
    for(int module = 0; module < staves[layer] * nModulesPerStave; module++){ // loop on modules
      std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))->GetListOfKeys()->At(module)))->GetName());

      lbdep[name] = static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get( lbdepPath.str().c_str() ) ))->GetListOfKeys() )->At(module) ))->ReadObj());

      lbdep[name]->SetName(name.c_str());
      //lbdep[name]->Divide(nEventsLBHistogram);
    }
  }

  for(std::map<std::string, TH1D*>::const_iterator module = lbdep.begin(); module != lbdep.end(); ++module){
    std::string moduleID = (*module).first;
    int LB_start = 0;
    int LB_end = 0;
    int flag_start = 0;
    //int flag_end = 0;
    int nbin = nEventsLBHistogram->GetNbinsX();
    for (int LB = 1; LB < nbin; LB++) {
      if(nEventsLBHistogram->GetBinContent(LB + 1) < 10.) continue; // nEventsLBHistogram starts from 0 // #events in LB >= 10

      if(lbdep[moduleID]->GetBinContent(LB + 1) < 1.) { // (#module hits in LB) / (#events in LB) < 1
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

void setPixelMapping(bool m_isIBL){
  std::string cmtpath = std::getenv("DATAPATH");
  std::vector<std::string> paths = splitter(cmtpath, ':');
  std::ifstream ifs;
  std::ifstream ifs2;
  std::ifstream ifs3;

  for (const auto& x : paths){
    if(is_file_exist((x + "/PixelMapping_Run2.dat").c_str())){
      std::cout << "datapath: " << x << std::endl;
      if(m_isIBL){
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
        m_pixelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
      }
      int tmp_hash; int tmp0; int tmp1; int tmp2; int tmp3; int tmp4; std::string tmp_id;
      while(ifs2 >> tmp0 >> tmp_hash >> tmp1 >> tmp2 >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp3 >> tmp4 >> tmp_id) {
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        m_hashMapping.push_back(std::make_pair(tmp_hash, tmp_position));
      }
      //break;
    }else if(is_file_exist((x + "/table_Run2.txt").c_str())){
      ifs3.open(x + "/table_Run2.txt");
      std::string str;
      std::string tmp_word1, tmp_word2, tmp_word3;
      int tmp_channel, tmp_hashid;
      std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(2);

      int flag_start = 0;
      while(getline(ifs3, str)){
	if(flag_start == 1){
	  int pos = 0;
	  pos = str.find("INFO") + 6;
	  if(str.find("INFO") != std::string::npos) tmp_channel = atoi( (str.substr(pos, 4)).c_str() );
	  pos = str.find("[") - 10;
	  if(str.find("[") != std::string::npos) tmp_hashid = atoi( (str.substr(pos, 8)).c_str() );
	  pos = str.find("[") + 19;
	  if(str.find("[") != std::string::npos) tmp_module_name = str.substr(pos, 19);
	  tmp_position[0] = tmp_channel;
	  tmp_position[1] = tmp_hashid;
	  m_channelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
	  //std::cout << tmp_module_name << ": " << tmp_position[0] << ", " << tmp_position[1] << std::endl;
	}
	if(str.find("PixMapOverlayWr") != std::string::npos) flag_start = 1;
      }//while(getline(ifs3, str))
    }
  }
}

std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta){
  for(unsigned int ii = 0; ii < m_pixelMapping.size(); ii++) {
    if (m_pixelMapping[ii].second.size() != 4) {
      std::cout << "getDCSIDFromPosition: Vector size is not 4!" << std::endl;
      return std::string("Error!");
    }
    if (m_pixelMapping[ii].second[0] != barrel_ec) continue;
    if (m_pixelMapping[ii].second[1] != layer) continue;
    if (m_pixelMapping[ii].second[2] != module_phi) continue;
    if (m_pixelMapping[ii].second[3] != module_eta) continue;
    return m_pixelMapping[ii].first;
  }
  std::cout << "[getDCSIDFromPosition] Not found!" << std::endl;
  return std::string("Error!");
}

int getHashFromPosition (int barrel_ec, int layer, int module_phi, int module_eta){
  for(unsigned int ii = 0; ii < m_hashMapping.size(); ii++) {
    if (m_hashMapping[ii].second.size() != 4) {
      std::cout << "getDCSIDFromPosition: Vector size is not 4!" << std::endl;
      return 0;
    }
    if (m_hashMapping[ii].second[0] != barrel_ec) continue;
    if (m_hashMapping[ii].second[1] != layer) continue;
    if (m_hashMapping[ii].second[2] != module_phi) continue;
    if (m_hashMapping[ii].second[3] != module_eta) continue;
    return m_hashMapping[ii].first;
  }
  std::cout << "[getHashFromPosition] Not found!" << std::endl;
  return 0;
}

std::vector<int> getPositionFromDCSID (std::string module_name){
  for(unsigned int ii = 0; ii < m_pixelMapping.size(); ii++) {
    if (m_pixelMapping[ii].first == module_name)
      return m_pixelMapping[ii].second;
  }
  std::cout << "[getPositionFromDCSID] Not found!" << std::endl;
  return m_pixelMapping[0].second;
}

std::vector<int> getChannelFromHashID (int hashid){
  for(unsigned int ii = 0; ii < m_channelMapping.size(); ii++) {
    if (m_channelMapping[ii].second[1] == hashid)
      return m_channelMapping[ii].second;
  }
  std::cout << "[getChannelFromHashID] Not found!" << std::endl;
  return m_channelMapping[1].second;
}

void make_pyfile(std::string srun, std::string stag_name, int npush_back, std::vector<std::string> vsmodule, std::vector<int> vLB_start, std::vector<int> vLB_end, bool flag_first){
  std::string spyfilename = "./PixMapOverlay_run" + srun;
  spyfilename += ".py";
  std::ofstream pyFile;
  pyFile.open(spyfilename.c_str(),std::ofstream::out);
  std::cout << spyfilename << std::endl;

  std::string sdbfilename = "PixMapOverlay_run" + srun;
  sdbfilename += ".db";

  pyFile << "import os, time,sys" << std::endl;
  pyFile << "from PyCool import cool" << std::endl;
  pyFile << "from CoolConvUtilities import AtlCoolLib" << std::endl;
  pyFile << "# For resolving tags" << std::endl;
  pyFile << "sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')" << std::endl;
  pyFile << "dbFile = '"<< sdbfilename << "'" << std::endl;
  pyFile << "dbName = 'CONDBR2'" << std::endl;
  pyFile << "path='/PIXEL/PixMapOverlay'" << std::endl;
  //  pyFile << "tag = 'PixMapOverlay-Test'" << std::endl;
  pyFile << "tag = '" << stag_name << "'" << std::endl;
  pyFile << "try:" << std::endl;
  pyFile << "    os.remove(dbFile)" << std::endl;
  pyFile << "except:" << std::endl;
  pyFile << "    pass" << std::endl;
  pyFile << "dbSvc = cool.DatabaseSvcFactory.databaseService()" << std::endl;
  pyFile << "dbString = \"sqlite://;schema=%s;dbname=%s\" % (dbFile, dbName)" << std::endl;
  pyFile << "try:" << std::endl;
  pyFile << "    db = dbSvc.createDatabase(dbString)" << std::endl;
  pyFile << "except Exception, e:" << std::endl;
  pyFile << "    print 'Problem creating database', e" << std::endl;
  pyFile << "    sys.exit(-1)" << std::endl;
  pyFile << "print \"Created database\", dbString" << std::endl;
  pyFile << "spec = cool.RecordSpecification()" << std::endl;
  pyFile << "spec.extend('moduleID', cool.StorageType.Int32)" << std::endl;
  pyFile << "spec.extend('ModuleSpecialPixelMap_Clob', cool.StorageType.String4k)" << std::endl;
  pyFile << "desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>'" << std::endl;
  pyFile << "folderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec)" << std::endl;
  pyFile << "folder = db.createFolder(path, folderSpec, desc, True)" << std::endl;
  pyFile << "data = cool.Record(spec)" << std::endl;
  pyFile << "" << std::endl;

  pyFile << "# Write BAD modules" << std::endl;
  if(flag_first == true){
    std::cout << "Make py-file for initialization." << std::endl;
    pyFile << "for channel in range(0, 2048):" << std::endl;
    pyFile << "    print \"Good modules: run\", 0, \"LB\", 0, \"-\", \"run\", 2147483647, \"LB\", 4294967295, \": \", channel" << std::endl;
    pyFile << "    since = (0 << 32) + 0" << std::endl;
    pyFile << "    until = (2147483647 << 32) + 4294967295" << std::endl;
    pyFile << "" << std::endl;
    pyFile << "    data['moduleID'] = channel" << std::endl;
    pyFile << "    data['ModuleSpecialPixelMap_Clob'] = '0 0'" << std::endl;
    pyFile << "    channel = channel + 1" << std::endl;
    pyFile << "    folder.storeObject(since, until, data, channel, tag)" << std::endl;
    pyFile << "" << std::endl;
  }else{
    std::cout << "Initialize for run" << srun << std::endl;
    pyFile << "for channel in range(0, 2048):" << std::endl;
    pyFile << "    print \"Good modules: run\", " << srun << ", \"LB\", 0, \"-\", \"run\", " << srun << ", \"LB\", 4294967295, \": \", channel" << std::endl;
    pyFile << "    since = (" << srun << " << 32) + 0" << std::endl;
    pyFile << "    until = (" << srun << " << 32) + 4294967295" << std::endl;
    pyFile << "" << std::endl;
    pyFile << "    data['moduleID'] = channel" << std::endl;
    pyFile << "    data['ModuleSpecialPixelMap_Clob'] = '0 0'" << std::endl;
    pyFile << "    channel = channel + 1" << std::endl;
    pyFile << "    folder.storeObject(since, until, data, channel, tag)" << std::endl;
    pyFile << "" << std::endl;

    std::vector <std::string>::iterator it_smodule = vsmodule.begin();
    std::vector <int>::iterator it_LBstart = vLB_start.begin();
    std::vector <int>::iterator it_LBend = vLB_end.begin();
    for(int i=0; i<npush_back; i++){
      std::vector<int> position = getPositionFromDCSID(*it_smodule);
      int barrel = position[0];
      int layer = position[1];
      int module_phi = position[2];
      int module_eta = position[3];
      int module_hash = getHashFromPosition(barrel, layer, module_phi, module_eta);
      //std::string dcs_position = getDCSIDFromPosition(barrel, layer, module_phi, module_eta);
      std::vector<int> channel_set = getChannelFromHashID(module_hash);
      int channel = channel_set[0];
      
      std::cout << *it_smodule << "(" << channel << "): " << *it_LBstart << "-" << (*it_LBend + 1) << std::endl;
      if(i == 0){
	pyFile << "badModulesList = [";
	pyFile << "[" << srun << ", " << *it_LBstart << ", " << (*it_LBend + 1) << ", " << channel << "]," << std::endl;
      }else if(i != (npush_back-1)){ 
	pyFile << "                  [" << srun << ", " << *it_LBstart << ", " << (*it_LBend + 1) << ", " << channel << "]," << std::endl;
      }else{
	pyFile << "                  [" << srun << ", " << *it_LBstart << ", " << (*it_LBend + 1)<< ", " << channel << "]]" << std::endl;
      }//(i != (npush_back-1))
      it_smodule++;
      it_LBstart++;
      it_LBend++;
    }//for(int i=0; i<npush_back; i++)

    pyFile << "nIOVs = len(badModulesList)" << std::endl;
    pyFile << "for iIOV in range(nIOVs):" << std::endl;
    pyFile << "    print \"BAD modules: run\", badModulesList[iIOV][0], \"LB\", badModulesList[iIOV  ][1], \"-\", \"run\", badModulesList[iIOV][0], \"LB\", badModulesList[iIOV][2], \": \", badModulesList[iIOV][3]" << std::endl;
    pyFile << "    since = (badModulesList[iIOV][0] << 32) + badModulesList[iIOV][1]" << std::endl;
    pyFile << "    until = (badModulesList[iIOV][0] << 32) + badModulesList[iIOV][2]" << std::endl;
    pyFile << "" << std::endl;
    pyFile << "    data['moduleID'] = badModulesList[iIOV][3]" << std::endl;
    pyFile << "    data['ModuleSpecialPixelMap_Clob'] = '1 0'" << std::endl;
    pyFile << "    channel = badModulesList[iIOV][3] + 1" << std::endl;
    pyFile << "    folder.storeObject(since, until, data, channel, tag)" << std::endl;
  }// if(flag_first == true)
  pyFile << "" << std::endl;
  pyFile << "# Close database" << std::endl;
  pyFile << "db.closeDatabase()" << std::endl;
  pyFile.close();
}

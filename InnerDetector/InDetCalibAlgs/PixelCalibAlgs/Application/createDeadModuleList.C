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

std::vector< std::pair< std::string, std::vector<int> > > pixelMapping;
//std::vector< std::pair< int, std::vector<int> > > m_moduleHashMapping;
std::vector< std::pair< int, std::vector<int> > > hashMapping;
std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);
int getHashFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);
std::vector<int> getPositionFromDCSID (std::string module_name);

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
  const bool isIBL = true;

  //-----------------------------------
  //Usage configuration of the program
  //------------------------------------
  std::string option;
  std::string runNumber;
  std::string finname;

  //bool optionOnline = false;

  if( argc == 3 ){
    runNumber=argv[argc - 2];
    finname=argv[argc - 1];
  }
  else{
    std::cout << "Usage: updateDeadMaps <runNumber:222222> <inputfile> " << std::endl;
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

  TH1D* nEventsHistogram = 0;
  TH1D* nEventsLBHistogram = 0;
  hitMapFile->GetObject("NEvents", nEventsHistogram);
  hitMapFile->GetObject("NEventsLB", nEventsLBHistogram);
  //double nEvents = nEventsHistogram->GetBinContent(1);

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
  if (isIBL) components.push_back("IBL");     // kazuki
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

    //TDirectory* lbdepDir = 0;

    for(int k = 1; k <= 3; k ++){

      std::ostringstream component, lbdepPath;

      component << "Disk" << k << (*hemisphere);
      lbdepPath << lbdepDirName << "/Disk" << k;

      //TDirectory* lbdepSubDir = 0;

      //if( !optionOnline ){
      //  lbdepSubDir = lbdepDir->mkdir(component.str().substr(0,5).c_str());
      //}

      for(int phi = 0; phi < 48; phi++){
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))
                ->GetListOfKeys()->At(phi)))->GetName());

        lbdep[name] =
          static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))
                    ->GetListOfKeys())->At(phi)))->ReadObj());

        lbdep[name]->SetName(name.c_str());
        //lbdep[name]->SetDirectory(lbdepSubDir);

        lbdep[name]->Divide(nEventsLBHistogram);

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
  layers.push_back("B-layer");
  layers.push_back("Layer1");
  layers.push_back("Layer2");


  std::string lbdepDirName("lbdep_barrel");

  //TDirectory* lbdepDir = 0;

  for(unsigned int layer = 0; layer < staves.size(); layer++){

    std::ostringstream lbdepPath;

    lbdepPath << "LBdep_barrel/" << layers[layer];

    //TDirectory* lbdepSubDir = 0;
    //
    // if( !optionOnline ){
    //   lbdepSubDir = lbdepDir->mkdir(layers[layer].c_str());
    // }

    int nModulesPerStave = 13;
    if (isIBL && layer == 0) nModulesPerStave = 20; // --- IBL --- //
    for(int module = 0; module < staves[layer] * nModulesPerStave; module++) // loop on modules
      //for(int j = 0; j < (staves[layer] * 13); j++)
    {

      std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(lbdepPath.str().c_str())))
              ->GetListOfKeys()->At(module)))->GetName());

      lbdep[name] =
        static_cast<TH1D*>
        (
         (static_cast<TKey*>
          (
           (
            (
             static_cast<TDirectory*>
             (
              hitMapFile->Get( lbdepPath.str().c_str() )
             )
            )->GetListOfKeys()
           )->At(module)
          )
         )->ReadObj()
        );


      lbdep[name]->SetName(name.c_str());
      //lbdep[name]->SetDirectory(lbdepSubDir);

      lbdep[name]->Divide(nEventsLBHistogram);
    }
  }


  //std::string testarea = std::getenv("TestArea");
  //ifstream ifs;
  char* tmppath = std::getenv("DATAPATH");
  if(tmppath == NULL){
      std::cout << "FATAL: Unable to retrieve environmental DATAPATH" << std::endl;
      exit(EXIT_FAILURE);
  }
  std::string cmtpath(tmppath);
  std::vector<std::string> paths = splitter(cmtpath, ':');
  std::ifstream ifs;
  std::ifstream ifs2;
  //std::map <std::string, int> DCSID2Hash;
  for (const auto& x : paths){
    if(is_file_exist((x + "/PixelMapping_Run2.dat").c_str())){
      if(isIBL){
        ifs.open(x + "/PixelMapping_Run2.dat");
        ifs2.open(x + "/HashVSdcsID.dat");
      } else {
        ifs.open(x + "/PixelMapping_May08.dat");
      }
      int tmp_barrel_ec; int tmp_layer; int tmp_module_phi; int tmp_module_eta; std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(4);
      //int counter = 0; // debug
      while(ifs >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp_module_name) {
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        pixelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
      }
      int tmp_moduleHash; int tmp_hash; int tmp1; int tmp2; int tmp3; int tmp4; std::string tmp_id;
      while(ifs2 >> tmp_moduleHash >> tmp_hash >> tmp1 >> tmp2 >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp3 >> tmp4 >> tmp_id) {
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        //hashMapping.push_back(std::make_pair(tmp_hash, tmp_position));
        hashMapping.push_back(std::make_pair(tmp_moduleHash, tmp_position));
      }
      break;
    }
  }


  //int alternativeNDisabledModules = 0;

  std::map<int, std::vector<std::string> > deadModuleList; // <LB, list>
  std::vector<std::string> tmp_list;
  int lastLB = 1;

  for (int LB = 1; LB < 2000; LB++) {
    if(nEventsLBHistogram->GetBinContent(LB + 1) < 10.) continue; // nEventsLBHistogram starts from 0 // #events in LB >= 10
    tmp_list.clear();
    //for(std::map<unsigned int, TH2D*>::const_iterator module = lbdep.begin(); module != lbdep.end(); ++module)
    for(std::map<std::string, TH1D*>::const_iterator module = lbdep.begin(); module != lbdep.end(); ++module)
    {

      std::string moduleID = (*module).first;

      if(lbdep[moduleID]->GetBinContent(LB + 1) < 1.) { // (#module hits in LB) / (#events in LB) < 1
        tmp_list.push_back(moduleID);
      }

      //std::vector<int> position = getPositionFromDCSID(moduleID);
      //int barrel = position[0];
      //int layer = position[1];
      //int module_phi = position[2];
      //int module_eta = position[3];
      //unsigned int component_index = 0;

    } // end loop over all modules
    if (deadModuleList[lastLB] != tmp_list) {
      deadModuleList[LB] = tmp_list;
      lastLB = LB;
    }
  } // end loop over LBs

  // Open file to save python fragment with disabled modules
  std::ofstream pyFile;
  std::stringstream pyfilename;
  pyfilename << "./python/WriteSQLite_run" << runNumber << ".py";
  pyFile.open(pyfilename.str().c_str(),std::ofstream::out);

  pyFile << "import os, time,sys" << std::endl;
  pyFile << " " << std::endl;
  pyFile << "from PyCool import cool" << std::endl;
  pyFile << "from CoolConvUtilities import AtlCoolLib" << std::endl;
  pyFile << "" << std::endl;
  pyFile << "# For resolving tags" << std::endl;
  pyFile << "sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')" << std::endl;
  pyFile << "" << std::endl;
  pyFile << "dbFile = 'Example.db'" << std::endl;
  pyFile << "dbName = 'CONDBR2'" << std::endl;
  pyFile << "path='/PIXEL/PixMapOverlay'" << std::endl;
  pyFile << "tag = 'PixMapOverlay-Test'" << std::endl;
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
  pyFile << "# run LB" << std::endl;
  pyFile << "runLumiList=[]" << std::endl;
  pyFile << "# Bad Pixel moduleID" << std::endl;
  pyFile << "badModulesList=[]" << std::endl;

  std::map<int, std::vector<std::string> >::const_iterator iter;
  for(iter = deadModuleList.begin(); iter != deadModuleList.end(); iter++){
    if( (iter->second).size() < 1) continue;
    int LB = iter->first;
    pyFile << "runLumiList.append([" << runNumber << ", " << LB << "])" << std::endl;
    pyFile << "badModulesList.append([";
    std::vector<std::string>::const_iterator module_iter;
    for(module_iter = (iter->second).begin(); module_iter != (iter->second).end(); module_iter++){
      std::string moduleID = *module_iter;
      std::vector<int> position = getPositionFromDCSID(moduleID);
      int barrel = position[0];
      int layer = position[1];
      int module_phi = position[2];
      int module_eta = position[3];
      //pyFile[LB] << "job.PixMapOverlayWriter.Modules += [ \"" << getHashFromPosition(barrel, layer, module_phi, module_eta) << "\" ] # " << moduleID << std::endl;
      //pyFile[LB] << "job.PixMapOverlayWriter.Modules += [ " << getHashFromPosition(barrel, layer, module_phi, module_eta) << " ] # " << moduleID << std::endl;
      pyFile << getHashFromPosition(barrel, layer, module_phi, module_eta) << ", ";
    }
    pyFile << "])" << std::endl;;
  }

  //pyFile << "runLumiList=[[290000, 0], " << std::endl;
  //pyFile << "             [290000, 10], " << std::endl;
  //pyFile << "             [290000, 20], " << std::endl;
  //pyFile << "             [2147483647,4294967295]] " << std::endl;
  pyFile << "runLumiList.append([2147483647,4294967295])" << std::endl;
  //pyFile << "# Bad Pixel moduleID" << std::endl;
  //pyFile << "badModulesList=[[1, 2, 3], " << std::endl;
  //pyFile << "                [1, 2, 4]," << std::endl;
  //pyFile << "                [1, 4]]" << std::endl;
  pyFile << "" << std::endl;
  pyFile << "nIOVs = len(runLumiList)-1" << std::endl;
  pyFile << "if nIOVs != len(badModulesList):" << std::endl;
  pyFile << "    print 'len(runLumiList)-1 = ', nIOVs" << std::endl;
  pyFile << "    print 'len(badModulesList) = ', len(badModulesList)" << std::endl;
  pyFile << "    print 'exit'" << std::endl;
  pyFile << "    sys.exit(-1)" << std::endl;
  pyFile << "" << std::endl;
  pyFile << "# Write GOOD modules. Here all modules are GOOD. Bad modules are overwritten. " << std::endl;
  pyFile << "print \"GOOD modules: run\", runLumiList[0][0], \"LB\", runLumiList[0][1], \"-\", \"run\", runLumiList[nIOVs][0], \"LB\", runLumiList[nIOVs][1]" << std::endl;
  pyFile << "since = (runLumiList[0    ][0] << 32) + runLumiList[0    ][1]" << std::endl;
  pyFile << "until = (runLumiList[nIOVs][0] << 32) + runLumiList[nIOVs][1]" << std::endl;
  pyFile << "for channel in range(2049):" << std::endl;
  pyFile << "    moduleID = channel-1" << std::endl;
  pyFile << "    data['moduleID'] = moduleID" << std::endl;
  pyFile << "    data['ModuleSpecialPixelMap_Clob'] = 'GOOD'" << std::endl;
  pyFile << "    folder.storeObject(since, until, data, channel, tag)" << std::endl;
  pyFile << "" << std::endl;
  pyFile << "# Write BAD modules" << std::endl;
  pyFile << "for iIOV in range(nIOVs):" << std::endl;
  pyFile << "    print \"BAD modules: run\", runLumiList[iIOV  ][0], \"LB\", runLumiList[iIOV  ][1], \"-\", \"run\", runLumiList[iIOV+1][0], \"LB\", runLumiList[iIOV+1][1]" << std::endl;
  pyFile << "    print len(badModulesList[iIOV]), \"bad modules\"" << std::endl;
  pyFile << "    since = (runLumiList[iIOV  ][0] << 32) + runLumiList[iIOV  ][1]" << std::endl;
  pyFile << "    until = (runLumiList[iIOV+1][0] << 32) + runLumiList[iIOV+1][1]" << std::endl;
  pyFile << "" << std::endl;
  pyFile << "    for channel in range(2049):" << std::endl;
  pyFile << "        moduleID = channel-1" << std::endl;
  pyFile << "        if moduleID in badModulesList[iIOV]:" << std::endl;
  pyFile << "            data['moduleID'] = moduleID" << std::endl;
  pyFile << "            data['ModuleSpecialPixelMap_Clob'] = 'BAD'" << std::endl;
  pyFile << "            folder.storeObject(since, until, data, channel, tag)" << std::endl;
  pyFile << "" << std::endl;
  pyFile << "# Close database" << std::endl;
  pyFile << "db.closeDatabase()" << std::endl;

#if 0
  std::map<int, std::ofstream> pyFile;
  std::map<int, std::vector<std::string> >::const_iterator iter;
  for(iter = deadModuleList.begin(); iter != deadModuleList.end(); iter++){
    if( (iter->second).size() < 1) continue;
    int LB = iter->first;
    std::stringstream pyfilename;
    pyfilename << "./python/PixMapOverlay_run" << runNumber << "_LB" << LB << ".py";
    pyFile[LB].open(pyfilename.str().c_str(),std::ofstream::out);
    //
    pyFile[LB] << "import AthenaCommon.AtlasUnixStandardJob" << std::endl;
    pyFile[LB] << "" << std::endl;
    pyFile[LB] << "from AthenaCommon.GlobalFlags import globalflags" << std::endl;
    pyFile[LB] << "globalflags.DetGeo = 'atlas'" << std::endl;
    pyFile[LB] << "globalflags.DataSource = 'data'" << std::endl;
    pyFile[LB] << "globalflags.InputFormat = 'pool'" << std::endl;
    pyFile[LB] << "globalflags.print_JobProperties()" << std::endl;
    pyFile[LB] << "globalflags.DetDescrVersion = \"ATLAS-R2-2015-03-01-00\" #\"ATLAS-IBL-02-00-00\"" << std::endl;
    pyFile[LB] << "" << std::endl;
    pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from AthenaCommon.DetFlags import DetFlags" << std::endl;
  pyFile[LB] << "DetFlags.all_setOff()" << std::endl;
  pyFile[LB] << "DetFlags.pixel_setOn()" << std::endl;
  pyFile[LB] << "DetFlags.Print()" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from IOVDbSvc.CondDB import conddb" << std::endl;
  pyFile[LB] << "#Fares: change for data map:" << std::endl;
  pyFile[LB] << "#conddb.setGlobalTag('OFLCOND-RUN12-SDR-22')#'OFLCOND-MC12-SDR-05')" << std::endl;
  pyFile[LB] << "conddb.setGlobalTag('CONDBR2-BLKPA-2015-11')#'OFLCOND-MC12-SDR-05')" << std::endl;
  pyFile[LB] << "" << std::endl;
  //pyFile[LB] << "conddb.iovdbsvc.dbConnection = \"sqlite://;schema=DeadModule_run" << runNumber << "_LB" << LB << ".db;dbname=CONDBR2\" #CONDBR2" << std::endl;
  pyFile[LB] << "conddb.iovdbsvc.dbConnection = \"sqlite://;schema=DeadModule_run" << runNumber << ".db;dbname=CONDBR2\" #CONDBR2" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from AtlasGeoModel import SetGeometryVersion" << std::endl;
  pyFile[LB] << "from AtlasGeoModel import GeoModelInit" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from AthenaCommon.AlgSequence import AlgSequence" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "job = AlgSequence()" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from PixelCalibAlgs.PixelCalibAlgsConf import PixMapOverlayWriter" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "job += PixMapOverlayWriter()" << std::endl;
  pyFile[LB] << "job.PixMapOverlayWriter.ModuleStatus = 1" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from AthenaServices.AthenaServicesConf import AthenaOutputStream" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "job += AthenaOutputStream( \"Stream1\" )" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "ServiceMgr += SpecialPixelMapSvc()" << std::endl;
  pyFile[LB] << "SpecialPixelMapSvc = ServiceMgr.SpecialPixelMapSvc" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "SpecialPixelMapSvc.RegisterCallback = False    " << std::endl;
  pyFile[LB] << "SpecialPixelMapSvc.OutputFolder = \"/PIXEL/PixMapOverlay\"" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "### configure OutputConditionsAlg" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "OutputConditionsAlg = OutputConditionsAlg(\"OutputConditionsAlg\",\"PixMapOverlay.root\")" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "OutputConditionsAlg.ObjectList=[ \"CondAttrListCollection#\" + SpecialPixelMapSvc.OutputFolder ]" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "OutputConditionsAlg.WriteIOV=True" << std::endl;
  pyFile[LB] << "" << std::endl;
  //==============================================================================
  pyFile[LB] << "OutputConditionsAlg.Run1=" << runNumber << std::endl;
  pyFile[LB] << "OutputConditionsAlg.LB1=" << LB << std::endl;
  pyFile[LB] << "OutputConditionsAlg.Run2=" << runNumber << std::endl;
  pyFile[LB] << "OutputConditionsAlg.LB2=2000" << std::endl;
  //===============================================================================
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "OutputConditionsAlg.IOVTagList=[\"PixMapOverlay-RUN2-DATA-TEST\"]" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "### configure IOVRegistrationSvc" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "ServiceMgr += IOVRegistrationSvc()" << std::endl;
  pyFile[LB] << "regSvc = ServiceMgr.IOVRegistrationSvc" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "regSvc.OverrideNames = [ \"ModuleSpecialPixelMap_Clob\" ]" << std::endl;
  pyFile[LB] << "regSvc.OverrideTypes = [ \"String4k\" ]" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "### configure the event selector" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "from GaudiSvc.GaudiSvcConf import EventSelector" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "ServiceMgr += EventSelector()" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "EventSelector.RunNumber         = 0" << std::endl;
  pyFile[LB] << "EventSelector.EventsPerRun      = 5" << std::endl;
  pyFile[LB] << "EventSelector.FirstEvent        = 1" << std::endl;
  pyFile[LB] << "EventSelector.InitialTimeStamp  = 0" << std::endl;
  pyFile[LB] << "EventSelector.TimeStampInterval = 5" << std::endl;
  pyFile[LB] << "theApp.EvtMax                   = 1" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "### configure the message service" << std::endl;
  pyFile[LB] << "# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )" << std::endl;
  pyFile[LB] << "" << std::endl;
  pyFile[LB] << "MessageSvc.OutputLevel      = 3" << std::endl;
  pyFile[LB] << "MessageSvc.debugLimit       = 100000" << std::endl;
  pyFile[LB] << "MessageSvc.infoLimit        = 100000" << std::endl;
  pyFile[LB] << "MessageSvc.errorLimit       = 1000" << std::endl;
  pyFile[LB] << "" << std::endl;

  //std::cout << "Dead module: "  << iter->second->GetTitle() << std::endl;
  std::vector<std::string>::const_iterator module_iter;
  for(module_iter = (iter->second).begin(); module_iter != (iter->second).end(); module_iter++){
    std::string moduleID = *module_iter;
    std::vector<int> position = getPositionFromDCSID(moduleID);
    int barrel = position[0];
    int layer = position[1];
    int module_phi = position[2];
    int module_eta = position[3];
    //pyFile[LB] << "job.PixMapOverlayWriter.Modules += [ \"" << getHashFromPosition(barrel, layer, module_phi, module_eta) << "\" ] # " << moduleID << std::endl;
    pyFile[LB] << "job.PixMapOverlayWriter.Modules += [ " << getHashFromPosition(barrel, layer, module_phi, module_eta) << " ] # " << moduleID << std::endl;
  }
  pyFile[LB].close();
} 
#endif

hitMapFile->Close();
delete hitMapFile;


return 0;
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
  std::cout << "Not found!" << std::endl;
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
  std::cout << "Not found!" << std::endl;
  return 0;
}

std::vector<int> getPositionFromDCSID (std::string module_name){
  for(unsigned int ii = 0; ii < pixelMapping.size(); ii++) {
    if (pixelMapping[ii].first == module_name)
      return pixelMapping[ii].second;
  }
  std::cout << "Not found!" << std::endl;
  return pixelMapping[0].second;
}

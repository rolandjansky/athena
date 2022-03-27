//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <set>
#include <algorithm>
#include <unistd.h>

#include "EventTPCnv/EventStreamInfo_p3.h"
#include "EventTPCnv/EventStreamInfo_p2.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"
#ifndef SIMULATIONBASE
#include "ByteStreamEventTPCnv/ByteStreamMetadata_p1.h"
#endif // not SIMULATIONBASE
#include "FileMetaData.h"
#include "CxxUtils/checker_macros.h"




class PoolFilePeeker {

public:
  PoolFilePeeker(const char* tf, const bool vbs=false);

  const FileMetaData& get() const {return m_fmd;}

private:
  template<typename T>
  bool findBranch(TBranch* br, const std::string& nametag, const std::string& type,
                  std::pair<bool,T*>& destination) const;

  FileMetaData m_fmd;
};




template<typename T>
bool PoolFilePeeker::findBranch(TBranch* br, const std::string& nametag, const std::string& type,
                                std::pair<bool,T*>& destination) const {
  const std::string brName(br->GetName());
  const std::string brType(br->GetClassName());
  if (brName.find(nametag)!=std::string::npos && brType==type) {
    if (destination.first) {
      std::cerr << "WARNING: Found more than one " << type << " labelled '" << nametag << "'. Ignoring " << brName << std::endl;
    }
    else {
      destination.first=true;
      br->SetAddress(&destination.second);
    }
    return true;
  }
  else
    return false;
}


PoolFilePeeker::PoolFilePeeker(const char* filename, const bool vbs) {


  m_fmd.m_fileName=filename;

  auto start = std::chrono::system_clock::now();
  std::unique_ptr<TFile> tf(TFile::Open(filename));
  if (!tf || tf->IsZombie()) {
    std::cerr << "Failed to open file " << filename << std::endl;
    return;
  }
  auto stop = std::chrono::system_clock::now();

  if (vbs) std::cout << "Time to open input file: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count() << " msec" << std::endl;


  TTree* md=(TTree*)tf->Get("MetaData");

  if (!md) return;

  std::pair<bool,EventStreamInfo_p3*> esi{false,nullptr};
  std::pair<bool,EventStreamInfo_p2*> esi2{false,nullptr};
  std::pair<bool,IOVMetaDataContainer_p1*> taginfo{false,nullptr};
  std::pair<bool,IOVMetaDataContainer_p1*> genparam{false,nullptr};
  std::pair<bool,IOVMetaDataContainer_p1*> simparam{false,nullptr};
  std::pair<bool,IOVMetaDataContainer_p1*> digiparam{false,nullptr};
#ifndef SIMULATIONBASE
  std::pair<bool,ByteStreamMetadataContainer_p1*> bmdc{false,nullptr};
#endif // not SIMULATIONBASE
  TObjArray* mdBranches=md->GetListOfBranches();
  const size_t mdnBranches=mdBranches->GetEntriesFast();
  for (size_t i=0;i<mdnBranches;++i) {
    TBranch* br=(TBranch*)mdBranches->At(i);
    if (vbs) std::cout << "Branch "<< br->GetName() << " of type " << br->GetClassName() << std::endl;
#ifndef SIMULATIONBASE
    if (findBranch(br,"ByteStreamMetadata","ByteStreamMetadataContainer_p1",bmdc)) continue;
#endif // not SIMULATIONBASE
    if (findBranch(br,"_TagInfo","IOVMetaDataContainer_p1",taginfo)) continue;
    if (findBranch(br,"_Generation_Parameters","IOVMetaDataContainer_p1",genparam)) continue;
    if (findBranch(br,"_Simulation_Parameters","IOVMetaDataContainer_p1",simparam)) continue;
    if (findBranch(br,"_Digitization_Parameters","IOVMetaDataContainer_p1",digiparam)) continue;
    if (findBranch(br,"Stream","EventStreamInfo_p3",esi)) continue;
    if (findBranch(br,"Stream","EventStreamInfo_p2",esi2)) continue;
  }//end loop over branches


  //Get the event!
  md->GetEntry(0);

  m_fmd.m_valid=true;

  if (genparam.first) {
    if (vbs) std::cout << " Got IOVMetaDataContainer for /Generation/Parameters " << std::endl;
    for (const auto& idx : genparam.second->m_payload.m_attrIndexes) {
      const unsigned short nameIdx=idx.nameIndex();
      if (idx.typeIndex()==IOVPayloadContainer_p1::ATTR_STRING) {
        const unsigned short iObj=idx.objIndex();
        const std::string objKey=genparam.second->m_payload.m_attrName[nameIdx];
        const std::string objValue=genparam.second->m_payload.m_string[iObj];
        if (vbs) std::cout << "Genparam: " << objKey << " : " <<  objValue << std::endl;

      }
    }//end loop over genparam
    delete genparam.second;
    genparam.second=nullptr;
  }//end if have genparam

  if (simparam.first) {
    if (vbs) std::cout << " Got IOVMetaDataContainer for /Simulation/Parameters " << std::endl;
    for (const auto& idx : simparam.second->m_payload.m_attrIndexes) {
      const unsigned short nameIdx=idx.nameIndex();
      if (idx.typeIndex()==IOVPayloadContainer_p1::ATTR_STRING) {
        const unsigned short iObj=idx.objIndex();
        const std::string objKey=simparam.second->m_payload.m_attrName[nameIdx];
        const std::string objValue=simparam.second->m_payload.m_string[iObj];
        if (vbs) std::cout << "Simparam: " << objKey << " : " <<  objValue << std::endl;
        if (objKey=="TRTRangeCut") {
          std::string::size_type sz;     // alias of size_t
          m_fmd.m_trtRangeCut=std::stod(objValue,&sz);
        }
        if (objKey=="beamType") {
          m_fmd.m_beamType=objValue;
        }

      }
    }//end loop over simparam
    delete simparam.second;
    simparam.second=nullptr;
  }//end if have simparam

  if (digiparam.first) {
    if (vbs) std::cout << " Got IOVMetaDataContainer for /Digitization/Parameters " << std::endl;
    for (const auto& idx : digiparam.second->m_payload.m_attrIndexes) {
      const unsigned short nameIdx=idx.nameIndex();
      if (idx.typeIndex()==IOVPayloadContainer_p1::ATTR_STRING) {
        const unsigned short iObj=idx.objIndex();
        const std::string objKey=digiparam.second->m_payload.m_attrName[nameIdx];
        const std::string objValue=digiparam.second->m_payload.m_string[iObj];
        if (vbs) std::cout << "Digiparam: " << objKey << " : " <<  objValue << std::endl;
        if (objKey=="IOVDbGlobalTag") {
          m_fmd.m_condTag=objValue;
        }
        if (objKey=="DetDescrVersion") {
          m_fmd.m_geoTag=objValue;
        }
        if (objKey=="beamType") {
          m_fmd.m_beamType=objValue;
        }

      }
    }//end loop over digiparam
    delete digiparam.second;
    digiparam.second=nullptr;
  }//end if have digiparam

  if (taginfo.first) {
    if (vbs) std::cout << " Got IOVMetaDataContainer for TagInfo " << std::endl;
    for (const auto& idx : taginfo.second->m_payload.m_attrIndexes) {
      //const unsigned short typeIdx=idx.typeIndex();
      const unsigned short nameIdx=idx.nameIndex();
      if (idx.typeIndex()==IOVPayloadContainer_p1::ATTR_STRING) {
        const unsigned short iObj=idx.objIndex();
        const std::string objKey=taginfo.second->m_payload.m_attrName[nameIdx];
        const std::string objValue=taginfo.second->m_payload.m_string[iObj];
        if (vbs) std::cout << "Taginfo: " << objKey << " : " <<  objValue << std::endl;
        if (objKey=="IOVDbGlobalTag") {
          m_fmd.m_condTag=objValue;
        }
        if (objKey=="GeoAtlas") {
          m_fmd.m_geoTag=objValue;
        }
        if (objKey=="beam_energy") {
          m_fmd.m_beamEnergy=atoi(objValue.c_str());
        }
        if (objKey=="beam_type") {
          m_fmd.m_beamType=objValue;
        }

      }
    }//end loop over taginfo
    delete taginfo.second;
    taginfo.second=nullptr;
  }//end if have taginfo

  if (esi.first) {
    if (vbs) std::cout << " Got EventStreamInfo_p3" << std::endl;
    m_fmd.m_runNumbers.insert(esi.second->m_runNumbers.begin(),
                            esi.second->m_runNumbers.end());
    m_fmd.m_lbNumbers.insert(esi.second->m_lumiBlockNumbers.begin(),
                           esi.second->m_lumiBlockNumbers.end());



    const std::vector<std::pair<unsigned int, std::string> >& itemList=esi.second->m_itemList;
    for (const auto& kw : itemList) {
      m_fmd.m_sgKeys.push_back(kw.second);
    }

    delete esi.second;
    esi.second=nullptr;

  }
  else if (esi2.first) { //Have legacy version of EventStreamInfo
    m_fmd.m_valid=true;
    if (vbs) std::cout << " Got legacy EventStreamInfo_p2" << std::endl;
    m_fmd.m_runNumbers.insert(esi2.second->m_runNumbers.begin(),
                            esi2.second->m_runNumbers.end());
    m_fmd.m_lbNumbers.insert(esi.second->m_lumiBlockNumbers.begin(),
                           esi2.second->m_lumiBlockNumbers.end());
    delete esi2.second;
    esi2.second=nullptr;
  }


  std::set<unsigned> bmc_runNumbers;
#ifndef SIMULATIONBASE
  if (bmdc.first) {
    if (vbs) std::cout << " Got ByteStreamMetadataContainer" << std::endl;
    m_fmd.m_isMC=false; //The presence of a ByteStreamMetaDataContainer indicates real data
    for (const auto& bmd : *bmdc.second) {
      bmc_runNumbers.insert(bmd.m_runNumber);
      if (m_fmd.m_stream.size()==0) {
        m_fmd.m_stream=bmd.m_stream;
      }
      else if (m_fmd.m_stream!=bmd.m_stream) {
        std::cerr << "Conflicting stream names in file! Got " <<
          m_fmd.m_stream << " and " << bmd.m_stream << std::endl;
        m_fmd.m_valid=false;
      }
      if (m_fmd.m_project.size()==0) {
        m_fmd.m_project=bmd.m_project;
      }
      else if (m_fmd.m_project!=bmd.m_project) {
        std::cerr << "Conflicting project names in file! Got " <<
          m_fmd.m_project << " and " << bmd.m_project << std::endl;
        m_fmd.m_valid=false;
      }

      const std::vector<std::string>& freeMDs=bmd.m_freeMetaDataStrings;
      for (const std::string& freeMD : freeMDs) {
        if (freeMD.compare(0,11,"Event type:")==0 && freeMD.find("is sim")!=std::string::npos) {
          m_fmd.m_isMC=true; //This is made of a simulated bytestream file
          break;
        }
      }
    }
    delete bmdc.second;
    bmdc.second=nullptr;
  }//end if have ByteStream Meta Data
#endif // not SIMULATIONBASE

  //We may have gotten run-numbers twice (once form ByteStream MD and once form EventStreamInfo)
  //Cross-check:
  if (m_fmd.m_runNumbers.size()==0 && bmc_runNumbers.size()) {
    //Unlikly case, run-number only via ByteStream metadata
    m_fmd.m_runNumbers=std::move(bmc_runNumbers);
  }
  else if (m_fmd.m_runNumbers.size() && bmc_runNumbers.size()) {
    //Both filled - cross-check!
    if (m_fmd.m_runNumbers != bmc_runNumbers) {
      std::cerr << "Mismatching run numbers in file metadata!" << std::endl;
      std::cerr << "EventStreamInfo: ";
      std::copy(m_fmd.m_runNumbers.begin(),m_fmd.m_runNumbers.end(),std::ostream_iterator<unsigned>(std::cerr, " "));
      std::cerr << std::endl;
      std::cerr << "ByteStreamEventInfo: ";
      std::copy(bmc_runNumbers.begin(),bmc_runNumbers.end(),std::ostream_iterator<unsigned>(std::cerr, " "));
      std::cerr << std::endl;
      m_fmd.m_valid=false;
    }
  }

  //Get the GUID:
  TTree* params=(TTree*)tf->Get("##Params");
  const unsigned nParamsEntries=params->GetEntries();

  char dbStr[1024];
  params->SetBranchAddress("db_string",&dbStr);

  const std::string starter("[NAME=FID][VALUE=");
  for (unsigned i=0;i<nParamsEntries;++i) {
    params->GetEntry(i);
    //Model: [NAME=FID][VALUE=41DB894B-C39C-BF47-81E9-C52658B117BF]
    const std::string field(dbStr);

    if (field.compare(starter)) {
      m_fmd.m_guid=field.substr(starter.size(),field.size()-starter.size()-1);
      break;
    }
  }//end loop over params-entries

  //Get number of events for CollectionTree
  TTree* tt=(TTree*)tf->Get("CollectionTree");
  if (!tt) {
    std::cerr << "Failed to read collectionTree" << std::endl;
    m_fmd.m_valid=false;
  }
  else {
    m_fmd.m_nEvents=tt->GetEntries();
  }

  return;
}

int main ATLAS_NOT_THREAD_SAFE (int argc, char** argv) {

  bool verbose=false;
  bool kvDump=false;
  int c;

  while ((c = getopt (argc, argv, "vk")) != -1) {
    switch (c) {
    case 'v':
      verbose=true;
      break;
    case 'k':
      kvDump=true;
      break;
    default:
      std::cerr << "Unkown command line option" << std::endl;
    return -1;
    }
  }

  const int nfiles=argc-optind;
  if (nfiles<=0) {
    std::cerr << "Expected at least one file name as parameter" << std::endl;
    return -1;
  }


  std::vector<FileMetaData> output;

  for (int iFile=optind;iFile<argc;++iFile) {
    const char* filename=argv[iFile];
    if (verbose) std::cout << "Checking file " << filename << std::endl;

    PoolFilePeeker pfp(filename,verbose);

    output.push_back(pfp.get());
  }//end loop over input file names



 if (kvDump) {
   for (const auto& o : output) o.keyValueDump();
 }
 else {
   for (const auto& o : output) o.dump();
 }

 return 0;
}

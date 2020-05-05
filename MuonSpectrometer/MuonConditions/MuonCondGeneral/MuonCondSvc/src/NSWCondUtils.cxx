/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <iostream>

#include "MuonCondSvc/MdtStringUtils.h"
#include "MuonCondSvc/NSWCondUtils.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

namespace MuonCalib{
  void NSWCondUtils::setNSWABLinesFromAscii(const std::string& filename, ALineMapContainer& writeALines, BLineMapContainer& writeBLines, const sTgcIdHelper* stgcHelper, const MmIdHelper* mmHelper)
  {
    std::ifstream inputFile;
    inputFile.open(filename);
    int nLines = 1;
    int nNewALines = 0;
    int nNewBLines = 0;
    std::string line;
    std::string type;
    std::string since_str;
    std::string till_str;

    while(std::getline(inputFile,line)){

      ++nLines;

      std::vector<std::string> tokens;
      std::string delimiter = ":";
      MuonCalib::MdtStringUtils::tokenize(line, tokens, delimiter);

      type = tokens[0];

      if(type.find("#") == 0){
        continue;
      }

      if(type.find("Header") == 0){
        std::string delimiter = "|";
        std::vector<std::string> tokens;
        MuonCalib::MdtStringUtils::tokenize(line, tokens, delimiter);
        since_str = tokens[1];
        till_str = tokens[2];
      }

      if(type.find("IOV") == 0){
        std::string delimiter = " ";
        MuonCalib::MdtStringUtils::tokenize(line, tokens, delimiter);
        int ival = 1;
        long int iovThisBlob = 0; 
        std::string str_iovThisBlob = tokens[ival];
        sscanf(str_iovThisBlob.c_str(), "%80ld", &iovThisBlob);
      }

      if(type.find("Corr") == 0){
        std::string delimiter = " ";
        std::vector<std::string> tokens;
        MuonCalib::MdtStringUtils::tokenize(line, tokens, delimiter);

        if(tokens.size() != 25){
          MsgStream log(Athena::getMessageSvc(), "NSWCondUtils");
          log<<MSG::WARNING<<"Invalid length in string retrieved from the test file "<< filename << "String length is "<< tokens.size()<< endmsg;
          break;
        }

        int ival =1;
        int phi, eta, mult;
        std::string stationType = tokens[ival++];
        
        std::string phi_str = tokens[ival++];
        sscanf(phi_str.c_str(),"%80d",&phi);

        std::string eta_str = tokens[ival++];
        sscanf(eta_str.c_str(),"%80d",&eta);

        std::string mult_str = tokens[ival++];
        sscanf(mult_str.c_str(),"%80d",&mult);

        float s, z, t, rots, rotz, rott;

        std::string s_str = tokens[ival++];
        sscanf(s_str.c_str(),"%80f",&s);

        std::string z_str = tokens[ival++];
        sscanf(z_str.c_str(),"%80f",&z);

        std::string t_str = tokens[ival++];
        sscanf(t_str.c_str(),"%80f",&t);

        std::string rots_str = tokens[ival++];
        sscanf(rots_str.c_str(),"%80f",&rots);

        std::string rotz_str = tokens[ival++];
        sscanf(rotz_str.c_str(),"%80f",&rotz);

        std::string rott_str = tokens[ival++];
        sscanf(rott_str.c_str(),"%80f",&rott);

        float bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en;
        std::string tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&bz);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&bp);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&bn);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&sp);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&sn);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&tw);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&pg);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&tr);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&eg);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&ep);
        tmp_str = tokens[ival++];
        sscanf(tmp_str.c_str(),"%80f",&en);

        Identifier id, id_mult;
        if(stationType == "MML" || stationType == "MMS"){
          id = mmHelper->elementID(stationType, eta, phi);
          id_mult = mmHelper->multilayerID(id, mult);
        }

        else if(stationType == "STL" || stationType == "STS"){
          id = stgcHelper->elementID(stationType, eta, phi);
          id_mult = stgcHelper->multilayerID(id, mult);
        }

        ALinePar newALine;
        newALine.setAmdbId(stationType, eta, phi, mult);
        newALine.setParameters(s, z, t, rots, rotz, rott);
        newALine.isNew(true);
        writeALines.insert(std::make_pair(id_mult, newALine));
        ++nNewALines;

        BLinePar newBLine;
        newBLine.setAmdbId(stationType, eta, phi, mult);
        newBLine.setParameters(bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en);
        newBLine.isNew(true);
        writeBLines.insert(std::make_pair(id_mult, newBLine));
        ++nNewBLines;
      }
    }
  }
}

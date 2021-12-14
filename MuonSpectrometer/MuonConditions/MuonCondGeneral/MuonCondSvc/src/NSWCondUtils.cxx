/*
  Copyright (C) 2020, 2021 CERN for the benefit of the ATLAS collaboration
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
#include <charconv>
#include "boost/algorithm/string.hpp"
namespace MuonCalib{
  void NSWCondUtils::setNSWABLinesFromAscii(const std::string& filename, ALineMapContainer& writeALines, BLineMapContainer& writeBLines, const sTgcIdHelper* stgcHelper, const MmIdHelper* mmHelper)
  {
    using namespace MuonCalib;
    std::ifstream inputFile;
    inputFile.open(filename);
    int nLines = 1;
    int nNewALines = 0;
    int nNewBLines = 0;
    std::string line;
    std::string_view since_str;
    std::string_view till_str;

    while(std::getline(inputFile,line)){

      ++nLines;

      char delimiter = ':';
      const auto tokens = MdtStringUtils::tokenize(line, delimiter);
      if (!tokens.size()) continue; // skip empty lines

      const auto &type = tokens[0];

      if(type[0] == '#') continue;  // skip comments

      if(boost::algorithm::starts_with(type, "Header")){
        char delimiter = '|';
        auto tokens = MdtStringUtils::tokenize(line, delimiter);
        since_str = tokens[1];
        till_str = tokens[2];
      }

      if(boost::algorithm::starts_with(type, "IOV")){
        char delimiter = ' ';
        auto tokens = MdtStringUtils::tokenize(line, delimiter);
        int ival = 1;
        long int iovThisBlob = 0; 
        std::string_view str_iovThisBlob = tokens[ival];
        std::from_chars(str_iovThisBlob.data(), str_iovThisBlob.data() + str_iovThisBlob.size(), iovThisBlob);
      }

      if(boost::algorithm::starts_with(type, "Corr")){
        char delimiter = ' ';
        auto tokens = MdtStringUtils::tokenize(line, delimiter);

        if(tokens.size() != 25){
          MsgStream log(Athena::getMessageSvc(), "NSWCondUtils");
          log<<MSG::WARNING<<"Invalid length in string retrieved from the test file "<< filename << "String length is "<< tokens.size()<< endmsg;
          break;
        }

        int ival =1;
        int phi, eta, mult;
        std::string_view stationType = tokens[ival++];
        phi = MdtStringUtils::atoi(tokens[ival++]);
        eta = MdtStringUtils::atoi(tokens[ival++]);
        mult = MdtStringUtils::atoi(tokens[ival++]);

        float s, z, t, rots, rotz, rott;
        s = MdtStringUtils::stof(tokens[ival++]);
        z = MdtStringUtils::stof(tokens[ival++]);
        t = MdtStringUtils::stof(tokens[ival++]);
        rots = MdtStringUtils::stof(tokens[ival++]);
        rotz = MdtStringUtils::stof(tokens[ival++]);
        rott = MdtStringUtils::stof(tokens[ival++]);

        float bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en;
        bz = MdtStringUtils::stof(tokens[ival++]);
        bp = MdtStringUtils::stof(tokens[ival++]);
        bn = MdtStringUtils::stof(tokens[ival++]);
        sp = MdtStringUtils::stof(tokens[ival++]);
        sn = MdtStringUtils::stof(tokens[ival++]);
        tw = MdtStringUtils::stof(tokens[ival++]);
        pg = MdtStringUtils::stof(tokens[ival++]);
        tr = MdtStringUtils::stof(tokens[ival++]);
        eg = MdtStringUtils::stof(tokens[ival++]);
        ep = MdtStringUtils::stof(tokens[ival++]);
        en = MdtStringUtils::stof(tokens[ival++]);

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

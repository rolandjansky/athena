/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_MUONFULLIDHELPER_H
#define JIVEXML_MUONFULLIDHELPER_H

class Identifier;
class MdtIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class CscIdHelper;
class sTgcIdHelper;
class MmIdHelper;

#include <string>

namespace JiveXML{

  /**
   * @class MuonFullIDHelper
   * @brief Generate the appropiate full Identifier string for the differen Muon
   *        subsystem IDs.
   **/
  namespace MuonFullIDHelper{

    //Static convertor methods for each subsystem
    std::string getFullID(Identifier,const MdtIdHelper&);
    std::string getFullID(Identifier,const RpcIdHelper&);
    std::string getFullID(Identifier,const TgcIdHelper&);
    std::string getFullID(Identifier,const CscIdHelper&);
    std::string getFullID(Identifier,const sTgcIdHelper&);
    std::string getFullID(Identifier,const MmIdHelper&);

  }
}
#endif

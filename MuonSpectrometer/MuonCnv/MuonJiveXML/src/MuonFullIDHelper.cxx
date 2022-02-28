/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFullIDHelper.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "JiveXML/DataType.h"

namespace JiveXML{

  namespace MuonFullIDHelper {

    /**
     * Get the identifier string for an MDT ID with the sequence:
     *   Detector/StationName/StationEta/StationPhi/Technology/Multilayer/TubeLayer/Tube
     * @param id the identifier
     * @param mdtHelper the identifier helper
     **/
    std::string getFullID(Identifier id, const MdtIdHelper& mdtHelper) {
      
      std::string idString = "7/";
      idString  +=  mdtHelper.stationNameString(mdtHelper.stationName(id))+"/";
      idString  +=  DataType(mdtHelper.stationEta(id)).toString()+"/";
      idString  +=  DataType(mdtHelper.stationPhi(id)).toString()+"/";
      idString  +=  "MDT/";
      idString  +=  DataType(mdtHelper.multilayer(id)).toString()+"/";
      idString  +=  DataType(mdtHelper.tubeLayer(id)).toString()+"/";
      idString  +=  DataType(mdtHelper.tube(id)).toString();
      return  idString;
    }

    /**
     * Get the identifier string for an RPC ID with the sequence:
     *   Detector/StationName/StationEta/StationPhi/Technology/DoubletR/DoubletZ/DoubletPhi/GasGap/MeasuresPhi/Strip
     * @param id the identifier
     * @param rpcHelper the identifier helper
     **/
    std::string getFullID(Identifier id, const RpcIdHelper& rpcHelper) {
      
      std::string idString = "7/";
      idString +=  rpcHelper.stationNameString(rpcHelper.stationName(id))+"/";
      idString +=  DataType(rpcHelper.stationEta(id)).toString()+"/";
      idString +=  DataType(rpcHelper.stationPhi(id)).toString()+"/";
      idString +=  "RPC/";
      idString +=  DataType(rpcHelper.doubletR(id)).toString()+"/";
      idString +=  DataType(rpcHelper.doubletZ(id)).toString()+"/";
      idString +=  DataType(rpcHelper.doubletPhi(id)).toString()+"/";
      idString +=  DataType(rpcHelper.gasGap(id)).toString()+"/";
      idString +=  DataType(rpcHelper.measuresPhi(id)).toString()+"/";
      idString +=  DataType(rpcHelper.strip(id)).toString();
      return  idString;
    }


    /**
     * Get the identifier string for an TGC ID with the sequence:
     *   Detector/StationName/StationEta/StationPhi/Technology/GasGap/IsStrip/Channel
     * @param id the identifier
     * @param tgcHelper the identifier helper
     **/
    std::string getFullID(Identifier id, const TgcIdHelper& tgcHelper) {
      
      std::string idString =  "7/";
      idString +=  tgcHelper.stationNameString(tgcHelper.stationName(id))+"/";
      idString +=  DataType(tgcHelper.stationEta(id)).toString()+"/";
      idString +=  DataType(tgcHelper.stationPhi(id)).toString()+"/";
      idString +=  "TGC/";
      idString +=  DataType(tgcHelper.gasGap(id)).toString()+"/";
      idString +=  DataType(tgcHelper.isStrip(id)).toString()+"/";
      idString +=  DataType(tgcHelper.channel(id)).toString();
      return  idString;
    }

    /**
     * Get the identifier string for an CSC ID with the sequence:
     *   Detector/StationName/StationEta/StationPhi/Technology/ChamberLayer/WireLayer/MeasuresPhi/Strip 
     * @param id the identifier
     * @param cscHelper the identifier helper
     **/
    std::string getFullID(Identifier id, const CscIdHelper& cscHelper) {

      std::string idString =  "7/";
      idString +=  cscHelper.stationNameString(cscHelper.stationName(id))+"/";
      idString +=  DataType(cscHelper.stationEta(id)).toString()+"/";
      idString +=  DataType(cscHelper.stationPhi(id)).toString()+"/";
      idString +=  "CSC/";
      idString +=  DataType(cscHelper.chamberLayer(id)).toString()+"/";
      idString +=  DataType(cscHelper.wireLayer(id)).toString()+"/";
      idString +=  DataType(cscHelper.measuresPhi(id)).toString()+"/";
      idString +=  DataType(cscHelper.strip(id)).toString();
      return  idString;
    }

    /**
     * Get the identifier string for an STGC ID with the sequence:
     *   Detector/StationName/StationEta/StationPhi/Technology/Multilayer/GasGap/ChannelType/Channel
     * @param id the identifier
     * @param stgcHelper the identifier helper
     **/
    std::string getFullID(Identifier id, const sTgcIdHelper& stgcHelper) {

      std::string idString =  "7/";
      idString +=  stgcHelper.stationNameString(stgcHelper.stationName(id))+"/";
      idString +=  DataType(stgcHelper.stationEta(id)).toString()+"/";
      idString +=  DataType(stgcHelper.stationPhi(id)).toString()+"/";
      idString +=  "STGC/";
      idString +=  DataType(stgcHelper.multilayer(id)).toString()+"/";
      idString +=  DataType(stgcHelper.gasGap(id)).toString()+"/";
      idString +=  DataType(stgcHelper.channelType(id)).toString()+"/";
      idString +=  DataType(stgcHelper.channel(id)).toString();
      return  idString;
    }

    /**
     * Get the identifier string for an MM ID with the sequence:
     *   Detector/StationName/StationEta/StationPhi/Technology/Multilayer/GasGap/Channel
     * @param id the identifier
     * @param MmHelper the identifier helper
     **/
    std::string getFullID(Identifier id, const MmIdHelper& mmHelper) {

      std::string idString =  "7/";
      idString +=  mmHelper.stationNameString(mmHelper.stationName(id))+"/";
      idString +=  DataType(mmHelper.stationEta(id)).toString()+"/";
      idString +=  DataType(mmHelper.stationPhi(id)).toString()+"/";
      idString +=  "MM/";
      idString +=  DataType(mmHelper.multilayer(id)).toString()+"/";
      idString +=  DataType(mmHelper.gasGap(id)).toString()+"/";
      idString +=  DataType(mmHelper.channel(id)).toString();
      return  idString;
    }



  }
}


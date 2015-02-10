/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

std::ostream& MuonFixedId::dump(std::ostream& os) const {
  os << technology() << ": stnEta " << eta() << ", stnPhi " << phi() << " " 
     << technologyString() << ": " << stationNameString() ;
  if( is_mdt() ){
     os << ", mdtMultiLayer  " << mdtMultilayer() << ", mdtTubeLayer "
	<< mdtTubeLayer() << ", mdtTube " << mdtTube() ; 
  } else if( is_rpc() ) { 
    os << ", rpcDoubletR " << rpcDoubletR() << ", rpcDoubletZ "
       << rpcDoubletZ() << ", rpcDoubletPhi " << rpcDoubletPhi() << ", rpcGasGap " << rpcGasGap() << ", rpcMeasuresPhi "
       << rpcMeasuresPhi() << ", rpcStrip " << rpcStrip() ; 
  } else if( is_tgc() ) { 
    os << ", tgcGasGap " << tgcGasGap() << ", tgcIsStrip " 
       <<  tgcIsStrip() << ", tgcChannel " << tgcChannel() ; 
  } else if( is_csc() ) {
    os << ", cscChamberLayer " << cscChamberLayer() << ", cscWireLayer "
       << cscWireLayer()  << ", cscMeasuresPhi " << cscMeasuresPhi() << ", cscStrip " << cscStrip() ; 
  } else
    os << "Invalid MuonFixedId" ;
  
  return os;
}

const char MuonFixedId::kStationNameStrings[MuonFixedId::kNumberOfStationNames][4] = { 
     // 1      2      3      4      5      6      7      8      9     10     11     12     13
     "BIL", "BIS", "BML", "BMS", "BOL", "BOS", "BEE", "BIR", "BMF", "BOF", "BOG", "BME", "BIM",
     //14     15      16    17     18     19     20     21     22     23     24
     "EIC", "EIL", "EEL", "EES", "EMC", "EML", "EMS", "EOC", "EOL", "EOS", "EIS",
     //25     26     27     28     29     30     31     32     33     34     35 
     "T1F", "T1E", "T2F", "T2E", "T3F", "T3E", "T4F", "T4E", "CSS", "CSL", "BMG" 
};

const char MuonFixedId::kTechnologyStrings[MuonFixedId::kNumberOfTechnologies][4] = {
  "MDT", "CSC", "TGC", "RPC"
};

} // end of namespace MuonCalib

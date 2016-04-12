/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_PHIETAUTIL_H
#define SRC_PHIETAUTIL_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibStandAloneExtraTools/StringUtil.h"

class PhiEtaNameConverter {

 public:
  PhiEtaNameConverter() {};     //!< default constructor
  ~PhiEtaNameConverter() {};   //!< destructor

  inline std::string chamberHardwareName(int stationId, int phi, int eta){
    std::string chamberName = "UNDEFINED" ;
    MuonCalib::MuonFixedId id(stationId) ;
    ToString ts ;
    if (!id.is_mdt() ) return chamberName ;
    chamberName = id.stationNumberToFixedStationString(stationId);
    int absoluteEta = abs(eta) ;
    std::string side = "A";
    if(eta<0) side="C";
    int sector = phi_8to16(stationId, phi) ;
    std::string sectorString ;
    if(sector<10) sectorString = "0"+ts(sector);
    if(sector>=10) sectorString = ts(sector);
    chamberName += ts(absoluteEta)+side+sectorString ;
    return chamberName ;
  }

  inline std::string chamberAthenaName(int stationId, int phi, int eta){
    std::string chamberName = "UNDEFINED" ;
    MuonCalib::MuonFixedId id(stationId) ;
    ToString ts ;
    if (!id.is_mdt() ) return chamberName ;
    chamberName = id.stationNumberToFixedStationString(stationId);
    chamberName += "_"+ts(phi)+"_"+ts(eta) ;
    return chamberName ;
  }

  inline  int phi_8to16(int station, int phi8){
    // ENDCAP :
    if (station>13 && station<25) {
      if (station==15 || station==16 ||station==19 ||station==22) { 
	return phi8*2-1 ;
      } else {
	return phi8*2 ;
      }
    }
    
    //large BIL,BML,BOL
    if(station==1||station==3||station==5
       || station==8   //BIR
       || station==13  //BIM
       || station==12  //BME
       ){
      return phi8*2-1;
    }
    //small. BIS,BMS,BOS
    if(station==2||station==4||station==6
      || station==7  //BEE
      || station==9  //BMF
      || station==10 //BOF
      || station==11 //BOG
      || station==35 //BMG
       ){
      return phi8*2;  
    }   
    
    if( station%2 == 0 ){
      return phi8*2;
    }
    return phi8*2-1;
  }  //end phi_8to16

  inline  int phi_16to8(int station, int phi16){
    if(station==1||station==3||station==5
       || station==8   //BIR
       || station==13  //BIM
       || station==12  //BME
       ){
      return (phi16+1)/2;
    }
    if(station==2||station==4||station==6
      || station==7  //BEE
      || station==9  //BMF
      || station==10 //BOF   
      || station==11 //BOG   
      || station==35 //BMG
        ){
      return phi16/2;  
    }   
    
    if(phi16%2 == 0 ){
      return phi16/2;
    }

    return (phi16+1)/2;

  }  //end phi_16to8

// CHE E' STA ROBBBA ????
//
inline  int eta_atlas2loc(int eta){
  int eta2= eta+7;
  if(eta<0) eta2=eta+8;
  //cout<<" atlas2loc "<<eta<<" "<<eta2<<endl;
  return eta2;
}

inline  int eta_loc2atlas(int eta){
  int eta2;
  if(eta<=7) eta2=eta-8;
  if(eta>=8) eta2=eta-7;
  //cout<<" loc2atlas "<<eta<<" "<<eta2<<endl;
  return eta2;
}

};

#endif // SRC_PHIETAUTIL_H

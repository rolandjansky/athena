/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCTriggerCondSvc/TGCTriggerBWCWReader.h"

#include "TGCTriggerCondSvc/TGCTriggerLUTs.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

TGCTriggerBWCWReader::TGCTriggerBWCWReader(int lutType):
  TGCTriggerLUTReader(lutType)
{
}



bool TGCTriggerBWCWReader::loadParameters(TGCTriggerLUTs* writeCdo,
                                          const CondAttrListCollection* readCdo) {

  MsgStream log(m_msgSvc, "TGCTriggerBWCWReader::loadParameters");
  if(m_svcLocator->service("MessageSvc", m_msgSvc) == StatusCode::FAILURE){
    return false;
  }

  CondAttrListCollection::const_iterator itr = readCdo->begin();
  CondAttrListCollection::const_iterator itr_e = readCdo->end();

  for(; itr!=itr_e; ++itr) {
    const unsigned int channel = (*itr).first;
    const coral::AttributeList& atr = (*itr).second;

    log << MSG::DEBUG<<"channel: " << channel<<endmsg;
    std::string file    = *(static_cast<const std::string*>((atr["file"]).addressOfData()));
    std::string data    = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
    std::string version = *(static_cast<const std::string*>((atr["version"]).addressOfData()));
    std::string type    = *(static_cast<const std::string*>((atr["type"]).addressOfData()));

    writeCdo->m_datamap[m_lutType][file] = data;
    writeCdo->m_data[m_lutType].push_back(data);
    writeCdo->m_file[m_lutType].push_back(file);
    writeCdo->m_version[m_lutType].push_back(version);
    writeCdo->m_type[m_lutType].push_back(type);

    log << MSG::INFO<<"file: " <<file<<endmsg;
    log << MSG::DEBUG<<"data: " << data<<endmsg;

  }
  return true;
}








bool TGCTriggerBWCWReader::readLUT(TGCTriggerLUTs* writeCdo){

  MsgStream log(m_msgSvc, "TGCTriggerBWCWReader::readLUT");
  if(m_svcLocator->service("MessageSvc", m_msgSvc) == StatusCode::FAILURE){
    return false;
  }

  const int numberOfCoincidenceType = 4; 

  const int moduleNumber[TGCTriggerLUTs::N_MODULETYPE]  =
    {  0,  1,   2,   2,  3,  4,   5,   5,  6,  7,   8,  8 };
  const std::string moduleName[TGCTriggerLUTs::N_MODULETYPE]=
    {"0a","1a","2a","2b","3a","4a","5a","5b","6a","7a","8a","8b"};
  const std::string sideName[TGCTriggerLUTs::N_SIDE] = {"a","c"};
  const std::string octantName[TGCTriggerLUTs::N_OCTANT] =
    {  "0", "1", "2", "3", "4", "5", "6", "7"};
  const std::string coincidenceTypeName[numberOfCoincidenceType] = {"HH","HL","LH","LL"};


  // initialize
  bool fullCW = (writeCdo->getType(TGCTriggerLUTs::CW_BW) == "full" );
  // loop over all files...
  for(int iSide = 0; iSide<TGCTriggerLUTs::N_SIDE; iSide++){
    for(int iOctant = 0; iOctant < TGCTriggerLUTs::N_OCTANT; iOctant++) {
      for(int iModule=0; iModule<TGCTriggerLUTs::N_MODULETYPE; iModule+=1) {
        for(int iCoinType=0; iCoinType!=numberOfCoincidenceType; iCoinType++){

          int phimod2=moduleName[iModule].find("b")!=std::string::npos ? 1 : 0;
          std::string fn;
          if (fullCW) {
            fn = "RPhiCoincidenceMapRun3_"
              +sideName[iSide]
              +octantName[iOctant]
              +moduleName[iModule]
              +coincidenceTypeName[iCoinType]+".db";
          }
          else{
            fn = "RPhiCoincidenceMapRun3_"
              +moduleName[iModule]
              +coincidenceTypeName[iCoinType]+".db";
          }

          bool Forward_type1=(moduleName[iModule]=="2b"||moduleName[iModule]=="5a"||moduleName[iModule]=="8b");
          bool Forward_type2=(moduleName[iModule]=="2a"||moduleName[iModule]=="5b"||moduleName[iModule]=="8a");

          if(iOctant%2==0 && Forward_type1){continue;}
          if(iOctant%2==1 && Forward_type2){continue;}
          int type = -1;
          int lDR, hDR, lDPhi, hDPhi;

          std::string data = writeCdo->getData(TGCTriggerLUTs::CW_BW, fn);
          std::istringstream stream(data);

          std::string buf,tag;
          char delimiter = '\n';
          int ssId;

          while(getline(stream,buf,delimiter)){
            std::istringstream header(buf);
            header>>tag;
            if(tag=="#") { // read header part.     
              header>>ssId>>lDR>>hDR>>lDPhi>>hDPhi;
              type = getType( lDR, hDR, lDPhi, hDPhi );
              if( type<0 ) {
                break;
              }

              // get window data
              std::map<int, std::map<int, char> >  bWindow;//<R,<~>>
              char pT;
              for(int ir=0; ir<=hDR-lDR; ir++) {
                getline(stream,buf,delimiter);
                std::map<int, char> aWindow;//<Phi,pT>
                for(int iphi=0; iphi<=hDPhi-lDPhi; iphi++){
                  pT = buf[iphi];
                  if (pT=='X'){continue;} // none of window is opened in this dR
                  aWindow[iphi+TGCTriggerLUTs::DPhi_offset] = pT;
                }
                // Warning : no window 
                if (aWindow.size()!=0){
                  bWindow[ir+TGCTriggerLUTs::DR_offset]=aWindow;
                }
              }

              int addr = subSectorAdd(ssId,moduleNumber[iModule],phimod2,type);
              if (writeCdo->m_mapDB_bw[iSide][iOctant].find(addr)!=writeCdo->m_mapDB_bw[iSide][iOctant].end()) {
               log << MSG::DEBUG<<"This subsector was already reserved."<<endmsg;
              } else {
                writeCdo->m_mapDB_bw[iSide][iOctant][addr]=bWindow;
              }
            }
          }


        }// coincidence type
      }// module
    }// octant
  }// side


  return true;

}






int TGCTriggerBWCWReader::getType(int lDR, int hDR, int lDPhi, int hDPhi ) const {
  int type = -1;
  if ( (lDR==-15) && (hDR==15) && (lDPhi==-7) && (hDPhi==7))      type = TGCTriggerBWCWReader::TMap_HH;
  else if ( (lDR==-15) && (hDR==15) && (lDPhi==-3) && (hDPhi==3)) type = TGCTriggerBWCWReader::TMap_HL;
  else if ( (lDR==-7) && (hDR==7) && (lDPhi==-7) && (hDPhi==7))   type = TGCTriggerBWCWReader::TMap_LH;
  else if ( (lDR==-7) && (hDR==7) && (lDPhi==-3) && (hDPhi==3))   type = TGCTriggerBWCWReader::TMap_LL;
  return type;
}

int TGCTriggerBWCWReader::subSectorAdd(int ssid, int modid, int phimod2, int type) const { 
  return (ssid+(modid<<8)+(phimod2<<12) + (type<<16) ); 
}


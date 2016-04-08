/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"
#include "CaloCondBlobObjs/Exception.h"
#include <iomanip>

//
//___________________________________________________________
//std::map<CaloCondUtils::SYSTEM,unsigned int> CaloCondUtils::m_nCells;

//
//___________________________________________________________
unsigned int
CaloCondUtils::getCoolChannelId(const SYSTEM sys)
{
  return static_cast<unsigned int>(sys);
}

//
//___________________________________________________________
unsigned int
CaloCondUtils::getNCells(const SYSTEM sys)
{
  /*
  //=== initialize on first usage
  if(!m_nCells.size()){
    m_nCells[EMECZNEG] = 31872;
    m_nCells[EMBZNEG ] = 54784;
    m_nCells[EMBZPOS ] = 54784;
    m_nCells[EMECZPOS] = 31872;
    m_nCells[HEC     ] =  5632;
    m_nCells[FCAL    ] =  3524;
    m_nCells[TILE    ] =  5184;
  }
  */

  switch (sys) {
  case EMBZNEG:
    return 54784;
    break;
  case EMBZPOS:
    return 54784;
    break;
  case EMECZNEG: 
    return 31872;
    break;
  case EMECZPOS: 
    return 31872;
    break;
  case HEC:
    return 5632;
    break;
  case TILE:
    return 5184;
    break;
  case FCAL:
    return 3524;
    break;
  }

  return 0;
}

//
//_____________________________________________________________________________
std::string 
CaloCondUtils::getFullTag(const std::string& folder, const std::string& tag)
{
  std::string folderTag("");
  //Copy folder name, ommiting slashes
  for (const char i : folder) {
    if (i!='/') {
      folderTag+=i;
    }
  }
  return (folderTag+"-"+tag);
}

//
//_____________________________________________________________________________
unsigned int 
CaloCondUtils::getDbCaloGain(int caloGain)
  //   enum CaloGain {
  //     TILELOWLOW =-16 ,
  //     TILELOWHIGH =-15 ,
  //     TILEHIGHLOW  = -12,
  //     TILEHIGHHIGH = -11,
  //     TILEONELOW   =-4,
  //     TILEONEHIGH  =-3,
  //     INVALIDGAIN = -1, LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,
  //     LARNGAIN =3,
  //     UNKNOWNGAIN=4};
{

  //=== do nothing for LAr gains
  if(caloGain>=0 && caloGain<=2) return static_cast<unsigned int>(caloGain);
  
  //=== translate tile gains
  if(caloGain==-16) return 0; //LOWLOW
  if(caloGain==-15) return 1; //LOWHIGH
  if(caloGain==-12) return 2; //HIGHLOW
  if(caloGain==-11) return 3; //HIGHHIGH
  if(caloGain== -4) return 0; //ONELOW
  if(caloGain== -3) return 3; //ONEHIGH

  //=== invalid gain
  {
    MsgStream log(Athena::getMessageSvc(), "CaloCondUtils::getDbCaloGain");
    log << MSG::WARNING << "invalid gain found... probably corrupted data /badly decoded ?   returns DbCaloGain 0" << endreq; 
    return 0;
  }
  //throw CaloCond::InvalidCaloGain("CaloCondUtils::getDbCaloGain()", caloGain);
}




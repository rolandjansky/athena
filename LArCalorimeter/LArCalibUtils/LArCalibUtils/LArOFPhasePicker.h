/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBUTILS_LArOFPhasePicker_H
#define LARCALIBUTILS_LArOFPhasePicker_H

/********************************************************************

NAME:     LArOFPhasePicker.h 
AUTHORS:  P. Strizenec
CREATED:  Mar. 2008

Used to pick-up a one OFC phase to fill online Db
Use a phases stored in CondDB (using a Tshaper raw condition) or a default 0.
********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"

class LArOFCBinComplete;
class LArOnlineID;

class LArOFPhasePicker : public AthAlgorithm
{

 public:

  LArOFPhasePicker(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArOFPhasePicker();

  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:

  const LArOFCBinComplete* m_inputPhase;
  const LArOnlineID*       m_onlineID; 

  //Job properties
  std::string m_keyOFC;
  std::string m_keyOFC_new;
  std::string m_keyShape;
  std::string m_keyShape_new;
  std::string m_keyPhase;
  std::string m_groupingName;

  int m_groupingType;
  int m_timeOffsetCorr;
  int m_defaultPhase;
  bool m_doShape, m_doOFC;

  //private methods:
  StatusCode pickOFC();
  StatusCode pickShape();



};
#endif

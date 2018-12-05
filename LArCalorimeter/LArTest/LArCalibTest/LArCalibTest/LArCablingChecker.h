/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCABLINGCHECKER
#define LARCABLINGCHECKER
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArDigitContainer.h"
//#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"

#include <fstream>
#include <vector>

class LArCablingChecker : public AthAlgorithm
{
 public:
  LArCablingChecker(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArCablingChecker();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  int m_count;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey{this, "BadChanKey", "LArBadChannel", "SG bad channels key"};
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "CalibLineKey", "LArCalibLineMap", "SG calib line key"};

  const LArOnlineID* m_onlineHelper;
  const LArEM_ID* m_emId;

  std::ofstream m_outfile;
  std::string m_key;
  std::string m_outFileName;
  //std::vector<unsigned> m_vPattern;

  std::vector<bool> *m_errorcellsThisEvent;
  int m_errorCounterThisEvent;
  std::vector<bool> *m_errorcellsPreviousEvent;
  int m_errorCounterPreviousEvent;
  int m_numberOfEventsWithThisErrorState;
  LArOnlineID::size_type m_channelHashMax;

  // Properties
  bool m_PrintAllCellsAndEvents;
  bool m_PrintDisconnectedCells;
  int m_ADCThreshold;
  int m_DACHighGainThreshold;
  int m_DACMediumGainThreshold;
  int m_DACLowGainThreshold;
  bool m_printEventSummary;
  bool m_useBadChannelTool;
};

#endif

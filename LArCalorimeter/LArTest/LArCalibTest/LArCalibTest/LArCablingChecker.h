/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCABLINGCHECKER
#define LARCABLINGCHECKER
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArCabling/LArCablingService.h"
//#include "CaloIdentifier/LArEM_ID.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArIdentifier/LArOnlineID.h"

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
  LArCablingService *m_larCablingSvc;
  ILArBadChanTool *m_caloBadChannelTool;

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

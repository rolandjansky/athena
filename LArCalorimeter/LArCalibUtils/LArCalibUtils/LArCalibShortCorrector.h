/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCALIBSHORTCORRECTOR_H
#define LARCALIBSHORTCORRECTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArTools/LArCablingService.h"
#include "LArElecCalib/ILArPedestal.h"

#include <vector>

class LArAccumulatedCalibDigit;
class ILArBadChanTool;
class CaloCell_ID;
class LArOnlineID;
class LArCablingService;
class HWIdentifier;


class LArCalibShortCorrector : public AthAlgorithm//, public IIncidentListener 
{
 public:
  LArCalibShortCorrector(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCalibShortCorrector();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}


 private:

  StatusCode findShortedNeighbors();

  ToolHandle<LArCablingService> m_larCablingSvc;
  ToolHandle<ILArBadChanTool> m_badChannelTool;

  const LArOnlineID*  m_onlineId;
  const CaloCell_ID*  m_caloCellId;
  
  const DataHandle<ILArPedestal> m_larPedestal;

  //Algorithm-Properties:
  std::vector<std::string> m_keylist;
  std::string m_pedKey;

  //private members
  std::vector<std::pair<HWIdentifier,HWIdentifier> > m_shortedNeighbors;
  typedef std::vector<std::pair<HWIdentifier,HWIdentifier> >::const_iterator SHORT_IT;
 
  bool m_shortsCached;
};

#endif

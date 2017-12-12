/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBDIGITMAKER
#define LARCALIBDIGITMAKER
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "GaudiKernel/ToolHandle.h"	// Modif J. Labbe from JF Marchand - Nov. 2009
#include "LArCabling/LArCablingService.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "CaloIdentifier/LArEM_ID.h"
#include <fstream>

class LArCalibDigitMaker : public AthAlgorithm
{
 public:
  LArCalibDigitMaker(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArCalibDigitMaker();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:
  ToolHandle<LArCablingService> m_larCablingSvc; //   LArCablingService *m_larCablingSvc; 	// Modif J. Labbe from JF Marchand - Nov. 2009
  ToolHandle<LArSuperCellCablingTool> m_larCablingSvc_SC; 
  //const LArOnlineID* m_onlineHelper;
  //JobOpts:
  std::vector<std::string> m_keylist;
  //std::string m_key;
  std::vector<unsigned> m_vPattern;
  std::vector<unsigned> m_vDAC;
  std::vector<unsigned> m_vDelay;
  std::vector<unsigned> m_vBoardIDs;
  unsigned m_nTrigger;
  double m_delayScale;
  bool m_dontRun;
};

#endif

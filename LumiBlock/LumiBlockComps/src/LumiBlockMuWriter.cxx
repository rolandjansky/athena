/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockComps/LumiBlockMuWriter.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"

//--------------------------------------------------
LumiBlockMuWriter::LumiBlockMuWriter(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_lumiTool("LuminosityTool"),
  m_writePileupEvents(false),
  m_lumiDatabase("COOLOFL_TRIGGER/COMP200"),
  m_lumiFolder("/TRIGGER/OFLLUMI/LBLESTOFL"),
  m_lumiTag("OflLumi-UPD2-005"),
  m_lumiChannel(0),
  m_lumiDB(NULL)
{
  declareProperty("LuminosityTool", m_lumiTool);
  declareProperty("WritePileupEvents", m_writePileupEvents);
  declareProperty("PileupLuminosityDatabase", m_lumiDatabase);
  declareProperty("PileupLuminosityFolder", m_lumiFolder);
  declareProperty("PileupLuminosityTag", m_lumiTag);
  declareProperty("PileupLuminosityChannel", m_lumiChannel);
}

StatusCode
LumiBlockMuWriter::initialize() 
{
  ATH_MSG_INFO("LumiBlockMuWriter::initialize()");

  // Get the luminosity tool
  CHECK(m_lumiTool.retrieve());

  ATH_MSG_INFO("LumiBlockMuWriter::initialize() done");

  return StatusCode::SUCCESS;
}

StatusCode
LumiBlockMuWriter::execute() 
{
  ATH_MSG_DEBUG("LumiBlockMuWriter::execute()");

  const EventInfo* eventInfo_c=0;
  CHECK(evtStore()->retrieve(eventInfo_c));

  unsigned int lumiblock = eventInfo_c->event_ID()->lumi_block();
  ATH_MSG_DEBUG(" lumiblock " << lumiblock);

  // Try to write mu back into EventInfo
  EventInfo* eventInfo=0;
  eventInfo = const_cast<EventInfo*>(eventInfo_c);

  if (eventInfo) {

    // float avgmu = m_muToolDB->averageInteractionsPerCrossing();
    // float instmu = m_muToolDB->actualInteractionsPerCrossing();

    float avgmu = m_lumiTool->lbAverageInteractionsPerCrossing();
    float instmu = 0.;

    if (m_lumiTool->muToLumi() > 0.)
      instmu = m_lumiTool->lbLuminosityPerBCID()/m_lumiTool->muToLumi(); 

    eventInfo->setAverageInteractionsPerCrossing(avgmu);
    eventInfo->setActualInteractionsPerCrossing(instmu);

    ATH_MSG_DEBUG(" called eventInfo::setAverageInteractionsPerCrossing("<<avgmu<<")");
    ATH_MSG_DEBUG(" called eventInfo::setActualInteractionsPerCrossing("<<instmu<<")");

    // Some debugging
    // float lbTime = m_lumiTool->lbDuration();
    // ATH_MSG_DEBUG(" lbtime: " << lbTime);

  } else {
    ATH_MSG_WARNING(" cannot get non-const eventInfo pointer! ");
  }

  if (m_writePileupEvents) {

    // Also try to write pile-up event info if it exists
    const PileUpEventInfo* puEventInfo_c = 0;
    CHECK(evtStore()->retrieve(puEventInfo_c));

    PileUpEventInfo::SubEvent::const_iterator s = puEventInfo_c->beginSubEvt();
    while (s != puEventInfo_c->endSubEvt()) {
      if (s->type()==4){  // Zero bias events, only ones with 'luminosity'
	
	unsigned int run = s->pSubEvt->event_ID()->run_number();
	unsigned int lb = s->pSubEvt->event_ID()->lumi_block();
	ATH_MSG_INFO("ACH9 sub event of type 4 with run/lb:" << run << "/" << lb);
	
	CoolQuery::LumiFolderData lumi = this->getDBLuminosity(run, lb);
	
	// And write this into the pileup event header
	s->pSubEvt->setAverageInteractionsPerCrossing(lumi.LBAvEvtsPerBX);
	
      }
      ++s;
    }
  }

  ATH_MSG_DEBUG("LumiBlockMuWriter::execute() done");
  return StatusCode::SUCCESS;
}

// Hack to access offline luminosity folder for arbitrary run/LB IOV
CoolQuery::LumiFolderData 
LumiBlockMuWriter::getDBLuminosity(unsigned int run, unsigned int lb) {

  CoolQuery::LumiFolderData lumi;

  // First check if we have a DB connection established
  if (m_lumiDB == NULL) {
    m_lumiDB = new CoolQuery(m_lumiDatabase, std::string());
    if (!m_lumiDB->openDbConn()) {
      ATH_MSG_WARNING(" failed to open luminosity DB folder " << m_lumiDatabase);
      delete m_lumiDB;
      return lumi;
    }
  }

  // Specify the IOV
  cool::ValidityKey start = run;
  start = (start<<32)+lb;
  cool::ValidityKey end = run;
  end = (end<<32) + lb + 1;

  m_lumiDB->setIOV(start, end);

  // And read the data
  std::map<cool::ValidityKey, CoolQuery::LumiFolderData> lumiDataMap;
  lumiDataMap.clear();
  lumiDataMap = m_lumiDB->getLumiFolderData(m_lumiFolder, m_lumiTag, m_lumiChannel);
  if (lumiDataMap.find(start) == lumiDataMap.end()) {
    // Didn't find our IOV
    ATH_MSG_WARNING(" failed to find luminosity for run/LB: " << run << "/" << lb);
  }

  return lumiDataMap[start];  // Will create default LumiFolderData if not found
}

StatusCode
LumiBlockMuWriter::finalize() 
{
  ATH_MSG_DEBUG("LumiBlockMuWriter::finalize()");

  if (m_lumiDB != NULL) delete m_lumiDB;

  ATH_MSG_INFO("LumiBlockMuWriter::finalize() done");

  return StatusCode::SUCCESS;
}


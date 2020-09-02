/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Trigger
#include "EnhancedBiasWeighter/ReadLumiBlock.h"

// COOL
#include "CoolApplication/Application.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoralKernel/Context.h"

// C/C++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

//-----------------------------------------------------------------------------
ReadLumiBlock::ReadLumiBlock()
  :m_cool_id(),
   m_cool_id_run1("COOLONL_TRIGGER/COMP200"),
   m_cool_id_run2("COOLONL_TRIGGER/CONDBR2"),
   m_cool_source(),
   m_run(0),
   m_triedSetup(false)
{
}

//-----------------------------------------------------------------------------
ReadLumiBlock::~ReadLumiBlock()
{
}

//-----------------------------------------------------------------------------
bool ReadLumiBlock::updateLumiBlocks(const uint32_t run, MsgStream& msg)
{
  //
  // Read configuration keys from COOL
  //
  m_triedSetup = true;
  m_cool_source.clear();
  m_run = run;

  if (m_run < 238742) m_cool_id = m_cool_id_run1;
  else                m_cool_id = m_cool_id_run2;

  cool::ValidityKey since(run);
  cool::ValidityKey until(run);
  cool::ValidityKey maskLB(0xffffffff);
  
  since <<= 32; // upper 31 bits are run number
  until <<= 32; // upper 31 bits are run number 
  until  += maskLB;

  if (!dbIsOpen()) { 
    if (!openDb(true, msg)) {
      msg << MSG::FATAL << "updateLumiBlocks - failed to open DB... " << m_cool_id << endmsg;    
      return false;
    }
  }

  if (m_cool_ptr->existsFolder("/TRIGGER/LUMI/LBLB")) {

    msg << MSG::INFO << "ReadRunData - reading folder: /TRIGGER/LUMI/LBLB" << endmsg;

    cool::IFolderPtr folderLB = m_cool_ptr->getFolder("/TRIGGER/LUMI/LBLB");
    cool::IObjectIteratorPtr objectsLB = folderLB->browseObjects(since, until, 0);

    while ( objectsLB -> goToNext() ) {
      const cool::IObject &obj = objectsLB->currentRef();
      const cool::IRecord &payload = obj.payload();
      const uint32_t lumi = (obj.since()  & 0xffff);

      const uint64_t start = payload["StartTime"].data<cool::UInt63>();
      const uint64_t end = payload["EndTime"].data<cool::UInt63>();
      const uint64_t length = end - start;

      m_lbLength[lumi] = length;

  	  msg << MSG::DEBUG << "  run=" << run
  	                    << "   LB=" << lumi
  		                  << "   Start=" << start
  		                  << "   End=" << end
  		                  << "   Length=" << length << endmsg;
    }
  } else { 
    msg << MSG::INFO << "ReadRunData - missing COOL folder: /TRIGGER/LUMI/LBLB in " << m_cool_id << endmsg; 
  }
  // more lumi info can be got from http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Trigger/TrigCost/TrigCostRate/src/ReadCool.cxx

  closeDb(msg);
  return true;
}

float ReadLumiBlock::getLumiBlockLength(const uint32_t lb, MsgStream& msg) const
{

	if (m_lbLength.count(lb) != 1) {
 		msg << MSG::INFO << "Unknown lumiblock number " << lb << endmsg;
 		return 60.0; // Sensible default
	}
	return m_lbLength.at(lb) * 1e-9; // Convert from ns
}

//---------------------------------------------------------------------------------------
cool::IDatabaseSvc& ReadLumiBlock::databaseService()
{
  static cool::Application app;
  return app.databaseService();
}

//---------------------------------------------------------------------------------------
bool ReadLumiBlock::dbIsOpen()
{ 
  return m_cool_ptr.use_count() > 0 && m_cool_ptr->isOpen();
}

//---------------------------------------------------------------------------------------
bool ReadLumiBlock::openDb(bool readOnly, MsgStream& msg)
{
  if(dbIsOpen()) return true;
  
  cool::IDatabaseSvc& dbSvc = databaseService();
  try {
    msg << MSG::DEBUG << "ReadLumiBlock::openDb - opening database '" << m_cool_id << "'" << endmsg;
    m_cool_ptr = dbSvc.openDatabase(m_cool_id, readOnly);
    
    return true;
  } catch(cool::DatabaseDoesNotExist& e) {
    msg << MSG::ERROR << "ReadLumiBlock::openDb - COOL exception caught: " << e.what() << endmsg;
	  msg << MSG::ERROR << "   could not open database: " << m_cool_id << endmsg;
    return false;
  }

  return false;
}

//---------------------------------------------------------------------------------------
void ReadLumiBlock::closeDb(MsgStream& msg)
{
  try {
    if(dbIsOpen()) {
      msg << MSG::DEBUG << "ReadLumiBlock::closeDd - closing database '" << m_cool_id << endmsg;
      m_cool_ptr->closeDatabase();
    }
  } catch(std::exception& e) {
    msg << MSG::ERROR << "ReadLumiBlock::closeDB - COOL exception caught: " << e.what() << endmsg;
	  msg << MSG::ERROR << "   could not close COOL database: " << m_cool_id << endmsg;
  }
  return;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

// COOL
#include "CoolApplication/Application.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoralKernel/Context.h"

// Trigger
#include "TrigConfStorage/TrigConfCoolFolderSpec.h"
#include "TrigCostMonitor/ReadLumiBlock.h"

using namespace std;

//-----------------------------------------------------------------------------
Trig::ReadLumiBlock::ReadLumiBlock()
  :m_cool_id(),
   m_cool_id_run1("COOLONL_TRIGGER/COMP200"),
   m_cool_id_run2("COOLONL_TRIGGER/CONDBR2"),
   m_cool_source(),
   m_run(0),
   m_triedSetup(false)
{
}

//-----------------------------------------------------------------------------
Trig::ReadLumiBlock::~ReadLumiBlock()
{
}

//-----------------------------------------------------------------------------
void Trig::ReadLumiBlock::updateLumiBlocks(const unsigned run)
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

  if(!dbIsOpen()) { 
    if(!openDb(true)) {
      m_infos << "updateLumiBlocks - failed to open DB... " << m_cool_id << " nothing to do" << endl;    
      return;
    }
  }

  if(m_cool_ptr->existsFolder("/TRIGGER/LUMI/LBLB")) {

    m_infos << "ReadRunData - reading folder: /TRIGGER/LUMI/LBLB" << endl;

    cool::IFolderPtr folderLB = m_cool_ptr->getFolder("/TRIGGER/LUMI/LBLB");
    cool::IObjectIteratorPtr objectsLB = folderLB->browseObjects(since, until, 0);

    while ( objectsLB -> goToNext() ) {
      const cool::IObject &obj = objectsLB->currentRef();
      const cool::IRecord &payload = obj.payload();
      const unsigned lumi = (obj.since()  & 0xffff);

      const uint64_t start = payload["StartTime"].data<cool::UInt63>();
      const uint64_t end = payload["EndTime"].data<cool::UInt63>();
      const uint64_t length = end - start;

      m_lbLength[lumi] = length;

  	  m_debug << "  run=" << run
  	          << "   LB=" << lumi
  		        << "   Start=" << start
  		        << "   End=" << end
  		        << "   Length=" << length << endl;
    }
  } else { 
    m_infos << "ReadRunData - missing COOL folder: /TRIGGER/LUMI/LBLB in " << m_cool_id << endl; 
  }
// more lumi info can be got from http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Trigger/TrigCost/TrigCostRate/src/ReadCool.cxx


  closeDb();
}

float Trig::ReadLumiBlock::getLumiBlockLength(unsigned lb) {

	if (m_lbLength.count(lb) != 1) {
 		m_infos << "Unknown lumiblock number " << lb << endl;
 		return 0; 
	}
	return m_lbLength[lb] * 1e-9; // Convert from ns
}

//---------------------------------------------------------------------------------------
void Trig::ReadLumiBlock::print(ostream &os) const
{
  //
  // Print unique configuration keys
  //  
  std::map<unsigned, uint64_t>::const_iterator it = m_lbLength.begin();

  for(; it != m_lbLength.end(); ++it) { 
    os << "LB:" << it->first << " Length:" << it->second << endl;
  }
}

//---------------------------------------------------------------------------------------
cool::IDatabaseSvc& Trig::ReadLumiBlock::databaseService()
{
  static cool::Application app;
  return app.databaseService();
}

//---------------------------------------------------------------------------------------
bool Trig::ReadLumiBlock::dbIsOpen()
{ 
  return m_cool_ptr.use_count()>0 && m_cool_ptr->isOpen();
}

//---------------------------------------------------------------------------------------
bool Trig::ReadLumiBlock::openDb( bool readOnly )
{
  if(dbIsOpen()) return true;
  
  cool::IDatabaseSvc& dbSvc = databaseService();
  try {
    m_debug << "ReadLumiBlock::openDb - opening database '" << m_cool_id << "'" << endl;
    m_cool_ptr = dbSvc.openDatabase(m_cool_id, readOnly);
    
    const int schemaVersion = TrigConf::TrigConfCoolFolderSpec::readSchemaVersion(m_cool_ptr);
    m_debug << "ReadLumiBlock::openDb - using schema version: " << schemaVersion << endl;

    return true;
  } catch(cool::DatabaseDoesNotExist& e) {
    m_infos << "ReadLumiBlock::openDb - COOL exception caught: " << e.what() << endl
	          << "   could not open database: " << m_cool_id << endl;
    return false;
  }

  return false;
}

//---------------------------------------------------------------------------------------
void Trig::ReadLumiBlock::closeDb()
{
  try {
    if(dbIsOpen()) {
      m_debug << "ReadLumiBlock::closeDd - closing database '" << m_cool_id << endl;
      m_cool_ptr->closeDatabase();
    }
  } catch(std::exception& e) {
    m_infos << "ReadLumiBlock::closeDB - COOL exception caught: " << e.what() << endl
	          << "   could not close COOL database: " << m_cool_id << endl;
  }
  
  TrigConf::TrigConfCoolFolderSpec::resetSchemaVersion();
  return;
}

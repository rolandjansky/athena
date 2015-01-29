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
#include "TrigCostMonitor/ReadConf.h"

using namespace std;

//-----------------------------------------------------------------------------
// Implementation details for ConfigKeys
//
Trig::ConfigKeys::ConfigKeys()
  :m_smk(0),
   m_bgk(0),
   m_hlt_ps(0),
   m_lv1_ps(0),
   m_smk_set(false),
   m_bgk_set(false),
   m_hlt_ps_set(false),
   m_lv1_ps_set(false)
{
}

//---------------------------------------------------------------------------------------
bool Trig::ConfigKeys::equalKeys(const ConfigKeys &rhs) const
{
  if(!isValid() || !rhs.isValid()) {
    return false;
  }

  return (getSMK()    == rhs.getSMK()    && 
	  getBGK()    == rhs.getBGK()    && 
	  getHLT_PS() == rhs.getHLT_PS() && 
	  getLV1_PS() == rhs.getLV1_PS());
}

//---------------------------------------------------------------------------------------
bool Trig::ConfigKeys::isValid() const
{
  return (m_smk_set && m_bgk_set && m_hlt_ps_set && m_lv1_ps_set);
}

//---------------------------------------------------------------------------------------
void Trig::ConfigKeys::print(ostream &os) const
{
  //
  // Print unique configuration keys
  //  
  os << "Valid = " << isValid()
     << " BGK = " << getBGK()
     << " SMK = " << getSMK()
     << " LV1PS = " << getLV1_PS()
     << " HLTPS = " << getHLT_PS();
}

//---------------------------------------------------------------------------------------
bool Trig::operator<(const ConfigKeys &lhs, const ConfigKeys &rhs)
{
  std::vector<unsigned> lhs_vec;
  lhs_vec.push_back(lhs.getBGK());
  lhs_vec.push_back(lhs.getSMK());
  lhs_vec.push_back(lhs.getLV1_PS());
  lhs_vec.push_back(lhs.getHLT_PS());

  std::vector<unsigned> rhs_vec;
  rhs_vec.push_back(rhs.getBGK());
  rhs_vec.push_back(rhs.getSMK());
  rhs_vec.push_back(rhs.getLV1_PS());
  rhs_vec.push_back(rhs.getHLT_PS());

  return lhs_vec < rhs_vec;
}

//---------------------------------------------------------------------------------------
bool Trig::operator==(const ConfigKeys &lhs, const ConfigKeys &rhs)
{
  return lhs.equalKeys(rhs);
}

//-----------------------------------------------------------------------------
Trig::ReadConf::ReadConf()
  :m_cool_id("COOLONL_TRIGGER/COMP200"),
   m_cool_source(),
   m_run(0)
{
}

//-----------------------------------------------------------------------------
Trig::ReadConf::~ReadConf()
{
}

//-----------------------------------------------------------------------------
void Trig::ReadConf::updateConfigKeys(const unsigned run)
{
  //
  // Read configuration keys from COOL
  //
  m_keys.clear();
  m_cool_source.clear();
  m_run = run;

  cool::ValidityKey since(run);
  cool::ValidityKey until(run);
  cool::ValidityKey maskLB(0xffffffff);
  
  since <<= 32; // upper 31 bits are run number
  until <<= 32; // upper 31 bits are run number
  until  += maskLB;

  if(!dbIsOpen()) { 
    if(!openDb(true)) {
      m_infos << "UpdateConfigKeys - failed to open DB... nothing to do" << endl;    
      return;
    }
  }

  //
  // Read lumi blocks for this run
  //
  m_infos << "UpdateConfigKeys - reading lumi ranges..." << endl;

  cool::IFolderPtr lbFolder = m_cool_ptr->getFolder("/TRIGGER/LUMI/LBLB");
  cool::IObjectIteratorPtr objectsLB = lbFolder->browseObjects(since, until, 0);
  
  while ( objectsLB -> goToNext() ) {
    const cool::IObject &obj = objectsLB->currentRef();
    const unsigned lb0 = (obj.since() & maskLB);

    m_debug << "   lumi: "
	    << "   since=" << (obj.since() & maskLB)
	    << "   until=" << (obj.until() & maskLB) << endl;
    
    m_keys[lb0] = ConfigKeys();
  }
  
  //
  // Read SMK key
  //
  m_infos << "UpdateConfigKeys - reading SMK key..." << endl;

  cool::IFolderPtr smkFolder = m_cool_ptr->getFolder("/TRIGGER/HLT/HltConfigKeys");
  cool::IObjectIteratorPtr objectsSMK = smkFolder->browseObjects(since, until, 0);
  
  while ( objectsSMK->goToNext() ) {
    const cool::IObject &obj = objectsSMK->currentRef();

    const unsigned smk = obj.payloadValue<cool::UInt32>("MasterConfigurationKey");
    m_cool_source      = obj.payloadValue<cool::String255>("ConfigSource");
    
    for(KeyMap::iterator kit = m_keys.begin(); kit != m_keys.end(); ++kit) {
      kit->second.setSMK(smk);
    }

    m_infos << "   run=" << run
	    << "   SMK=" << smk
	    << "   since=" << (obj.since() & maskLB)
	    << "   until=" << (obj.until() & maskLB)
	    << "   source=" << m_cool_source << endl;
  }

  //
  // Read HLT PS key
  //
  m_infos << "UpdateConfigKeys - reading HLT config keys..." << endl;

  cool::IFolderPtr hltKeyFolder = m_cool_ptr->getFolder("/TRIGGER/HLT/PrescaleKey");
  cool::IObjectIteratorPtr objectsHLT = hltKeyFolder->browseObjects(since, until, 0);
  
  while ( objectsHLT->goToNext() ) {
    const cool::IObject &obj = objectsHLT->currentRef();
    
    const unsigned hltps = obj.payloadValue<cool::UInt32>("HltPrescaleKey");
    unsigned lb0 = obj.since() & maskLB;
    unsigned lb1 = obj.until() & maskLB;
  
    if(lb1 == 0) lb1 = m_keys.size();

    for(unsigned i = lb0; i <= lb1; ++i) {
      m_keys[i].setHLT_PS(hltps);
    }

    m_infos << "   HLT PS=" << hltps << "   since=" << lb0 << "   until=" << lb1 << endl;
  }
  
  //
  // Read LV1 config keys
  //
  m_infos << "UpdateConfigKeys - reading LV1 config keys..." << endl;

  cool::IFolderPtr lvl1CkFolder = m_cool_ptr->getFolder("/TRIGGER/LVL1/Lvl1ConfigKey");
  cool::IObjectIteratorPtr objectsLV1 = lvl1CkFolder->browseObjects(since, until, 0);
  
  while(objectsLV1->goToNext()) {
    const cool::IObject &obj = objectsLV1->currentRef();

    const unsigned lv1ps = obj.payloadValue<cool::UInt32>("Lvl1PrescaleConfigurationKey");
    unsigned lb0 = obj.since() & maskLB;
    unsigned lb1 = obj.until() & maskLB;

    if(lb1 == 0) lb1 = m_keys.size();

    for(unsigned i = lb0; i <= lb1; ++i) {
      m_keys[i].setLV1_PS(lv1ps);
    }

    m_infos << "   LV1 PS=" << lv1ps << "   since=" << lb0 << "   until=" << lb1 << endl;
  }
  
  //
  // Read bunch group id keys
  //
  m_infos << "UpdateConfigKeys - reading bunch group id keys..." << endl;

  cool::IFolderPtr bgFolder = m_cool_ptr->getFolder("/TRIGGER/LVL1/BunchGroupKey");
  cool::IObjectIteratorPtr objectsBG = bgFolder->browseObjects(since, until, 0);

  while(objectsBG->goToNext()) {
    const cool::IObject &obj = objectsBG->currentRef();

    const unsigned bgid = obj.payloadValue<cool::UInt32>("Lvl1BunchGroupConfigurationKey");
    unsigned lb0 = obj.since() & maskLB;
    unsigned lb1 = obj.until() & maskLB;

    if(lb1 == 0) lb1 = m_keys.size();

    for(unsigned i = lb0; i <= lb1; ++i) {
      m_keys[i].setBGK(bgid);
    }
    
    m_infos << "   BG id=" << bgid << "   since=" << lb0 << "   until=" << lb1 << endl;
  }

  closeDb();
}

//---------------------------------------------------------------------------------------
void Trig::ReadConf::print(ostream &os) const
{
  //
  // Print unique configuration keys
  //  
  KeyMap::const_iterator pit = m_keys.end();

  for(KeyMap::const_iterator kit = m_keys.begin(); kit != m_keys.end(); ++kit) { 
    const ConfigKeys &key = kit->second;           

    if(pit != m_keys.end() && key.equalKeys(pit->second)) {
      continue;
    }

    if(!key.isValid()) {
      os << " Invalid config key: ";
    }

    key.print(os);
    os << endl;
    
    pit = kit;
  }
}

//---------------------------------------------------------------------------------------
const std::map<unsigned, Trig::ConfigKeys> Trig::ReadConf::getNewKeys() const
{
  //
  // Return lumi blocks where keys change
  //  
  KeyMap keys;
  KeyMap::const_iterator pit = m_keys.end();

  for(KeyMap::const_iterator kit = m_keys.begin(); kit != m_keys.end(); ++kit) { 
    const ConfigKeys &key = kit->second;    
       
    if(!key.isValid()) {
      continue;
    }

    if(pit != m_keys.end() && key.equalKeys(pit->second)) {
      continue;
    }

    keys[kit->first] = kit->second;
    pit = kit;
  }
  
  return keys;
}

//---------------------------------------------------------------------------------------
cool::IDatabaseSvc& Trig::ReadConf::databaseService()
{
  static cool::Application app;
  return app.databaseService();
}

//---------------------------------------------------------------------------------------
bool Trig::ReadConf::dbIsOpen()
{ 
  return m_cool_ptr.use_count()>0 && m_cool_ptr->isOpen();
}

//---------------------------------------------------------------------------------------
bool Trig::ReadConf::openDb( bool readOnly )
{
  if(dbIsOpen()) return true;
  
  cool::IDatabaseSvc& dbSvc = databaseService();
  try {
    cout << "ReadConf::openDb - opening database '" << m_cool_id << "'" << endl;
    m_cool_ptr = dbSvc.openDatabase(m_cool_id, readOnly);
    
    const int schemaVersion = TrigConf::TrigConfCoolFolderSpec::readSchemaVersion(m_cool_ptr);
    cout << "ReadConf::openDb - using schema version: " << schemaVersion << endl;

    return true;
  }
  catch(cool::DatabaseDoesNotExist& e) {
    cout << "ReadConf::openDb - COOL exception caught: " << e.what() << endl
	    << "   could not open database: " << m_cool_id << endl;
    return false;
  }

  return false;
}

//---------------------------------------------------------------------------------------
void Trig::ReadConf::closeDb()
{
  try {
    if(dbIsOpen()) {
      cout << "ReadConf::closeDd - closing database '" << m_cool_id << endl;
      m_cool_ptr->closeDatabase();
    }
  } catch(std::exception& e) {
    cout << "ReadConf::closeDB - COOL exception caught: " << e.what() << endl
      << "   could not close COOL database: " << m_cool_id << endl;
  }
  
  TrigConf::TrigConfCoolFolderSpec::resetSchemaVersion();
  return;
}

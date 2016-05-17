/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IOVSvc/CondSvc.h"
#include "AthenaKernel/CondCont.h"
#include "GaudiKernel/SvcFactory.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <fstream>

// DECLARE_COMPONENT(CondSvc)

std::string r_t("\\[([0-9]+),([0-9]+)\\]");
std::string r_r = "\\s*\\{" + r_t + "-" + r_t + "\\}\\s*";
std::string r_e = "\\s*\\{" + r_t + "-" + r_t + "\\}=([0-9]+)\\s*";
std::string r_ef = "\\s*\\{" + r_t + "-" + r_t + "\\}=(-*[0-9]*\\.*[0-9]*)\\s*";
boost::regex rr(r_r);
boost::regex re(r_e);
boost::regex ref(r_ef);


//---------------------------------------------------------------------------

CondSvc::CondSvc( const std::string& name, ISvcLocator* svcLoc ):
  base_class(name,svcLoc),
  m_sgs("StoreGateSvc/ConditionStore", name)
{

  declareProperty("CondFile",m_file="");
  
}

//---------------------------------------------------------------------------

CondSvc::~CondSvc() {

}

//---------------------------------------------------------------------------

StatusCode
CondSvc::initialize() {

  // Initialise mother class in order to print DEBUG messages during initialize()
  StatusCode sc(AthService::initialize());
  msg().setLevel( m_outputLevel.value() );

  if (!sc.isSuccess()) {
    warning () << "Base class could not be initialized" << endmsg;
    return StatusCode::FAILURE;
  }

  if (!m_sgs.isValid()) {
    error() << "could not get ConditionStore" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_file == "") {
    ATH_MSG_DEBUG("db file not set");
    return StatusCode::SUCCESS;
  }

  if (readDbFile(m_file).isFailure()) {
    return StatusCode::FAILURE;
  }

  std::ostringstream ost;
  ost << " Printing CondDB registry";
  for (auto e : m_registry) {
    ost << std::endl << "  - id: " << e.first << "  r:";
    for (auto r : e.second) {
      ost << "  " << r.range() << " :: " << *r.objPtr();
    }
  }

  ATH_MSG_DEBUG( ost.str() );  

  return StatusCode::SUCCESS;

}
//---------------------------------------------------------------------------

StatusCode
CondSvc::readDbFile(const std::string& fname) {

  StatusCode sc(StatusCode::SUCCESS);

  ATH_MSG_DEBUG("reading cond db from \"" << fname << "\"");

  std::ifstream ifs (fname);
  std::string line;
  if(ifs.is_open()) {

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ");

    IOVEntryT<ICondSvc::dbData_t> ie;

    while( getline (ifs, line) ) {
      
      // ignore anything after a "#" and blank lines
      size_t fh = line.find("#");
      if(fh != std::string::npos) 
        line.erase(fh,line.length()-fh);
      if (line.length() == 0) continue;
      
      tokenizer tokens(line, sep);
      auto it = tokens.begin();
      
      std::string dbKey = *it;
      
      ++it;
      
      while (it != tokens.end()) {
        if (parse(ie,*it)) {
          m_registry[dbKey].push_back( ie );
        } else {
          error() << "while reading " << fname << " problem parsing " << *it 
                  << " in line\n" << line << endmsg;
          sc = StatusCode::FAILURE;
        }
        ++it;
      }
    }
    ifs.close();
  } else {
    error() << "unable to open file " << m_file << endmsg;
    sc = StatusCode::FAILURE;
  }

  return sc;

}
//---------------------------------------------------------------------------

void
CondSvc::dump() const {

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  info() << "CondSvc::dump()";

  info() << "\ndumping id->alg map\n";
  for (auto ent : m_idMap) {
    info() << std::endl << " + " << ent.first << " : ";
    for (auto a : ent.second) {
      info() << " " << a->name();
    }
  }

  info() << "\n\ndumping alg->id map\n";
  for (auto ent : m_algMap) {
    info() << std::endl << " + " << ent.first->name() << " : ";
    for (auto a : ent.second) {
      info() << " " << a;
    }
  }

  info() << std::endl <<  endmsg;
    
}

//---------------------------------------------------------------------------

StatusCode
CondSvc::finalize() {

  ATH_MSG_DEBUG( "CondSvc::finalize()" );

  for ( auto e : m_registry ) {
    for ( auto ie : e.second ) {
      delete ie.objPtr();
      ie.setPtr(0);
    }
  }

  return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------------

StatusCode 
CondSvc::regHandle(IAlgorithm* alg, const Gaudi::DataHandle& dh, 
                   const std::string& key) {

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  ATH_MSG_DEBUG( "regHandle: alg: " << alg->name() << "  id: " << dh.fullKey()
                 << "  dBkey: " << key );

  if (dh.mode() != Gaudi::DataHandle::Writer) {
    info() << dh.fullKey() << " is a ReadHandle. no need to register"
           << endmsg;
    return StatusCode::SUCCESS;
  }

  id_map_t::iterator itd2 = m_idMap.find(dh.fullKey());
  if (itd2 != m_idMap.end()) {
    IAlgorithm *ia = *(itd2->second.begin());
    if (ia->name() != alg->name()) {
      error() << "WriteCondHandle " << dh.fullKey() 
              << " is already registered against a different Algorithm " 
              << ia->name()
              << ". This is not allowed."
              << endmsg;
      return StatusCode::FAILURE;
    }
    // FIXME : so why is it a set<IAlgorithm*> ??
  }

  m_keyMap[key] = dh.fullKey();
  m_condAlgs.insert(alg);

  //  id_map_t::iterator itd2 = m_idMap.find(dh.fullKey());
  if (itd2 != m_idMap.end()) {
    itd2->second.insert( alg );
  } else {
    m_idMap[dh.fullKey()] = IAlgHashSet { alg };
  }

  alg_map_t::iterator ita2 = m_algMap.find(alg);
  if (ita2 != m_algMap.end()) {
    ita2->second.insert( dh.fullKey() );
  } else {
    m_algMap[alg] = DataObjIDColl { dh.fullKey() };
  }

  return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------------

bool
CondSvc::getInvalidIDs(const EventContext* ctx, DataObjIDColl& invalidIDs) {
  std::lock_guard<std::recursive_mutex> lock(m_lock);

  EventIDBase now(ctx->eventID().run_number(), ctx->eventID().event_number());

  std::ostringstream ost;
  ost << "getInvalidIDS " << ctx->eventID() 
      << ": retrieving all ConstIterator<CondContBase>";
  SG::ConstIterator<CondContBase> cib, cie;
  if (m_sgs->retrieve(cib,cie).isSuccess()) {
    while(cib != cie) {
      ost << std::endl << "  + " << cib.key() << "  " << cib->valid(now) 
          << "  id: "  << cib->id();
      
      if (! (cib->valid(now)) ) {
        invalidIDs.insert( cib->id() );
      }

      ++cib;      
    }
  } else {
    ATH_MSG_DEBUG(ost.str());
    ATH_MSG_DEBUG("ConditionStore is empty");
    return false;
  }

  ost << std::endl << "  -> found " << invalidIDs.size() << " invalid IDs";

  ATH_MSG_DEBUG( ost.str() );

  return true;
}

//---------------------------------------------------------------------------

bool
CondSvc::getIDValidity(const EventContext* ctx, DataObjIDColl& validIDs,
                       DataObjIDColl& invalidIDs) {
  std::lock_guard<std::recursive_mutex> lock(m_lock);

  EventIDBase now(ctx->eventID());

  std::ostringstream ost;
  ost << "getValidIDS " << ctx->eventID() 
      << ": retrieving all ConstIterator<CondContBase>";
  SG::ConstIterator<CondContBase> cib, cie;
  if (m_sgs->retrieve(cib,cie).isSuccess()) {
    while(cib != cie) {
      ost << std::endl << "  + " << cib.key() << "  v: " << cib->valid(now) 
          << "  id: "  << cib->id();
      
      if ( cib->valid(now) ) {
        validIDs.insert( cib->id() );
      } else {
        invalidIDs.insert( cib->id() );
      }

      ++cib;      
    }
  } else {
    ATH_MSG_DEBUG( ost.str() );
    ATH_MSG_DEBUG( "Condition store empty." );
    return false;
  }

  ost  << std::endl << "  -> found " << validIDs.size() << " valid, "
       << invalidIDs.size() << " invalid IDs";

  ATH_MSG_DEBUG( ost.str() );

  return true;
}

//---------------------------------------------------------------------------

bool
CondSvc::getValidIDs(const EventContext* ctx, DataObjIDColl& validIDs) {
  std::lock_guard<std::recursive_mutex> lock(m_lock);

  EventIDBase now(ctx->eventID());

  std::ostringstream ost;
  ost << "getValidIDS " << ctx->eventID() 
      << ": retrieving all ConstIterator<CondContBase>";
  SG::ConstIterator<CondContBase> cib, cie;
  if (m_sgs->retrieve(cib,cie).isSuccess()) {
    while(cib != cie) {
      ost << std::endl << "  + " << cib.key() << "  v: " << cib->valid(now) 
          << "  id: "  << cib->id();
      
      if ( cib->valid(now) ) {
        validIDs.insert( cib->id() );
      }

      ++cib;      
    }
  } else {
    ATH_MSG_DEBUG( ost.str() );
    ATH_MSG_DEBUG( "Condition store empty." );
    return false;
  }

  ost  << std::endl << "  -> found " << validIDs.size() << " valid IDs";

  ATH_MSG_DEBUG( ost.str() );

  return true;
}

//---------------------------------------------------------------------------

std::set<std::string>
CondSvc::getUnchangedAlgs(const DataObjIDColl& ids) {
  std::lock_guard<std::recursive_mutex> lock(m_lock);

  std::ostringstream ost;
  ost << "getUnchangedAlgs():";

  std::set<std::string> algs;
  id_map_t::const_iterator itr;

  for (auto id : ids) {
    ost << "\n  id: " << id;
    itr = m_idMap.find(id);
    if (itr == m_idMap.end()) {
      ost << " does not belong to any Alg!!! this shouldn't happen";
    } else {
      ost << " belongs to";
      // loop over all Algs registered to this obj
      for (auto alg : itr->second) {
        ost << " " << alg->name();

        // loop over all objs registered against this alg, make sure they
        // are ALL unchanged
        bool match = true;
        alg_map_t::const_iterator itr2 = m_algMap.find(alg);
        for (auto id2 : itr2->second) {
          if ( ids.find(id2) == ids.end()) {
            ost << " but has obj " << id2 << " that is not valid";
            match = false;
          }
        }

        if (match) {
          algs.insert( alg->name() );
        }
      }
    }
    //    ost << std::endl;
  }

  ATH_MSG_DEBUG( ost.str() );
    
  return algs;
}

//---------------------------------------------------------------------------

bool 
CondSvc::parse(EventIDRange& t, const std::string& s) {

  boost::smatch m;
  boost::regex_match(s,m,rr);

  // for (auto res : m) {
  //   cout << " - " << res << endl;
  // }

  if (m.size() != 5) { return false; }

  t = EventIDRange( EventIDBase(std::stoi(m[1]),std::stoi(m[2])), 
                    EventIDBase(std::stoi(m[3]), std::stoi(m[4])));

  return true;

}

//---------------------------------------------------------------------------

bool
CondSvc::parse(IOVEntryT<ICondSvc::dbData_t>& ie, const std::string& s) {

  boost::smatch m;
  boost::regex_match(s,m,ref);

  if (m.size() != 6) { return false; }

  ie.setRange( EventIDRange( EventIDBase(std::stoi(m[1]), std::stoi(m[2])), 
                             EventIDBase(std::stoi(m[3]), std::stoi(m[4])) ) );

  ICondSvc::dbData_t *v = new ICondSvc::dbData_t( std::stof(m[5]) );
  ie.setPtr(v);

  return true;

}

//---------------------------------------------------------------------------

StatusCode
CondSvc::getRange(const std::string& dbKey , const EventContext* ctx,
                  EventIDRange& rng, ICondSvc::dbData_t& val) const {

  std::lock_guard<std::recursive_mutex> lock(m_lock);

  registry_t::const_iterator itr = m_registry.find(dbKey);

  if (itr == m_registry.end()) {
    error() << "getRange: no dbKey " << dbKey << " found in registry" 
            << endmsg;
    return StatusCode::FAILURE;
  }

  for (auto e : itr->second) {
    if (e.range().isInRange(EventIDBase(ctx->eventID()))) {
      rng = e.range();
      val = *(e.objPtr());
      return StatusCode::SUCCESS;
    }
  }

  error() << "getRange: no range for Time " << ctx->eventID() 
          << " found for dbKey "  << dbKey << endmsg;

  return StatusCode::FAILURE;
}

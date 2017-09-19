/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ASCIICondDbSvc.h"
#include "AthenaKernel/CondCont.h"
#include "GaudiKernel/SvcFactory.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <fstream>

std::string r_t("\\[([0-9]+),([0-9]+)\\]");
std::string r_r = "\\s*\\{" + r_t + "-" + r_t + "\\}\\s*";
std::string r_e = "\\s*\\{" + r_t + "-" + r_t + "\\}=([0-9]+)\\s*";
std::string r_ef = "\\s*\\{" + r_t + "-" + r_t + "\\}=(-*[0-9]*\\.*[0-9]*)\\s*";
boost::regex rr(r_r);
boost::regex re(r_e);
boost::regex ref(r_ef);


//---------------------------------------------------------------------------

ASCIICondDbSvc::ASCIICondDbSvc( const std::string& name, ISvcLocator* svcLoc ):
  base_class(name,svcLoc)
{}

//---------------------------------------------------------------------------

ASCIICondDbSvc::~ASCIICondDbSvc() {

}

//---------------------------------------------------------------------------

StatusCode
ASCIICondDbSvc::initialize() {

  // Initialise mother class in order to print DEBUG messages during initialize()
  StatusCode sc(AthService::initialize());
  msg().setLevel( m_outputLevel.value() );

  if (!sc.isSuccess()) {
    warning () << "Base class could not be initialized" << endmsg;
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
ASCIICondDbSvc::readDbFile(const std::string& fname) {

  StatusCode sc(StatusCode::SUCCESS);

  ATH_MSG_DEBUG("reading cond db from \"" << fname << "\"");

  std::ifstream ifs (fname);
  std::string line;
  if(ifs.is_open()) {

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ");

    IOVEntryT<IASCIICondDbSvc::dbData_t> ie;

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
    error() << "unable to open file " << (std::string) m_file << endmsg;
    sc = StatusCode::FAILURE;
  }

  return sc;

}
//---------------------------------------------------------------------------

void
ASCIICondDbSvc::dump() const {

  std::ostringstream ost;
  dump(ost);

  info() << ost.str() << endmsg;

}


//---------------------------------------------------------------------------

void
ASCIICondDbSvc::dump(std::ostringstream& ost) const {

  std::lock_guard<std::mutex> lock(m_lock);

  ost << "ASCIICondDbSvc::dump()";

  ost << "\n";
    
}

//---------------------------------------------------------------------------

StatusCode
ASCIICondDbSvc::finalize() {

  ATH_MSG_DEBUG( "ASCIICondDbSvc::finalize()" );

  if (msgLvl(MSG::DEBUG)) {
    std::ostringstream ost;
    dump(ost);
    
    ATH_MSG_DEBUG( ost.str() );
  }

  for ( auto e : m_registry ) {
    for ( auto ie : e.second ) {
      delete ie.objPtr();
      ie.setPtr(0);
    }
  }


  return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------------

bool 
ASCIICondDbSvc::parse(EventIDRange& t, const std::string& s) {

  boost::smatch m;
  boost::regex_match(s,m,rr);

  // for (auto res : m) {
  //   cout << " - " << res << endl;
  // }

  if (m.size() != 5) { return false; }

  EventIDBase start(std::stoi(m[1]), std::stoi(m[2]));
  EventIDBase   end(std::stoi(m[3]), std::stoi(m[4]));

  start.set_lumi_block(m_lbn);
  end.set_lumi_block(m_lbn);
  
  t = EventIDRange(start, end);

  return true;

}

//---------------------------------------------------------------------------

bool
ASCIICondDbSvc::parse(IOVEntryT<IASCIICondDbSvc::dbData_t>& ie, const std::string& s) {

  boost::smatch m;
  boost::regex_match(s,m,ref);

  if (m.size() != 6) { return false; }

  EventIDBase start(std::stoi(m[1]), std::stoi(m[2]));
  EventIDBase   end(std::stoi(m[3]), std::stoi(m[4]));

  start.set_lumi_block(m_lbn);
  end.set_lumi_block(m_lbn);

  ie.setRange(EventIDRange(start,end));
  
  IASCIICondDbSvc::dbData_t *v = new IASCIICondDbSvc::dbData_t( std::stof(m[5]) );
  ie.setPtr(v);

  return true;

}

//---------------------------------------------------------------------------

StatusCode
ASCIICondDbSvc::getRange(const std::string& dbKey , const EventContext& ctx,
                  EventIDRange& rng, IASCIICondDbSvc::dbData_t& val) const {

  std::lock_guard<std::mutex> lock(m_lock);

  registry_t::const_iterator itr = m_registry.find(dbKey);

  if (itr == m_registry.end()) {
    error() << "getRange: no dbKey " << dbKey << " found in registry" 
            << endmsg;
    return StatusCode::FAILURE;
  }

  for (auto e : itr->second) {
    debug() << "compare " << e.range() << " with " << ctx.eventID()
            << endmsg;
    if (e.range().isInRange(EventIDBase(ctx.eventID()))) {
      rng = e.range();
      val = *(e.objPtr());
      return StatusCode::SUCCESS;
    }
  }

  error() << "getRange: no range for Time " << ctx.eventID() 
          << " found for dbKey "  << dbKey << endmsg;

  return StatusCode::FAILURE;
}

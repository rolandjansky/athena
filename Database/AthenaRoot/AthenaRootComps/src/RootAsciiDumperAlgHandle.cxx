///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootAsciiDumperAlgHandle.cxx 
// Implementation file for class RootAsciiDumperAlgHandle
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "RootAsciiDumperAlgHandle.h"

// STL includes
#include <sstream>
#include <stdio.h>
// to get the printing format specifiers (e.g. PRId64)
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// linux i/o includes
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// FrameWork includes
#include "GaudiKernel/Property.h"

// SGTools
#include "SGTools/BuiltinsClids.h"  // to put ints,... in evtstore
#include "SGTools/StlVectorClids.h" // to put std::vectors... in evtstore

namespace Athena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
RootAsciiDumperAlgHandle::RootAsciiDumperAlgHandle( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_ofname(""),
  m_ofd(-1),
  m_nentries(0),
  m_runnbr(),
  m_evtnbr(),
  m_el_n(),
  m_el_eta(),
  m_el_jetcone_dr()
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty("AsciiFileName",
                  m_ofname = "d3pd.ascii",
                  "Name of the ascii file where the content of the "
                  "ROOT n-tuple file will be dumped.");

  declareProperty("RunNumber",
                  m_runnbr = SG::RVar<uint32_t>("RunNumber"),
                  "handle to the run-nbr in event (read)");

  declareProperty("EventNumber",
                  m_evtnbr = SG::RVar<uint32_t>("EventNumber"),
                  "handle to the evt-nbr in event (read)");

  declareProperty("el_n",
                  m_el_n = SG::RVar<int32_t>("el_n"),
                  "handle to the nbr of electrons in event (read)");

  declareProperty("el_eta",
                  m_el_eta = SG::RVar<std::vector<float> >("el_eta"),
                  "handle to the eta of electrons in event (read)");

  declareProperty("el_jetcone_dr",
                  m_el_jetcone_dr = SG::RVar<std::vector<std::vector<float> > >("el_jetcone_dr"),
                  "handle to the jetcone-dR of electrons in event (read)");


  declareProperty("eiKey",
                  m_eiKey = "EventInfo");

}

// Destructor
///////////////
RootAsciiDumperAlgHandle::~RootAsciiDumperAlgHandle()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode RootAsciiDumperAlgHandle::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_MSG_INFO("dumping data into file ["
               << m_ofname << "]...");
  if (m_ofname.empty()) {
    ATH_MSG_ERROR("cannot dump data into an empty file name!");
    return StatusCode::FAILURE;
  }
  m_ofd = open(m_ofname.c_str(), 
               O_WRONLY | O_CREAT | O_TRUNC,
               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (m_ofd < 0) {
    ATH_MSG_ERROR("problem opening file [" << m_ofname << "] with "
                  "write permissions.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_eiKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode RootAsciiDumperAlgHandle::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  if (m_ofd > 0) {
    fflush(NULL);
    if (close(m_ofd) < 0) {
      ATH_MSG_WARNING("problem while closing [" << m_ofname << "]");
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RootAsciiDumperAlgHandle::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  uint64_t nevts = m_nentries;
  m_nentries += 1;

  if (!m_runnbr.ptr()) {
    ATH_MSG_WARNING("could not retrieve [" << m_runnbr.name() 
                    << "] from store");
    return StatusCode::RECOVERABLE;
  }

  if (!m_evtnbr.ptr()) {
    ATH_MSG_WARNING("could not retrieve [" << m_evtnbr.name() 
                    << "] from store");
    return StatusCode::RECOVERABLE;
  }

  if (!m_el_n.ptr()) {
    ATH_MSG_WARNING("could not retrieve [" << m_el_n.name() 
                    << "] from store");
    return StatusCode::RECOVERABLE;
  }

  SG::ReadHandle<xAOD::EventInfo> ei (m_eiKey);
  static const SG::AuxElement::Accessor<std::string> tupleName ("tupleName");
  static const SG::AuxElement::Accessor<std::string> collectionName ("collectionName");
  std::string collName = collectionName(*ei);
  std::string::size_type pos = collName.rfind ("/");
  if (pos != std::string::npos) {
    collName.erase (0, pos+1);
  }

  {
    char* buf = 0;
    int buf_sz = asprintf
      (&buf,
       "%03" PRId64 ".%s = %s\n"
       "%03" PRId64 ".%s = %s\n"
       "%03" PRId64 ".%s = %u\n"
       "%03" PRId64 ".%s = %u\n"
       "%03" PRId64 ".%s = %i\n",
       nevts,
       "collectionName",
       collName.c_str(),
       nevts,
       "tupleName",
       tupleName(*ei).c_str(),
       nevts,
       "RunNumber",
       *m_runnbr,
       nevts,
       "EventNumber",
       *m_evtnbr,
       nevts,
       "el_n",
       *m_el_n);
    write(m_ofd, buf, buf_sz);
    free(buf);
  }

  if (*m_el_n > 0) {
    if (!m_el_eta.ptr()) {
      ATH_MSG_WARNING("could not retrieve [" << m_el_eta.name() 
                      << "] from store");
      return StatusCode::RECOVERABLE;
    }
    
    if (!m_el_jetcone_dr.ptr()) { 
      ATH_MSG_WARNING("could not retrieve [" << m_el_jetcone_dr.name() 
                      << "] from store");
      return StatusCode::RECOVERABLE;
    }
    
    {
      std::stringstream bufv;
      for (int32_t ii = 0; ii < *m_el_n; ++ii) {
        bufv << (*m_el_eta)[ii];
        if (ii != (*m_el_n)-1) {
          bufv << ", ";
        }
      }
      char* buf = 0;
      int buf_sz = asprintf
        (&buf,
         "%03" PRId64 ".%s = [%s]\n",
         nevts,
         "el_eta",
         bufv.str().c_str());
      write(m_ofd, buf, buf_sz);
      free(buf);
    }


    {
      std::stringstream bufv;
      for (int32_t ii = 0; ii < *m_el_n; ++ii) {
        bufv << "[";
        for (std::size_t jj = 0, jjmax = (*m_el_jetcone_dr)[ii].size();
             jj < jjmax;
             ++jj) {
          bufv << (*m_el_jetcone_dr)[ii][jj];
          if (jj != jjmax-1) {
            bufv << ", ";
          }
        }
        bufv << "]";
        if (ii != (*m_el_n)-1) {
          bufv << ", ";
        }
      }
      char* buf = 0;
      int buf_sz = asprintf
        (&buf,
         "%03" PRId64 ".%s = [%s]\n",
         nevts,
         "el_jetcone_dr",
         bufv.str().c_str());
      write(m_ofd, buf, buf_sz);
      free(buf);
    }
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace Athena

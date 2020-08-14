///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// StorePayloadMon.cxx 
// Implementation file for class StorePayloadMon
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PerfMonComps includes
#include "PerfMonStorePayloadMon.h"

// C includes
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

// STL includes
#include <vector>
#include <sstream>

// FrameWork includes
#include "Gaudi/Property.h"
#include "SGTools/DataProxy.h"

// pmon edm
#include "PerfMonEvent/MallocStats.h"

namespace PerfMon {

struct DpLoad
{
  const SG::DataProxy* proxy;
  MallocStats::return_type b0; //> number of bytes before flushing proxy
  MallocStats::return_type b1; //> number of bytes after  flushing proxy
};

  typedef ::std::vector<DpLoad> DpLoads_t;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
StorePayloadMon::StorePayloadMon( const std::string& name, 
                                  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_store("StoreGateSvc/StoreGateSvc", name),
  m_clidsvc("ClassIDSvc/ClassIDSvc", name),
  m_stream(-1),
  m_stream_name("pmon_payload.ascii")
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty("Store", m_store,
                  "StoreGateSvc instance to monitor for "
                  "DataProxy payload sizes");

  declareProperty("OutputName",
                  m_stream_name = "pmon_payload.ascii",
                  "Name of the output file where the monitoring data will "
                  "be stored");

  declareProperty("DisplayMallinfos",
                  m_displayMallinfos = false,
                  "display mallinfos stats after each event");
}

// Destructor
///////////////
StorePayloadMon::~StorePayloadMon()
{
  if (m_stream>0) {
    fflush(NULL); //> flushes all output streams
    close(m_stream);
  }

}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode StorePayloadMon::initialize()
{
  // retrieve store to monitor
  if (!m_store.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve [" << m_store.typeAndName() << "]");
    return StatusCode::FAILURE;
  }

  // retrieve clidsvc
  if (!m_clidsvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve [" << m_clidsvc.typeAndName() << "]");
    return StatusCode::FAILURE;
  }

  // open payload-mon stream
  m_stream = open(m_stream_name.c_str(),
                  O_WRONLY | O_CREAT,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (m_stream<0) {
    ATH_MSG_ERROR("could not open payload-mon stream file ["
                  << m_stream_name << "]");
    return StatusCode::FAILURE;
  }

  { // describe data format
    const char* hdr = "#data format: [[(bytes_0, bytes_1, delta, clid, sg_key),]]\n";
    write(m_stream, hdr, strlen(hdr));
    const char* data_hdr = "data = [\n";
    write(m_stream, data_hdr, strlen(data_hdr));
  }

  return StatusCode::SUCCESS;
}

StatusCode StorePayloadMon::finalize()
{
  {
    const char *ftr = "] # data\n";
    write(m_stream, ftr, strlen(ftr));
  }
  return StatusCode::SUCCESS;
}

inline 
PerfMon::MallocStats::return_type
pmon_get_mem()
{ 
  struct mallinfo m = mallinfo();
  return m.uordblks + m.hblkhd - m.fsmblks;
}

#if 0
inline
std::string
diff_minfos(const PerfMon::MallocStats& ms0,
            const PerfMon::MallocStats& ms1)
{
  const struct mallinfo& m0 = ms0.m_infos;
  const struct mallinfo& m1 = ms1.m_infos;

#define PP_DIFF(f) m0. f << " -> " << m1. f << " => " << m1. f - m0. f
#define PP_TOT(m) (m.uordblks + m.hblkhd - m.fsmblks)

  std::ostringstream s;
  s << "-------------\n"
    << "arena   : "<<PP_DIFF(arena)<< "\n"
    << "ordblks : "<<PP_DIFF(ordblks)<< "\n" /* number of free chunks */
    << "smblks  : "<<PP_DIFF(smblks)<< "\n"  /* number of fastbin blocks */
    << "hblks   : "<<PP_DIFF(hblks)<< "\n"   /* number of mmapped regions */
    << "hblkhd  : "<<PP_DIFF(hblkhd)<< "\n"  /* space in mmapped regions */ 
    << "usmblks : "<<PP_DIFF(usmblks)<< "\n" /* maximum total allocated space */
    << "fsmblks : "<<PP_DIFF(fsmblks)<< "\n" /* space available in freed fastbin blocks */
    << "uordblks: "<<PP_DIFF(uordblks)<< "\n" /* total allocated space */
    << "fordblks: "<<PP_DIFF(fordblks)<< "\n" /* total free space */
    << "keepcost: "<<PP_DIFF(keepcost)<< "\n" /* top-most, releasable (via malloc */
    << "==> " << PP_TOT(m0) << " -> " << PP_TOT(m1) 
    << " => " << (PP_TOT(m1) - PP_TOT(m0))
    << "\n"
    << std::ends;

#undef PP_DIFF
#undef PP_TOT

  return s.str();
}
#endif

StatusCode StorePayloadMon::execute()
{  
  std::vector<const SG::DataProxy*> proxies = m_store->proxies();
  const std::size_t n = proxies.size();
  DpLoads_t data(n);
  
  static std::size_t ievt = 0;
  PerfMon::MallocStats::return_type b0 = pmon_get_mem();
  for (std::size_t i = 0; i!=n; ++i) {
    SG::DataProxy* proxy = const_cast<SG::DataProxy*>(proxies[i]); // Blech!
    DpLoad& mon = data[i];
    mon.proxy = proxy;
    mon.b0 = -1;
    mon.b1 = -1;
    if (proxy->clID() == 2101) { // event-info
      // do not flush this one, as it is needed everywhere...
      continue;
    }
#ifndef __APPLE__
    malloc_trim(0); // compact heap...
#endif
    mon.b0 = pmon_get_mem();
    m_store->clearProxyPayload(proxy);
    mon.b1 = pmon_get_mem();

    // if ( mon.b1 > mon.b0 ) {
    //   std::cerr << "=========[" << mon.proxy->name() << "]=======\n"
    //             << "evt: " << ievt << "\n"
    //             << diff_minfos(mi0, mi1)
    //             << "\n";
    // }
  }
  // {
  //   const bool forceRemove = true;
  //   m_store->clearStore(forceRemove).ignore();
  // }
  PerfMon::MallocStats::return_type b1 = pmon_get_mem();

  if (m_displayMallinfos) {
    PerfMon::MallocStats().dump();
    //malloc_stats();
  }

  const char* hdr = " [ ## new-event\n";
  write(m_stream, hdr, strlen(hdr));

#define PMON_DP_FMT "   (%10d, %10d, %10d, \"%s\", \"%s\"),\n"
  for (std::size_t i = 0; i!=n; ++i) {
    DpLoad& mon = data[i];
    std::string clid = "";
    if (!m_clidsvc->getTypeNameOfID(mon.proxy->clID(), clid).isSuccess()) {
      std::stringstream o;
      o << mon.proxy->clID() << std::ends;
      clid = o.str();
    }
    char *buf = NULL;
    int buf_sz = asprintf
      (&buf,
       PMON_DP_FMT,
       mon.b0, mon.b1, mon.b1 - mon.b0,
       clid.c_str(), 
       mon.proxy->name().c_str());
    write(m_stream, buf, buf_sz);
    free(buf);
  }
  
  char *buf = NULL;
  int buf_sz = asprintf
    (&buf,
     PMON_DP_FMT,
     b0, b1, b1 - b0,
     "StoreGateSvc", m_store.name().c_str());
  write(m_stream, buf, buf_sz);
  free(buf);
 
#undef PMON_DP_FMT

  const char* ftr = " ],\n";
  write(m_stream, ftr, strlen(ftr));

  ATH_MSG_INFO("flush-store: " << b0 << " -> " << b1 << " -- delta= "
               << b1 -b0);

  ++ievt;
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

} //> end namespace PerfMon

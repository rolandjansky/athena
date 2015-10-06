///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ThinningOutputTool.cxx 
// Implementation file for class ThinningOutputTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <climits>
#include <cstdlib> // for atoi
#include <stdexcept>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

// AthenaKernel
#include "AthenaKernel/IThinningSvc.h"

// SGTools
#include "SGTools/DataProxy.h"

// AthenaServices includes
#include "ThinningOutputTool.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ThinningOutputTool::ThinningOutputTool( const std::string& type, 
					const std::string& name, 
					const IInterface* parent ) : 
  AthAlgTool( type, name, parent ),
  m_thinningSvc ( "ThinningSvc",  name ),
  m_activeSvc   ( 0 )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "ThinningSvc",
		   m_thinningSvc = ThinningSvc_t( "ThinningSvc", name ),
		   "Handle to the service which has thinned data" );

  declareProperty( "Proxies",
		   m_proxies,
		   "list of pairs (clid,key) the tool will forcingly read "
		   "from input to ensure a sound thinning state" )
    ->declareUpdateHandler(&ThinningOutputTool::propertyHandler, this);

   // Declare IAthenaOutputStreamTool interface
  declareInterface<IAthenaOutputTool>(this);

}

// Destructor
///////////////
ThinningOutputTool::~ThinningOutputTool()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinningOutputTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // Get pointer to ThinningSvc and cache it :
  if ( !m_thinningSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Unable to retrieve pointer to ThinningSvc");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO("proxies to be forcingly read: " << m_proxies.size());
  if (msgLvl(MSG::VERBOSE)) {
    for (std::size_t i = 0, imax = m_proxies.size(); i != imax; ++i) {
      ATH_MSG_VERBOSE(" clid=[" << m_proxies[i][0] << "] sg_key=["
		      << m_proxies[i][1] << "]");
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode ThinningOutputTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode ThinningOutputTool::preExecute()
{
  bool allGood = true;
  ATH_MSG_DEBUG ("Calling IThinningSvc::commit()...");

  // retrieve the currently active thinning svc
  m_activeSvc = IThinningSvc::instance();
    
  // set our thinning svc as the active one
  static const bool override = true;
  IThinningSvc* svc = IThinningSvc::instance( &*m_thinningSvc, override );
  if ( svc != &*m_thinningSvc ) {
    ATH_MSG_ERROR
      ("Could not set our peer ThinningSvc as the current active one !");
    // put back the previous one
    IThinningSvc::instance( m_activeSvc, override );
    // bail-out
    return StatusCode::FAILURE;
  }

  const bool doThinning = m_thinningSvc->thinningOccurred();
  if (doThinning && m_proxies.size()>0) {
    typedef std::vector<const SG::DataProxy*> SgProxies_t;
    SgProxies_t proxies = m_thinningSvc->proxies(); // just in case
    for ( std::size_t i = 0, imax = m_proxies.size();
	  i != imax;
	  ++i ) {
      const Proxy_t& proxy = m_proxies[i];
      CLID clid = std::atol(proxy[0].c_str());
      const std::string& key = proxy[1];
      ATH_MSG_VERBOSE("loading proxy(" << clid << ", " << key << ")...");
      if (key != "*") {
	SG::DataProxy* p = m_thinningSvc->proxy(clid, key);
	if (!p) {
	  // this proxy is only accessible *when* back navigation is 'ON'
	  ATH_MSG_VERBOSE("null pointer to proxy(" <<clid<<"#"<<key<< ")");
	  continue;
	}
	if ( p->isValid() ) {
	  ATH_MSG_DEBUG 
	    ("force-loading proxy(clid=[" << p->clID() << "], "
	     << "key=[" << p->name() << "])...");
	  if ( 0 == p->accessData() ) {
	    ATH_MSG_WARNING
	      ("Could not accessData(clid=[" << p->clID() << "], "
	       << "key=[" << p->name() << "]) !" 
	       << endreq
	       << "Thinning might segfault under certain conditions...");
	  }
	}
      } else {
	// get all proxies with CLID 'clid'
	ATH_MSG_VERBOSE("requested to load all proxies with clid ["
			<<clid<<"]...");
	for (std::size_t j = 0, jmax = proxies.size(); j != jmax; ++j) {
	  // FIXME: linear search !
	  if (proxies[j]->clID() != clid) {
	    continue;
	  }
	  // lovely...
	  SG::DataProxy* p = const_cast<SG::DataProxy*>(proxies[j]);
	  if (p->isValid()) {
	    ATH_MSG_DEBUG
	      ("force-loading proxy(clid=[" << p->clID() << "], "
	       << "key=[" << p->name() << "])...");
	    if ( 0 == p->accessData() ) {
	      ATH_MSG_WARNING
		("Could not accessData(clid=[" << p->clID() << "], "
		 << "key=[" << p->name() << "]) !" 
		 << endreq
		 << "Thinning might segfault under certain conditions...");
	    }
	  }
	}
      } //> a wildcard was there...
    } //> loop over my proxy list
  } //> thinning occured and proxies to load

  // apply thinning only if needed
  if ( doThinning && !m_thinningSvc->commit().isSuccess() ) {
    ATH_MSG_ERROR
      ("IThinningSvc::commit() failed !" << endreq
       << "Containers (and ElementLinks pointing to their elements) are "
       << "most probably in a corrupted state !!");
    allGood = false;
  }

  return allGood 
    ? StatusCode::SUCCESS 
    : StatusCode::FAILURE
    ;
}
  
StatusCode ThinningOutputTool::postExecute()
{
  bool allGood = true;
  ATH_MSG_DEBUG ("Calling IThinningSvc::rollback()...");

  const bool doThinning = m_thinningSvc->thinningOccurred();
  if ( doThinning && !m_thinningSvc->rollback().isSuccess() ) {
    ATH_MSG_ERROR
      ("IThinningSvc::rollback() failed !" << endreq
       << "Containers (and ElementLinks pointing to their elements) are "
       << "most probably in a corrupted state !!");
    allGood = false;
  }

  // restore old active svc
  static const bool override = true;
  IThinningSvc* svc = IThinningSvc::instance( m_activeSvc, override );
  if ( svc != m_activeSvc ) {
    ATH_MSG_WARNING
      ("Could not setup the previous active thinning svc as the "\
       "current active one !");
  }

  return allGood 
    ? StatusCode::SUCCESS 
    : StatusCode::FAILURE
    ;
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

void
ThinningOutputTool::propertyHandler(Property& /*m_proxies*/)
{
  const Proxies_t& proxies = m_proxies;

  for (Proxies_t::const_iterator i = proxies.begin(), iend = proxies.end();
       i != iend;
       ++i) {
    const Proxy_t& proxy = *i;
    if (proxy.size() != 2) {
      ATH_MSG_ERROR("invalid proxy format - need 2 items. got: " << 
		    proxy.size());
      throw std::runtime_error("invalid proxy format (need 2 items)");
    }
    const long clid = std::atol(proxy[0].c_str());
    if (clid == LONG_MAX || clid == LONG_MIN || clid <= 0) {
      ATH_MSG_ERROR("invalid clid number [" << proxy[0] << "] (converts to ["
		    << clid << "])");
      throw std::runtime_error("invalid clid number");
    }
  }
}

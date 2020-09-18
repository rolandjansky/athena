/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  CaloRunClusterCorrections.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2009
 * @brief Meta-tool to construct and run a list of correction tools.
 *
 * See the header file for a complete description.
 */


#include "CaloRunClusterCorrections.h"
#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloRec/ToolWithConstantsMixin.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/Array.h"
#include "AthenaKernel/getMessageSvc.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include <set>


using xAOD::CaloCluster;

namespace {


/// Return the number of characters A and B have in common at the start.
size_t match_len (const std::string& a, const std::string& b)
{
  size_t len = 0;
  while (len < a.size() && len < b.size() && a[len] == b[len])
    ++len;
  return len;
}


} // anonymous namespace


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
CaloRunClusterCorrections::CaloRunClusterCorrections (const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent)
  : CaloClusterProcessor (type, name, parent),
    m_jos       ("JobOptionsSvc", name),
    m_toolsvc   ("ToolSvc",       name),
    m_coolInlineTool("Blob2ToolConstants",this)
{
  declareProperty ("CorrSpecs",     m_corrspecs);
  declareProperty ("KeepList",      m_keeplist);
  declareProperty ("PreserveOrder", m_preserveOrder = false);
  declareProperty ("NoClearProps",  m_noClearProps = false);

  declareProperty ("JobOptionsSvc", m_jos);
  declareProperty ("ToolSvc",       m_toolsvc);
  declareProperty ("COOLFolder",    m_folderName);
}


/**
 * @brief Standard initialize method.
 */
StatusCode CaloRunClusterCorrections::initialize ATLAS_NOT_THREAD_SAFE /*Can Register callbacks but no need to be thread safe*/ ()
{
  // Fetch services used.
  CHECK( m_jos.retrieve() );
  CHECK( m_toolsvc.retrieve() );

  // Parse input properties.
  CHECK( parseCorrspecs() );
  CHECK( parseKeeplist() );

  // Create tools that don't depend on DB.
  CHECK( createTools() );

  // Set up callbacks for tools do depend on the DB.
  // Note: we _must_ return @c SUCCESS after doing this, or we may
  // cause a crash.
  registerCallbacks();

  ATH_CHECK( m_affKey.initialize() );
  ATH_CHECK( m_LArBCKey.initialize() );
  ATH_CHECK( m_TileBCKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard finalize method.
 */
StatusCode CaloRunClusterCorrections::finalize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Execute on a single cluster.
 * @param The cluster to process.
 * @param ctx The event context.
 *
 * This will iterate over all correction tools and call them on the
 * cluster.
 *
 * Warning: Any defined tools that cannot process single clusters
 * (only cluster collections) will be skipped!
 */
StatusCode CaloRunClusterCorrections::execute (const EventContext& ctx,
                                               CaloCluster* cluster) const
{
  // Loop over tools and run each one on the cluster.
  for (unsigned int itool = 0;
       itool < m_toolorder.size();
       ++itool)
  {
    const Tool& tool = m_tools[m_toolorder[itool]];
    if (tool.clusproc)
      CHECK( tool.clusproc->execute (ctx, cluster) );
    else {
      REPORT_MESSAGE (MSG::WARNING)
        << "Tool " << tool.name
        << " is not defined for individual clusters; skipped!";
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Execute on an entire collection of clusters.
 * @param The container of clusters.
 * @param ctx The event context.
 *
 * This will iterate over all correction tools and call them on the
 * cluster collection.
 */
StatusCode
CaloRunClusterCorrections::execute (const EventContext& ctx,
                                    xAOD::CaloClusterContainer* collection) const
{
  // Loop over correction tools.
  for (unsigned int itool = 0;
       itool < m_toolorder.size();
       ++itool)
  {
    const Tool& tool = m_tools[m_toolorder[itool]];

    // See if we should store an intermediate cluster container before
    // the next correction.
    if (!tool.save_container.empty()) {
      xAOD::CaloClusterContainer* interimCont=CaloClusterStoreHelper::makeContainer(&(*evtStore()),tool.save_container,msg());
      CaloClusterStoreHelper::copyContainer(collection,interimCont);
      CHECK(CaloClusterStoreHelper::finalizeClusters(&(*evtStore()),interimCont, tool.save_container, msg()));
    }
    
    CHECK( tool.collproc->execute (ctx, collection) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Change the name of the CaloCellContainer used by this tool.
 * @param name The new container name.
 */
StatusCode
CaloRunClusterCorrections::setCaloCellContainerName
  (const std::string& name)
{
  for (size_t itool = 0; itool < m_tools.size(); ++itool) {
    ISetCaloCellContainerName* setter = m_tools[itool].setcellcontname;
    if (setter)
      CHECK (setter->setCaloCellContainerName (name) );
  }
  return StatusCode::SUCCESS;
}


//****************************************************************************


/**
 * @brief Parse the supplied correction specification and create
 *        the Tools vector.  Do not actually create the tools yet.
 */
StatusCode CaloRunClusterCorrections::parseCorrspecs ATLAS_NOT_THREAD_SAFE ()
{
  size_t ispec = 0;
  while (ispec < m_corrspecs.size()) {
    // Starting a new tool.
    Tool tool;
    tool.collproc = nullptr;
    tool.clusproc = nullptr;
    tool.setcellcontname = nullptr;

    // -- Name.
    tool.name = m_corrspecs[ispec++];
    std::string fullname = this->name() + "." + tool.name;

    // -- Key or class.
    if (ispec >= m_corrspecs.size()) {
      REPORT_ERROR(StatusCode::FAILURE)
        << "Malformed corrections spec: missing location.";
      return StatusCode::FAILURE;
    }

    tool.use_callback = false;
    if (m_corrspecs[ispec][0] == '+') {
      // Initializing from JO.
      tool.clsname = m_corrspecs[ispec].substr (1);
    }
    else if (m_corrspecs[ispec][0] == '@') {
      // Initializing directly from detector store (POOL).
      tool.sgkey = m_corrspecs[ispec].substr (1);
    }
    else {
      // Initializing from COOL.
      // The SG key here is the same as the folder name.
      tool.sgkey = m_corrspecs[ispec];
      tool.use_callback = true;
    }

    if (tool.clsname.empty() && tool.sgkey.empty()) {
      REPORT_ERROR(StatusCode::FAILURE)
        << "Malformed corrections spec: missing key-or-classname.";
      return StatusCode::FAILURE;
    }

    // Set tool properties needed to access the detector store.
    // We don't want the tool itself to register a callback; we'll
    // do that ourselves and forward callbacks as appropriate.
    if (!tool.sgkey.empty()) {
      m_jos->set (fullname + ".detStoreKey", tool.sgkey);
      m_jos->set (fullname + ".useCallback", Gaudi::Utils::toString(false));
      m_jos->set (fullname + ".COOLFolder",  m_folderName);
    }

    ++ispec;

    // -- Prefix.
    if (ispec >= m_corrspecs.size()) {
      REPORT_ERROR(StatusCode::FAILURE)
        << "Malformed corrections spec: missing prefix.";
      return StatusCode::FAILURE;
    }
    tool.prefix = m_corrspecs[ispec++];
    tool.resolved_prefix = tool.prefix;
    if (!tool.prefix.empty())
      m_jos->set (fullname + ".prefix", tool.prefix);

    // -- Put properties in the JO service.
    while (ispec < m_corrspecs.size()) {
      std::string key = m_corrspecs[ispec++];
      if (key.empty()) break;

      if (ispec >= m_corrspecs.size()) {
        REPORT_ERROR(StatusCode::FAILURE)
          << "Malformed corrections spec for tool " << tool.name
          << ": No value for key " << key;
        return StatusCode::FAILURE;
      }

      m_jos->set (fullname + "." + key, m_corrspecs[ispec++]);
    }

    // Done with this tool.
    m_tools.push_back (tool);
    REPORT_MESSAGE(MSG::DEBUG) << "Done parsing for tool" << tool.name;
  }

  // We don't need the property value any more, and it can be pretty long.
  // Clear it out to reclaim memory.
  if (!m_noClearProps) {
    m_jos->set (this->name() + ".CorrSpecs", "");
  }
  m_corrspecs.clear();

  return StatusCode::SUCCESS;
}


/**
 * @brief Parse the supplied @c KeepList property.
 */
StatusCode CaloRunClusterCorrections::parseKeeplist()
{
  for (size_t ikeep = 0; ikeep < m_keeplist.size(); ikeep++) {
    // Get the next tool name from the property.
    std::string toolname = m_keeplist[ikeep++];

    // Check that a following element exists.
    if (ikeep >= m_keeplist.size()) {
      REPORT_ERROR(StatusCode::FAILURE)
        << "Malformed KeepList property.";
      return StatusCode::FAILURE;
    }

    // Find the tool in our vector.
    Tool* tool = nullptr;
    for (size_t itool = 0; itool < m_tools.size(); itool++) {
      if (m_tools[itool].name == toolname) {
        tool = &m_tools[itool];
        break;
      }

      // Tool names are generally like `trcorr_GLOBAL_gam35'.
      // Also try matching on the base part of the name, before
      // the first underscore.
      std::string::size_type pos = m_tools[itool].name.find ('_');
      if (pos != std::string::npos &&
          m_tools[itool].name.substr (0, pos) == toolname)
      {
        tool = &m_tools[itool];
        break;
      }
    }

    if (tool) {
      // Save the requested container name.
      tool->save_container = m_keeplist[ikeep];
    }
    else {
      REPORT_MESSAGE(MSG::WARNING)
        << "Didn't find correction tool " << toolname
        << " while reading KeepList.";
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Create all tools that we can during initialization.
 *        Set up to create remaining tools during a callback.
 */
StatusCode CaloRunClusterCorrections::createTools ATLAS_NOT_THREAD_SAFE /*Binds to callback*/()
{
  // Set to true if creation of any tools is deferred to a callback.
  bool any_cb = false;

  // Loop over tools.
  for (size_t itool = 0; itool < m_tools.size(); itool++) {
    Tool& tool = m_tools[itool];
    const Tool& ctool = tool;
    REPORT_MESSAGE(MSG::DEBUG) << "Start creating tool " << tool.name;
    if (tool.sgkey.empty()) {
      // No pool constants --- we should have been given the class name.
      // Create the tool immediately.
      REPORT_MESSAGE(MSG::DEBUG) << "No Pool constants"; 
      CHECK( makeTool (tool) );
    }
    else if (tool.use_callback) {
      REPORT_MESSAGE(MSG::DEBUG) << "With callback";
      // Reading this tool from COOL.
      any_cb = true;

      // Set up a callback function.
      // The tool will be actually created there.

      // But only register once for each db object!
      if (std::find (m_registeredDBObjects.begin(),
                     m_registeredDBObjects.end(),
                     tool.sgkey) ==
          m_registeredDBObjects.end())
      {
        m_registeredDBObjects.push_back (tool.sgkey);
        // Note: Don't actually register the callback here.
        // The reason is that if initialize() fails later on,
        // then this CaloRunClusterCorrections tool will end up
        // being deleted, leaving a dangling callback reference.
        // (Also note that there's no way to delete a callback
        // once it's been registered.)
        // So defer the callback registration until after all other
        // initialization steps have succeeded.
      }
      else {
        // Already set up the callback.  Just bind the handle.
        tool.use_callback = false;
	REPORT_MESSAGE(MSG::DEBUG) << "Callback already registered";
        CHECK( detStore()->bind (ctool.dbconstants, tool.sgkey) );
      }
    }
    else {
      // Otherwise, configuring directly from the detector store.
      // Read the payload now to find the class name and create the tool.
      REPORT_MESSAGE(MSG::DEBUG) << "Constants from DetStore"; 
      CHECK( detStore()->bind (ctool.dbconstants, tool.sgkey) );
      CHECK( fixPrefix (tool) );
      CHECK( clsnameFromDBConstants (tool) );
      CHECK( makeTool (tool) );
    }
  }

  // Put the corrections in the proper order.
  CHECK( orderCorrections (any_cb) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Register correction callbacks.
 *
 * Note that we cannot return @c FAILURE after the first callback
 * has been registered (see comment in @c createTools).
 */
void
CaloRunClusterCorrections::registerCallbacks ATLAS_NOT_THREAD_SAFE /*Registers callback*/ ()
{
  if (!m_folderName.empty()) { //COOL inline storage

    StatusCode sc=m_coolInlineTool.retrieve(); 
    if (sc.isFailure()) {
      REPORT_ERROR(sc) << "Can't retrieve Blob2ToolConstants Tool";
    }
      

    //std::vector<std::string> sgkeys;
    std::vector<Tool>::const_iterator it=m_tools.begin();
    std::vector<Tool>::const_iterator it_e=m_tools.end();
    for (;it!=it_e;++it) {
      sc=m_coolInlineTool->addFolder(m_folderName,it->sgkey);
      if (sc.isFailure()) {
	REPORT_ERROR(sc) << "Can't add folder to Blob2ToolConstants Tool";
      }
    }
    sc=detStore()->regFcn(&Blob2ToolConstants::fillObjects,
			  &(*m_coolInlineTool),
			  &CaloRunClusterCorrections::updateTools,
			  this,
                          true);
    
    if (sc.isFailure()) {
      REPORT_ERROR(sc)
	<< "Can't register callback for COOL inline storage ";
      //return sc;
    }
  }
  else { //payload stored in POOL file
    for (unsigned int itool = 0; itool < m_tools.size(); itool++) {
      const Tool& ctool = m_tools[itool];
      if (ctool.use_callback) {
	StatusCode sc =
	  detStore()->regFcn(&CaloRunClusterCorrections::updateTools,
			     this,
			     ctool.dbconstants,
			     ctool.sgkey,
                             true);
	if (sc.isFailure()) {
	  // Note that once we've started registering callbacks,
	  // we cannot return FAILURE from initialize() --- otherwise,
	  // we'll be left with a dangling callback.
	  REPORT_ERROR(sc)
	    << "Can't register callback for tool " << ctool.name;
	}
      }
    }
  }
}


/**
 * @brief Actually create a tool.
 * @param tool The tool to create.
 */
StatusCode CaloRunClusterCorrections::makeTool (Tool& tool)
{
  // Release any old tool.
  if (tool.collproc)
    tool.collproc->release();

  // Get the tool.
  CHECK( m_toolsvc->retrieveTool (tool.clsname,
                                  tool.name,
                                  tool.collproc,
                                  this) );

  // Downcast as a other interfaces.
  tool.clusproc = dynamic_cast<CaloClusterProcessor*> (tool.collproc);
  tool.setcellcontname = 
    dynamic_cast<ISetCaloCellContainerName*> (tool.collproc);

  
  REPORT_MESSAGE(MSG::DEBUG) << "Done making tool " << tool.name;

  return StatusCode::SUCCESS;
}


/**
 * @brief DB callback function.
 * @param i (Unused)
 * @param keys List of DB folders with changes.
 *
 * This is called when tool constants read from the DB have changed
 * (or after the DB is accessed for the first time).
 */
StatusCode
CaloRunClusterCorrections::updateTools ATLAS_NOT_THREAD_SAFE /*callbacks*/ (IOVSVC_CALLBACK_ARGS_P( i, keys))
{
  REPORT_MESSAGE(MSG::DEBUG) << "In IOV Callback method updateTools";

  if (!m_folderName.empty()) {
    //COOL-inline case
    if (std::find(keys.begin(),keys.end(),m_folderName)==keys.end()) {
       REPORT_MESSAGE(MSG::DEBUG)
	 << "The cool folder we care about (" << m_folderName<< ") is not in the list of keys. Do nothing." << endmsg;
       return StatusCode::SUCCESS;
    }
    else
       REPORT_MESSAGE(MSG::DEBUG)
	 << "Found cool folder " << m_folderName << " in the list of keys." << endmsg;
  }

    for (size_t itool = 0; itool < m_tools.size(); ++itool) {
      Tool& tool = m_tools[itool];
      const Tool& ctool = m_tools[itool];
      if (!tool.sgkey.empty()) { // && tool.use_callback) {
        // Found a tool that needs updating.

        // Verify that we have the constants.
	//        if (!ctool.dbconstants.isValid()) {
	if (detStore()->retrieve(ctool.dbconstants,ctool.sgkey).isFailure()) {
          REPORT_ERROR(StatusCode::FAILURE)
            << "Constants for tool " << tool.name << " are not valid.";
          return StatusCode::FAILURE;
        }

        // If the class name has changed, recreate the tool.
        // This will also create the tool the first time,
        // since tool.clsname should be blank then.
        if (ctool.dbconstants->clsname() != tool.clsname) {
          CHECK( fixPrefix (tool) );
          CHECK( clsnameFromDBConstants (tool) );
          CHECK( makeTool (tool) );
        }
        else {
          // Otherwise, just update the constants.
          CaloRec::ToolWithConstantsMixin* mixin =
            dynamic_cast<CaloRec::ToolWithConstantsMixin*> (tool.collproc);
          if (mixin)
            CHECK( mixin->processConstantsFromDB (i, keys) );
          else {
            REPORT_ERROR(StatusCode::FAILURE)
              << "Attempted to read constants from database for tool "
              << tool.name << " that does not derive from "
              << "CaloRec::ToolWithConstantsMixin.";
            return StatusCode::FAILURE;
          }
        }// end else create tool
      }//end if found tool to update
    }//end loop over tools

  // Put the corrections in the proper order.
  CHECK( orderCorrections (false) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Initialize the clsname member of the tool structure.
 * @param tool The tool on which to operate.
 *
 * Fetch the class name from the @c ToolConstants structure.
 * If needed, this will expand ${} constructions; these are available
 * to allow tools which use difference C++ classes for different
 * layers/regions to share the same @c ToolConstants structure.
 *
 * ${LAYER} in the class name will be expanded to either `1' or `2',
 * and ${BE} will be expanded to either `b' or `e', depending on the
 * setting of the @c region property.
 */
StatusCode 
CaloRunClusterCorrections::clsnameFromDBConstants ATLAS_NOT_THREAD_SAFE (Tool& tool)
{
  const Tool& ctool = tool;
  const CaloRec::ToolConstants& tc = *ctool.dbconstants;
  tool.clsname = tc.clsname();

  std::string::size_type ipos = 0;
  while ((ipos = tool.clsname.find ('$', ipos)) != std::string::npos) {
    if (tool.clsname.substr (ipos, 8) == "${LAYER}") {
      int region;
      CHECK( getConstant (tool, "region", region) );
      const char* out = "X";
      switch (region) {
      case CaloClusterCorrectionCommon::EMB1:
      case CaloClusterCorrectionCommon::EME1:
        out = "1";
        break;
      case CaloClusterCorrectionCommon::EMB2:
      case CaloClusterCorrectionCommon::EME2:
        out = "2";
        break;
      default:
        REPORT_ERROR(StatusCode::FAILURE)
          << "Bad region code expanding ${LAYER} for tool " << tool.name;
        return StatusCode::FAILURE;
      }
      tool.clsname.replace (ipos, 8, out);
    }
    else if (tool.clsname.substr (ipos, 5) == "${BE}") {
      int region;
      CHECK( getConstant (tool, "region", region) );
      const char* out = "X";
      switch (region) {
      case CaloClusterCorrectionCommon::EMB2:
      case CaloClusterCorrectionCommon::EMB1:
        out = "b";
        break;
      case CaloClusterCorrectionCommon::EME1:
      case CaloClusterCorrectionCommon::EME2:
        out = "e";
        break;
      default:
        REPORT_ERROR(StatusCode::FAILURE)
          << "Bad region code expanding ${BE} for tool " << tool.name;
        return StatusCode::FAILURE;
      }
      tool.clsname.replace (ipos, 5, out);
    }
    else
      ++ipos;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Initialize @c resolved_prefix for a tool.
 * @param tool The tool on which to operate.
 *
 * If the prefix that was specified at configuration time does not
 * exist in the @c ToolConstants structure, use instead the longest
 * matching prefix.
 */
StatusCode CaloRunClusterCorrections::fixPrefix ATLAS_NOT_THREAD_SAFE (Tool& tool)
{
  const Tool& ctool = tool;
  using Maptype = CaloRec::ToolConstants::Maptype;
  const Maptype& map = ctool.dbconstants->map();
  if (map.empty()) {
    // If there are no constants, then the prefix setting can't matter...
    return StatusCode::SUCCESS;
  }

  std::string prefix = tool.prefix;

  Maptype::const_iterator it = map.lower_bound (prefix);

  if (it == map.end() || it->first.substr (0, prefix.size()) != prefix) {
    // Prefix not found.  Try to find a match among the existing prefixes.

    // Strip a trailing dot and region code off the prefix.
    std::string tail;
    std::string::size_type len = prefix.size();
    if (len > 0 && prefix[len-1] == '.') {
      tail = ".";
      prefix.erase (len-1);
      --len;
    }

    if (len >= 2) {
      std::string tmp = prefix.substr (len-2, 2);
      if (tmp == "b1" || tmp == "b2" || tmp == "e1" || tmp == "e2") {
        tail = tmp + tail;
        prefix.erase (len-2);
        len -= 2;
      }
    }

    // Extract matching prefixes; find the longest match.
    int longest_len = -1;
    std::string best_match;
    for (it = map.begin(); it != map.end(); it++) {
      std::string::size_type ipos = it->first.find (tail);
      if (ipos != std::string::npos) {
        int this_len = match_len (it->first, prefix);
        if (this_len > longest_len) {
          longest_len = this_len;
          best_match = it->first.substr(0, ipos);
        }
      }
    }

    if (longest_len < 0) {
      REPORT_ERROR(StatusCode::FAILURE)
        << "No matches for prefix " << tool.prefix
        << " for tool " << tool.name;
      return StatusCode::FAILURE;
    }

    prefix = best_match + tail;

    // Warn if the match isn't long enough.
    if (longest_len < 3) {
      REPORT_MESSAGE(MSG::WARNING)
        << "No good match for prefix " << tool.prefix
        << " for tool " << tool.name
        << "; using " << prefix;
    }

    REPORT_MESSAGE(MSG::DEBUG)
      << "For tool " << tool.name << " using prefix " << prefix
      << " instead of requested prefix " << tool.prefix;
  }

  // Set the prefix.
  std::string fullname = this->name() + "." + tool.name;
  m_jos->set (fullname + ".prefix", prefix);
  tool.resolved_prefix = prefix;

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill in @c m_toolorder to run corrections in the proper order.
 */
StatusCode
CaloRunClusterCorrections::orderCorrections ATLAS_NOT_THREAD_SAFE (bool allowMissing)
{
  // Clear out any previous setting.
  m_toolorder.clear();
  m_toolorder.reserve (m_tools.size());

  // Loop over tools.
  for (size_t itool = 0; itool < m_tools.size(); itool++) {
    Tool& tool = m_tools[itool];
    const Tool& ctool = tool;
    if (m_preserveOrder) {
      // If PreserveOrder was requested, execute tools in the exact order
      // in which they were listed.  Don't check for dummies either.
      tool.order = itool;
      m_toolorder.push_back (itool);
    }
    else if (tool.sgkey.empty()) {
      // Tool is being initialized from JO.  Find the order property from JOS.
      const std::string fullname = this->name() + "." + tool.name;
      tool.order = std::stoi(m_jos->get(fullname + ".order", "-1"));

      // It's an error if it wasn't set.
      if (tool.order == -1) {
        REPORT_ERROR(StatusCode::FAILURE)
          << "Tool " << tool.name
          << " initialized from JO has no `order' property set.";
        return StatusCode::FAILURE;
      }

      // We want to sort this tool.
      m_toolorder.push_back (itool);
    }
    else if (tool.collproc) {
      // Initializing from detector store or cool.
      // First check to see if this is a dummy.
      int isDummy;
      CHECK( getConstant (ctool, "isDummy", isDummy) );
      if (isDummy != 0) {
        // It's a dummy.  Skip this tool.
        tool.order = -1;
      }
      else {
        // Get the order parameter and sort this tool.
        int order;
        CHECK( getConstant (ctool, "order", order) );
        tool.order = order;
        m_toolorder.push_back (itool);
      }
    }
    else if (!allowMissing) {
      // We don't have any valid constants for this tool.
      REPORT_ERROR(StatusCode::FAILURE)
        << "Tool " << tool.name << " has no valid constants.";
      return StatusCode::FAILURE;
    }
  }

  // Put the list of tool indices in the proper order.
  std::sort (m_toolorder.begin(),
             m_toolorder.end(),
             ToolorderSort (m_tools));

  // Make a debugging dump of the results of the sort.
  std::string names;
  for (size_t ii = 0; ii < m_toolorder.size(); ii++)
    names += m_tools[m_toolorder[ii]].name + " ";
  REPORT_MESSAGE(MSG::DEBUG)
    << "Sorted tools: " << names;

  return StatusCode::SUCCESS;
}


/**
 * @brief Sorting helper: constructor.
 * @param the_tools The vector of Tool structs.
 */
CaloRunClusterCorrections::ToolorderSort::ToolorderSort
  (const std::vector<Tool>& the_tools)
  : tools (the_tools)
{
}


/**
 * @brief Sorting helper: comparison.
 * @param a Index of first tool to compare.
 * @param b Index of second tool to compare.
 */
bool CaloRunClusterCorrections::ToolorderSort::operator() (int a, int b) const
{
  return tools[a].order < tools[b].order;
}


/**
 * @brief Helper to get integer constant from a @c ToolConstants structure.
 * @param tool The tool for which to retrieve the constant.
 * @param pname The constant name (without prefix).
 * @param out[out] The retrieved parameter.
 */
StatusCode
CaloRunClusterCorrections::getConstant ATLAS_NOT_THREAD_SAFE (const Tool& tool,
                                                              const std::string& pname,
                                                              int& out)
{
  if (!tool.dbconstants.isValid()) {
    REPORT_ERROR(StatusCode::FAILURE)
      << "DB Constants for tool " << tool.name << " are not valid.";
    return StatusCode::FAILURE;
  }

  try {
    const CxxUtils::Arrayrep& rep =
      tool.dbconstants->getrep (name(), tool.resolved_prefix + pname);
    const CxxUtils::Array<0> arr (rep);
    out = static_cast<int> (arr);
  }
  catch (const std::exception& e) {
    REPORT_ERROR(StatusCode::FAILURE)
      << "Caught exception retrieving property "
      << tool.resolved_prefix + pname
      << " for tool " << tool.name << ": " << e.what();
    std::ostringstream oss;
    tool.dbconstants->writeConstants (oss, tool.name);
    REPORT_ERROR(StatusCode::FAILURE)
      << " " << oss.str();
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}



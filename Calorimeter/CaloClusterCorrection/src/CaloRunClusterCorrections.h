// This file's extension implies that it's C, but it is really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* @file  CaloRunClusterCorrections.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2009
 * @brief Meta-tool to construct and run a list of correction tools.
 */


#ifndef CALORUNCLUSTERCORRECTIONS_H
#define CALORUNCLUSTERCORRECTIONS_H


#include "CaloRec/CaloClusterProcessor.h"
#include "CaloConditions/ToolConstants.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
#include "StoreGate/DataHandle.h" 
#include "AthenaKernel/IOVSvcDefs.h"
#include <vector>
#include <string>
#include <set>

#include "Gaudi/Interfaces/IOptionsSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloRec/Blob2ToolConstants.h"

#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "TileConditions/TileBadChannels.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "CxxUtils/checker_macros.h"

class MsgStream;
class IToolSvc;


/*
 * @brief Meta-tool to construct and run a list of correction tools.
 *
 * The cluster corrections consist of a set of individual correction
 * tools, each with a specific version, and run in a specific order.
 * We want to be able to configure this set of corrections entirely
 * from the database, given only only a global database tag.  Since the
 * set of tools run, their versions (including the proper C++ class to use),
 * and the order in which they run all can depend on the global correction
 * version, we cannot create the individual correction tools from
 * job options.  Instead, we must be able to defer that until the
 * database is accessed.
 *
 * This is handled by this `metatool'.  Rather than create individual
 * correction tools, the job configuration creates an instance of this
 * tool (which is itself a correction tool).  The job configuration
 * supplies this tool with a description of how to determine which
 * tools to create; in general, this will involve looking in the database.
 * During initialization, this tool will then figure out which tools
 * need to be run and will create them.
 *
 * Consider first the case of reading from the database.  We get a list
 * of all possible correction tools that may be used; for each of these
 * we get the corresponding database folder and the prefix to use to find
 * this tool's properties in the @c ToolConstants objects (as well as any
 * property overrides).
 *
 * For each tool, we look up the its data in the corresponding database
 * folder; this will be a @c ToolConstants object.  Now, the first issue
 * is that in order to know in which folders we need to look, all possible
 * correction tools need to be specified during job configuration; but
 * in general, only some of those tools will be used for any given tag.
 * For the folders that aren't used for a tag, that tag will be bound
 * to a dummy @c ToolConstants object; i.e., it will have the @c isDummy
 * property set to @c True.  In this way, the unused tools can be skipped.
 *
 * We also need to know the order in which to execute tools.  Each tool
 * has an `order' property, which is an integer.  Tools should be executed
 * in increasing order of this parameter.
 *
 * In addition to being read from the database, tools may be initialized
 * directly from the detector store (i.e., from a pool file) or entirely
 * from job options.
 *
 * The needed configuration is communicated to this tool via the @c CorrSpecs
 * property.  This is a vector of strings with this layout:
 *   Each tool is defined by:
 *     TOOLNAME LOCATION PREFIX PARM-NAME PARM-VAL ... ""
 *
 *   First element is the name of the tool to create.
 *
 *   For the second element:
 *     - If it starts with `+', it should be followed by the C++
 *       name of the class to create.
 *     - If it starts with `@', it should be followed by the SG
 *       detector store key, to be read directly from pool
 *       (i.e., don't use callbacks).
 *     - Otherwise, it should be a COOL folder name.
 *
 *   Third element is the prefix for this tool's constants in the
 *   ToolConstants structure.  (Can be blank if no ToolConstants
 *   structure is being used.)
 *
 *   This is then followed by a set of property name/value pairs.
 *   These override anything read from the detector store.
 *   The list is terminated with an empty string.
 *
 *   Such lists for all corrections are concatenated.
 *
 * Additional properties of interest include:
 *
 *   @c KeepList This allows one to preserve in SG intermediate results
 *               from running the corrections.  This is a vector of strings.
 *               These come in pairs; the first of a pair gives a correction
 *               tool name and the second a SG container name.  Before the
 *               named tool executes, the cluster list will be saved to SG
 *               with the specified name.
 *
 *               In addition to full tool names, which are usually of the
 *               form `larupdate_GLOBAL_gam37', one can also specify just
 *               the base part of the tool name; that is, before the
 *               first underscore.
 *
 *               Note that this only operates when the correction is called
 *               for a list of clusters; it won't work if the correction
 *               is called for individual clusters.
 *
 *   @c PreserveOrder If true, corrections will be executed strictly in the
 *                    order in which they're listed in @c CorrSpecs
 *                    (ignoring the @c order properties of the tools).
 *
 * Service/tool names used may also be overridden through properties.
 */
class CaloRunClusterCorrections
  : public CaloClusterProcessor,
    public ISetCaloCellContainerName
{
public:
  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  CaloRunClusterCorrections (const std::string& type,
                             const std::string& name,
                             const IInterface* parent);


  /// Standard initialize method.
  virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE /*Can Register callbacks but no need to be thread safe*/ () override;


  /// Standard finalize method.
  virtual StatusCode finalize() override;

  using CaloClusterProcessor::execute;

  /**
   * @brief Execute on a single cluster.
   * @param The cluster to process.
   * @param ctx The event context.
   *
   * Warning: Any defined tools that cannot process single clusters
   * (only cluster collections) will be skipped!
   */
  virtual StatusCode execute (const EventContext& ctx,
                              xAOD::CaloCluster* cluster) const override;


  /**
   * @brief Execute on an entire collection of clusters.
   * @param The container of clusters.
   * @param ctx The event context.
   *
   * This will iterate over all the clusters in @c collection
   * and call @c execute on each one individually.
   */
  virtual StatusCode execute (const EventContext& ctx,
                              xAOD::CaloClusterContainer* collection) const override;


  /**
   * @brief Change the name of the CaloCellContainer used by this tool.
   * @param name The new container name.
   */
  virtual StatusCode setCaloCellContainerName (const std::string& name) override;


private:
  /// Internal structure to hold information about a correction tool.
  struct Tool {
    // Not thread-safe due to the use of DataHandle.
    Tool() ATLAS_CTORDTOR_NOT_THREAD_SAFE = default;
    Tool(const Tool&) ATLAS_CTORDTOR_NOT_THREAD_SAFE = default;
    ~Tool() ATLAS_CTORDTOR_NOT_THREAD_SAFE = default;

    /// The name of the tool.
    std::string name;

    /// Key of this tool in the detector store.
    /// Empty if being initialized from JO.
    std::string sgkey;

    /// Name of the C++ class for this tool.
    std::string clsname;

    /// Prefix to use in the @c ToolConstants structure,
    /// as specified in @c CorrSpecs.
    std::string prefix;

    /// Actual @c ToolConstants prefix to use (after resolving the request
    /// among those prefixes actually existing).
    std::string resolved_prefix;

    /// Order in which this tool should be executed.
    /// Set to -1 for a dummy tool.
    int order;

    /// True if we should set up a callback for this tool.
    bool use_callback;

    /// If non-empty, copy cluster list and save to SG with this name
    /// before running this tool.
    std::string save_container;

    /// Handle to the constants structure.
    // Note1: Don't access this until the vector<Tool> is complete.
    // Note2: This must always be accessed via a const reference.
    DataHandle<CaloRec::ToolConstants> dbconstants;

    /// Pointer to the correction tool.
    CaloClusterCollectionProcessor* collproc;

    /// Pointer to the correction tool as a @c CaloClusterProcessor,
    /// or 0 if that interface isn't supported by this tool.
    CaloClusterProcessor* clusproc;

    /// Pointer to the correction tool as an @c ISetCaloCellContainerName,
    /// or 0 if that interface isn't supported by this tool.
    ISetCaloCellContainerName* setcellcontname;
  };


  /// Helper for ordering tools.
  struct ToolorderSort {
    ToolorderSort (const std::vector<Tool>& the_tools);
    bool operator() (int a, int b) const;
    const std::vector<Tool>& tools;
  };


  /**
   * @brief Parse the supplied correction specification and create
   *        the Tools vector.  Do not actually create the tools yet.
   */
  StatusCode parseCorrspecs ATLAS_NOT_THREAD_SAFE ();


  /**
   * @brief Parse the supplied @c KeepList property.
   */
  StatusCode parseKeeplist();


  /**
   * @brief Create all tools that we can during initialization.
   *        Set up to create remaining tools during a callback.
   */
  StatusCode createTools ATLAS_NOT_THREAD_SAFE /*Binds to CallBack*/ ();


  /**
   * @brief Register correction callbacks.
   *
   * Note that we cannot return @c FAILURE after the first callback
   * has been registered (see comment in @c createTools).
   */
  void registerCallbacks ATLAS_NOT_THREAD_SAFE /*Registers callback*/ ();


  /**
   * @brief Actually create a tool.
   * @param tool The tool to create.
   */
  StatusCode makeTool (Tool& tool);


  /**
   * @brief DB callback function.
   * @param i (Unused)
   * @param keys List of DB folders with changes.
   *
   * This is called when tool constants read from the DB have changed
   * (or after the DB is accessed for the first time).
   */
  StatusCode updateTools ATLAS_NOT_THREAD_SAFE /*callbacks*/ (IOVSVC_CALLBACK_ARGS);   



  /**
   * @brief Initialize the clsname member of the tool structure.
   * @param tool The tool on which to operate.
   *
   * Fetch the class name from the @c ToolConstants structure.
   * If needed, this will expand ${} constructions; these are available
   * to allow tools which use difference C++ classes for different
   * layers/regions to share the same @c ToolConstants structure.
   *
   * ${LAYER} in the classname will be expanded to either `1' or `2',
   * and ${BE} will be expanded to either `b' or `e', depending on the
   * setting of the @c region property.
   */
  StatusCode clsnameFromDBConstants ATLAS_NOT_THREAD_SAFE (Tool& tool);


  /**
   * @brief Initialize @c resolved_prefix for a tool.
   * @param tool The tool on which to operate.
   *
   * If the prefix that was specified at configuration time does not
   * exist in the @c ToolConstants structure, use instead the longest
   * matching prefix.
   */
  StatusCode fixPrefix ATLAS_NOT_THREAD_SAFE (Tool& tool);


  /**
   * @brief Fill in @c m_toolorder to run corrections in the proper order.
   */
  StatusCode orderCorrections ATLAS_NOT_THREAD_SAFE (bool allowMissing);


  /**
   * @brief Helper to get integer constant from a @c ToolConstants structure.
   * @param tool The tool for which to retrieve the constant.
   * @param pname The constant name (without prefix).
   * @param out[out] The retrieved parameter.
   */
  StatusCode getConstant ATLAS_NOT_THREAD_SAFE (const Tool& tool,
                                                const std::string& pname,
                                                int& out);

  //-----------
  // Properties

  /// Correction specifications.  See above for format.
  std::vector<std::string> m_corrspecs;

  /// List of intermediate results to preserve.  See above for format.
  std::vector<std::string> m_keeplist;

  /// If true, execute tools exactly in the specified order
  /// (ignore order information from DB).
  bool m_preserveOrder;

  /// If true, don't clear out the values of long properties.
  bool m_noClearProps;

  /// The job options service.
  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> m_jos;

  /// The tool service.
  ServiceHandle<IToolSvc> m_toolsvc;

  /// Information about tools we're managing.
  std::vector<Tool> m_tools;

  /// Vector of indices into the @c m_tools list.
  /// The tools will be executed in the order of appearance in this list.
  std::vector<int> m_toolorder;

  /// List of folders for which we've already registered callbacks.
  std::vector<std::string> m_registeredDBObjects;

  /// COOL folder name in case of COOL inline storage
  std::string  m_folderName;

  /// AlgTool to convert COOL inline data into ToolConstants objects
  ToolHandle<Blob2ToolConstants> m_coolInlineTool;

  // FIXME: Dependencies don't get propagated from dynamically-created
  // correction tools.  Hardcode this dependency as a workaround.
  SG::ReadCondHandleKey<CaloAffectedRegionInfoVec> m_affKey{this,
     "LArAffectedRegionKey", "LArAffectedRegionInfo", "SG key for affected regions cond object"};
  SG::ReadCondHandleKey<LArBadChannelCont> m_LArBCKey { this, 
      "LArBadChanKey","LArBadChannel","SG Key of LArBadChannelCont object"};
  SG::ReadCondHandleKey<TileBadChannels> m_TileBCKey{this,
      "TileBadChanKey", "TileBadChannels", "Input Tile bad channel status"};

};


#endif // not CALORUNCLUSTERCORRECTIONS_H

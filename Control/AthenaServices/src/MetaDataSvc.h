/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_METADATASVC_H
#define ATHENASERVICES_METADATASVC_H

/** @file MetaDataSvc.h
 *  @brief This file contains the class definition for the MetaDataSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include <map>
#include <set>
#include <typeinfo>

#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IMetaDataSvc.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "Gaudi/Property.h"  // no forward decl: typedef
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/IFileMgr.h"  // for FILEMGR_CALLBACK_ARGS
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declarations
class IAddressCreator;
class StoreGateSvc;
class IAlgTool;
class OutputStreamSequencerSvc;

namespace Io {
  class FileAttr;
}

static const InterfaceID IID_MetaDataSvc("MetaDataSvc", 1, 0);

// forward declaration to allow making SvcFactory a friend
template <class TYPE>
class SvcFactory;

/**
 * @brief Manages the content of the metadata stores
 *
 * This service orchestrates the reading of metadata from input files and its
 * propogation to the output with the help of the IMetaDataTool interfaces in
 * @c m_metaDataTools. The MetaDataSvc
 * reacts to BeginInputFile and EndInputFile incidents. It relies on the
 * AthenaPoolCnvSvc (or whatever persistnecy service) to call the the
 * appropriate
 * prepareOutput method when commiting the MetaDataStore content to the output
 * file.
 *
 * On BeginInputFile it clears the InputMetaDataStore and populates it with
 * content from the new input file. The @c beginInputFile methods of all tools
 * are called to allow required actions on the metadata objects.
 *
 * On EndInputFile the MetaDataSvc calls the @c endInputFile methods of all
 * the interfaces in @c m_metaDataTools .
 *
 * In jobs with multiple parallel output streams (SharedWriter) the metadata
 * content from multiple streams is put into the output MetaDataStore. To do
 * this the The MetaDataSvc accesses the memory shared with the output streams,
 * and adds the content of their MetaDataStore to the shared
 * InputMetaDataStore. This is done by the client calling @c shmProxy for each
 * object to be added to the MetaDataStore. Afterwards the client *must* also
 * fire BeingInputFile and EndInputFile incidents to allow the MetaDataTools
 * to move their objects from the InputMetaDataStore to the MetaDataStore
 * taking care of possible merges.
 *
 * Example configuration:
 * @code{.py}
 *     ca = ComponentAccumulator()
 *     ca.addService(CompFactory.StoreGateSvc("MetaDataStore"))
 *     ca.addService(CompFactory.StoreGateSvc("InputMetaDataStore"))
 *     metaDataSvc = CompFactory.MetaDataSvc("MetaDataSvc")
 *     ca.addService(
 *         CompFactory.ProxyProviderSvc(ProviderNames=["MetaDataSvc"]))
 *     metaDataSvc.MetaDataTools += [someMetaDataTool]
 * @endcode
 *
 * @class MetaDataSvc
 * @see IMetaDataTool
 * @see AthenaPoolCnvSvc
 **/
class MetaDataSvc : public ::AthService,
                    virtual public IAddressProvider,
                    virtual public IIncidentListener,
                    virtual public IMetaDataSvc,
                    virtual public IIoComponent {
  // Allow the factory class access to the constructor
  friend class SvcFactory<MetaDataSvc>;  // <-- obsolete, remove

 public:  // Constructor and Destructor
  /// Standard Service Constructor
  MetaDataSvc(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~MetaDataSvc();

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_MetaDataSvc; }

 public:  // Non-static members
  /**
   * \ingroup GaudiService Methods implementing the Gaudi state machine
   * @{
   */
  /** initialize the metadata service at the beginning of the job*/
  virtual StatusCode initialize() override;
  /** stop the metadata service */
  virtual StatusCode stop() override;
  /** finalize the metadata service at the end of the job */
  virtual StatusCode finalize() override;
  /**@}*/
  
  /**
   * \ingroup InputFileMetaData Methods implementing file transitions
   * @{
   */
  /** Fills the input metadata store with the content of the new input file.
   * Once the GUID and name of the input file are determined from the incident
   * this function blocks stopping the MetaDataSvc by setting
   * @c m_allowMetaDataStop to false. Then attempts first to clear the input
   * metadata store and prepare it for the metadata tools by calling
   * @c initInputMetaDataStore. The loops over all MetaDataTool interfaces
   * in @c m_metaDataTools and calls their beginInputFile implementations.
   *
   * @see initInputMetaDataStore()
   * @see m_metaDataTools
   * @param inc The BeginInputFile incident fired on opening a new file
   * @return FAILURE when @c inc is not a file incident, the input metadata
   *         store cannot be set up, or a MetaDataTool fails. Otherwise
   *         SUCCESS.
   */
  virtual StatusCode newMetadataSource(const Incident& /* inc */);

  /** Allows metadata tools to act before an input file is closed.
   * Grabs the file GUID then calls the the endInputFile method of all
   * metadata tools in @c m_metaDataTools providing the GUID as argument.
   *
   * @param inc is a file incident fired before closing an input file
   * @return SUCCESS unless a MetaDataTool fails, then FAILURE
   */
  virtual StatusCode retireMetadataSource(const Incident& /* inc */);
  /**@}*/

  /** Makes the metadata store ready for output.
   * Calls the metaDataStop method of all MetaDataTool interfaces in
   * m_metaDataTool. Then release the metadata tools. This implementation
   * assumes SharedWriter is *not* being used.
   *
   * @return SUCCESS unless a MetaDataTool fails, then FAILURE
   */
  virtual StatusCode prepareOutput();

  /**
   * \ingroup SupportSharedWriter Support writing parallel streams
   * @{
   */
  /** Makes the metadata store ready for output with SharedWriter
   * Calls the metaDataStop method of all MetaDataTool interfaces in
   * m_metaDataTool. Then release the metadata tools.
   *
   * @param outputName is the name of the output file being written
   * @return SUCCESS unless a MetaDataTool fails, then FAILURE
   */
  virtual StatusCode prepareOutput(const std::string& outputName);

  /** Loads file metadata from memory shared between streams in SharedWriter
   * This function clears the input and output metadata stores. Then calls
   * @c addProxyToInputMetaDataStore using @c filename as argument.
   * @see addProxyToInputMetaDataStore()
   * @param filename stream token with "[NUM=n]" appended, n is the stream number
   * @return FAILURE if any step isn't successful, else SUCCESS
   */
  virtual StatusCode shmProxy(const std::string& filename) override;
  /**@}*/

  /** Provide access to the interfaces implemented by this service */
  StatusCode queryInterface(const InterfaceID& riid,
                            void** ppvInterface) override;

  /**
   * \ingroup IAddressProvider IAddressProvider implementation
   * @{
   */
  /** does nothing.
   * @param storeID [IN] store ID, this function only preloads detector store
   *                     addresses.
   * @param tads [OUT] list of the transient addresses which were preloaded.
   * @return SUCCESS
   */
  StatusCode preLoadAddresses(StoreID::type /* storeID */,
                              IAddressProvider::tadList& /* tads */) override {
    return StatusCode::SUCCESS;
  }

  /** loads addresses of versioned metadata object.
   * loops over all versions of the @c DataHeader and adds addresses of objects
   * from @c DataHeaderElements to the @c tads with a version corresponding to
   * version number of the DataHeader that contained them.
   * @see ObjectWithVersion
   * @see DataHeader
   * @param storeID [IN] the identity of the store
   * @param tads [OUT] list of addresses to objects from versions keys
   * @returns FAILURE the DataHeader could not be retrieved, else SUCCESS
   */
  StatusCode loadAddresses(StoreID::type storeID,
                           IAddressProvider::tadList& tads) override;

  /** always fails
   * @param storeID [IN] the identity of the store
   * @param tad [IN] transient address to be updated
   * @param ctx [IN] current event's context information
   * @return FAILURE
   */
  StatusCode updateAddress(StoreID::type /* storeID */,
                           SG::TransientAddress* /* tad */,
                           const EventContext& /* ctx */) override {
    return StatusCode::FAILURE;
  }
  /**@}*/

  /** react to file incidents.
   * calls @c newMetadataSource on BeginInputFile and @c retireMetadataSource
   * on EndInputFile. Does 
   * @param incident is expected to be BeginInputFile or EndInputFile
   */
  virtual void handle(const Incident& incident) override;

  /// Transition output metadata file - fire MeteDataStop incident to transition
  /// OutputStream
  StatusCode transitionMetaDataFile();

  /** Implements IIoComponent interface
   * sets m_outputPreprared to false and prints some information.
   * @return SUCCESS
   * @see IIoComponent
   */
  virtual StatusCode io_reinit() override;

  StatusCode rootOpenAction(FILEMGR_CALLBACK_ARGS);

  virtual StoreGateSvc* outputDataStore() const override final {
    return &*m_outputDataStore;
  }

  virtual const std::string currentRangeID() const override final;

  CLID remapMetaContCLID(const CLID& item_id) const;

  class ToolLockGuard {
   public:
    ToolLockGuard(const MetaDataSvc& mds) : m_mds(mds) { m_mds.lockTools(); }
    ~ToolLockGuard() { m_mds.unlockTools(); }
    ToolLockGuard(const ToolLockGuard&) = delete;
    void operator=(const ToolLockGuard&) = delete;

   private:
    const MetaDataSvc& m_mds;
  };

  void lockTools() const;
  void unlockTools() const;

  void recordHook(const std::type_info&) override;
  void removeHook(const std::type_info&) override;

 private:
  /** Adds a proxy for a metadata object to the input metadata store.
   * The file, class, container, key, and stream number of the metadata object
   * are parsed from the provided string token. An opaque address for the
   * object is generated from the parsed information and recorded in the
   * input metadata store under the key name. If the key contains 'Aux.'
   * it is linked to a IConstAuxStore.
   *
   * Note that the MetaDataTool corresponding to the object is looked up via
   * classID and created if not found in m_metaDataTools. Some special handling
   * for the EventFormat, EventStreamInfo, and FileMetaData tools should ensure
   * proper propogation ---this part seems fragile.
   *
   * This function may called directly or when a BeginInputFile incident is
   * fired.
   *
   * \addtogroup SupportSharedWriter
   * @param tokenStr is the string representation of a @c Token of a metadata
   *                 object
   * @returns SUCCESS unless the proxy could not be generated or added or the
   *                  tool could not be found or set up
   */
  StatusCode addProxyToInputMetaDataStore(const std::string& tokenStr);
  /// Initialize input metadata store - can be called directly or via
  /// BeginInputFile incident
  StatusCode initInputMetaDataStore(const std::string& fileName);

 private:  // data
  ServiceHandle<StoreGateSvc> m_inputDataStore;
  ServiceHandle<StoreGateSvc> m_outputDataStore;
  ServiceHandle<IAddressCreator> m_addrCrtr;
  ServiceHandle<IFileMgr> m_fileMgr;
  ServiceHandle<IIncidentSvc> m_incSvc;
  ServiceHandle<OutputStreamSequencerSvc> m_outSeqSvc;
  ServiceHandle<IClassIDSvc> m_classIDSvc{"ClassIDSvc", name()};

  long m_storageType;
  bool m_clearedInputDataStore;
  bool m_clearedOutputDataStore;
  bool m_allowMetaDataStop;
  bool m_outputPreprared;
  std::map<std::string, CLID> m_persToClid;
  std::map<CLID, std::string> m_toolForClid;
  std::map<std::string, std::string> m_streamForKey;

  std::set<CLID> m_handledClasses;

 private:  // properties
  /// MetaDataContainer, POOL container name for MetaData.
  StringProperty m_metaDataCont;
  /// MetaDataTools, vector with the MetaData tools
  ToolHandleArray<IMetaDataTool> m_metaDataTools;
};

#endif

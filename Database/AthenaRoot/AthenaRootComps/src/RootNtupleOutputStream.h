// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAROOTCOMPS_ROOTNTUPLEOUTPUTSTREAM_H
#define ATHENAROOTCOMPS_ROOTNTUPLEOUTPUTSTREAM_H

// STL include files
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>

// fwk includes
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/FilteredAlgorithm.h"

// forward declarations
template <class ConcreteAlgorithm> class AlgFactory;
class IClassIDSvc;
class StoreGateSvc;
class IAthenaOutputStreamTool;
class IAthenaOutputTool;

namespace SG {
   class DataProxy;
   class IFolder;
   class FolderItem;
}

namespace Athena {

/** @class Athena::RootNtupleOutputStream
   * @brief algorithm that marks for write data objects in SG
   * 
   * @author binet@cern.ch
   * $Id: RootNtupleOutputStream.h 500674 2012-05-14 21:36:37Z gemmeren $
   */
class RootNtupleOutputStream 
  : virtual public IIncidentListener,
            public FilteredAlgorithm
{
  friend class AlgFactory<Athena::RootNtupleOutputStream>;

public:
  typedef std::vector<SG::DataProxy*>     Items;

protected:
  /// handle to the @c StoreGateSvc store where the data we want to
  /// write out resides
  ServiceHandle<StoreGateSvc> m_dataStore;

  /// Name of the persistency service capable to write data from the store
  std::string              m_persName;
  /// Name of the OutputStreamTool used for writing
  StringProperty           m_writingTool;
  /// Name of the output file
  std::string              m_outputName;
  /// tag of processing stage:
  StringProperty           m_processTag;
   
  typedef ServiceHandle<IClassIDSvc> IClassIDSvc_t;
  IClassIDSvc_t m_pCLIDSvc;
   
  /// Vector of item names
  StringArrayProperty      m_itemList;
  /// Collection of objects beeing selected
  IDataSelector            m_objects;
  /// Number of events written to this output stream
  int                      m_events;
  /// set to true to force read of data objects in item list
  bool m_forceRead;
  /// set to true to allow data objects being copied persistent to persistent (without SG retrieve).
  bool m_persToPers;
  std::vector<unsigned int> m_exemptPersToPers;
  /// set to true to allow defaults being provided for non-existent data objects.
  bool m_provideDef;
  /// set to true to trigger streaming of data on execute()
  bool m_writeOnExecute;
  /// set to true to trigger streaming of data on finalize()
  bool m_writeOnFinalize;
  /// set to write out everything from input DataHeader
  bool m_itemListFromTool;

  /// dynamic output itemlist: 
  ///   if enabled rediscover object list to be written out at each event
  ///   otherwise: reuse the one from the first event.
  bool m_dynamicItemList;

  /// list of selected proxies.
  std::vector<SG::FolderItem> m_selection;

  /// pointer to AthenaOutputStreamTool
  ToolHandle<IAthenaOutputStreamTool> m_streamer;
  /// vector of AlgTools that that are executed by this stream
  ToolHandleArray<IAthenaOutputTool> m_helperTools;

public:
  typedef std::vector<std::pair<std::string, std::string> > TypeKeyPairs;
  
  /// Standard algorithm Constructor
  RootNtupleOutputStream(const std::string& name, ISvcLocator* pSvcLocator); 
  /// Standard Destructor
  virtual ~RootNtupleOutputStream();

  /// \name implement IAlgorithm
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();
  //@}
  /// Stream the data
  virtual StatusCode write();

private:
  /// Clear list of selected objects
  void clearSelection();
  /// Collect data objects for output streamer list
  void collectAllObjects();
  /// Return the list of selected objects
  IDataSelector* selectedObjects() {
    return &m_objects;
  }
  /// Incident service handle listening for LastInputFile
  void handle(const Incident& incident);
};

} //> ns Athena

#endif // ATHENAROOTCOMPS_ROOTNTUPLEOUTPUTSTREAM_H

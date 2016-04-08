/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLOUTPUTSTREAMTOOL_H
#define ATHENAPOOLOUTPUTSTREAMTOOL_H
/** @file AthenaPoolOutputStreamTool.h
 *  @brief This is the AthenaPool version of AthenaOutputStreamTool.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaKernel/IAthenaOutputStreamTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class IAthenaPoolCnvSvc;
class IClassIDSvc;
class StoreGateSvc;
class DataHeader;
namespace SG {
   class IFolder;
}

/** @class AthenaPoolOutputStreamTool
 *  @brief This is the AthenaPool version of AthenaServices/AthenaOutputStreamTool.
 **/
class AthenaPoolOutputStreamTool : public ::AthAlgTool, virtual public IAthenaOutputStreamTool {
public:
   /// Standard AlgTool Constructor
   AthenaPoolOutputStreamTool(const std::string& type,
	   const std::string& name,
	   const IInterface* parent);
   /// Destructor
   virtual ~AthenaPoolOutputStreamTool();

   /// AthAlgTool Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();

   /// Specify which data store and conversion service to use
   /// and whether to extend provenence
   ///   Only use if one wants to override jobOptions
   StatusCode connectServices(const std::string& dataStore,
	   const std::string& cnvSvc,
	   bool extendProvenenceRecord);

   /// Connect to the output stream
   ///   Must connectOutput BEFORE streaming
   ///   Only specify "outputName" if one wants to override jobOptions
   StatusCode connectOutput(const std::string& outputName);

   /// Commit the output stream after having streamed out objects
   ///   Must commitOutput AFTER streaming
   StatusCode commitOutput();

   /// Finalize the output stream after the last commit, e.g. in
   /// finalize
   StatusCode finalizeOutput();

   /// Stream out objects. Provide vector of typeName/key pairs.
   ///   If key is empty, assumes only one object and this
   ///   will fail if there is more than one
   typedef std::pair<std::string, std::string> TypeKeyPair;
   typedef std::vector<TypeKeyPair>            TypeKeyPairs;
   virtual StatusCode streamObjects(const TypeKeyPairs& typeKeys);

   /// Stream out a vector of objects
   ///   Must convert to DataObject, e.g.
   ///   #include "SGTools/StorableConversions.h"
   ///     T* obj = xxx;
   ///     DataObject* dataObject = SG::asStorable(obj);
   typedef std::vector<DataObject*> DataObjectVec;
   virtual StatusCode streamObjects(const DataObjectVec& dataObjects);

   /// Fill refs of an object - done as second iteration over
   /// objects, after streamObject
   virtual StatusCode fillObjectRefs(const DataObjectVec& dataObjects);

   virtual StatusCode getInputItemList(SG::IFolder* m_p2BWrittenFromTool);

   virtual void dataHeaderSatellitesHandler(Property& /* theProp */);

private:
   /// Do the real connection to services
   virtual StatusCode connectServices();

private:
   /// OutputFile, name of the output db name
   StringProperty  m_outputName;
   /// ProcessTag, tag of processing stage: default to SG key of DataHeader (Stream name)
   StringProperty  m_processTag;
   /// ProvenanceTags, list of processing tags to propagate in DataHeader, to enable BackNavigation
   /// default = ['*'] (will propagate all entries)
   StringArrayProperty m_provenanceTags; 
   std::string m_provTagList;
   /// DataHeaderSatellites, list of additional DataHeader, which point to subset of DataObjects only
   /// default = [], example: ['basic/:EventInfo']
   StringArrayProperty m_dataHeaderSatellites;
   std::map<std::string, std::set<CLID> > m_satDhClidMap;
   /// OutputCollection, custom container name prefix for DataHeader: default = "" (will result in "POOLContainer_")
   StringProperty  m_outputCollection;
   /// PoolContainerPrefix, prefix for top level POOL container: default = "CollectionTree"
   StringProperty  m_containerPrefix;
   /// TopLevelContainerName, naming hint policy for top level POOL container: default = ""
   StringProperty  m_containerNameHint;
   /// SubLevelBranchName, naming hint policy for POOL branching: default = "<type>/<key>"
   StringProperty  m_branchNameHint;
   /// AttributeListKey, optional key for AttributeList to be written as part of the DataHeader: default = ""
   StringProperty  m_attrListKey;

   /// Pointer to the data store
   ServiceHandle<StoreGateSvc>   m_store;
   /// Keep reference to the data conversion service
   ServiceHandle<IAthenaPoolCnvSvc> m_conversionSvc;
   /// Ref to ClassIDSvc to convert type name to clid
   ServiceHandle<IClassIDSvc>    m_clidSvc;
   /// Current DataHeader for streamed objects
   DataHeader*     m_dataHeader;
   /// Name of DataHeader key
   std::string     m_dataHeaderKey;
   /// Flag to tell whether connectOutput has been called
   bool            m_connectionOpen;
   /// Flag as to whether to extend provenance via the DataHeader
   bool            m_extendProvenanceRecord;

   /// set of skipped item keys, because of missing CLID
   std::set<std::string> m_skippedItems;

};

#endif

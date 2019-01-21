/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAOUTPUTSTREAMTOOL_H
#define ATHENAOUTPUTSTREAMTOOL_H
/** @file AthenaOutputStreamTool.h
 *  @brief This is the implementation of IAthenaOutputStreamTool.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

class IClassIDSvc;
class IConversionSvc;
class StoreGateSvc;
class DataHeader;
namespace SG {
   class IFolder;
}

/** @class AthenaOutputStreamTool
 *  @brief This is the implementation of IAthenaOutputStreamTool.
 **/
class AthenaOutputStreamTool : public ::AthAlgTool, virtual public IAthenaOutputStreamTool {
public:
   /// Standard AlgTool Constructor
   AthenaOutputStreamTool(const std::string& type,
	   const std::string& name,
	   const IInterface* parent);
   /// Destructor
   virtual ~AthenaOutputStreamTool();

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
   StatusCode connectOutput(const std::string& outputName = "");

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

private:
   /// Do the real connection to services
   virtual StatusCode connectServices();

private:
   StringProperty  m_outputName{ this, "OutputFile", "", "name of the output db name"};
   StringProperty  m_processTag{ this, "ProcessingTag", "", "tag of processing stage: default to SG key of DataHeader (Stream name)"};
   StringProperty  m_outputCollection{ this, "OutputCollection", "", "custom container name prefix for DataHeader: default = "" (will result in \"POOLContainer_\")"};
   StringProperty  m_containerPrefix{ this, "PoolContainerPrefix", "", "prefix for top level POOL container: default = \"CollectionTree\""};
   StringProperty  m_containerNameHint{ this, "TopLevelContainerName", "0", "naming hint policy for top level POOL container: default = \"0\""};
   StringProperty  m_branchNameHint{ this, "SubLevelBranchName", "0", "naming hint policy for POOL branching: default = \"0\"" };
   SG::ReadHandleKey<AthenaAttributeList>  m_attrListKey{this, "AttributeListKey", "", "optional key for AttributeList to be written as part of the DataHeader: default = \"\""};

   ServiceHandle<StoreGateSvc>   m_store{ this, "Store", "StoreGateSvc/DetectorStore", "Pointer to the data store"};
   /// Keep reference to the data conversion service
   ServiceHandle<IConversionSvc> m_conversionSvc;
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

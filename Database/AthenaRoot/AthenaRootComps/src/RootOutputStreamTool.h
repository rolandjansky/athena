///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// RootOutputStreamTool.h
// Header file for class Athena::RootOutputStreamTool
// Author Peter van Gemmeren <gemmeren@anl.gov>
///////////////////////////////////////////////////////////////////
#ifndef ATHENAROOTCOMPS_ROOTOUTPUTSTREAMTOOL_H
#define ATHENAROOTCOMPS_ROOTOUTPUTSTREAMTOOL_H 1

/** @file Athena::RootOutputStreamTool.h
 *  @brief This is the AthenaRoot version of AthenaOutputStreamTool.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaKernel/IAthenaOutputStreamTool.h"

// fwk
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

class StoreGateSvc;
class IConversionSvc;
class IClassIDSvc;

namespace Athena {
/** @class Athena::RootOutputStreamTool
 *  @brief This is the AthenaRoot version of AthenaServices/AthenaOutputStreamTool.
 **/
class RootOutputStreamTool : virtual public ::IAthenaOutputStreamTool, public ::AthAlgTool {
public:
  /// Standard AlgTool Constructor
  RootOutputStreamTool(const std::string& type, const std::string& name, const IInterface* parent);

  /// Destructor
  virtual ~RootOutputStreamTool();

  /// Gaudi AlgTool Interface method implementations:
  StatusCode initialize();
  StatusCode finalize();

  /// Specify which data store and conversion service to use
  /// and whether to extend provenence
  ///   Only use if one wants to override jobOptions
  StatusCode connectServices(const std::string& dataStore, const std::string& cnvSvc, bool extendProvenenceRecord);

  /// Connect to the output stream
  ///   Must connectOutput BEFORE streaming
  ///   Only specify "outputName" if one wants to override jobOptions
  StatusCode connectOutput(const std::string& outputName);

  /// Commit the output stream after having streamed out objects
  ///   Must commitOutput AFTER streaming
  StatusCode commitOutput(bool doCommit = false);

  /// Finalize the output stream after the last commit, e.g. in
  /// finalize
  StatusCode finalizeOutput();

  /// Stream out objects. Provide vector of typeName/key pairs.
  ///   If key is empty, assumes only one object and this
  ///   will fail if there is more than one
  StatusCode streamObjects(const IAthenaOutputStreamTool::TypeKeyPairs& typeKeys, const std::string& outputName = "");

  /// Stream out a vector of objects
  ///   Must convert to DataObject, e.g.
  ///   #include "AthenaKernel/StorableConversions.h"
  ///     T* obj = xxx;
  ///     DataObject* dataObject = SG::asStorable(obj);
  StatusCode streamObjects(const IAthenaOutputStreamTool::DataObjectVec& dataObjects, const std::string& outputName = "");

  /// Fill refs of an object - done as second iteration over
  /// objects, after streamObject
  StatusCode fillObjectRefs(const IAthenaOutputStreamTool::DataObjectVec& dataObjects);

  StatusCode getInputItemList(SG::IFolder* m_p2BWrittenFromTool);

private:
  /// Default constructor:
  RootOutputStreamTool(); //< not implemented
  RootOutputStreamTool(const RootOutputStreamTool&); //< not implemented
  RootOutputStreamTool& operator=(const RootOutputStreamTool&); //< not implemented

private:
  /// ServiceHandle to the data store service
  ServiceHandle< ::StoreGateSvc> m_storeSvc;
  /// ServiceHandle to the data conversion service
  ServiceHandle< ::IConversionSvc> m_conversionSvc;
  /// ServiceHandle to clid service
  ServiceHandle< ::IClassIDSvc> m_clidSvc;

  /// Name of the output file
  std::string m_outputName;

  /// Name of the output tuple
  std::string m_treeName;
};

}//> end namespace Athena

#endif

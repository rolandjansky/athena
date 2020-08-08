/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  OVERLAYBYTESTREAMUTILS_BYTESTREAMMULTIPLEOUTPUTSTREAMCOPYTOOL_H
# define OVERLAYBYTESTREAMUTILS_BYTESTREAMMULTIPLEOUTPUTSTREAMCOPYTOOL_H
/**
 * @file ByteStreamMultipleOutputStreamCopyTool.h
 *
 * @brief Implementation of IAthenaOutputStreamTool for Copying ByteStream from input
 *
 *        The input and output should be configured by the ByteStreamInputSvc and ByteStreamOutputSvc.
 *
 * @author Andy Haas <ahaas@slac.stanford.edu>
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

// Gaudi
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include <vector>

// ByteStream

#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 ** @class ByteStreamMultipleOutputStreamCopyTool
 **
 ** @brief This is a tool that implements the IAthenaOutputStreamTool for
 **    copying ByteStream from input.
 **
 **/

class ByteStreamMultipleOutputStreamCopyTool  :  public AthAlgTool, virtual public IAthenaOutputStreamTool
{

public:

  /// Constructor
  ByteStreamMultipleOutputStreamCopyTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent);
  /// Destructor
  virtual ~ByteStreamMultipleOutputStreamCopyTool();

  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();


  /// Connect to the output stream
  ///   Must connectOutput BEFORE streaming
  ///   Only specify "outputName" if one wants to override jobOptions
  virtual StatusCode connectOutput(const std::string& outputName = "")  ;

  /// Commit the output stream after having streamed out objects
  ///   Must commitOutput AFTER streaming
  virtual StatusCode commitOutput(bool doCommit = false) ;

  /// Finalize the output stream after the last commit, e.g. in
  /// finalize
  virtual StatusCode finalizeOutput();

  /// No need to connect Services.
  virtual StatusCode connectServices(const std::string& dataStore,
                                     const std::string& cnvSvc,
                                     bool extendProvenenceRecord = false)  ;

  /// No object written for this tool
  virtual StatusCode streamObjects(const TypeKeyPairs& typeKeys, const std::string& outputName = "") ;


  /// no stream of vector of objects either.
  virtual StatusCode streamObjects (const DataObjectVec& dataObjects, const std::string& outputName = "") ;


  /// no refs for this tool.
  virtual StatusCode fillObjectRefs(const DataObjectVec& dataObjects) ;

  /// Get ItemList from the OutputStreamTool (e.g. all input objects)
  virtual StatusCode getInputItemList(SG::IFolder* m_p2BWrittenFromTool) ;

private:

  // Handle for BS input Svc
  ServiceHandle<ByteStreamInputSvc> m_inputSvc;
  
  // Whether to use lbn map or not
  bool m_uselbnmap;
  std::string m_lbn_map_file;
  std::string m_trigfile;
  void initlbnmap();

  // Handle for BS output Svc
  std::vector< ServiceHandle<ByteStreamOutputSvc> > m_outputSvc;
  int m_NoutputSvc;

  int m_skipevents;
 
};


#endif // OVERLAYBYTESTREAMUTILS_BYTESTREAMMULTIPLEOUTPUTSTREAMCOPYTOOL_H

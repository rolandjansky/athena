///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootConnection.h
// Header file for class Athena::RootConnection
// Author: Peter van Gemmeren <gemmeren@anl.gov>
///////////////////////////////////////////////////////////////////
#ifndef ATHENAROOTCOMPS_ROOTCONNECTION_H
#define ATHENAROOTCOMPS_ROOTCONNECTION_H 1

/** @file RootConnection.h
 *  @brief This file contains the class definition for the Athena::RootConnection class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// STL includes
#include <string>

// fwk includes
#include "GaudiKernel/StatusCode.h"

// fwd declares
class IInterface;
class TFile;
class TTree;
class TBranch;

namespace Athena {
/** @class Athena::RootConnection
 *  @brief This class provides the implementation of Athena::RootConnection class, similar to Gaudi IDataConnection
 **/
class RootConnection
{
public:
  /// Standard constructor
  RootConnection(const IInterface* own, const std::string& pfn);
  /// Standard destructor
  virtual ~RootConnection();

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  /// Open data stream in read mode
  StatusCode connectRead();
  /// Open data stream in write mode
  StatusCode connectWrite(const std::string& mode);
  /// Commit data stream to ROOT
  StatusCode commit();
  /// Release data stream and release implementation dependent resources
  StatusCode disconnect();
  /// Check if connected to data source
  bool isConnected() const;
  /// Read root byte buffer from input stream
  StatusCode read(void* const data, size_t len);
  /// Write root byte buffer to output stream
  StatusCode write(const void* data, unsigned long& len);
  /// Set the container name and type, creating TTree and TBranch as needed
  StatusCode setContainer(const std::string& container, const std::string& type);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  /// File ID of the connection
  std::string m_fid;
  /// Physical file name of the connection
  std::string m_pfn;
  /// Age counter
  //int m_age;
  /// Owner pointer
  //const IInterface* m_owner;

  /// Pointer to the Root event data file
  TFile* m_file;
  /// Pointer to the main event data tree
  TTree* m_tree;
  /// Pointer to the current data branch
  TBranch* m_branch;
  /// Branch typecode for branch we are asked to write out
  char m_branchTypeCode;

  //FIXME: use a RootConnectionSetup class to
  //       gather this kind of configuration
  // -> autoflush, split, buffersize,...
};

} //> namespace Athena

#endif /* !ATHENAROOTCOMPS_ROOTCONNECTION_H */

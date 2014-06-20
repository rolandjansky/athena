///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootConnection.h
// Header file for class Athena::RootConnection
// Author: Peter van Gemmeren <gemmeren@anl.gov>
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef ATHENAROOTCOMPS_ROOTCONNECTION_H
#define ATHENAROOTCOMPS_ROOTCONNECTION_H 1

/** @file RootConnection.h
 *  @brief This file contains the class definition for the Athena::RootConnection class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// STL includes
#include <string>
#include "CxxUtils/unordered_map.h"

// fwk includes
#include "GaudiKernel/StatusCode.h"
#include "AthenaRootKernel/IIoSvc.h"

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
  RootConnection(const IInterface* own, IIoSvc::Fd fd);
  /// Standard destructor
  virtual ~RootConnection();

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  /// Open data stream in read mode
  StatusCode connectRead();
  /// Open data stream in write mode
  StatusCode connectWrite(IIoSvc::IoType mode);
  StatusCode addMetadata(const std::string& key, const void* obj, const std::type_info& ti);
  StatusCode commit();
  /// Release data stream and release implementation dependent resources
  StatusCode disconnect();
  /// Check if connected to data source
  bool isConnected() const;
  /// Read root byte buffer from input stream
  StatusCode read(void* const data, size_t len);
  /// Write root byte buffer to output stream
  StatusCode write(const void* data, unsigned long& len);
  StatusCode setContainer(const std::string& container,
                          const std::string& type,
                          const void* data);

  void setTreeName(const std::string& name);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  /// file descriptor
  IIoSvc::Fd m_fd;
  /// File ID of the connection
  std::string m_fid;
  /// Physical file name of the connection
  std::string m_pfn;
  /// Age counter
  int m_age;
  /// Owner pointer
  const IInterface* m_owner;

  /// Pointer to the Root event data file
  TFile* m_file;
  /// Pointer to the main event data tree
  TTree* m_tree;
  /// Pointer to the current data branch
  TBranch* m_branch;

  //FIXME: use a RootConnectionSetup class to
  //       gather this kind of configuration
  // -> autoflush, split, buffersize,...
  /// name of the tree
  std::string m_treeName;

  struct BranchDescr {
    void* buffer;
    char typecode;
  };
  typedef SG::unordered_map<TBranch*, BranchDescr*> BranchDescriptors_t;
  /// a map of branch descriptors for each branch we are asked to write out
  BranchDescriptors_t m_descr;
};

} //> namespace Athena

#endif /* !ATHENAROOTCOMPS_ROOTCONNECTION_H */

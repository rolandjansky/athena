// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOOLS_AANTUPLESTREAM_H
#define ANALYSISTOOLS_AANTUPLESTREAM_H 1

#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIoComponent.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <map>
#include <vector>

namespace coral
{
  class AttributeList;
  class AttributeListSpecification;
}

class StoreGateSvc;

class ITHistSvc;

class TFile;
class TTree;
class TBranch;

class AANTupleStream : virtual public IIoComponent,
                               public AthAlgorithm
{
public:
    
  /// Standard AANTupleStream Constructor
  AANTupleStream(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Standard Destructor
  virtual ~AANTupleStream();

  /// Initialize AANTupleStream
  virtual StatusCode initialize();

  /// Terminate AANTupleStream
  virtual StatusCode finalize();

  /// Working entry point
  virtual StatusCode execute();

private:

  /// Initialize the output collection
  StatusCode initCollection();

  /// Get ref from a proxy
  StatusCode getRef(CLID id, const std::string& key,
		    std::string& ref);

  /// Split the address to get pool token
  StatusCode splitAddress( const std::string& address,
			   const std::string& match,
			   std::string&       address_header,
			   std::string&       address_data ) const;

  /// initialize sub-algos
  StatusCode initialize_subAlgos ();

  /// execute sub-algos
  StatusCode execute_subAlgos ();

  /// write schema of AttributeList
  void writeAttributeListSpecification();

  /// setup Tree
  void setupTree();

  /// write Token and AttributeList
  bool writeTokenAttrList( const std::string& token, const coral::AttributeList& attributeList );
  
  /// get filter alogs
  StatusCode getFilters();

  /// check filters passed
  bool isEventAccepted() const;

  StatusCode initSchema();

  /** @brief callback method to reinitialize the internal state of
   *         the component for I/O purposes (e.g. upon @c fork(2))
   */
  StatusCode io_reinit ();

  /// Persistency service
  ServiceHandle<IAddressCreator> m_persSvc;

  /// Collection attribute specification
  coral::AttributeListSpecification* m_attribSpec;

  /// Name of the output collection
  std::string m_fileName;
  
  /// Vector of extra parent ref names to register in collection
  StringArrayProperty m_extraRefNames;

  /// Flag to signal whether or not to write input data header
  BooleanProperty          m_writeInputDH;

  /// Flag to signal whether data header exists
  BooleanProperty          m_existDH;

  /// Name of Stream
  std::string m_streamName;

  /// write schema in execute()
  BooleanProperty m_lateSchema;

  /// macro
  std::string m_macro;

  /// first event
  bool m_schemaDone;

  /// Name of the output tree
  std::string m_treeName;

  /// Vector of names of Algorithms that this stream accepts
  std::vector<std::string> m_acceptNames;

  /// Vector of Algorithms that this stream accepts
  std::vector<Algorithm*> m_acceptAlgs;

  /// TTree
  TTree * m_tree;

  /// TBranch which contains Token info
  TBranch * m_tokenBranch;

  /// THistSvc
  ServiceHandle<ITHistSvc> m_tHistSvc;

  // Sub-algorithms  as given in jobOptions type/name
  std::vector<std::string> m_membersNames;

  /// for string branches
  static const unsigned int s_maxLengthOfStrings = 5000;
  char m_tokenCString[s_maxLengthOfStrings];
};

#endif

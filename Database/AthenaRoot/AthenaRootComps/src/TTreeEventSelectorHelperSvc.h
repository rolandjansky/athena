///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TTreeEventSelectorHelperSvc.h 
// Header file for class TTreeEventSelectorHelperSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_TTREEEVENTSELECTORHELPERSVC_H
#define ATHENAROOTCOMPS_TTREEEVENTSELECTORHELPERSVC_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// GaudiKernel
#include "GaudiKernel/IInterface.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;
class TTreeEventSelector;


class TTreeEventSelectorHelperSvc
  : public ::AthService
{ 
  friend class SvcFactory<TTreeEventSelectorHelperSvc>;
  friend class TTreeEventSelector;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TTreeEventSelectorHelperSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~TTreeEventSelectorHelperSvc(); 

  // Assignment operator: 
  //TTreeEventSelectorHelperSvc &operator=(const TTreeEventSelectorHelperSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// callback to synchronize the list of input files
  void setupInputCollection( Property& inputCollectionsName );

  /// configure the tuple service to deliver our tree to clients (if any)
  StatusCode configureTupleSvc();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  TTreeEventSelectorHelperSvc();

  // Containers
  
  /// List of input files containing @c TTree 
  StringArrayProperty m_inputCollectionsName;

  /// name of the @c TChain in the chain of ROOT files to read in
  std::string m_tupleName;

  /// list of branches to activate in the @c TChain
  /// The default is to have no active branch and let the user activate
  /// the ones she needs (for 'obvious' performance reasons)
  std::vector<std::string> m_activeBranchNames;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& TTreeEventSelectorHelperSvc::interfaceID() 
{ 
  static const InterfaceID IID_TTreeEventSelectorHelperSvc
    ("TTreeEventSelectorHelperSvc", 1, 0);
  return IID_TTreeEventSelectorHelperSvc; 
}



#endif //> !ATHENAROOTCOMPS_TTREEEVENTSELECTORHELPERSVC_H

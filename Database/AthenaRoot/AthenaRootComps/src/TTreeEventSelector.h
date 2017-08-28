///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TTreeEventSelector.h 
// Header file for class TTreeEventSelector
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_TTREEEVENTSELECTOR_H 
#define ATHENAROOTCOMPS_TTREEEVENTSELECTOR_H 

// framework includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IAddressProvider.h"

#include <unordered_set>

// Forward declaration
class ISvcLocator;
class StoreGateSvc;
class ITHistSvc;
class TChain;
class IClassIDSvc;
class IDictLoaderSvc;

/** @brief Class implementing the GAUDI @c IEvtSelector interface using 
 *         ROOT @c TTree as a backend
 */
class TTreeEventSelector : 
  virtual public IEvtSelector,
  virtual public IEventSeek,
  virtual public IAddressProvider,
  virtual public AthService
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  TTreeEventSelector( const std::string& name, ISvcLocator* svcLoc );

  /// Destructor: 
  virtual ~TTreeEventSelector(); 

  // Athena hooks
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  
  ///@{
  /// @c IEvtSelector interface
  virtual StatusCode createContext( Context*& refpCtxt ) const;

  virtual StatusCode last( Context& refContext ) const;
  virtual StatusCode next( Context& refCtxt ) const;
  virtual StatusCode next( Context& refCtxt, int jump ) const;
  virtual StatusCode previous( Context& refCtxt ) const;
  virtual StatusCode previous( Context& refCtxt, int jump ) const;
  virtual StatusCode rewind( Context& refCtxt ) const;

  virtual StatusCode createAddress( const Context& refCtxt, 
                                    IOpaqueAddress*& ) const;
  virtual StatusCode releaseContext( Context*& refCtxt ) const;
  virtual StatusCode resetCriteria( const std::string& cr, 
                                    Context& ctx )const;
  ///@}

  ///@{
  /// @c IEventSeek interface
  /**
   * @brief Seek to a given event number.
   * @param evtnum  The event number to which to seek.
   */
  virtual StatusCode seek (int evtnum);

  /**
   * @brief return the current event number.
   * @return The current event number.
   */
  virtual int curEvent () const;
  ///@}
  
  ///@{
  /// @c IAddressProvider interface
  ///get all addresses from Provider : Called before Begin Event
  virtual 
  StatusCode preLoadAddresses(StoreID::type storeID, tadList& list);
 
   /// get all new addresses from Provider for this Event.
  virtual 
  StatusCode loadAddresses(StoreID::type storeID, tadList& list);
 
  /// update a transient Address
  virtual 
  StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                           const EventContext& ctx);
  ///@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// callback to synchronize the list of input files
  void setupInputCollection( Property& inputCollectionsName );

  /// helper method to create proxies
  StatusCode createRootBranchAddresses(StoreID::type storeID,
                                       tadList &tads);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to the @c StoreGateSvc event store
  StoreGateSvc_t m_storeGate;

  typedef ServiceHandle<IClassIDSvc> ICLIDSvc_t;
  /// Pointer to the @c IClassIDSvc
  ICLIDSvc_t m_clidsvc;

  typedef ServiceHandle<IDictLoaderSvc> IDictSvc_t;
  /// Pointer to the @c IDictLoaderSvc
  IDictSvc_t m_dictsvc;

  typedef ServiceHandle<ITHistSvc> ITHistSvc_t;
  // /// Pointer to the @c ITHistSvc containing the @c TTree 
  ITHistSvc_t m_tupleSvc;

  /// List of input files containing @c TTree 
  StringArrayProperty m_inputCollectionsName;

  /// Name of @c TTree to load from collection of input files
  StringProperty m_tupleName;

  /// List of branches to activate in the @c TTree 
  StringArrayProperty m_activeBranchNames;

  /// Number of events to skip at the beginning 
  long m_skipEvts;

  /// Number of Events read so far.
  mutable long m_nbrEvts;

  /// Total number of events
  long m_totalNbrEvts;

  /// The @c TTree we are reading from file(s)
  TChain* m_tuple;

  /** The (python) selection function to apply on the @c TChain we are reading
   */
  //PyObject* m_pySelectionFct;

  // the list of transient addresses we "manage" or know about
  // these addresses are the actual TTree's branch names
  std::unordered_set<SG::TransientAddress*> m_rootAddresses;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHENAROOTCOMPS_TTREEEVENTSELECTOR_H

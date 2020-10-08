///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiEventSelector.h 
// Header file for class McAsciiEventSelector
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCASCIIEVENTSELECTOR_MCASCIIEVENTSELECTOR_H 
#define MCASCIIEVENTSELECTOR_MCASCIIEVENTSELECTOR_H 

// STL includes
#include <vector>
#include <string>
#include <utility> // for std::pair

// Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IEvtSelector.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/MsgStream.h"

// Forward declaration
class EventSource;
class ISvcLocator;
class McAsciiCnvSvc;

class McAsciiEventSelector : virtual public IEvtSelector,
			     virtual public AthService
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  McAsciiEventSelector( const std::string& name, ISvcLocator* svcLoc );

  /// Destructor: 
  virtual ~McAsciiEventSelector(); 

  // Athena hooks
  virtual StatusCode initialize();
  virtual StatusCode stop();
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

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Pointer to the converter service
  McAsciiCnvSvc* m_asciiCnvSvc;

  /// List of input files
  Gaudi::Property<std::vector<std::string>> m_asciiFileNames;

  typedef std::size_t                       EvtSize_t;
  typedef std::pair<std::string, EvtSize_t> AsciiFile_t;
  typedef std::vector<AsciiFile_t>          AsciiFileList_t;

  /// List of ASCII files
  AsciiFileList_t m_asciiFiles;

  /// Iterator over input files
  mutable AsciiFileList_t::const_iterator m_curFile;

  /// number of events processed for the current input ASCII file
  mutable std::size_t m_curEvtNbr;

  // mimic @c McEventSelector properties
  Gaudi::CheckedProperty<int> m_runNbr;
  Gaudi::CheckedProperty<int> m_firstEventNbr;
  Gaudi::CheckedProperty<int> m_eventsPerRun;
  Gaudi::CheckedProperty<int> m_initTimeStamp;
  Gaudi::Property<int> m_timeStampInterval;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCASCIIEVENTSELECTOR_MCASCIIEVENTSELECTOR_H

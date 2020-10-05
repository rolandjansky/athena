///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SGAudSvc.h 
// Header file for class SGAudSvc
// Author: Ilija Vukotic <ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef SGMONITORING_SGAUDSVC_H 
#define SGMONITORING_SGAUDSVC_H 

// STL includes
#include <string>
#include <iosfwd>
#include <map>
#include <set>
#include <fstream>


// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "AthenaKernel/IClassIDSvc.h"

#include "SGAudCore/ISGAudSvc.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;
class IChronoStatSvc;
class IAlgContextSvc;
class AlgContextSvc;

class SGAudSvc : virtual public ISGAudSvc,
		 virtual public IIncidentListener,
		 public AthService
{ 

protected:
    
  friend class SvcFactory<SGAudSvc>;


public: 

  /// Constructor with parameters: 
  SGAudSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SGAudSvc(); 

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  //@}

  static const InterfaceID& interfaceID();
  

/** incident service handle for EndEvent. Calls monitor. There should be more elegant way to get number of events passed.
*/
  void handle( const Incident& incident );

  // do the auditing, called from DataStore.cxx 
  virtual void SGAudit(const std::string& key, const CLID& id, 
		       const int& fnc, const int& store_id);

  
/** 
* @brief Gets name of curently running algorithm from AlgContextSvc.
*/
  bool SGGetCurrentAlg();
/** 
* @brief For implementing custom increased granularity auditing of for instance tools.
*/
  void setFakeCurrentAlg(const std::string&);
/** 
* @brief For implementing custom increased granularity auditing of for instance tools.
*/
  void clearFakeCurrentAlg();
 
  private: 

  /// Default constructor: 
  SGAudSvc();

  void SGAudRETRIEVE(std::string SGobject);
  void SGAudRECORD(std::string SGobject);

  void getNobj(std::string name);
  void addRead();
  void addWrite();

  /// just counts events. called at EndEvent incident
  void monitor();

  void writeJSON();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// MsgStream for talking with the outside world
  MsgStream m_msg;

  /// Pointer to the @c AlgContextScv
  IAlgContextSvc *p_algCtxSvc;
  IClassIDSvc* m_pCID;
  
  /// Name of the output file 
  std::string m_outFileName, m_allFileName, m_sumFileName;  

  /// Whether to ignore fake current algs
  bool m_ignoreFakeAlgs;

  /// Whether to use CLID or Data Obj Name in JSON output file
  bool m_useCLID;

  /// Vector of accessed SG objects names
  std::vector<std::string> m_vObj;
  /// Vector of names of algorithms accessing SG
  std::vector<std::string> m_vAlg;
  /// map counting Reads of each object by each algorithm.
  std::map<int,int> m_timesRead;
  /// map counting Writes of each object by each algorithm.
  std::map<int,int> m_timesWritten;
  std::string m_currAlg;
  std::string m_currObj;
  std::string m_fakeCurrAlg;
  int m_nCurrAlg;
  int m_nCurrObj;
  int m_nEvents;
  int m_startEvent;

  // map<"alg_name", set<"cid/key"> >
  typedef std::map<std::string, std::set<std::string> > DataMap;
  DataMap m_read;
  DataMap m_write;

  std::ofstream m_ofa, m_ofs;
  bool m_inExec;
}; 


/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& SGAudSvc::interfaceID() 
{ 
   return ISGAudSvc::interfaceID(); 
}

#endif 

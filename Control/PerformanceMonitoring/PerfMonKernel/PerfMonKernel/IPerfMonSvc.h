///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// IPerfMonSvc.h 
// Header file for class IPerfMonSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONKERNEL_IPERFMONSVC_H 
#define PERFMONKERNEL_IPERFMONSVC_H 1

/** @class IPerfMonSvc
 *  This is the interface to the main service scheduling a performance 
 *  monitoring job.
 */

// STL includes
#include <utility> // std::pair
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StatEntity.h"

// PerfMonKernel includes
#include "PerfMonKernel/PerfMonDefs.h"

// forward declaration
namespace AIDA { class IBaseHistogram; }
namespace PerfMon { struct Component; }
namespace PerfMon { struct IoContainer; }

class IPerfMonSvc : virtual public IMonitorSvc
{ 
  ///////////////////////////////////////////////////////////////////
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IPerfMonSvc();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// Retrieve the current state of Gaudi's finite state machine
  PerfMon::State::Type monState() const 
  { return m_monState; }

  /// Retrieve the name of the current state of Gaudi's FSM
  const std::string& monStateName() const 
  { return PerfMon::Steps[m_monState]; }

  /// @brief return the list of components' names for a given step
  std::vector<std::string>
  components(PerfMon::State::Type step) const
  { return this->components(PerfMon::Steps[step]); }

  /// @brief return the list of components' names for a given step
  virtual
  std::vector<std::string>
  components(const std::string& stepName) const = 0;

  /// @brief return the list of io-components' names
  virtual
  std::vector<std::string>
  io_components() const = 0;

  /// retrieve the monitored value for the component `compName` and 
  /// for the step `step` (ini,evt,fin,cbk,...)
  const PerfMon::Component*
  component(PerfMon::State::Type step,
	    const std::string& compName) const
  { return this->component(PerfMon::Steps[step], compName); }

  /// retrieve the monitored value for the component `compName` and 
  /// for the step `stepName` (ini,evt,fin,cbk,...)
  virtual
  const PerfMon::Component*
  component(const std::string& stepName,
	    const std::string& compName) const = 0;

  /// retrieve the I/O monitored value for the I/O component `compName`
  virtual
  const PerfMon::IoContainer*
  io_component(const std::string& compName) const = 0;

  /// retrieve the domain name for a given component `compName`
  virtual
  void
  domain(const std::string& compName, std::string& domain) const = 0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** Declare monitoring information
      @param name Monitoring information name knwon to the external system
      @param var Monitoring Listener address
      @param desc Textual description
      @param owner Owner identifier of the monitoring information (needed to peform clean up
  */
  virtual void declareInfo( const std::string& name, 
			    const bool& var, 
			    const std::string& desc, 
			    const IInterface* owner ) = 0;
  virtual void declareInfo( const std::string& name, 
			    const int& var, 
			    const std::string& desc,  
			    const IInterface* owner ) = 0;
  virtual void declareInfo( const std::string& name, 
			    const long& var, 
			    const std::string& desc, 
			    const IInterface* owner ) = 0;
  virtual void declareInfo( const std::string& name, 
			    const double& var, 
			    const std::string& desc, 
			    const IInterface* owner ) = 0;
  virtual void declareInfo( const std::string& name, 
			    const std::string& var, 
			    const std::string& desc, 
			    const IInterface* owner ) = 0;
  virtual void declareInfo( const std::string& name, 
			    const std::pair<double,double>& var, 
			    const std::string& desc, 
			    const IInterface* owner ) = 0;
  virtual void declareInfo( const std::string& name, 
			    const AIDA::IBaseHistogram* var, 
			    const  std::string& desc, 
			    const IInterface* owner ) = 0;
  virtual void declareInfo( const std::string& name, 
			    const std::string& format, 
			    const void * var, int size, 
			    const std::string& desc, 
			    const IInterface* owner ) = 0;
  virtual void declareInfo(const std::string& name, 
                           const StatEntity& var, 
                           const std::string& desc, 
                           const IInterface* owner) = 0;

  /** Undeclare monitoring information
      @param name Monitoring information name knwon to the external system
      @param owner Owner identifier of the monitoring information
  */
  virtual void undeclareInfo( const std::string& name, 
			      const IInterface* owner ) = 0;
  /** Undeclare monitoring information
      @param owner Owner identifier of the monitoring information
  */
  virtual void undeclareAll( const IInterface* owner ) = 0;

  /** Get the names for all declared monitoring informations for a given
      owener. If the owner is NULL, then it returns for all owners
      informationUndeclare monitoring information
  */
  virtual std::set<std::string>* getInfos(const IInterface* owner = 0) = 0;

  /** Start collecting monitoring data for a given component, for a given
   *  step of the Gaudi FSM
   */
  virtual void startAud( const std::string& stepName,
			 const std::string& compName = "PerfMonSlice" ) = 0;

  /** Stop collecting monitoring data for a given component, for a given
   *  step of the Gaudi FSM
   */
  virtual void stopAud( const std::string& stepName,
			const std::string& compName = "PerfMonSlice" ) = 0;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// set the current state of the Gaudi's FSM
  void setMonState( PerfMon::State::Type step )
  { m_monState = step; }

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// current state of the Gaudi's FSM
  PerfMon::State::Type m_monState;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IPerfMonSvc::interfaceID() 
{ 
  static const InterfaceID IID_IPerfMonSvc("IPerfMonSvc", 1, 0);
  return IID_IPerfMonSvc; 
}

#endif //> PERFMONKERNEL_IPERFMONSVC_H

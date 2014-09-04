// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        1         2         3         4         5         6         7
//234567890123456789012345678901234567890123456789012345678901234567890123456789
//
#ifndef EVENTSHAPETOOLS_EVENTETDENSITYPROVIDERBASE_H
#define EVENTSHAPETOOLS_EVENTETDENSITYPROVIDERBASE_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "EventKernel/INavigable4Momentum.h"
#include "EventKernel/ISignalState.h"

#include "AthenaBaseComps/AthAlgTool.h"

//#include "fastjet/RangeDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/PseudoJet.hh"

#include "JetInterface/IFastJetInterfaceTool.h"

#include "EventShapeInterface/IEventEtDensityProvider.h"

#include <vector>
#include <map>


class EventEtDensityProviderBase : virtual public IEventEtDensityProvider,
				   public AthAlgTool
{
public:

  /*! @brief @c AlgTool constructor */
  EventEtDensityProviderBase(const std::string& type,const std::string& name,
			     const IInterface* pParent);
  /*! @brief Base tool destructor */
  virtual ~EventEtDensityProviderBase();

  /*! @brief Common tool initialization */
  virtual StatusCode initialize();

  /*! @brief Access to event transverse energy density */
  virtual double rho();
  virtual double sigma();
  virtual double area();
  /*! @brief Access to event transverse energy density at given direction 
   * 
   *  @param[in] eta pseudorapidity at which Et density is evaluated - 
   *             ignored in base class implementation.
   *
   *  @note Derived classes should implement this function if direction 
   *  dependent densities are provided. 
   */
  virtual double rho(double eta);
  virtual double sigma(double eta);
  virtual double area(double eta);
  /*! @brief Access to event transverse energy densities in a given range
   *
   *  @param[in] etaMin lower boundary of pseudo-rapidity range in which
   *                    Et density is evaluated - ignored in base class 
   *                    implementation
   *  @param[in] etaMax upper boundary of pseudo-rapidity range in which
   *                    Et density is evaluated - ignored in base class 
   *                    implementation
   *
   *  @note Derived classes should implement this function if range
   *  dependent densities are provided. 
   */
  virtual double rho(double etaMin,double etaMax);
  virtual double sigma(double etaMin,double etaMax);
  virtual double area(double etaMin,double etaMax);

  virtual const EventEtDensity& eventEtDensity();
  virtual const EventEtDensity& eventEtDensity(double eta);
  virtual const EventEtDensity& eventEtDensity(double etaMin,double etaMax);

  /*! @brief @c IncidentListener implementation */
  virtual void handle(const Incident& incident);

  //  virtual bool eventEtDensity(EventEtDensity& density);

  /*! @brief Internal signal state type */
  typedef P4SignalState::State         state_t;
  /*! @brief Internal signal state key type */
  typedef std::string                  statekey_t;
  /*! @brief Internal signal state map type */
  typedef std::map<statekey_t,state_t> statemap_t;

protected:

  static statekey_t m_stateDefaultKey;
  static statekey_t m_stateUnknownKey;
  static statekey_t m_stateNoReqKey;

  static state_t    m_stateDefault;
  static state_t    m_stateUnknown;

  static statemap_t m_stateMap;

  /*! @brief Internal jet type */
  typedef IFastJetInterfaceTool::fjet_t     jet_t;
  /*! @brief Internal jet container type */
  typedef IFastJetInterfaceTool::fjetlist_t jetcontainer_t;

  /*! @brief Internal navigable four-momentum type */
  typedef INavigable4Momentum       mom_t;
  /*! @brief Internal navigable four-momentum container type */
  typedef std::vector<const mom_t*> momcontainer_t;

  //  fastjet::RangeDefinition etaRange() const;
  fastjet::Selector etaRange() const;
  double etaMin() const;
  double etaMax() const;

  /*! @brief Action invoked by the @c IncidentListener::handle */
  virtual StatusCode eventAction();

  /*! @brief Interface to client action implementation
   * 
   *  Typically to implementation of density calculation for overall
   *  event density.
   */
  virtual StatusCode executeAction()                            = 0;
  /*! @brief Interface to client action implementation
   * 
   *  Typically to implementation of density calculation for
   *  event density a given direction.
   *
   *  @param[in] eta pseudorapidity at which density is evaluated.
   */
  virtual StatusCode executeAction(double eta)                  = 0;
  /*! @brief Interface to client action implementation
   * 
   *  Typically to implementation of density calculation for
   *  event density in a given range.
   *
   *  @param[in] etaMin lower boundary of range in pseudorapidity at which 
   *                    density is evaluated.
   *  @param[in] etaMax upper boundary of range in pseudorapidity at which 
   *                    density is evaluated.
   */
  virtual StatusCode executeAction(double etaMin,double etaMax) = 0;

  /*! @brief Helper to cache the event density value
   *
   *  @param[in] rho actual value for event transverse energy density
   */
  virtual void setValue(double rhoMedian,double rhoRMS=0.,double area=0.,
			double etaMin=-5.,double etaMax=5.);
  virtual void setValue(const EventEtDensity& density);

  const std::string& inputCollection() const;
  std::string&       inputCollection();

  virtual EventEtDensity&       valueServer();
  virtual const EventEtDensity& valueServer()                            const;
  virtual const EventEtDensity& valueServer(double eta)                  const;
  virtual const EventEtDensity& valueServer(double etaMin,double etaMax) const;

  void eventReset();
  void setResetFlag(bool flag);
  bool isReset();

  size_t convertToPJ(const momcontainer_t& navMoms,jetcontainer_t& pseudoJets,
		     double pTthresh=0.);
  size_t convertToPJ(const momcontainer_t& navMoms,jetcontainer_t& pseudoJets,
		     state_t signalState,double pTthresh=0.);

  ToolHandle<IFastJetInterfaceTool> m_fastJetTool;
  bool m_haveFastJetTool;

  ServiceHandle<IIncidentSvc> p_IIncidentSvc;

  static const std::string& collectionKeyDefault();

  static state_t           findState(const statekey_t& stateKey);
  static const statekey_t& findStateKey(state_t state);
  static const statemap_t& getMap();
  static const statekey_t& stateKeyDefault();
  static state_t           stateDefault();
  static const statekey_t& stateKeyUnknown();
  static state_t           stateUnknown();
  static const statekey_t& stateNoReq();

  template<class C>
  StatusCode convert(const C* pColl,momcontainer_t& navMoms)
  {
    navMoms.clear();
    if ( pColl->empty() ) return StatusCode::SUCCESS; 

    if ( dynamic_cast<const mom_t*>(pColl->front()) == 0 )
      {
	ATH_MSG_ERROR("INavigable4Momentum is not in the inheritance tree of "
		      << "retrieved signal object");
	return StatusCode::FAILURE;
      }

    typename C::const_iterator fColl(pColl->begin());
    typename C::const_iterator lColl(pColl->end());
    for ( ; fColl != lColl; ++fColl ) 
      { navMoms.push_back(dynamic_cast<const mom_t*>(*fColl)); }
    return StatusCode::SUCCESS;
  }

  template<class C> 
  StatusCode convert(const C* pColl,jetcontainer_t& pseudoJets,
		     double pTthresh=0.)
  {
    momcontainer_t navMoms;
    if ( convert(pColl,navMoms).isFailure() ) { return StatusCode::FAILURE; }
    else { if ( navMoms.empty() ) return StatusCode::SUCCESS; }
   
    if ( m_haveSignalState )
      {
	if ( dynamic_cast<const ISignalState*>(navMoms.front()) == 0 )
	  {
	    ATH_MSG_ERROR("Tool misconfiguration: SignalState requested "
			  <<"but input signal objects do not support it");
	    return StatusCode::FAILURE;
	  }
	else
	  {
	    convertToPJ(navMoms,pseudoJets,m_signalState,pTthresh);
	    return StatusCode::SUCCESS;
	  }
      }
    else
      {
	convertToPJ(navMoms,pseudoJets,pTthresh);
	return StatusCode::SUCCESS;
      }
  }

  state_t   signalState() const {return  m_signalState;}

private:

  bool m_resetFlag;

  std::string m_inputCollection;

  EventEtDensity m_rhoValue;
  EventEtDensity m_rhoNullValue;

  double m_etaMin;
  double m_etaMax;

  statekey_t m_signalStateKey;
  state_t    m_signalState;
  bool       m_haveSignalState;

  static std::string m_keyDefault;
};

inline void EventEtDensityProviderBase::eventReset()
{ m_resetFlag = true; m_rhoValue.reset(); }

inline void EventEtDensityProviderBase::setResetFlag(bool flag)
{ m_resetFlag = flag; }

inline bool EventEtDensityProviderBase::isReset()
{ return m_resetFlag; }

inline fastjet::Selector EventEtDensityProviderBase::etaRange() const
{ return fastjet::SelectorRapRange(this->etaMin(),this->etaMax()); }

inline double EventEtDensityProviderBase::etaMin() const
{ return m_etaMin; }

inline double EventEtDensityProviderBase::etaMax() const
{ return m_etaMax; }

inline const std::string& EventEtDensityProviderBase::collectionKeyDefault()
{ return m_keyDefault; }

inline const EventEtDensityProviderBase::statekey_t& 
EventEtDensityProviderBase::stateKeyDefault()
{ return m_stateDefaultKey; }

inline const EventEtDensityProviderBase::statekey_t&
EventEtDensityProviderBase::stateKeyUnknown()
{ return m_stateUnknownKey; }

inline EventEtDensityProviderBase::state_t 
EventEtDensityProviderBase::stateDefault()
{ return m_stateDefault; }

inline EventEtDensityProviderBase::state_t
EventEtDensityProviderBase::stateUnknown()
{ return m_stateUnknown; }

inline const EventEtDensityProviderBase::statekey_t&
EventEtDensityProviderBase::stateNoReq()
{ return m_stateNoReqKey; }

inline const std::string& EventEtDensityProviderBase::inputCollection() const
{ return m_inputCollection; }

inline std::string& EventEtDensityProviderBase::inputCollection()
{ return m_inputCollection; }
 
/*! @class EventEtDensityProviderBase
 *  
 *  @brief Base class for event Et density calculators using @c FastJet .  
 *
 *  This tool base class supports application in any other tool, without 
 *  need for a configuration provided by a special algorithm. It provides a
 *  reset flag, which is set to @c true by a "BeginEvent" incident handler. 
 *  If this flag is found to be @c true in any of the accessor methods
 *  @c rho() , @c rho(eta) , or @c rho(etaMin,etaMax) , the corresponding 
 *  @c executeAction() , @c executeAction(eta) , or 
 *  @c executeAction(etaMin,etaMax) are invoked. These are oure virtual in this
 *  base class and have to be implemented in the derived (client) classes. The
 *  result of any invaction of the @c executeAction should be an updated 
 *  Et density. After the reset flag is found to be @c true, it is reset 
 *  by the first accessor method invoked for a new event, such that 
 *  @c executeAction is at most called once per event and accessor invocation.
 *
 *  Note that in a typical client application @c executeAction retrieves the
 *  inout data for the event Et density calculation, converts it to 
 *  @c fastjet::PseudoJet and invokes @c FastJet for the density calculation.
 *  It caches the result, either globally for the whole event, or in 
 *  pseudorapidity ranges (look-up), or in a functional form.  
 */
#endif

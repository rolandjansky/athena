// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEREC_EVENTFEATUREALGORITHMBASE_H
#define EVENTSHAPEREC_EVENTFEATUREALGORITHMBASE_H


#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "EventShapeInterface/IEventFeatureTool.h"
#include "EventShapeInterface/IEventFeatureAlgorithm.h"

#include <map>

class EventFeatureStore;

class EventFeatureAlgorithmBase : virtual public IEventFeatureAlgorithm,
				  public AthAlgorithm
{
public:

  /*! @brief Default @c Algorithm constructor */
  EventFeatureAlgorithmBase(const std::string& name,ISvcLocator* pSvcLocator);
  /*! @brief Baseclass destructor */
  virtual ~EventFeatureAlgorithmBase();


  /*! @brief Default algorithm initialize
   *
   *  Invokes @c appInitialize() after setting up the basic features and 
   *  process controls. 
   */
  virtual StatusCode initialize();
  /*! @brief Default algorithm execution 
   *
   *  Invokes @c appExecute(...) according to the process control switches.
   */
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual const std::string& getFeatureStoreKey() const;

protected:

  virtual StatusCode appInitialize();
  virtual StatusCode appExecute(EventFeatureStore* pStore,
				const INavigable4MomentumCollection* pColl);
  virtual StatusCode appExecute(EventFeatureStore* pStore);
  virtual StatusCode appFinalize();

  virtual void executeMonitor(const std::string& toolName,
			      const std::string& toolType,
			      StatusCode         checkOut);

  virtual void executeSummary(bool cleanUp=false);

  const std::string& collectionKey()   const;
  const std::string& featureStoreKey() const;

  bool haveFeatureTools()    const;
  bool haveInputCollection() const;
  bool createFeatureStore()  const;

  StatusCode recordStore(EventFeatureStore* pStore,const std::string& sgKey);

private:

  std::string m_inputCollectionKey;
  bool        m_haveInputCollection;

  std::string m_eventFeatureStoreKey;
  bool        m_createFeatureStore;

  typedef ToolHandleArray<IEventFeatureTool> tool_array_t;
  typedef tool_array_t::iterator             tool_iterator_t;
  typedef tool_array_t::const_iterator       tool_const_iterator_t;

  tool_array_t m_eventFeatureTools;
  bool         m_haveFeatureTools;

  typedef std::pair<std::string,std::string>   tag_t;
  typedef std::pair<unsigned int,unsigned int> data_t;
  typedef std::map<tag_t,data_t>               map_t;

  map_t m_executeStats;
};

/*! @class EventFeatureAlgorithmBase 
 *
 *  Basic implementations for managing and executing @c IEventFeatureTool
 *  typed tools. This implementation baseclass provides a complete 
 *  implementation of the @c IEventFeatureAlgorithm interface and of all 
 *  virtual functions in the @c Algorithm class (here used through the 
 *  @c AthAlgorithm sub-class). 
 *
 *  Sub-class should implement the @c appInitialize() , @c appExecute(...) and 
 *  @c appFinalize() methods as needed and if the baseclass behaviour in
 *  @c initialize() , @c execute() and @c finalize() methods are desired. 
 */
inline const std::string& EventFeatureAlgorithmBase::collectionKey() const
{ return m_inputCollectionKey; }

inline const std::string& EventFeatureAlgorithmBase::featureStoreKey() const
{ return m_eventFeatureStoreKey; }

inline bool EventFeatureAlgorithmBase::haveFeatureTools() const
{ return m_haveFeatureTools; }

inline bool EventFeatureAlgorithmBase::haveInputCollection() const
{ return m_haveInputCollection; }

inline bool EventFeatureAlgorithmBase::createFeatureStore() const
{ return m_createFeatureStore; }

inline StatusCode 
EventFeatureAlgorithmBase::recordStore(EventFeatureStore* pStore,
				       const std::string& sgKey)
{ return evtStore()->record(pStore,sgKey); }
#endif


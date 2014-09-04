// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_EVENTFEATURETOOLBASE_H
#define EVENTSHAPETOOLS_EVENTFEATURETOOLBASE_H

#include "StoreGate/DataHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloUtils/CaloClusterSignalState.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "JetUtils/JetSignalStateHelper.h"

#include "EventShapeEvent/EventFeature.h"
#include "EventShapeEvent/EventFeatureStore.h"

#include "EventShapeUtils/EventShapeHelpers.h"

#include "EventShapeInterface/StaticConstants.h"
#include "EventShapeInterface/IEventFeatureTool.h"
#include "EventShapeInterface/IEventFeatureAlgorithm.h"

#include <vector>
#include <sstream>
#include <iomanip>

template<class INTERFACE=IEventFeatureTool>
class EventFeatureToolBase : public AthAlgTool, 
			     virtual public INTERFACE
{
public:

  /*! Publish interface type */
  typedef INTERFACE interface_t;

  /*! @brief Default tool constructor */
  EventFeatureToolBase(const std::string& type,const std::string& name,
		       const IInterface* pParent);
  /*! @brief Implementation base class destructor */
  virtual ~EventFeatureToolBase();

  /*! @brief Public interface to initizalize base tool
   * 
   *  Provides common initialization for all derived tools. <i>This 
   *  implementation should not be changed by clients!</i> The client
   *  hook @c appInitialize() is called internally.
   *
   *  @return @c StatusCode::FAILURE if initialization failed, else 
   *          @c StatusCode::SUCCESS . 
   */
  virtual StatusCode initialize();
  /*! @brief Public interface to execute base tool
   * 
   *  Provides common functionality for execution of client provided 
   *  implementations. <i>This 
   *  implementation should not be changed by clients!</i> The client
   *  hook @c appexecute(...) with the same parameter is called internally.
   *
   *  This method is to be invoked for the calculation of event features,
   *  the derived tool collects all the required data to perform this 
   *  calculation.  
   *
   *  @param[in] pStore pointer to modifiable @c EventFeatureStore . 
   *
   *  @return @c StatusCode::SUCCESS in all cases.
   */
  virtual StatusCode execute(EventFeatureStore* pStore);
  /*! @brief Public interface to execute base tool
   * 
   *  Provides common functionality for execution of client provided 
   *  implementations. <i>This 
   *  implementation should not be changed by clients!</i> The client
   *  hook @c appexecute(...) with the same parameter is called internally.
   *
   *  This method is to be invoked for the calculation of event features,
   *  the derived tool collects all the required data to perform this 
   *  calculation.  
   *
   *  @param[in] pStore pointer to modifiable @c EventFeatureStore . 
   *  @param[in] pColl pointer to non-modifiable collection of 
   *                   @c INavigable4Momentum typed objects
   *
   *  @return @c StatusCode::SUCCESS in all cases.
   */
  virtual StatusCode execute(EventFeatureStore* pStore,
			     const INavigable4MomentumCollection* pColl);
  /*! @brief Public interface to finalize base tool
   * 
   *  Provides common finalization for all derived tools. <i>This 
   *  implementation should not be changed by clients!</i> The client
   *  hook @c appFinalize() is called internally.
   *
   *  @return @c StatusCode::FAILURE if finalization failed, else 
   *          @c StatusCode::SUCCESS . 
   */
  virtual StatusCode finalize();

  // /*! @brief Access to event feature
  //  * 
  //  *  Implementation of corresponding @c IEventFeatureTool::eventFeature
  //  *  interface. Simple look-up of scalar value in @c EventFeatureStore by
  //  *  default, but can implement more complex feature construction as 
  //  *  implemented by sub-class tool.
  //  *
  //  *  @return @c true if feature tagged @c tag found in store, else @c false
  //  *
  //  *  @param[in]  tag feature tag in @c EventFeatureStore
  //  *  @param[out] feature reference to modifiable scalar feature store
  //  */
  // virtual bool eventFeature(const std::string& tag, double& feature);
  /*! @brief Access to event feature
   *
   *  Implementation of corresponding @c IEventFeatureTool::eventFeature
   *  interface. Simple look-up of scalar value in @c EventFeatureStore by
   *  default, but can implement more complex feature construction as 
   *  implemented by sub-class tool.
   *
   *  @return @c true if features tagged @c tag found in store, else @c false
   *
   *  @param[in] tag feature tag in @c EventFeatureStore
   *  @param[out] features reference to modifiable vector feature store
   */
  virtual bool eventFeature(const std::string&   tag, 
			    std::vector<double>& features);
  /*! @brief Access to event feature
   *
   *  Implementation of corresponding @c IEventFeatureTool::eventFeature
   *  interface. Simple look-up of scalar value in @c EventFeatureStore by
   *  default, but can implement more complex feature construction as 
   *  implemented by sub-class tool.
   *
   *  @return @c true if @C EventFeature tagged @c tag found in store, else 
   *          @c false
   *
   *  @param[in] tag feature tag in @c EventFeatureStore
   *  @param[out] pFeature reference to variable holding pointer to 
   *                       non-modifiable @c EventFeature object
   */
  virtual bool eventFeature(const std::string&   tag,
			    const EventFeature*& pFeature);
  // /*! @brief Access to event feature
  //  *
  //  *  Implementation of corresponding @c IEventFeatureTool::eventFeature
  //  *  interface. Simple look-up of scalar value in @c EventFeatureStore by
  //  *  default, but can implement more complex feature construction as 
  //  *  implemented by sub-class tool.
  //  *
  //  * @return @c true if feature of type @c type and tagged @c tag found in 
  //  *         store, else @c false
  //  * 
  //  * @param[in]  type feature type in @c EventFeatureStore
  //  * @param[in]  tag feature tag in @c EventFeatureStore
  //  * @param[out] feature reference to modifiable scalar feature store
  //  */
  // virtual bool eventFeature(EventFeature::FeatureType type,
  // 			    const std::string& tag, double& feature);
  /*! @brief Access to event feature
   *
   *  Implementation of corresponding @c IEventFeatureTool::eventFeature
   *  interface. Simple look-up of scalar value in @c EventFeatureStore by
   *  default, but can implement more complex feature construction as 
   *  implemented by sub-class tool.
   *
   * @return @c true if features of type @c type and tagged @c tag found in 
   *         store, else @c false
   *
   * @param[in]  type feature type in @c EventFeatureStore
   * @param[in] tag feature tag in @c EventFeatureStore
   * @param[out] features reference to modifiable vector feature store
   */
  virtual bool eventFeature(EventFeature::FeatureType type,
			    const std::string& tag, 
			    std::vector<double>& features);
  /*! @brief Access to event feature
   *
   *  Implementation of corresponding @c IEventFeatureTool::eventFeature
   *  interface. Simple look-up of scalar value in @c EventFeatureStore by
   *  default, but can implement more complex feature construction as 
   *  implemented by sub-class tool.
   *
   *  @return @c true if @C EventFeature tagged @c tag found in store, else 
   *          @c false
   *
   *  @param[in]  type feature type in @c EventFeatureStore
   *  @param[in] tag feature tag in @c EventFeatureStore
   *  @param[out] pFeature reference to variable holding pointer to 
   *                       non-modifiable @c EventFeature object
   */
  virtual bool eventFeature(EventFeature::FeatureType type,
			    const std::string& tag,
			    const EventFeature*& pFeature);

private:

  /*! @brief Tool property: @c StoreGate key to input 
   *         @c INavigable4MomentumCollection for feature calculation
   */
  std::string                 m_collectionKey;
  /*! @brief Tool property: @c EventFeatureStore internal key for tagging a 
   *         specific @c EventFeatur */
  std::string                 m_eventFeatureKey;
  /*! @brief Tool property: @c StoreGate key for input or output 
   *         @c EventFeatureStore */
  std::string                 m_eventFeatureStoreKey;

  /*! @brief Tool property: key for requested signal state */
  std::string                 m_signalStateKey;
  /*! @brief Tool property: requested feature type */
  std::string                 m_featureTypeKey;

  /*! @brief Tool request: signal state */
  P4SignalState::State        m_signalState;
  /*! @brief Tool request: requested feature type */
  EventFeature::FeatureType   m_featureType;

  /*! @brief Tool process flag: flags non-default signal state (if @c false ) */
  bool                        m_signalStateIsDefault;
  /*! @brief Tool process flag: indicates generating feature mode 
   *         (if @c true ) */
  bool                        m_generateFeature;

  /*! @brief Tool internals: return handle book-keeping */
  std::map<std::string,unsigned int> m_returnMsg;

protected:

  /*! @brief Tool cache: pointer to actual @c EventFeatureStore */  
  DataHandle<EventFeatureStore> m_eventFeatureStore;

  /*! @brief Tool helper: retrieve generic collection from @c StoreGate */
  const INavigable4MomentumCollection* collection(const std::string& sgKey);
  /*! @brief Tool internals: return handle message tracking */
  unsigned int returnTracking(const std::string& msg);

public:

  /*! @brief Tool constant: default signal state tag */
  static std::string          m_defaultTypeKey;
  /*! @brief Tool constant: success return message */
  static std::string          m_successMsg;
  // /*! @brief Tool constant: invalid (@c double ) value tag */
  // static double               m_invalidValue;
  // /*! @brief Tool constant: @c NULL (@c double ) value tag */
  // static double               m_nullValue;

  /*! @brief Tool helper: access to invalid value tag */
  static double invalidValue();
  /*! @brief Tool helper: access to null value tag */
  static double nullValue();
  /*! @brief Tool helper: access to invalid key tag */
  static const std::string& invalidKey();
  /*! @brief Tool helper: access to default signal state tag */
  static const std::string& defaultStateKey();
  
protected:

  /*! @brief Tool abstract interface: sub-class initialize
   * 
   *  Not a true interface as a no-op default implementation is provided.
   *  Invoked from @c EventFeatureToolBase::intialize() . 
   */
  virtual StatusCode appInitialize();
  /*! @brief Tool abstract interface: sub-class execute
   *
   *  Abstract interface requires sub-class implementation. Invoked from 
   *  @c EventFeatureToolBase::execute(...) . 
   */
  virtual StatusCode appExecute(EventFeatureStore* pStore,
				const INavigable4MomentumCollection* pColl) = 0;
  /*! @brief Tool abstract interface: sub-class execute
   *
   *  Abstract interface requires sub-class implementation. Invoked from 
   *  @c EventFeatureToolBase::execute(...) . 
   */
  virtual StatusCode appExecute(EventFeatureStore* pStore) = 0;
  /*! @brief Tool abstract interface: sub-class finalize
   * 
   *  Not a true interface as a no-op default implementation is provided.
   *  Invoked from @c EventFeatureToolBase::finalize() . 
   */
  virtual StatusCode appFinalize();

  /*! @brief Tool helper: sub-class accessor to non-modifiable collection key */
  const std::string& collectionKey() const;
  /*! @brief Tool helper: sub-class accessor to modifiable collection key */
  std::string& collectionKey();             
  /*! @brief Tool helper: sub-class accessor to non-modifable feature key */
  const std::string& featureKey() const;
  /*! @brief Tool helper: sub-class access to modifiable feature key */
  std::string& featureKey();
  /*! @brief Tool helper: sub-class accessor to non-modifable feature store
      key */
  const std::string& featureStoreKey() const;
  /*! @brief Tool helper: sub-class access to modifiable feature store key */
  std::string& featureStoreKey();
  /*! @brief Tool helper: sub-class access to non-modifiable feature type key*/
  const std::string& featureTypeKey() const; 
  /*! @brief Tool helper: sub-class access to set feature type key
   *
   *  Invoking this method also changes the feature type, in addition to its
   *  name.
   * 
   *  @param[in] key feature type name
   *
   *  @return @c true is @c key represents a known feature type, else @c false .
   */
  bool setFeatureTypeKey(const std::string& key); 
  /*! @brief Tool helper: sub-class access to non-modifiable signal state key */
  const std::string& signalStateKey() const;
  /*! @brief Tool helper: sub-class access to set signal state key 
   *
   *  Invoking this method also changes the signal state, in addition to its
   *  name.
   * 
   *  @param[in] key signal state name
   *
   *  @return @c true is @c key represents a known signal state, else @c false .
   */
  bool setSignalStateKey(const std::string& key);

  /*! @brief Tool helper: sub-class access to non-modifiable signal state 
      default tag */
  bool signalStateIsDefault() const;
  /*! @brief Tool helper: sub-class access to set signal state default tag */
  void setSignalStateIsDefault(bool tag);
  /*! @brief Tool helper: sub-class accesss to non-modifiable feature generator
       tag */
  bool generateFeature() const;
  /*! @brief Tool helper: sub-class access to set feature generator tag */
  void setGenerateFeature(bool tag);

  /*! @brief Tool helper: sub-class access to non-modifiable signal state 
      request */
  P4SignalState::State signalState() const;
  /*! @brief Tool helper: sub-class access to set signal state */
  void setSignalState(P4SignalState::State state);
  /*! @brief Tool helper: sub-class access to non-modifiable feature type */
  EventFeature::FeatureType featureType() const;
  /*! @brief Tool helper: sub-class access to set feature type */
  void setFeatureType(EventFeature::FeatureType type);

  /*! @brief Tool helper: method return handler (default) */
  virtual StatusCode returnHandle();
  /*! @brief Tool helper: method return handler with message */
  virtual StatusCode returnHandle(const std::string& msg);

  /*! @brief Tool helper: check requested feature type against configured type 
   */
  bool checkFeatureType(EventFeature::FeatureType type);
};

/*! @class EventFeatureToolBase
 *
 *  @brief Provides default implementations for @c IEventFeatureTool interface.
 *
 *  This tool can be implemented as feature generator or feature retrieval.
 *
 *  <b>Feature generation</b><br/>
 *
 *  There are two different ways of implementing the @c EventFeature
 *  generation:
 *
 *  -# <code> execute(EventFeatureStore* pStore)</code> can be used for 
 *     generating one or more @c EventFeature (s) independent of input from 
 *     @c StoreGate
 *     (A) or using the property @c "InputCollectionKey" to retrieve a
 *     data collection from @c StoreGate (B). Properties are 
 *     -# @p EventFeatureKey [required,no default] - identifies 
 *                               @c EventFeatureStore in @c StoreGate; if 
 *                          default in option (A), it is expected that 
 *                          the @c EventFeature is calculated without 
 *                          input from @c StoreGate
 *     -# @p EventFeatureType [optional,default @c EventFeature::UNKNOWN] -
 *                          sets @c EventFeature type in @c EventFeatureStore 
 *     -# @p InputCollectionKey [optional,default 
 *                          @c EventFeatureToolBase::m_invalidKey ] - if set 
 *                          invokes collection retrieval from @c StoreGate 
 *                          (required for option (B)).
 *     -# @p SignalState [optional,default 
 *                          @c EventFeatureToolBase::m_defaultStateKey ] -
 *                          controls signal state for feature calculation in
 *                          @C ISignalState typed objects
 *  -# <code> execute(EventFeatureStore* pStore,const 
 *                    INavigable4MomentumCollection* pColl)</code> can be used
 *     to generate @c EventFeature (s) from the input collection pointed to
 *     by @c pStore . Properties are
 *     -# @p EventFeatureKey [required,no default] - identifies 
 *                               @c EventFeatureStore in @c StoreGate; if 
 *                          default in option (A), it is expected that 
 *                          the @c EventFeature is calculated without 
 *                          input from @c StoreGate
 *     -# @p EventFeatureType [optional,default @c EventFeature::UNKNOWN] -
 *                          sets @c EventFeature type in @c EventFeatureStore 
 *     -# @p InputCollectionKey [ignored,default 
 *                          @c EventFeatureToolBase::m_invalidKey ] - no 
 *                          influence on method behaviour
 *     -# @p SignalState [optional,default @p "CALIBRATED" ] -
 *                          controls signal state for feature calculation in
 *                          @C ISignalState typed objects
 *
 *  <b>Feature retrieval</b>
 *
 *  In this state the tool retrieves @c EventFeature (s) from StoreGate
 *  (A) or from internal cache (B), where (B) is only possible in connection
 *  with on the fly generation in any of the two configurations discussed
 *  above (meaning one of the @c execute(...) methods must be invoked 
 *  <i>before</i> using any of the access methods.
 *
 *  Access to event feature data in an @c EventFeature Store or, if implemented 
 *  by clients, features derived from data in an @c EventFeature store is
 *  possible using one or several of the @c eventFeature(...) methods.   
 *
 *  -# Option (A):@n
 *     In this case a @c EventFeatureStore is retrieved from @c StoreGate
 *     and @c EventFeature (s) are extracted from this store using any of the
 *     @c eventFeature(...) access methods. <i>It is not required to
 *     invoke @c execute(...) before accessing the data!</i> Properties are
 *     -# @p EventFeatureKey [required,no default] - identifies 
 *                               @c EventFeatureStore in @c StoreGate to be
 *                               retrieved
 *     -# @p EventFeatureType [optional,default @c EventFeature::UNKNOWN] -
 *                          sets @c EventFeature type to be retrieved if
 *                          accessors @c eventFeature(...) without explicit
 *                          type indicator are used
 *     -# @p InputCollectionKey [ignored,default 
 *                          @c EventFeatureToolBase::m_invalidKey ] - no 
 *                          influence on tool behaviour
 *     -# @p SignalState [ignored, default @p "CALIBRATED" ] - no influence
 *                          on tool behaviour
 *  -# Option (B):@n
 *     This option requires one of the @c execute(...) methods discussed
 *     above to be invoked prior to invoking any access method. The 
 *     requested @c EventFeature data is accessed from internal cache 
 *     and reflects the configuration used for generation (see above).
 *                          
 *     
 *
 *  This virtual implementation base class requires sub-classes to implement
 *  the @c appExecute() application interfaces, and provides default no-op 
 *  behaviour for the client hooks @c appInitialize() and @c appFinalize() . 
 *  These three methods should be implemented or superseded by clients to 
 *  implement their specific behaviours, <b>not</b> the corresponding public 
 *  interfaces @c EventFeatureToolBase::initialize(), 
 *  @c EventFeatureToolBase::execute(...), and 
 *  @c EventFeatureToolBase::finalize() , which all provide basic 
 *  implementations useful for all clients and are the public interfaces to
 *  calculate event features. They internally call the application interfaces
 *  to invoke the client provided functionalities.
 */
template<class INTERFACE>
inline double EventFeatureToolBase<INTERFACE>::invalidValue()
{ return EventShapeConstants::invalidValue(); }

template<class INTERFACE>
inline double EventFeatureToolBase<INTERFACE>::nullValue()
{ return EventShapeConstants::nullValue(); }

template<class INTERFACE>
inline const std::string& EventFeatureToolBase<INTERFACE>::invalidKey()
{ return EventShapeConstants::invalidKey(); }

template<class INTERFACE>
inline const std::string& EventFeatureToolBase<INTERFACE>::defaultStateKey()
{ return EventShapeConstants::defaultStateKey(); }

template<class INTERFACE>
inline const std::string& EventFeatureToolBase<INTERFACE>::collectionKey() const
{ return m_collectionKey; }
template<class INTERFACE>
inline std::string& EventFeatureToolBase<INTERFACE>::collectionKey()
{ return m_collectionKey; }
             
template<class INTERFACE>
inline const std::string& EventFeatureToolBase<INTERFACE>::featureKey() const
{ return m_eventFeatureKey; }
template<class INTERFACE>
inline std::string& EventFeatureToolBase<INTERFACE>::featureKey()
{ return m_eventFeatureKey; }

template<class INTERFACE>
inline const std::string& 
EventFeatureToolBase<INTERFACE>::featureStoreKey() const
{ return m_eventFeatureStoreKey; }
template<class INTERFACE>
inline std::string& EventFeatureToolBase<INTERFACE>::featureStoreKey()
{ return m_eventFeatureStoreKey; }

template<class INTERFACE>
inline const std::string& 
EventFeatureToolBase<INTERFACE>::featureTypeKey() const
{ return m_featureTypeKey; }

template<class INTERFACE>
inline const std::string& 
EventFeatureToolBase<INTERFACE>::signalStateKey() const
{ return m_signalStateKey; }

template<class INTERFACE>
inline bool EventFeatureToolBase<INTERFACE>::signalStateIsDefault() const
{ return m_signalStateIsDefault; }
template<class INTERFACE>
inline void EventFeatureToolBase<INTERFACE>::setSignalStateIsDefault(bool tag)
{ m_signalStateIsDefault = tag; }

template<class INTERFACE>
inline bool EventFeatureToolBase<INTERFACE>::generateFeature() const
{ return m_generateFeature; }
template<class INTERFACE>
inline void EventFeatureToolBase<INTERFACE>::setGenerateFeature(bool tag)
{ m_generateFeature = tag; }

template<class INTERFACE>
inline P4SignalState::State EventFeatureToolBase<INTERFACE>::signalState() const
{ return m_signalState; }

template<class INTERFACE>
inline EventFeature::FeatureType 
EventFeatureToolBase<INTERFACE>::featureType() const
{ return m_featureType; }

#include "EventShapeTools/EventFeatureToolBase.icc"
#endif

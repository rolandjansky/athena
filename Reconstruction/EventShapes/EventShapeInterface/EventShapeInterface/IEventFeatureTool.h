// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEINTERFACE_IEVENTFEATURETOOL_H
#define EVENTSHAPEINTERFACE_IEVENTFEATURETOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "NavFourMom/INavigable4MomentumCollection.h"

#include "EventShapeEvent/EventFeature.h"

#include <vector>
#include <string>

class EventFeatureStore;

static const InterfaceID IID_IEventFeatureTool("IEventFeatureTool",1,0);

class IEventFeatureTool : virtual public IAlgTool
{
public:

  /*! @brief Destructor */
  virtual ~IEventFeatureTool() { }

  /*! @brief Access to @c InterfaceID for derived tools */
  static const InterfaceID& interfaceID() { return IID_IEventFeatureTool; }

  /*! @brief Tool initialization */
  virtual StatusCode initialize() = 0;
  /*! @brief Tool execute method 
   *
   * Retrieves input collection according to tool configuration and calls
   * @c execute(...) with a pointer to this collection as parameter. The
   * @c EventFeatureStore is used to store the parameters needed to calculate
   * the event feature (if any).
   *
   * @param[in] pStore pointer to modifiable @c EventFeatureStore
   */
  virtual StatusCode execute(EventFeatureStore* pStore) = 0;
  /*! @brief Tool execute method with input reference 
   * 
   * Interface to tool implementation providing a pointer to the input
   * collection.
   *
   * @param[in] pStore pointer to modifiable @c EventFeatureStore 
   * @param[in] pColl pointer to non-modifiable collection of genetic 
   *                  input collection 
   */
  virtual StatusCode execute(EventFeatureStore* pStore,
			     const INavigable4MomentumCollection* pColl) = 0;
  /*! @brief Tool finalize */
  virtual StatusCode finalize() = 0;

  // /* @brief Retrieve scalar global event feature
  //  *
  //  * @return @c true if feature tagged @c tag found in store, else @c false
  //  * 
  //  * @param[in]  tag feature tag in @c EventFeatureStore
  //  * @param[out] feature reference to modifiable scalar feature store
  //  */
  // virtual bool eventFeature(const std::string& tag,double& feature) = 0;
  /*! @brief Retrieve vector event feature
   *
   * @return @c true if features tagged @c tag found in store, else @c false
   *
   * @param[in] tag feature tag in @c EventFeatureStore
   * @param[out] features reference to modifiable vector feature store
   */
  virtual bool eventFeature(const std::string& tag,
			    std::vector<double>& features) = 0;
  /*! @brief Retrieve event feature object
   *
   * @return @c true if @C EventFeature tagged @c tag found in store, else 
   *         @c false
   *
   * @param[in] tag feature tag in @c EventFeatureStore
   * @param[out] pFeature reference to variable holding pointer to 
   *                      non-modifiable @c EventFeature object
   */
  virtual bool eventFeature(const std::string&   tag,
			    const EventFeature*& pFeature) = 0;

  // /* @brief Retrieve scalar global event feature
  //  *
  //  * @return @c true if feature of type @c type and tagged @c tag found in 
  //  *         store, else @c false
  //  * 
  //  * @param[in]  type feature type in @c EventFeatureStore
  //  * @param[in]  tag feature tag in @c EventFeatureStore
  //  * @param[out] feature reference to modifiable scalar feature store
  //  */
  // virtual bool eventFeature(EventFeature::FeatureType type,
  // 			    const std::string& tag,double& feature) = 0;
  /*! @brief Retrieve vector event feature
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
			    std::vector<double>& features) = 0;
  /*! @brief Retrieve event feature object
   *
   * @return @c true if @C EventFeature tagged @c tag found in store, else 
   *         @c false
   *
   * @param[in]  type feature type in @c EventFeatureStore
   * @param[in] tag feature tag in @c EventFeatureStore
   * @param[out] pFeature reference to variable holding pointer to 
   *                      non-modifiable @c EventFeature object
   */
  virtual bool eventFeature(EventFeature::FeatureType type,
			    const std::string&   tag,
			    const EventFeature*& evtFeature) = 0;
};

/*! @class IEventFeatureTool
 *
 *  @brief Interface for tools calculating and/or retrieving event features as
 *         functions of a feature type and a feature tag.
 */
#endif

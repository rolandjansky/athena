// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEEVENT_EVENTFEATURESTORE_H
#define EVENTSHAPEEVENT_EVENTFEATURESTORE_H

#include "DataModel/DataVector.h"

#include "CLIDSvc/CLASS_DEF.h"

#include "EventShapeEvent/EventFeature.h"

class EventFeatureStore : public DataVector<EventFeature>
{
public:

  template<class T>
  friend class EventFeatureStoreConverterBase;

  /*! @brief Standard @c DataVector constructor */
  EventFeatureStore(SG::OwnershipPolicy own=SG::OWN_ELEMENTS);
  /*! @brief Baseclass destructor */
  virtual ~EventFeatureStore();

  /*! @brief Retrieve method for @c EventFeature (@c const retrieve)
   * 
   *  The requested @c EventFeature is characterized by a type and a tag.
   *
   *  @param[in]  type     feature type 
   *  @param[in]  tag      feature tag
   *  @param[out] pFeature pointer to non-modifiable @c EventFeature object
   *                       retrieved from @c EventFeatureStore
   *  
   *  @return @c true if retrieval was successful, i.e. the @c EventFeature
   *          object identified by @c type and @c tag existed in the store;
   *          @c false and <code>pFeature = 0</code> if object not known. 
   */
  virtual bool retrieve(EventFeature::FeatureType type,
			const std::string& tag,
			const EventFeature* &pFeature) const;
  /*! @brief Retrieve method for @c EventFeature
   * 
   *  The requested @c EventFeature is characterized by a type and a tag.
   *
   *  @param[in]  type     feature type 
   *  @param[in]  tag      feature tag
   *  @param[out] pFeature pointer to modifiable @c EventFeature object
   *                       retrieved from @c EventFeatureStore
   *  
   *  @return @c true if retrieval was successful, i.e. the @c EventFeature
   *          object identified by @c type and @c tag existed in the store;
   *          @c false and <code>pFeature = 0</code> if object not known. 
   */
  virtual bool retrieve(EventFeature::FeatureType type,
			const std::string& tag,
			EventFeature* &pFeature);

  /*! @brief Find @c EventFeature by identifiers (@c const access)
   * 
   *  @param[in]  type     feature type 
   *  @param[in]  tag      feature tag
   *
   *  @return @c DataVector<EventFeature>::end() if object with given 
   *          identifiers not found.  
   */
  virtual const_iterator find(EventFeature::FeatureType type,
			      const std::string& tag) const;
  /*! @brief Find @c EventFeature by identifiers
   * 
   *  @param[in]  type     feature type 
   *  @param[in]  tag      feature tag
   *
   *  @return @c DataVector<EventFeature>::end() if object with given 
   *          identifiers not found.  
   */
  virtual iterator       find(EventFeature::FeatureType type,
			      const std::string& tag);
  /*! @brief Find iterator for given @c EventFeature (@c const access)
   *
   *  @param[in] pFeature pointer to non-modifiable @c EventFeature object
   * 
   *  @return @c DataVector<EventFeature>::end() if object pointed to not 
   *          found.  
   */
  virtual const_iterator find(const EventFeature* pFeature) const;
  /*! @brief Find iterator for given @c EventFeature
   *
   *  @param[in] pFeature pointer to modifiable @c EventFeature object
   * 
   *  @return @c DataVector<EventFeature>::end() if object pointed to not 
   *          found.  
   */
  virtual iterator       find(EventFeature* pFeature);

private:

  //  static EventFeature m_invalidConstReference;
  //  static EventFeature m_invalidReference;
};

CLASS_DEF(EventFeatureStore, 64059916, 1)

/*! @class EventFeatureStore
 *
 *  @brief Storable container for @c EventFeature objects
 *
 *  This is a @c DataVector<EventFeature> extended with some support for
 *  searches by feature type and tag.
 */
#endif

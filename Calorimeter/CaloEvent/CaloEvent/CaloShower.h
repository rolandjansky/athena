/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOSHOWER_H
#define CALOEVENT_CALOSHOWER_H

#include "CaloEvent/CaloSamplingData.h"
#include "CaloEvent/CaloClusterMomentStore.h"

class CaloShower
{
 public:

  /*! \brief Moment store type */
  typedef CaloClusterMomentStore                 moment_store;

  /*! \brief Sampling data store type */
  typedef CaloSamplingData                       sampling_store;
 
  /*! \brief Constructor */
  CaloShower() { };
  explicit CaloShower(const CaloShower* pShower);
  explicit CaloShower(const CaloShower& rShower);
  /*! \brief Destructur */
  virtual ~CaloShower() { };

  /*! \brief Access cluster moment store
   * 
   *  Returns const reference to moment store.
   */
  const moment_store& getMomentStore() const;

  /*! \brief Access cluster moment store
   *
   *  \overload
   *
   *  Returns non-const reference to moment store.
   */
  moment_store& getMomentStore();

  /*! \brief Set moment store
   *
   *  \param rMomStore reference to a non-modifiable moment store
   *
   *  Makes a copy of the store to allow downstream modifications. Depreciated.
   */
  void setMomentStore(const moment_store& rMomStore);

  /*! \brief Retrieves the non-modifiable sampling data store */
  const sampling_store& getSamplingStore() const;
  /*! \brief Retrieves the modifiable sampling data store */
  sampling_store& getSamplingStore();
  /*! \brief Set the sampling data store */
////  void setSamplingStore(const sampling_store& rSamStore);

 private:

  moment_store   m_momentStore;
  sampling_store m_samplingStore;

  CaloShower& operator= (const CaloShower& rShower);
}; 
/*! \class CaloShower
 *  \brief Data class for cluster variables associated with a \a CaloCluster 
 * 
 *   It is implemented as a concrete base class. 
 */
inline CaloShower::moment_store& CaloShower::getMomentStore() 
{ return m_momentStore; }
inline const CaloShower::moment_store& CaloShower::getMomentStore() const
{ return m_momentStore; }
inline void CaloShower::setMomentStore(const moment_store& rMomStore)
{ m_momentStore = rMomStore; }
inline CaloShower::sampling_store& CaloShower::getSamplingStore()
{ return m_samplingStore; }
inline const CaloShower::sampling_store& CaloShower::getSamplingStore() const
{ return m_samplingStore; }
#endif
 

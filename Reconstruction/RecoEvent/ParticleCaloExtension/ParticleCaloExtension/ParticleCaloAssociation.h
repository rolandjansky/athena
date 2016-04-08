/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_PARTICLECALOASSOCIATION_H
#define REC_PARTICLECALOASSOCIATION_H

#include <vector>
#include "AthContainers/DataVector.h"

namespace Trk {
  class CaloExtension;
}

namespace Rec {
  
  /** class storing calorimeter cell/cluster association with IParticle objects */
  template<class T>
  class ParticleCaloAssociation {
  public:
    /** typedef of Cell vector */
    typedef std::vector<T> Data;

    /** constructor taking CaloExtension, a vector of cells and a cone size as arguments */
    ParticleCaloAssociation( const Trk::CaloExtension& caloExtension, Data&& data, float coneSize );

    /** return calo extension */
    const Trk::CaloExtension& caloExtension() const;

    /** return vector of associated objects */
    const Data& data() const;

    /** return cone-size used for association */
    float associationConeSize() const;
    
    /** move constructor */
    ParticleCaloAssociation(ParticleCaloAssociation&& other);

    /** move assignment operator */
    ParticleCaloAssociation& operator=(ParticleCaloAssociation&& other);

    /** update data */
    void updateData( Data&& data, float coneSize );


  private:
    /** no default constructor */
    ParticleCaloAssociation() = delete;
    
    /** no copy constructor */
    ParticleCaloAssociation(const ParticleCaloAssociation&) = delete;

    /** no assignment operator */
    ParticleCaloAssociation& operator=(const ParticleCaloAssociation&) = delete;

    /** CaloExtension */
    const Trk::CaloExtension* m_caloExtension;

    /** cell information */
    Data m_data;   // vector of associated objects

    /** cone size used for association */
    float m_associationConeSize;

  };
  
  template<class T>
  inline float ParticleCaloAssociation<T>::associationConeSize() const {
    return m_associationConeSize;
  } 

  template<class T>
  inline const Trk::CaloExtension& ParticleCaloAssociation<T>::caloExtension() const {
    return *m_caloExtension;
  }

  template<class T>
  inline const typename ParticleCaloAssociation<T>::Data& ParticleCaloAssociation<T>::data() const {
    return m_data;
  }

  template<class T>
  inline void ParticleCaloAssociation<T>::updateData( Data&& data, float coneSize ) {
    m_data = data;
    m_associationConeSize = coneSize;
  }

  template<class T>
  inline ParticleCaloAssociation<T>::ParticleCaloAssociation( const Trk::CaloExtension& caloExtension, Data&& data, float coneSize ) :
    m_caloExtension(&caloExtension),
    m_data(data),
    m_associationConeSize(coneSize) {

  }


}

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_PARTICLECALOEXTENSION_H
#define XAOD_PARTICLECALOEXTENSION_H

#include "xAODTracking/TrackingPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#ifndef XAOD_ANALYSIS
// Athena includes
#include "TrkParameters/TrackParameters.h"
#endif // not XAOD_ANALYSIS

#include <vector>
class CaloCell;

namespace xAOD {

  /** class storing calorimeter information associated with track based objects */
  class ParticleCaloExtension {
  public:
    /** constructor taking calo extrapolation as input. All vectors should be of the same length */
    ParticleCaloExtension( float charge,
                           std::vector<std::vector<float> >&& parameters,
                           std::vector<std::vector<float> >&& parametersCovariance,
                           std::vector<int>&& identifiers );

    /** destructor */
    ~ParticleCaloExtension();

    /** no copy constructor */
    ParticleCaloExtension(const ParticleCaloExtension&) = delete;

    /** no assignment operator */
    ParticleCaloExtension& operator=(const ParticleCaloExtension&) = delete;

    /// Returns the number of additional parameters stored in the Particle.
    size_t numberOfParameters() const;

    /// Returns the track parameter vector at 'index'.
    const CurvilinearParameters_t trackParameters(unsigned int index) const;

    /// @brief Return the ParameterPosition of the parameters at 'index'.
    int parameterIdentifier(unsigned int index) const;

    /// @brief fill the matrix with the covariance at position 'index', returns false if the parameters at 'index' does not have a covariance
    bool trackParameterCovarianceMatrix(ParametersCovMatrix_t& matrix, unsigned int index) const;

#ifndef XAOD_ANALYSIS
    /// @brief Returns a curvilinear representation of the parameters at 'index'.
    /// @note This is only available in Athena.
    const Trk::CurvilinearParameters curvilinearParameters(unsigned int index) const;
#endif // not XAOD_ANALYSIS

    /** return whether cells were already associated or not */
    bool cellsAreAssociated() const;

    /** return vector of cells */
    const std::vector<CaloCell*>& caloCells() const;

    /** set vector of cells */
    void setCaloCells(const std::vector<CaloCell*>& cells);

  private:
    /// charge
    float m_charge;

    /// parameters of the intersections with detector layers
    std::vector<std::vector<float> > m_parameters;

    /// covariance of parameters of the intersections with detector layers
    std::vector<std::vector<float> > m_parametersCovariance;

    /// identifiers of the intersections with detector layers
    std::vector<int> m_identifiers;

    /** cell information */
    bool                   m_cellsAreSet; // bool to store whether already set
    std::vector<CaloCell*> m_caloCells;   // vector of cells
  };

  /// Returns the number of additional parameters stored in the Particle.
  inline size_t ParticleCaloExtension::numberOfParameters() const {
    return m_parameters.size();
  }

  /// Returns the track parameter vector at 'index'.
  inline const CurvilinearParameters_t ParticleCaloExtension::trackParameters(unsigned int index) const {
    CurvilinearParameters_t tmp;
    tmp << m_parameters[index][0],m_parameters[index][1],m_parameters[index][2],
      m_parameters[index][3],m_parameters[index][4],m_parameters[index][5];
    return tmp;
  }

  inline bool ParticleCaloExtension::trackParameterCovarianceMatrix(ParametersCovMatrix_t& cov, unsigned int index) const {
    const std::vector<float>& covVec = m_parametersCovariance[index];
    if( !covVec.empty() ) Amg::expand( covVec.begin(), covVec.end(),cov );
    else{
      cov.setIdentity();
      return false;
    }
    return true;
  }

  /// @brief Return the ParameterPosition of the parameters at 'index'.
  inline int ParticleCaloExtension::parameterIdentifier(unsigned int index) const {
    return m_identifiers[index];
  }

  inline bool ParticleCaloExtension::cellsAreAssociated() const {
    return m_cellsAreSet;
  }

  inline const std::vector<CaloCell*>& ParticleCaloExtension::caloCells() const {
    return m_caloCells;
  }

  inline void ParticleCaloExtension::setCaloCells(const std::vector<CaloCell*>& cells) {
    m_caloCells = cells;
    m_cellsAreSet=true;
  }

#ifndef XAOD_ANALYSIS
  inline const Trk::CurvilinearParameters ParticleCaloExtension::curvilinearParameters(unsigned int index) const {

    // copy the correct values into the temp matrix
    ParametersCovMatrix_t* cov = 0;
    if( !m_parametersCovariance[index].empty() ) {
      cov = new ParametersCovMatrix_t();
      trackParameterCovarianceMatrix(*cov,index);
    }
    // retrieve the parameters to build the curvilinear frame
    Amg::Vector3D pos( m_parameters[index][0],m_parameters[index][1],m_parameters[index][2]);
    Amg::Vector3D mom(m_parameters[index][3],m_parameters[index][4],m_parameters[index][5]);
    Trk::CurvilinearParameters param(pos,mom,m_charge,cov);

    return param;
  }
#endif // not XAOD_ANALYSIS

}

#include "xAODCore/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
typedef DataVector<xAOD::ParticleCaloExtension> ParticleCaloExtensionCollection;
CLASS_DEF(ParticleCaloExtensionCollection, 1251279391 , 1 )

#endif

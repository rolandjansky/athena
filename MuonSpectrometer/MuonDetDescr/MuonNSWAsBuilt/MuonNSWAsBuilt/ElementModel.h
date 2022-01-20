/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_ELEMENTMODEL_H
#define MUONNSWASBUILT_ELEMENTMODEL_H

#include <vector>
#include "GeoPrimitives/GeoPrimitives.h"

namespace NswAsBuilt {

  /**
  * Pure abstract class representing the deformation model of an Element
  *
  * The sub-classes of ElementModel implement the transform method, which
  * converts a point (or a set of points) provided in local coordinates to the
  * higher-level coordinate system. This transformation is constituted of a
  * rigid component (translations and rotation) and possibly deformations.
  *
  * The transform method makes use of a ParameterVector structure, which is a
  * wrapper around the std::vector of model parameters, and holds in addition
  * the cache-able part (the rigid component) of the transform represented by
  * this model.
  *
  * The precise definition of the ParameterVector parameters is specific to the
  * concrete implementations of ElementModel
  *
  * The ParameterVector is not saved in this class or its sub-classes, and must
  * be provided externally (i.e. by the class Element) on the call to the
  * method transform.
  */
  class ElementModel {
    public:
      using ipar_t = unsigned int;
      using VectorSet = Eigen::Matrix<double, 3, Eigen::Dynamic, Eigen::ColMajor|Eigen::AutoAlign, 3, 5>;
      using VectorSetRef = Eigen::Ref<VectorSet>;

      virtual ~ElementModel() = default;

     /* The ParameterVector: a wrapper around a std::vector<double> containing
      * the actual model parameters.
      * In addition, this struct may save the cache-able rigid component of the
      * transformation represented by the parameter vector */
      struct ParameterVector {
        std::vector<double> parameters;
        Eigen::Affine3d transformCache;
        bool transformCacheValid = false;

        // Contructed: provide the number of parameters
        ParameterVector(size_t npars) : parameters(npars, 0.0) {}

        // accessors
        double operator[] (size_t ipar) const { return parameters[ipar]; }
        double& operator[] (size_t ipar) { return parameters[ipar]; }
      };

     /* The number of parameters used */
      virtual int nParameters() const = 0;

     /* Transform a set of vectors expressed in local frame, stored in a matrix */
      virtual void transform(const ParameterVector& parvec, VectorSetRef local) const = 0;

     /* Cache the rigid component of this deformation model */
      virtual void cacheTransform(ParameterVector& parvec) const = 0;

     /* Helper methods to convert parameter index to string representation */
      virtual ipar_t getParameterIndex(const std::string& parname) const = 0;
      virtual std::string getParameterName(ipar_t ipar) const = 0;
  };
}

#endif


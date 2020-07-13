/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ProjectionMatricesSet.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_PROJECTIONMATRICESSET_H
#define TRKEVENTPRIMITIVES_PROJECTIONMATRICESSET_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
// Trk
#include "TrkEventPrimitives/ParamDefs.h"

/** standard namespace for Tracking*/
namespace Trk {


  /**@class ProjectionMatricesSet 
     @brief the matrices to access the variably-dimensioned
          local parameters and map them to the defined
          five track parameters.
    
     @author Andreas.Salzburger@cern.ch
    */
     
  class ProjectionMatricesSet {

    public:
      /**Explicit constructor for 1-dimensional vector */
      ProjectionMatricesSet(int maxdim);
      
      /**Expansion matrix return*/
      const Amg::MatrixX& expansionMatrix(int mtx) const;
      
      /**Reduction matrix return*/
      const Amg::MatrixX& reductionMatrix(int mtx) const;
      
      /**Accessors for members*/
      int accessor(int mtx, ParamDefs par) const;
      
    private:
      int                             m_maxdim;
      std::vector<Amg::MatrixX> m_expansions;
      std::vector<Amg::MatrixX> m_reductions;
      std::vector<std::vector<int>>   m_accessors;
     
  };    

inline const Amg::MatrixX& ProjectionMatricesSet::expansionMatrix(int mtx) const { return m_expansions[mtx]; }

inline const Amg::MatrixX& ProjectionMatricesSet::reductionMatrix(int mtx) const { return m_reductions[mtx]; }
    
inline int ProjectionMatricesSet::accessor(int mtx, ParamDefs par) const { return (par-(m_accessors[mtx])[par]); }

} // end of namespace


#endif // TRKEVENTPRIMITIVES_PROJECTIONMATRICESSET_H

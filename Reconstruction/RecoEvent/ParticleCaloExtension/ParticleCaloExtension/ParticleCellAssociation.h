/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_PARTICLECELLASSOCIATION_H
#define REC_PARTICLECELLASSOCIATION_H

#include "ParticleCaloExtension/ParticleCaloAssociation.h"
#include "ParticleCaloExtension/ParticleCellIntersection.h"
#include <vector>
#include <algorithm>

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

namespace Rec {
  
  /** class storing calorimeter cell association with IParticle objects */
  class ParticleCellAssociation : public ParticleCaloAssociation<const CaloCell*> {
  public:
    /** typedef for vector of cell intersections */
    typedef std::vector< std::pair<const CaloCell*,ParticleCellIntersection*> > CellIntersections;
    
    /** constructor */
    ParticleCellAssociation( const Trk::CaloExtension& caloExtension, Data&& data, float coneSize, 
                             const CellIntersections&& intersections, const CaloCellContainer* container=0 );

    /** destructor */
    virtual ~ParticleCellAssociation();

    /** access to all intersected cells */
    const CellIntersections& cellIntersections() const { return m_cellInteresections; }

    /** access to cell intersection of a give cell, returns zero if the cell was not intersected */
    const ParticleCellIntersection* cellIntersection( const CaloCell& cell ) const;

    /** access to container */
    const CaloCellContainer* container() const;

  private:
    /** no copying nor deletion */
    ParticleCellAssociation(const ParticleCellAssociation& ) = delete;
    const ParticleCellAssociation& operator=(const ParticleCellAssociation& ) = delete;

    /** list of intersected cells */
    CellIntersections m_cellInteresections;

    /** pointer to the container */
    const CaloCellContainer* m_container;

  };
  
  inline const ParticleCellIntersection* ParticleCellAssociation::cellIntersection( const CaloCell& cell ) const {
    auto pos = std::find_if( m_cellInteresections.begin(), m_cellInteresections.end(), 
                             [&]( const std::pair<const CaloCell*,ParticleCellIntersection*>& entry ){
                               return entry.first == &cell;
                             });
    if( pos != m_cellInteresections.end() ) return pos->second;
    return 0;
  }

  inline const CaloCellContainer* ParticleCellAssociation::container() const {
    return m_container;
  }
                                                
}

#endif

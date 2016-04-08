/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloExtension/ParticleCellAssociation.h"


namespace Rec {
  
  ParticleCellAssociation::ParticleCellAssociation( const Trk::CaloExtension& caloExtension, ParticleCellAssociation::Data&& data, 
                                                    float coneSize, const CellIntersections&& intersections, const CaloCellContainer* container ) :
    ParticleCaloAssociation<const CaloCell*>(caloExtension,std::move(data),coneSize), 
    m_cellInteresections(intersections), m_container(container)
  {
  
  }

  ParticleCellAssociation::~ParticleCellAssociation() {
    for( auto el : m_cellInteresections ){
      delete el.second;
    }
  }
}


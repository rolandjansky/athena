/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_PARTICLECELLINTERSECTION_H
#define REC_PARTICLECELLINTERSECTION_H

class CaloCell;

namespace Rec {
  
  /** class storing information on the intersection of a track with a cell */
  class ParticleCellIntersection {
  public:
    /** constructor taking a cell, expected energy loss, entry and exit position */
    ParticleCellIntersection( const CaloCell& cell, double expectEnergyLoss, double pathLength );
    
    /** return cell */
    const CaloCell& cell() const { return *m_cell; }

    /** return expected energy loss */
    double expectedEnergyLoss() const { return m_expectedEnergyLoss; }
    
    /** entry position */
    double pathLength() const { return m_pathLength; }

  private:
    const CaloCell* m_cell;
    double          m_expectedEnergyLoss;
    double          m_pathLength;
  };

  inline ParticleCellIntersection::ParticleCellIntersection( const CaloCell& cell, double expectEnergyLoss, double pathLength ) :
    m_cell(&cell),m_expectedEnergyLoss(expectEnergyLoss),m_pathLength(pathLength) {
  }
    
}

#endif

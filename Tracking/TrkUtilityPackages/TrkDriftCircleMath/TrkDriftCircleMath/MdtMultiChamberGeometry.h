/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTMULTICHAMBERGEOMETRY_H
#define DCMATH_MDTMULTICHAMBERGEOMETRY_H

#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/ChamberGeometry.h"
#include "TrkDriftCircleMath/MdtChamberGeometry.h"
#include "TrkDriftCircleMath/Line.h"

#include <ostream>
#include <vector>

namespace TrkDriftCircleMath {

  class MdtMultiChamberGeometry : public ChamberGeometry {
  public:
    
    MdtMultiChamberGeometry();

    MdtMultiChamberGeometry( const std::vector<MdtChamberGeometry>& chambers );
  
    virtual ~MdtMultiChamberGeometry();

    virtual const std::vector<LocPos>& allTubes() const;

    virtual const DCVec& tubesPassedByLine( const Line& line, int ml ) const;
    virtual const DCVec& tubesPassedByLine( const Line& line ) const { return tubesPassedByLine( line, -1 ); }

    virtual unsigned int nlay() const;
    
    virtual const LocPos& tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const;

    virtual bool validGeometry() const { return m_validGeometry; }

    virtual void print() const;

    virtual double tubeRadius() const;

    double stationTheta() const;
    
    const MdtStationId& stationId() const { return m_chambers.at(0).stationId(); }
    
    unsigned int nChambers() const { return m_chambers.size(); }

  private:

    std::vector<MdtChamberGeometry>  m_chambers;
    mutable std::vector<LocPos>      m_allTubes;
    mutable DCVec                    m_crossedTubes;
    bool 														 m_validGeometry;
  };

  inline double MdtMultiChamberGeometry::stationTheta() const {
    double theta = 0.;
    if( m_chambers.empty() || !m_validGeometry ) return 0.;

    std::vector<MdtChamberGeometry>::const_iterator chit = m_chambers.begin();
    std::vector<MdtChamberGeometry>::const_iterator chit_end = m_chambers.end();
    for( ;chit!=chit_end;++chit ) theta += chit->stationTheta();
    return theta/m_chambers.size();
  }

  inline double MdtMultiChamberGeometry::tubeRadius() const {
    if( m_chambers.empty() ) return 14.6;
    return m_chambers.front().tubeRadius();
  }

}


#endif

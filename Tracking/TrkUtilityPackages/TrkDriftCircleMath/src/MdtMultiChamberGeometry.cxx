/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/MdtMultiChamberGeometry.h"
#include "TrkDriftCircleMath/SortDcsByY.h"

#include <iostream>
#include <iterator>
#include <algorithm>

namespace TrkDriftCircleMath {
  
  MdtMultiChamberGeometry::MdtMultiChamberGeometry():
    m_chambers{},
		m_allTubes{},
		m_crossedTubes{},
		m_validGeometry{}
  {
    m_crossedTubes.reserve(30);
  }

  MdtMultiChamberGeometry::MdtMultiChamberGeometry( const std::vector<MdtChamberGeometry>& chambers )
    : m_chambers(chambers)
  {
    std::vector<MdtChamberGeometry>::const_iterator it = m_chambers.begin();
    std::vector<MdtChamberGeometry>::const_iterator it_end = m_chambers.end();
    m_validGeometry = true;
    for(; it!=it_end;++it ){
      if( !it->validGeometry() ){
				m_validGeometry = false;
				break;
      }
    }
    m_crossedTubes.reserve(60);
  }

  
  MdtMultiChamberGeometry::~MdtMultiChamberGeometry() {}


  const std::vector<LocPos>& MdtMultiChamberGeometry::allTubes() const
  {
    if( !m_validGeometry) return m_allTubes;
    if( !m_allTubes.empty() ) return m_allTubes;

    std::vector<MdtChamberGeometry>::const_iterator it = m_chambers.begin();
    std::vector<MdtChamberGeometry>::const_iterator it_end = m_chambers.end();

    for(; it!=it_end;++it ){
      const std::vector<LocPos>& chamberTubes = it->allTubes();
      m_allTubes.reserve( m_allTubes.size()+chamberTubes.size() );
      m_allTubes.insert( m_allTubes.end(),chamberTubes.begin(), chamberTubes.end() );
    }    
    return m_allTubes;
  }

  
  const DCVec& MdtMultiChamberGeometry::tubesPassedByLine( const Line& line, int ml ) const
  {
    m_crossedTubes.clear();

    if( !m_validGeometry){ 
      std::cout << " >>>>> invalid geometry <<<<< " << std::endl;
      return m_crossedTubes;
    }
    std::vector<MdtChamberGeometry>::const_iterator it = m_chambers.begin();
    std::vector<MdtChamberGeometry>::const_iterator it_end = m_chambers.end();

//     std::cout << " crossed tubes for MdtMultiChamberGeometry " << std::endl;
    for(; it!=it_end;++it ){
      it->tubesPassedByLine( line, ml, m_crossedTubes );
    }    

    std::stable_sort( m_crossedTubes.begin(),m_crossedTubes.end(),SortDcsByY() );

    return m_crossedTubes;
  }

  void MdtMultiChamberGeometry::print() const {
    std::vector<MdtChamberGeometry>::const_iterator it = m_chambers.begin();
    std::vector<MdtChamberGeometry>::const_iterator it_end = m_chambers.end();
    for(; it!=it_end;++it ){
      it->print();
    }
  }

  unsigned int MdtMultiChamberGeometry::nlay() const {
    std::vector<MdtChamberGeometry>::const_iterator it = m_chambers.begin();
    if(it != m_chambers.end()) return it->nlay();
    else return 0;      
  }

  const LocPos& MdtMultiChamberGeometry::tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const {
    std::vector<MdtChamberGeometry>::const_iterator it = m_chambers.begin();
    if(it->validId(ml,lay,tube)) return it->tubePosition(ml,lay,tube);
    else return it->tubePosition(1,0,0);
    
  }
}

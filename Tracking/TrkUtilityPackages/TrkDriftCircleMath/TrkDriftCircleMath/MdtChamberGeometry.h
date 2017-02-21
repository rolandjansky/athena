/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTCHAMBERGEOMETRY_H
#define DCMATH_MDTCHAMBERGEOMETRY_H

#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/MdtStationId.h"
#include "TrkDriftCircleMath/ChamberGeometry.h"
#include "TrkDriftCircleMath/ResidualWithLine.h"
#include <cassert>
#include <vector>

namespace TrkDriftCircleMath {

  class MdtChamberGeometry : public ChamberGeometry {
  public:
    
    MdtChamberGeometry();

    MdtChamberGeometry( MdtStationId id, unsigned int nml, unsigned int nlay,
 			unsigned int ntubesml0, unsigned int ntubesml1, 
			LocPos tube0ml0, LocPos tube0ml1, 
			double tubeDist, double tubeStage, double layDist, double stationTheta );  
  
    virtual ~MdtChamberGeometry();

    void init();
    void setGeometry(unsigned int nml, unsigned int nlay,
		     unsigned int ntubesml0, unsigned int ntubesml1, 
		     LocPos tube0ml0, LocPos tube0ml1, 
		     double tubeDist, double tubeStage, double layDist, double stationTheta ); 

    unsigned int nml() const { return m_nml; }
    unsigned int nlay() const { return m_nlay; }
    unsigned int ntubesml0() const { return m_ntubesml[0]; }
    unsigned int ntubesml1() const { return m_ntubesml[1]; }

    const MdtStationId& stationId() const { return m_id; }

    const LocPos& firstTubeMl0() const { return m_firstTube[0]; }
    const LocPos& firstTubeMl1() const { return m_firstTube[1]; }

    double tubeDist() const { return m_tubeDist; }
    double tubeStage() const { return m_tubeStage; }
    double layerDist() const { return m_layDist; }

    double stationTheta() const { return m_stationTheta; }

    double tubeRadius() const { return m_tubeRad; } 

    const std::vector<LocPos>& allTubes() const;
    
    const LocPos& tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const;

    const DCVec& tubesPassedByLine( const Line& line, int ml ) const;
    const DCVec& tubesPassedByLine( const Line& line ) const { return tubesPassedByLine( line, -1 ); }

    /** methods taking reference to output vector to reduce memory usage */
    void tubesPassedByLine( const Line& line, int ml, DCVec& crossedTubes ) const;
    void tubesPassedByLine( const Line& line, DCVec& crossedTubes ) const { return tubesPassedByLine( line, -1, crossedTubes ); }

    /** set that this is the second multi layer instead of the first,
	used in case the first ML is dead */
    void isSecondMultiLayer( bool isSecond ) { m_isSecondMultiLayer = isSecond; }

    bool validGeometry() const { return m_validGeometry; }

    void print() const;

    bool   validId(  unsigned int ml, unsigned int lay,unsigned int tube) const;
  private:
    double xPosTube(unsigned int ml, unsigned int lay, unsigned int tube) const;
    double yPosTube(unsigned int ml,unsigned int lay) const;

    MdtStationId  m_id;
    unsigned int  m_nml;
    unsigned int  m_nlay;
    std::vector<unsigned int>  m_ntubesml;
    double        m_tubeDist;
    double        m_tubeRad;
    double        m_tubeStage;
    double        m_layDist;
    double        m_stationTheta;

    mutable std::vector<int>    m_wasInit;
    mutable std::vector<LocPos> m_firstTube;
    mutable std::vector<LocPos> m_allTubes;
    mutable DCVec        m_crossedTubes;
    mutable LocPos       m_cachedPos;
    bool m_validGeometry;
    bool m_isSecondMultiLayer;

    mutable ResidualWithLine m_resLine;

  };
  
  inline const LocPos& MdtChamberGeometry::tubePosition(unsigned int ml,
							unsigned int lay, 
							unsigned int tube) const {
    assert( validId(ml,lay,tube) );
    m_cachedPos.setX(xPosTube(ml,lay,tube));
    m_cachedPos.setY(yPosTube(ml,lay));
    return m_cachedPos;
  }


  inline double MdtChamberGeometry::xPosTube(unsigned int ml, unsigned int lay, unsigned int tube) const
  {
    double xpos = tube*m_tubeDist + m_firstTube[ml].x();
    /*
    if( m_id.stName() == 53 || m_id.stName() == 54 ){
      std::cout<<"x tube position for tube "<<ml<<","<<lay<<","<<tube<<std::endl;
      std::cout<<"tube*m_tubeDist+m_firstTube[ml].x()="<<tube<<"*"<<m_tubeDist<<"+"<<m_firstTube[ml].x()<<"="<<xpos<<std::endl;
    }
    */
    if( lay%2 == 1 && !(m_id.stName() == 53 || m_id.stName() == 54)){
      /*
      if( m_id.stName() == 53 || m_id.stName() == 54 ){
	std::cout<<"odd layer, adjust the x positions by the tube staggering"<<std::endl;
	if(m_nlay==4 && ml==1) std::cout<<"4-layer ml, in ml1, so subtract "<<m_tubeStage<<std::endl;
	else std::cout<<"add "<<m_tubeStage<<std::endl;
      }
      */
      if( m_nlay == 4 && ml == 1 ) xpos -= m_tubeStage;
      else xpos += m_tubeStage;
    }
    return xpos;
  }

  inline double MdtChamberGeometry::yPosTube(unsigned int ml, unsigned int lay) const
  {
    return lay*m_layDist + m_firstTube[ml].y();
  }

}


#endif

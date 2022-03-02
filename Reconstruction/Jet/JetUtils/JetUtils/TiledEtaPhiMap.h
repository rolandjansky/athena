// emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  
     @file TiledEtaPhiMap.h

     Define a fast lookup map for points on the eta-phi cylinder and related utilities.
     See TiledEtaPhiMap.

     @author P-A Delsart
     @date before 2010
*/

#ifndef JETUTIL_TILEDETAPHIMAP_H
#define JETUTIL_TILEDETAPHIMAP_H


#include <list>
#include <vector>

#include <cmath>


#include "JetUtils/JetDistances.h"

namespace JetTiledMap {

  struct utils {
    static constexpr double DR2(double eta1, double phi1, double eta2,double phi2) {
      double deta = eta1 - eta2;
      constexpr double pi = M_PI;
      constexpr double twopi = 2.0*pi;
      double dphi = phi1 - phi2;
      while ( dphi > pi ) dphi -= twopi;
      while ( dphi <= -pi ) dphi += twopi;
      return dphi*dphi + deta*deta;
    }
  };

  enum Directions {
    N=0, NE,E,SE,S,SW,W,NW
  };
  

  /// class Tile 
  /// 
  /// represents a tile of the map. It holds pointers to its neighbour tiles 
  /// POINT is the type of points used by the map. DIST2 defines a 2D distance between POINTS. 
  template<class POINT, class DIST2>
  class Tile : public std::list<POINT> {
  public:
    typedef typename std::list<POINT> tilecontainerbase_t;
    typedef typename tilecontainerbase_t::iterator iterator;
    typedef typename tilecontainerbase_t::const_iterator const_iterator;
    typedef typename std::vector<POINT> pointvec_t;
    
    Tile(){this->clear();for(int i=0;i<8;i++) m_neighbour[i]=0;}
    
    /// Fill a vector of all points from this tile within deltaR of POINT p. (r2 is R *squared*)
    void fillPointsInDr2(POINT &p, double r2, pointvec_t& points ) const;
    
    /// the void tile never contains any point.
    static Tile voidTile;
  public:
    Tile<POINT,DIST2>* m_neighbour[8];
    static DIST2 dr2;
    POINT center;
  };
  

  
  
  /// \class TiledEtaPhiMap
  /// 
  /// An eta-phi map providing a relatively fast way of retrieving points at a 
  /// given distance of a point in the plan (cylinder in this case).
  /// It works by simply dividing the plan in rectangular tiles  of identical size and restricting 
  /// the search only in a tile and its neighbours.
  /// IMPORTANT : the search is accurate only up to distance rmax where rmax is given to the init() function.
  template<class POINT, class DIST2 = typename POINT::DR2>
  class TiledEtaPhiMap { 
  public:
    typedef  Tile<POINT, DIST2> tile_t;

  public:
    TiledEtaPhiMap():  m_etarange(10.0), m_halfetarange(5.), m_size(0) {};
    virtual ~TiledEtaPhiMap(){}
    void init(double rmax);    
    
    void insert(POINT & p);   

    /// retrieve all points  within deltaR of p. WARNING !! wrong results if r>rmax !
    std::vector<POINT> pointsInDr(POINT &p, double r) const ;

    virtual void clear();

    virtual void reset(){m_tiles.clear();m_size=0;}
    
    unsigned int size()const {return m_size;} ;
    
    void setEtaRange(double r){m_etarange=r; m_halfetarange = r/2;}

  protected:

    double  m_etarange{} ;
    double  m_halfetarange{};
    double m_rmax{};
    size_t m_ndivX{}, m_ndivY{};
    double m_sizeX{},m_sizeY{};
    
    unsigned int m_size{};
    

    /// index of the tile containing p
    size_t tileIndex(POINT &p) const {

      int indx = int( (p.x()+m_halfetarange)/m_sizeX) ;
      if(indx<0) indx=0;
      if(indx>=static_cast<int>(m_ndivX)) indx=m_ndivX-1;
      int indy = int((M_PI-p.y())/m_sizeY);      
      if(indy>=static_cast<int>(m_ndivY)) indy=m_ndivY-1;
      
      return tileIndex_i(indx,indy);
    }

    /// retrieve the tile index from its integer coordinates.
    size_t tileIndex_i(int ix, int iy) const {return ix*m_ndivY + iy;};
    
    std::vector<tile_t> m_tiles;
    
  };
  
  
}

#include "JetUtils/TiledEtaPhiMap.icc"

#endif

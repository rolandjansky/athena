/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// emacs this is -*-C++-*-
#ifndef _CYL_GEOM_HH_
#define _CYL_GEOM_HH_

/**  
     A collection of routines for geometric tasks in 2D and on a cylinder.
     Including a convex hull finder.
     
     @author P.A. Delsart
     @date (first implementation) June , 2007
 */

#include <set>
#include <list>
#include <vector>
#include <cmath>

namespace JetGeom {

  /** Very basic point objects*/
class point_t : public std::pair<float , float>{ 
public:
  point_t(float x, float y){first = x; second = y;}
};
class point_set_t : public std::set<point_t> {};
class point_vect_t : public std::vector<point_t> {};

typedef std::list<point_t> point_list_t;


  /** @class line_t describe an oriented segment/line in a simplistic
      way */
class line_t {
public:
  
  /** constructor by giving the coeff of equation of the line (@param
      cx x + @param cy y +@param cc = 0). If @param to_r is true the
      line is oriented toward positive x */
  line_t(float cx, float cy, float cc, bool to_r) : m_cx(cx),m_cy(cy),m_cc(cc), m_oriented_r(to_r) {};
  /** Constructor by giving 2 points. This also sets orientation*/
  line_t(point_t p1, point_t p2);
  
  float m_cx, m_cy, m_cc;
  bool m_oriented_r;

  
  bool is_above(point_t &p);
  bool is_below(point_t &p);

  bool is_left(point_t &p) {return m_oriented_r ? is_above(p) : is_below(p);};
  bool is_right(point_t &p) {return m_oriented_r ? is_below(p) : is_above(p);};

  void phi_shift(float dphi) {m_cc = m_cc - m_cy*dphi;}

  point_t intercept_y(float y);
  point_t intercept_x(float x);
  
};




/// Find convex hull of a set of points in euclidian plan. The hull
/// points are filled into @param outSet
template< class inT, class ouT>
void findConvexHull(inT &inSet, ouT &outSet );
  /// specialization for point_set_t
template<class ouT>
void findConvexHull(point_set_t &inSet, ouT &outSet );
// internally used by findConvexHull
template< class inT>
void _findConvexHull(point_set_t &inSet, inT &outSet );

// internally used by findConvexHull
void testHullLine(point_list_t &hull, point_t p);


template<class T>
float polygon_area(T& line);

template<class T>
float polygon_lenght(T& line);



// *******************************************************
// Cylindrical geometry **********************************
// *******************************************************

/// return average phi (i.e. second coordinate) of the point container
/// @param set, if max(deltaphi) < pi (i.e. points are located on one
/// side of cylinder) 
/// otherwise return -10.
template <class T>
float getMeanPhi(T &set);
  /// Return max distance betweens point @param p and points of @param set
template<class T>
float max_deltaR(point_t p, T& set);


/// copy @param inSet into @param outSet with all phi coordonitates recnetered by phicenter
template<class T , class T2>
void recenter_set(T &inSet, T2 &outSet, float phicenter);
/// copy @param inSet into @param outSet with all phi coordonitates recentered by
/// average phi of @param inSet
template<class T , class T2>
void recenter_set(T &inSet, T2 &outSet);


  /// convert @param phi into a value in [-Pi,Pi]
inline float in_mPI_pPI(float phi);
  /// convert @param p second coordinate (phi) into a value in [-Pi,Pi]
inline void  fix2pi(point_t &p);
  /// distances between points
inline float deltaR(point_t &p1, point_t &p2);
inline float deltaR2(point_t &p1, point_t &p2);

inline float deltaPhi(point_t &p1, point_t &p2);
inline float deltaPhi(float phi1, float phi2);



// *******************************************************
// Utils function **********************************
// *******************************************************
void listToSet(point_list_t &inList, point_set_t &outSet);
  template<class T> 
  void clear_delete(T &container);
  template<class T> 
  void delete_content(T &container);
  



// ***************************************************************************

// ***********************************************
// Inlined and template implementations ****************************
// ***********************************************
#ifndef G__DICTIONARY


template<class T> 
void clear_delete(T &container){
  typedef typename T::iterator it_t;
  it_t it =  container.begin();
  it_t itE = container.end();
  for(; it != itE; ++it) delete *it;
  container.clear();
}
template<class T> 
void delete_content(T &container){
  typedef typename T::iterator it_t;
  it_t it =  container.begin();
  it_t itE = container.end();
  for(; it != itE; ++it) delete *it;
}

template<class T>
float max_deltaR(point_t p, T& set){
  typedef typename T::iterator it_t;
  it_t it  = set.begin();
  it_t itE = set.end();
  float r=0;
  for(; it != itE; ++it){
    float t = deltaR(p, *it);
    if(t>r) r=t;
  }
  return r;
}

template<class T>
float polygon_area(T& line){
  typedef typename T::iterator it_t;
  it_t it = line.begin();
  it_t itE = line.end();
  float a=0;
  for(; it != itE; ++it){
    it_t itp=it;
    itp++;
    if(itp == itE) itp =  line.begin();
    a += ( (*it).first*(*itp).second - (*itp).first*(*it).second );
  }
  return a/2;
}

template<class T>
float polygon_lenght(T &line){
  typedef typename T::iterator it_t;
  it_t it = line.begin();
  it_t itE = line.end();
  float a=0;
  for(; it != itE; ++it){
    it_t itp=it;
    itp++;
    if(itp == itE) itp =  line.begin();
    a += deltaR(*it,*itp);
  }
  return a;
}

inline float abs_dphi(float phi1, float phi2){
  float d = fabs(phi1 - phi2);
  return d < M_PI ?  d: fabs(d-2*M_PI);
}
inline float deltaPhi(float phi1, float phi2){
  return in_mPI_pPI(phi1-phi2);
}
inline float deltaPhi(point_t &p1, point_t &p2){
  return deltaPhi(p1.second,p2.second);
}
inline  float in_mPI_pPI(float phi){
  while(phi < -M_PI) phi += 2*M_PI;
  while(phi >= M_PI) phi -= 2*M_PI;
  return phi;
}
inline void fix2pi(point_t &p){
  while(p.second < -M_PI) p.second += 2*M_PI;
  while(p.second >= M_PI) p.second -= 2*M_PI;
}

inline point_t recenter(const point_t &p, const point_t &center){
  point_t n(p.first, p.second - center.second);
  fix2pi(n);
  return n;
}
inline point_t recenter(const point_t &p, float phicenter){
  point_t n(p.first, p.second - phicenter);
  fix2pi(n);
  return n;
}
inline float deltaR2(point_t &p1, point_t &p2){
  return  (p1.first-p2.first)*(p1.first-p2.first) + 
		abs_dphi(p1.second,p2.second)*abs_dphi(p1.second,p2.second) ;
}
inline float deltaR(point_t &p1, point_t &p2){
  return  sqrt( deltaR2(p1,p2) );
}

template<class T , class T2>
void recenter_set(T &inSet, T2 &outSet, float phicenter){
  typedef typename T::iterator it_t;
  typedef typename T2::iterator it_t2;
  it_t it = inSet.begin();
  it_t itE = inSet.end();
  it_t2 it2 = outSet.begin();
  for(; it!= itE; ++it){
    it2 = outSet.insert(it2,recenter(*it,phicenter));
  }
}
template<class T , class T2>
void recenter_set(T &inSet, T2 &outSet){
  float phicenter = getMeanPhi(inSet);
  if (phicenter <-9) return;
  recenter_set(inSet,outSet,phicenter);
}

template <class T>
float getMeanPhi(T &set){
  // return mean
  typedef typename T::iterator it_t;
  it_t it = set.begin();
  it_t end = set.end();
  point_t fp = (*it);
  float max= 0,min=0, mean=0;
  int n=0;
  for(; it!= end; ++it){
    float phi = recenter((*it),fp).second;
    if( phi >0){
      max = phi>max ? phi : max;
    }else{
      min = phi<min ? phi : min;
    }
    mean+=phi; n++;
  }
  if ( (max-min) <= (M_PI) )
    return in_mPI_pPI(mean/n+fp.second);
  else return -10;
}



inline  line_t::line_t(point_t p1, point_t p2){
  m_cy = p1.first - p2.first;
  m_cx = p2.second - p1.second;
  m_cc = -(m_cx*p1.first+m_cy*p1.second);  
  m_oriented_r = (p1.first < p2.first);
}

inline bool line_t::is_above(point_t &p){
  return  ( (m_cy!=0) && ( p.second >= - (m_cx*p.first+m_cc)/m_cy) );
}

inline bool line_t::is_below(point_t &p){
  return  ( (m_cy!=0) && ( p.second <= - (m_cx*p.first+m_cc)/m_cy) ) ;
}

inline point_t line_t::intercept_y(float y){
  if(m_cx==0) return point_t(-99999,y);
  return point_t( (-m_cc - m_cy*y)/m_cx , y);
}

inline point_t line_t::intercept_x(float x){
  if(m_cy==0) return point_t(x,-99999);
  return point_t( x, (-m_cc - m_cx*x)/m_cy );
}



// Convex Hull *************************************

template< class inT, class ouT>
void findConvexHull(inT &inSet, ouT &outSet ){
  point_set_t rset;
  typedef typename inT::iterator it_t;
  it_t it = inSet.begin();
  it_t itE = inSet.end();
  for(; it!=itE; ++it) rset.insert(*it);
  _findConvexHull(rset,outSet);
}

template<class ouT>
void findConvexHull(point_set_t &inSet, ouT &outSet ){
  _findConvexHull(inSet, outSet);
}

template <class T>
void _findConvexHull(point_set_t &inSet, T &outSet ){

  if(inSet.size()<4){  
    outSet.insert(outSet.begin(), inSet.begin(), inSet.end() );
    return;
  }
  
  point_set_t::iterator s_it = inSet.begin();
  point_t point00 = (*s_it);
  point_t point01= point00;
  while(point01.first == point00.first){
    ++s_it;
    point01 = *s_it;
  }
  --s_it; point01 = *s_it; 

  s_it = inSet.end(); --s_it;
  point_t point11 = (*s_it);
  point_t point10= point11;
  while(point10.first == point11.first){
    --s_it;
    point10 = *s_it;
  }
  ++s_it; point10 = *s_it; 

  // lower line :
  line_t lmin(point00, point10);
  // lower line :
  line_t lmax(point01, point11);
    
  // fill upper and lower list :
  point_list_t lowPoints, upPoints;

  s_it = inSet.begin();
  if(point00 == point01 ){ // make sure point is in both list
    lowPoints.push_back(point00);
    upPoints.push_back(point00);
    s_it++;
  }
  //std::cout << " lp size =" << lowPoints.size() << std::endl;
  point_set_t::iterator s_itE = inSet.end(); 
  s_itE--; // we'll deal ws_ith the end by hand
  for(; s_it!= s_itE; ++s_it){
    point_t p = *s_it;
    //std::cout << " point "<< p.first << " , "<< p.second << std::endl;
    if( lmin.is_below(p)) {
      lowPoints.push_back(p);
      continue;
    }
    if( lmax.is_above(p)) {
      upPoints.push_front(p); // insert front soit is well sorted
    }

  }
  //std::cout << " lp size =" << lowPoints.size() << std::endl;
  if(point11 == point10 ){ // make sure point is in both list
    lowPoints.push_back(point11);
    upPoints.push_front(point11);
    s_it++;
  }else{
    lowPoints.push_back(point10);
    upPoints.push_front(point00);
  }
  //std::cout << " lp size =" << lowPoints.size() << std::endl;
  //std::cout << lowPoints.size() << " " << upPoints.size() << std::endl;

  // find lower hull :
  point_list_t lowHull;
  point_list_t::iterator it = lowPoints.begin();
  point_list_t::iterator itE = lowPoints.end();
  lowHull.push_back(point00); // here point00 == (*it)
  ++it;
  lowHull.push_back(*it);  // always put next point in stack
  ++it;
  unsigned int nn = lowPoints.size();
  if(nn>2){
    //std::cout << "building low hull : "<< lowPoints.size() <<std::endl; 
    while(it!=itE){    
      testHullLine(lowHull, *it);
      ++it;
    }
  }
  // find upper hull :
  point_list_t upHull;
  it = upPoints.begin();
  itE = upPoints.end();
  upHull.push_back(point11); // here point11 == (*it)
  ++it;
  upHull.push_back(*it);  // always put next point in stack
  ++it; 
  if(upPoints.size()>2){
    //std::cout << "building up hull : "<< upPoints.size() <<std::endl; 
    while(it!=itE){   
      testHullLine(upHull, *it);    
      ++it;
    }
  }
//   std::cout << " low size = "<< lowHull.size() << std::endl;
//   std::cout << " up size = "<< upHull.size() << std::endl;
  // join Hull !
  outSet.insert(outSet.begin(), lowHull.begin(), lowHull.end() );
  outSet.insert((outSet.end()), ++(upHull.begin()), --(upHull.end()) ); // avoid duplication
  
  //  std::cout << " Out size = "<< outSet.size() << std::endl;

}


#endif //DICT


} // namespace JetGeom
#endif


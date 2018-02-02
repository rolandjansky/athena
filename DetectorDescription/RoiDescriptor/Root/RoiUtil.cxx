//
//   @file    RoiUtil.cxx         
//   
//            non-member, non friend RoiDescriptor utility functions
//            to improve encapsulation
//
//   @author M.Sutton
// 
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiUtil.cxx, v0.0   Sat 31 Oct 2015 09:54:33 CET sutt $


#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RoiUtil.h"

#include <cmath>

#ifndef M_2PI
static const double M_2PI = 2*M_PI;
#endif

#ifndef M_PIF
static const float  M_PIF = float(M_PI);
#endif



namespace RoiUtil { 
  
class range_error : public std::exception { 
public:
  range_error( const char* s ) : std::exception(), m_str(s) { } 
  virtual const char* what() const throw() { return m_str; } 
private:
  const char*  m_str;
};
  
}




/// test whether a stub is contained within the roi
bool RoiUtil::contains( const IRoiDescriptor& roi, double z0, double dzdr ) {
  static const double maxR = 1100; // maximum radius of RoI - outer TRT radius ~1070 mm - should be configurable? 
  double zouter = dzdr*maxR + z0; 
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::contains_internal( *(*itr), z0, zouter ) ) return true;
    return false;
  }
  else return contains_internal( roi, z0, zouter ); 
} 




bool RoiUtil::contains_internal( const IRoiDescriptor& roi, double z0, double zouter )  {
  if ( z0<=roi.zedPlus() && z0>=roi.zedMinus() && zouter<=roi.zedOuterPlus() && zouter>=roi.zedOuterMinus() ) return true;
  return false;
} 
  


bool RoiUtil::contains_zrange( const IRoiDescriptor& roi, double z0, double dzdr, double zmin, double zmax ) { 
 static const double maxR = 1100; // maximum radius of RoI - outer TRT radius ~1070 mm - should be configurable? 
  double zouter = dzdr*maxR + z0; 
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::contains_zrange_internal( *(*itr), z0, zouter, zmin, zmax ) ) return true;
    return false;
  }
  else return contains_zrange_internal( roi, z0, zouter, zmin, zmax ); 
}




bool RoiUtil::contains_zrange_internal( const IRoiDescriptor& roi, double z0, double zouter, double zmin, double zmax )  {
  if ( z0<=zmax && z0>=zmin && zouter<=roi.zedOuterPlus() && zouter>=roi.zedOuterMinus() ) return true;
  return false;
} 
  



/// test whether a stub is contained within the roi
bool RoiUtil::containsPhi( const IRoiDescriptor& roi, double phi ) {
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::containsPhi( *(*itr), phi ) ) return true;
      return false;
  }
  else {
    if ( roi.isFullscan() ) return true; 
    if ( roi.phiPlus()>roi.phiMinus() ) return ( phi<roi.phiPlus() && phi>roi.phiMinus() );
    else                                return ( phi<roi.phiPlus() || phi>roi.phiMinus() );
  } 
}




/// non member, non friend interface function 
/// test whether a stub is contained within the roi                                                                                                                                                                  
bool RoiUtil::roiContainsZed( const IRoiDescriptor& roi, double z, double r ) {
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( roiContainsZed( *roi.at(i), z, r ) ) return true;
    return false;
  }
  if ( roi.isFullscan() ) return true;
  double zminus = r*roi.dzdrMinus()+roi.zedMinus();
  double zplus  = r*roi.dzdrPlus() +roi.zedPlus();
  return ( z>=zminus && z<=zplus );
}



bool RoiUtil::containsZed( const IRoiDescriptor& roi, double z, double r ) {
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::containsZed( *(*itr), z, r ) ) return true;
    return false;
  }
  else { 
    if ( roi.isFullscan() ) return true;
    double zminus = r*roi.dzdrMinus() + roi.zedMinus();
    double zplus  = r*roi.dzdrPlus()  + roi.zedPlus();
    return ( z>=zminus && z<=zplus );
  }
}




bool RoiUtil::contains( const IRoiDescriptor& roi, double z, double r, double phi ) {
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::contains( *(*itr), z, r, phi ) ) return true;
    return false;
  }
  else { 
    return ( RoiUtil::containsZed( roi, z, r ) && RoiUtil::containsPhi( roi, phi ) );
  }
}
  


bool RoiUtil::roiContains( const IRoiDescriptor& roi, double z, double r, double phi ) { 
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( RoiUtil::roiContains( *roi.at(i), z, r, phi ) ) return true;
    return false;
  } 
  if ( roi.isFullscan() ) return true;
  return ( RoiUtil::roiContainsZed( roi, z, r ) && RoiUtil::containsPhi( roi, phi ) ); 
}






double RoiUtil::phicheck(double phi) { 
  while ( phi> M_PIF ) phi-=M_2PI;
  while ( phi<-M_PIF ) phi+=M_2PI;
  if ( !(phi >= -M_PIF && phi <= M_PIF) ) { // use ! of range rather than range to also catch nan etc
    throw range_error( (std::string("phi out of range: ")+std::to_string(phi)).c_str() ); 
  } 
  return phi;
}


double RoiUtil::etacheck(double eta) {
  if ( !(eta>-100  && eta<100) ) { // check also for nan
    throw range_error( (std::string("eta out of range: ")+std::to_string(eta)).c_str() ); 
  } 
  return eta;
}


double RoiUtil::zedcheck(double zed ) {
  if ( !(zed>-100000  && zed<100000 ) ) { // check also for nan
    throw range_error( (std::string("zed out of range: ")+std::to_string(zed)).c_str() ); 
  } 
  return zed;
}


bool operator==( const IRoiDescriptor& roi0,  const IRoiDescriptor& roi1 ) { 
  
  /// trivial self comparison
  if ( &roi0 == &roi1 ) return true;

  /// same compositness ?
  if ( roi0.composite() != roi1.composite() ) return false;

  if ( !roi0.composite() ) { 
    /// not composite

    /// check full scan - all non-composite full scan rois are equivalent
    if ( roi0.isFullscan() != roi1.isFullscan() ) return false;
    if ( roi0.isFullscan() ) return true;

    /// check geometry 
    if ( std::fabs(roi0.zed()     -roi1.zed()     )>1e-7 ) return false;
    if ( std::fabs(roi0.zedPlus() -roi1.zedPlus() )>1e-7 ) return false;
    if ( std::fabs(roi0.zedMinus()-roi1.zedMinus())>1e-7 ) return false;
   
    if ( std::fabs(roi0.eta()     -roi1.eta()     )>1e-7 ) return false;
    if ( std::fabs(roi0.etaPlus() -roi1.etaPlus() )>1e-7 ) return false;
    if ( std::fabs(roi0.etaMinus()-roi1.etaMinus())>1e-7 ) return false;

    /// Fixme: naive phi differwnce - should test for the phi=pi boundary 
    ///        for the case of very close angles but wrapped differently
    if ( std::fabs(roi0.phi()     -roi1.phi()    ) >1e-7 ) return false;
    if ( std::fabs(roi0.phiPlus() -roi1.phiPlus()) >1e-7 ) return false;
    if ( std::fabs(roi0.phiMinus()-roi1.phiMinus())>1e-7 ) return false;
  } 
  else { 
    /// check constituents
    if ( roi0.size() != roi1.size() ) return false;
    for ( unsigned i=roi0.size() ; i-- ; ) if ( !( *roi0.at(i) == *roi1.at(i) ) ) return false;  
  }
  
  return true;
}



bool operator!=( const IRoiDescriptor& roi0,  const IRoiDescriptor& roi1 ) { return !(roi0==roi1); } 

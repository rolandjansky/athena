// emacs: this is -*- c++ -*-
//
//   @file    RoiUtil.h        
//
//            non-member, non friend RoiDescriptor utility functions
//            to improve encapsulation
//                   
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiUtil.h, v0.0   Sat 31 Oct 2015 09:55:29 CET sutt $


#ifndef  ROIUTIL_H
#define  ROIUTIL_H

// #include "IRegionSelector/IRoiDescriptor.h"

class IRoiDescriptor;

namespace RoiUtil { 

/// see whether a segment is contained within the roi in r-z
bool contains( const IRoiDescriptor& roi, double z0, double dzdr );
bool contains_internal( const IRoiDescriptor& roi, double z0, double zouter );
 
bool contains_zrange( const IRoiDescriptor& roi, double z0, double dzdr, double zmin, double zmax );
bool contains_zrange_internal( const IRoiDescriptor& roi, double z0, double zouter, double zmin, double zmax );

/// see whether a point is contained within the roi (in phi and r-z)
bool contains( const IRoiDescriptor& roi, double z, double r, double phi );
bool containsPhi( const IRoiDescriptor& roi, double phi );
bool containsZed( const IRoiDescriptor& roi, double z, double r );

/// old non-member, non-friend interface methods - left in for backwards 
/// compatibility
bool roiContainsZed( const IRoiDescriptor& roi, double z, double r );
bool roiContains( const IRoiDescriptor& roi, double z, double r, double phi);
 
/// basic range checkers 
double phicheck(double phi);
double etacheck(double eta);
double zedcheck(double zed);

}

bool operator==( const IRoiDescriptor& roi0, const IRoiDescriptor& roi1 );
bool operator!=( const IRoiDescriptor& roi0, const IRoiDescriptor& roi1 );




#endif  // ROIUTIL_H 











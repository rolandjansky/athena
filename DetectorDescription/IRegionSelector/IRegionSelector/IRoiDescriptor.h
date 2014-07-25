// emacs: this is -*- c++ -*-
//
//   @file    IRoiDescriptor.h        
//
//             RoiDescriptor interface - only basic accessors                   
//  
//   @author sutt@cern.ch
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: IRoiDescriptor.h, v0.0   Fri 14 Jun 2013 00:52:09 CEST sutt $

#ifndef IREGIONSELECTOR_INEWROIDESCRIPTOR_H
#define IREGIONSELECTOR_INEWROIDESCRIPTOR_H

#define __newroidescriptor

/// hmmm
#include "CLIDSvc/CLASS_DEF.h"

/**
 **   @brief Describes the API of the Region of Ineterest geometry
 **/

class IRoiDescriptor {

public:

  /**
   * @brief default constructor
   */
  IRoiDescriptor() { }
 
  // Destructor
  virtual ~IRoiDescriptor() { }

  /// Methods to retrieve data members
  
  /// directions
  virtual   double phi()  const = 0; 
  virtual   double eta()  const = 0;
  virtual   double zed()  const = 0;

  /// the zed and eta values at the most forward
  /// and most rear ends of the RoI
  virtual   double zedPlus()  const = 0;
  virtual   double zedMinus() const = 0;

  virtual   double etaPlus()  const = 0;
  virtual   double etaMinus() const = 0;

  /// extreme phi values
  virtual   double phiPlus()  const = 0;
  virtual   double phiMinus() const = 0;


  /// identifiers
  virtual   unsigned int roiId()   const = 0; 
  virtual   unsigned int l1Id()    const = 0;
  virtual   unsigned int roiWord() const = 0;


  /// which roi version?
  virtual int version() const = 0;

  /// cast to a string
  virtual operator std::string() const = 0;  



  /// is this a full detector RoI?
  virtual bool isFullscan() const = 0;



  /// Super RoI access methods 

  /// am I a SuperRoi?
  virtual bool composite() const = 0;

  /// number of constituents
  virtual unsigned size() const = 0;

  /// find an RoiDescriptor constituent
  virtual const IRoiDescriptor* at(int i) const = 0;



  /// useful methods to determine whether items lie
  /// partially within the RoI
  
  /// accessors to calculate z position at radius r
  /// along the RoI boundaries
  virtual  double zedMin(double r) const = 0;
  virtual  double zedMax(double r) const = 0;

  /// accessors to calculate r position at position
  /// z along the RoI boundaries
  virtual  double rhoMin(double z) const = 0;
  virtual  double rhoMax(double z) const = 0;


  /// return the gradients 
  virtual double dzdrMinus() const = 0;
  virtual double dzdrPlus()  const = 0;

  virtual double drdzMinus() const = 0;
  virtual double drdzPlus()  const = 0;


  /// see whether a segment is contained within the roi in r-z
  virtual bool contains( double z0, double dzdr )            const = 0;
  virtual bool contains_internal( double z0, double zouter ) const = 0;

  /// see whether a point is contained within the roi (in phi and r-z)
  virtual bool contains( double _z, double _r, double _phi ) const = 0;
  virtual bool containsPhi( double _phi )                    const = 0;
  virtual bool containsZed( double _z, double _r )           const = 0;

};


// inline 
// bool operator==( const IRoiDescriptor& a, const IRoiDescriptor& b ) { 
//   double epsilon=0.0001; // arbitrary, but seems to be reasonable
//   if ( a.roiWord() != b.roiWord() )           return false;
//   if ( std::fabs(a.phi()-b.phi()) > epsilon ) return false;
//   if ( std::fabs(a.eta()-b.eta()) > epsilon ) return false;
//   if ( std::fabs(a.zed()-b.zed()) > epsilon)  return false;
// }

// inline 
// bool operator!=( const TrigRoiDescriptor& a, const TrigRoiDescriptor& b ) { return !(a==b); }

//////////////////////////////////////////////////////////////////
// helper operators

inline
std::ostream& operator<<( std::ostream& s, const IRoiDescriptor& d ) {
  return s << std::string(d);
}


// inline
// std::ostream operator<<( std::ostream& s, const IRoiDescriptor& r ) { 
//   s << "[ Roi"
//     << " phi=" << r.phi() << " phiMinus=" << r.phiMinus() << " phiPlus="  << r.phiPlus()
//     << " eta=" << r.eta() << " etaMinus=" << r.etaMinus() << " etaPlus="  << r.etaPlus()
//     << " zed=" << r.zed() << " zedMinus=" << r.zedMinus() << " zedPlus="  << r.zedPlus()
//     << " l1Id=" << r.l1Id() << " roiWord=" << r.roiWord();
//   return s;
// } 


CLASS_DEF(IRoiDescriptor,12665988,1)

#endif // IREGIONSELECTOR_INEWROIDESCRIPTOR_H



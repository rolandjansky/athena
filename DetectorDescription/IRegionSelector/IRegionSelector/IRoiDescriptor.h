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

#ifndef IREGIONSELECTOR_IROIDESCRIPTOR_H
#define IREGIONSELECTOR_IROIDESCRIPTOR_H

#include <vector> 
#include <iostream> 

/// hmmm - is this actually ever needed ??
// #include "CLIDSvc/CLASS_DEF.h"

/**
 **   @brief Describes the API of the Region of Ineterest geometry
 **/

class IRoiDescriptor {

public:

  // iterator 
  typedef std::vector<const IRoiDescriptor*>::const_iterator roi_iterator;

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

  /// const limit iterators
  virtual roi_iterator  begin() const = 0;
  virtual roi_iterator  end()   const = 0;



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

  /// zed limits at some outer radius
  virtual   double zedOuterPlus()  const = 0;
  virtual   double zedOuterMinus() const = 0;


};


//////////////////////////////////////////////////////////////////
// helper operators

inline
std::ostream& operator<<( std::ostream& s, const IRoiDescriptor& d ) {
  return s << std::string(d);
}


/// is this really needed ??
// CLASS_DEF(IRoiDescriptor,12665988,1)

#endif // IREGIONSELECTOR_INEWROIDESCRIPTOR_H



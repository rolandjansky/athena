// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   @file    INewRoiDescriptor.h        
//
//            New RoiDescriptor interface - only basic accessors                   
//  
//   @author sutt@cern.ch
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigNewRoiDescriptor.h, v0.0   Fri 14 Jun 2013 00:52:09 CEST sutt $

#ifndef IREGIONSELECTOR_INEWROIDESCRIPTOR_H
#define IREGIONSELECTOR_INEWROIDESCRIPTOR_H

/**
 * @brief Describes the API of the Region of Ineterest geometry
 */

class INewRoiDescriptor {

public:

  /**
   * @brief default constructor
   */
  INewRoiDescriptor() { }
 
  // Destructor
  virtual ~INewRoiDescriptor() { }

  /// Methods to retrieve data members
  
  /// directions
  virtual   double phi()  const = 0; 
  virtual   double eta()  const = 0;
  virtual   double zed()  const = 0;

  /// identifiers
  virtual   unsigned int roiId()   const = 0; 
  virtual   unsigned int l1Id()    const = 0;
  virtual   unsigned int roiWord() const = 0;

  /// the zed and eta values at the most forward
  /// and most rear ends of the RoI
  virtual   double zedPlus()  const  = 0;
  virtual   double zedMinus() const  = 0;

  virtual   double etaPlus()  const  = 0;
  virtual   double etaMinus() const  = 0;

  /// extreme phi values
  virtual   double phiPlus()  const  = 0;
  virtual   double phiMinus() const  = 0;

  /// does this overlap with another RoI - almost certainly
  /// yes because of the z extent 
  ///  virtual bool overlap(const INewRoiDescriptor& roi) const = 0;

  /// Super RoI access methods 

  /// am I a SuperRoi?
  virtual bool composite() const = 0;

  /// number of constituents
  virtual unsigned size() const = 0;

  /// find an RoiDescriptor constituent
  virtual INewRoiDescriptor* at(int i) const = 0;

};

#endif // IREGIONSELECTOR_INEWROIDESCRIPTOR_H



//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARRAWCONDITIONS_LARCONDITIONSVSCALIB_H
#define LARRAWCONDITIONS_LARCONDITIONSVSCALIB_H

#include "LArRawConditions/LArConditionsVsCalibBase.h"
#include "LArRawConditions/LArConditionsVsCalibDB.h"

/** Template class for condition objects organized vs calibration line
 *
 * @author M. Fanti
 * @version  0-0-1 ,  7/02/06
 *
 * History:
 *  - 
 */

template < class T >
class LArConditionsVsCalib: 
  public LArConditionsVsCalibBase,
  public LArConditionsVsCalibDB<T>
{
  
public:

  //  static const unsigned nChannelsBoard = 128 ;   // channels per calibration board
  //  typedef typename LArConditionsVsCalibDB<T>::nChannelsBoard nChannelsBoard ;

//   typedef unsigned                                CB_Id ;     // this refers to Calib Board [feedthrough,slot]
//   typedef std::vector<T>                          T_vector ;  // vector of 128 T's
//   typedef typename std::pair<CB_Id,T_vector>      CB_Tvec_pair ;
//   typedef typename std::vector<CB_Tvec_pair>      CB_vector ;
//   typedef typename CB_vector::const_iterator      Const_CB_It ;
//   typedef typename CB_vector::iterator            CB_It ;

  typedef typename LArConditionsVsCalibDB<T>::CB_Id              CB_Id ;
  typedef typename LArConditionsVsCalibDB<T>::T_vector           T_vector ;
  typedef typename LArConditionsVsCalibDB<T>::CB_Tvec_pair       CB_Tvec_pair ;
  typedef typename LArConditionsVsCalibDB<T>::CB_vector          CB_vector ;
  typedef typename LArConditionsVsCalibDB<T>::Const_CB_It        Const_CB_It ;
  typedef typename LArConditionsVsCalibDB<T>::CB_It              CB_It ;
  
  LArConditionsVsCalib() {} ;
  virtual ~LArConditionsVsCalib() {} ;

  // retrieving conditions object using online ID (ChID is read-out or calibration channel)
  virtual const T& get(const HWIdentifier& ChID) const ;

  // retrieving conditions object using offline ID
  virtual const T& get(const Identifier& CellID) const ;

  // set method filling the data payload
  void setPdata(const HWIdentifier& ChID, const T& payload) ; 
  void setPdata(CB_Id id, const T_vector& payload_v) ;
  
protected:

  /// Access to standard empty value
  T& empty() const;

// //   bool operator< (const CB_Tvec_pair& x, const CB_Tvec_pair& y) 
// //   { return (x.first < y.first); } ;

//   class PairSort {
//   public:
//     bool operator () (const CB_Tvec_pair& x, const CB_Tvec_pair& y) 
//     { return (x.first < y.first); }
//     bool operator () (const CB_Tvec_pair& x, const unsigned int& id) 
//     { return (x.first < id); }
//     bool operator () (const unsigned int& id, const CB_Tvec_pair& y) 
//     { return (id < y.first); }
//   };

  
  
} ;

#ifndef __REFLEX__
#include "LArConditionsVsCalib.icc"
#endif 


#endif



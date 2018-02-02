// emacs: this is -*- c++ -*-
//
//   @file    TIDAFeature.h        
//
//            wrapper class to wrap objects such that they can be added 
//            to a generic store                     
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrackFeature.h, v0.0   Fri  7 Aug 2015 10:28:29 CEST sutt $


#ifndef  TIDA_FEATURE_H
#define  TIDA_FEATURE_H

#include <iostream>



namespace TIDA { 



/// very basic Feature base class just so that we can store derived 
/// classes onto the store 

class FeatureBase { 

public:

  virtual ~FeatureBase() { }  

  virtual void dummy() const = 0; 
};

}


inline std::ostream& operator<<( std::ostream& s, const TIDA::FeatureBase&  ) { 
  return s;
}




namespace TIDA {


/// actual template of the Feature wrapper - stores a pointer to an object
/// and inherits from FeatureBase

template<typename T>
class Feature : public FeatureBase {

public:

  typedef T value_type;

public:

  Feature( T* t ) : m_t(t) { } 

  virtual ~Feature() { } 

  T*       cptr()       { return m_t; } 
  const T* cptr() const { return m_t; } 

private:

  /// only needed to satify the pure virtual method 
  /// of the FeatureBase 
  virtual void dummy() const { };

private:

  T* m_t;

};

}


template<typename T>
inline std::ostream& operator<<( std::ostream& s, const TIDA::Feature<T>&  ) { 
  return s;
}



#endif  // TRACKFEATURE_H 











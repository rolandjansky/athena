/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Identifiers
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 03 May 2005
 * Last Update  : 09 May 2005
 ***************************************************************************/

/** \<\<class description should come here\>\> */

#ifndef MUONCALIBIDENTIFIER_HASHMAKER_H
# define MUONCALIBIDENTIFIER_HASHMAKER_H
// std

// other packages

// this package

// macro to create a full hash class implementation
#define CREATE_HASH_CLASS(ClassName) \
class ClassName {								\
public:										\
   ClassName() : m_data(UINT_MAX) {};						\
   explicit ClassName( unsigned int dat ) : m_data(dat) {}			\
   operator unsigned int () const {						\
      return m_data;								\
   }										\
   const ClassName& operator=( const ClassName& rhs ) {				\
      m_data = rhs.m_data;							\
      return *this;								\
   }										\
   const ClassName& operator=( unsigned int rhs ) {				\
      m_data = rhs;								\
      return *this;								\
   }										\
   bool isValid() const {							\
      return m_data != UINT_MAX;						\
   }										\
private:									\
   unsigned int m_data;								\
};


#endif // MUONCALIBIDENTIFIER_HASHMAKER_H

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IFilterCuts.h 
// Header file for class IFilterCuts
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_IFILTERCUTS_H 
#define ANALYSISUTILS_IFILTERCUTS_H 

/** IFilterCuts is a class which is used internally  
 *  by the filters to copy their cut properties one to another 
 *  (via the IFilterCuts::setFilter(ifilter) method).
 *  Note that this class (contrary to IFilter) is not templated.
 */

class IFilterCuts
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  IFilterCuts();

  /** Copy constructor: 
   */
  IFilterCuts( const IFilterCuts& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~IFilterCuts();

  /** Assignment operator: 
   */
  IFilterCuts& operator=( const IFilterCuts& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Pure virtual function (to be overloaded by each of the children
   *  classes) to copy the cut properties of a given filter to another one.
   *  It copies the properties of ifilter to the current IFilterCuts
   *  object.
   *  \param ifilter the IFilterCuts which holds a set of properties 
   *  one wants to copy to the current IFilterCuts object.
   */
  virtual void setFilter( const IFilterCuts * ifilter ) = 0;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline IFilterCuts::IFilterCuts() {}

inline IFilterCuts::IFilterCuts( const IFilterCuts& /*rhs*/ ) 
{
  // nothing to do because no data member (yet?)
}

inline IFilterCuts::~IFilterCuts() {}

inline IFilterCuts& IFilterCuts::operator=( const IFilterCuts& rhs ) 
{
  if ( this != & rhs ) {
    //do nothing because no data member to copy (yet?)
  }
  return *this;
}

#endif //> ANALYSISUTILS_IFILTERCUTS_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// IAssocFilter.h 
// Header file for class IAssocFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISASSOCIATION_IASSOCFILTER_H 
#define ANALYSISASSOCIATION_IASSOCFILTER_H 

/** \class IAssocFilter is a (templated) interface class to manage the
 *  construction/filtering of associations.
 *  It is the equivalent (but for associations) of the IFilter<T> class.
 *  The method of interest here (which will be overloaded by derived classes)
 *  is the "bool isAccepted(obj,assoc)" method which returns true if the
 *  pair of objects can be associated wrt the current set of filter cuts.
 */

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AnalysisUtils includes
#include "AnalysisUtils/IFilterCuts.h"

// Forward declaration

template< typename OBJ, typename ASSO>
class IAssocFilter : public IFilterCuts
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  IAssocFilter();

  /** Copy constructor: 
   */
  IAssocFilter( const IAssocFilter<OBJ,ASSO>& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~IAssocFilter(); 

  /** Assignment operator: 
   */
  IAssocFilter &operator=(const IAssocFilter<OBJ,ASSO>& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Main filter method. This method takes the decision whether or not
   *  the filter has to accept the association between both objects.
   * It returns true or false accordingly.
   */
  virtual bool isAccepted( const OBJ * object, 
			   const ASSO * associated ) const=0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Reset the range cuts for the internal variables (px,py,...)
   * After calling this method, the filter will accept any association
   */
  virtual void reset()=0;
  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
template<typename OBJ, typename ASSO>
IAssocFilter<OBJ, ASSO>::IAssocFilter() :
  IFilterCuts()
{}

template<typename OBJ, typename ASSO>
IAssocFilter<OBJ, ASSO>::IAssocFilter( const IAssocFilter<OBJ, ASSO>& rhs ) :
  IFilterCuts(rhs)
{}

template<typename OBJ, typename ASSO>
IAssocFilter<OBJ, ASSO>::~IAssocFilter()
{}

template<typename OBJ, typename ASSO>
IAssocFilter<OBJ, ASSO>&
IAssocFilter<OBJ, ASSO>::operator=( const IAssocFilter<OBJ, ASSO>& rhs )
{
  if ( this != &rhs ) {
    IFilterCuts::operator=(rhs);
  }
  return *this;
}

#endif //> ANALYSISASSOCIATION_IASSOCFILTER_H

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocs.h 
// Header file for class INav4MomAssocs
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef NAVFOURMOM_INAV4MOMASSOCS_H 
#define NAVFOURMOM_INAV4MOMASSOCS_H 

// STL includes

// DataModel includes
#include "DataModel/AssociationMap.h"
#include "DataModel/DataLink.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"

// Forward declaration

class INav4MomAssocs : public AssociationMap< INavigable4MomentumCollection, 
                                              INavigable4MomentumCollection >

{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  INav4MomAssocs();

  /** Copy constructor: 
   */
  INav4MomAssocs( const INav4MomAssocs& rhs );

  /** Assignment operator: 
   */
  INav4MomAssocs& operator=( const INav4MomAssocs& rhs ); 

  /** Constructor with parameters: 
   */

  /** Destructor: 
   */
  ~INav4MomAssocs(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Retrieve all the associated-to objects which have been associated
   *  to the given \param object
   */
  bool assocs( const INavigable4Momentum* object,
               INavigable4MomentumCollection& assocs ) const;

  /** Retrieve all the associated-to ElementLinks_to_objects which have
   *  been associated to the given \param objectLink
   */
  bool getAssociations( const INav4MomLink& objectLink,
                        INav4MomLinkContainer& associatedElementLinks ) const;

  /** Retrieve all the associated-to ElementLinks_to_objects which have
   *  been associated to the given \param object
   */
  bool getAssociations( const INavigable4Momentum* object,
                        INav4MomLinkContainer& associatedElementLinks ) const;

  /** Reimplement the method from the base class to not shaddow it */
  bool getAssociations(const AssociationMap< INavigable4MomentumCollection,
                       INavigable4MomentumCollection >::object_type* objPointer,
                       AssociationMap< INavigable4MomentumCollection,
                       INavigable4MomentumCollection >::asso_list& assocs) const
  {
    return AssociationMap< INavigable4MomentumCollection, 
      INavigable4MomentumCollection >::getAssociations( objPointer, assocs );
  }

  /** Reimplement the method from the base class to not shaddow it */
  bool getAssociations(const AssociationMap< INavigable4MomentumCollection,
                       INavigable4MomentumCollection >::object_iterator& objIter,
                       AssociationMap< INavigable4MomentumCollection,
                       INavigable4MomentumCollection >::asso_list& assocs) const
  {
    return AssociationMap< INavigable4MomentumCollection, 
      INavigable4MomentumCollection >::getAssociations( objIter, assocs );
  }

  /** Retrieve all the associated-to objects which have been associated
   *  to the given \param object
   */
  //template<typename CONT>
  //CONT getAssociations(const INavigable4Momentum* object)const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** @brief connect this association container to other ones.
   *  It is meant to be able to have underlying association stores
   *  which can be queried and laying into StoreGate without having 
   *   to copy them.
   */
  void addAssocStore( const DataLink<INav4MomAssocs>& assocStore );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  typedef DataLink<INav4MomAssocs> INav4MomAssocsLink_t;
  std::map<std::string, INav4MomAssocsLink_t> m_assocStores;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 


// Destructor
///////////////
inline INav4MomAssocs::~INav4MomAssocs() 
{}

CLASS_DEF( INav4MomAssocs, 94197919, 1 )

#endif //> NAVFOURMOM_INAV4MOMASSOCS_H

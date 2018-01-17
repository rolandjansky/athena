///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv_p3.h 
// Header file for class INav4MomAssocsCnv_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_p3_H 
#define EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_p3_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// DataModel includes
#include "AthLinks/ElementLink.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/DataLinkCnv_p2.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocs_p3.h"

// Forward declaration
class MsgStream;

class INav4MomAssocsCnv_p3 : public T_AthenaPoolTPCnvBase<
                                              INav4MomAssocs, 
                                              INav4MomAssocs_p3
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// converter for @c ElementLink to @c INavigable4MomentumCollection
  typedef ElementLinkCnv_p3< ElementLink<INavigable4MomentumCollection> >
          INav4MomCnv_t;
  typedef DataLinkCnv_p2<DataLink<INav4MomAssocs> > IAssocStoresCnv_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  INav4MomAssocsCnv_p3();

  /** Destructor: 
   */
  virtual ~INav4MomAssocsCnv_p3();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c INav4MomAssocs
   *  from its persistent representation @c INav4MomAssocs_p3
   */
  virtual void persToTrans( const INav4MomAssocs_p3* pers, 
                            INav4MomAssocs* trans, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c INav4MomAssocs_p3
   *  from its transient representation @c INav4MomAssocs
   */
  virtual void transToPers( const INav4MomAssocs* trans, 
                            INav4MomAssocs_p3* pers, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// converter for @c ElementLink to @c INavigable4MomentumCollection
  INav4MomCnv_t m_inav4MomLinkCnv;

  /// converter for @c DataLink to @c INav4MomAssocs
  IAssocStoresCnv_t m_assocStoresCnv;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline INav4MomAssocsCnv_p3::INav4MomAssocsCnv_p3()
{}

inline INav4MomAssocsCnv_p3::~INav4MomAssocsCnv_p3()
{}

#endif //> EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_p3_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_VECTELEMENTLINKCNV_P1_H
#define DATAMODELATHENAPOOL_VECTELEMENTLINKCNV_P1_H

/**
 *  @file ElementLinkVectorCnv_p1.h
 *  @brief This file contains the definition for the VectorElementLinkCnv_p1 class.
 NOTE: it should be included first, or the private->public hack will not work
 *  @author Marcin.Nowak@cern.ch
 **/

#include "VectorElementLink_p1.h"
#include "ElementLinkCnv_p3.h"

/** @class VectorElementLinkCnv_p1<LINK_VECT>
 *  @brief Converter template for converters between transient VectorElementLink and its persistent representation. Template argument LINK_VECT is the type of the transient VectorElementLink. The type of the persistent link vector representation is automatically deducted (it is based on the primiteve C++ type used by IndexingPolicy) - it can be integer index type, or string index type. More types can be added in ElementLink_p1.h
 **/

template <class LINK_VECT>
class VectorElementLinkCnv_p1
   : public T_AthenaPoolTPCnvBase< LINK_VECT, typename GeneratePersVectorELinkType_p1<LINK_VECT >::type > {
public:
  typedef	LINK_VECT	LinkVect_t;
  typedef 	typename GeneratePersVectorELinkType_p1< LinkVect_t >::type	PersLinkVect_t;

  VectorElementLinkCnv_p1() {}
  
  virtual void persToTrans(const PersLinkVect_t* pers, LinkVect_t* trans, MsgStream& log) ;
  virtual void transToPers(const LinkVect_t* trans, PersLinkVect_t* pers, MsgStream& log) ;
  
  virtual void persToTrans(const PersLinkVect_t& pers, LinkVect_t& trans, MsgStream& log) ;
  virtual void transToPers(const LinkVect_t& trans, PersLinkVect_t& pers, MsgStream& log) ;

protected:
  ElementLinkCnv_p3< typename LinkVect_t::value_type >          m_elLinkCnv;
};

#include "VectorElementLinkCnv_p1.icc"

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_ELEMENTLINKVECTCNV_P1_H
#define DATAMODELATHENAPOOL_ELEMENTLINKVECTCNV_P1_H

/**
 *  @file ElementLinkVectorCnv_p1.h
 *  @brief This file contains the class definition for the ElementLinkVectorCnv_p1 class.
 NOTE: it should be included first, or the private->public hack will not work
 *  @author Marcin.Nowak@cern.ch
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ElementLinkVector_p1.h"
#include "DataLinkCnv_p1.h"

/** @class ElementLinkVectorCnv_p1<LINK_VECT>
 *  @brief Converter template for converters between transient ElementLinkVector and its persistent representation. Template argument LINK_VECT is the type of the transient ElementLinkVector. The type of the persistent link vector representation is automatically deducted (it is based on the primiteve C++ type used by IndexingPolicy) - it can be integer index type, or string index type. More types can be added in ElementLink_p1.h
 **/

template <class LINK_VECT>
class ElementLinkVectorCnv_p1
   : public T_AthenaPoolTPCnvBase< LINK_VECT, typename GeneratePersELinkVectorType_p1<LINK_VECT >::type > {
public:
  typedef	LINK_VECT	LinkVect_t;
  typedef 	typename GeneratePersELinkVectorType_p1< LinkVect_t >::type	PersLinkVect_t;
  typedef       typename PersLinkVect_t::Index_t Index_t;

  ElementLinkVectorCnv_p1() {}
  
  virtual void persToTrans(const PersLinkVect_t* pers, LinkVect_t* trans, MsgStream& log) ;
  virtual void transToPers(const LinkVect_t* trans, PersLinkVect_t* pers, MsgStream& log) ;
  
  virtual void persToTrans(const PersLinkVect_t& pers, LinkVect_t& trans, MsgStream& log) ;
  virtual void transToPers(const LinkVect_t& trans, PersLinkVect_t& pers, MsgStream& log) ;

protected:
  DataLinkVectorCnv_p1<typename LinkVect_t::DataLinkVector >	m_DLinkVectorCnv;
};

#ifndef __REFLEX__
#include "ElementLinkVectorCnv_p1.icc"
#endif // not __REFLEX__

#endif

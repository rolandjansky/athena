/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
namespace SG {
  class ThinningCache;
}

/** @class ElementLinkVectorCnv_p1<LINK_VECT>
 *  @brief Converter template for converters between transient ElementLinkVector and its persistent representation. Template argument LINK_VECT is the type of the transient ElementLinkVector. The type of the persistent link vector representation is automatically deducted (it is based on the primiteve C++ type used by IndexingPolicy) - it can be integer index type, or string index type. More types can be added in ElementLink_p1.h
 **/

template <class LINK_VECT>
class ElementLinkVectorCnv_p1
   : public T_AthenaPoolTPCnvConstBase< LINK_VECT, typename GeneratePersELinkVectorType_p1<LINK_VECT >::type > {
public:
  typedef	LINK_VECT	LinkVect_t;
  typedef 	typename GeneratePersELinkVectorType_p1< LinkVect_t >::type	PersLinkVect_t;
  typedef       typename PersLinkVect_t::Index_t Index_t;

  using base_class = T_AthenaPoolTPCnvConstBase< LINK_VECT, PersLinkVect_t >;
  using base_class::transToPers;
  using base_class::persToTrans;

  ElementLinkVectorCnv_p1() {}
  
  void transToPers(const LinkVect_t& trans, PersLinkVect_t& pers,
                   const SG::ThinningCache* cache,
                   MsgStream& log) const;

  void transToPers(const LinkVect_t& trans, PersLinkVect_t& pers, MsgStream& log) const;
  void persToTrans(const PersLinkVect_t& pers, LinkVect_t& trans, MsgStream& log) const;

  virtual void transToPers(const LinkVect_t* trans, PersLinkVect_t* pers, MsgStream& log) const override;
  virtual void persToTrans(const PersLinkVect_t* pers, LinkVect_t* trans, MsgStream& log) const override;


protected:
  DataLinkVectorCnv_p1<typename LinkVect_t::DataLinkVector >	m_DLinkVectorCnv;
};

#ifndef __REFLEX__
#include "ElementLinkVectorCnv_p1.icc"
#endif // not __REFLEX__

#endif

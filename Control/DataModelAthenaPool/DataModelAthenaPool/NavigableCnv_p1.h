/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLECNV_P1_H
#define DATAMODELATHENAPOOL_NAVIGABLECNV_P1_H

/**
 *  @file NavigableCnv_p1.h
 *  @brief This file contains the class definition for the NavigableCnv_p1 class.
 NOTE: it should be included first, or the private->public hack will not work
 *  @author Marcin.Nowak@cern.ch
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthLinks/ElementLinkVector.h"
#include "ElementLinkVectorCnv_p1.h"
#include "Navigable_p1.h"

/** @class NavigableCnv_p1<NAV>
 *  @brief Converter template for converters between transient Navigable and its persistent representation. Template argument NAV is the type of the transient Navigable. 
 **/

template <class NAV, typename RPAR = typename NAV::parameter_type>
class NavigableCnv_p1
   : public T_AthenaPoolTPCnvBase< NAV, typename GeneratePersNavigableType<NAV, RPAR >::type > {
public:
  typedef	NAV							Navigable_t;
  typedef 	typename GeneratePersNavigableType<Navigable_t, RPAR >::type	PersNavigable_t;
  typedef	ElementLinkVector<typename NAV::container_type > 	ElementLinkVect_t;

  NavigableCnv_p1() {}

  // standard interface for TP converter
  virtual void transToPers(const Navigable_t* trans, PersNavigable_t* pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t* pers, Navigable_t* trans, MsgStream& log) ;

  virtual void transToPers(const Navigable_t& trans, PersNavigable_t& pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t& pers, Navigable_t& trans, MsgStream& log) ;

protected:
  // converter for element link vector
  ElementLinkVectorCnv_p1<ElementLinkVect_t >	m_linkCnv;
};


// Specialized version of the converter for Navigables without weight parameters

template <class NAV >
class NavigableCnv_p1<NAV, NavigationDefaults::DefaultWeight >
   : public T_AthenaPoolTPCnvBase< NAV, typename GeneratePersNavigableType<NAV >::type > {
public:
  typedef	NAV							Navigable_t;
  typedef 	typename GeneratePersNavigableType<Navigable_t >::type	PersNavigable_t;
  typedef	ElementLinkVector<typename NAV::container_type > 	ElementLinkVect_t;

  NavigableCnv_p1() {}

  // standard interface for TP converter
  virtual void transToPers(const Navigable_t* trans, PersNavigable_t* pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t* pers, Navigable_t* trans, MsgStream& log) ;

  virtual void transToPers(const Navigable_t& trans, PersNavigable_t& pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t& pers, Navigable_t& trans, MsgStream& log) ;

protected:
  // converter for element link vector
  ElementLinkVectorCnv_p1<ElementLinkVect_t >	m_linkCnv;
};


#ifndef __REFLEX__
#include "NavigableCnv_p1.icc"
#endif // not __REFLEX__

#endif


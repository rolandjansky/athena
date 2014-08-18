/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLECNV_P2_H
#define DATAMODELATHENAPOOL_NAVIGABLECNV_P2_H

/**
 *  @file NavigableCnv_p2.h
 *  @brief This file contains the class definition for the NavigableCnv_p2 class.
 NOTE: it should be included first, or the private->public hack will not work
 *  @author Marcin.Nowak@cern.ch
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthLinks/ElementLink.h"
#include "VectorElementLinkCnv_p1.h"
#include "Navigable_p2.h"

/** @class NavigableCnv_p2<NAV>
 *  @brief Converter template for converters between transient Navigable and its persistent representation. Template argument NAV is the type of the transient Navigable. 
 **/

template <class NAV, typename RPAR = typename NAV::parameter_type>
class NavigableCnv_p2
   : public T_AthenaPoolTPCnvBase< NAV, typename GeneratePersNavigableType_p2<NAV, RPAR >::type > {
public:
  typedef	NAV							Navigable_t;
  typedef 	typename GeneratePersNavigableType_p2<Navigable_t, RPAR >::type	PersNavigable_t;
  typedef	std::vector<ElementLink<typename NAV::container_type> > ElementLinkVect_t;

  NavigableCnv_p2() {}

  // standard interface for TP converter
  virtual void transToPers(const Navigable_t* trans, PersNavigable_t* pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t* pers, Navigable_t* trans, MsgStream& log) ;

  virtual void transToPers(const Navigable_t& trans, PersNavigable_t& pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t& pers, Navigable_t& trans, MsgStream& log) ;

protected:
  // converter for element link vector
  VectorElementLinkCnv_p1< ElementLinkVect_t >	m_linkCnv;
};


// Specialized version of the converter for Navigables without weight parameters

template <class NAV >
class NavigableCnv_p2<NAV, NavigationDefaults::DefaultWeight >
   : public T_AthenaPoolTPCnvBase< NAV, typename GeneratePersNavigableType_p2<NAV >::type > {
public:
  typedef	NAV							Navigable_t;
  typedef 	typename GeneratePersNavigableType_p2<Navigable_t >::type PersNavigable_t;
  typedef	std::vector<ElementLink<typename NAV::container_type> > ElementLinkVect_t;

  NavigableCnv_p2() {}

  // standard interface for TP converter
  virtual void transToPers(const Navigable_t* trans, PersNavigable_t* pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t* pers, Navigable_t* trans, MsgStream& log) ;

  virtual void transToPers(const Navigable_t& trans, PersNavigable_t& pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigable_t& pers, Navigable_t& trans, MsgStream& log) ;

protected:
  // converter for element link vector
  VectorElementLinkCnv_p1< ElementLinkVect_t >	m_linkCnv;
};


#ifndef __REFLEX__
#include "NavigableCnv_p2.icc"
#endif // not __REFLEX__

#endif


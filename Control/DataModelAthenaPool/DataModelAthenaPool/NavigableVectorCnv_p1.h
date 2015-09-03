/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLEVECTCNV_P1_H
#define DATAMODELATHENAPOOL_NAVIGABLEVECTCNV_P1_H

/**
 *  @file NavigableVectorCnv_p1.h
 *  @brief This file contains the class definition for the NavigableVectorCnv_p1 class.
 NOTE: it should be included first, or the private->public hack will not work
 *  @author Marcin.Nowak@cern.ch
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ElementLinkVectorCnv_p1.h"
#include "NavigableVector_p1.h"

/** @class NavigableVectorCnv_p1<NAV>
 *  @brief Converter template for converters between transient NavigableVector and its persistent representation. Template argument NAV is the type of the transient Navigable. 
 **/

template < class NAVVECT, typename RPAR = typename NAVVECT::value_type::parameter_type >
class NavigableVectorCnv_p1
   : public T_AthenaPoolTPCnvBase< NAVVECT, typename GeneratePersNavVectorType<typename NAVVECT::value_type, RPAR >::type >
{
public:
  typedef	NAVVECT						NavigableVector_t;
  typedef	typename NavigableVector_t::value_type		Navigable_t;
  typedef 	typename GeneratePersNavVectorType< Navigable_t, RPAR >::type	PersNavigableVector_t;
  typedef	ElementLinkVector<typename Navigable_t::container_type > 	ElementLinkVect_t;

  NavigableVectorCnv_p1() {}

  // standard interface for TP converter
  // (takes the whole vector of navigables at once)
  virtual void transToPers(const NavigableVector_t* trans, PersNavigableVector_t* pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigableVector_t* pers, NavigableVector_t* trans, MsgStream& log) ;

  virtual void transToPers(const NavigableVector_t& trans, PersNavigableVector_t& pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigableVector_t& pers, NavigableVector_t& trans, MsgStream& log) ;

  // additional API for adding Navigables one by one to the persistent vector
  virtual void	startTransToPers( PersNavigableVector_t& pers, MsgStream& log);
  virtual void 	addTransNavigable( const Navigable_t& trans );
  virtual void 	finishTransToPers();

  // additional API for reading Navigables one by one from the persistent vector
  virtual void	startPersToTrans( const PersNavigableVector_t& pers, MsgStream& log);
  virtual void	getNextNavigable( Navigable_t& trans );
  virtual void	finishPersToTrans();
  
   
protected:
  // converter for element link vector
  ElementLinkVectorCnv_p1<ElementLinkVect_t >	m_linkCnv;

  PersNavigableVector_t		*m_pers;
  MsgStream			*m_log;
  ElementLinkVect_t 		*m_tempLinkVect;

  int				m_linkIterator;
  int				m_navigableIterator;
};


// Specialized version of the converter for NavigableVectors without weight parameters

template < class NAVVECT >
class NavigableVectorCnv_p1<NAVVECT, NavigationDefaults::DefaultWeight >
   : public T_AthenaPoolTPCnvBase< NAVVECT, typename GeneratePersNavVectorType<typename NAVVECT::value_type >::type >
{
public:
  typedef	NAVVECT						NavigableVector_t;
  typedef	typename NavigableVector_t::value_type		Navigable_t;
  typedef 	typename GeneratePersNavVectorType< Navigable_t >::type	PersNavigableVector_t;
  typedef	ElementLinkVector<typename Navigable_t::container_type > 	ElementLinkVect_t;

  NavigableVectorCnv_p1() {}

  // standard interface for TP converter
  // (takes the whole vector of navigables at once)
  virtual void transToPers(const NavigableVector_t* trans, PersNavigableVector_t* pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigableVector_t* pers, NavigableVector_t* trans, MsgStream& log) ;

  virtual void transToPers(const NavigableVector_t& trans, PersNavigableVector_t& pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigableVector_t& pers, NavigableVector_t& trans, MsgStream& log) ;

  // additional API for adding Navigables one by one to the persistent vector
  virtual void	startTransToPers( PersNavigableVector_t& pers, MsgStream& log);
  virtual void 	addTransNavigable( const Navigable_t& trans );
  virtual void 	finishTransToPers();

  // additional API for reading Navigables one by one from the persistent vector
  virtual void	startPersToTrans( const PersNavigableVector_t& pers, MsgStream& log);
  virtual void	getNextNavigable( Navigable_t& trans );
  virtual void	finishPersToTrans();
  
   
protected:
  // converter for element link vector
  ElementLinkVectorCnv_p1<ElementLinkVect_t >	m_linkCnv;

  PersNavigableVector_t		*m_pers;
  MsgStream			*m_log;
  ElementLinkVect_t 		*m_tempLinkVect;

  int				m_linkIterator;
  int				m_navigableIterator;
};




/*
template <class NAV >
class NavigableVectorCnv_p1<NAV, NavigationDefaults::DefaultWeight >
   : public T_AthenaPoolTPCnvBase< NAV, typename GeneratePersNavVectorType<NAV >::type > {
public:
  typedef	NAV							NavigableVector_t;
  typedef 	typename GeneratePersNavVectorType<Navigable_t >::type	PersNavigableVector_t;
  typedef	ElementLinkVector<typename NAV::container_type > 	ElementLinkVect_t;

  NavigableVectorCnv_p1() {}

  // standard interface for TP converter
  virtual void transToPers(const NavigableVector_t* trans, PersNavigableVector_t* pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigableVector_t* pers, NavigableVector_t* trans, MsgStream& log) ;

  virtual void transToPers(const NavigableVector_t& trans, PersNavigableVector_t& pers, MsgStream& log) ;
  virtual void persToTrans(const PersNavigableVector_t& pers, NavigableVector_t& trans, MsgStream& log) ;

protected:
  // converter for element link vector
  ElementLinkVectorCnv_p1<ElementLinkVect_t >	m_linkCnv;
};
*/


#include "NavigableVectorCnv_p1.icc"

#endif


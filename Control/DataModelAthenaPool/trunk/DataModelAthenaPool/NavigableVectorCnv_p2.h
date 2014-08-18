/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLEVECTCNV_P2_H
#define DATAMODELATHENAPOOL_NAVIGABLEVECTCNV_P2_H

/**
 *  @file NavigableVectorCnv_p2.h
 *  @brief This file contains the class definition for the NavigableVectorCnv_p2 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "ElementLinkCnv_p3.h"
#include "NavigableVector_p2.h"

/** @class NavigableVectorCnv_p2<NAV>
 *  @brief Converter template for converters between a vector of Navigables and its persistent representation. Template argument NAVVECT is the type of the transient Navigable vector (coded with std::vector<> in mind). 
 **/

template < class NAVVECT, typename RPAR = typename NAVVECT::value_type::parameter_type >
class NavigableVectorCnv_p2
   : public T_AthenaPoolTPCnvBase< NAVVECT, typename GeneratePersNavVectorType_p2<typename NAVVECT::value_type, RPAR >::type >
{
public:
  typedef	NAVVECT						NavigableVector_t;
  typedef	typename NavigableVector_t::value_type		Navigable_t;
  typedef 	typename GeneratePersNavVectorType_p2< Navigable_t, RPAR >::type	PersNavigableVector_t;

  NavigableVectorCnv_p2() {}

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
  // converter for element links
  ElementLinkCnv_p3< ElementLink< typename Navigable_t::container_type > > m_linkCnv;

  PersNavigableVector_t		*m_pers;
  MsgStream			*m_log;

  int				m_linkIterator;
  int				m_navigableIterator;
};



// Specialized version of the converter for NavigableVectors without weight parameters

template < class NAVVECT >
class NavigableVectorCnv_p2<NAVVECT, NavigationDefaults::DefaultWeight >
   : public T_AthenaPoolTPCnvBase< NAVVECT, typename GeneratePersNavVectorType_p2<typename NAVVECT::value_type >::type >
{
public:
  typedef	NAVVECT						NavigableVector_t;
  typedef	typename NavigableVector_t::value_type		Navigable_t;
  typedef 	typename GeneratePersNavVectorType_p2< Navigable_t >::type	PersNavigableVector_t;

  NavigableVectorCnv_p2() {}

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
  // converter for element links
  ElementLinkCnv_p3< ElementLink< typename Navigable_t::container_type > > m_linkCnv;

  PersNavigableVector_t		*m_pers;
  MsgStream			*m_log;

  int				m_linkIterator;
  int				m_navigableIterator;
};


#include "NavigableVectorCnv_p2.icc"

#endif


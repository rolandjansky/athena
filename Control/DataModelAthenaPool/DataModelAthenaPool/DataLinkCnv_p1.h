/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_DATALINKCNV_P1_H
#define DATAMODELATHENAPOOL_DATALINKCNV_P1_H

/**
 *  @file DataLinkCnv_p1.h
 *  @brief This file contains the class definition for the DataLinkCnv_p1 class and DataLinkVectorCnv_p1 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataLink_p1.h"

/** @class DataLinkCnv_p1<DLINK>
 *  @brief Converter template for converters between transient DataLink and its persistent representation. Template argument DLINK is the type of the transient DataLink. 
 **/

template <class DLINK_TYPE>
class DataLinkCnv_p1
   : public T_AthenaPoolTPCnvBase< DLINK_TYPE, DataLink_p1 > {
public:
  typedef	DLINK_TYPE	DLink_t;
  typedef 	DataLink_p1	PersDLink_t;

  DataLinkCnv_p1() {}
  
  virtual void persToTrans(const PersDLink_t* pers, DLink_t* trans, MsgStream& log) ;
  virtual void transToPers(const DLink_t* trans, PersDLink_t* pers, MsgStream& log) ;

  virtual void persToTrans(const PersDLink_t& pers, DLink_t& trans, MsgStream& log) ;
  virtual void transToPers(const DLink_t& trans, PersDLink_t& pers, MsgStream& log) ;
};

#ifndef __REFLEX__
#include "DataLinkCnv_p1.icc"
#endif // not __REFLEX__


/** @class DataLinkVectorCnv_p1<DLINKVector_TYPE>
 *  @brief Converter template for converters between transient vectors of DataLinks and their persistent representation DataLinkVector_p1. Template argument DLINKVector_TYPE is the type of the transient DataLink vector. 
 **/

template <class DLINKVector_TYPE>
class DataLinkVectorCnv_p1
   : public T_AthenaPoolTPCnvStdVector< DLINKVector_TYPE,
				     DataLinkVector_p1,
				     DataLinkCnv_p1<typename DLINKVector_TYPE::value_type > >
{
public:
  DataLinkVectorCnv_p1() {}
};

#endif

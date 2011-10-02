// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IOVDbDataModelDict.h
 *
 * @brief This file includes the class for dictionary definitions
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: IOVDbDataModelDict.h,v 1.1 2008-06-16 15:19:38 schaffer Exp $
 */
#ifndef IOVDBDATAMODEL_IOVDBDATAMODELDICT_H
# define IOVDBDATAMODEL_IOVDBDATAMODELDICT_H 1

#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

#include "RootUtils/PyROOTIteratorFuncs.h" 

namespace IOVDbDataModelDict {

  struct tmp {
    RootUtils::PyROOTIteratorFuncs<IOVPayloadContainer::const_iterator> m_itr;
  };

} //> ! namespace IOVDbDataModelDict

#endif // IOVDBDATAMODEL_IOVDBDATAMODELDICT_H

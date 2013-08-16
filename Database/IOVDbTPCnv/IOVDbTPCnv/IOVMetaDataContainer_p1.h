/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBTMCNV_IOVMETADATACONTAINER_P1_H
#define IOVDBTMCNV_IOVMETADATACONTAINER_P1_H

/**
 * @file IOVMetaDataContainer_p1.h
 *
 * @brief This class is a container for conditions data. It is
 * intended to be used to store conditions data from COOL as file meta
 * data.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Antoine Perus <perus@lal.in2p3.fr>
 *
 * $Id: IOVMetaDataContainer_p1.h,v 1.3 2007-07-23 13:53:29 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>
#include "IOVDbTPCnv/IOVPayloadContainer_p1.h"

#include <string>
#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class IOVMetaDataContainer_p1
 *
 * @brief This class is a container_p1 for conditions data. It is
 * intended to be used to store conditions data from COOL as file meta
 * data.
 *
 */

class IOVMetaDataContainer_p1 {
public:

    IOVMetaDataContainer_p1();

    std::string                     m_folderName;
    std::string                     m_folderDescription;
    IOVPayloadContainer_p1          m_payload;
};


inline 
IOVMetaDataContainer_p1::IOVMetaDataContainer_p1()
{}



#endif  //  IOVDBTMCNV_IOVMETADATACONTAINER_P1_H

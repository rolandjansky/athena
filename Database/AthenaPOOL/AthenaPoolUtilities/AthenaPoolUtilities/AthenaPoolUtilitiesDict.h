/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolUtilitiesDict.h
 *
 * @brief This file includes the class for dictionary definitions
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolUtilitiesDict.h,v 1.7 2008-10-27 21:24:23 cranshaw Exp $
 */
#ifndef ATHENAPOOLUTILITIES_ATHENAPOOLUTILITIESDICT_H
# define ATHENAPOOLUTILITIES_ATHENAPOOLUTILITIESDICT_H

#ifdef __REFLEX__
#include "CoralBase/VersionInfo.h"
#undef CORAL240AS
#include "CoralBase/AttributeList.h"
#endif

#include "AthenaPoolUtilities/CondMultChanCollImpl.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollAddress.h"
#include "AthenaPoolUtilities/CondAttrListVecAddress.h"

#include "AthenaPoolUtilities/TPObjRef.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"
#include "AthenaPoolUtilities/TPIntegerVector_p2.h"

namespace AthenaPoolUtilitiesDummyClasses 
{
   struct tmp {
     CondAttrListCollection::const_iterator               m_it;
     CondAttrListCollection::iov_const_iterator           m_it1;
     std::pair<unsigned int, coral::AttributeList>        m_pair;
     std::pair<unsigned int, IOVRange>                    m_pair1;
     std::vector<TPObjRef>                                m_refvect;
     std::vector<CondAttrListCollection*>                 m_collvect;
     std::vector<CondAttrListCollection*>::const_iterator m_it2;

     TPIntegerVectorStorage_p2 tpivdummy; // not sure it's needed ...
   };
}

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#endif // ATHENAPOOLUTILITIES_ATHENAPOOLUTILITIESDICT_H

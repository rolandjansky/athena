/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCOMMONTPCNV_EVENTCOMMONTPCNVDICT_H
#define EVENTCOMMONTPCNV_EVENTCOMMONTPCNVDICT_H
/**
 * @file EventCommonTPCnvDict.h
 *
 * @brief Header file for dictionary generation
 *
 * @author  Sebastien Binet <binet@cern.ch>
 */

#include <vector>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventCommonTPCnv/P4EEtaPhiM_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMFloat_p2.h"
#include "EventCommonTPCnv/P4IPtCotThPhiM_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMFloat_p2.h"
#include "EventCommonTPCnv/P4PxPyPzE_p1.h"

#include "EventCommonTPCnv/HepLorentzVector_p1.h"

#include "EventCommonTPCnv/INav4MomLinkContainer_p1.h"

#include "EventCommonTPCnv/IParticleLinkContainer_p1.h"

#include "EventCommonTPCnv/INav4MomAssocs_p1.h"
#include "EventCommonTPCnv/INav4MomAssocs_p2.h"
#include "EventCommonTPCnv/INav4MomAssocs_p3.h"



namespace EventCommonTPCnvDict_tmp {

  struct temp {

    // not strictly needed for this package, but in order to rationalize
    // creation of dicts. we put that here so it is not scattered over clients
    std::vector<P4EEtaPhiM_p1>     m_p4eetaphims;
    std::vector<P4EEtaPhiMFloat_p2>     m_p4eetaphimvs;
    std::vector<P4IPtCotThPhiM_p1> m_p4iptcotthphims;
    std::vector<P4PtEtaPhiM_p1>    m_p4ptetaphims;
    std::vector<P4PtEtaPhiMFloat_p2>    m_p4ptetaphimvs;
    std::vector<P4PxPyPzE_p1>      m_p4pxpypzs;

    INav4MomLinkContainer_p1 m_inav4momlink_p1;

    IParticleLinkContainer_p1 m_ipartlink_p1;

    INav4MomAssocs_p2 m_assocs_p2;
    INav4MomAssocs_p2::Assocs_t m_assocs_assocs_p2;
    INav4MomAssocs_p2::AssocElem_t m_assocs_elems_p2;
    INav4MomAssocs_p3 m_assocs_p3;
    INav4MomAssocs_p3::Assocs_t m_assocs_assocs_p3;
    INav4MomAssocs_p3::AssocElem_t m_assocs_elems_p3;
  };
}

#endif // EVENTCOMMONTPCNV_EVENTCOMMONTPCNVDICT_H


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DPDATTRIBUTENAMES_H
#define DPDATTRIBUTENAMES_H

/****************************************
 Author - Tulay Cuhadar Donszelmann
 Dec 2008
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>
#include <map>


/** list of ALL the DPD attributes and their types */
namespace DPDId {
  static const int NdpdAttr=2;    // This the number of DPD TAG attributes
  static const int NumDpd = 1;    // This is the max number of DPD words in the TAG per some Attributes
  enum { Daod=0, Desd=1};
}

/** This is the list of DPD Phys TAG attributes */
static const std::string DPDAttributeNames[DPDId::NdpdAttr] = {"DAODStreams",
                                                               "DESDStreams"};

/** This is the list of DPD Phys TAG attributes */
static const std::string DPDAttributeUnitNames[DPDId::NdpdAttr] = {UnitNames[Unt::Bitmask],
                                                                   UnitNames[Unt::Bitmask]};

/** This is the list of DPD Phys TAG attributes */
static const std::string DPDAttributeGroupNames[DPDId::NdpdAttr] = {GroupNames[Grp::TriggerStream],
                                                                    GroupNames[Grp::TriggerStream]
                                                                   };

struct DPDBits
{ 
   DPDBits();

   std::map<const std::string, unsigned int> daodMap;
   std::map<const std::string, unsigned int> desdMap;
};

DPDBits::DPDBits()
{
   daodMap.insert(std::make_pair("Other_StreamDAOD_2PH_AcceptEvent"    ,0)); 
   daodMap.insert(std::make_pair("Other_StreamDAOD_EMTAU_AcceptEvent"  ,1)); 
   daodMap.insert(std::make_pair("Other_StreamDAODM_SGLEM_AcceptEvent" ,2)); 
   daodMap.insert(std::make_pair("Other_StreamDAOD_TAUJET_AcceptEvent" ,3));
   daodMap.insert(std::make_pair("Other_StreamDAODM_SGLPH_AcceptEvent" ,4));
   daodMap.insert(std::make_pair("Other_StreamDAODM_SGLTAU_AcceptEvent",5)); 
   daodMap.insert(std::make_pair("Other_StreamDAOD_JETMET_AcceptEvent" ,6));
   daodMap.insert(std::make_pair("Other_StreamDAOD_EMJET_AcceptEvent"  ,7)); 
   daodMap.insert(std::make_pair("Other_StreamDAOD_EMMET_AcceptEvent"  ,8));
   daodMap.insert(std::make_pair("Other_StreamDAOD_2EM_AcceptEvent"    ,9));
   daodMap.insert(std::make_pair("Other_StreamDAOD_MUJET_AcceptEvent" ,10)); 
   daodMap.insert(std::make_pair("Other_StreamDAOD_2MU_AcceptEvent"   ,11));
   daodMap.insert(std::make_pair("Other_StreamDAOD_MUTAU_AcceptEvent" ,12)); 
   daodMap.insert(std::make_pair("Other_StreamDAODM_SGLMU_AcceptEvent",13));
   daodMap.insert(std::make_pair("Other_StreamDAOD_MUMET_AcceptEvent" ,14)); 
   daodMap.insert(std::make_pair("Other_StreamDAOD_EMMU_AcceptEvent"  ,15));

   desdMap.insert(std::make_pair("Other_StreamDESDM_EGAMMA_AcceptEvent" ,0)); 
   desdMap.insert(std::make_pair("Other_StreamDESDM_TRACK_AcceptEvent"  ,1));
   desdMap.insert(std::make_pair("Other_StreamDESD_MBIAS_AcceptEvent"   ,2)); 
   desdMap.insert(std::make_pair("Other_StreamDESD_MET_AcceptEvent"     ,3));
   desdMap.insert(std::make_pair("Other_StreamDESD_DESD_SGLMU_AcceptEvent",4)); 
   desdMap.insert(std::make_pair("Other_StreamDESD_PHOJET_AcceptEvent"  ,5));
   desdMap.insert(std::make_pair("Other_StreamDESDM_CALJET_AcceptEvent" ,6)); 
   desdMap.insert(std::make_pair("Other_StreamDESD_COLLCAND_AcceptEvent",7));
   desdMap.insert(std::make_pair("Other_StreamDESD_SGLEL_AcceptEvent"   ,8)); 
   desdMap.insert(std::make_pair("Other_StreamDESDM_MUON_AcceptEvent"   ,9));
}

#endif



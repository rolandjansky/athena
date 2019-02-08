/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LARHITLIST_H
#define LARDIGITIZATION_LARHITLIST_H
#include <vector>
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "Identifier/Identifier.h"
#include "LArIdentifier/LArOnlineID.h"

class LArHitList {

 typedef std::vector< std::pair<float,float> > LARLIST;

 private:
   Identifier m_identifier;
   HWIdentifier m_online;
   const CaloDetDescrElement * m_caloDDE;
   bool m_inWindows;
   LARLIST m_list;

 public:

   LArHitList(Identifier id, const CaloDetDescrElement *calodde, ToolHandle<LArCablingLegacyService>& cablingService){
     m_identifier = id;
     m_online =  cablingService->createSignalChannelID(id);
     m_caloDDE = calodde;
     m_inWindows = false;
     m_list.clear();
   };

  ~LArHitList(){};

   void AddHit(float energy,float time) {
    std::pair<float,float> TimeE(energy,time);
    m_list.push_back(TimeE);
   }

   void Reset() {
     m_inWindows=false;
     m_list.clear();
   }

   HWIdentifier getOnlineIdentifier() {
     return(m_online);
   }

   Identifier getIdentifier() {
     return(m_identifier);
   }

   const CaloDetDescrElement* getCaloDDE() {return m_caloDDE;};

   LARLIST* getData() { return &m_list;}

   bool inWindows() {return m_inWindows;};

   void setInWindows() {m_inWindows=true;};

};
#endif

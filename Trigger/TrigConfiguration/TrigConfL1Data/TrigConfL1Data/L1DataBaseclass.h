/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     L1DataBaseclass.h
//PACKAGE:  TrigConfL1Data           
//                                                     
//AUTHOR:   D.Berge (CERN)	David.Berge@cern.ch 
//CREATED:  03 July 2007
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_L1DataBaseclass
#define TrigConf_L1DataBaseclass

#include "TrigConfL1Data/TrigConfData.h"

namespace TrigConf {
  
   class L1DataBaseclass : public TrigConfData {
   public:
      L1DataBaseclass() :
         TrigConfData(),
         m_Lvl1MasterId(0)
      {}
      virtual ~L1DataBaseclass() = default;

      unsigned int lvl1MasterTableId() const { return m_Lvl1MasterId; }
      void setLvl1MasterTableId ( unsigned int id) { m_Lvl1MasterId = id; }

   private:
    
      unsigned int m_Lvl1MasterId;

   };
}

#endif

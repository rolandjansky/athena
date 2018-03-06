/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_SlhcIdConverter.h
 * @file Header file for converter functions for Identifiers 64(SLHC) / 32(normal) bit
 * @author Shaun Roe (shaun.roe@cern.ch)
 **/
 #include "Identifier/Identifier.h"
 
 namespace SCT_ConditionsServices{
   inline unsigned int castId(const Identifier & identifier){
     return identifier.get_identifier32().get_compact();
   }
 }//end of namespace



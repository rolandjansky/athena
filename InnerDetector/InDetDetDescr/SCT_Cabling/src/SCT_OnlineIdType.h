/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_OnlineIdType_H
#define SCT_OnlineIdType_H
/**
 *  @file SCT_OnlineIdType.h
 *  Header file for function to check the Rod or online Id for validity
 *
 *  @author Shaun Roe 
 *  @date 25/02/2009
 *
 */
namespace SCT_Cabling{
  enum DataSource {ROD_VALID , ROD_DB_ONLY, ROD_MC_ONLY, ROD_INVALID};
  DataSource 
  onlineIdType(const unsigned int rodId);
}
#endif

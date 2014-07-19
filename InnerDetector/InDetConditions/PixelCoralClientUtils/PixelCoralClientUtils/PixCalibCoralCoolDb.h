/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _PixCalibCoralCoolDb_h_
#define _PixCalibCoralCoolDb_h_

#include "CoraCool/CoraCoolDatabase.h"

#include <string>

class PixCalibCoralCoolDb
{
 public:
  PixCalibCoralCoolDb(std::string dbString, int verbose);
  ~PixCalibCoralCoolDb();

  bool init();
  bool load(cool::ValidityKey vk);
  
/*   bool save(cool::ValidityKey vk1, */
/* 	    cool::ValidityKey vk2); */

  bool saveCalibData(std::string textfile, long long FK);
  bool referenceToRunInterval(long long FK, cool::ValidityKey vk1, cool::ValidityKey vk2, const std::string tagname );

 private:
  std::string m_dbstring;
  int m_verbose;

  CoraCoolDatabasePtr m_coradb;
};

#endif

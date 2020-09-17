/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCORALCLIENTUTILS_PIXCALIBCORALCOOLDB_H
#define PIXELCORALCLIENTUTILS_PIXCALIBCORALCOOLDB_H

#include "CoraCool/CoraCoolDatabase.h"
#include "CxxUtils/checker_macros.h"

#include <string>

class ATLAS_NOT_THREAD_SAFE PixCalibCoralCoolDb // Use of singleton databaseService
{
 public:
  PixCalibCoralCoolDb(const std::string& dbString, int verbose);
  ~PixCalibCoralCoolDb();

  bool init();
  bool load(cool::ValidityKey vk);
  
/*   bool save(cool::ValidityKey vk1, */
/* 	    cool::ValidityKey vk2); */

  bool saveCalibData(std::string textfile, long long FK);
  bool referenceToRunInterval(long long FK, cool::ValidityKey vk1, cool::ValidityKey vk2, const std::string& tagname );

 private:
  std::string m_dbstring;

  CoraCoolDatabasePtr m_coradb;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldMapMgr_H
#define FieldMapMgr_H

#include <string>

#include "PathResolver/PathResolver.h"

class FieldMapMgr {
public:
        static FieldMapMgr* GetFieldMapMgr();
	static std::string GetFieldMapFile();
	static std::string GetFieldMapFileName();
	static std::string FindPath(std::string);
	static void SetFieldMapFile(std::string);
private:
        static FieldMapMgr* thePointer;        
        static std::string fieldMapFile;
};

#endif

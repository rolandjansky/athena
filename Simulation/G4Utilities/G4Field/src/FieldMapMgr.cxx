/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Field/FieldMapMgr.h"
#include "PathResolver/PathResolver.h"

#include <iostream>

std::string FieldMapMgr::fieldMapFile = "fieldmap.dat";

FieldMapMgr* FieldMapMgr::thePointer = FieldMapMgr::GetFieldMapMgr();

FieldMapMgr* FieldMapMgr::GetFieldMapMgr()
{
	if (!thePointer) thePointer=new FieldMapMgr;
	return thePointer;
}

void FieldMapMgr::SetFieldMapFile(std::string file)
{
	fieldMapFile=file;
}

std::string FieldMapMgr::GetFieldMapFileName()
{
	return fieldMapFile;	
}
std::string FieldMapMgr::GetFieldMapFile()
{
	std::string file=PathResolver::find_file(fieldMapFile,"DATAPATH");
	if (file.empty())
		std::cout<<" file "<<fieldMapFile<<" not found!"<<std::endl;
	return file;	
}
std::string FieldMapMgr::FindPath(std::string name_file) 
{
	std::string file=PathResolver::find_file(name_file,"DATAPATH");
	if (file.empty())
		std::cout<<" file "<<fieldMapFile<<" not found!"<<std::endl;
	return file;	
}

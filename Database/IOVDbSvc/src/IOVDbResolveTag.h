/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IOVDbSvc_IOVDbResolveTag_h
#define IOVDbSvc_IOVDbResolveTag_h
// @file IOVDbResolveTag.h
// Header for tag resolving function, associating global tag to local folder
// @author Shaun Roe
// @date 10 April 2018

#include <string>

	namespace IOVDbNamespace{
	std::string
	resolveCrestTag(const std::string & globalTagName, const std::string & folderName, const std::string & forceTag="");
	
	}
	#endif

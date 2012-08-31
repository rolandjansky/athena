/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HTMLUtil_h__
#define __HTMLUtil_h__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

void CreateMain(const std::string www_dir="");

void CreateTop(const std::string textFile,
	       const std::string www_dir="");

void CreateMenu(std::vector<std::string> e_chain,
		std::vector<std::string> mu_chain,
		std::vector<std::string> j_chain,
		const std::string www_dir="");


void img2html(std::vector<std::string> chain,
	      const std::string width,
	      const std::string height,
	      const std::string file_type,
	      const std::string www_dir="");

#endif// __HTMLUtil_h__

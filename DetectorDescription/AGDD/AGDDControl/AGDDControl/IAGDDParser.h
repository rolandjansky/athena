/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAGDDParser_H
#define IAGDDParser_H


#include <string>

class IAGDDParser {
public:
	IAGDDParser():m_fileName("") {}
	IAGDDParser(std::string s):m_fileName(s) {}
	virtual ~IAGDDParser() {;}
	virtual bool ParseFile(std::string)=0;
	virtual bool ParseFileAndNavigate(std::string)=0;
	virtual bool ParseString(std::string)=0;
	virtual bool ParseStringAndNavigate(std::string)=0;
	virtual void navigateTree()=0;
protected:
	std::string m_fileName;
};

#endif

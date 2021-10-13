/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAGDDParser_H
#define IAGDDParser_H


#include <string>
#include <xercesc/dom/DOM.hpp>

class AGDDController;


class IAGDDParser {
public:
	IAGDDParser():m_fileName("") {}
	IAGDDParser(const std::string& s):m_fileName(s) {}
	virtual ~IAGDDParser() {;}
	virtual bool ParseFile(const std::string&)=0;
	virtual bool ParseFileAndNavigate(AGDDController& c,
                                          const std::string&)=0;
	virtual bool ParseString(const std::string&)=0;
	virtual bool ParseStringAndNavigate(AGDDController& c,
                                            const std::string&)=0;
	virtual bool WriteToFile(const std::string&)=0;
	virtual void navigateTree(AGDDController& c)=0;
        virtual void elementLoop() = 0;
        virtual void elementLoop(AGDDController& c, xercesc::DOMNode*) = 0;
protected:
	std::string m_fileName;
};

#endif

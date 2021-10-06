/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLHandlerStore_H
#define XMLHandlerStore_H

class XMLHandler;
class AGDDController;

#include <map>
#include <string>

#include <xercesc/dom/DOM.hpp>

//using namespace xercesc;

typedef std::map<std::string,XMLHandler*,std::less<std::string> > handlerStore;

class XMLHandlerStore:public handlerStore {
public:
	XMLHandlerStore();
	void RegisterHandler(XMLHandler*);
        XMLHandler* GetHandler(const std::string& name);
	XMLHandler* GetHandler(xercesc::DOMNode *);
	void Handle(AGDDController& c, xercesc::DOMNode *);
};

#endif

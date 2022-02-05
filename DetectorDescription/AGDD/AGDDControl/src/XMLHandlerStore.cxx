/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/XMLHandler.h"

#include <iostream>

using namespace xercesc;

XMLHandlerStore::XMLHandlerStore()
{
}

void XMLHandlerStore::RegisterHandler(XMLHandler* handler)
{
	std::string name=handler->GetName();
	if (this->find(name)!=this->end()) 
		std::cout<<" handler "<<name<<" already defined!"<<std::endl;
	else
		(*this)[name]=handler;
}

void XMLHandlerStore::Handle(AGDDController& c, DOMNode *element)
{
	char* temp=XMLString::transcode(element->getNodeName());
	std::string name=temp;
	XMLString::release(&temp);
	if (this->find(name)!=this->end()) 
		((*this)[name])->Handle(c, element);
	else
		std::cout<<" Handler for "<<name<<" not found! continuing"<<std::endl;
}

XMLHandler* XMLHandlerStore::GetHandler(const std::string& name)
{
  auto it = this->find (name);
  if (it != this->end()) {
    return it->second;
  }
  std::cout<<" Handler for "<<name<<" not found! continuing"<<std::endl;
  return nullptr;
}


XMLHandler* XMLHandlerStore::GetHandler(DOMNode *element)
{
        char* temp=XMLString::transcode(element->getNodeName());
        std::string name=temp;
        XMLString::release(&temp);
        return GetHandler (name);
}


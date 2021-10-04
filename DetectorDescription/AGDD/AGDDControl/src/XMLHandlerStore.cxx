/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/XMLHandler.h"

#include <iostream>

using namespace xercesc;

XMLHandlerStore* XMLHandlerStore::s_theStore=0;

XMLHandlerStore::XMLHandlerStore()
{
}

XMLHandlerStore* XMLHandlerStore::GetHandlerStore()
{
	if (!s_theStore) s_theStore=new XMLHandlerStore;
	return s_theStore;
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

XMLHandler* XMLHandlerStore::GetHandler(DOMNode *element)
{
        char* temp=XMLString::transcode(element->getNodeName());
        std::string name=temp;
        XMLString::release(&temp);
	if (this->find(name)!=this->end()) 
		return (*this)[name];
	else
	{
		std::cout<<" Handler for "<<name<<" not found! continuing"<<std::endl;
		return 0;
	}
}


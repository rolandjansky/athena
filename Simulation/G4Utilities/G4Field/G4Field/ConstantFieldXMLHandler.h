/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ConstantFieldXMLHandler_H
#define ConstantFieldXMLHandler_H

#include "FadsXMLParser/DescriptionFactory.h"
#include <string>

class ConstantField;

class ConstantFieldXMLHandler: public FADS::DescriptionFactory {
public:
        ConstantFieldXMLHandler(std::string s,ConstantField *);
        void BuildDescription();
private:
	ConstantField *m_theField;
};

#endif

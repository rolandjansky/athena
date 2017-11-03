/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Field/ConstantFieldXMLHandler.h"
#include "G4Field/ConstantField.h"

ConstantFieldXMLHandler::ConstantFieldXMLHandler(std::string s,ConstantField *c):
						DescriptionFactory(s),m_theField(c)
{
}

void ConstantFieldXMLHandler::BuildDescription()
{
	if (CheckAttribute("Bx")) 
		m_theField->SetXComponent(GetAttributeAsDoubleAndUnit("Bx"));
	if (CheckAttribute("By")) 
		m_theField->SetYComponent(GetAttributeAsDoubleAndUnit("By"));
	if (CheckAttribute("Bz")) 
		m_theField->SetZComponent(GetAttributeAsDoubleAndUnit("Bz"));
	if (CheckAttribute("Xmin"))
		m_theField->SetXMinField(GetAttributeAsDoubleAndUnit("Xmin"));
	if (CheckAttribute("Xmax"))
		m_theField->SetXMaxField(GetAttributeAsDoubleAndUnit("Xmax"));
	if (CheckAttribute("Ymin"))
		m_theField->SetYMinField(GetAttributeAsDoubleAndUnit("Ymin"));
	if (CheckAttribute("Ymax"))
		m_theField->SetYMaxField(GetAttributeAsDoubleAndUnit("Ymax"));
	if (CheckAttribute("Zmin"))
		m_theField->SetZMinField(GetAttributeAsDoubleAndUnit("Zmin"));
	if (CheckAttribute("Zmax"))
		m_theField->SetZMaxField(GetAttributeAsDoubleAndUnit("Zmax"));
}

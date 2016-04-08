/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4Field/ConstantFieldXMLHandler.h"
#include "G4Field/ConstantField.h"

ConstantFieldXMLHandler::ConstantFieldXMLHandler(std::string s,ConstantField *c):
						DescriptionFactory(s),theField(c)
{
}

void ConstantFieldXMLHandler::BuildDescription()
{
	if (CheckAttribute("Bx")) 
		theField->SetXComponent(GetAttributeAsDoubleAndUnit("Bx"));
	if (CheckAttribute("By")) 
		theField->SetYComponent(GetAttributeAsDoubleAndUnit("By"));
	if (CheckAttribute("Bz")) 
		theField->SetZComponent(GetAttributeAsDoubleAndUnit("Bz"));
	if (CheckAttribute("Xmin"))
		theField->SetXMinField(GetAttributeAsDoubleAndUnit("Xmin"));
	if (CheckAttribute("Xmax"))
		theField->SetXMaxField(GetAttributeAsDoubleAndUnit("Xmax"));
	if (CheckAttribute("Ymin"))
		theField->SetYMinField(GetAttributeAsDoubleAndUnit("Ymin"));
	if (CheckAttribute("Ymax"))
		theField->SetYMaxField(GetAttributeAsDoubleAndUnit("Ymax"));
	if (CheckAttribute("Zmin"))
		theField->SetZMinField(GetAttributeAsDoubleAndUnit("Zmin"));
	if (CheckAttribute("Zmax"))
		theField->SetZMaxField(GetAttributeAsDoubleAndUnit("Zmax"));
}

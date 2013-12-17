/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
#include "FadsRun/FadsRunManagerMessenger.h"
#include "FadsXMLParser/XMLReader.h"

#include "G4UIcommand.hh"

namespace FADS {

FadsRunManagerMessenger::FadsRunManagerMessenger(FadsRunManager *v)
{
	rm=v;

	readXML=new G4UIcommand("/control/ReadXML",this);
	readXML->SetGuidance("Read in an XML file");
	G4UIparameter* parameter;
	G4bool omitable;
	parameter = new G4UIparameter ("FileName", 's', omitable = false);
	readXML->SetParameter(parameter);
}

FadsRunManagerMessenger::~FadsRunManagerMessenger()
{
	delete readXML;
}

void FadsRunManagerMessenger::SetNewValue(G4UIcommand * command,G4String newValues)
{
	if (command==readXML)
	{
		XMLReader *temp=XMLReader::GetXMLReader();
		temp->Process(newValues);
	}
}

G4String FadsRunManagerMessenger::GetCurrentValue(G4UIcommand * command)
{
	G4String s="Undefined";
	return s;
}

}	// end namespace
*/


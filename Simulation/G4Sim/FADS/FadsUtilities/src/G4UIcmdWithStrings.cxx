/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsUtilities/G4UIcmdWithStrings.h"

namespace FADS {

G4UIcmdWithStrings::G4UIcmdWithStrings
(const char * theCommandPath,G4UImessenger * theMessenger,int nparam)
:G4UIcommand(theCommandPath,theMessenger),m_ncommands(nparam)
{
	for (int i=0;i<nparam;i++)
	{
		G4UIparameter *par=new G4UIparameter('s');
		SetParameter(par);
	}
}

std::vector<std::string> G4UIcmdWithStrings::GetNewStringVectorValue(G4String paramString)
{
  	const char* t = paramString;
  	std::istringstream is((char*)t);
  	std::vector<std::string> temp;
  	for (int i=0;i<m_ncommands;i++)
  	{
  		char s[20]; 
  		is>>s;
		temp.push_back(s);
	}
	return temp;
}

G4String G4UIcmdWithStrings::ConvertToString(std::vector<std::string> vec)
{
//  	char st[100];
  	std::ostringstream os(std::ostringstream::out);
  	for (int i=0;i<m_ncommands;i++)
  	{
	  	os << vec[i].c_str();
	}
	os<<'\0';
  	G4String vl = os.str();
  	return vl;
}

void G4UIcmdWithStrings::SetParameterName
(std::vector<std::string> vec,G4bool omittable,G4bool currentAsDefault)
{
	for (int i=0;i<m_ncommands;i++)
	{
		G4UIparameter * thePar= GetParameter(i);
		thePar->SetParameterName(vec[i].c_str());
		thePar->SetOmittable(omittable);
		thePar->SetCurrentAsDefault(currentAsDefault);
	}
}

void G4UIcmdWithStrings::SetDefaultValue(std::vector<std::string> defVal)
{
	for (int i=0;i<m_ncommands;i++)
	{
  		G4UIparameter * thePar = GetParameter(i);
		thePar->SetDefaultValue(defVal[i].c_str());
	}
}

}	// end namespace

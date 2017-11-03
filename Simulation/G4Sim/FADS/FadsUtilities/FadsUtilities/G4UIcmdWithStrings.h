/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UIcmdWithStrings_H
#define G4UIcmdWithStrings_H

#include "G4UIcommand.hh"
#include <string>
#include <vector>

// class description:
//  A concrete class of G4UIcommand. The command defined by this class
// takes three strings.
//  General information of G4UIcommand is given in G4UIcommand.hh.

namespace FADS {

class G4UIcmdWithStrings : public G4UIcommand
{
public: // with description
    	G4UIcmdWithStrings
    	(const char * theCommandPath,G4UImessenger * theMessenger, int np=3);
    	std::vector<std::string> GetNewStringVectorValue(G4String paramString);
    	G4String ConvertToString(std::vector<std::string> vec);
    	void SetParameterName(std::vector<std::string> vec,
      	G4bool omittable,G4bool currentAsDefault=false); 
    	void SetDefaultValue(std::vector<std::string> defVal);
private:
	int m_ncommands;  
};

}	// end namespace

#endif

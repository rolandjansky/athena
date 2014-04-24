/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldMenu_H
#define FieldMenu_H

#include "G4AtlasControl/SimpleMenu.h"
#include <string>

class FieldMenu : public SimpleMenu {
public:
	FieldMenu();
	void list();
	void initialize();
	void fieldmap(const std::string fieldMapfilename);
	void select(const std::string fieldName);
	void assign(const std::string fieldMap, const std::string name, const std::string volName);
	void setDeltaOneStep(double);
	void setDeltaIntersection(double);
	void setMinimumEpsilonStep(double);
	void setMaximumEpsilonStep(double);
	void setDeltaOneStep(std::string,double);
	void setDeltaIntersection(std::string,double);
	void setMinimumEpsilonStep(std::string,double);
	void setMaximumEpsilonStep(std::string,double);
	void ListAvailableSteppers();
	void SetCurrentStepper(const std::string stepperName);
	void UseCurrentStepper();
	void UseStepper(const std::string stepperName);
	void UseStepper(const std::string volName, const std::string stepperName);
	void SetEquationOfMotion(const std::string );

    void SetDefaultStepper(const std::string a) { m_defaultStepper=a; }
    const std::string& GetDefaultStepper() const { return m_defaultStepper; }
  private:
	std::string m_defaultStepper;   
};

#endif

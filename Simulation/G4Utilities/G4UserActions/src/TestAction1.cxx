/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/TestAction1.h"
#include <iostream>

static TestAction1 ts1("TestAction1");

void TestAction1::BeginOfEventAction(const G4Event* /*anEvent*/)
{
	ATH_MSG_INFO("#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction1 - BeginOfEvent       ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################");
}
void TestAction1::EndOfEventAction(const G4Event* /*anEvent*/)
{
	ATH_MSG_INFO("#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction1 - EndOfEvent         ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################");
}
void TestAction1::BeginOfRunAction(const G4Run* /*aRun*/)
{
	ATH_MSG_INFO("#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction1 - BeginOfRun         ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################");
}
void TestAction1::EndOfRunAction(const G4Run* /*aRun*/)
{
	ATH_MSG_INFO("#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction1 - EndOfRun           ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################");
}
void TestAction1::SteppingAction(const G4Step* /*aStep*/)
{
	ATH_MSG_DEBUG("#########################################"<<std::endl<<
			   "##                                     ##"<<std::endl<<
			   "##    TestAction1 - Stepping           ##"<<std::endl<<
			   "##                                     ##"<<std::endl<<
			   "#########################################");
}

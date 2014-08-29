/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/TestAction2.h"
#include <iostream>

static TestAction2 ts2("TestAction2");

void TestAction2::BeginOfEventAction(const G4Event* /*anEvent*/)
{
	log() << MSG::INFO <<"#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction2 - BeginOfEvent       ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################"<<endreq;
}
void TestAction2::EndOfEventAction(const G4Event* /*anEvent*/)
{
	log() << MSG::INFO <<"#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction2 - EndOfEvent         ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################"<<endreq;
}
void TestAction2::BeginOfRunAction(const G4Run* /*aRun*/)
{
	log() << MSG::INFO <<"#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction2 - BeginOfRun         ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################"<<endreq;
}
void TestAction2::EndOfRunAction(const G4Run* /*aRun*/)
{
	log() << MSG::INFO <<"#########################################"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "##    TestAction2 - EndOfRun           ##"<<std::endl<<
		   "##                                     ##"<<std::endl<<
		   "#########################################"<<endreq;
}
void TestAction2::SteppingAction(const G4Step* /*aStep*/)
{
	if (log().level()<=MSG::DEBUG)
		log() << MSG::DEBUG <<"#########################################"<<std::endl<<
			   "##                                     ##"<<std::endl<<
			   "##    TestAction2 - Stepping           ##"<<std::endl<<
			   "##                                     ##"<<std::endl<<
			   "#########################################"<<endreq;
}

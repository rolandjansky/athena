/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PixCalibKnowledgeDb.h"
#include "PixelCoralClientUtils/PixCalibCoralCoolDb.h"

#include "CoolKernel/ValidityKey.h"

#include<iostream>
#include<string>
#include<cstdlib>
#include <sys/time.h>

using namespace std;

int main(int argc, char **argv)
{
  if (argc<2 || argv[1][0]!='-') {
    cout << argv[0] << ": tool to talk to pixel coral database." << endl;
    cout << "options:" << endl;
    cout << "\t-s idTag connTag cfgTag cfgModTag calibtag calibTag1 calibTag2 calibTag3 etc" << endl;
    cout << "\t store a list of calibration tags corresponding to a certain idTag connTag cfgTag cfgModTag in knowledge db"<< endl;
    cout << "\t-r UNIXTimeInSeconds" << endl;
    cout << "\t Find the list of calibration-tags valid for a certain UNIX-time in seconds" << endl;
    return 1;
  }

  string tableName("CALIB_DATA");

  string connString("sqlite_file:pcd_knowledge.db");
  char* ctmp = getenv("PIXEL_CAL_KNOWLEDGE");
  if (ctmp) connString = ctmp;

  int c = argv[1][1];

  switch (c) {
  case 's':
    {
      if(argc > 2 && c==115) {

	//first 5 arguments relate to run-conditions
	long int runNumber = atol(argv[2]);
	string idTag = argv[3];
	string connTag = argv[4];
	string cfgTag = argv[5];
	string cfgModTag = argv[6];

	string calibtags_in_string = "";
	for(int i=7;i<argc;i++){
	  if(i!=7) calibtags_in_string = calibtags_in_string+","+argv[i];
	  else calibtags_in_string = argv[i];
	}
	
	std::cout << "Store a reference that currently the latest calibration-tags are defined by: " << std::endl;
	std::cout << calibtags_in_string << std::endl;
	
	std::cout << "Associated to the following run-conditions: " << std::endl;
	std::cout << "runNumber " << runNumber << std::endl;
	std::cout << "idTag: " << idTag << std::endl;
	std::cout << "connTag: " << connTag << std::endl;
	std::cout << "cfgTag: " << cfgTag << std::endl;
	std::cout << "cfgModTag: " << cfgModTag << std::endl;

	struct timeval UNIXTime;
	gettimeofday(&UNIXTime,NULL);
	std::cout << "UNIXTime at moment of storing tags: " << UNIXTime.tv_sec << std::endl;

	PixCalibKnowledgeDb knowledgeDB(connString, tableName, 1);
	knowledgeDB.init(coral::Update);
	knowledgeDB.saveCorrespondingConfig(UNIXTime.tv_sec,runNumber,calibtags_in_string,idTag,connTag,cfgTag,cfgModTag);
      }
    }
  case 'r':
    {
      if(argc > 2 && argc < 4 && c==114) {

	long int Utime = atol(argv[2]);
       
	std::cout << "Read calibration-Tags valid for UNIXTimeInSeconds " << Utime << std::endl;
	
	PixCalibKnowledgeDb knowledgeDB(connString, tableName, 1);
	knowledgeDB.init(coral::Update);
	knowledgeDB.readCorrespondingCalibTag(Utime);
      } 
    }

    break;
  default:
    cout << "unknown option " << argv[1] << endl;
  }

  return 0;
}

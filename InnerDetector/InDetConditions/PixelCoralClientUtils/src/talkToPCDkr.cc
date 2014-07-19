/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PCDkr.h"
////#include "PixelCoralClientUtils/PixelCalibData.hh"

#include<iostream>
#include<string>
#include<cstdlib>

using namespace std;

int main(int argc, char **argv)
{
  if (argc<2 || argv[1][0]!='-') {
    cout << argv[0] << ": tool to talk to knowledge repository." << endl;
    cout << "options:" << endl;
    cout << "\t-s tr cid" << endl;
    cout << "\t\tcreate a new record with configuration tag/revision tr\n\t\tand calibration identifier cid" << endl;
    cout << "\t-v tr" << endl;
    cout << "\t\tvalidate configuration tag/revision tr" << endl;
    cout << "\t-r" << endl;
    cout << "\t\tdump the table" << endl;
    return 1;
  }

  string tableName("CALIB_KR");

  string connString("");
  char* ctmp = getenv("CAN_CALIB_DB");
  if (ctmp) connString = ctmp;

  int c = argv[1][1];
  switch (c) {
  case 'r':
    {
      cout << "dumping table " << tableName << " at " << connString << endl;

      PCDkr io(connString, tableName, 1);
      io.init(coral::ReadOnly);
      io.load();
    }
    break;

  case 's':
    {
      if (argc<3) {
	cout << "usage: " << argv[0] << " -s tag cid" << endl;
	return 1;
      }
      string tag(argv[2]);
      string cid(argv[3]);
      cout << "saving configuration to " << tableName << " at " << connString << endl;
      cout << "configuration \"" << tag << "\", cid \"" << cid << "\"" << endl;

      PCDkr io(connString, tableName, 1);
      io.init(coral::Update);
      io.save(tag, cid);
    }
    break;

  case 'v':
    {
      if (argc<2) {
	cout << "usage: " << argv[0] << " -v tag" << endl;
	return 1;
      }
      string tag(argv[2]);
      cout << "validating configuration in " << tableName << " at " << connString << endl;
      cout << "configuration \"" << tag << "\"" << endl;

      PCDkr io(connString, tableName, 1);
      io.init(coral::Update);
      io.validate(tag);
    }
    break;

  default:
    cout << "unknown option " << argv[1] << endl;
  }

  return 0;
}

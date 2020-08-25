/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/PCDio.h"
////#include "PixelCoralClientUtils/PixelCalibData.hh"

#include<iostream>
#include<string>
#include<cstdlib>

using namespace std;

int main ATLAS_NOT_THREAD_SAFE (int argc, char **argv) // Thread unsafe PCDio class is used.
{
  if (argc<3 || argv[1][0]!='-') {
    cout << argv[0] << ": tool to talk to pixel coral database." << endl;
    cout << "options:" << endl;
    cout << "\t-r tag [revision]" << endl;
    cout << "\t\tread existing table from the database to pcd.sav" << endl;
    cout << "\t-s tag revision sources" << endl;
    cout << "\t\tsave pcd.dat to the database" << endl;
    return 1;
  }

  string tableName("CALIB_DATA");

  string connString("");
  char* ctmp = getenv("CAN_CALIB_DB");
  if (ctmp) connString = ctmp;

  int c = argv[1][1];
  switch (c) {
  case 'r':
    {
      string tag(argv[2]);
      int revision = 0;
      if (argc>=4) revision = atoi(argv[3]);
      cout << "loading pcd from " << tableName << " at " << connString << endl;
      cout << "tag \"" << tag << "\", revision " << revision << endl;

      PCDio io(connString, tableName, 1);
      io.init(coral::ReadOnly);
      io.load(tag, revision);
    }
    break;

  case 's':
    {
      string tag(argv[2]);
      int revision(atoi(argv[3]));
      string sources(argv[4]);
      cout << "saving pcd to " << tableName << " at " << connString << endl;
      cout << "tag \"" << tag << "\", revision " << revision << endl;

      PCDio io(connString, tableName, 1);
      io.init(coral::Update);
      io.save(tag, revision, sources);
    }
    break;

  default:
    cout << "unknown option " << argv[1] << endl;
  }

  return 0;
}

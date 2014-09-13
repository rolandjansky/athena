/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ccdb-perf.cxx,v 1.7 2008-05-06 22:37:30 beringer Exp $

#include "CoralDB/CoralDB.h"
#include "CoralDB/CompoundTag.h"
#include "CoralDB/Connection.h"

#include "CoralDB/Timer.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

using namespace std;
using namespace CoralDB;


int main(int argc, char* argv[]) {

  if (argc<5) {
    cerr << endl;
    cerr << "Usage: ccdb-perf DBCONNECTION IDTAG TAG N CMD [ARGS]" << endl;
    cerr << endl;
    return 1;
  }

  string dbConnection(argv[1]);
  string idTag(argv[2]);
  string tag(argv[3]);
  long nLoops = atol(argv[4]);
  string cmd(argv[5]);

  cout << endl;
  cout << "Repeating cmd " << cmd << " " << nLoops << " times for database " << dbConnection << " ..." << endl;
  cout << endl;

  Timer tconn("Establish connection",1);
  tconn.start();
  ::CoralDB::CoralDB db(dbConnection, coral::Update, seal::Msg::Error, false /*autoCommit*/);
  tconn.stop();

  db.setCompoundTag(CompoundTag(idTag,tag,tag,tag));

  Timer t0(cmd+" -- first",1);
  Timer tn(cmd,nLoops);

  if (cmd=="findConn") {

    vector<Connection> tmp;
    t0.start();
    tmp = db.findConnections(argv[6],argv[7],true);
    t0.stop();
    //tmp.print(cout);

    tn.start();
    for (int i=0; i<nLoops; i++) 
      tmp = db.findConnections(argv[6],argv[7],true);
    tn.stop();

    tconn.print();
    t0.print();
    tn.print();

  } else

  if (cmd=="writeCLOB") {

    string s;
    for (int i=0; i<nLoops; i++) {
      char c = 'A' + rand() % 26;
      s = s + c;
    }

    Timer tstart("Transaction start for insert");
    db.transactionStart();
    tstart.stop();

    Timer tinsert("Insert string");
    db.insertCLOB("ROOT","testCLOB",s);
    tinsert.stop();

    Timer tcommit("Transaction commit for insert");
    db.transactionCommit();
    tcommit.stop();

    tstart.print();
    tinsert.print();
    tcommit.print();

  } else

  if (cmd=="findCLOB") {

    t0.start();
    string r = db.findCLOB("ROOT","testCLOB");
    t0.stop();
    cout << r << endl;

    tn.start();
    for (int i=0; i<nLoops; i++) 
      r = db.findCLOB("ROOT","testCLOB");
    tn.stop();

    t0.print();
    tn.print();

  } else

  if (cmd=="deleteCLOB") {

    Timer tdelete("Delete CLOB");
    db.transactionStart();
    db.deleteCLOB("ROOT","testCLOB");
    db.transactionCommit();
    tdelete.stop();

    tdelete.print();

  }

  else {
    cerr << "*** FATAL: Unknown command: " << cmd << endl;
    cerr << endl;
    return 1;
  }

  cout << endl;
  return 0;
}

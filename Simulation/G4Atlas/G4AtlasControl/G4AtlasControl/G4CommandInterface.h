/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasControl_G4CommandInterface_H
#define G4AtlasControl_G4CommandInterface_H

///
/// @todo The classes in this header have dangerously common names!
///

#include <string>

/// @todo NEEDS DOCUMENTATION
struct Control{
  Control();
  static void execute(const std::string);
  static void loop(const std::string,const std::string,int,int,int stepSize=1);
  static void foreach(const std::string,const std::string,const std::string);
  static void suppressAbortion(int);
  static void verbose(int);
  static void saveHistory(const std::string);
  static void stopSavingHistory();
  static void alias(const std::string,const std::string);
  static void unalias(const std::string);
  static void listAlias();
  static void shell(const std::string);
  static void manual(const std::string);
  static void createHTML(const std::string);
  static void maximumStoredHistory(int);
};

/// @todo NEEDS DOCUMENTATION
struct Units {
  Units();
  static void list();
};

/// @todo NEEDS DOCUMENTATION
struct Viewer {
  static void flush(const std::string viewer="");
} ;

/// @todo NEEDS DOCUMENTATION
struct Vis {
  Vis();
  static void drawVolume(const std::string volName="");
  static void open(const std::string sysName="",int winSize=600);
  Viewer viewer;
};

/// @todo NEEDS DOCUMENTATION
struct Gtest {
  Gtest();
  static void tolerance(double);
  static void position(double,double,double);
  static void direction(double,double,double);
  static void line_test(bool);
  static void grid_cells(double,double,double);
  static void grid_test(bool);
  static void cylinder_geometry(int,int,int);
  static void cylinder_scaleZ(double);
  static void cylinder_scaleRho(double);
  static void cylinder_test(bool);
  static void recursion_start(int);
  static void recursion_depth(int);
  static void run(bool);
  static void recursive_test();
};

/// @todo NEEDS DOCUMENTATION
struct Gnavigator{
  Gnavigator();
  static void reset();
  static void verbose(int);
  static void check_mode(bool);
};

/// @todo NEEDS DOCUMENTATION
struct Geometry {
  Geometry();
  static Gtest test;
  static Gnavigator navigator;
};

/// @todo NEEDS DOCUMENTATION
struct Tracking {
  Tracking();
  static void abort();
  static void resume();
  static void storeTrajectory(int);
  static void verbose(int);
};

/// @todo NEEDS DOCUMENTATION
struct Estack {
  static void list();
  static void clear(int);
};

/// @todo NEEDS DOCUMENTATION
namespace G4Command {
struct Event {
  Event();
  static void abort();
  static void verbose(int);
  Estack stack;
};
}

/// @todo NEEDS DOCUMENTATION
struct Run {
  Run();
  static void initialize();
  static void beamOn(int,const std::string macro="NULL",int i=-1);
  static void verbose(int);
  static void dumpRegion(const std::string region="ALL");
  static void dumpCouples();
  static void optimizeGeometry(bool fl=true);
  static void breakAtBeginOfEvent(bool fl=true);
  static void breakAtEndOfEvent(bool fl=true);
  static void abort(bool fl=false);
  static void abortCurrentEvent();
  static void geometryModified();
  static void physicsModified();
  static void cutoffModified();
  static void randomNumberStatusDirectory(const std::string fName="./");
  static void storeRandomNumberStatus(bool fl=true);
  static void restoreRandomNumberStatus(const std::string fName="currentRun.rndm");
};

/// @todo NEEDS DOCUMENTATION
struct Random {
  Random();
  static void setDirectoryName(const std::string dir="./");
  static void setSavingFlag(bool fl=true);
  static void saveThisRun();
  static void saveThisEvent();
  static void resetEngineFrom(const std::string fName="currentRun.rndm");
};

/// @todo NEEDS DOCUMENTATION
struct Particle {
  Particle();
};

/// @todo NEEDS DOCUMENTATION
struct Process {
  Process();
  static void list(const std::string type="all");
  static void verbose(int i=1);
  static void setVerbose(int,const std::string type="all");
  static void dump(const std::string,const std::string particle="all");
  static void activate(const std::string, const std::string particle="all");
  static void inactivate(const std::string,const std::string particle="all");
};


/// @todo NEEDS DOCUMENTATION
struct Physics_engine {
  Physics_engine();
};

/// @todo NEEDS DOCUMENTATION
struct G4Commands {
  G4Commands();
  Control control;
  Units units;
  Vis vis;
  Geometry geometry;
  Tracking tracking;
  G4Command::Event event;
  Run run;
  Random random;
  Particle particle;
  Process process;
  Physics_engine physics_engine;
};

#endif

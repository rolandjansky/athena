/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/G4CommandInterface.h"

#include "G4UImanager.hh"
#include "G4VisManager.hh"

#include <iostream>
#include <sstream>

Gtest Geometry::test;
Gnavigator Geometry::navigator;

Control::Control()
{
}
void Control::execute(const std::string fileName)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/execute "+fileName);
}
void Control::loop(const std::string mac,const std::string counter,int init,int fin,int stepSize)
{
  std::ostringstream oss;
  oss<<"/control/loop "<<mac<<" "<<counter<<" "<<init<<" "<<fin
     <<stepSize;
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand(oss.str());
}
void Control::foreach(const std::string macro,const std::string counter,const std::string valueList)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/foreach "+macro+" "+counter+" "+valueList);
}
void Control::suppressAbortion(int i)
{
  std::ostringstream oss;
  oss<<"/control/suppressAbortion "<<i;
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand(oss.str());
}
void Control::verbose(int i)
{
  std::ostringstream oss;
  oss<<"/control/verbose "<<i;
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand(oss.str());
}
void Control::saveHistory(const std::string fileName)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/saveHistory "+fileName);
}
void Control::stopSavingHistory()
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/stopSavingHistory");
}
void Control::alias(const std::string aliasName,const std::string aliasValue)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/alias "+aliasName+" "+aliasValue);
}
void Control::unalias(const std::string aliasName)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/unalias "+aliasName);
}

void Control::listAlias()
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/listAlias");
}
void Control::shell(const std::string command)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/shell "+command);
}
void Control::manual(const std::string dirPath)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/manual "+dirPath);
}
void Control::createHTML(const std::string dirPath)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/control/createHTML "+dirPath);
}
void Control::maximumStoredHistory(int i)
{
  std::ostringstream oss;
  oss<<"/control/maximumStoredHistory "<<i;
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand(oss.str());
}

Units::Units()
{
}

void Units::list()
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/units/list");
}

Vis::Vis()
{
}

void Vis::drawVolume(const std::string volName)
{
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->ApplyCommand("/vis/drawVolume"+volName);
}

void Vis::open(const std::string sysName,int winSize)
{
  std::ostringstream oss;
  oss<<"/vis/open "<<sysName<<" "<<winSize;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}

void Viewer::flush(const std::string viewer)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/vis/viewer/flush "+viewer);
}

Gtest::Gtest()
{
}
void Gtest::tolerance(double d)
{
  std::ostringstream oss;
  oss<<"/geometry/test/tolerance "<<d;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::position(double d1,double d2,double d3)
{
  std::ostringstream oss;
  oss<<"/geometry/test/position "<<d1<<" "<<d2<<" "<<d3;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::direction(double d1,double d2,double d3)
{
  std::ostringstream oss;
  oss<<"/geometry/test/direction "<<d1<<" "<<d2<<" "<<d3;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::line_test(bool t)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  std::string tst="FALSE";
  if (t) tst="TRUE";
  UI->ApplyCommand("/geometry/test/line_test "+tst);
}
void Gtest::grid_cells(double d1,double d2,double d3)
{
  std::ostringstream oss;
  oss<<"/geometry/test/grid_cells "<<d1<<" "<<d2<<" "<<d3;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::grid_test(bool t)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  std::string tst="FALSE";
  if (t) tst="TRUE";
  UI->ApplyCommand("/geometry/test/grid_test "+tst);
}
void Gtest::cylinder_geometry(int i1,int i2,int i3)
{
  std::ostringstream oss;
  oss<<"/geometry/test/cylinder_geometry "<<i1<<" "<<i2<<" "<<i3;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::cylinder_scaleZ(double d1)
{
  std::ostringstream oss;
  oss<<"/geometry/test/cylinder_scaleZ "<<d1;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::cylinder_scaleRho(double d1)
{
  std::ostringstream oss;
  oss<<"/geometry/test/cylinder_scaleRho "<<d1;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::cylinder_test(bool t)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  std::string tst="FALSE";
  if (t) tst="TRUE";
  UI->ApplyCommand("/geometry/test/cylinder_test "+tst);
}
void Gtest::recursion_start(int i1)
{
  std::ostringstream oss;
  oss<<"/geometry/test/recursion_start "<<i1;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::recursion_depth(int i1)
{
  std::ostringstream oss;
  oss<<"/geometry/test/recursion_depth "<<i1;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gtest::run(bool t)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  std::string tst="FALSE";
  if (t) tst="TRUE";
  UI->ApplyCommand("/geometry/test/run "+tst);
}
void Gtest::recursive_test()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/geometry/test/recursive_test ");
}

Gnavigator::Gnavigator()
{
}

void Gnavigator::reset()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/geometry/navigator/reset");
}
void Gnavigator::verbose(int i)
{
  std::ostringstream oss;
  oss<<"/geometry/navigator/verbose "<<i;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Gnavigator::check_mode(bool t)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  std::string tst="FALSE";
  if (t) tst="TRUE";
  UI->ApplyCommand("/geometry/navigator/check_mode "+tst);
}

Geometry::Geometry()
{
}

Tracking::Tracking()
{
}

void Tracking::abort()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/tracking/abort");
}

void Tracking::resume()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/tracking/resume");
}

void Tracking::storeTrajectory(int i)
{
  std::ostringstream oss;
  oss<<"/tracking/storeTrajectory "<<i;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}

void Tracking::verbose(int i)
{
  std::ostringstream oss;
  oss<<"/tracking/verbose "<<i;
  std::cout<<"setting tracking/verbose to "<<i<<std::endl;
  std::cout<<oss.str()<<std::endl;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}

void Estack::list()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/event/stack/list");
}

void Estack::clear(int i)
{
  std::ostringstream oss;
  oss<<"/event/stack/clear "<<i;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}

namespace G4Command {
Event::Event()
{
}

void Event::abort()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/event/abort");
}

void Event::verbose(int i)
{
  std::ostringstream oss;
  oss<<"/event/verbose "<<i;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
}


Run::Run()
{
}

void Run::initialize()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/initialize");
}
void Run::beamOn(int i,const std::string macro,int k)
{
  std::ostringstream oss;
  oss<<"/run/beamOn "<<i<<" "<<macro<<" "<<k;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}

void Run::verbose(int i)
{
  std::ostringstream oss;
  oss<<"/run/verbose "<<i;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}

void Run::dumpRegion(const std::string region)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/dumpRegion "+region);
}
void Run::dumpCouples()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/dumpCouples");
}
void Run::optimizeGeometry(bool fl)
{
  std::string tfl=FALSE;
  if (fl) tfl=TRUE;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/optimizeGeometry "+tfl);
}
void Run::breakAtBeginOfEvent(bool fl)
{
  std::string tfl=FALSE;
  if (fl) tfl=TRUE;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/breakAtBeginOfEvent "+tfl);
}
void Run::breakAtEndOfEvent(bool fl)
{
  std::string tfl=FALSE;
  if (fl) tfl=TRUE;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/breakAtEndOfEvent "+tfl);
}
void Run::abort(bool fl)
{
  std::string tfl=FALSE;
  if (fl) tfl=TRUE;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/abort "+tfl);
}
void Run::abortCurrentEvent()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/abortCurrentEvent");
}
void Run::geometryModified()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/geometryModified");
}
void Run::physicsModified()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/physicsModified");
}
void Run::cutoffModified()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/cutoffModified");
}
void Run::randomNumberStatusDirectory(const std::string fName)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/randomNumberStatusDirectory "+fName);
}
void Run::storeRandomNumberStatus(bool fl)
{
  std::string tfl=FALSE;
  if (fl) tfl=TRUE;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/storeRandomNumberStatus "+tfl);
}
void Run::restoreRandomNumberStatus(const std::string fName)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/restoreRandomNumberStatus "+fName);
}

Random::Random()
{
}

void Random::setDirectoryName(const std::string dir)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/random/setDirectoryName "+dir);
}
void Random::setSavingFlag(bool fl)
{
  std::string tfl=FALSE;
  if (fl) tfl=TRUE;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/random/setSavingFlag "+tfl);
}
void Random::saveThisRun()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/random/saveThisRun");
}
void Random::saveThisEvent()
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/random/saveThisEvent");
}
void Random::resetEngineFrom(const std::string fName)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/random/resetEngineFrom "+fName);
}

Particle::Particle()
{
}

Process::Process()
{
}

void Process::list(const std::string type)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/process/list "+type);
}
void Process::verbose(int i)
{
  std::ostringstream oss;
  oss<<"/process/verbose "<<i;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Process::setVerbose(int i,const std::string type)
{
  std::ostringstream oss;
  oss<<"/process/setVerbose "<<i<<" "<<type;
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand(oss.str());
}
void Process::dump(const std::string procName,const std::string particle)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/process/dump "+procName+" "+particle);
}
void Process::activate(const std::string procName, const std::string particle)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/process/activate "+procName+" "+particle);
}
void Process::inactivate(const std::string procName,const std::string particle)
{
  G4UImanager *UI=G4UImanager::GetUIpointer();
  UI->ApplyCommand("/process/inactivate "+procName+" "+particle);
}
Physics_engine::Physics_engine()
{
}

G4Commands::G4Commands()
{
  std::cout<<" creating the python interface to G4 "<<std::endl;
}

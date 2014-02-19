/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/setupBeamline.h"
#include "FPTracker/Beamline.h"
#include "FPTracker/Particle.h"
#include "FPTracker/getAlfaMagnetConfigFiles.h"
#include "FPTracker/ConfigData.h"

#include "boost/program_options/cmdline.hpp"
#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

int main(int argc, char** argv) {

  namespace po = boost::program_options;
  
  using namespace FPTracker;

  po::options_description desc("Options");
  desc.add_options()
    ("help", "write help msg")
    ("c",  po::value<std::string>(), "configuration file directory (default /afs/cern.ch/user/a/asbrizzi/links/twiss/v001/0090.00m/8TeV")
    ("be", po::value<double>(),      "pbeam0[GeV] (default 4000)")
    ("em", po::value<double>(),      "endMarker[m] (236.888)")
    ("x",  po::value<double>(),      "particle x position[m] (0)")
    ("y",  po::value<double>(),      "particle y position[m] (0)")
    ("z",  po::value<double>(),      "particle z position[m] (0)")
    ("px", po::value<double>(),      "particle x momentum[GeV] (0)")
    ("py", po::value<double>(),      "particle y momentum[GeV] (0)")
    ("pz", po::value<double>(),      "particle z momentum[GeV] (4000)")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  ConfigData configData;

  if ( vm.count("help") ) { std::cout<<desc<<'\n'; return 1; }
  
  std::string confDir   = vm.count("c")  ? vm["c"].as<std::string>() : "/afs/cern.ch/user/a/asbrizzi/links/twiss/v001/0090.00m/8TeV";
  configData.IP         = 1;
  configData.useaper    = 1;
  configData.pbeam0     = vm.count("be") ? vm["be"].as<double>() : 4000;
  configData.brho       = configData.pbeam0/ConfigData::speedOfLight;
  configData.endMarker  = vm.count("em") ? vm["em"].as<double>() : 236.888;
  configData.absZMagMax = configData.endMarker;

  double particleX  = (vm.count("x"))  ? vm["x"].as<double>()  : 0;
  double particleY  = (vm.count("y"))  ? vm["y"].as<double>()  : 0;
  double particleZ  = (vm.count("z"))  ? vm["z"].as<double>()  : 0;
  double particlePX = (vm.count("px")) ? vm["px"].as<double>() : 0;
  double particlePY = (vm.count("py")) ? vm["py"].as<double>() : 0;
  double particlePZ = (vm.count("pz")) ? vm["pz"].as<double>() : 4000;

  std::cout << "confDir = "   << confDir << '\n';
  std::cout << "configData\n" << configData;
  std::cout << "particleX[m]   : " << particleX  << std::endl;
  std::cout << "particleY[m]   : " << particleY  << std::endl;
  std::cout << "particleZ[m]   : " << particleZ  << std::endl;
  std::cout << "particlePX[GeV]: " << particlePX << std::endl;
  std::cout << "particlePY[GeV]: " << particlePY << std::endl;
  std::cout << "particlePZ[GeV]: " << particlePZ << std::endl;

  int magver = 3;
  
  boost::shared_ptr< std::ifstream > magfile1 = getAlfaMagnetConfigFiles(confDir, beam1);
  boost::shared_ptr< std::ifstream > magfile2 = getAlfaMagnetConfigFiles(confDir, beam2);
  
  Beamline fBeamline1 = setupBeamline(configData, beam1, magver, magfile1); std::cout << fBeamline1;
  Beamline fBeamline2 = setupBeamline(configData, beam2, magver, magfile2); std::cout << fBeamline2;
  
  boost::shared_ptr<IParticle> iParticle(new Particle(particleX, particleY, particleZ, particlePX, particlePY, particlePZ));

  IParticle& fParticle = *iParticle;

  std::cout << "fpTracker: " << fParticle << std::endl;

  (fParticle.direction().z()>0.) ? fBeamline1.track(fParticle) : fBeamline2.track(fParticle);

  std::cout << "fpTracker: " << fParticle << std::endl;

  return 0;
}

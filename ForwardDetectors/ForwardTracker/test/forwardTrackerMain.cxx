/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/BeamlineSetup.h"
#include "ForwardTracker/Beamline.h"
#include "ForwardTracker/ConfigData.h"
#include "ForwardTracker/ForwardTrackerConstants.h"
#include "ForwardTracker/Particle.h"
#include "ForwardTracker/TwissFile.h"

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
  
  using namespace ForwardTracker;

  po::options_description desc("Options");
  desc.add_options()
    ("help", "write help msg (default values)")
    ("f1", po::value<std::string>(), "twissfile1 (/afs/cern.ch/atlas/offline/external/TwissFiles/v001/0090.00m/8TeV/alfaTwiss1.txt)")
    ("f2", po::value<std::string>(), "twissfile2 (/afs/cern.ch/atlas/offline/external/TwissFiles/v001/0090.00m/8TeV/alfaTwiss2.txt)")
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

  ConfigData cData;

  if (vm.count("help")) { std::cout << desc << std::endl; return 1; }
  if (vm.count("f1"))   { cData.twissFile1 = vm["f1"].as<std::string>(); }
  if (vm.count("f2"))   { cData.twissFile2 = vm["f2"].as<std::string>(); }
  if (vm.count("em"))   { cData.endMarker  = vm["em"].as<double>(); } 

  double particleX  = (vm.count("x"))  ? vm["x"].as<double>()  : 0;
  double particleY  = (vm.count("y"))  ? vm["y"].as<double>()  : 0;
  double particleZ  = (vm.count("z"))  ? vm["z"].as<double>()  : 0;
  double particlePX = (vm.count("px")) ? vm["px"].as<double>() : 0;
  double particlePY = (vm.count("py")) ? vm["py"].as<double>() : 0;
  double particlePZ = (vm.count("pz")) ? vm["pz"].as<double>() : 4000;
      
  std::cout << "ConfigData "       << cData;
  std::cout << "particleX[m]   : " << particleX  << std::endl;
  std::cout << "particleY[m]   : " << particleY  << std::endl;
  std::cout << "particleZ[m]   : " << particleZ  << std::endl;
  std::cout << "particlePX[GeV]: " << particlePX << std::endl;
  std::cout << "particlePY[GeV]: " << particlePY << std::endl;
  std::cout << "particlePZ[GeV]: " << particlePZ << std::endl;

  Beamline fBeamline1 = BeamlineSetup(cData, beam1); std::cout << fBeamline1;
  Beamline fBeamline2 = BeamlineSetup(cData, beam2); std::cout << fBeamline2;
    
  boost::shared_ptr<IParticle> iParticle(new Particle(particleX, particleY, particleZ, particlePX, particlePY, particlePZ, 1));

  IParticle& fParticle = *iParticle;
  
  fParticle.setDebug(1);

  (fParticle.direction().z()>0.) ? fBeamline1.track(fParticle) : fBeamline2.track(fParticle);

  return 0;
}

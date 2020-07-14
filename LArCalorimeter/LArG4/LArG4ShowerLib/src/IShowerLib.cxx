/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// header file
#include "LArG4ShowerLib/IShowerLib.h"
#include "TTree.h"
#include "G4Track.hh"
#include "AtlasHepMC/GenParticle.h"

#include <sstream>

namespace ShowerLib {

  bool IShowerLib::readMeta(TTree* source) {
	  char detector[40];
	  char release[40];
	  char geometry[40];
	  char geant[40];
	  char physics[40];
	  char comment[100];
	  source->SetBranchAddress("detector",&detector);
	  source->SetBranchAddress("particle",&m_particle);
	  source->SetBranchAddress("release",&release);
	  source->SetBranchAddress("geometry",&geometry);
	  source->SetBranchAddress("geantVersion",&geant);
	  source->SetBranchAddress("physicsList",&physics);
	  source->SetBranchAddress("comment",&comment);
	  source->GetEntry();
	  m_detector = detector;
	  m_release = release;
	  m_geometry = geometry;
	  m_geantVersion = geant;
	  m_physicsList = physics;
	  m_comment = comment;
	  return true;
  }

  bool IShowerLib::writeMeta(TTree* source) const {
	  char detector[40];
	  char release[40];
	  char geometry[40];
	  char geant[40];
	  char physics[40];
	  char comment[100];
	  int  particle(m_particle);
	  m_detector.copy(detector,39,0);
	  detector[m_detector.length()] = '\0';
	  m_release.copy(release,39,0);
	  release[m_release.length()] = '\0';
	  m_geometry.copy(geometry,39,0);
	  geometry[m_geometry.length()] = '\0';
	  m_geantVersion.copy(geant,39,0);
	  geant[m_geantVersion.length()] = '\0';
	  m_physicsList.copy(physics,39,0);
	  physics[m_physicsList.length()] = '\0';
	  m_comment.copy(comment,99,0);
	  comment[m_comment.length()] = '\0';
	  source->Branch("detector",&detector,"detector/C");
	  source->Branch("particle",&particle,"particle/I");
	  source->Branch("release",&release,"release/C");
	  source->Branch("geometry",&geometry,"geometry/C");
	  source->Branch("geantVersion",&geant,"geantVersion/C");
	  source->Branch("physicsList",&physics,"physicsList/C");
	  source->Branch("comment",&comment,"comment/C");
	  source->Fill();
	  return true;
  }

  const std::string IShowerLib::statistics() const {

      if (m_filled) {
      std::stringstream out;

      out << "======================================================" << std::endl;
      out << "             Statistics of the shower library         " << std::endl;
      out << "======================================================" << std::endl;
      out << "Library Type: "<< getName() <<" PARTICLE: " << m_particle << " DETECTOR: "<< m_detector << std::endl;
	  out << m_release << " " << m_geometry << " " << m_geantVersion << " " << m_physicsList << std::endl;
	  if (printParameters().length() > 0)
	      out << printParameters() << std::endl;
	  out << m_comment << std::endl;
	  return out.str();
      } else { //statistics for the genlib, shorter version
      std::stringstream out;
      out << getName() <<" PARTICLE: " << m_particle << " DETECTOR: "<< m_detector << ", ";
      out << m_release << " " << m_geometry << " " << m_geantVersion << " " << m_physicsList << std::endl;
      if (printParameters().length() > 0)
	      out << printParameters() << std::endl;
      out << m_comment << std::endl;
      return out.str();
      }
  }

} // namespace IShowerLib

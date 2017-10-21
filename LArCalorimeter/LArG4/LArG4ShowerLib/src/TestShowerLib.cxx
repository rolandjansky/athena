/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// this header file
#include "LArG4ShowerLib/TestShowerLib.h"

// CLHEP incldues
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>

//#include <algorithm>
//#include <functional>
#include <sstream>
#include <fstream>

#include <iostream>

// local includes
//#include "LArG4ShowerLib/PositionBin.h"

// G4 includes
#include "G4Track.hh"

#include "LArG4Code/EnergySpot.h"
#include "LArG4ShowerLib/ShowerEnergySpot.h"

#include "TTree.h"
#include "TFile.h"
#include "TParameter.h"

#define LIB_VERSION 10

namespace ShowerLib {

  TestShowerLib::~TestShowerLib()
  {
	  library::const_iterator libit;
	  for (libit = m_libData.begin(); libit != m_libData.end(); libit ++) {
		  delete (*libit).first.vertex;
		  delete (*libit).first.momentum;
	  }
  }

  IShowerLib* TestShowerLib::readFromROOTFile(TFile* source)
  {
      TParameter<int>* ver;
      ver = (TParameter<int>*)source->Get("version");

      if ((ver == NULL) || (ver->GetVal() != LIB_VERSION)) return NULL; //Test library header = 10

      TTree* TTreeMeta = (TTree*)source->Get("meta");
      TTree* TTreeLib  = (TTree*)source->Get("library");

      if ((TTreeMeta == NULL) || (TTreeLib == NULL)) return NULL;

      std::cout << "TestShowerLib header found." << std::endl;

	  TestShowerLib* newlib = new TestShowerLib();

	  if (!(newlib->readMeta(TTreeMeta)) || !(newlib->read(TTreeLib))) {
		  delete newlib;
	      std::cout << "TestShowerLib read unsuccessful." << std::endl;
		  return NULL;
	  }

	  return newlib;

  }

  IShowerLib* TestShowerLib::createEmptyLib(const std::string& inputFile)
  {
	  /*
	   * Test library Structure format:
	   *
	   * VER PART DET
	   * COMMENT
	   *
	   * where
	   *
	   * VER == 10
	   */
	  std::ifstream filestr(inputFile.c_str(),std::ios::in);

	  if (!filestr.is_open()) {
		  std::cout << "TestShowerLib         " << inputFile << ": bad file!" << std::endl;
		  return NULL;
	  }

	  std::string instr;
	  std::getline(filestr,instr);
	  std::stringstream ss(instr);

	  int ver;

	  ss >> ver;

	  if (ver != LIB_VERSION) {
		  return NULL;
	  }


	  int part;
	  std::string det;

	  ss >> part >> det;


	  TestShowerLib* newlib = new TestShowerLib();

	  newlib->m_detector = det;
	  newlib->m_particle = part;
	  newlib->m_filled = false;

	  std::getline(filestr,instr);
	  newlib->m_comment = instr;

	  return newlib;
  }


  std::vector<EnergySpot>* TestShowerLib::getShower(const G4Track* , ShowerLibStatistics* , int ) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return NULL;
	  }

	  std::cout << "Library is only for testing, not for production use" << std::endl;
	  return NULL;
  }

  double TestShowerLib::getContainmentZ(const G4Track* ) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return 0.0;
	  }

	  std::cout << "Library is only for testing, not for production use" << std::endl;
	  return 0.0;
  }

  double TestShowerLib::getContainmentR(const G4Track* ) const
  {
	  if (!m_filled) {
		  std::cout << "Library is not created for production use" << std::endl;
		  return 0.0;
	  }

	  std::cout << "Library is only for testing, not for production use" << std::endl;
	  return 0.0;
  }

bool TestShowerLib::storeShower(const HepMC::GenParticle* genParticle, const Shower* shower)
  {
	  if (m_filled) {
		  std::cout << "ERROR: filled" << std::endl;
		  return false;
	  }

	  genInfo theinfo;
	  theinfo.vertex = new HepMC::FourVector(genParticle->production_vertex()->position());
	  theinfo.momentum = new HepMC::FourVector(genParticle->momentum());

	  storedShower storedshower;
	  storedshower.first = theinfo;
	  storedshower.second = *shower;

	  m_libData.push_back(storedshower);

	  return true;
  }

  bool TestShowerLib::writeToROOT(TFile* dest)
  {
	  if (m_libData.empty()) return false;
	  TParameter<int> ver("version",LIB_VERSION);

      dest->WriteObject(&ver,"version");

      TTree TTreeMeta;
      TTree TTreeLib;

      write(&TTreeLib);
      writeMeta(&TTreeMeta);

      dest->WriteObject(&TTreeLib,"library");
      dest->WriteObject(&TTreeMeta,"meta");

	  return true;
  }


  bool TestShowerLib::read(TTree* source)
  {
	  /*
	   * Eta Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |    vertex    |    vertex    |    vertex    |  num of  |  cont  |  - shower header
	   * |      X       |      Y       |      Z       |  hits    |    Z   |
	   * ------------------------------------------------------------------
	   * |   momentum   |   momentum   |   momentum   |  truth   |  cont  |  - shower header
	   * |      X       |      Y       |      Z       |  energy  |    R   |
	   * ------------------------------------------------------------------
	   * |x-coord of hit|y-coord of hit|z-coord of hit|dep.energy|hit time|  - hit
	   */
	  int nentr = source->GetEntriesFast();
	  if (nentr < 3) return false;
	  Float_t x,y,z,e,time;
	  source->SetBranchAddress("x",&x);
	  source->SetBranchAddress("y",&y);
	  source->SetBranchAddress("z",&z);
	  source->SetBranchAddress("e",&e);
	  source->SetBranchAddress("time",&time);
	  int entr = 0;

	  do {
		  //read eta bin header
		  source->GetEntry(entr++);
		  int nhits = (int)(e+0.1); // +0.1 just in case - c++ has low round
		  Shower shower;
		  shower.setZSize(time);
		  genInfo theinfo;
		  theinfo.vertex = new HepMC::FourVector(x,y,z,0);
		  source->GetEntry(entr++);
		  shower.setRSize(time);
		  theinfo.momentum = new HepMC::FourVector(x,y,z,e);
		  for(int i = 0; i < nhits; i++) {
			  source->GetEntry(entr++); //variables mean what the name suggests
			  shower.push_back(new ShowerEnergySpot(G4ThreeVector(x,y,z),e,time));
		  }
		  m_libData.push_back(storedShower(theinfo,shower));
	  } while (entr < nentr);

	  m_filled = true;
	  return true;
  }

  bool TestShowerLib::write(TTree* dest) const
  {
	  /*
	   * Eta Energy library format:
	   * |       x      |       y      |       z      |    e     |  time  |  - name of branch in TTree
	   * ------------------------------------------------------------------
	   * |    vertex    |    vertex    |    vertex    |  num of  |  cont  |  - shower header
	   * |      X       |      Y       |      Z       |   hits   |    Z   |
	   * ------------------------------------------------------------------
	   * |   momentum   |   momentum   |   momentum   |  truth   |  cont  |  - shower header
	   * |      X       |      Y       |      Z       |  energy  |    R   |
	   * ------------------------------------------------------------------
	   * |x-coord of hit|y-coord of hit|z-coord of hit|dep.energy|hit time|  - hit
	   */
	  Float_t x,y,z,e,time;
	  dest->Branch("x",&x);
	  dest->Branch("y",&y);
	  dest->Branch("z",&z);
	  dest->Branch("e",&e);
	  dest->Branch("time",&time);
	  library::const_iterator libit;
	  for (libit = m_libData.begin(); libit != m_libData.end(); libit ++) {
		  HepMC::FourVector vertex = *((*libit).first.vertex);
		  HepMC::FourVector momentum = *((*libit).first.momentum);
		  x = vertex.x();
		  y = vertex.y();
		  z = vertex.z();
		  e = (*libit).second.size();
		  time = (*libit).second.getZSize();
		  dest->Fill(); //eta bin header
		  x = momentum.px();
		  y = momentum.py();
		  z = momentum.pz();
		  e = momentum.e();
		  time = (*libit).second.getRSize();
		  dest->Fill(); //eta bin header
		  Shower::const_iterator iter;
		  for (iter = (*libit).second.begin(); iter != (*libit).second.end(); iter++) {
			  x = (*iter)->GetPosition().x();
			  y = (*iter)->GetPosition().y();
			  z = (*iter)->GetPosition().z();
			  e = (*iter)->GetEnergy();
			  time = (*iter)->GetTime();
			  dest->Fill();
		  }
	  }
	  //dest->Write();
	  return true;
  }

  ShowerLibStatistics* TestShowerLib::createStatistics() const
  {
	  return NULL;
  }

} // namespace ShowerLib

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "CustomMonopoleFactory.h"
// package headers
#include "CustomMonopole.h"
// CLHEP headers
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
// STL headers
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <map>
#include <set>

bool CustomMonopoleFactory::loaded = false;
//std::set<G4ParticleDefinition *> CustomMonopoleFactory::m_particles;
std::set<CustomMonopole *> CustomMonopoleFactory::m_particles;

//bool CustomMonopoleFactory::isCustomMonopole(G4ParticleDefinition *particle)
bool CustomMonopoleFactory::isCustomMonopole(CustomMonopole *particle)
{
  return (particle && m_particles.find(particle)!=m_particles.end());
}

// CustomMonopole* CustomMonopoleFactory::getCustomMonopole(G4ParticleDefinition *particle)
// {
//   return (m_particles.find(particle)!=m_particles.end());
// }

void CustomMonopoleFactory::loadCustomMonopoles()
{
  if(loaded) return;
  loaded = true;
  std::ifstream configFile("particles.txt");
  //  std::ifstream configFile("stophadrons.txt");
  G4String pType="custom";
  G4String pSubType="";
  //  bool first = true;
  int pdgCode;
  double mass;
  double elCharge;
  double magCharge;
  std::string name,line;
  // This should be compatible IMO to SLHA
  while(getline(configFile,line))
    {
      std::string::size_type beg_idx,end_idx;
      bool isQball = false;
      bool isFCP = false; // Wendy Taylor: fractionally charged particle

      beg_idx = line.find_first_not_of("\t #");
      if(beg_idx > 0 && line[beg_idx-1] == '#') continue;
      end_idx = line.find_first_of( "\t ", beg_idx);
      if (end_idx == std::string::npos) continue;
      char *endptr;
      pdgCode  = strtol(line.substr( beg_idx, end_idx - beg_idx ).c_str(), &endptr, 0);
      if (endptr[0] != '\0') {
        throw std::invalid_argument("CustomMonopoleFactory::loadCustomMonopoles: Could not convert string to int: " + line.substr( beg_idx, end_idx - beg_idx ));
      }

      G4cout << "CustomMonopoleFactory: pdgCode = " << pdgCode << G4endl;

      beg_idx = line.find_first_not_of("\t ",end_idx);
      end_idx = line.find_first_of( "\t #", beg_idx);
      if (end_idx == std::string::npos) continue;

      mass  = atof(line.substr( beg_idx, end_idx - beg_idx ).c_str());

      beg_idx = line.find_first_not_of("\t ",end_idx);
      end_idx = line.find_first_of( "\t #", beg_idx);
      if (end_idx == std::string::npos) continue;

      elCharge  = atof(line.substr( beg_idx, end_idx - beg_idx ).c_str());

      beg_idx = line.find_first_not_of("\t ",end_idx);
      end_idx = line.find_first_of( "\t #", beg_idx);

      if (end_idx == std::string::npos) continue;
      magCharge  = atof(line.substr( beg_idx, end_idx - beg_idx ).c_str());

      beg_idx = line.find_first_not_of("\t# ",end_idx);
      end_idx = line.length();
      name = line.substr( beg_idx, end_idx - beg_idx );
      while(name.c_str()[0] == ' ') name.erase(0,1);
      while(name[name.size()-1] == ' ') name.erase(name.size()-1,1);
      std::string lowerCaseName(name);
      for(unsigned int il=0; il < lowerCaseName.length(); ++il) lowerCaseName[il] = std::tolower(lowerCaseName[il]);
      isQball = (lowerCaseName.find("qball") != std::string::npos) ? true : false;

      isFCP = (lowerCaseName.find("fcp") != std::string::npos) ? true : false;

      G4cout << "CustomMonopoleFactory: name = " << name << G4endl;

      if(abs(pdgCode) / 1000000 == 0)
        {
          G4cout << "CustomMonopoleFactory: pdgCode too low: " << pdgCode << " " << abs(pdgCode) / 1000000  << G4endl;
          continue;
        }
      else
        {

          double elChargeFromPDGcode = (isQball) ? (pdgCode/10)%10000/10. : (pdgCode/10)%1000/1. ;

          if (isFCP) 
	    {
	      // pdgCode charge encoding scheme developed by quanyin.li@cern.ch
	      float XX, YY; // XX = numerator, YY = denominator
	      XX = (abs(pdgCode)/1000)%100;
	      YY = (abs(pdgCode)/10)%100;
	      G4cout << "CustomMonopoleFactory: XX = " << XX << ", YY = " << YY << G4endl;

	      elChargeFromPDGcode = (pdgCode>0) ? round(100.*XX/YY)/100. : -round(100.*XX/YY)/100.;
	      G4cout << "CustomMonopoleFactory: elChargeFromPDGcode = " << elChargeFromPDGcode << G4endl;
	    }

          if (!isQball && !isFCP && abs((int)(pdgCode/10000)) == 412) elChargeFromPDGcode = -elChargeFromPDGcode;


          if (elChargeFromPDGcode != elCharge) {
            G4cout << "CustomMonopoleFactory: El. charges for "<< name <<" from PDGcode and 3d col. of particles.txt file do not agree: " <<  elChargeFromPDGcode << " / " << elCharge << G4endl;
            G4Exception("CustomMonopoleFactory::loadCustomMonopoles","WrongElCharges",FatalException,"El charge from PDGcode and 3d col. of particles.txt file do not agree");
          }
          if (elCharge == 0.0 && magCharge == 0.0) {
            G4cout << "CustomMonopoleFactory: Both electric and magnetic charges are ZEROs. Skip the particle. " << G4endl;
            continue;
          }

        }

      G4cout << "CustomMonopoleFactory: pType is " << pType << G4endl;
      G4cout << "CustomMonopoleFactory: PDGcode of "  << name << " is " << pdgCode << G4endl;
      G4cout << "CustomMonopoleFactory: Mass of "     << name << " is " << mass   << G4endl;
      G4cout << "CustomMonopoleFactory: Electrical Charge of " << name << " is "<< elCharge << G4endl;
      G4cout << "CustomMonopoleFactory: Magnetic Charge of " << name << " is "<< magCharge << G4endl;

      //      CustomMonopole *particle  = CustomMonopole::MonopoleDefinition(
      CustomMonopole *particle  = new CustomMonopole(
                                                     name,  mass * CLHEP::GeV ,  0.0*CLHEP::MeV,      CLHEP::eplus*elCharge,
                                                     0,              +1,        0,
                                                     0,               0,        0,
                                                     pType,           0,       +1,      pdgCode,
                                                     true,         -1.0,        NULL);
      //             magCharge);

      particle->SetMagneticCharge(magCharge);
      particle->PrintMonopoleInfo();
      m_particles.insert(particle);
    }

  configFile.close();

  if (m_particles.empty())  {
    G4cout << "CustomMonopoleFactory:  No particles have been loaded." << G4endl;
    G4Exception("CustomMonopoleFactory::loadCustomMonopoles","NoParticlesLoaded",FatalException,"No particles have been loaded");
  }
  return;
}

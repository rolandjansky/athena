/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlantisDumper_H
#define G4AtlantisDumper_H

#include "FadsActions/UserAction.h"
#include <fstream>
#include <string>

class G4AtlantisDumper: public FADS::UserAction {

 public:
  //G4AtlantisDumper(std::string s): FADS::UserAction(s),_file(NULL){};
  
G4AtlantisDumper(const std::string s); 
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);
    
 private:
    int m_nsteps;
    std::ofstream *m_file;

    double m_tedep_cut,m_length_cut,m_p_cut, m_ke_cut;
    std::string m_filename;
};
#endif


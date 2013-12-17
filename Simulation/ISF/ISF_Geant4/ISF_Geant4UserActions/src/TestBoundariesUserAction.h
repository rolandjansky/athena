/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_Geant4Tools_TestBoundariesUserAction_h
#define ISF_Geant4Tools_TestBoundariesUserAction_h


#include "FadsActions/UserAction.h"
#include <string>
#include <map>

class TTree;
class TFile;
#include "TMath.h"

namespace iGeant4 {

  class TestBoundariesUserAction: public FADS::UserAction {
  public:
    TestBoundariesUserAction(std::string s);
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);
  private:
    typedef std::map<std::string,int> SMap;
    SMap sel;
    std::map<std::string,std::string> mother;

    TFile * file;
    TTree * tree;
    struct Data {
      Float_t x,y,z;
      Int_t volume;
      Bool_t enter;
      Bool_t exit;
      Bool_t leave;
    Data() : x(0.),y(0.),z(0.), volume(0),enter(false),exit(false),leave(false) {}
      void Reset() {
	enter=exit=leave=false;
      }
      void Set(Float_t _x, Float_t _y, Float_t _z,
	       Int_t _vol) {
	x=_x; y=_y; z=_z;
	volume=_vol;
      }	
    } data;
  };

}


#endif //  ISF_Geant4Tools_SteppingUserAction_h

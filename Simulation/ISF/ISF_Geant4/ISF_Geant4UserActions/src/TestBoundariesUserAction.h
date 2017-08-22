/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_Geant4Tools_TestBoundariesUserAction_h
#define ISF_Geant4Tools_TestBoundariesUserAction_h


#include <string>
#include <map>

class TTree;
class TFile;
#include "TMath.h"


#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"


namespace G4UA{

  namespace iGeant4{
    
    class TestBoundariesUserAction:
    public AthMessaging, public G4UserRunAction, public G4UserSteppingAction
    {
      
    public:
      TestBoundariesUserAction();
      virtual void BeginOfRunAction(const G4Run*) override;
      virtual void EndOfRunAction(const G4Run*) override;
      virtual void UserSteppingAction(const G4Step*) override;
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
      
    }; // class TestBoundariesUserAction
    
    
  } // iGeant4
  
} // namespace G4UA 
  
  
#endif //  ISF_Geant4Tools_SteppingUserAction_h

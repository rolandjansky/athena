/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_FluxRecorder_H
#define G4UserActions_FluxRecorder_H



#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"

#include <vector>

// Forward declarations
class TH1D;

namespace G4UA
{

  class FluxRecorder : public G4UserRunAction,
                       public G4UserEventAction,
                       public G4UserSteppingAction
  {

  public:
    FluxRecorder();
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;
  private:
    enum scoringVolume {RPCOlz,RPCOmz,RPCOhz,RPCMlz,RPCMmz,RPCMhz,MDTIlz,MDTImz,MDTIhz,
                        LMDTo,LMDTm,LMDTi,BMDTo,BMDTm,BMDTi,SWo,SWm,SWi,SWc,SWt,
                        bflz,bfhz,bslz,bshz,btlz,bthz,ffle,ffme,ffhe,fsle,fsme,fshe,ftle,ftme,fthe,
                        CY1,CY2,lastVol};
    TH1D * m_flux[lastVol][9][2];
    TH1D * m_fluxE[lastVol][9];

    double m_nev;
    std::vector<int> m_list;
    void findVolume( const double, const double, const double, const double );

  }; // class FluxRecorder


} // namespace G4UA

#endif

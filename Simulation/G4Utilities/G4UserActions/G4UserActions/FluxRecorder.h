/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_FluxRecorder_H
#define G4UserActions_FluxRecorder_H



#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"

#include <vector>

// Forward declarations
class TH1D;

namespace G4UA
{

  class FluxRecorder : public IBeginRunAction, public IEndRunAction,
                       public IEndEventAction, public ISteppingAction
  {

  public:
    FluxRecorder();
    virtual void beginOfRun(const G4Run*) override;
    virtual void endOfRun(const G4Run*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void processStep(const G4Step*) override;
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

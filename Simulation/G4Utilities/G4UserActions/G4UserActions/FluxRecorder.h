/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FluxRecorder_H
#define FluxRecorder_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>
#include <vector>

class TH1D;

class FluxRecorder: public FADS::ActionsBase , public FADS::UserAction {

  public:
   FluxRecorder(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),m_nev(0.) {}
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);

  private:
   enum scoringVolume {RPCOlz,RPCOmz,RPCOhz,RPCMlz,RPCMmz,RPCMhz,MDTIlz,MDTImz,MDTIhz,
                       LMDTo,LMDTm,LMDTi,BMDTo,BMDTm,BMDTi,SWo,SWm,SWi,SWc,SWt,
                       bflz,bfhz,bslz,bshz,btlz,bthz,ffle,ffme,ffhe,fsle,fsme,fshe,ftle,ftme,fthe,
                       CY1,CY2,lastVol};
   TH1D * m_flux[lastVol][9][2];
   TH1D * m_fluxE[lastVol][9];
   double m_nev;
   std::vector<int> m_list;
   void findVolume( const double , const double , const double , const double ) ;

};

#endif

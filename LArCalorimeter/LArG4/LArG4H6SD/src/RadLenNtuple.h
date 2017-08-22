/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_RadLenNtuple_H
#define LARG4H6SD_RadLenNtuple_H

#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"

#include <string>
#include "GaudiKernel/NTuple.h"
#include <vector>

// Event information
#include "StoreGate/ReadHandle.h"
#include "GeneratorObjects/McEventCollection.h"


namespace G4UA
{
  /// @brief NEEDS DOCUMENTATION
  class RadLenNtuple final: public G4UserSteppingAction, public G4UserEventAction {

  public:

    struct Config
    {
      std::string mcEventCollName="GEN_EVENT";
      int verboseLevel=3;
    };

    RadLenNtuple(const Config& config): m_mcEvtColl(config.mcEventCollName), m_verboseLevel(config.verboseLevel) {}
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;

    virtual StatusCode initialize();


  private:

    NTuple::Item<float>   m_tot_x;
    NTuple::Item<float>   m_tot_ni;
    NTuple::Item<float>   m_cryo_x;
    NTuple::Item<float>   m_cryo_ni;
    NTuple::Item<float>   m_em_x;
    NTuple::Item<float>   m_em_ni;
    NTuple::Item<float>   m_hec_x;
    NTuple::Item<float>   m_hec_ni;
    NTuple::Item<float>   m_fcal_x;
    NTuple::Item<float>   m_fcal_ni;
    NTuple::Item<float>   m_fcal_y;
    NTuple::Item<float>   m_em_y;
    NTuple::Item<float>   m_hec_y;
    NTuple::Item<float>   m_cryo_y;
    NTuple::Item<float>   m_xcoord;

    // Data access
    SG::ReadHandle<McEventCollection> m_mcEvtColl;
    int m_verboseLevel;

  };
}
#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "RIO_OnTrackErrorScalingKit.h"
#include "MuonRIO_OnTrack/MuonEtaPhiRIO_OnTrackErrorScaling.h"

typedef char * CharPtr_t;
class MuonEtaPhiRIO_OnTrackErrorScalingKit
  : public RIO_OnTrackErrorScalingSpecialisedKit<MuonEtaPhiRIO_OnTrackErrorScaling>
{
public:
  MuonEtaPhiRIO_OnTrackErrorScalingKit(const std::string &prefix) {
    // use C-style array of strings to satisfy the interface
    m_names=new CharPtr_t[MuonEtaPhiRIO_OnTrackErrorScaling::kNParamTypes];
    for (unsigned int idx=0; idx< MuonEtaPhiRIO_OnTrackErrorScaling::kNParamTypes; ++idx ) {
      std::string tmp( prefix + " " + MuonEtaPhiRIO_OnTrackErrorScaling::paramNames()[idx] );
      tmp.shrink_to_fit();
      m_names[idx]=new char[ tmp.capacity() ];
      strncpy( m_names[idx], tmp.c_str(), tmp.capacity() );
    }
  }
  ~MuonEtaPhiRIO_OnTrackErrorScalingKit() {
    for (unsigned idx=0; idx<MuonEtaPhiRIO_OnTrackErrorScaling::kNParamTypes; ++idx) {
      delete [] m_names[idx];
    }
    delete [] m_names;
  }
  virtual unsigned int nParametres() const override {
    return MuonEtaPhiRIO_OnTrackErrorScaling::kNParamTypes;
  }
  virtual const char* const* paramNames()  const override {
    return const_cast<const char* const*>(m_names);
  }
private:
  char **m_names;
};

#include "MuonRIO_OnTrack/MDTRIO_OnTrackErrorScaling.h"
class MDTRIO_OnTrackErrorScalingKit
  : public RIO_OnTrackErrorScalingSpecialisedKit<MDTRIO_OnTrackErrorScaling>
{
public:
  MDTRIO_OnTrackErrorScalingKit() {}

  ~MDTRIO_OnTrackErrorScalingKit() {}

  virtual unsigned int nParametres() const override {
    return MDTRIO_OnTrackErrorScaling::kNParamTypes;
  }
  virtual const char* const* paramNames()  const override {
    return MDTRIO_OnTrackErrorScaling::paramNames();
  }
};


namespace {
  // register all kits with the help of a dummy function which sets a global anonymous bool
  bool registered = ( []() -> bool {
    return
         RIO_OnTrackErrorScalingKitManager::instance().registerKit("RPCRIO_OnTrackErrorScaling",    new MuonEtaPhiRIO_OnTrackErrorScalingKit("RPC"))
      && RIO_OnTrackErrorScalingKitManager::instance().registerKit("TGCRIO_OnTrackErrorScaling",    new MuonEtaPhiRIO_OnTrackErrorScalingKit("TGC"))
      && RIO_OnTrackErrorScalingKitManager::instance().registerKit("CSCRIO_OnTrackErrorScaling",    new MuonEtaPhiRIO_OnTrackErrorScalingKit("CSC"))
      && RIO_OnTrackErrorScalingKitManager::instance().registerKit("MDTRIO_OnTrackErrorScaling",    new MDTRIO_OnTrackErrorScalingKit);
    } ) ();
}

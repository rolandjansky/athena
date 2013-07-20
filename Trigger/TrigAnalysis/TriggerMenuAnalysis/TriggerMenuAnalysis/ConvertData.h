/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ConvertData_h__
#define __ConvertData_h__
/*
  ConvertData.h
*/
#include <vector>
#include "TriggerMenuNtuple/TrigStatus.h"

class RoIData_Muon;
class RoIData_TileMu;
class RoIData_Electron;
class RoIData_Tau;
class RoIData_Jet;
class RoIData_MissingET;

class MuonObjectsInRoI;
class TileMuObjectsInRoI;
class ElectronObjectsInRoI;
class JetObjectsInRoI;
class TauObjectsInRoI;
class MissingETObjectsInRoI;

class MuonFeature;
class MuonFeatureDetails;
class TrigInDetTrackCollection;
class CombinedMuonFeature;
class IsoMuonFeature;
class MSMuon;
class CBMuon;
namespace Rec {
  class TrackParticle;
}
class TrigMuonEFInfoContainer;
class Muon_ROI;

bool convertElectronData(RoIData_Electron& out, const ElectronObjectsInRoI& in);
bool convertMuonData(RoIData_Muon& out, const MuonObjectsInRoI& in);
bool convertTauData(RoIData_Tau& out, const TauObjectsInRoI& in);
bool convertJetData(RoIData_Jet& out, const JetObjectsInRoI& in);
bool convertTileMuData(RoIData_TileMu& out, const TileMuObjectsInRoI& in);
bool convertMissingETData(RoIData_MissingET& out, const MissingETObjectsInRoI& in);

//for Muon
void setL1(RoIData_Muon& m, const Muon_ROI* f);
void setL1(RoIData_TileMu& m, const Muon_ROI* f);

//void setMuFast(RoIData_Muon& m, const MuonFeature* f, std::vector<TrigStatus> status);
//void setMuFastDetails(RoIData_Muon& m, const MuonFeatureDetails* f);
//void setMuComb(RoIData_Muon& m, const CombinedMuonFeature* f, std::vector<TrigStatus> status);
//void setMuIso(RoIData_Muon& m, const IsoMuonFeature* f, std::vector<TrigStatus> status);
//void setInDet(RoIData_Muon& m, const TrigInDetTrackCollection* f, std::vector<TrigStatus> status);
//void setMuonEF(RoIData_Muon& m, const TrigMuonEFInfoContainer* c, std::vector<TrigStatus> status);
//void setMuonEF(RoIData_TileMu& m, const TrigMuonEFInfoContainer* c, std::vector<TrigStatus> status);

void setMuFast(RoIData_Muon& m, const MuonFeature* f, std::vector<std::string>& name, std::vector<int>& status);
void setMuFastDetails(RoIData_Muon& m, const MuonFeatureDetails* f);
void setMuComb(RoIData_Muon& m, const CombinedMuonFeature* f, std::vector<std::string>& name, std::vector<int>& status);
void setMuIso(RoIData_Muon& m, const IsoMuonFeature* f, std::vector<std::string>& name, std::vector<int>& status);
void setInDet(RoIData_Muon& m, const TrigInDetTrackCollection* f);
void setMuonEF(RoIData_Muon& m, const TrigMuonEFInfoContainer* c, std::vector<std::string>& name, std::vector<int>& status);
void setMuonEF(RoIData_TileMu& m, const TrigMuonEFInfoContainer* c, std::vector<std::string>& name, std::vector<int>& status);

double calc_dphi(double phi1, double phi2);
double calc_dR(double eta1, double phi1, double eta2, double phi2);
void setTrigStatus(const std::vector<TrigStatus>& v, std::vector<std::string>& vs, std::vector<int>& vi);

void setMSMuon(MSMuon& m, const Rec::TrackParticle* p);
void setSAMuon(CBMuon& m, const Rec::TrackParticle* p);
void setCBMuon(CBMuon& m, const Rec::TrackParticle* p);

#endif // __ConvertData_h__


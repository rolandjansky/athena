/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TrackSystems/TrackTypes.h"

//______________________________________________
qint32 TrackType::typeToInt(const Type&t)
{
  switch (t) {
  case TrkTrack: return 0;
  case TrackParticle: return 1;
  case LV2Track: return 2;
  case TruthTrack: return 3;
  case FatrasTruthTrack: return 4;
  case TrkSegment: return 5;
  case RefittedTrack: return 6;
  case AnalysisObject: return 7;
  case SimulationTrack: return 8;
  case xAOD: return 9;
  case Unknown: return 10;
  default: return -1;
  }
}

//______________________________________________
TrackType::Type TrackType::intToType(const qint32& i)
{
  switch (i) {
  case 0: return TrkTrack;
  case 1: return TrackParticle;
  case 2: return LV2Track;
  case 3: return TruthTrack;
  case 4: return FatrasTruthTrack;
  case 5: return TrkSegment;
  case 6: return RefittedTrack;
  case 7: return AnalysisObject;
  case 8: return SimulationTrack;
  case 9: return xAOD;
  case 10: return Unknown;
  default: return Unknown;
  }
}

//______________________________________________
QString TrackType::typeToString(const Type& t)
{
  switch (t) {
  case TrkTrack: return "TrkTrack";
  case TrackParticle: return "TrackParticle";
  case LV2Track: return "LV2Track";
  case TruthTrack: return "TruthTrack";
  case FatrasTruthTrack: return "FatrasTruthTrack";
  case RefittedTrack: return "RefittedTrack";
  case TrkSegment: return "TrkSegment";
  case AnalysisObject: return "AnalysisObject";
  case SimulationTrack: return "SimulationTrack";
  case xAOD: return "xAOD";
  case Unknown: return "Unknown";
  default: return "ERROR";
  }
}

//______________________________________________
TrackType::Type TrackType::stringToType(const QString&str, bool & status)
{
  status = true;
  if (str=="TrkTrack") return TrkTrack;
  if (str=="TrackParticle") return TrackParticle;
  if (str=="LV2Track") return LV2Track;
  if (str=="TruthTrack") return TruthTrack;
  if (str=="FatrasTruthTrack") return FatrasTruthTrack;
  if (str=="RefittedTrack") return RefittedTrack;
  if (str=="TrkSegment") return TrkSegment;
  if (str=="AnalysisObject") return AnalysisObject;
  if (str=="SimulationTrack") return SimulationTrack;
  if (str=="xAOD") return xAOD;
  if (str=="Unknown") return Unknown;
  status = false;
  return Unknown;
}

//______________________________________________
QString TrackType::typeToSectionHeader(const Type& t)
{
  switch (t) {
  case TrkTrack: return "Detailed Tracks";
  case TrackParticle: return "Track Particles";
  case LV2Track: return "Trigger Tracks";
  case TruthTrack: return "Truth Tracks";
  case FatrasTruthTrack: return "Fatras Truth Tracks";
  case RefittedTrack: return "Refitted Tracks";
  case TrkSegment: return "Track Segments";
  case AnalysisObject: return "Analysis Objects";
  case SimulationTrack: return "Forward Region Simulation Tracks";
  case xAOD: return "xAOD";
  case Unknown: return "Unknown";
  default: return "Error";
  }
}

//______________________________________________
TrackType::Type TrackType::sectionHeaderToType(const QString&str, bool & status)
{
  status = true;
  if (str=="Detailed Tracks") return TrkTrack;
  if (str=="Track Particles") return TrackParticle;
  if (str=="Trigger Tracks") return LV2Track;
  if (str=="Truth Tracks") return TruthTrack;
  if (str=="Fatras Truth Tracks") return FatrasTruthTrack;
  if (str=="Refitted Tracks") return RefittedTrack;
  if (str=="Track Segments") return TrkSegment;
  if (str=="Analysis Objects") return AnalysisObject;
  if (str=="Forward Region Simulation Tracks") return SimulationTrack;
  if (str=="xAOD") return xAOD;
  if (str=="Unknown") return Unknown;
  status = false;
  return Unknown;
}

//______________________________________________
QString TrackType::typeToSectionHeaderToolTip(const Type& t)
{
  switch (t) {
  case TrkTrack: return "Trk::Track's (detailed track objects as output by reconstruction)";
  case TrackParticle: return "TrackParticle's (summarized track objects, typically found in AOD's)";
  case LV2Track: return "LVL2 Trigger Tracks (tracks as found by the second level trigger algorithms)";
  case TruthTrack: return "Truth Tracks (truth tracks, determined by combining the HepMC event record with simulation hits)";
  case FatrasTruthTrack: return "Fatras Truth Tracks (detailed track objects, but constructed directly by Fatras rather than being the output of reconstruction)";
  case RefittedTrack: return "Trk::Track's resulting from interactive refits within VP1";
  case TrkSegment: return "Track Segments (track parts found in a single muon chamber or ID part. These are usually input to higher level global track reconstruction algorithms)";
  case AnalysisObject: return "AOD Analysis Objects (high level reconstruction objects, such as Muons, egamma etc)";
  case SimulationTrack: return "Forward Region Simulation Tracks";
  case xAOD: return "xAOD Analysis Objects (high level reconstruction objects, such as Muons, egamma etc)";
  case Unknown: return "Unknown";
  default: return "Error";
  }
}

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          Summary.cxx  -  description
                             -------------------
    begin                : Tue Feb 17 2004
    email                : edward.moyse@cern.ch
 ***************************************************************************/

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "GaudiKernel/MsgStream.h"

unsigned int Trk::TrackSummary::s_numberOfInstantiations=0;
const int    Trk::TrackSummary::SummaryTypeNotSet=-1;

Trk::TrackSummary::TrackSummary()
    :
    m_information(numberOfTrackSummaryTypes, SummaryTypeNotSet),
    m_eProbability(numberOfeProbabilityTypes, 0.5),
    m_dedx(-1),
    m_nhitsdedx(-1),
    m_nhitsoverflowdedx(-1),
    m_idHitPattern(0),m_indetTrackSummary(0),m_muonTrackSummary(0)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // new TrackSummary, so increment total count
#endif        
}

Trk::TrackSummary::TrackSummary( const std::vector<int>& information, const std::vector<float>& eProbability, std::bitset<numberOfDetectorTypes>& hitPattern, float dedx, int nhitsdedx, int noverflowdedx)
    :
    m_information( information ),
    m_eProbability( eProbability ),
    m_dedx(dedx),
    m_nhitsdedx(nhitsdedx),
    m_nhitsoverflowdedx(noverflowdedx),
    m_idHitPattern( hitPattern.to_ulong() ),m_indetTrackSummary(0),m_muonTrackSummary(0)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // new TrackSummary, so increment total count
#endif        
}

Trk::TrackSummary::TrackSummary( const TrackSummary& rhs )
    :
    m_information(rhs.m_information),
    m_eProbability(rhs.m_eProbability),
    m_dedx(rhs.m_dedx),
    m_nhitsdedx(rhs.m_nhitsdedx),
    m_nhitsoverflowdedx(rhs.m_nhitsoverflowdedx),
    m_idHitPattern(rhs.m_idHitPattern)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // new TrackSummary, so increment total count
#endif        
  if( rhs.m_indetTrackSummary ) {
    m_indetTrackSummary = new InDetTrackSummary(*rhs.m_indetTrackSummary);
  } else m_indetTrackSummary = 0;
  if( rhs.m_muonTrackSummary ) {
    m_muonTrackSummary = new MuonTrackSummary(*rhs.m_muonTrackSummary);
  } else m_muonTrackSummary = 0;
}

Trk::TrackSummary& Trk::TrackSummary::operator=(const TrackSummary& rhs) {
  if (&rhs !=this){
    m_information  = rhs.m_information;
    m_eProbability = rhs.m_eProbability;
    m_dedx         = rhs.m_dedx;
    m_nhitsdedx    = rhs.m_nhitsdedx;
    m_nhitsoverflowdedx = rhs.m_nhitsoverflowdedx;
    m_idHitPattern = rhs.m_idHitPattern;
    delete m_indetTrackSummary;
    m_indetTrackSummary = rhs.m_indetTrackSummary ? new InDetTrackSummary(*rhs.m_indetTrackSummary) : 0;
    delete m_muonTrackSummary;
    m_muonTrackSummary = rhs.m_muonTrackSummary ? new MuonTrackSummary(*rhs.m_muonTrackSummary) : 0;
  }
  return *this;
}


Trk::TrackSummary::~TrackSummary()
{
  delete m_indetTrackSummary;
  delete m_muonTrackSummary;
#ifndef NDEBUG
  s_numberOfInstantiations--; // delete TrackSummary, so decrement total count
#endif        
}

Trk::TrackSummary& Trk::TrackSummary::operator+=(const TrackSummary& ts)
{
    if (this!=&ts)
    {       
        for (int i=0;i<numberOfTrackSummaryTypes;++i) {
          // if added number is <0, leave as is (this also catches the case where both are <0)
          if (ts.m_information[i]<0) continue;
       
          if (m_information[i]<0) m_information[i]++; 
           m_information[i]+= ts.m_information[i];
        }
        for (int i=0;i<numberOfeProbabilityTypes;++i) m_eProbability[i] *= ts.m_eProbability[i];
        if (m_dedx<0 && ts.m_dedx>=0) {
          m_dedx=ts.m_dedx;
          m_nhitsdedx=ts.m_nhitsdedx;
	  m_nhitsoverflowdedx = ts.m_nhitsoverflowdedx;
        }
        if (!m_muonTrackSummary)  m_muonTrackSummary  = ts.m_muonTrackSummary  ? new MuonTrackSummary(*ts.m_muonTrackSummary) : 0; 
        if (!m_indetTrackSummary) m_indetTrackSummary = ts.m_indetTrackSummary ? new InDetTrackSummary(*ts.m_indetTrackSummary) :0;
	// FIXME - do we need to support adding of extension objects? How would this even work?
    }
    return *this;
}


namespace Trk {
namespace {

template <class T_out>
inline
T_out& dumpTrackSummary( T_out& out, const TrackSummary& trackSum )
{
  out << "Persistant track summary information:"<<"\n";
  out << " * Number of contrib. Pixel Layer: "<<trackSum.get(numberOfContribPixelLayers)<<"\n";
  out << " * Number of Innermost Pixel layer hits        : "<<trackSum.get(numberOfInnermostPixelLayerHits)<<"\n";
  out << " * Number of Innermost Pixel layer shared hits : "<<trackSum.get(numberOfInnermostPixelLayerSharedHits)<<"\n";
  out << " * Number of Innermost Pixel layer outliers    : "<<trackSum.get(numberOfInnermostPixelLayerOutliers)<<"\n";
  out << " * Expect Innermost Pixel layer hits (0/1)     : "<<trackSum.get(expectInnermostPixelLayerHit)<<"\n";
  out << " * Number of Next-To-Innermost Pixel layer hits        : "<<trackSum.get(numberOfNextToInnermostPixelLayerHits)<<"\n";
  out << " * Number of Next-To-Innermost Pixel layer shared hits : "<<trackSum.get(numberOfNextToInnermostPixelLayerSharedHits)<<"\n";
  out << " * Number of Next-To-Innermost Pixel layer outliers    : "<<trackSum.get(numberOfNextToInnermostPixelLayerOutliers)<<"\n";
  out << " * Expect Next-To-Innermost Pixel layer hits (0/1)     : "<<trackSum.get(expectNextToInnermostPixelLayerHit)<<"\n";
  out << " * Number of pixel hits          : "<<trackSum.get(numberOfPixelHits)<<"\n";
  out << " * Number of pixel outliers      : "<<trackSum.get(numberOfPixelOutliers)<<"\n";
  out << " * Number of spoilt pixel hits   : "<<trackSum.get(numberOfPixelSpoiltHits)<<"\n";
  out << " * Number of pixel holes         : "<<trackSum.get(numberOfPixelHoles)<<"\n";
  out << " * Number of pixel shared hits   : "<<trackSum.get(numberOfPixelSharedHits)<<"\n";
  out << " * Number of GangedPixels        : "<<trackSum.get(numberOfGangedPixels)<<"\n";
  out << " * Number of GangedFlaggedFakes  : "<<trackSum.get(numberOfGangedFlaggedFakes)<<"\n";
  out << " * Number of dead pixel sensors  : "<<trackSum.get(numberOfPixelDeadSensors)<<"\n";
  out << " * Number of DBM hits            : "<<trackSum.get(numberOfDBMHits)<<"\n";
  out << " * Number of SCT hits            : "<<trackSum.get(numberOfSCTHits)<<"\n";
  out << " * Number of SCT outliers        : "<<trackSum.get(numberOfSCTOutliers)<<"\n";
  out << " * Number of SCT holes           : "<<trackSum.get(numberOfSCTHoles)<<"\n";
  out << " * Number of SCT double Holes    : "<<trackSum.get(numberOfSCTDoubleHoles)<<"\n";
  out << " * Number of SCT shared hits     : "<<trackSum.get(numberOfSCTSharedHits)<<"\n";
  out << " * Number of dead SCT sensors    : "<<trackSum.get(numberOfSCTDeadSensors)<<"\n";
  out << " * Number of spoilt SCT hits     : "<<trackSum.get(numberOfSCTSpoiltHits)<<"\n";
  out << " * Number of TRT hits            : "<<trackSum.get(numberOfTRTHits)<<"\n";
  out << " * Number of TRT xenon hits      : "<<trackSum.get(numberOfTRTXenonHits)<<"\n";
  out << " * Number of TRT outliers        : "<<trackSum.get(numberOfTRTOutliers)<<"\n";
  out << " * Number of TRT TR hits         : "<<trackSum.get(numberOfTRTHighThresholdHits)<<"\n";
  out << " * Number of TRT TR hits total   : "<<trackSum.get(numberOfTRTHighThresholdHitsTotal)<<"\n";
  out << " * Number of TRT TR outliers     : "<<trackSum.get(numberOfTRTHighThresholdOutliers)<<"\n";
  out << " * Number of TRT holes           : "<<trackSum.get(numberOfTRTHoles)<<"\n";
  out << " * Number of TRT tube hits       : "<<trackSum.get(numberOfTRTTubeHits)<<"\n";
  out << " * Number of TRT Shared hits     : "<<trackSum.get(numberOfTRTSharedHits)<<"\n";
  out << " * Number of dead TRT straws     : "<<trackSum.get(numberOfTRTDeadStraws)<<"\n";
  out << " * Number of MDT hits            : "<<trackSum.get(numberOfMdtHits)<<"\n";
  out << " * Number of MDT holes           : "<<trackSum.get(numberOfMdtHoles)<<"\n";

  // New Small Wheel
  out << " * Number of STGC hits           : phi "<<trackSum.get(numberOfStgcPhiHits)
      << " eta " <<trackSum.get(numberOfStgcEtaHits)<<"\n";
  out << " * Number of STGC holes          : phi "<<trackSum.get(numberOfStgcPhiHoles)
      << " eta  "<<trackSum.get(numberOfStgcEtaHoles)<<"\n";
  out << " * Number of MM hits             : "<<trackSum.get(numberOfMmHits)<<"\n";
  out << " * Number of MM holes            : "<<trackSum.get(numberOfMmHoles)<<"\n";

  out << " * Number of TGC hits            : phi "<<trackSum.get(numberOfTgcPhiHits)
      << " eta " <<trackSum.get(numberOfTgcEtaHits)<<"\n";
  out << " * Number of TGC holes           : phi "<<trackSum.get(numberOfTgcPhiHoles)
      << " eta  "<<trackSum.get(numberOfTgcEtaHoles)<<"\n";
  out << " * Number of RPC hits            : phi "<<trackSum.get(numberOfRpcPhiHits)
      << " eta " <<trackSum.get(numberOfRpcEtaHits) <<"\n";
  out << " * Number of RPC holes           : phi "<<trackSum.get(numberOfRpcPhiHoles)
      << " eta  "<<trackSum.get(numberOfRpcEtaHoles)<<"\n";
  out << " * Number of CSC hits            : phi "<<trackSum.get(numberOfCscPhiHits)
      << " eta " <<trackSum.get(numberOfCscEtaHits) <<"\n";
  out << " * Number of unspoilt CSC etahits:"<<trackSum.get(numberOfCscUnspoiltEtaHits) <<"\n";
  out << " * Number of CSC holes           : phi "<<trackSum.get(numberOfCscPhiHoles)
      << " eta  "<<trackSum.get(numberOfCscEtaHoles)<<"\n";
  out << " * Number of Outliers            : "<<trackSum.get(numberOfOutliersOnTrack)<<"\n";
  out << "Transient track summary information:"<<"\n";
  out << " * scatter of chi2 on Surface    : "<<float(trackSum.get(standardDeviationOfChi2OS))/100.<<"\n";
    
  out << " --------------------------------- " << "\n";
  out << " * Electron probability combined:  " << trackSum.getPID(eProbabilityComb) << "\n";
  out << " * Electron probability from HT:   " << trackSum.getPID(eProbabilityHT)   << "\n";
  out << " * Electron probability from ToT:  " << trackSum.getPID(eProbabilityToT)  << "\n";
  out << " * Electron probability from Brem: " << trackSum.getPID(eProbabilityBrem) << "\n";
  out << " --------------------------------- " << "\n";

  out << " dE/dx from pixels               : " << trackSum.getPixeldEdx() << " MeV g^-1 cm^2" << "\n";
  out << " number of hits used for dE/dx   : " << trackSum.numberOfUsedHitsdEdx() << "\n";
  out << " number of overflow hits used for dE/dx   : " << trackSum.numberOfOverflowHitsdEdx() << "\n";
  //this is a bit nasty, but I don't have access to internal data members
  out << " Hit pattern (see DetectorType enum for meaning) : ";
  for (int i=0; i<Trk::numberOfDetectorTypes; ++i) 
    {
      if (trackSum.isHit(static_cast<Trk::DetectorType>(i))) {
    out<<"1";
      }else{
    out<<"0";
      }
    }
    out << "\n";
    if (0!=trackSum.indetTrackSummary()){
        out<<*(trackSum.indetTrackSummary());
    }
    if (0!=trackSum.muonTrackSummary()){
        out<<*(trackSum.muonTrackSummary());
    }
  return out;
}
}
}

std::ostream& Trk::operator<<( std::ostream& out, const TrackSummary& trackSum )
{
  return dumpTrackSummary(out,trackSum);
}

MsgStream& Trk::operator<<( MsgStream& out, const TrackSummary& trackSum )
{
  return dumpTrackSummary(out,trackSum);
}

unsigned int Trk::TrackSummary::numberOfInstantiations() 
{
  return s_numberOfInstantiations;
}


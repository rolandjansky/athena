/***************************************************************************
                          Summary.cxx  -  description
                             -------------------
    begin                : Tue Feb 17 2004
    copyright            : (C) 2004 by emoyse
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


std::ostream& Trk::operator<<( std::ostream& out, const TrackSummary& trackSum )
{
  out << "Persistant track summary information:"<<std::endl;
  out << " * Number of contrib. Pixel Layer: "<<trackSum.get(numberOfContribPixelLayers)<<std::endl;
  out << " * Number of Innermost Pixel layer hits        : "<<trackSum.get(numberOfInnermostPixelLayerHits)<<std::endl;
  out << " * Number of Innermost Pixel layer shared hits : "<<trackSum.get(numberOfInnermostPixelLayerSharedHits)<<std::endl;
  out << " * Number of Innermost Pixel layer outliers    : "<<trackSum.get(numberOfInnermostPixelLayerOutliers)<<std::endl;
  out << " * Expect Innermost Pixel layer hits (0/1)     : "<<trackSum.get(expectInnermostPixelLayerHit)<<endreq;
  out << " * Number of Next-To-Innermost Pixel layer hits        : "<<trackSum.get(numberOfNextToInnermostPixelLayerHits)<<std::endl;
  out << " * Number of Next-To-Innermost Pixel layer shared hits : "<<trackSum.get(numberOfNextToInnermostPixelLayerSharedHits)<<std::endl;
  out << " * Number of Next-To-Innermost Pixel layer outliers    : "<<trackSum.get(numberOfNextToInnermostPixelLayerOutliers)<<std::endl;
  out << " * Expect Next-To-Innermost Pixel layer hits (0/1)     : "<<trackSum.get(expectNextToInnermostPixelLayerHit)<<endreq;
  out << " * Number of pixel hits          : "<<trackSum.get(numberOfPixelHits)<<std::endl;
  out << " * Number of pixel outliers      : "<<trackSum.get(numberOfPixelOutliers)<<std::endl;
  out << " * Number of spoilt pixel hits   : "<<trackSum.get(numberOfPixelSpoiltHits)<<std::endl;
  out << " * Number of pixel holes         : "<<trackSum.get(numberOfPixelHoles)<<std::endl;
  out << " * Number of pixel shared hits   : "<<trackSum.get(numberOfPixelSharedHits)<<std::endl;
  out << " * Number of GangedPixels        : "<<trackSum.get(numberOfGangedPixels)<<std::endl;
  out << " * Number of GangedFlaggedFakes  : "<<trackSum.get(numberOfGangedFlaggedFakes)<<std::endl;
  out << " * Number of dead pixel sensors  : "<<trackSum.get(numberOfPixelDeadSensors)<<std::endl;
  out << " * Number of SCT hits            : "<<trackSum.get(numberOfSCTHits)<<std::endl;
  out << " * Number of SCT outliers        : "<<trackSum.get(numberOfSCTOutliers)<<std::endl;
  out << " * Number of SCT holes           : "<<trackSum.get(numberOfSCTHoles)<<std::endl;
  out << " * Number of SCT double Holes    : "<<trackSum.get(numberOfSCTDoubleHoles)<<std::endl;
  out << " * Number of SCT shared hits     : "<<trackSum.get(numberOfSCTSharedHits)<<std::endl;
  out << " * Number of dead SCT sensors    : "<<trackSum.get(numberOfSCTDeadSensors)<<std::endl;
  out << " * Number of spoilt SCT hits     : "<<trackSum.get(numberOfSCTSpoiltHits)<<std::endl;
  out << " * Number of TRT hits            : "<<trackSum.get(numberOfTRTHits)<<std::endl;
  out << " * Number of TRT xenon hits      : "<<trackSum.get(numberOfTRTXenonHits)<<std::endl;
  out << " * Number of TRT outliers        : "<<trackSum.get(numberOfTRTOutliers)<<std::endl;
  out << " * Number of TRT TR hits         : "<<trackSum.get(numberOfTRTHighThresholdHits)<<std::endl;
  out << " * Number of TRT TR outliers     : "<<trackSum.get(numberOfTRTHighThresholdOutliers)<<std::endl;
  out << " * Number of TRT holes           : "<<trackSum.get(numberOfTRTHoles)<<std::endl;
  out << " * Number of TRT tube hits       : "<<trackSum.get(numberOfTRTTubeHits)<<std::endl;
  out << " * Number of dead TRT straws     : "<<trackSum.get(numberOfTRTDeadStraws)<<std::endl;
  out << " * Number of MDT hits            : "<<trackSum.get(numberOfMdtHits)<<std::endl;
  out << " * Number of MDT holes           : "<<trackSum.get(numberOfMdtHoles)<<std::endl;
  out << " * Number of TGC hits            : phi "<<trackSum.get(numberOfTgcPhiHits)
      << " eta " <<trackSum.get(numberOfTgcEtaHits)<<std::endl;
  out << " * Number of TGC holes           : phi "<<trackSum.get(numberOfTgcPhiHoles)
      << " eta  "<<trackSum.get(numberOfTgcEtaHoles)<<std::endl;
  out << " * Number of RPC hits            : phi "<<trackSum.get(numberOfRpcPhiHits)
      << " eta " <<trackSum.get(numberOfRpcEtaHits) <<std::endl;
  out << " * Number of RPC holes           : phi "<<trackSum.get(numberOfRpcPhiHoles)
      << " eta  "<<trackSum.get(numberOfRpcEtaHoles)<<std::endl;
  out << " * Number of CSC hits            : phi "<<trackSum.get(numberOfCscPhiHits)
      << " eta " <<trackSum.get(numberOfCscEtaHits) <<std::endl;
  out << " * Number of unspoilt CSC etahits:"<<trackSum.get(numberOfCscUnspoiltEtaHits) <<std::endl;
  out << " * Number of CSC holes           : phi "<<trackSum.get(numberOfCscPhiHoles)
      << " eta  "<<trackSum.get(numberOfCscEtaHoles)<<std::endl;
  out << " * Number of Outliers            : "<<trackSum.get(numberOfOutliersOnTrack)<<std::endl;
  out << "Transient track summary information:"<<std::endl;
  out << " * scatter of chi2 on Surface    : "<<float(trackSum.get(standardDeviationOfChi2OS))/100.<<std::endl;

  out << " --------------------------------- " << std::endl;
  out << " * Electron probability combined:  " << trackSum.getPID(eProbabilityComb) << std::endl;
  out << " * Electron probability from HT:   " << trackSum.getPID(eProbabilityHT)   << std::endl;
  out << " * Electron probability from ToT:  " << trackSum.getPID(eProbabilityToT)  << std::endl;
  out << " * Electron probability from Brem: " << trackSum.getPID(eProbabilityBrem) << std::endl;
  out << " --------------------------------- " << std::endl;

  out << " dE/dx from pixels               : " << trackSum.getPixeldEdx() << " MeV g^-1 cm^2" << std::endl;
  out << " number of hits used for dE/dx   : " << trackSum.numberOfUsedHitsdEdx() << std::endl;
  out << " number of overflow hits used for dE/dx   : " << trackSum.numberOfOverflowHitsdEdx() << std::endl;
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
    out << std::endl;
    if (0!=trackSum.indetTrackSummary()){
        out<<*(trackSum.indetTrackSummary());
    }
    if (0!=trackSum.muonTrackSummary()){
        out<<*(trackSum.muonTrackSummary());
    }
  return out;
}

MsgStream& Trk::operator<<( MsgStream& out, const TrackSummary& trackSum )
{
  out << "Track summary information:"<<endreq;
  out << " * Number of contrib. Pixel Layer: "<<trackSum.get(numberOfContribPixelLayers)<<endreq;
  out << " * Number of B layer hits        : "<<trackSum.get(numberOfBLayerHits)<<endreq;
  out << " * Number of B layer outliers    : "<<trackSum.get(numberOfBLayerOutliers)<<endreq;
  out << " * Number of B layer shared hits : "<<trackSum.get(numberOfBLayerSharedHits)<<endreq;
  out << " * Expect B layer hits (0/1)     : "<<trackSum.get(expectBLayerHit)<<endreq;
  out << " * Number of pixel hits          : "<<trackSum.get(numberOfPixelHits)<<endreq;
  out << " * Number of pixel outliers      : "<<trackSum.get(numberOfPixelOutliers)<<endreq;
  out << " * Number of spoilt pixel hits   : "<<trackSum.get(numberOfPixelSpoiltHits)<<endreq;
  out << " * Number of pixel holes         : "<<trackSum.get(numberOfPixelHoles)<<endreq;
  out << " * Number of pixel shared hits   : "<<trackSum.get(numberOfPixelSharedHits)<<endreq;
  out << " * Number of GangedPixels        : "<<trackSum.get(numberOfGangedPixels)<<endreq;
  out << " * Number of GangedFlaggedFakes  : "<<trackSum.get(numberOfGangedFlaggedFakes)<<endreq;
  out << " * Number of dead pixel sensors  : "<<trackSum.get(numberOfPixelDeadSensors)<<endreq;
  out << " * Number of SCT hits            : "<<trackSum.get(numberOfSCTHits)<<endreq;
  out << " * Number of SCT outliers        : "<<trackSum.get(numberOfSCTOutliers)<<endreq;
  out << " * Number of SCT holes           : "<<trackSum.get(numberOfSCTHoles)<<endreq;
  out << " * Number of SCT double Holes    : "<<trackSum.get(numberOfSCTDoubleHoles)<<endreq;
  out << " * Number of SCT shared hits     : "<<trackSum.get(numberOfSCTSharedHits)<<endreq;
  out << " * Number of dead SCT sensors    : "<<trackSum.get(numberOfSCTDeadSensors)<<endreq;
  out << " * Number of spoilt SCT hits     : "<<trackSum.get(numberOfSCTSpoiltHits)<<endreq;
  out << " * Number of TRT hits            : "<<trackSum.get(numberOfTRTHits)<<endreq;
  out << " * Number of TRT xenon hits      : "<<trackSum.get(numberOfTRTXenonHits)<<endreq;
  out << " * Number of TRT outliers        : "<<trackSum.get(numberOfTRTOutliers)<<endreq;
  out << " * Number of TRT TR hits         : "<<trackSum.get(numberOfTRTHighThresholdHits)<<endreq;
  out << " * Number of TRT TR outliers     : "<<trackSum.get(numberOfTRTHighThresholdOutliers)<<endreq;
  out << " * Number of TRT holes           : "<<trackSum.get(numberOfTRTHoles)<<endreq;
  out << " * Number of TRT tube hits       : "<<trackSum.get(numberOfTRTTubeHits)<<endreq;
  out << " * Number of dead TRT straws     : "<<trackSum.get(numberOfTRTDeadStraws)<<endreq;
  out << " * Number of MDT hits            : "<<trackSum.get(numberOfMdtHits)<<endreq;
  out << " * Number of MDT holes           : "<<trackSum.get(numberOfMdtHoles)<<endreq;
  out << " * Number of TGC hits            : phi "<<trackSum.get(numberOfTgcPhiHits)
      << " eta " <<trackSum.get(numberOfTgcEtaHits)<<endreq;
  out << " * Number of TGC holes           : phi "<<trackSum.get(numberOfTgcPhiHoles)
      << " eta " <<trackSum.get(numberOfTgcEtaHoles) <<endreq;
  out << " * Number of RPC hits            : phi "<<trackSum.get(numberOfRpcPhiHits)
      << " eta " <<trackSum.get(numberOfRpcEtaHits) <<endreq;
  out << " * Number of RPC holes           : phi "<<trackSum.get(numberOfRpcPhiHoles)
      << " eta " <<trackSum.get(numberOfRpcEtaHoles) <<endreq;
  out << " * Number of CSC hits            : phi "<<trackSum.get(numberOfCscPhiHits)
      << " eta " <<trackSum.get(numberOfCscEtaHits) <<endreq;
  out << " * Number of unspoilt CSC etahits:"<<trackSum.get(numberOfCscUnspoiltEtaHits) <<endreq;
  out << " * Number of CSC holes           : phi "<<trackSum.get(numberOfCscPhiHoles)
      << " eta " <<trackSum.get(numberOfCscEtaHoles) <<endreq;
  out << " * Number of Outliers            : "<<trackSum.get(numberOfOutliersOnTrack)<<endreq;
  out << "Transient track summary information:"<<endreq;
  out << " * scatter of chi2 on Surface    : "<<float(trackSum.get(standardDeviationOfChi2OS))/100.<<endreq;
    
  out << " --------------------------------- " << endreq;
  out << " * Electron probability combined:  " << trackSum.getPID(eProbabilityComb) << endreq;
  out << " * Electron probability from HT:   " << trackSum.getPID(eProbabilityHT)   << endreq;
  out << " * Electron probability from ToT:  " << trackSum.getPID(eProbabilityToT)  << endreq;
  out << " * Electron probability from Brem: " << trackSum.getPID(eProbabilityBrem) << endreq;
  out << " --------------------------------- " << endreq;

  out << " dE/dx from pixels               : " << trackSum.getPixeldEdx() << " MeV g^-1 cm^2" << endreq;
  out << " number of hits used for dE/dx   : " << trackSum.numberOfUsedHitsdEdx() << endreq;
  out << " number of overflow hits used for dE/dx   : " << trackSum.numberOfOverflowHitsdEdx() << std::endl;
    
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
    out << endreq;
    if (0!=trackSum.indetTrackSummary()){
        out<<*(trackSum.indetTrackSummary());
    }
    if (0!=trackSum.muonTrackSummary()){
        out<<*(trackSum.muonTrackSummary());
    }
  
  return out;
}

unsigned int Trk::TrackSummary::numberOfInstantiations() 
{
  return s_numberOfInstantiations;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXTRAPOLATEMUONTOIPTOOL_H
#define EXTRAPOLATEMUONTOIPTOOL_H

#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"


namespace Trk{
  class IExtrapolator;
}
namespace Muon{
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
}

/**
   Tool to extrapolate tracks in the muon system to the IP. Internally uses IMuonTrackThroughCalo for 
   the back extrapolation.

   @author Camilla Maiani
*/
class ExtrapolateMuonToIPTool : virtual public Muon::IMuonTrackExtrapolationTool, public AthAlgTool  {
 public:

  /** Constructors */
  ExtrapolateMuonToIPTool(const std::string&, const std::string&, const IInterface*);

  /** Destructor */
  virtual ~ExtrapolateMuonToIPTool();

  /** initialize */
  virtual StatusCode initialize();

  /** initialize */
  virtual StatusCode finalize();

  /** @brief extrapolate all tracks in the track collection to the IP
      @param   muonTracks the track collection with muon tracks to be extrapolated to the IP
      @return  TrackCollection with the tracks at the IP, ownership is passed to the caller
  */
  TrackCollection* extrapolate(const TrackCollection& muonTracks) const;

  /** @brief extrapolate a muon track the IP, will return 0 if the back extrapolation fails
      @param   muonTrack the moun inpu track
      @return  Track at the IP, ownership is passed to the caller, return zero if back extrapolation failed
  */
  Trk::Track* extrapolate(const Trk::Track& muonTrack) const; 

 private:
  
  /** find measured parameters closest to IP to start back extrapolation */ 
  const Trk::TrackParameters* findMeasuredParametersClosestToIP( const Trk::Track& track ) const;


  ToolHandle<Trk::IExtrapolator>   m_extrapolator;              //!< Extrapolator
  ToolHandle<Muon::MuonEDMHelperTool>    m_helper;               //!< muon EDM helper tool
  ToolHandle<Muon::MuonEDMPrinterTool>   m_printer;              //!< muon EDM printer tool

  mutable unsigned int m_nextrapolations;
  //mutable unsigned int m_ngoodExtrapolations;
  mutable unsigned int m_failedClosestPars;
  mutable unsigned int m_failedExtrapolationLowMom;
  mutable unsigned int m_failedExtrapolationHighMom;
  mutable unsigned int m_failedPerigeeCreation;
};

#endif // EXTRAPOLATEMUONTOIPTOOL_H


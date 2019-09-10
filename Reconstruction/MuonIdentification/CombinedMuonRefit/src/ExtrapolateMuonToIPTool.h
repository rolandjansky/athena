/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXTRAPOLATEMUONTOIPTOOL_H
#define EXTRAPOLATEMUONTOIPTOOL_H

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <atomic>

namespace Trk{
  class IExtrapolator;
}
namespace Muon{
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

  std::unique_ptr<const Trk::Perigee> createPerigee( const Trk::TrackParameters& pars ) const;

  ToolHandle<Trk::IExtrapolator>   m_extrapolator;              //!< Extrapolator
  ToolHandle<Trk::IExtrapolator>   m_muonExtrapolator;              //!< MuonExtrapolator
  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
    "Handle to the service providing the IMuonEDMHelperSvc interface" };               //!< muon EDM helper tool
  ToolHandle<Muon::MuonEDMPrinterTool>   m_printer;              //!< muon EDM printer tool

  mutable std::atomic_uint m_nextrapolations;
  mutable std::atomic_uint m_failedClosestPars;
  mutable std::atomic_uint m_failedExtrapolationLowMom;
  mutable std::atomic_uint m_failedExtrapolationHighMom;
  mutable std::atomic_uint m_failedPerigeeCreation;
};

#endif // EXTRAPOLATEMUONTOIPTOOL_H


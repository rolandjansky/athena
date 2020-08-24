/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrackExtensionAlg
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for extension trackS reconstructed in Pxels and SCT to TRT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 22/08/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackExtensionAlg_H
#define TRT_TrackExtensionAlg_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"

// Store Gate handle keys
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrkTrack/TrackExtensionMap.h"
#include "TrkTrack/TrackCollection.h"

#include "CxxUtils/checker_macros.h"
#include <atomic>

namespace InDet {

	class TRT_TrackExtensionAlg : public AthReentrantAlgorithm {

		///////////////////////////////////////////////////////////////////
		// Public methods:
		///////////////////////////////////////////////////////////////////
  
	public:

		///////////////////////////////////////////////////////////////////
		// Standard Algotithm methods
		///////////////////////////////////////////////////////////////////

		TRT_TrackExtensionAlg(const std::string &name, ISvcLocator *pSvcLocator);
		virtual ~TRT_TrackExtensionAlg() {}
		StatusCode initialize();
		StatusCode execute(const EventContext& ctx) const;
		StatusCode finalize();

		///////////////////////////////////////////////////////////////////
		// Print internal algorithm parameters and status
		///////////////////////////////////////////////////////////////////

	protected:
		///////////////////////////////////////////////////////////////////
		// Protected data 
		///////////////////////////////////////////////////////////////////

                struct Counter_t {
                   int m_nTracks = 0 ;  // Number input  tracks
                   int m_nTracksExtended = 0;  // Number output tracks
                   Counter_t &operator += (const Counter_t &a) {
                      m_nTracks         +=a.m_nTracks;
                      m_nTracksExtended +=a.m_nTracksExtended;
                      return *this;
                   }
                };
                mutable std::mutex m_counterMutex ATLAS_THREAD_SAFE;
                mutable Counter_t  m_totalCounts  ATLAS_THREAD_SAFE;

		ToolHandle<ITRT_TrackExtensionTool> m_trtExtension{this, "TrackExtensionTool", "InDet::TRT_TrackExtensionTool_xk", "TRT track extension tool"};

		// Data handle keys
		SG::ReadHandleKey<TrackCollection> m_inputTracksKey	{ this, "InputTracksLocation", "SiSPSeededTracks", "Input tracks container name" };
		SG::WriteHandleKey<TrackExtensionMap> m_outputTracksKey { this, "ExtendedTracksLocation", "ExtendedTracksMapPhase", "Output tracks container name" };

		///////////////////////////////////////////////////////////////////
		// Protected methods
		///////////////////////////////////////////////////////////////////

		MsgStream& dumpConditions(MsgStream& out) const;
                MsgStream& dumpEvent(MsgStream& out, const Counter_t &counter) const;

	};
}
#endif // TRT_TrackExtensionAlg_H

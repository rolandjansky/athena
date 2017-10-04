/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"

// Store Gate handle keys
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrkTrack/TrackExtensionMap.h"
#include "TrkTrack/TrackCollection.h"

namespace InDet {

	class TRT_TrackExtensionAlg : public AthAlgorithm {

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
		StatusCode execute();
		StatusCode finalize();

		///////////////////////////////////////////////////////////////////
		// Print internal algorithm parameters and status
		///////////////////////////////////////////////////////////////////

		MsgStream& dump (MsgStream& out) const;
		std::ostream& dump (std::ostream& out) const;

	protected:

		///////////////////////////////////////////////////////////////////
		// Protected data 
		///////////////////////////////////////////////////////////////////

		int m_outputlevel;  // Print level for debug
		int m_nprint;  // Kind of  print    
		int m_nTracks;  // Number input  tracks
		int m_nTracksExtended;  // Number output tracks
		int m_nTracksTotal;  // Number input  tracks
		int m_nTracksExtendedTotal;  // Number output tracks
		//		std::string m_tracksLocation;  // Input  tracks location
		//		std::string m_extendedTracksLocation;  // Output tracks location
		ToolHandle<ITRT_TrackExtensionTool> m_trtExtension{this, "TrackExtensionTool", "InDet::TRT_TrackExtensionTool_xk", "TRT track extension tool"};

		// Data handle keys
		SG::ReadHandleKey<TrackCollection> m_inputTracksKey	{ this, "InputTracksLocation", "SiSPSeededTracks", "Input tracks container name" };
		SG::WriteHandleKey<TrackExtensionMap> m_outputTracksKey { this, "ExtendedTracksLocation", "ExtendedTracksMapPhase", "Output tracks container name" };

		///////////////////////////////////////////////////////////////////
		// Protected methods
		///////////////////////////////////////////////////////////////////

		MsgStream& dumpConditions(MsgStream& out) const;
		MsgStream& dumpEvent(MsgStream& out) const;

	};

	MsgStream& operator << (MsgStream&, const TRT_TrackExtensionAlg&);
	std::ostream& operator << (std::ostream&, const TRT_TrackExtensionAlg&); 
}
#endif // TRT_TrackExtensionAlg_H

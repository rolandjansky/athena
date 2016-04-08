/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 06.07.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_MDTChamberGeometryDeterminationH
#define MuonCalib_MDTChamberGeometryDeterminationH

//:::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: CLASS NtupleMDTChamberGeometryDeterminationTool ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::

/// \class NtupleMDTChamberGeometryDeterminationTool
///
/// This class can be used to measure the positions of tube, layer, or
/// multilayer positions and orientations with respect to a selected multilayer
/// of the same chamber. The class uses straight muon tracks for the geometry
/// determination.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 06.07.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// CLHEP //
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

// ROOT //
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"

#include "AthenaBaseComps/AthAlgTool.h"

// AtlasConditions //
#include "MuonIdHelpers/MdtIdHelper.h"

// MuonCalib //
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"

class MdtCalibInputSvc;


namespace MuonGM {
class MuonDetectorManager;
}

namespace MuonCalib {

class IIdToFixedIdTool;

class NtupleMDTChamberGeometryDeterminationTool : public AthAlgTool,
					virtual public NtupleCalibrationTool {

public:
// Constructors //
	NtupleMDTChamberGeometryDeterminationTool(const std::string & t,
				const std::string & n, const IInterface * p);
	///< Constructor of the tool.

	~NtupleMDTChamberGeometryDeterminationTool(void);
	///< Destructor of the tool.

// Methods //
// methods requires by the base classes //
	StatusCode initialize(void);
					///< initialization of the tool
	StatusCode finalize(void);
					///< finalization of the tool;
					///< a text file with the corrected
					///< geometry and the initial geometry
					///< of the investigated chambers is
					///< written
	StatusCode handleEvent(const MuonCalibEvent & event, int evnt_nb,
			const std::vector<MuonCalibSegment *> & segments,
			unsigned int position);
					///< analysis of the event
	bool KeepSegments(void) const;
					///< returns true, if segments should
					///< be stored in memory, false
					///< otherwise

private:
// job options //
	bool m_write_control_histograms; // = true, if control histograms should
					 //         be written;
					 // = false, if no control histgrams
					 //         should be written
	std::string m_granularity; // = "TUBE", if the positions of single
				   //           wires should be determined;
				   // = "LAYER", if the positions and
				   //            orientations of multilayers
				   //            should be determined;
				   // = "MULTILAYER", if the position and
				   //                 orientation of multilayers
				   //                 should be determined
	unsigned int m_reference_ml; // reference multilayer
	double m_road_width; // road width for pattern recognition
	std::string m_geometry_file_name; // name of the geometry file
	std::string m_MDT_ID_helper; // name of the MDT ID helper
	std::string m_idToFixedIdToolType; // type of the muon fixed id tool
	std::string m_idToFixedIdToolName; // name of the muon fixed id tool
	std::string m_ROOT_file_name; // name of the output ROOT file

// ROOT file and control ntuple //
	TFile *m_tfile; // pointer to the ROOT file in which the control
			// histograms are written on request
	TNtuple *m_ntuple; // control ntuple


// calibration parameters //
	ServiceHandle<MdtCalibInputSvc> m_input_service; // pointer to the calibration input
					   // service

// chamber parameters //
	std::map<NtupleStationId, unsigned int> m_nb_ml; // number of
							 // multilayers
	std::map<NtupleStationId, unsigned int> m_nb_ly; // number of layers per
							 // multilayer
	std::map<NtupleStationId, unsigned int> m_nb_tb; // number of tubes per
						         // layer

// straight-line fitters //
	QuasianalyticLineReconstruction m_qfitter; // needed for pattern
						   // recognition
	DCSLFitter m_dfitter; // good refitter

// variables for the geometry determination //
	unsigned int m_grouping; // = 1, if the positions of single wires should
				 //      be determined;
				 // = 2, if the positions and orientations of 
				 //      multilayers should be determined;
				 // = 3, if the positions and orientations of
				 //      multilayers should be determined
	unsigned int m_index_counter; // auxiliary index counter
	std::map< NtupleStationId, unsigned int > m_index;
				// map of indices to allow for the access to the
				// alignment objects;
				// this is used as a temporary solution for
				// problems with maps of vectors of HepVectors
	std::vector< std::vector<CLHEP::HepVector> > m_v; // vectors of alignment 
						     // parameters
	std::vector< std::vector<CLHEP::HepSymMatrix> > m_A; // alignment equation
							// m_A*m_v = m_B
	std::vector< std::vector<CLHEP::HepVector> > m_B; // alignment equation:
						     // m_A*m_v = m_B

// bookkeeping in order to write the geometry file //
	std::map<NtupleStationId, MuonFixedId> m_id; // station identifiers
	std::vector< std::vector<CLHEP::Hep3Vector> > m_RO; // read-out sides of the
						     // wires
	std::vector< std::vector<CLHEP::Hep3Vector> > m_HV; // high-voltage sides of
						     // the wires
	// The end of the anode wires are needed because storegate  and //
	// geomodel might be finalized before this tool.                //

// helpers //
	const MdtIdHelper *m_MdtIdHelper; // pointer to the MDT ID helper
	const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon
						     // detector manager
	const MuonCalib::IIdToFixedIdTool *m_id_tool; // identifier converter

// private methods //
	void createMaps(const MuonFixedId & id);
					// create the maps for the given
					// station id if they do not
					// exist
	void determineMispositioning(void); // determine the mispositioning of
					    // selected objects (tubes, layers,
					    // or multilayers)
	void writeOutGeometry(void); // method which writes the initial and
				     // final geometry to a text file

};

}

#endif

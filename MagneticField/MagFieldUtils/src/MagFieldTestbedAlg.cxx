/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MagFieldTestbedAlg.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
/*
 * Elmar Ritsch & Robert Langenberg
 *
 */
// class include
#include "MagFieldUtils/MagFieldTestbedAlg.h"

// Framework
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Chrono.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// MagneticField
#include "MagFieldInterfaces/IMagFieldSvc.h"

// G4includes
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4Field.hh"

// floating point precision
#include <limits>

//root
#include "TTree.h"
#include "TFile.h"

#include "TRandom3.h"

// performance test
#include <time.h>
#include <vector>
#include <cmath> // isnan

namespace {
const double solenoidRadius = 1075.;
const double solenoidZhalf = 2820.;
}

using std::isnan;

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructor
////////////////
MagField::MagFieldTestbedAlg::MagFieldTestbedAlg(const std::string& name,
		ISvcLocator* pSvcLocator) :
		::AthAlgorithm(name, pSvcLocator), m_magFieldSvc("AtlasFieldSvc",
				name), m_thistSvc("THistSvc", name), m_histStream(
				"MagFieldTestbedAlg"), m_chronoSvc("ChronoStatSvc", name), m_tree(0), m_treeName("field"), m_generateBox(false), m_minX(-11999.),
				m_halfX(11999.), m_halfY(11999.), m_halfZ(22999.), m_stepsX(200), m_stepsY(
				200), m_stepsZ(200), m_numberOfReadings(0), m_refFile(""), m_refTreeName(
				"field"), m_absTolerance(1e-7), m_relTolerance(0.01), m_xyzt(), m_field(), m_explicitX(
				0), m_explicitY(0), m_explicitZ(0), m_complete(false), m_useG4Field(
				false), m_recordReadings(true), m_onlyCheckSolenoid(
				false), m_coordsAlongBeam(false), m_explicitCoords(false) {
	// histogram service
	declareProperty("THistService", m_thistSvc, "The HistogramService");
	declareProperty("HistogramStreamName", m_histStream = "MagFieldTestbedAlg",
			"Name of the THistSvc output stream");

	// TTree object name
	declareProperty("ROOTTreeName", m_treeName = "field",
			"Name of the TTree object in the output file.");

	// boundaries for the magfield validation
	declareProperty("GenerateBox", m_generateBox = false, "generate a box centered in (0,0,0) using halfXYZ");
	declareProperty("MinX", m_minX = -11998., "minimum x for box generation");
	declareProperty("HalfX", m_halfX = 11998., "half-length along x-direction");
	declareProperty("HalfY", m_halfY = 11998., "half-length along y-direction");
	declareProperty("HalfZ", m_halfZ = 22998., "half-length along z-direction");

	// number of steps in each dimension (granularity)
	declareProperty("StepsX", m_stepsX = 200,
			"Number of steps along x-direction (granularity)");
	declareProperty("StepsY", m_stepsY = 200,
			"Number of steps along x-direction (granularity)");
	declareProperty("StepsZ", m_stepsZ = 200,
			"Number of steps along x-direction (granularity)");

	// validate field against a reference file:
	declareProperty("ReferenceFile", m_refFile = "",
			"Filename of a reference file to compare the output with");
	declareProperty("ReferenceTTreeName", m_refTreeName = "field",
			"TTree object name in the reference file");
	declareProperty("AbsTolerance", m_absTolerance = 1e-7,
			"Numerical tolerance when comparing against reference.");
	declareProperty("RelTolerance", m_relTolerance = 0.01,
			"Numerical tolerance when comparing against reference.");
	declareProperty("useG4Field", m_useG4Field);
	declareProperty("recordReadings", m_recordReadings = true);
	declareProperty("numberOfReadings", m_numberOfReadings = 0);
	declareProperty("useDerivatives", m_useDerivatives = false);
	declareProperty("onlyCheckSolenoid", m_onlyCheckSolenoid = false);
	declareProperty("coordsAlongBeam", m_coordsAlongBeam = false);
	declareProperty("explicitCoords", m_explicitCoords = false);
	declareProperty("explicitX", m_explicitX = 0.);
	declareProperty("explicitY", m_explicitY = 0.);
	declareProperty("explicitZ", m_explicitZ = 0.);

}

// Destructor
///////////////
MagField::MagFieldTestbedAlg::~MagFieldTestbedAlg() {
}

// Athena hook:
StatusCode MagField::MagFieldTestbedAlg::initialize() {
	ATH_MSG_INFO("entering initialize()...");

	if (m_chronoSvc.retrieve().isFailure()) {
	   ATH_MSG_ERROR("Could not find chrono svc");
	   return StatusCode::FAILURE;
	}

	if (!m_useG4Field) {
		//TODO: DUMMY reading to initialize newMagFieldSvc
		m_xyzt[3] = 0.;
		getFieldValue();
	}

	if (m_recordReadings || !(m_refFile.empty())) {
		// retrieve the histogram service
		if (m_thistSvc.retrieve().isSuccess()) {

			// Create the prefix of histogram names for the THistSvc
			std::string prefix = "/" + m_histStream + "/";

			// the ROOT tree
			m_tree = new TTree(m_treeName.c_str(), "Magnetic Field in Atlas");
			m_tree->Branch("pos", &m_xyzt, "x/D:y/D:z/D");
			m_tree->Branch("field", &m_field, "fx/D:fy/D:fz/D");

			// G4Field doesn't allow to get derivatives
			if (m_useDerivatives) {
				m_tree->Branch("derivatives", &m_deriv,
						"d1/D:d2/D:d3/D:d4/D:d5/D:d6/D:d7/D:d8/D:d9/D");
			}
			// register this ROOT TTree to the THistSvc
			if (m_thistSvc->regTree(prefix + m_treeName, m_tree).isFailure()) {
				ATH_MSG_ERROR("Unable to register TTree to THistSvc");
				return StatusCode::FAILURE;
			}
			// failure in THistSvc retrieve
		} else {
			ATH_MSG_ERROR("Unable to retrieve HistogramSvc");
			return StatusCode::FAILURE;
		}
	}

	m_referenceCount = 0;

	// success
	ATH_MSG_INFO("end of initialize()");
	return StatusCode::SUCCESS;
}

// Athena hook:
StatusCode MagField::MagFieldTestbedAlg::finalize() {
	ATH_MSG_INFO("entering finalize()...");

	ATH_MSG_INFO("end of finalize()");
	return StatusCode::SUCCESS;
}

// Athena hook:
StatusCode MagField::MagFieldTestbedAlg::execute() {

	// OUTPUT ONLY EXPLICIT COORDINATE THEN EXIT
	if (m_explicitCoords) {
		if (fetchEnvironment().isFailure()) {
			ATH_MSG_ERROR("Unable to fetch the magnetic field!");
			return StatusCode::FAILURE;
		}

		m_xyzt[0] = m_explicitX;
		m_xyzt[1] = m_explicitY;
		m_xyzt[2] = m_explicitZ;

		getFieldValue();

		ATH_MSG_INFO("Field Value at: " << m_explicitX << ", " << m_explicitY << ", " << m_explicitZ << "is: " << m_field[0] << ", " << m_field[1] << ", " << m_field[2] );

		m_complete = true;
	}

	// if field validation was not done yet -> run it
	if (!m_complete) {
		if (fetchEnvironment().isFailure()) {
			ATH_MSG_ERROR("Unable to fetch the magnetic field!");
			return StatusCode::FAILURE;
		}
// no reference file given?
		if (m_refFile.empty()) {
			//
			// create normal validation output file
			//

			// set fixed time (will not be written to the ROOT tree)

			std::vector < CLHEP::Hep3Vector > coordinatesToBeChecked;
			double maxRadius = m_halfX;
			double maxZ = m_halfZ;

			if (m_onlyCheckSolenoid) {
				maxRadius = solenoidRadius;
				maxZ = solenoidZhalf;
			}
			if (m_numberOfReadings != 0 && !m_coordsAlongBeam) {
				TRandom3 ran;

				if (!m_generateBox) {
                                   // generate inside a cylinder
                                   Chrono chrono(&(*m_chronoSvc), "MFTBcylGen");
                                   for (long i = 0; i < m_numberOfReadings; i++) {
                                   	CLHEP::Hep3Vector temp;
                                   
                                   	double maxRadius = m_halfX;
                                   	double maxZ = m_halfZ;
                                   
                                   	if (m_onlyCheckSolenoid) {
                                   		maxRadius = solenoidRadius;
                                   		maxZ = solenoidZhalf;
                                   	}
                                   	temp.setX(ran.Uniform(maxRadius * (-1), maxRadius));
                                   	temp.setY(ran.Uniform(maxRadius * (-1), maxRadius));
                                   	temp.setZ(ran.Uniform(maxZ * (-1), maxZ));
                                   	//only generate values within radius < 11.998; for onlyCheckSolenoid radius < 1075
                                   
                                   	double radius = sqrt(
                                   			pow(temp.getX(), 2) + pow(temp.getY(), 2));
                                   	if (radius > m_halfX) {
                                   		i--;
                                   		continue;
                                   	}
                                   	coordinatesToBeChecked.push_back(temp);
                                   }
				} else {
                                   // generate inside a box
                                   Chrono chrono(&(*m_chronoSvc), "MFTBboxGen");
                                   for (long i = 0; i < m_numberOfReadings; i++) {
                                   	CLHEP::Hep3Vector temp;
                                   
                                   	double maxRadius = sqrt(pow(m_halfX, 2) + pow(m_halfY,2));
                                   	double maxZ = m_halfZ;
                                   
                                   	if (m_onlyCheckSolenoid) {
                                   		maxRadius = solenoidRadius;
                                   		maxZ = solenoidZhalf;
                                   	}
                                   	temp.setX(ran.Uniform(m_minX, m_halfX)); // TODO: tidy this up...
                                   	temp.setY(ran.Uniform(m_halfY * (-1), m_halfY));
                                   	temp.setZ(ran.Uniform(maxZ * (-1), maxZ));
                                   	//only generate values within radius < 11.998; for onlyCheckSolenoid radius < 1075
                                   
                                   	double radius = sqrt(
                                   			pow(temp.getX(), 2) + pow(temp.getY(), 2));
                                   	if (radius > maxRadius) {
                                   		i--;
                                   		continue;
                                   	}
                                   	coordinatesToBeChecked.push_back(temp);
                                   }
				}
			}

			if (m_coordsAlongBeam) {

				TRandom3 rand;
				CLHEP::Hep3Vector temp;
				double r, phi, theta;

				for (long i = 0; i < m_numberOfReadings;) {
					r = 0.;
					phi = rand.Uniform(3.14 * (-1), 3.14);
					theta = rand.Uniform(0, 3.14);
					while (r < maxRadius) {
						temp.setX(r * sin(theta) * cos(phi));
						temp.setY(r * sin(phi) * sin(theta));
						temp.setZ(r * cos(phi));
						r += 10.;
						i++;
						coordinatesToBeChecked.push_back(temp);
					}
				}
			}

			// initialize performance timer
			time_t seconds;
			seconds = time(NULL);

			if (m_numberOfReadings != 0) {

				for (int i = 0; i < m_numberOfReadings; i++) {
					m_xyzt[0] = coordinatesToBeChecked.at(i).getX();
					m_xyzt[1] = coordinatesToBeChecked.at(i).getY();
					m_xyzt[2] = coordinatesToBeChecked.at(i).getZ();
					//TODO: delete random coordinates, this segfaults
//					delete &randomCoordinates.at(i);
					getFieldValue();
					if (m_recordReadings) {
						m_tree->Fill();
					}
				}
			} else {
				// loop over x
				for (int stepx = 0; stepx < m_stepsX; stepx++) {
					m_xyzt[0] = -maxRadius + (2 * maxRadius) / m_stepsX * stepx;

					// loop over y
					for (int stepy = 0; stepy < maxRadius; stepy++) {
						m_xyzt[1] = -maxRadius
								+ (2 * maxRadius) / maxRadius * stepy;

						// loop over z
						for (int stepz = 0; stepz < maxZ; stepz++) {

							m_xyzt[2] = -maxZ + (2 * maxZ) / m_stepsZ * stepz;

							// compute the field
							getFieldValue();

							// fill the ROOT Tree
							if (m_recordReadings) {

								m_tree->Fill();
							}
						}
					}
				}
			}
			seconds = time(NULL) - seconds;

			if (m_numberOfReadings != 0) {
				ATH_MSG_INFO(
						"the reading of " << m_numberOfReadings
								<< " random readings took " << seconds
								<< "seconds.");
			} else {
				ATH_MSG_INFO(
						"the reading of " << m_stepsX * m_stepsY * m_stepsZ
								<< " values in a grid took " << seconds
								<< "seconds.");
			}
		}
		// reference file given
		else {
			//
			// validate against reference file and create output file with positions
			// from this file
			//

			ATH_MSG_INFO(
					"will now run comparison against given reference file: "
							<< m_refFile);
			if (checkWithReference() == true)
				ATH_MSG_INFO("comparison against reference file successful!");
			else {
				ATH_MSG_ERROR("comparison against reference file FAILED!!!!");
				return StatusCode::FAILURE;
			}
		}

		// never run the validation again
		m_complete = true;
	}

	return StatusCode::SUCCESS;
}

StatusCode MagField::MagFieldTestbedAlg::fetchEnvironment() {
	if (m_useG4Field) {
		// use the transportation manager singleton to access the p_g4field
		G4TransportationManager *transm =
				G4TransportationManager::GetTransportationManager();
		G4FieldManager *fieldm = (transm) ? transm->GetFieldManager() : 0;
		p_g4field = (fieldm) ? fieldm->GetDetectorField() : 0;

		if (!p_g4field) {
			return StatusCode::FAILURE;
		}
  }
	return StatusCode::SUCCESS;
}

void MagField::MagFieldTestbedAlg::getFieldValue() {
	if (m_useG4Field) {
		p_g4field->GetFieldValue(m_xyzt, m_field);
	} else {
		// use new magnetic field service
		m_magFieldSvc->getField(m_xyzt, m_field,
				m_useDerivatives ? m_deriv : 0);
	}
	if (isnan(m_deriv[0]) || isnan(m_deriv[1]) || isnan(m_deriv[2])
			|| isnan(m_deriv[3]) || isnan(m_deriv[4]) || isnan(m_deriv[5])
			|| isnan(m_deriv[6]) || isnan(m_deriv[7]) || isnan(m_deriv[8])) {
		ATH_MSG_INFO(
				"at least one Derivative was NaN at: " << m_xyzt[0] << ", "
						<< m_xyzt[1] << ", " << m_xyzt[2]);
	}
}

// compare local field with reference file ROBERT: HISTOGRAM
bool MagField::MagFieldTestbedAlg::checkWithReference() {

	// setup the reference TTree
	TFile *refF = TFile::Open(m_refFile.c_str());
	TTree *refT = (refF) ? (TTree*) refF->Get(m_refTreeName.c_str()) : 0;

	//comparison with Masahiro's reference file
//	std::string temp = "scan";
//	TTree *refT = (refF) ? (TTree*) refF->Get(temp.c_str()) : 0;

	if (!refT) {
		ATH_MSG_ERROR(
				"Unable to read the tree '" << m_refTreeName << "'"
						<< "from file: " << m_refFile);
		return false;
	}
	// enable all input branches
	refT->SetBranchStatus("*", 1);

	// setup the reference file branches
	for (int i = 0; i < 4; i++)
		m_xyzt[i] = 0.;
	static double refField[3] = { 0., 0., 0. };
	static double refDerivatives[9] = { 0., 0., 0., 0., 0., 0., 0., 0., 0. };

	refT->SetBranchAddress("pos", &m_xyzt);
	refT->SetBranchAddress("field", &refField);
	if (m_useDerivatives) {
		refT->SetBranchAddress("derivatives", &refDerivatives);
	}

	// number of reference TTree entries
	Long64_t nentries = refT->GetEntries();

	// setup the diff TTree
//	TTree *diffT = new TTree("fieldDiff",
//			"Magnetic Field Differences in AtlasG4");
	static double fieldAbsDiff;
	static double fieldRelDiff;
	//
	m_tree->Branch("ReferenceField", &refField, "x/D:y/D:z/D");
	m_tree->Branch("fieldAbsDiff", &fieldAbsDiff, "diff/D");
	m_tree->Branch("fieldRelDiff", &fieldRelDiff, "diff/D");

	if (m_useDerivatives) {
		m_tree->Branch("ReferenceDerivatives", &refDerivatives,
				"d1/D:d2/D:d3/D:d4/D:d5/D:d6/D:d7/D:d8/D:d9/D");
	}

	// register the diff ROOT TTree to the THistSvc
//	std::string prefix = "/" + m_histStream + "/";
//	if (m_thistSvc->regTree(prefix + "fieldDiff", diffT).isFailure()) {
//		ATH_MSG_ERROR("Unable to register TTree to THistSvc");
//		return false;
//	}

	// to keep track of the errors:
	double biggestAbsDiff = 0.;
	double biggestRelDiff = 0.;

	// loop over all TTree entries
	bool pass = true;
	for (Long64_t i = 0; i < nentries; i++) {
		// read the next entry in the reference ttree
		//  -> fill the m_xyzt and refField variables
		refT->GetEntry(i);

		// compute the field at the position given
		// from the reference file
		getFieldValue();

		// compute the difference
		double mFieldTotal = 0;
		double refFieldTotal = 0;
		for (int i = 0; i < 3; i++) {
			mFieldTotal += m_field[i] * m_field[i];
			refFieldTotal += refField[i] * refField[i];
		}
		mFieldTotal = sqrt(mFieldTotal);
		refFieldTotal = sqrt(refFieldTotal);

		fieldAbsDiff = fabs(mFieldTotal - refFieldTotal);
		if (refFieldTotal != 0) {
			fieldRelDiff = fieldAbsDiff / refFieldTotal;
		}
		if (m_useDerivatives) {

			if (fabs((m_deriv[0] - refDerivatives[0]) / refDerivatives[0])
					> m_relTolerance
					|| fabs(
							(m_deriv[1] - refDerivatives[1])
									/ refDerivatives[1]) > m_relTolerance
					|| fabs(
							(m_deriv[2] - refDerivatives[2])
									/ refDerivatives[2]) > m_relTolerance
					|| fabs(
							(m_deriv[3] - refDerivatives[3])
									/ refDerivatives[3]) > m_relTolerance
					|| fabs(
							(m_deriv[4] - refDerivatives[4])
									/ refDerivatives[4]) > m_relTolerance
					|| fabs(
							(m_deriv[5] - refDerivatives[5])
									/ refDerivatives[5]) > m_relTolerance
					|| fabs(
							(m_deriv[6] - refDerivatives[6])
									/ refDerivatives[6]) > m_relTolerance
					|| fabs(
							(m_deriv[7] - refDerivatives[7])
									/ refDerivatives[7]) > m_relTolerance
					|| fabs(
							(m_deriv[8] - refDerivatives[8])
									/ refDerivatives[8]) > m_relTolerance) {
				ATH_MSG_INFO(
						"referenceDerivatives 1-9: " << refDerivatives[0]
								<< ", " << refDerivatives[1] << ", "
								<< refDerivatives[2] << ", "
								<< refDerivatives[3] << ", "
								<< refDerivatives[4] << ", "
								<< refDerivatives[5] << ", "
								<< refDerivatives[6] << ", "
								<< refDerivatives[7] << ", "
								<< refDerivatives[8]);
				ATH_MSG_INFO(
						"derivatives 1-9:          " << m_deriv[0] << ", "
								<< m_deriv[1] << ", " << m_deriv[2] << ", "
								<< m_deriv[3] << ", " << m_deriv[4] << ", "
								<< m_deriv[5] << ", " << m_deriv[6] << ", "
								<< m_deriv[7] << ", " << m_deriv[8]);
				ATH_MSG_INFO(
						"relative differences: "
								<< ((m_deriv[0] - refDerivatives[0])
										/ refDerivatives[0]) << ", "
								<< ((m_deriv[1] - refDerivatives[1])
										/ refDerivatives[1]) << ", "
								<< ((m_deriv[2] - refDerivatives[2])
										/ refDerivatives[2]) << ", "
								<< ((m_deriv[3] - refDerivatives[3])
										/ refDerivatives[3]) << ", "
								<< ((m_deriv[4] - refDerivatives[4])
										/ refDerivatives[4]) << ", "
								<< ((m_deriv[5] - refDerivatives[5])
										/ refDerivatives[5]) << ", "
								<< ((m_deriv[6] - refDerivatives[6])
										/ refDerivatives[6]) << ", "
								<< ((m_deriv[7] - refDerivatives[7])
										/ refDerivatives[7]) << ", "
								<< ((m_deriv[8] - refDerivatives[8])
										/ refDerivatives[8]));
				ATH_MSG_INFO(
						"coordinates: " << m_xyzt[0] << ", " << m_xyzt[1]
								<< ", " << m_xyzt[2]);
				m_referenceCount++;

			}
		}

		// check if difference is smaller than one Gauss or if greater if < 1%
		// only pass if below tolerances
		if (fieldAbsDiff > m_absTolerance && fieldRelDiff > m_relTolerance) {
//			if (pass) {
			ATH_MSG_INFO(
					"local reading" << m_field[0] << " " << m_field[1] << " "
							<< m_field[2]);
			ATH_MSG_INFO(
					"reference reading" << refField[0] << " " << refField[1]
							<< " " << refField[2]);
			ATH_MSG_INFO(
					"coordinates: " << m_xyzt[0] << ", " << m_xyzt[1] << ", "
							<< m_xyzt[2]);
//			}
			pass = false;
		}
		if (fieldAbsDiff > biggestAbsDiff) {
			biggestAbsDiff = fieldAbsDiff;
		}
		if (fieldRelDiff > biggestRelDiff) {
			biggestRelDiff = fieldRelDiff;
		}

		// fill the magField ROOT Tree (using: m_xyzt, m_field)
		m_tree->Fill();

		// fill the diffT ROOT Tree (using: m_xyzt, refField)
//		diffT->Fill();
	}

	// close the reference file
	refF->Close();

	ATH_MSG_INFO("number of values unequal: " << m_referenceCount);
	ATH_MSG_INFO("number of readings from reference file: " << nentries);

	ATH_MSG_INFO(
			std::setprecision(20)
					<< "1) biggest absolute difference in the mag field comparison: "
					<< biggestAbsDiff << " (tolerance: " << m_absTolerance
					<< ")." << endmsg << "2) biggest relative difference: "
					<< biggestRelDiff << " (tolerance: " << m_relTolerance
					<< ")" << std::setprecision(-1));

	// tests passed
	return pass;
}

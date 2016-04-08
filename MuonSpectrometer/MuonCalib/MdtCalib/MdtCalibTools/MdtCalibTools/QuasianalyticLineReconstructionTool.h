/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.03.2006, AUTHOR: OLIVER KORTNER
// Modified: 15.07.2006 by O. Kortner, interface to additional functionality
//                      of QuasianalyticLineReconstruction added.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef QuasianalyticLineReconstructionToolH
#define QuasianalyticLineReconstructionToolH

//:::::::::::::::::::::::::::::::::::::::::::::::
//:: CLASS QuasianalyticLineReconstructionTool ::
//:::::::::::::::::::::::::::::::::::::::::::::::

/// \class QuasianalyticLineReconstructionTool
/// Tool to create an instance of the IMdtSegmentFitter implementation in
/// QuasianalyticLineReconstruction.
/// The user can specify the road width of the pattern recognition in the
/// job-options file by the setting "roadWidth" to the desired value.
/// The user can also request a chi^2 refit at the end of the quasianalytic
/// segment reconstruction.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 15.07.2006

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// Gaudi //
#include "AthenaBaseComps/AthAlgTool.h"

// MuonCalib //
#include "MdtCalibITools/IMdtSegmentFitterTool.h"
#include "MdtCalibInterfaces/IMdtSegmentFitter.h"


namespace MuonCalib {
  
class QuasianalyticLineReconstructionTool : 
			public AthAlgTool, virtual public IMdtSegmentFitterTool {
  
public:
// Constructors //
	QuasianalyticLineReconstructionTool(const std::string & type,
	                    const std::string & name, const IInterface *parent);
	///< AlgTool constructor

	virtual ~QuasianalyticLineReconstructionTool(){};
	///< Virtual destructor

// Methods //
	StatusCode initialize();
	                                ///< AlgTool initialize method
	StatusCode finalize();
	                                ///< AlgTool finalize method
 	IMdtSegmentFitter* getImp() { return m_implementation; }
	                                ///< Interface method: return the
	                                ///< implementation pointer

private:
	double m_road_width; // road width used in the pattern recognition of
	                     // QuasianalyticLineReconstruction
	bool m_refit; // enable the chi^2 refit at the end of the quasianalytic
	              // segment reconstruction
	IMdtSegmentFitter *m_implementation; // pointer to the concrete
	                                     // implementation
 
};

}

#endif

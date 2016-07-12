/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 02.02.2009, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_BFieldCorrectionUpdateToolH
#define MuonCalib_BFieldCorrectionUpdateToolH

//::::::::::::::::::::::::::::::::::::::
//:: CLASS BFieldCorrectionUpdateTool ::
//::::::::::::::::::::::::::::::::::::::

/// \class BFieldCorrectionUpdateTool
///
/// This class can be used to add the Lorentz angle effect to simulated drift
/// times for Monte-Carlo data without the Lorentz angle effect. The main
/// purpose of this class is to test the Lorentz angle correction in combination
/// with calibration algorithms.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 02.02.2009

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::


// MuonCalib //
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"
#include "MdtCalibData/BFieldCorFunc.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace MuonCalib {

class BFieldCorrectionUpdateTool : public AthAlgTool, virtual public CalibSegmentPreparationTool {
  public:
    // Constructor //
    BFieldCorrectionUpdateTool(const std::string &t, const std::string &n, const IInterface *p);
    ///< Constructor of the tool.

    ~BFieldCorrectionUpdateTool(void);
    ///< Destructor of the tool.

// Methods //
// methods required by the base classes //
    StatusCode initialize(void);
    ///< initialization of the tool
    StatusCode finalize(void);
                            ///< finalization of the tool
    void prepareSegments(
      const MuonCalibEvent *& /*event*/,
      std::map<NtupleStationId, MuonCalibSegment *> &segments);
    ///< Method to add the Lorentz effect to the
    ///< measured drift times.

  private:
    std::string m_rt_file_name; // name of the r-t files to be used for the
                                 // B field correction
    IRtRelation *m_rt; // pointer to the reference r-t relationship at B=0
    BFieldCorFunc *m_corr_func; // pointer to the B field correction function

    bool initialize_BFieldCorFunc(void); // method to initialize the Lorentz angle correction

};

}  //namespace MuonCalib
#endif

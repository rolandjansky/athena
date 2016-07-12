/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 02.02.2009, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF THE CONSTRUCTORS AND METHODS DEFINED IN THE CLASS ::
//::                      BFieldCorrectionUpdateTool                     ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>

// MuonCalib //
#include "MdtCalibData/RtSpline.h"
#include "CalibNtupleAnalysisAlg/BFieldCorrectionUpdateTool.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::
BFieldCorrectionUpdateTool::BFieldCorrectionUpdateTool(
                                    const std::string &t,
                                    const std::string &n,
                                    const IInterface *p) : AthAlgTool(t, n, p) {

/////////////////
// JOB OPTIONS //
/////////////////
  m_rt_file_name = string("truth_rt.dat");
  declareProperty("rtFile", m_rt_file_name);

///////////////////////
// DECLARE INTERFACE //
///////////////////////
  declareInterface< CalibSegmentPreparationTool >(this);

////////////////////
// RESET POINTERS //
////////////////////
  m_rt = 0;
}

//*****************************************************************************

//::::::::::::::::
//:: DESTRUCTOR ::
//::::::::::::::::
BFieldCorrectionUpdateTool::~BFieldCorrectionUpdateTool(void) {
}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD initialize ::
//:::::::::::::::::::::::
StatusCode BFieldCorrectionUpdateTool::initialize(void) {

/////////////
// OBJECTS //
/////////////

/////////////
// MESSAGE //
/////////////
  ATH_MSG_INFO( "Initializing tool..." );

/////////////////////////////////////////////////////////////////
// READ r-t RELATIONSHIP AND INITIALIZE THE B-FIELD CORRECTION //
/////////////////////////////////////////////////////////////////
  if (!initialize_BFieldCorFunc()) {
    ATH_MSG_FATAL( "Could not initialize the magnetic field correction!" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}  //end BFieldCorrectionUpdateTool::initialize

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::
StatusCode BFieldCorrectionUpdateTool::finalize(void) {

/////////////
// OBJECTS //
/////////////

/////////////
// MESSAGE //
/////////////
  ATH_MSG_INFO( "Finalizing tool..." );

  return StatusCode::SUCCESS;
}

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD prepareSegments ::
//::::::::::::::::::::::::::::
void BFieldCorrectionUpdateTool::prepareSegments(
                    const MuonCalibEvent *& /*event*/,
                    std::map<NtupleStationId, MuonCalibSegment *> &segments) {

//////////////////////////////////////////////////////////////////////////////
// LOOP OVER THE SEGMENTS AND THEIR HITS TO ADD THE LORENTZ ANGLE EFFECT TO //
// THE MEASURED DRIFT TIMES                                                 //
//////////////////////////////////////////////////////////////////////////////
  for (std::map<NtupleStationId, MuonCalibSegment *> :: iterator it=
	 segments.begin(); it!= segments.end(); it++) {

    for (unsigned int k=0; k<it->second->mdtHOT().size(); k++) {
      MdtCalibHitBase *hit = it->second->mdtHOT()[k];
      hit->setDriftTime(hit->driftTime() +
        m_corr_func->correction_to_B(hit->driftTime(), hit->bFieldPara(), hit->bFieldPerp()));
    }

  }

  return;
}   //end BFieldCorrectionUpdateTool::prepareSegments

//*****************************************************************************

//:::::::::::::::::::::::::::::::::::::
//:: METHOD initialize_BFieldCorFunc ::
//:::::::::::::::::::::::::::::::::::::
bool BFieldCorrectionUpdateTool::initialize_BFieldCorFunc(void) {

/////////////
// OBJECTS //
/////////////
  ifstream rt_file;
  char s[255]; // auxiliary character pointer
  double dummy; // auxiliary double
  vector<double> r,t; // entries in the r-t table
  vector<double> B_corr_param(2);

//////////////////////////////////////////////
// OPEN THE r-t FILE AND READ THE r-t TABLE //
//////////////////////////////////////////////

// check that the file exists //
  rt_file.open(m_rt_file_name.c_str());
  if (rt_file.fail()) {
    ATH_MSG_FATAL( "Could not open file '" << m_rt_file_name << "'!" );
    return false;
  }

// read the file //
  rt_file.getline(s, 255);

  while (!rt_file.eof()) {
    rt_file >> dummy;
    if (rt_file.eof()) {
      break;
    }
    r.push_back(dummy);
    rt_file >> dummy; t.push_back(dummy);
    rt_file >> dummy;
  }

// create a spline r-t relationship //
  vector<double> param(2*r.size());
  for (unsigned int k=0; k<r.size(); k++) {
    param[2*k] = t[k];
    param[2*k+1] = r[k];
  }

  m_rt = new RtSpline(param);

////////////////////////////////////////////
// CREATE THE B FIELD CORRECTION FUNCTION //
////////////////////////////////////////////
  B_corr_param[0] = 3080.0;
  B_corr_param[1] = 0.11;

  m_corr_func = new BFieldCorFunc("high", B_corr_param, m_rt);
  
    return true;
}  //end BFieldCorrectionUpdateTool::initialize_BFieldCorFunc

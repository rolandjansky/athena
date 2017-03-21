/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////
//                                  //
//  Class: TRT_SimDriftTimeTool     //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
////////////////////////////////////

#include "TRT_SimDriftTimeTool.h"

#include "TRT_BarrelDriftTimeData.h"
#include "TRT_ScaledDriftTimeData.h"

#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/ServiceHandle.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h"

TRT_SimDriftTimeTool::TRT_SimDriftTimeTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AthAlgTool( type, name, parent )
  , m_minDistance( 0*CLHEP::mm )
  , m_maxDistance( 2.0001*CLHEP::mm )
  , m_maxField ( -999 )
  , m_maxFieldSquaredLimit(999)
  , m_invMaxFieldSquared(0.0)
  , m_digversion(0)
  , m_nTabulatedDistances(4000)//4000 means resolution of 0.5micron and 96KB of memory.
  , m_invDeltaTabulatedDistances( (m_nTabulatedDistances-1)/(m_maxDistance-m_minDistance) )
{
  // declare my special interface
  declareInterface<ITRT_SimDriftTimeTool>(this);
}

//__________________________________________________________________________
StatusCode TRT_SimDriftTimeTool::initialize()
{

  ATH_MSG_INFO("Initializing TRT_SimDriftTimeTool.");

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //             Get dig. version through            //
  //             the TRT Detector Manager            //
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  // Get the TRT Detector Manager
  const InDetDD::TRT_DetectorManager * detmgr;
  ATH_CHECK(detStore()->retrieve(detmgr,"TRT"));
  ATH_MSG_INFO("Retrieved TRT_DetectorManager with version " << detmgr->getVersion().majorNum());

  m_digversion = detmgr->digitizationVersion();

  if (m_digversion<11)
    {
      ATH_MSG_FATAL("digversion < 11 (" << m_digversion << ") is no longer supported. The job will die now :(");
      return StatusCode::FAILURE;
    }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //              Select DriftTime Data              //
  //            ( based on dig. version )            //
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  std::vector<ITRT_DriftTimeData*> pDTData;
  pDTData.push_back(new TRT_BarrelDriftTimeData(m_digversion,0)); // Xe straws
  pDTData.push_back(new TRT_BarrelDriftTimeData(m_digversion,1)); // Kr straws
  pDTData.push_back(new TRT_BarrelDriftTimeData(m_digversion,2)); // Ar straws

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //          Make lookup tables of the data         //
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  m_maxField = pDTData[0]->MaxTabulatedField();
  m_maxFieldSquaredLimit = 1.1*m_maxField*1.1*m_maxField;
  m_invMaxFieldSquared = 1.0/(m_maxField*m_maxField);

  for (unsigned int i=0; i<pDTData.size(); i++)
    {

      // Tabulate mean drifttimes at no and max field.
      // Fill different tables for Xe, Kr and Ar-based gas mixtures
      std::vector<double> _m_table_of_dist2meanDT_at_noField;
      std::vector<double> _m_table_of_dist2meanDT_at_maxField;

      _m_table_of_dist2meanDT_at_noField.resize(m_nTabulatedDistances);
      _m_table_of_dist2meanDT_at_maxField.resize(m_nTabulatedDistances);

      for (unsigned int distIndex = 0; distIndex < m_nTabulatedDistances; distIndex++)
        {
          double distance = ((m_maxDistance-m_minDistance)*distIndex)/(m_nTabulatedDistances-1.0)+m_minDistance;
          if (distIndex == m_nTabulatedDistances-1) distance = m_maxDistance;//to avoid a numerical mistake.
          _m_table_of_dist2meanDT_at_noField[distIndex]  = pDTData[i]->DriftTimeAtNoField(distance);
          _m_table_of_dist2meanDT_at_maxField[distIndex] = pDTData[i]->DriftTimeAtMaxField(distance);
        }

      m_table_of_dist2meanDT_at_noField.push_back(_m_table_of_dist2meanDT_at_noField);
      m_table_of_dist2meanDT_at_maxField.push_back(_m_table_of_dist2meanDT_at_maxField);

    }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  return StatusCode::SUCCESS;

}

//__________________________________________________________________________
// Don't worry; physical checks on "dist" and "effectivefield_squared" are already performed in TRTProcessingOfStraw.cxx
double TRT_SimDriftTimeTool::getAverageDriftTime( const double& dist,
                                                  const double& effectivefield_squared,
                                                  int strawGasType) const
{

  if (effectivefield_squared>m_maxFieldSquaredLimit)
    { //a little extrapolation is ok
      ATH_MSG_WARNING ("Extrapolation from tabulated field values greater than 10% (at "
                       << std::sqrt(effectivefield_squared)/CLHEP::tesla<<" Tesla)");
    }

  const unsigned int distIndex(static_cast<int>( m_invDeltaTabulatedDistances*(dist-m_minDistance) ));

  if ( distIndex >= m_nTabulatedDistances)
    {
      ATH_MSG_FATAL("getAverageDriftTime: Somehow distIndex (" << distIndex << " ) is out of bounds ("
                    << m_nTabulatedDistances << "). The job will die now.");
      throw;
    }

  const double C(m_table_of_dist2meanDT_at_noField[strawGasType][distIndex]);
  const double A((m_table_of_dist2meanDT_at_maxField[strawGasType][distIndex] - C) * m_invMaxFieldSquared);

  return (A * effectivefield_squared) + C;

}

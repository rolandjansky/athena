/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/



#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CaloGeoHelpers/proxim.h"
#include "CaloEvent/CaloRecoStatus.h"
#include "CaloEvent/CaloCluster.h"

#include "CaloClusterCellWeightCalib.h"

#include <cstdlib>

std::string const CaloClusterCellWeightCalib::m_posName = "Signal";
std::string const CaloClusterCellWeightCalib::m_absName = "AbsSignal";
std::string const CaloClusterCellWeightCalib::m_rawName = "RawSignal"; 

std::string const CaloClusterCellWeightCalib::m_defName = m_absName;

//////////////////////////////
// Constructor & Destructor //
//////////////////////////////

CaloClusterCellWeightCalib::
CaloClusterCellWeightCalib(const std::string& type,
			   const std::string& name,
			   const IInterface* pParent)
  : CaloClusterProcessor(type,name,pParent)
  , m_directionCalculation(m_defName)
  , m_calibNoiseLikeAll(true)
  , m_noiseDirectionCalculation(m_defName)
  , m_eThreshold(0.)
  , m_ignoreGeoWghts(false)
  , m_cellWeight(this)
  , m_calc(nullptr)
  , m_calc_noise(nullptr)
{
  declareProperty("Direction",                m_directionCalculation);
  declareProperty("BelowThresholdLikeAll",    m_calibNoiseLikeAll);
  declareProperty("BelowThresholdDirection",  m_noiseDirectionCalculation);
  declareProperty("EnergyThreshold",          m_eThreshold);
  declareProperty("IgnoreGeoWeights",         m_ignoreGeoWghts);
  declareProperty("CellSignalWeightTool",     m_cellWeight);
}

CaloClusterCellWeightCalib::~CaloClusterCellWeightCalib()
{ }

////////////////////
// Initialization //
////////////////////

StatusCode CaloClusterCellWeightCalib::initialize()
{
  MsgStream report(msgSvc(),name());

  // configure direction calculations
  StatusCode checkOut = m_calibNoiseLikeAll
    ? this->setupAll(report)  // all clusters the same
    : this->setupSpc(report); // dedicated noise cluster treatment
  if ( checkOut.isFailure() )
    {
      report << MSG::ERROR
	     << "failed to configure direction calculations."
	     << endmsg;
      return StatusCode::FAILURE;
    }

  // retrieve tool
  if ( m_cellWeight.empty() || (m_cellWeight.retrieve()).isFailure() )
    {
      report << MSG::ERROR
	     << "*** configuration insufficient *** "
	     << "no hadronic cell calibration tool configured"
	     << endmsg;
      return StatusCode::FAILURE;
    }

  //
  return StatusCode::SUCCESS;
}

///////////////
// Execution //
///////////////

StatusCode
CaloClusterCellWeightCalib::execute(const EventContext& /*ctx*/,
                                    xAOD::CaloCluster* pClus) const
{

  // retrieve raw cluster signals  
  double clusterE(pClus->rawE());
  double clusterEta(pClus->rawEta());
  double clusterPhi(pClus->rawPhi());

 //   report << MSG::INFO << "<UNCALIBRATED> cluster kinematics ("
 // 	 << pClus->e() << ","
 //  	 << pClus->eta() << ","
 // 	 << pClus->phi() << ")" << endmsg;

  CaloClusterChangeSignalState chss(pClus,xAOD::CaloCluster::ALTCALIBRATED);

  pClus->setE(clusterE);
  pClus->setEta(clusterEta);
  pClus->setPhi(clusterPhi);

  //   report << MSG::INFO << "<ALTCALIBRATED> cluster kinematics, initial ("
  // 	 << pClus->e() << "/" << clusterE << ","
  // 	 << pClus->eta() << "/" << clusterEta << ","
  // 	 << pClus->phi() << "/" << clusterPhi << ")" << endmsg;

  // calculate new 4-vector
  StatusCode checkOut(StatusCode::SUCCESS);
  if ( m_calibNoiseLikeAll ) 
    {
      checkOut = (this->*m_calc)(pClus);
    }
  else
    {
      checkOut = pClus->e() > 0. 
	? (this->*m_calc)(pClus) : (this->*m_calc_noise)(pClus);
    }

  //   report << MSG::INFO << "<ALTCALIBRATED> cluster kinematics, final ("
  // 	 << pClus->e() << ","
  // 	 << pClus->eta() << ","
  // 	 << pClus->phi() << ")" << endmsg;

  // check result
  if ( checkOut.isFailure() )
    {
      msg(MSG::WARNING)
	     << "problem in calculation of cell weighted signal state"
	     << endmsg;
      return StatusCode::SUCCESS;
    }
  else
    {
      // change reco status
      pClus->recoStatus().setStatus(CaloRecoStatus::CALIBRATEDALT);
      return checkOut;
    }
}

/////////////////////////
// Internal Use: Setup //
/////////////////////////

StatusCode CaloClusterCellWeightCalib::setupAll(MsgStream& report)
{
  std::string conf;
  std::string tag("all");
  return this->setup(m_directionCalculation,tag,m_calc,conf,report)
    ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode CaloClusterCellWeightCalib::setupSpc(MsgStream& report)
{
  std::string conf;
  std::string aboveTag("above threshold");
  std::string belowTag("below threshold");
  return 
    this->setup(m_directionCalculation,aboveTag,m_calc,conf,report) &&
    this->setup(m_noiseDirectionCalculation,belowTag,m_calc_noise,conf,report)
    ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

bool CaloClusterCellWeightCalib::setup(const std::string& name,
				       const std::string& tag,
				       CALCULATOR&        calc,
				       std::string&       conf,
				       MsgStream&         report)
{
  if ( CaloClusterCellWeightCalib::cmpNoCase(name,m_posName) )
    {
      report << MSG::INFO
	     << "cluster direction ("
	     << tag
	     << ") from positive cells only"
	     << endmsg;
      conf = name;
      calc = m_ignoreGeoWghts 
	? &CaloClusterCellWeightCalib::f_dirPosNW
	: &CaloClusterCellWeightCalib::f_dirPos;
      return true;
    }
  else if ( CaloClusterCellWeightCalib::cmpNoCase(name,m_absName) )
    {
      report << MSG::INFO
	     << "cluster direction ("
	     << tag 
	     << ") from absolute cell signals"
	     << endmsg;
      conf = name;
      calc = m_ignoreGeoWghts 
	? &CaloClusterCellWeightCalib::f_dirAbsNW
	: &CaloClusterCellWeightCalib::f_dirAbs;
      return true;
    }
  else if ( CaloClusterCellWeightCalib::cmpNoCase(name,m_rawName) )
    {
      report << MSG::INFO
	     << "cluster direction ("
	     << tag
	     << ") from raw signals (unchanged by this tool)"
	     << endmsg;
      conf = name;
      calc = m_ignoreGeoWghts 
	? &CaloClusterCellWeightCalib::f_dirRawNW
	: &CaloClusterCellWeightCalib::f_dirRaw;
      return true;
    }
  else
    {
      report << MSG::WARNING
	     << "invalid configuration, use default!"
	     << endmsg;
      return this->setup(m_defName,tag,calc,conf,report);
    }
}

bool CaloClusterCellWeightCalib::cmpNoCase(const std::string& a,
					   const std::string& b)
{
  // same length required
  if ( a.length() != b.length() ) return false;
  // adapted from Stroustrup, The C++ Programming Language, Special
  //  Edition, (2001 printing), p 591.
  std::string::const_iterator p1(a.begin());
  std::string::const_iterator p2(b.begin());
  while ( p1 != a.end() && p2 != b.end() )
    {
      if ( std::toupper(*p1) != std::toupper(*p2) ) return false;
      ++p1;
      ++p2;
    }
  return true;
}

/////////////////////////////////////////
// Internal Use: Direction Calculators //
/////////////////////////////////////////

// -- keep em scale direction, use geometrical cell signal weights
StatusCode CaloClusterCellWeightCalib::f_dirRaw(xAOD::CaloCluster* pClus) const
{
  // loop cells in clusters
  double eCal(0.);
  xAOD::CaloCluster::cell_iterator fCell(pClus->cell_begin());
  xAOD::CaloCluster::cell_iterator lCell(pClus->cell_end());
  for ( ; fCell != lCell; fCell++ )
    {
      const CaloCell* cell = *fCell;
      // 
      eCal += 
	fCell.weight()     * 
	cell->e()                   *
        m_cellWeight->wtCell(cell);
    }
  // set cluster kinematics (cluster is in ALTCALIBRATED state
  pClus->setE(eCal);
  //
  return StatusCode::SUCCESS;
}
// -- keep em scale direction, ignore geometrical cell signal weights
StatusCode CaloClusterCellWeightCalib::f_dirRawNW(xAOD::CaloCluster* pClus) const
{
  // loop cells in clusters
  double eCal(0.);
  for (const CaloCell* cell : *pClus)
    {
      // 
      eCal += 
	cell->e()                   *
        m_cellWeight->wtCell(cell);
    }
  // set cluster kinematics (cluster is in ALTCALIBRATED state
  pClus->setE(eCal);
  //
  return StatusCode::SUCCESS;
}

// -- direction from signals above threshold, use geo weights
StatusCode CaloClusterCellWeightCalib::f_dirPos(xAOD::CaloCluster* pClus) const
{
  // loop cells in clusters
  double eCal(0.);
  double eRef(0.);
  double etaRef(0.);
  double phiRef(0.);
  xAOD::CaloCluster::cell_iterator fCell(pClus->cell_begin());
  xAOD::CaloCluster::cell_iterator lCell(pClus->cell_end());
  for ( ; fCell != lCell; fCell++ )
    {
      const CaloCell* cell = *fCell;
      //
      double eWght(fCell.weight() *
		   cell->e()               *
		   m_cellWeight->wtCell(cell));
      eCal += eWght;
      //
      if ( eWght > m_eThreshold )
	{
	  double phiCell(proxim(cell->phi(),phiRef)); 
	  etaRef += cell->eta() * eWght;
	  phiRef  = CaloPhiRange::fix((phiCell * eWght + eRef * phiRef)/(eRef+eWght));
	  eRef   += eWght;
	}
    }
  // set cluster kinematics
  pClus->setE(eCal);
  if ( eRef > 0. )
    {
      pClus->setEta(etaRef/eRef);
      pClus->setPhi(phiRef);
    }
  //
  return StatusCode::SUCCESS;
}
// -- direction from signals above threshold, ignore geo weights
StatusCode CaloClusterCellWeightCalib::f_dirPosNW(xAOD::CaloCluster* pClus) const
{
  // loop cells in clusters
  double eCal(0.);
  double eRef(0.);
  double etaRef(0.);
  double phiRef(0.);
  for (const CaloCell* cell : *pClus)
    {
      //
      double eWght(cell->e() *
		   m_cellWeight->wtCell(cell));
      eCal += eWght;
      //
      if ( eWght > m_eThreshold )
	{
	  double phiCell(proxim(cell->phi(),phiRef)); 
	  etaRef += cell->eta() * eWght;
	  phiRef  = CaloPhiRange::fix((phiCell * eWght + eRef * phiRef)/(eRef+eWght));
	  eRef   += eWght;
	}
    }
  // set cluster kinematics
  pClus->setE(eCal);
  if ( eRef > 0. )
    {
      pClus->setEta(etaRef/eRef);
      pClus->setPhi(phiRef);
    }
  //
  return StatusCode::SUCCESS;
}

StatusCode CaloClusterCellWeightCalib::f_dirAbs(xAOD::CaloCluster* pClus) const
{
  // loop cells in clusters
  double eCal(0.);
  double eRef(0.);
  double etaRef(0.);
  double phiRef(0.);
  xAOD::CaloCluster::cell_iterator fCell(pClus->cell_begin());
  xAOD::CaloCluster::cell_iterator lCell(pClus->cell_end());
  for ( ; fCell != lCell; fCell++ )
    {
      const CaloCell* cell = *fCell;
      //
      double eWght(fCell.weight() *
		   cell->e()               *
		   m_cellWeight->wtCell(cell));
      double phiCell(proxim(cell->phi(),phiRef)); 
      //
      eCal   += eWght;
      eWght   = fabs(eWght);
      etaRef += cell->eta() * eWght;
      phiRef  =	CaloPhiRange::fix((phiCell * eWght + eRef * phiRef)/(eRef+eWght));
      eRef   += eWght;
    }
  // set cluster kinematics
  pClus->setE(eCal);
  if ( eRef > 0. )
    {
      pClus->setEta(etaRef/eRef);
      pClus->setPhi(phiRef);
    }
  //
  return StatusCode::SUCCESS;
}

StatusCode CaloClusterCellWeightCalib::f_dirAbsNW(xAOD::CaloCluster* pClus) const
{
  // loop cells in clusters
  double eCal(0.);
  double eRef(0.);
  double etaRef(0.);
  double phiRef(0.);
  for (const CaloCell* cell : *pClus)
    {
      //
      double eWght(cell->e() * m_cellWeight->wtCell(cell));
      double phiCell(proxim(cell->phi(),phiRef)); 
      //
      eCal   += eWght;
      eWght   = fabs(eWght);
      etaRef += cell->eta() * eWght;
      phiRef  = CaloPhiRange::fix((phiCell * eWght + eRef * phiRef)/(eRef+eWght));
      eRef   += eWght;
    }
  // set cluster kinematics
  pClus->setE(eCal);
  if ( eRef > 0. )
    {
      pClus->setEta(etaRef/eRef);
      pClus->setPhi(phiRef);
    }
  //
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloLCWeightTool.cxx,v 1.9 2009-01-27 09:09:15 gunal Exp $
//
// Description: see CaloLCWeightTool.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloUtils/CaloLCWeightTool.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloUtils/CaloSamplingHelper.h"
#include "CaloUtils/CaloLCCoeffHelper.h"
#include "CaloEvent/CaloPrefetch.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/ListItem.h"
//#include "StoreGate/StoreGateSvc.h" 

#include "xAODCaloEvent/CaloClusterKineHelper.h"

CaloLCWeightTool::CaloLCWeightTool(const std::string& type,
				   const std::string& name,
				   const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_key("HadWeights"),
    m_signalOverNoiseCut(2),
    m_useHadProbability(false),
    m_interpolate(false),
    m_calo_id(nullptr),
    m_calo_dd_man(nullptr),
    m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault")
{

  declareInterface<IClusterCellWeightTool>(this);
  declareProperty("CorrectionKey",m_key);
  // Minimal Signal Over Noise (|E|/sigma) level for cells 
  declareProperty("SignalOverNoiseCut",m_signalOverNoiseCut);
  // Use EM_PROBABILITY Moment to apply relative weights
  declareProperty("UseHadProbability",m_useHadProbability);
  declareProperty("CaloNoiseTool", m_noiseTool);
  // Use Interpolation or not
  declareProperty("Interpolate",m_interpolate);
  m_interpolateDimensionNames.resize(3);
  m_interpolateDimensionNames[0] = "DIMW_ETA";
  m_interpolateDimensionNames[1] = "DIMW_ENER";
  m_interpolateDimensionNames[2] = "DIMW_EDENS";
  declareProperty("InterpolateDimensionNames", m_interpolateDimensionNames);
  declareProperty("UpdateSamplingVars",m_updateSamplingVars=false);
}

StatusCode CaloLCWeightTool::initialize()
{
  if(m_interpolate) {
    msg(MSG::INFO) << "Interpolation is ON, dimensions: ";
    for(std::vector<std::string>::iterator it=m_interpolateDimensionNames.begin(); it!=m_interpolateDimensionNames.end(); it++){
      msg(MSG::INFO) << " " << (*it);
    }
    msg() << endmsg;
    for(std::vector<std::string>::iterator it=m_interpolateDimensionNames.begin(); it!=m_interpolateDimensionNames.end(); it++){
      CaloLocalHadDefs::LocalHadDimensionId id = CaloLCCoeffHelper::getDimensionId( (*it) );
      if(id!=CaloLocalHadDefs::DIMU_UNKNOWN) {
        m_interpolateDimensions.push_back(int(id));
      }else{
        ATH_MSG_WARNING( "Dimension '" << (*it) << "' is invalid and will be excluded."  );
      }
    }
  }

  ATH_MSG_INFO( "Initializing " << name()  );

  ATH_CHECK( m_key.initialize() );

  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance(); 
  m_calo_id   = m_calo_dd_man->getCaloCell_ID();
   
  //---- retrieve the noisetool ----------------
   
  if(m_noiseTool.retrieve().isFailure()){
    ATH_MSG_INFO( "Unable to find tool for CaloNoiseTool");
  }    else {
    ATH_MSG_INFO(  "Noise Tool retrieved"  );
  } 
 
  m_sampnames.reserve(CaloSampling::Unknown);
  for (int iSamp=0;iSamp<CaloSampling::Unknown;iSamp++) {
     m_sampnames.push_back(CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)iSamp));
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloLCWeightTool::weight(xAOD::CaloCluster *theCluster) const
{
  const CaloLocalHadCoeff* data(0);
  SG::ReadCondHandle<CaloLocalHadCoeff> rch(m_key);
  data = *rch;
  if(data==0) {
    ATH_MSG_ERROR("Unable to access conditions object");
    return StatusCode::FAILURE;
  }
  // this is not super effective, but try to put is here first, and optimize later
  std::vector<int> isAmpMap(CaloSampling::Unknown,-1);
  for (int iArea=0;iArea<data->getSizeAreaSet();iArea++) {
    for (int iSamp=0;iSamp<CaloSampling::Unknown;iSamp++) {
      if ( m_sampnames[iSamp] == data->getArea(iArea)->getTitle() ) {
        ATH_MSG_DEBUG("Found Area for Sampling " << CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)iSamp));
        isAmpMap[iSamp] = iArea;
        break;
      }
    }
  }

  double eEM = theCluster->e();

  std::vector<float> vars(5);

  CaloLCCoeffHelper hp;
  CaloLocalHadCoeff::LocalHadCoeff parint;

  double pi0Prob = 0;
  if ( m_useHadProbability) {
    if (!theCluster->retrieveMoment(xAOD::CaloCluster::EM_PROBABILITY,pi0Prob)) {
      ATH_MSG_WARNING ("Cannot find cluster moment EM_PROBABILITY");
    }
  }
  else if (theCluster->recoStatus().checkStatus(CaloRecoStatus::TAGGEDEM)) {
    pi0Prob = 1.;
  }
  if ( pi0Prob < 0 ) 
    pi0Prob = 0;
  if ( pi0Prob > 1 )
    pi0Prob = 1;

  if ( eEM > 0 ) {
    // loop over all cells
    
    xAOD::CaloCluster::cell_iterator itrCell = theCluster->cell_begin();
    xAOD::CaloCluster::cell_iterator itrCellEnd = theCluster->cell_end();
    for (;itrCell!=itrCellEnd; ++itrCell) {
      CaloPrefetch::nextDDE(itrCell, itrCellEnd);
      // check calo and sampling index for current cell
      Identifier myId = itrCell->ID();
      CaloCell_ID::CaloSample theSample = CaloCell_ID::CaloSample(m_calo_id->calo_sample(myId));
      if ( isAmpMap[theSample] >= 0 ) {
	double sigma =  m_noiseTool->getNoise(*itrCell,ICalorimeterNoiseTool::ELECTRONICNOISE);
	double energy = fabs(itrCell->e());
	double ratio = 0;
	if ( std::isfinite(sigma) && sigma > 0 ) 
	  ratio = energy/sigma;
	if ( ratio > m_signalOverNoiseCut ) {
	  double volume = 0;
	  double density = 0;
          const CaloDetDescrElement* myCDDE = itrCell->caloDDE();
	  if ( myCDDE ) {
	    volume = myCDDE->volume();
	  }
	  if ( volume > 0 ) 
	    density = energy/volume;
	  if ( density > 0 ) {
	    double abseta = fabs(itrCell->eta());
	    double log10edens = log10(density);
	    double log10cluse = log10(eEM);
	    const CaloLocalHadCoeff::LocalHadDimension *logeDim = data->getArea(isAmpMap[theSample])->getDimension(3);
	    double lemax = logeDim->getXmax()-0.5*logeDim->getDx();
	    if ( log10cluse > lemax ) log10cluse = lemax;

            vars[CaloLocalHadDefs::DIMW_SIDE] = static_cast<float> ((itrCell->eta()<0?-1.0:1.0));
            vars[CaloLocalHadDefs::DIMW_ETA] = static_cast<float> (abseta);
            vars[CaloLocalHadDefs::DIMW_PHI] = static_cast<float> (itrCell->phi());
            vars[CaloLocalHadDefs::DIMW_ENER] = static_cast<float> (log10cluse);
            vars[CaloLocalHadDefs::DIMW_EDENS] = static_cast<float> (log10edens);

            bool isDataOK = false;
            double wData(0);

            // accessing coefficients (non-interpolated)
            int iBin = data->getBin(isAmpMap[theSample],vars);
            if ( iBin >= 0 ) {
              const CaloLocalHadCoeff::LocalHadCoeff * pData = data->getCoeff(iBin);
              if ( pData && (*pData)[CaloLocalHadDefs::BIN_ENTRIES] > 10 ) {
                isDataOK = true;
                wData = (*pData)[CaloLocalHadDefs::BIN_WEIGHT];
              }
              if(m_interpolate) {
                // accesing interpolated coefficients
                bool isa = hp.Interpolate(data, isAmpMap[theSample], vars, parint, m_interpolateDimensions);
                if(isa && parint[CaloLocalHadDefs::BIN_ENTRIES] > 10) {
                  isDataOK = true;
                  wData = parint[CaloLocalHadDefs::BIN_WEIGHT];
                }
              }
            }

            if(isDataOK) {
              ATH_MSG_DEBUG(" weight(" 
			    << theSample << ", " 
			    << vars[0] << ", " 
			    << vars[1] << ", " 
			    << vars[2] << ", " 
			    << vars[3] << ", " 
			    << vars[4] << ") = " 
			    << wData);
              double weight = itrCell.weight();//theCluster->getCellWeight(itrCell); // fastest!
              weight *= (pi0Prob + (1-pi0Prob)*wData);
              // reweight cell in cluster
              theCluster->reweightCell(itrCell,weight);
            }
	  } // density
	} // noise cut
      } // sampling
    } // itrCell
    CaloClusterKineHelper::calculateKine(theCluster,true,m_updateSamplingVars);
  } // eEM

  // assume that the weighting could be called more than once. In that case 
  // eEM is the result of the previous step and the current e/eEM ratio
  // should be multiplied with the existing HAD_WEIGHT moment
  double new_weight (1);
  if (!theCluster->retrieveMoment(xAOD::CaloCluster::HAD_WEIGHT,new_weight)) {
    ATH_MSG_ERROR("Cannot retrieve HAD_WEIGHT cluster moment." );
    return StatusCode::FAILURE;
  }

  if ( eEM > 0 || eEM < 0 ) {
    new_weight *= theCluster->e()/eEM;
  }
  theCluster->insertMoment(xAOD::CaloCluster::HAD_WEIGHT,new_weight);

  return StatusCode::SUCCESS;
}

CaloLCWeightTool::~CaloLCWeightTool()
{
}



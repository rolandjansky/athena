/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloLCOutOfClusterTool.cxx,v 1.3 2009-01-27 09:09:16 gunal Exp $
//
// Description: see CaloLCOutOfClusterTool.h
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
#include "CaloUtils/CaloLCOutOfClusterTool.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloUtils/CaloLCCoeffHelper.h"
#include "CaloUtils/CaloSamplingHelper.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloPrefetch.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

/*
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h" 
*/

using xAOD::CaloCluster;

CaloLCOutOfClusterTool::CaloLCOutOfClusterTool(const std::string& type,
					       const std::string& name,
					       const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_key("OOCCorrection"),
    m_useEmProbability(false),
    m_useHadProbability(false),
    m_interpolate(false)

{ 

  declareInterface<IClusterCellWeightTool>(this);
  m_invalidSamplingNames.resize(3);

  m_invalidSamplingNames[0] = "PreSamplerB";
  m_invalidSamplingNames[1] = "PreSamplerE";
  m_invalidSamplingNames[2] = "TileGap3";

  declareProperty("CorrectionKey",m_key);
  declareProperty("UseEmProbability",m_useEmProbability);
  declareProperty("UseHadProbability",m_useHadProbability);
  declareProperty("InvalidSamplings",m_invalidSamplingNames);

  // Use Interpolation or not
  declareProperty("Interpolate",m_interpolate);
  m_interpolateDimensionNames.resize(3);
  m_interpolateDimensionNames[0] = "DIMO_ENER";
  m_interpolateDimensionNames[1] = "DIMO_ETA";
  m_interpolateDimensionNames[2] = "DIMO_LAMBDA";
  declareProperty("InterpolateDimensionNames", m_interpolateDimensionNames);
  declareProperty("UpdateSamplingVars",m_updateSamplingVars=false);
}

StatusCode CaloLCOutOfClusterTool::initialize()
{
  ATH_MSG_INFO( "Initializing " << name()  );

  //--- check sampling names to use exclude in correction
  std::vector<std::string>::iterator samplingIter = m_invalidSamplingNames.begin(); 
  std::vector<std::string>::iterator samplingIterEnd = m_invalidSamplingNames.end(); 
  for(; samplingIter!=samplingIterEnd; samplingIter++) { 
    int theSampling(CaloSampling::Unknown);
    for (unsigned int jsamp = 0;jsamp< CaloSampling::Unknown; jsamp++) {
      if ( *samplingIter == CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)jsamp)) {
	theSampling = jsamp;
	break;
      }
    }
    if ( theSampling == CaloSampling::Unknown ) {
      msg(MSG::ERROR) << "Calorimeter sampling " 
	  << *samplingIter
          << " is not a valid Calorimeter sampling name and will be ignored! "
          << "Valid names are: ";
      for (unsigned int jsamp = 0;jsamp< CaloSampling::Unknown; jsamp++) {
	msg() << CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)jsamp);
	if ( jsamp < CaloSampling::Unknown-1) 
	  msg() << ", ";
	else 
	  msg() << ".";
      }
      msg() << endmsg;
    }
    else {
      m_invalidSamplings.insert(theSampling);
    }
  }

  msg(MSG::INFO) << "Samplings to exclude from the out-of-cluster weighting:";
  samplingIter = m_invalidSamplingNames.begin(); 
  for(; samplingIter!=samplingIterEnd; samplingIter++)  
    msg() << " " << *samplingIter;
  msg() << endmsg;

  if(m_interpolate) {
    msg(MSG::INFO) << "Interpolation is ON, dimensions: ";
    for(std::vector<std::string>::iterator it=m_interpolateDimensionNames.begin(); it!=m_interpolateDimensionNames.end(); it++){
      msg() << " " << (*it);
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

  // callback for conditions data
  ATH_CHECK(  m_key.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode CaloLCOutOfClusterTool::weight(CaloCluster *theCluster) const
{
  double eWeightedOrig = theCluster->e();
  double eWeighted = theCluster->e();
  // get ratio of weighted and em cluster
  double ratio=1.;
  std::vector<float> vars(5);

  CaloLCCoeffHelper hp;

  double pi0Prob = 0;
  if ( m_useEmProbability || m_useHadProbability) {
    theCluster->retrieveMoment(CaloCluster::EM_PROBABILITY,pi0Prob);
  }
  else if (theCluster->recoStatus().checkStatus(CaloRecoStatus::TAGGEDEM)) {
    pi0Prob = 1.;
  }
  if ( pi0Prob < 0 ) 
    pi0Prob = 0;
  if ( pi0Prob > 1 )
    pi0Prob = 1;

  double classifyWeight = 1;
  if ( m_useEmProbability ) {
    classifyWeight = pi0Prob;
  }
  else if ( m_useHadProbability ) {
    classifyWeight = 1-pi0Prob;
  }
  double eEM = theCluster->rawE();
  // basic energy can be 0 if the locking tool option FixBasicEnergy is False
  // in that case simply use the weighted energy ...
  if ( eEM == 0. ) {
    ATH_MSG_DEBUG("Basic Energy is 0. Use weighted energy instead");
    eEM = eWeighted;
  }
  
  

  // subtract the samplings to ignore from eEM and eWeighted, assuming that
  // they would not have received any cell weights in prior calibration 
  // steps
  std::set<int>::const_iterator ivSamplingIter = m_invalidSamplings.begin(); 
  std::set<int>::const_iterator ivSamplingIterEnd = m_invalidSamplings.end(); 
  for(; ivSamplingIter!=ivSamplingIterEnd; ivSamplingIter++) {
    eEM       -= theCluster->eSample((CaloCluster::CaloSample)(*ivSamplingIter));
    eWeighted -= theCluster->eSample((CaloCluster::CaloSample)(*ivSamplingIter));
  }

  if ( eEM > 0 && eWeighted > 0 ) {
    // ratio is em energy over weighted energy without the samplings
    // to ignore. This means also the OOC weight tables have to be made 
    // relative to the em energy wihtout the samplings to ignore ...
    ratio = eEM / eWeighted;

    if ( ratio < 0.3 || ratio > 3 ) {
      ATH_MSG_DEBUG("The ratio eEM/eWeighted = " 
		    << ratio << " is out of normal range [0.3,3]" 
		    << " - this mean we have mainly noise ... using 1 instead");
      ratio = 1.0;
    }
    
    double log10cluse = log10(eEM);

    SG::ReadCondHandle<CaloLocalHadCoeff> h (m_key);
    const CaloLocalHadCoeff* data = *h;
    if (!data) {
      ATH_MSG_ERROR("Unable to access conditions object");
      return StatusCode(CaloRecoStatus::TAGGEDUNKNOWN);
    }

    const CaloLocalHadCoeff::LocalHadDimension *logeDim = data->getArea(0)->getDimension(2);
    double lemax = logeDim->getXmax()-0.5*logeDim->getDx();
    double lemin = logeDim->getXmin()+0.5*logeDim->getDx();
    if ( log10cluse > lemax ) 
      log10cluse = lemax;
    if ( log10cluse < lemin ) 
      log10cluse = lemin;

    double center_lambda,isolation;
    if ( theCluster->retrieveMoment(CaloCluster::ISOLATION,isolation) 
	 && theCluster->retrieveMoment(CaloCluster::CENTER_LAMBDA, center_lambda)) {
      if ( isolation > 0 ) {
	if ( center_lambda > 0 ) {
	  const double abseta = fabs(theCluster->eta());
	  const double log10lambda = log10(center_lambda);

	  vars[CaloLocalHadDefs::DIMO_SIDE] = static_cast<float> ((theCluster->eta()<0?-1.0:1.0));
	  vars[CaloLocalHadDefs::DIMO_PHI] = static_cast<float> (theCluster->phi());
	  vars[CaloLocalHadDefs::DIMO_ENER] = static_cast<float> (log10cluse);
	  vars[CaloLocalHadDefs::DIMO_ETA] = static_cast<float> (abseta);
	  vars[CaloLocalHadDefs::DIMO_LAMBDA] = static_cast<float> (log10lambda);

          bool isDataOK = false;
          double oocData(0);

          // accessing coefficients (non-interpolated)
          int iBin = data->getBin(0,vars);
          if ( iBin >= 0 ) {
            const CaloLocalHadCoeff::LocalHadCoeff * pData = data->getCoeff(iBin);
            if ( pData && (*pData)[CaloLocalHadDefs::BIN_ENTRIES] > 0 ) {
              isDataOK = true;
              oocData = (*pData)[CaloLocalHadDefs::BIN_WEIGHT];
            }
            if(m_interpolate) {
              // accesing interpolated coefficients
              CaloLocalHadCoeff::LocalHadCoeff parint;
              bool isa = hp.Interpolate(data, 0,vars,parint, m_interpolateDimensions);
              if(isa && parint[CaloLocalHadDefs::BIN_ENTRIES] > 0) {
                isDataOK = true;
                oocData = parint[CaloLocalHadDefs::BIN_WEIGHT];
              }
            }
          }

	  ATH_MSG_DEBUG("interpolation_ON=" << m_interpolate
			<< "isDataOK="<<isDataOK
			<< "side = " << vars[0]
			<< ", phi = " << vars[1]
			<< ", log10cluse = " << log10cluse
			<< ", eta = " << abseta
			<< ", log10lambda = " << log10lambda
			<< ", ratio = " << ratio
			<< ", isol = " << isolation
			<< ", oocData = " << oocData);

          if(isDataOK){
            const double oocWeight = 1.+classifyWeight*isolation*ratio*oocData;
            // loop over all cells
	    CaloCluster::cell_iterator itrCell = theCluster->cell_begin();
            CaloCluster::cell_iterator itrCellEnd = theCluster->cell_end();
            for (;itrCell!=itrCellEnd; ++itrCell) {
              CaloPrefetch::nextDDE(itrCell, itrCellEnd);
              const CaloDetDescrElement* cDDE = itrCell->caloDDE();
              if ( cDDE && m_invalidSamplings.find(cDDE->getSampling()) == m_invalidSamplings.end() ) { //Fixme ... could use a bit-pattern
                double weight = itrCell.weight();//theCluster->getCellWeight(itrCell); // fastest!
                weight *= oocWeight;
                // reweight cell in cluster
                theCluster->reweightCell(itrCell,weight);
              } else if (cDDE) {
                ATH_MSG_DEBUG("Exclude cell with sampling = " << cDDE->getSampling());
              }
            }//cell-loop
	    CaloClusterKineHelper::calculateKine(theCluster,true,m_updateSamplingVars);
          }
	} // log10lambda
      } // isolation
    } // moments
  }
  
  // assume that the weighting could be called more than once. In that
  // case eWeighted is the result of the previous step and the current
  // e/eWeighted ratio should be multiplied with the existing
  // OOC_WEIGHT moment
  if ( eWeightedOrig > 0 || eWeightedOrig < 0 ) { 
    double old_weight(1);
    if (!theCluster->retrieveMoment(CaloCluster::OOC_WEIGHT,old_weight)) {
      ATH_MSG_ERROR("Cannot retrieve OOC_WEIGHT cluster moment." );
      return StatusCode::FAILURE;
    }
    const double new_weight = old_weight*theCluster->e()/eWeightedOrig;
    theCluster->insertMoment(CaloCluster::OOC_WEIGHT,new_weight); 
  }
  return StatusCode::SUCCESS;
}

CaloLCOutOfClusterTool::~CaloLCOutOfClusterTool()
{
}



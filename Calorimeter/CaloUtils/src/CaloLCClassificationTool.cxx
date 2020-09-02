/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloLCClassificationTool.cxx,v 1.7 2009-01-27 09:09:15 gunal Exp $
//
// Description: see CaloLCClassificationTool.h
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
#include "CaloUtils/CaloLCClassificationTool.h"
#include "CaloUtils/CaloLCCoeffHelper.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "StoreGate/ReadCondHandle.h"

using xAOD::CaloCluster;

CaloLCClassificationTool::CaloLCClassificationTool(const std::string& type,
						   const std::string& name,
						   const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_key("EMFracClassify"),
    m_useSpread(false),
    m_useNormalizedEnergyDensity(true),
    m_maxProbability(0.5),
    m_storeClassificationProbabilityInAOD(true),
    m_interpolate(false),
    m_absOpt(false)
{ 

   declareInterface<IClusterClassificationTool>(this);
  // Name of data object in conditions store
  declareProperty("ClassificationKey",m_key);
  // Use Spread of EM probability for estimate 
  declareProperty("UseSpread",m_useSpread);
  // Use normalized energy density
  declareProperty("UseNormalizedEnergyDensity",m_useNormalizedEnergyDensity);
  // Maximal EM probability below which all clusters are considered hadronic
  declareProperty("MaxProbability",m_maxProbability);
  // Store the classification Probability as a cluster moment in AOD
  // (if false it goes to ESD only)
  declareProperty("StoreClassificationProbabilityInAOD",m_storeClassificationProbabilityInAOD);
  // Use Interpolation or not
  declareProperty("Interpolate",m_interpolate);
  //Use weighting of negative clusters?
  declareProperty("WeightingOfNegClusters",m_absOpt);

  m_interpolateDimensionNames.resize(3);
  m_interpolateDimensionNames[0] = "DIMC_ETA";
  m_interpolateDimensionNames[1] = "DIMC_EDENS";
  m_interpolateDimensionNames[2] = "DIMC_LAMBDA";
  declareProperty("InterpolateDimensionNames", m_interpolateDimensionNames);
}

StatusCode CaloLCClassificationTool::initialize()
{
  ATH_MSG_INFO( "Initializing " << name()  );

  ATH_CHECK( m_key.initialize() );

  if(m_interpolate) {
    msg(MSG::INFO) << "Interpolation is ON, dimensions: ";
    for(std::vector<std::string>::iterator it=m_interpolateDimensionNames.begin(); it!=m_interpolateDimensionNames.end(); ++it){
      msg() << " " << (*it);
    }
    msg() << endmsg;
    for(std::vector<std::string>::iterator it=m_interpolateDimensionNames.begin(); it!=m_interpolateDimensionNames.end(); ++it){
      CaloLocalHadDefs::LocalHadDimensionId id = CaloLCCoeffHelper::getDimensionId( (*it) );
      if(id!=CaloLocalHadDefs::DIMU_UNKNOWN) {
        m_interpolateDimensions.push_back(int(id));
      }else{
        ATH_MSG_WARNING( "Dimension '" << (*it) << "' is invalid and will be excluded."  );
      }
    }
  }

  return StatusCode::SUCCESS;
}

CaloRecoStatus::StatusIndicator CaloLCClassificationTool::classify(CaloCluster* thisCluster) const
{
  const CaloLocalHadCoeff* condObject(nullptr);
  SG::ReadCondHandle<CaloLocalHadCoeff> rch(m_key);
  condObject = *rch;
  if(condObject==nullptr) {
    ATH_MSG_ERROR("Unable to access conditions object");
    return CaloRecoStatus::TAGGEDUNKNOWN;
  }

  CaloLCCoeffHelper hp;
  CaloLocalHadCoeff::LocalHadCoeff parint;

  CaloRecoStatus::StatusIndicator recStat = CaloRecoStatus::TAGGEDUNKNOWN; 
  double probPi0 = 0;
  // on ESD only
  std::vector<float> vars(6);
  if ( thisCluster->e() > 0 || m_absOpt ) { //with abs option, always weight cluster
    double log10cluse = log10(thisCluster->e());
    if( m_absOpt ) log10cluse = log10(fabs(thisCluster->e()));
    double log10cluseOrig = log10cluse;
    
    const CaloLocalHadCoeff::LocalHadDimension *logeDim = condObject->getArea(0)->getDimension(3);
    double lemax = logeDim->getXmax()-0.5*logeDim->getDx();
    double lemin = logeDim->getXmin()+0.5*logeDim->getDx();
    if ( log10cluse > lemax ) 
      log10cluse = lemax;
    if ( log10cluse < lemin ) 
      log10cluse = lemin;
    
    double m1_dens,center_lambda;
    //Fixme ... check moavailability ..
    if (thisCluster->retrieveMoment(CaloCluster::FIRST_ENG_DENS,m1_dens) &&
	thisCluster->retrieveMoment(CaloCluster::CENTER_LAMBDA,center_lambda)) { 
      if ( m1_dens > 0  &&  center_lambda > 0) {
	const double abseta = fabs(thisCluster->eta());
	double log10edens = log10(m1_dens);
	if ( m_useNormalizedEnergyDensity ) log10edens -= log10cluseOrig;
	double log10lambda = log10(center_lambda);

	vars[CaloLocalHadDefs::DIMC_SIDE]   = static_cast<float> ((thisCluster->eta()<0?-1.0:1.0));
	vars[CaloLocalHadDefs::DIMC_ETA]    = static_cast<float> (abseta);
	vars[CaloLocalHadDefs::DIMC_PHI]    = static_cast<float> (thisCluster->phi());
	vars[CaloLocalHadDefs::DIMC_ENER]   = static_cast<float> (log10cluse);
	vars[CaloLocalHadDefs::DIMC_EDENS]  = static_cast<float> (log10edens);
	vars[CaloLocalHadDefs::DIMC_LAMBDA] = static_cast<float> (log10lambda);

	bool isDataOK = false;
	double emProb(0);

	int iBin = condObject->getBin(0,vars);
	if ( iBin >= 0 ) {
	  const CaloLocalHadCoeff::LocalHadCoeff * pData = condObject->getCoeff(iBin);
	  if ( pData && (*pData)[CaloLocalHadDefs::BIN_ENTRIES] > 0 ) {
	    isDataOK = true;
	    emProb = (*pData)[CaloLocalHadDefs::BIN_WEIGHT];
	    if (m_useSpread) {
	      double emErr = sqrt((*pData)[CaloLocalHadDefs::BIN_ENTRIES])*(*pData)[CaloLocalHadDefs::BIN_ERROR];
	      emProb += emErr;
	    }

	    if(m_interpolate) {
	      // accesing interpolated coefficients
	      bool isa = hp.Interpolate(condObject, 0,vars,parint, m_interpolateDimensions);
	      if(isa) {
		if ( parint[CaloLocalHadDefs::BIN_ENTRIES] > 0 ) {
		  isDataOK = true;
		  emProb = parint[CaloLocalHadDefs::BIN_WEIGHT];
		  if (m_useSpread) {
		    double emErr = sqrt(parint[CaloLocalHadDefs::BIN_ENTRIES])*parint[CaloLocalHadDefs::BIN_ERROR];
		    emProb += emErr;
		  }
		}
	      }//end if isa
	    }//end if m_interpolate
	  } //end if pData
	} //end if iBin

	if(isDataOK) {
	  probPi0 = emProb;
	  if ( emProb < m_maxProbability )
	    recStat = CaloRecoStatus::TAGGEDHAD;
	  else 
	    recStat = CaloRecoStatus::TAGGEDEM;
	}

      } // if >0
    } //if got sucessfully retrieved moments
    else {
      ATH_MSG_ERROR( "Can not retrieve one or more of the cluster moments "
                     << "CENTER_LAMBDA, FIRST_ENG_DENS for current cluster"
                     << "- classification not possible - "
                     << "you need to enable those moments in the cluster maker!"
                     );
      recStat = CaloRecoStatus::TAGGEDUNKNOWN;
    } 
  }
  
  thisCluster->insertMoment(CaloCluster::EM_PROBABILITY,probPi0);
  thisCluster->insertMoment(CaloCluster::HAD_WEIGHT,1.0);
  thisCluster->insertMoment(CaloCluster::OOC_WEIGHT,1.0);
  thisCluster->insertMoment(CaloCluster::DM_WEIGHT,1.0);

  return recStat;
}

CaloLCClassificationTool::~CaloLCClassificationTool()
{
}


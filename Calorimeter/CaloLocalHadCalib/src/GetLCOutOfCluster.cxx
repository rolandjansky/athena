/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: GetLCOutOfCluster.cxx,v 1.2 2008-11-04 14:33:37 menke Exp $
//
// Description: see GetLCOutOfCluster.h
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
#include "CaloLocalHadCalib/GetLCOutOfCluster.h"

//---------------
// C++ Headers --
//---------------
#include "CaloLocalHadCalib/GetLCDefs.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloUtils/CaloSamplingHelper.h"
#include "StoreGate/ReadHandle.h"

#include "AthenaKernel/errorcheck.h"

#include "TFile.h"
#include "TProfile2D.h"
#include "TString.h"
#include <iterator>
#include <cmath>

//###############################################################################
GetLCOutOfCluster::GetLCOutOfCluster(const std::string& name, 
				     ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_outputFile(nullptr),
    m_clusterCollName("CaloTopoClusters"),
    m_NormalizationType("Lin"),
    m_NormalizationTypeNumber(0),
    m_ClassificationType("None"),
    m_ClassificationTypeNumber(0)
{
 
  std::vector<Gaudi::Histo1DDef> dims(6);
  dims[0] = Gaudi::Histo1DDef("side",-1.5,1.5,1);
  dims[1] = Gaudi::Histo1DDef("|eta|",0.,5.,50);
  dims[2] = Gaudi::Histo1DDef("phi",-M_PI,M_PI,1);
  dims[3] = Gaudi::Histo1DDef("log10(E_clus (MeV))",log10(200),log10(1e6),14);
  dims[4] = Gaudi::Histo1DDef("log10(lambda_clus (mm))",0.0,4.0,14);
  dims[5] = Gaudi::Histo1DDef("weight",0.,3.0,1);

  mapinsert(dims);

  // Dimensions to use for classification
  declareProperty("OutOfClusterDimensions",m_dimensionsmap);

  // Name of output file to save histograms in
  declareProperty("OutputFileName",m_outputFileName);  
  
  // Name of ClusterContainer to use
  declareProperty("ClusterCollectionName",m_clusterCollName);  

  // Name of Samplings to ignore 
  m_invalidSamplingNames.resize(3);

  m_invalidSamplingNames[0] = "PreSamplerB";
  m_invalidSamplingNames[1] = "PreSamplerE";
  m_invalidSamplingNames[2] = "TileGap3";

  declareProperty("InvalidSamplings",m_invalidSamplingNames);

  // Normalization type "Const", "Lin", "Log", "NClus"
  declareProperty("NormalizationType",m_NormalizationType); 

  // Classification type "None" for single pion MC or 
  //                     "ParticleID_EM" for ParticleID based em-type clusters  
  //                     "ParticleID_HAD" for ParticleID based had-type clusters  
  declareProperty("ClassificationType", m_ClassificationType);
}

//###############################################################################

GetLCOutOfCluster::~GetLCOutOfCluster()
{ }

//###############################################################################

StatusCode GetLCOutOfCluster::initialize() {

  m_outputFile = std::make_unique<TFile>(m_outputFileName.c_str(),"RECREATE");
  m_outputFile->cd();
  m_ooc.resize(0);

  mapparse();

  if ( m_NormalizationType == "Lin" ) {
    ATH_MSG_INFO( "Using weighting proportional to E_calib" );
    m_NormalizationTypeNumber = GetLCDefs::LIN;
  }
  else if ( m_NormalizationType == "Log" ) {
    ATH_MSG_INFO( "Using weighting proportional to log(E_calib)" );
    m_NormalizationTypeNumber = GetLCDefs::LOG;
  }
  else if ( m_NormalizationType == "NClus" ) {
    ATH_MSG_INFO( "Using weighting proportional to 1/N_Clus_E_calib>0" );
    m_NormalizationTypeNumber = GetLCDefs::NCLUS;
  }
  else {
    ATH_MSG_INFO( "Using constant weighting" );
    m_NormalizationTypeNumber = GetLCDefs::CONST;
  }

  if ( m_ClassificationType == "None" ) {
    ATH_MSG_INFO( "Expecting single particle input" );
    m_ClassificationTypeNumber = GetLCDefs::NONE;
  }
  else if ( m_ClassificationType == "ParticleID_EM" ) {
    ATH_MSG_INFO( "Expecting ParticleID simulation as input -- use EM type clusters only" );
    m_ClassificationTypeNumber = GetLCDefs::PARTICLEID_EM;
  }
  else if ( m_ClassificationType == "ParticleID_HAD" ) {
    ATH_MSG_INFO( "Expecting ParticleID simulation as input -- use HAD type clusters only" );
    m_ClassificationTypeNumber = GetLCDefs::PARTICLEID_HAD;
  }
  else {
    ATH_MSG_WARNING( " unknown classification type " << m_ClassificationType << " given! Using None instead" );
    m_ClassificationTypeNumber = GetLCDefs::NONE;
  }

  int iside(-1);
  int ieta(-1);
  int iphi(-1);
  int ilogE(-1);
  int iloglambda(-1);
  int iweight(-1);
  
  m_isampmap.resize(3,-1);
  for(unsigned int idim=0;idim<m_dimensions.size();idim++) {
    if (      m_dimensions[idim].title() == "side" ) {
      iside = idim;
      m_isampmap[0] = iside;
    }
    else if ( m_dimensions[idim].title() == "|eta|" ) 
      ieta = idim;
    else if ( m_dimensions[idim].title() == "phi" ) {
      iphi = idim;
      m_isampmap[1] = iphi;
    }
    else if ( m_dimensions[idim].title() == "log10(E_clus (MeV))" ) {
      ilogE = idim;
      m_isampmap[2] = ilogE;
    }
    else if ( m_dimensions[idim].title() == "log10(lambda_clus (mm))" )
      iloglambda = idim;
    else if ( m_dimensions[idim].title() == "weight" )
      iweight = idim;
  }
  if ( ilogE < 0 || ieta < 0 || iloglambda < 0 || iweight < 0 || iside < 0 ) {
    ATH_MSG_FATAL(" Mandatory dimension log10E, |eta|, log10lambda or weight missing ...");
    return StatusCode::FAILURE;
  }
  int nside = m_dimensions[iside].bins();
  int nphi = (iphi>=0?m_dimensions[iphi].bins():1);
  int nlogE = m_dimensions[ilogE].bins();
  m_ooc.resize(nside*nphi*nlogE,nullptr);
  for ( int jside=0;jside<nside;jside++) {
    for ( int jphi=0;jphi<nphi;jphi++) {
      for(int jlogE=0;jlogE<nlogE;jlogE++) {
	TString oname("ooc");
	  oname += "_iside_";
	  oname += jside;
	  oname += "_[";
	  oname += m_dimensions[iside].lowEdge();
	  oname += ",";
	  oname += m_dimensions[iside].highEdge();
	  oname += ",";
	  oname += nside;
	  oname += "]";
	  oname += "_iphi_";
	  oname += jphi;
	  oname += "_[";
	  oname += (iphi>=0?m_dimensions[iphi].lowEdge():-1);
	  oname += ",";
	  oname += (iphi>=0?m_dimensions[iphi].highEdge():-1);
	  oname += ",";
	  oname += nphi;
	  oname += "]";
	  oname += "_ilogE_";
	  oname += jlogE;
	  oname += "_[";
	  oname += m_dimensions[ilogE].lowEdge();
	  oname += ",";
	  oname += m_dimensions[ilogE].highEdge();
	  oname += ",";
	  oname += nlogE;
	  oname += "]";
	  int iO = jlogE*nphi*nside+jphi*nside+jside;
	  m_ooc[iO] = new TProfile2D(oname,oname,
				     m_dimensions[ieta].bins(),
				     m_dimensions[ieta].lowEdge(),
				     m_dimensions[ieta].highEdge(),
				     m_dimensions[iloglambda].bins(),
				     m_dimensions[iloglambda].lowEdge(),
				     m_dimensions[iloglambda].highEdge(),
				     m_dimensions[iweight].lowEdge(),
				     m_dimensions[iweight].highEdge());
	  m_ooc[iO]->SetXTitle("|#eta_{clus}|");
	  m_ooc[iO]->SetYTitle("log_{10}(#lambda_{clus} (mm))");
	  m_ooc[iO]->SetZTitle("E_{out of cluster} / E_{clus}^{EM-no-PS/Gap3} / Isolation");
      }
    }
  }
  //--- check sampling names to use exclude in correction
  for (const std::string& name : m_invalidSamplingNames) {
    int theSampling(CaloSampling::Unknown);
    for (unsigned int jsamp = 0;jsamp< CaloSampling::Unknown; jsamp++) {
      if ( name == CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)jsamp)) {
	theSampling = jsamp;
	break;
      }
    }
    if ( theSampling == CaloSampling::Unknown ) {
      msg(MSG::ERROR)  << "Calorimeter sampling " 
	  << name
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
  for (const std::string& name : m_invalidSamplingNames)
    msg() << " " << name;
  msg() << endmsg;

  ATH_CHECK( m_clusterCollName.initialize() );

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode GetLCOutOfCluster::finalize()
{
  ATH_MSG_INFO( "Writing out histograms" );
  m_outputFile->cd();
  for(unsigned int i=0;i<m_ooc.size();i++) {
    m_ooc[i]->Write();
  }
  m_outputFile->Close();
  
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode GetLCOutOfCluster::execute()
{
  SG::ReadHandle<xAOD::CaloClusterContainer> cc (m_clusterCollName);

  // total calib hit energy of all clusters 
  double eCalibTot(0.); 
  double nClusECalibGt0 = 0.0;

  for (const xAOD::CaloCluster* theCluster : *cc) {
    double eC=999; 
    if (!theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT,eC)) {
      ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_TOT" );
      return StatusCode::FAILURE;      
    }
    if ( m_ClassificationTypeNumber != GetLCDefs::NONE ) {
      double emFrac=-999; 
      if (!theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM,emFrac)){
	ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_FAC_EM" );
	return StatusCode::FAILURE;
      }
      if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_EM && emFrac < 0.5 )
	eC = 0;
      if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_HAD && emFrac > 0.5 )
	eC = 0;
    }
    eCalibTot += eC;
    if ( eC > 0 ) {
      nClusECalibGt0++;
    }
  }

  if ( eCalibTot > 0 ) {
    const double inv_eCalibTot = 1. / eCalibTot;
    const double inv_nClusECalibGt0 = 1. / nClusECalibGt0;
    for (const xAOD::CaloCluster* pClus : *cc) {
      double eng = pClus->e();

      if ( m_ClassificationTypeNumber != GetLCDefs::NONE ) {
	double emFrac=-999; 
	if (!pClus->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM,emFrac)){
	  ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_FAC_EM");
	  return StatusCode::FAILURE;
	}
	if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_EM && emFrac < 0.5 )
	  continue;
	if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_HAD && emFrac > 0.5 )
	  continue;
      }

      // subtract the samplings to ignore from eng
      for (int sampling : m_invalidSamplings) {
	eng -= pClus->eSample((CaloSampling::CaloSample)(sampling));
      }
      
      if ( eng > 0 ) { 

	int iside = 0;
	int iphi = 0;
	int ilogE = 0;
	int nside = 1;
	int nphi = 1;
	int nlogE = 1;
	
	if ( m_isampmap[0] >= 0 ) {
	  const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[0]];
	  nside = hd.bins();
	  iside = (int)(nside*(((pClus->eta()<0?-1.0:1.0) - hd.lowEdge())
			       /(hd.highEdge()-hd.lowEdge())));
	  if ( iside < 0 || iside > nside-1 ) {
	    ATH_MSG_WARNING( " Side index out of bounds " <<
	      iside << " not in [0," << nside-1 << "]" ); 
	    iside = -1;
	  }
	}
	
	if ( m_isampmap[1] >= 0 ) {
	  const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[1]];
	  nphi = hd.bins();
	  iphi = (int)(nphi*((pClus->phi() - hd.lowEdge())
			     /(hd.highEdge()-hd.lowEdge())));
	  if ( iphi < 0 || iphi > nphi-1 ) {
	    ATH_MSG_WARNING( " Phi index out of bounds " <<
	      iphi << " not in [0," << nphi-1 << "]" ); 
	    iphi = -1;
	  }
	}
	
	const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[2]];
	nlogE = hd.bins();
	ilogE = (int)(nlogE*((log10(eng) - hd.lowEdge())
			     /(hd.highEdge()-hd.lowEdge())));
	if ( ilogE >= 0 && ilogE < nlogE ) {
	  double lamb,eout,etot,isol;
	  if (!pClus->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,lamb) ||
	      !pClus->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_OUT_L,eout) ||
	      !pClus->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT,etot) ||
	      !pClus->retrieveMoment(xAOD::CaloCluster::ISOLATION,isol)) {
	    ATH_MSG_ERROR( "Failed to retrieve a cluster moment (CENTER_LAMBDA,ENG_CALIB_OUT,ENG_CALIB_TOT,ISOLATION)" );
	    return StatusCode::FAILURE;
	  }
	
	  if ( lamb > 0 &&
	       etot > 0 &&
	       isol > 0.5 )
          {
	    int iO = ilogE*nphi*nside+iphi*nside+iside;
	    if (m_ooc[iO]) {
	      double norm = 0.0;
	      if ( m_NormalizationTypeNumber == GetLCDefs::LIN ) {
		norm = etot*inv_eCalibTot;
	      }
	      else if ( m_NormalizationTypeNumber == GetLCDefs::LOG ) {
                // cluster has to have at least 1% of the calib hit E
                norm = log10(etot*inv_eCalibTot)+2.0;
	      }
	      else if ( m_NormalizationTypeNumber == GetLCDefs::NCLUS ) {
                norm = inv_nClusECalibGt0;
	      }
	      else {
		norm = 1.0;
	      }
	      if ( norm > 0 ) {
		m_ooc[iO]->Fill(fabs(pClus->eta()),log10(lamb),eout/eng/isol,norm);
	      }
	    }
	  }
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

void GetLCOutOfCluster::mapinsert(const std::vector<Gaudi::Histo1DDef> & dims) {
  for (unsigned int i=0;i<dims.size();i++) {
    m_dimensionsmap[dims[i].title()] = dims[i];
  }
}

void GetLCOutOfCluster::mapparse() {

  for (std::pair<const std::string, Gaudi::Histo1DDef>& p : m_dimensionsmap) {
    m_dimensions.push_back(p.second);
    ATH_MSG_DEBUG(" New Dimension: " 
		  << p.second.title() << ", [" << p.second.lowEdge()
		  << ", " << p.second.highEdge() 
		  << ", " << p.second.bins()
		  << "]");
  }
}

//###############################################################################

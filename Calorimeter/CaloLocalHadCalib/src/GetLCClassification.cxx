/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: GetLCClassification.cxx,v 1.1.1.1 2008-11-04 08:56:11 menke Exp $
//
// Description: see GetLCClassification.h
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
#include "CaloLocalHadCalib/GetLCClassification.h"

//---------------
// C++ Headers --
//---------------
#include "CaloLocalHadCalib/GetLCDefs.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h" 
#include "TFile.h"
#include "TH2F.h"
#include "TString.h"
#include <iterator>
#include <cmath>

//###############################################################################
GetLCClassification::GetLCClassification(const std::string& name, 
					 ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_outputFile(0),
    m_clusterCollName("CaloTopoCluster"),
    m_NormalizationType("Lin"),
    m_NormalizationTypeNumber(0),
    m_ClassificationType("None"),
    m_ClassificationTypeNumber(0)
{
 
  std::vector<Gaudi::Histo1DDef> dims(6);
  dims[0] = Gaudi::Histo1DDef("side",-1.5,1.5,1);
  dims[1] = Gaudi::Histo1DDef("|eta|",0.,5.,25);
  dims[2] = Gaudi::Histo1DDef("phi",-M_PI,M_PI,1);
  dims[3] = Gaudi::Histo1DDef("log10(E_clus (MeV))",log10(200),log10(1e6),14);
  dims[4] = Gaudi::Histo1DDef("log10(<rho_cell (MeV/mm^3)>)-log10(E_clus (MeV))",-9.0,-4.0,20);
  dims[5] = Gaudi::Histo1DDef("log10(lambda_clus (mm))",0.0,4.0,20);

  mapinsert(dims);

  // Dimensions to use for classification
  declareProperty("ClassificationDimensions",m_dimensionsmap);

  // Name of output file to save histograms in
  declareProperty("OutputFileName",m_outputFileName);  
  
  // Name of ClusterContainer to use
  declareProperty("ClusterCollectionName",m_clusterCollName);  

  // Normalization type "Const", "Lin", "Log", "NClus"
  declareProperty("NormalizationType",m_NormalizationType); 

  // Classification type "None" for single pion MC or 
  //                     "ParticleID_EM" for ParticleID based em-type clusters  
  //                     "ParticleID_HAD" for ParticleID based had-type clusters  
  declareProperty("ClassificationType", m_ClassificationType);

}

//###############################################################################

GetLCClassification::~GetLCClassification()
{ }

//###############################################################################

StatusCode GetLCClassification::initialize()
{
  //---- initialize the StoreGateSvc ptr ----------------
  
  m_outputFile = new TFile(m_outputFileName.c_str(),"RECREATE");
  m_outputFile->cd();
  m_hclus.resize(0);
  mapparse();

  if ( m_NormalizationType == "Lin" ) {
    ATH_MSG_INFO( "Using weighting proportional to E_calib"  );
    m_NormalizationTypeNumber = GetLCDefs::LIN;
  }
  else if ( m_NormalizationType == "Log" ) {
    ATH_MSG_INFO( "Using weighting proportional to log(E_calib)"  );
    m_NormalizationTypeNumber = GetLCDefs::LOG;
  }
  else if ( m_NormalizationType == "NClus" ) {
    ATH_MSG_INFO( "Using weighting proportional to 1/N_Clus_E_calib>0"  );
    m_NormalizationTypeNumber = GetLCDefs::NCLUS;
  }
  else {
    ATH_MSG_INFO( "Using constant weighting"  );
    m_NormalizationTypeNumber = GetLCDefs::CONST;
  }

  if ( m_ClassificationType == "None" ) {
    ATH_MSG_INFO( "Expecting single particle input"  );
    m_ClassificationTypeNumber = GetLCDefs::NONE;
  }
  else if ( m_ClassificationType == "ParticleID_EM" ) {
    ATH_MSG_INFO( "Expecting ParticleID simulation as input -- use EM type clusters only"  );
    m_ClassificationTypeNumber = GetLCDefs::PARTICLEID_EM;
  }
  else if ( m_ClassificationType == "ParticleID_HAD" ) {
    ATH_MSG_INFO( "Expecting ParticleID simulation as input -- use HAD type clusters only"  );
    m_ClassificationTypeNumber = GetLCDefs::PARTICLEID_HAD;
  }
  else {
    ATH_MSG_WARNING( " unknown classification type " << m_ClassificationType << " given! Using None instead"  );
    m_ClassificationTypeNumber = GetLCDefs::NONE;
  }

  int iside(-1);
  int ieta(-1);
  int iphi(-1);
  int ilogE(-1);
  int ilogrho(-1);
  int iloglambda(-1);
  
  m_isampmap.resize(4,-1);
  for(unsigned int idim=0;idim<m_dimensions.size();idim++) {
    if (      m_dimensions[idim].title() == "side" ) {
      iside = idim;
      m_isampmap[0] = iside;
    }
    else if ( m_dimensions[idim].title() == "|eta|" ) {
      ieta = idim;
      m_isampmap[1] = ieta;
    }
    else if ( m_dimensions[idim].title() == "phi" ) {
      iphi = idim;
      m_isampmap[2] = iphi;
    }
    else if ( m_dimensions[idim].title() == "log10(E_clus (MeV))" ) {
      ilogE = idim;
      m_isampmap[3] = ilogE;
    }
    else if ( m_dimensions[idim].title() == "log10(<rho_cell (MeV/mm^3)>)-log10(E_clus (MeV))" )
      ilogrho = idim;
    else if ( m_dimensions[idim].title() == "log10(lambda_clus (mm))" )
      iloglambda = idim;
  }
  if ( ilogE < 0 || ilogrho < 0 || iloglambda < 0 ) {
    ATH_MSG_FATAL( " Mandatory dimension log10E, log10rho or log10lambda missing ..." );
    return StatusCode::FAILURE;
  }
  int nside = (iside>=0?m_dimensions[iside].bins():1);
  int neta = (ieta>=0?m_dimensions[ieta].bins():1);
  int nphi = (iphi>=0?m_dimensions[iphi].bins():1);
  int nlogE = m_dimensions[ilogE].bins();
  m_hclus.resize(nside*neta*nphi*nlogE,0);
  for ( int jside=0;jside<nside;jside++) {
    for ( int jeta=0;jeta<neta;jeta++) {
      for ( int jphi=0;jphi<nphi;jphi++) {
	for(int jlogE=0;jlogE<nlogE;jlogE++) {
	  TString hname("hclus");
	  hname += "_iside_";
	  hname += jside;
	  hname += "_[";
	  hname += (iside>=0?m_dimensions[iside].lowEdge():-1);
	  hname += ",";
	  hname += (iside>=0?m_dimensions[iside].highEdge():-1);
	  hname += ",";
	  hname += nside;
	  hname += "]";
	  hname += "_ieta_";
	  hname += jeta;
	  hname += "_[";
	  hname += (ieta>=0?m_dimensions[ieta].lowEdge():-1);
	  hname += ",";
	  hname += (ieta>=0?m_dimensions[ieta].highEdge():-1);
	  hname += ",";
	  hname += neta;
	  hname += "]";
	  hname += "_iphi_";
	  hname += jphi;
	  hname += "_[";
	  hname += (iphi>=0?m_dimensions[iphi].lowEdge():-1);
	  hname += ",";
	  hname += (iphi>=0?m_dimensions[iphi].highEdge():-1);
	  hname += ",";
	  hname += nphi;
	  hname += "]";
	  hname += "_ilogE_";
	  hname += jlogE;
	  hname += "_[";
	  hname += m_dimensions[ilogE].lowEdge();
	  hname += ",";
	  hname += m_dimensions[ilogE].highEdge();
	  hname += ",";
	  hname += nlogE;
	  hname += "]";
	  int iH = jlogE*nphi*neta*nside+jphi*neta*nside+jeta*nside+jside;
	  m_hclus[iH] = new TH2F(hname,hname,
				 m_dimensions[ilogrho].bins(),
				 m_dimensions[ilogrho].lowEdge(),
				 m_dimensions[ilogrho].highEdge(),
				 m_dimensions[iloglambda].bins(),
				 m_dimensions[iloglambda].lowEdge(),
				 m_dimensions[iloglambda].highEdge());
	  m_hclus[iH]->Sumw2();
	  m_hclus[iH]->SetXTitle("log10(<#rho_{cell}> (MeV/mm^{3})) - log10(E_{clus} (MeV))");
	  m_hclus[iH]->SetYTitle("log10(#lambda_{clus} (mm))");
	  m_hclus[iH]->SetZTitle("Number of Clusters");
	}
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode GetLCClassification::finalize()
{
  ATH_MSG_INFO( "Writing out histograms"  );
  m_outputFile->cd();
  for(unsigned int i=0;i<m_hclus.size();i++) {
    m_hclus[i]->Write();
  }
  m_outputFile->Close();
  
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode GetLCClassification::execute()
{
  const DataHandle<xAOD::CaloClusterContainer> cc ;
  StatusCode sc = evtStore()->retrieve(cc,m_clusterCollName);

  if(sc != StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Could not retrieve ClusterContainer " 
                   << m_clusterCollName << " from StoreGate"  );
    return sc;
  }

  // total calib hit energy of all clusters 
  double eCalibTot(0.); 
  double nClusECalibGt0 = 0.0;

  xAOD::CaloClusterContainer::const_iterator clusIter = cc->begin();
  xAOD::CaloClusterContainer::const_iterator clusIterEnd = cc->end();
  for( ;clusIter!=clusIterEnd;clusIter++) {
    const xAOD::CaloCluster * theCluster = (*clusIter);      
    double eC=999; 
    if (!theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT,eC)) {
      ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_TOT"  );
      return StatusCode::FAILURE;      
    }
    if ( m_ClassificationTypeNumber != GetLCDefs::NONE ) {
      double emFrac = 0;
      if (!theCluster->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM,emFrac)) {
	ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_FRAC_EM"  );
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
    clusIter = cc->begin();
    for( ;clusIter!=clusIterEnd;clusIter++) {
      const xAOD::CaloCluster * pClus = (*clusIter);
      double eng = pClus->e();
      if ( eng > 0 ) { 
	if ( m_ClassificationTypeNumber != GetLCDefs::NONE ) {
	  double emFrac = 0;
	  if (pClus->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM,emFrac)) {
	    ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_FRAC_EM"  );
	    return StatusCode::FAILURE;      
	  }
	  if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_EM && emFrac < 0.5 )
	    continue;
	  if (m_ClassificationTypeNumber == GetLCDefs::PARTICLEID_HAD && emFrac > 0.5 )
	    continue;
	}

	double eta = fabs(pClus->eta());
      
	int iside = 0;
	int ieta = 0;
	int iphi = 0;
	int ilogE = 0;
	int nside = 1;
	int neta = 1;
	int nphi = 1;
	int nlogE = 1;
	
	if ( m_isampmap[0] >= 0 ) {
	  const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[0]];
	  nside = hd.bins();
	  iside = (int)(nside*(((pClus->eta()<0?-1.0:1.0) - hd.lowEdge())
			       /(hd.highEdge()-hd.lowEdge())));
	  if ( iside < 0 || iside > nside-1 ) {
	    ATH_MSG_WARNING( " Side index out of bounds " <<
                             iside << " not in [0," << nside-1 << "]"  );
	    iside = -1;
	  }
	}
	
	if ( m_isampmap[1] >= 0 ) {
	  const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[1]];
	  neta = hd.bins();
	  ieta = (int)(neta*((eta - hd.lowEdge())
			     /(hd.highEdge()-hd.lowEdge())));
	  if ( ieta < 0 || ieta > neta-1 ) {
	    ATH_MSG_WARNING( " Eta index out of bounds " <<
                             ieta << " not in [0," << neta-1 << "]"  );
	    ieta = -1;
	  }
	}
	if ( m_isampmap[2] >= 0 ) {
	  const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[2]];
	  nphi = hd.bins();
	  iphi = (int)(nphi*((pClus->phi() - hd.lowEdge())
			     /(hd.highEdge()-hd.lowEdge())));
	  if ( iphi < 0 || iphi > nphi-1 ) {
	    ATH_MSG_WARNING( " Phi index out of bounds " <<
                             iphi << " not in [0," << nphi-1 << "]"  );
	    iphi = -1;
	  }
	}
	
	const Gaudi::Histo1DDef & hd = m_dimensions[m_isampmap[3]];
	nlogE = hd.bins();
	ilogE = (int)(nlogE*((log10(eng) - hd.lowEdge())
			     /(hd.highEdge()-hd.lowEdge())));
	if ( ilogE >= 0 && ilogE < nlogE ) {
	  double dens=0,lamb=0,ecal=0;
	  if (!pClus->retrieveMoment(xAOD::CaloCluster::ENG_CALIB_TOT,ecal)) {
	    ATH_MSG_ERROR( "Failed to retrieve cluster moment ENG_CALIB_TOT"  );
	    return StatusCode::FAILURE;      
	  }
	  if (!pClus->retrieveMoment(xAOD::CaloCluster::FIRST_ENG_DENS,dens)) {
	    ATH_MSG_ERROR( "Failed to retrieve cluster moment FIRST_ENG_DENS"  );
	    return StatusCode::FAILURE;      
	  }
	  if (!pClus->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,lamb)) {
	    ATH_MSG_ERROR( "Failed to retrieve cluster moment CENTER_LAMBDA"  );
	    return StatusCode::FAILURE;      
	  }
	  if ( dens > 0 && 
	       lamb > 0 && 
	       ecal > 0 ) {
	    int iH = ilogE*nphi*neta*nside+iphi*neta*nside+ieta*nside+iside;
	    if ( m_hclus[iH]) {
	      double norm = 0.0;
	      if ( m_NormalizationTypeNumber == GetLCDefs::LIN ) {
		norm = ecal*inv_eCalibTot;
	      }
	      else if ( m_NormalizationTypeNumber == GetLCDefs::LOG ) {
		if ( ecal > 0 ) {
		  // cluster has to have at least 1% of the calib hit E
		  norm = log10(ecal*inv_eCalibTot)+2.0;
		}
	      }
	      else if ( m_NormalizationTypeNumber == GetLCDefs::NCLUS ) {
		if ( ecal > 0 ) {
		  norm = inv_nClusECalibGt0;
		}
	      }
	      else {
		norm = 1.0;
	      }
	      if ( norm > 0 ) {
		m_hclus[iH]->Fill(log10(dens)-log10(eng),log10(lamb),norm);
	      }
	    }
	  }
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

void GetLCClassification::mapinsert(const std::vector<Gaudi::Histo1DDef> & dims) {
  for (unsigned int i=0;i<dims.size();i++) {
    m_dimensionsmap[dims[i].title()] = dims[i];
  }
}

void GetLCClassification::mapparse() {

  std::map<std::string,Gaudi::Histo1DDef>::iterator miter = m_dimensionsmap.begin();
  std::map<std::string,Gaudi::Histo1DDef>::iterator mend = m_dimensionsmap.end();
  
  for( ; miter != mend; miter++ ) {
    m_dimensions.push_back(miter->second);
    ATH_MSG_DEBUG( " New Dimension: " 
	<< miter->second.title() << ", [" << miter->second.lowEdge()
	<< ", " << miter->second.highEdge() 
	<< ", " << miter->second.bins()
                   << "]"  );
  }
}

//###############################################################################

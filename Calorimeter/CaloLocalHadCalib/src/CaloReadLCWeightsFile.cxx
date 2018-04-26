/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloLocalHadCalib/CaloReadLCWeightsFile.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloUtils/CaloSamplingHelper.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TList.h"
#include "TAxis.h"
#include "TProfile2D.h"
#include <iostream>
#include <sstream>

CaloReadLCWeightsFile::CaloReadLCWeightsFile(const std::string & name, 
					     ISvcLocator * pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator) {
  declareProperty("LCWeightFileName",m_LCWeightFileName);
  declareProperty("CorrectionKey",m_key="HadWeights");

  m_data = new CaloLocalHadCoeff();
}


CaloReadLCWeightsFile::~CaloReadLCWeightsFile() {}

StatusCode CaloReadLCWeightsFile::initDataFromFile(std::string theLCWeightFileName)
{
  
  // Find the full path to filename:
  std::string file = PathResolver::find_file (theLCWeightFileName, "DATAPATH");
  msg(MSG::INFO) << "Reading file  " << file << endmsg;

  std::vector<int> isampmap(CaloSampling::Unknown,-1);
  unsigned int nAreas(0);
  std::vector<CaloLocalHadCoeff::LocalHadArea> theAreas;
  for (unsigned int iArea=0;iArea<CaloSampling::Unknown;iArea++) {
    CaloLocalHadCoeff::LocalHadArea theArea(CaloSamplingHelper::getSamplingName((CaloSampling::CaloSample)iArea).c_str(),0,3);
    theAreas.push_back(theArea);
  }

  for (int iread=0;iread<2;iread++) {
    // first parse to define Areas; second to fill all data
    TFile* theLCWeightFile = new TFile(file.c_str());
    if ( !theLCWeightFile ) {
      return StatusCode::FAILURE;
    }
    TList * theKeyList = theLCWeightFile->GetListOfKeys();
    while ( theKeyList->GetEntries() ) {
      TProfile2D * prof = (TProfile2D *)theLCWeightFile->Get(theKeyList->First()->GetTitle());
      // parse histogram title to find sampling and bins
      std::string sTitle(prof->GetTitle());
      // first the Sampling
      std::string key("_isamp_");
      size_t found = sTitle.find(key);
      if ( found == std::string::npos ) {
	msg(MSG::ERROR) << "Could not find key " << key << " in current histogram." << endmsg;
      }
      else {
	std::istringstream tstr(sTitle.substr(found+key.length()));
	int isamp(-1);
	tstr >> isamp;
	if ( isamp <= 0 || isamp >= CaloSampling::Unknown ) {
	  msg(MSG::ERROR) << "Found invalid sampling " << isamp << " in current histogram." << endmsg;
	}
	else {
	  // now the other dimensions
	  bool allValid(true);

	  std::vector<std::string> keys;
	  keys.push_back(std::string("_iside_"));
	  keys.push_back(std::string("_ieta_"));
	  keys.push_back(std::string("_iphi_"));
	  
	  std::vector<std::string> names;
	  names.push_back(std::string("side"));
	  names.push_back(std::string("|eta|"));
	  names.push_back(std::string("phi"));
	  names.push_back(std::string(prof->GetXaxis()->GetTitle()));
	  names.push_back(std::string(prof->GetYaxis()->GetTitle()));
	  
	  std::vector<int> types;
	  types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
	  types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
	  types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
	  types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
	  types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
	  
	  std::vector<int> ibin(names.size(),-1);
	  std::vector<double> rmin(names.size(),-1);
	  std::vector<double> rmax(names.size(),-1);
	  std::vector<int> nbin(names.size(),-1);
	  unsigned int idim;
	  
	  for (idim=0;idim<keys.size();idim++) {
	    found = sTitle.find(keys[idim]);
	    if ( found == std::string::npos ) {
	      msg(MSG::ERROR) << "Could not find key " << keys[idim] << " in current histogram." << endmsg;
	      allValid = false;
	    }
	    else {
	      char c;
	      std::istringstream tstr(sTitle.substr(found+keys[idim].length()));
	      tstr >> ibin[idim] >> c >> c >> rmin[idim] >> c >> rmax[idim] >> c >> nbin[idim];
	      if ( ibin[idim] < 0 || ibin[idim] >= nbin[idim] ) {
		msg(MSG::ERROR) << "Found invalid bin number " << ibin[idim] << " not in valid range [0," << nbin[idim] << " in current histogram." << endmsg;
		allValid = false;
	      }
	    }
	  }
	  if ( allValid ) {
	    // final 2 dimensions are from TProfile2D itself
	    nbin[names.size()-2] = prof->GetNbinsX();
	    rmin[names.size()-2] = prof->GetXaxis()->GetXmin();
	    rmax[names.size()-2] = prof->GetXaxis()->GetXmax();
	    nbin[names.size()-1] = prof->GetNbinsY();
	    rmin[names.size()-1] = prof->GetYaxis()->GetXmin();
	    rmax[names.size()-1] = prof->GetYaxis()->GetXmax();
	    // book new area from the first histo found for current sampling
	    if ( isampmap[isamp] < 0 ) {
	      isampmap[isamp] = nAreas;
	      nAreas++;
	      for (idim = 0;idim<names.size();idim++) {
		CaloLocalHadCoeff::LocalHadDimension theDim(names[idim].c_str(),types[idim],nbin[idim],rmin[idim],rmax[idim]);
		theAreas[isamp].addDimension(theDim);
	      }
	    }
	    else if ( iread == 1 ) {
	      // now fill all data for current histogram
	      TAxis * xax = prof->GetXaxis();
	      TAxis * yax = prof->GetYaxis();
	      for (ibin[names.size()-2]=0;ibin[names.size()-2]<prof->GetNbinsX();ibin[names.size()-2]++) {
		for (ibin[names.size()-1]=0;ibin[names.size()-1]<prof->GetNbinsY();ibin[names.size()-1]++) {
		  float logEClus = xax->GetBinCenter(ibin[names.size()-2]+1);
		  float logEDensCell = yax->GetBinCenter(ibin[names.size()-1]+1);
		  int iBin = prof->FindBin(logEClus,logEDensCell);
		  
		  CaloLocalHadCoeff::LocalHadCoeff theData(3,0);
		  theData[CaloLocalHadDefs::BIN_WEIGHT]  = prof->GetBinContent(iBin);
		  theData[CaloLocalHadDefs::BIN_ENTRIES] = prof->GetBinEntries(iBin);
		  theData[CaloLocalHadDefs::BIN_ERROR]   = prof->GetBinError(iBin);
		  
		  msg(MSG::INFO) << "Now set data for isamp=" << isamp
		      << " and bins: ";
		  for(unsigned int ii=0;ii<ibin.size();ii++)
		    msg() << ibin[ii] << " ";
		  msg() << endmsg;
		  m_data->setCoeff(m_data->getBin(isampmap[isamp],ibin),theData);
		}
	      }
	    }
	  }
	}
      }
      theKeyList->RemoveFirst();
    }
    if ( iread == 0 ) {
      // create the data object from all areas
      for(unsigned int jsamp=0;jsamp<theAreas.size();jsamp++) {
	if ( theAreas[jsamp].getNdim() > 0 ) { 
	  msg(MSG::INFO) << "adding Area for jsamp=" << jsamp << " with nDim = " << theAreas[jsamp].getNdim() << endmsg;
	  m_data->addArea(theAreas[jsamp]);
	}
      }
    }
    theLCWeightFile->Close();
    delete theLCWeightFile;
  }
  return StatusCode::SUCCESS;
}
                            
StatusCode CaloReadLCWeightsFile::initialize() {
  msg(MSG::INFO) << " Building CaloLocalHadCoeff object " << endmsg;
  StatusCode sc;
  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     msg(MSG::ERROR) << "Unable to get the DetectorStore" << endmsg;
     return sc;
  }
  sc=initDataFromFile(m_LCWeightFileName);
  if (sc.isFailure()) {
     msg(MSG::ERROR) << "Unable to read input Data File" << endmsg;
     return sc;
  }
  sc=detStore->record(m_data,m_key);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to record CaloLocalHadCoeff" << endmsg;
    return sc;
  }
  sc=detStore->setConst(m_data);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to lock CaloLocalHadCoeff" << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;
}


StatusCode  CaloReadLCWeightsFile::execute() 
{ 
  return StatusCode::SUCCESS;
}

StatusCode  CaloReadLCWeightsFile::finalize() 
{ 
  return StatusCode::SUCCESS;
}

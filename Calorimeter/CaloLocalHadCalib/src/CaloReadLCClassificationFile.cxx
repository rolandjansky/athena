/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloLocalHadCalib/CaloReadLCClassificationFile.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TAxis.h"
#include "TProfile2D.h"
#include <iostream>
#include <sstream>


CaloReadLCClassificationFile::CaloReadLCClassificationFile(const std::string & name, 
							   ISvcLocator * pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator) {
  declareProperty("LCClassificationFileName",m_LCClassificationFileName);
  declareProperty("ClassificationKey",m_key="EMFrac");

  m_data = new CaloLocalHadCoeff();
}


CaloReadLCClassificationFile::~CaloReadLCClassificationFile() {}


StatusCode CaloReadLCClassificationFile::initDataFromFile(std::string theLCClassificationFileName)
{
  // Find the full path to filename:
  std::string file = PathResolver::find_file (theLCClassificationFileName, "DATAPATH");
  msg(MSG::INFO) << "Reading file  " << file << endmsg;
  TFile* theLCClassificationFile = new TFile(file.c_str());
  if ( !theLCClassificationFile ) {
    return StatusCode::FAILURE;
  }
  
  CaloLocalHadCoeff::LocalHadArea theArea("Classification",0,3);

  TList * theKeyList = theLCClassificationFile->GetListOfKeys();
  while ( theKeyList->GetEntries() ) {
    TProfile2D * prof = (TProfile2D *)theLCClassificationFile->Get(theKeyList->First()->GetTitle());
    // parse histogram title to find all dimensions and bins
    std::string sTitle(prof->GetTitle());
    bool allValid(true);

    std::vector<std::string> keys;
    keys.push_back(std::string("_iside_"));
    keys.push_back(std::string("_ieta_"));
    keys.push_back(std::string("_iphi_"));
    keys.push_back(std::string("_ilogE_"));
	  
    std::vector<std::string> names;
    names.push_back(std::string("side"));
    names.push_back(std::string("|eta|"));
    names.push_back(std::string("phi"));
    names.push_back(std::string("log10(E_clus (MeV))"));
    names.push_back(std::string(prof->GetXaxis()->GetTitle()));
    names.push_back(std::string(prof->GetYaxis()->GetTitle()));
	  
    std::vector<int> types;
    types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
    
    std::vector<int> ibin(names.size(),-1);
    std::vector<double> rmin(names.size(),-1);
    std::vector<double> rmax(names.size(),-1);
    std::vector<int> nbin(names.size(),-1);
    unsigned int idim;
	  
    for (idim=0;idim<keys.size();idim++) {
      size_t found = sTitle.find(keys[idim]);
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
      // book new area from the first histo found 
      if ( theArea.getNdim() == 0 ) {
	for (idim = 0;idim<names.size();idim++) {
	  CaloLocalHadCoeff::LocalHadDimension theDim(names[idim].c_str(),types[idim],nbin[idim],rmin[idim],rmax[idim]);
	  theArea.addDimension(theDim);
	}
	msg(MSG::INFO) << "adding Area with nDim = " << theArea.getNdim() << endmsg;
	m_data->addArea(theArea);
      }
      // now fill all data for current histogram
      TAxis * xax = prof->GetXaxis();
      TAxis * yax = prof->GetYaxis();
      for (ibin[names.size()-2]=0;ibin[names.size()-2]<prof->GetNbinsX();ibin[names.size()-2]++) {
	for (ibin[names.size()-1]=0;ibin[names.size()-1]<prof->GetNbinsY();ibin[names.size()-1]++) {
	  float logEDens = xax->GetBinCenter(ibin[names.size()-2]+1);
	  float loglambda = yax->GetBinCenter(ibin[names.size()-1]+1);
	  int iBin = prof->FindBin(logEDens,loglambda);
		  
	  CaloLocalHadCoeff::LocalHadCoeff theData(3,0);
	  theData[CaloLocalHadDefs::BIN_WEIGHT]  = prof->GetBinContent(iBin);
	  theData[CaloLocalHadDefs::BIN_ENTRIES] = prof->GetBinEntries(iBin);
	  theData[CaloLocalHadDefs::BIN_ERROR]   = prof->GetBinError(iBin);
		  
	  msg(MSG::INFO) << "Now set data for bins: ";
	  for(unsigned int ii=0;ii<ibin.size();ii++)
	    msg() << ibin[ii] << " ";
	  msg() << endmsg;
	  m_data->setCoeff(m_data->getBin(0,ibin),theData);
	}
      }
    }
    theKeyList->RemoveFirst();
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloReadLCClassificationFile::initialize() {
  msg(MSG::INFO) << " Building CaloLocalHadCoeff object " << endmsg;
  StatusCode sc;
  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     msg(MSG::ERROR) << "Unable to get the DetectorStore" << endmsg;
     return sc;
  }
  sc=initDataFromFile(m_LCClassificationFileName);
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

StatusCode  CaloReadLCClassificationFile::execute() 
{ 
  return StatusCode::SUCCESS;
}

StatusCode  CaloReadLCClassificationFile::finalize() 
{ 
  return StatusCode::SUCCESS;
}

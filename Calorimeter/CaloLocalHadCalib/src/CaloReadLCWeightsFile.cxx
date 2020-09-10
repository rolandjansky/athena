/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
}


CaloReadLCWeightsFile::~CaloReadLCWeightsFile() {}

StatusCode CaloReadLCWeightsFile::initDataFromFile(const std::string& theLCWeightFileName,
                                                   CaloLocalHadCoeff& data)
{
  // Find the full path to filename:
  std::string file = PathResolver::find_file (theLCWeightFileName, "DATAPATH");
  ATH_MSG_INFO( "Reading file  " << file  );

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
	ATH_MSG_ERROR( "Could not find key " << key << " in current histogram."  );
      }
      else {
	std::istringstream tstr(sTitle.substr(found+key.length()));
	int isamp(-1);
	tstr >> isamp;
	if ( isamp <= 0 || isamp >= CaloSampling::Unknown ) {
	  ATH_MSG_ERROR( "Found invalid sampling " << isamp << " in current histogram."  );
	}
	else {
	  // now the other dimensions
	  bool allValid(true);

	  std::vector<std::string> keys;
	  keys.emplace_back("_iside_");
	  keys.emplace_back("_ieta_");
	  keys.emplace_back("_iphi_");
	  
	  std::vector<std::string> names;
	  names.emplace_back("side");
	  names.emplace_back("|eta|");
	  names.emplace_back("phi");
	  names.emplace_back(prof->GetXaxis()->GetTitle());
	  names.emplace_back(prof->GetYaxis()->GetTitle());
	  
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
	      ATH_MSG_ERROR( "Could not find key " << keys[idim] << " in current histogram."  );
	      allValid = false;
	    }
	    else {
	      char c;
	      std::istringstream tstr(sTitle.substr(found+keys[idim].length()));
	      tstr >> ibin[idim] >> c >> c >> rmin[idim] >> c >> rmax[idim] >> c >> nbin[idim];
	      if ( ibin[idim] < 0 || ibin[idim] >= nbin[idim] ) {
		ATH_MSG_ERROR( "Found invalid bin number " << ibin[idim] << " not in valid range [0," << nbin[idim] << " in current histogram."  );
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
		  
		  msg() << MSG::INFO << "Now set data for isamp=" << isamp
		      << " and bins: ";
		  for(unsigned int ii=0;ii<ibin.size();ii++)
		    msg() << ibin[ii] << " ";
		  msg() << endmsg;
		  data.setCoeff(data.getBin(isampmap[isamp],ibin),theData);
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
	  ATH_MSG_INFO( "adding Area for jsamp=" << jsamp << " with nDim = " << theAreas[jsamp].getNdim()  );
	  data.addArea(theAreas[jsamp]);
	}
      }
    }
    theLCWeightFile->Close();
    delete theLCWeightFile;
  }
  return StatusCode::SUCCESS;
}
                            
StatusCode CaloReadLCWeightsFile::initialize() {
  ATH_MSG_INFO( " Building CaloLocalHadCoeff object "  );
  auto data = std::make_unique<CaloLocalHadCoeff>();
  ATH_CHECK( initDataFromFile(m_LCWeightFileName, *data) );
  ATH_CHECK( detStore()->record(std::move(data), m_key, false) );
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

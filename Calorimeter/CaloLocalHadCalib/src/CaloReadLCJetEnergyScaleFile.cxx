/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloLocalHadCalib/CaloReadLCJetEnergyScaleFile.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TAxis.h"
#include "TProfile2D.h"
#include <iostream>
#include <sstream>


CaloReadLCJetEnergyScaleFile::CaloReadLCJetEnergyScaleFile(const std::string & name, 
							   ISvcLocator * pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator) {
  declareProperty("LCJetEnergyScaleFileNames",m_LCJetEnergyScaleFileNames);
  declareProperty("LCJetEnergyScaleJetCollectionNames",m_LCJetEnergyScaleJetCollectionNames);
  declareProperty("CorrectionKey",m_key="JESCorrection");
}


CaloReadLCJetEnergyScaleFile::~CaloReadLCJetEnergyScaleFile() {}


StatusCode CaloReadLCJetEnergyScaleFile::initDataFromFile(std::vector<std::string> &theLCJetEnergyScaleFileNames, std::vector<std::string> &theLCJetEnergyScaleJetCollectionNames,
                                                          CaloLocalHadCoeff& data)
{
  for (unsigned int iFile=0;iFile<theLCJetEnergyScaleFileNames.size();iFile++) {
    // Find the full path to filename:
    std::string file = PathResolver::find_file (theLCJetEnergyScaleFileNames[iFile], "DATAPATH");
    ATH_MSG_INFO( "Reading file  " << file  );
    TFile* theLCJetEnergyScaleFile = new TFile(file.c_str());
    if ( !theLCJetEnergyScaleFile ) {
      return StatusCode::FAILURE;
    }
  
    CaloLocalHadCoeff::LocalHadArea theArea(theLCJetEnergyScaleJetCollectionNames[iFile].c_str(),0,3);
  
    TList * theKeyList = theLCJetEnergyScaleFile->GetListOfKeys();
    while ( theKeyList->GetEntries() ) {
      TProfile2D * prof = (TProfile2D *)theLCJetEnergyScaleFile->Get(theKeyList->First()->GetTitle());
      // parse histogram title to find all dimensions and bins
      std::string sTitle(prof->GetTitle());
      bool allValid(true);

      std::vector<std::string> keys;
      keys.emplace_back("ieta_inv_");
	  
      std::vector<std::string> names;
      names.emplace_back("|eta|");
      names.emplace_back("side");
      names.emplace_back("phi");
      // the next two should actually come from the histograms
      names.emplace_back("log10(E_jet (MeV))");
      names.emplace_back("frac_low_E_clus");
      //    names.push_back(std::string(prof->GetXaxis()->GetTitle()));
      //    names.push_back(std::string(prof->GetYaxis()->GetTitle()));
      
      std::vector<int> types;
      types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
      types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
      types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
      types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
      types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
      
      std::vector<int> ibin(names.size(),-1);
      std::vector<double> rmin(names.size(),-1);
      std::vector<double> rmax(names.size(),-1);
      std::vector<int> nbin(names.size(),-1);
      unsigned int idim;
	  
      for (idim=0;idim<keys.size();idim++) {
	size_t found = sTitle.find(keys[idim]);
	if ( found == std::string::npos ) {
	  ATH_MSG_ERROR( "Could not find key " << keys[idim] << " in current histogram."  );
	  allValid = false;
	}
	else {
	  std::istringstream tstr(sTitle.substr(found+keys[idim].length()));
	  tstr >> ibin[idim];
	  
	  // these should come from the histogram title eventually ...
	  nbin[idim] = 25;
	  rmin[idim] = 0.; // |eta| min
	  rmax[idim] = 5.; // |eta| max
	  
	  if ( ibin[idim] < 0 || ibin[idim] >= nbin[idim] ) {
	    ATH_MSG_ERROR( "Found invalid bin number " << ibin[idim] << " not in valid range [0," << nbin[idim] << " in current histogram."  );
	    allValid = false;
	  }
	}
      }
      if ( allValid ) {
	// next 2 are side and phi
	ibin[1] = 0;
	nbin[1] = 1;
	rmin[1] = -1.5; // side min
	rmax[1] =  1.5; // side max
	
	ibin[2] = 0;
	nbin[2] = 1;
	rmin[2] = -M_PI; // phi min
	rmax[2] =  M_PI; // phi max
	
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
	  ATH_MSG_INFO( "adding Area with nDim = " << theArea.getNdim()  );
	  data.addArea(theArea);
	}
	// now fill all data for current histogram
	TAxis * xax = prof->GetXaxis();
	TAxis * yax = prof->GetYaxis();
	for (ibin[names.size()-2]=0;ibin[names.size()-2]<prof->GetNbinsX();ibin[names.size()-2]++) {
	  for (ibin[names.size()-1]=0;ibin[names.size()-1]<prof->GetNbinsY();ibin[names.size()-1]++) {
	    float logE = xax->GetBinCenter(ibin[names.size()-2]+1);
	    float frac = yax->GetBinCenter(ibin[names.size()-1]+1);
	    int iBin = prof->FindBin(logE,frac);
	    
	    CaloLocalHadCoeff::LocalHadCoeff theData(3,0);
	    theData[CaloLocalHadDefs::BIN_WEIGHT]  = prof->GetBinContent(iBin);
	    theData[CaloLocalHadDefs::BIN_ENTRIES] = prof->GetBinEntries(iBin);
	    theData[CaloLocalHadDefs::BIN_ERROR]   = prof->GetBinError(iBin);
	    
	    msg() << MSG::INFO << "Now set data for bins: ";
	    for(unsigned int ii=0;ii<ibin.size();ii++)
	      msg() << ibin[ii] << " ";
	    msg() << endmsg;
            int dbin = data.getBin(iFile,ibin);
            if (dbin >= 0)
              data.setCoeff(dbin,theData);
	  }
	}
      }
      theKeyList->RemoveFirst();
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CaloReadLCJetEnergyScaleFile::initialize() {
  ATH_MSG_INFO( " Building CaloLocalHadCoeff object "  );
  auto data = std::make_unique<CaloLocalHadCoeff>();
  ATH_CHECK( initDataFromFile(m_LCJetEnergyScaleFileNames, m_LCJetEnergyScaleJetCollectionNames, *data) );
  ATH_CHECK( detStore()->record(std::move(data), m_key, false) );
  return StatusCode::SUCCESS;
}

StatusCode  CaloReadLCJetEnergyScaleFile::execute() 
{ 
  return StatusCode::SUCCESS;
}

StatusCode  CaloReadLCJetEnergyScaleFile::finalize() 
{ 
  return StatusCode::SUCCESS;
}

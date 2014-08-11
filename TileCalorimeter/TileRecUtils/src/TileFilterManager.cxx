/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************************************************
// Filename : TileFilterManager.cxx
// Author   : F. Merritt, A. Aurisano
// Created  : March 2004

//**************************************************************************************

#include "TileRecUtils/TileFilterManager.h"
#include "TileConditions/TileInfo.h"

#include <algorithm>
#include <iostream>
#include "boost/io/ios_state.hpp"

// =======================================================================================

// Constructor
TileFilterManager::TileFilterManager(int mode, int level, int &npMax, int &nSam, int &inTdig, int &ibSam, 
				     int &ieSam, int &bCr, int &eCr, int &nShape,
				     int &inShape, std::vector<double> &Shape, bool lDebug)
  : map_ind2icr()
  , map_icr2ind()
{
  /* Store information needed for fitting. */

  debug = lDebug;     // Stored as a private data member.
  Fmode = mode;
  Flevel = level;
  NParamMax = npMax;
  Nshape = nShape;
  InTshape = inShape;
  Ndig=nSam;
  InTdig=inTdig;
  jBsamp=ibSam;
  jEsamp=ieSam;
  jBcross=bCr;
  jEcross=eCr;
  InTcross = -jBcross;
  Ncross=jEcross-jBcross+1;
  std::cout << " TileFilterManager constructor.  NParamMax =" << NParamMax << ", Ndig =" << Ndig << std::endl;

  // Parameters that should be read in externally.
  rchisqCut = 4.0;
  chiCut = 3.5;
  int NParamMin = 2;
  std::cout << " FilterMode=" << Fmode << ", NParamMax=" << NParamMax << ", NParamMin="
       << NParamMin << ", chiCut=" << chiCut << ", rchisqCut=" << rchisqCut << std::endl;
  //
  // Create the tables of offsets for calculating Fitter indices.

  MakeFitterOffsetTables();

  //
  // Create the maps taking us from "digit number" idig to "crossing index" ind.  
  // "idig" corresponds to the array of digits which are read out, ranging from zero
  // to Ndig, with the in-time digit defined by InTdig.  It is more convenient for
  // fitting to re-order these so that the index of the in-time crossing is zero,
  // and the other are sequentially numbered 1,2,..., Ncr.
  //
  std::cout << " Number of digits read =" << Ndig << " .  In-time digit has idig=" 
       << InTdig << "." << std::endl;
  int ind=0;
  for (int icr=0; icr<Ncross; icr++) {
    if(icr==InTdig)  ind = 0;
    if(icr<InTdig)   ind = icr - jBcross + 1;
    if(icr>InTdig)   ind = icr - jBcross;
    map_icr2ind[icr] = ind;
    map_ind2icr[ind] = icr;
  }
  if(debug) {
    for(int ind = 0; ind<Ncross; ind++) {
      int icr = map_ind2icr[ind];
      int kdisp = icr - InTdig;
      std::cout << " Index =" << ind << "  crossing #  =" << icr 
	   << ", displacement from InTdig =" << kdisp << " crossings." << std::endl;
    }
  }
/* Create the shaping matrix.  Row = crossing (of deposit), col = digit. */
  //  std::vector<double> ShapeMat[Ncross][Ndig];
  for (int ind = 0; ind<Ncross; ind++) {
    int icr = map_ind2icr[ind];
    int OffSet = InTshape;
    double * Xshape = new double[Ndig];
    for(int idig=0; idig<Ndig; idig++) {
      int k=OffSet-icr+idig;
      if(k<0) k=0;
      if(k>=Nshape) k=Nshape-1;
      Xshape[idig]=Shape[k];
    }
    CrossShape.push_back(Xshape);
  }
  /* Print out the Shape Matrix. */
  if(debug) {
    boost::io::ios_base_all_saver coutsave (std::cout);
    std::cout << " TileFilterManager: ShapingMatrix.   Nshape=" << Nshape << ", InTshape=" << InTshape
              << ", Ndig=" << Ndig << ", InTdig=" << InTdig 
              << ", Ncross=" << Ncross << std::endl;
    for(int ind=0; ind<Ncross; ind++) {
      double * Xshape = CrossShape[ind];
      std::cout << " ind=" << ind << " Shape=" ;
      for (int idig=0; idig<Ndig; idig++) {
        std::cout << " " << std::setw(6) << std::setprecision(3) << Xshape[idig];
      }
      std::cout << std::endl;
    }
  }
  
  //  vNparam.reserve(NParamMax);
  //Print the FitterIndex arrays.
  std::vector<int> Crossings;
  //int NpileMax = NParamMax - 2;
  int NampMax = NParamMax - 1;
  for(int ipile=0; ipile<NampMax; ipile++) {
    int Nmax = NfitIndex[ipile];
    if(debug)
      std::cout << " Crossing configurations for Nparam=" << ipile+2
                << ", Npileup=" << ipile << ": " << Nmax << " configurations." << std::endl;
    for(int iconfig=0; iconfig<Nmax; iconfig++) {
      Crossings.clear();
      getVcross(ipile, iconfig, Crossings);
      int nParam = ipile+2;
      int kFitIndex = getFitIndex(nParam, Crossings);
      int ncr = Crossings.size();
      if(debug) {
	std::cout << "     Npile=" << std::setw(2) << ipile << ", iconfig=" << std::setw(3)
	     << iconfig << " (kF=" << std::setw(3) << kFitIndex << ")  => Vcross=";
	for(int icr=0; icr<ncr; icr++) {
	  std::cout << " " << std::setw(3) << Crossings[icr];
	}
	std::cout << std::endl;
      }
    }
  }
  // Make the Fitter Arrays
  /* int iok = */ MakeFitterArrays();

    /* Initialization has successfully completed.  */
  // Set debug = false for execution phase.
  debug = true;
  return;
}

// ===================================================================================

// Destructor
TileFilterManager::~TileFilterManager() {

  for (unsigned int ind = 0; ind<CrossShape.size(); ++ind) {
    delete [] CrossShape[ind];
  }

  for (unsigned int ind = 0; ind<OffsetVector.size(); ++ind) {
    delete [] OffsetVector[ind];
  }
}

// ===================================================================================

int TileFilterManager::FitDigits(TileFilterResult &tResult, bool lDebug) { 
  int icode = -1;

  //  std::cout << " FitDigits:  Fmode=" << Fmode << ", lDebug=" << lDebug << std::endl;
  if(Fmode==2) {
    icode = FitDigits1(tResult, lDebug);
    return icode;
  }
  if(Fmode==3) {
    icode = FitDigits2(tResult, lDebug);
    return icode;
  }
  std::cout << " ERROR in TileFitDigits !!  Fmode =" << Fmode << std::endl;

  return icode;
}

// ===================================================================================

int TileFilterManager::FitDigits1(TileFilterResult &tResult, bool lDebug) { 
  int icode = -1;
  debug = lDebug;
  // Get references to the key variable in tResult.
  HepVector& digits = tResult.getDigRef();
  //HepVector& fitAmp = tResult.getParamRef();
  //HepVector& fitErr = tResult.getErrRef();
  HepVector& residuals = tResult.getResidRef();
  double& chisqRef = tResult.getChisqRef();
  int& Npar = tResult.getNparRef();
  std::vector<int>& vcross = tResult.getVcrossRef();
  int& iFitIndex = tResult.getFitIndexRef();

  // First find crossing with highest amplitude.
  int jcross = FindHighestResidual(digits);
  int jparam = ((jcross<0) ? 0 : map_icr2ind[jcross]);
  //  if(debug) std::cout << " Highest crossing = " << jcross << "(cind=" << jparam
  //		 << "), amp=" << digits[jcross] << std::endl;

  // Initialize fitting parameters
  Npar=1;
  iFitIndex = -1;
  chisqRef = 999.;
  int iret = 0;

  jparam = 0; // Set this to be intime crossing [FSM, 7/30/04]
  // Include in-time crossing (jparam=0) in fit even if it is not the maximum.
  //if(jparam != 0) iret = tResult.addCross(0);

  // Start loop over fits, adding a new parameter each time until reduced chisq is OK.
  double rchisq = 999.;
  icode = -1;
  double digSigma = tResult.getSigDig();
  while(Npar<NParamMax) {
    if(debug) std::cout << " FilterManager.FitDigits1, while loop. Npar=" << Npar << ", NParamMax=" << NParamMax << std::endl;
    iret = tResult.addCross(jparam);
    if(iret != 0) {
      icode = iret;
      break;
    }
    //    if(debug) tResult.SnapShot(0);
    iFitIndex = getFitIndex(Npar, vcross);
    std::vector<TileFitter>& vFitter = vNpFitter[Npar-2];
    TileFitter& tileFitter = vFitter[iFitIndex];
    iret = tileFitter.FitAmp(tResult, false);
    if(debug) tResult.SnapShot(2);
    rchisq = chisqRef/(Ndig-Npar);

    // Chisq is small enough, so the fit has been successful.
    if(rchisq<rchisqCut) {
      icode = 0;
      break;
    }
    // Have hit max param even though chisq is still big (problem!).  
    if(Npar==NParamMax) {
      icode = 5;
      break;
    }
    // Find index of the highest residual.
    jcross = FindHighestResidual(residuals);
    jparam = ((jcross<0) ? 0 : map_icr2ind[jcross]);
    // If jparam is already in list, terminate search (problem?).
    bool ldup=false;
    int Namp = vcross.size();
    for(int i=1; i<Namp; i++) {
      if(vcross[i]==jparam) ldup=true;
    }
    if(ldup) {
      icode = 4;
      break;
    }

    double chi = ((jcross<0) ? 0.0 : residuals[jcross]/digSigma);
    if(chi<chiCut) {
      icode = 3;
      break;
    }
  }
  if(debug) {
    std::cout << " End of loop.  icode =" << icode << ", Npar=" << Npar << std::endl;
    tResult.SnapShot(1);
  }
  return icode;
}

// ===================================================================================

int TileFilterManager::FitDigits2(TileFilterResult &tResult, bool lDebug) { 
  //int iret = -1;
  int icode = 0;
  debug = lDebug;
  // Get references to the key variable in tResult.
  HepVector& digits = tResult.getDigRef();
  HepVector& fitAmp = tResult.getParamRef();
  HepVector& fitErr = tResult.getErrRef();
  //HepVector& residuals = tResult.getResidRef();
  double& chisqRef = tResult.getChisqRef();
  int& Npar = tResult.getNparRef();
  int Namp = Npar - 1;
  std::vector<int>& vcross = tResult.getVcrossRef();
  int& iFitIndex = tResult.getFitIndexRef();
  if(debug) {
    boost::io::ios_base_all_saver coutsave (std::cout);
    //    tResult.SnapShot(0);
    std::cout << " digits=" ;
    for(int i=0; i<Ndig; i++) {
      std::cout << " " << std::setw(6) << std::setprecision(2) << digits[i];
    }
    std::cout << std::endl;
  }

  // Make a crossing vector that contains all allowed amplitudes.
  Namp = Flevel - 1;
  for(int iamp=0; iamp<Namp; iamp++) {
    /*iret =*/ tResult.addCross(iamp);
  }

  //Namp and Npar could have changed. Npar is incremented by addCross.
  Namp = Npar - 1;
  if(debug) tResult.SnapShot(0);

  // Initialize fitting parameters
  iFitIndex = -1;
  chisqRef = 999.;

  // Start loop over fits, removing one or more amplitudes each time.
  // double rchisq = 999.;
  icode = -1;

  int Npass = 0;
  while(Npar>1) {
    Npass +=1;
    if(debug) std::cout << " FilterManager.FitDigits2, while loop. Npar=" 
                   << Npar << ", NParamMax=" << NParamMax << std::endl;
    //    if(debug) tResult.SnapShot(0);
    iFitIndex = getFitIndex(Npar, vcross);
    if(debug) std::cout << " Npar=" << Npar << ", iFitIndex=" << iFitIndex << std::endl;
    std::vector<TileFitter>& vFitter = vNpFitter[Npar-2];
    TileFitter& tileFitter = vFitter[iFitIndex];
    //    if(debug) std::cout << " Ready to call tileFitter.FitAmp" << std::endl;
    /*iret =*/ tileFitter.FitAmp(tResult, false);
    if(debug) tResult.SnapShot(2);
    // If Npar is down to 2 parameters (ped + inTime), terminate fitting search.
    if(Npar<=2) {
      icode = 3;
      break;
    }

    // Calculate significance chi of each fit parameter..
    // double chiCutLow[4] = {chiCut, 1.50, 0.75, 0.};
    const int Ndim = 12;
    double chiAmp[Ndim];
    int iAmp[Ndim];
    int Npile = 0;
    for(int i=2; i<Npar; i++) {
      chiAmp[Npile] = fitAmp[i]/fitErr[i];
      iAmp[Npile] = vcross[i-1];
      if(debug) std::cout << " set chiAmp: i=" << i << ", iAmp=" << iAmp[Npile] << ",  chi=" << chiAmp[Npile] << std::endl;  
      Npile += 1;
    }
    int ndrop = 0;
    int crdrop = 0;
    int ndropMax = 4;
    while(ndrop<ndropMax) {
      if(debug) std::cout << " top of drop loop.  ndrop=" << ndrop << ", Npass=" << Npass << std::endl;
      double chiMin = chiCut;
      //      chiMin = chiCutLow[ndrop];
      int idrop = -1;
      for (int i=0; i<Npile; i++) {
	//	if(debug) std::cout << "drop candidate: i=" << i << ", iAmp=" << iAmp[i]
	//       << ", chiAmp=" << chiAmp[i] <<", chiMin=" << chiMin << std::endl;
        if(iAmp[i]<0) continue;
        if(chiAmp[i]>chiMin) continue;
        chiMin = chiAmp[i];
        idrop = i;
	crdrop = iAmp[i];
      }
      if(debug) std::cout << " end of Npile loop.  idrop=" << idrop << ", crdrop=" << crdrop << ", ndrop=" << ndrop << std::endl; 
      if(idrop>-1) {
        /*iret =*/ tResult.dropCross(crdrop);
        ndrop += 1;
	iAmp[idrop] = - iAmp[idrop];
        icode = 1;
        if(debug) std::cout << " ndrop=" << ndrop << ", idrop=" << idrop 
		       << ", crdrop=" << crdrop << ", chiMin=" << chiMin << std::endl;
      } else {
        icode = 6;
        break;
      }
      if(debug) std::cout << "FitDig2:  Npass=" << Npass << ", ndrop=" << ndrop << std::endl;
    } // end of ndrop loop
    if(debug) std::cout << " have fallen out of drop loop.  ndrop=" << ndrop << ", Npass=" << Npass << std::endl;
    if(ndrop==0) {
      icode=0;
      break;
    }
  } // end of while loop

  if(debug) {
    std::cout << " TileFilterManager:  End of pass loop.  icode =" << icode << ", Npar=" << Npar 
	 << ", Npass=" << Npass << std::endl;
    tResult.SnapShot(2);
  }
  return icode;
}

// ===================================================================================

int TileFilterManager::FindHighestResidual(HepVector &digits) const {
  int icrMax=-1;
  double ampMax = -999.;
  for (int icr=0; icr<Ncross; icr++) {
    if(digits[icr]>ampMax) {
      icrMax = icr;
      ampMax = digits[icr];
    }
  }
  return icrMax;
}

// ===================================================================================

int TileFilterManager::FindLowestCrossing(HepVector &digits) const {
  int icrMin=-1;
  double ampMin = +9999.;
  for (int icr=0; icr<Ncross; icr++) {
    if(icr==InTdig) continue;
    if(digits[icr]<ampMin) {
      icrMin = icr;
      ampMin = digits[icr];
    }
  }
  return icrMin;
}

// ===================================================================================

void TileFilterManager::MakeFitterOffsetTables() {

  int NpileupMax = NParamMax - 2;
  int NampMax = NParamMax - 1;
  int Npileup = NpileupMax;
  if (debug)
    std::cout << " Enter MakeFitterOffsetTables:  Npileup=" << Npileup 
              << ", NampMax=" << NampMax << ", Ncross=" << Ncross << std::endl;
//For ipileup=0 (special case), offset = index of crossing.
  int * Offset0 = new int[Ncross];
  for(int index=0; index<Ncross; index++) {
    Offset0[index] = index;
  }
  OffsetVector.push_back(Offset0);
  //
  //For ipileup=1, number offsets sequentially starting with zero for index=1.
  int * Offset1 = new int[Ncross];
  for(int index=0; index<Ncross; index++) {
    if(index<2) Offset1[index]=0;
    else Offset1[index]=index-1;
  }
  OffsetVector.push_back(Offset1);

  // For Npileup>1, use iterative formula (offsets = coeff of binary expansion)
  if(NampMax>2) {
    for(int ipile=2; ipile<NampMax; ipile++) {
      int * vlast = OffsetVector[ipile-1];
      int * Offset = new int[Ncross];
      for(int index = 0; index<Ncross; index++) {
	if(index<=ipile) {Offset[index]=0;}
	else {Offset[index] = Offset[index-1] + vlast[index-1];}
      }
    OffsetVector.push_back(Offset);
    }
  }
  // Find the number of FitIndex values for each number of parameters.
  for(int ipile=0; ipile<NampMax; ipile++) {
    int Nmax;
    int * Offset = OffsetVector[ipile];
    if(ipile<=1) Nmax = Offset[Ncross-1] + 1;
    if(ipile>1) {
      int * vlast = OffsetVector[ipile-1];
      Nmax = Offset[Ncross-1]+vlast[Ncross-1];
    }
    NfitIndex.push_back(Nmax);
  }

 // Print out the Offset table.
  if (debug) {
    std::cout << " *** TileFilter Offset table for Npileup=" << Npileup 
              << " and Ncross=" << Ncross << std::endl;
    for(int ipile=0; ipile<NampMax; ipile++) {
      int * Offset = OffsetVector[ipile];
      std::cout << "       ipile=" << std::setw(3) << ipile << ":  Offsets = ";
      for (int index=0; index<Ncross; index++) {
        std::cout << " " << std::setw(3) << Offset[index];
      }
      std::cout << ";  NfitIndex=" << std::setw(3) << NfitIndex[ipile] << std::endl;
    }
  }
  
  return;
}

// ===================================================================================

int TileFilterManager::MakeSPD(bool debugMakeSPD, std::vector<int>& vcross, HepMatrix& SPD) {
  int iret=-1;
  int Namp = vcross.size();
  int Nparam = Namp + 1;
  // First row of SPD is always for the pedestal.
  for(int idig=0; idig<Ndig; idig++) {
    SPD[0][idig] = 1.0;
  }
  // The remaining rows correspond to the crossing amplitudes specified by vcross.
  for(int ipar=1; ipar<Nparam; ipar++) {
    int jcr = vcross[ipar-1];
    double * Xshape = CrossShape[jcr];
    for(int idig=0; idig<Ndig; idig++) {
      SPD[ipar][idig] = Xshape[idig];
    }
  }
  if(debugMakeSPD) {
    std::cout << " Make SPD for NP=" << Nparam << ", vcross=";
    for(int iamp=0; iamp<Namp; iamp++) {
      std::cout << " " << vcross[iamp];
    }
    std::cout << std::endl;
    for(int ipar=0; ipar<Nparam; ipar++) {
      std::cout << " ip=" << ipar << " SPD=";
      for(int idig=0; idig<Ndig; idig++) {
	std::cout << " " << SPD[ipar][idig];
      }
      std::cout << std::endl;
    }
  } // end debugMakeSPD printout
  return iret;
}

// ===================================================================================

int TileFilterManager::MakeFitterArrays() {

  if(debug) std::cout << " TileFilterManager::MakeFitterArrays.  Will print out first matrix "
		 << "only for each vFitter vector (one for each value of Nparam)." << std::endl;
  bool cdebug = false;
  int NampMax = NParamMax - 1;
  for(int iamp=0; iamp<NampMax; iamp++) {
    int Nparam = iamp+2;       // number of parameters in this series of fits.
    int Nindex = NfitIndex[iamp];  // number of configurations for this Nparam.
    if(debug) std::cout << " ===>    Nparam=" << Nparam << " => Nindex=" << Nindex
		   << " TileFitter objects:" << std::endl;
    std::vector<TileFitter> vFitter(Nindex);
    for(int index=0; index<Nindex; index++) {
      //      if(debug) cdebug = (index==0)||(index==Nindex-1);
      if(debug) cdebug = (index==Nindex-1);
      std::vector<int> vcross;
      getVcross(iamp, index, vcross);
      HepMatrix SPD(Nparam,Ndig);
      MakeSPD(cdebug, vcross, SPD);            // fill the matrix elements
      // If constraints are needed, set Icon.
      int Icon = 0;
      if(Nparam>Ndig-1) Icon = 1;
      if(Nparam>Ndig)   Icon = 2;
      TileFitter * tileFitter = new TileFitter(cdebug, Nparam, Ndig, index, SPD, Icon);
      vFitter[index] = *tileFitter;
      delete tileFitter;
    }
    vNpFitter.push_back(vFitter);
  }
  return 0;
}

// ===================================================================================

std::vector<int>& TileFilterManager::get_NfitIndex() {
  return NfitIndex;
}

// ===================================================================================

void TileFilterManager::getCuts(double& rchisqC, double& chiC) {
  rchisqC = rchisqCut;
  chiC = chiCut;
  return;
}

// ===================================================================================

void TileFilterManager::getVcross(int nPileup, int iconfig, std::vector<int>& vcross) {

  vcross.clear();
  int kconfig = iconfig;
  int icrmax = Ncross-1;
  for(int ipile=nPileup; ipile>-1; ipile--) {
    int * Offset = OffsetVector[ipile];
    for(int icross=icrmax; icross>=0; icross--) {
      //      std::cout << "icross=" << icross << ", Offset[icross]=" << Offset[icross]
      //	   << ", kconfig=" << kconfig << ", icrmax=" << icrmax << std::endl;
      if(Offset[icross]<=kconfig) {
	int icr = icross;
	icrmax=icross-1;
	kconfig = kconfig - Offset[icross];
	vcross.push_back(icr);
	break;
      }
      if(kconfig<0) {
	std::cout << " ERROR!! In getVcross, kconfig=" << kconfig << std::endl;
      }
    }
  }
  sort(vcross.begin(), vcross.end());

  return;
}

// ===================================================================================

int TileFilterManager::getFitIndex(int Nparam, std::vector<int>& vcross) {

  int Index=0;
  int Namp = Nparam-1;
  if(Namp<=0) {
    std::cout << " TileFilterManager.getFitIndex called when Nparam=" << Nparam << std::endl;}
  for(int ipar=0; ipar<Namp; ipar++) {
    int * Offset = OffsetVector[ipar];
    int jcr = vcross[ipar];
    Index += Offset[jcr];
  }

  //  if(debug) {
  //    std::cout << " getFitIndex: Nparam=" << Nparam << ", Index=" << Index
  //	 << ", Vcross=";
  //    for(int ipar=0; ipar<Namp; ipar++) {
  //      std::cout << " " << vcross[ipar];
  //    }
  //    std::cout << std::endl;
  //  }

  return Index;
}

// ===================================================================================

std::vector<double>& TileFilterManager::getFitterErr(int Nparam, int iconfig) {
  int ipile = Nparam - 2;
  std::vector<TileFitter>& vFitter = vNpFitter[ipile];
  TileFitter& tileFitter = vFitter[iconfig];
  std::vector<double>& fitErr = tileFitter.getErrRef();
  return fitErr;
}

// ===================================================================================

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define ZDCNLCalibration_cxx
#include "ZDCNLCalibration.h"

#include <TF1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iomanip>
#include <numeric>
#include <stdexcept>

void ZDCNLCalibration::FillLumiBlockEvtMap()
{
  Long64_t numEntries = m_tree->GetEntriesFast();

  unsigned int countPassed = 0;

  int lastLumiBlock = -1;
  int LBStartEntry = -1;

  for (Long64_t entry = 0; entry < numEntries; entry++) {
    Long64_t numBytes = m_tree->GetEntry(entry);
    
    if (m_useGRL && passBits !=0) {
      //std::cout << "lumiBlock = " << lumiBlock << " passBits = " << passBits << std::endl;	
      continue; // reject based on GRL or errors
    }
    
    if (lumiBlock != lastLumiBlock) {
      if (lastLumiBlock > -1) {
	m_LumiBlockEvtMap.insert(LBEvtMap::value_type(lumiBlock, std::pair<unsigned int, unsigned int>(LBStartEntry, entry - 1)));
      }

      LBStartEntry = entry;
      lastLumiBlock = lumiBlock;
    }

    countPassed++;
  }
  
  if (m_debugLevel >= 0) {
    std::cout << "From a total of " << numEntries << " entries, " << countPassed << " were selected for analysis" << std::endl; 
  }
}

void ZDCNLCalibration::AddCalibration(size_t side, std::string tag, const CalibData& calib)
{
  std::map<std::string, CalibData>::iterator iter = m_calibrations.at(side).find(tag);
  if (iter != m_calibrations[side].end()) (*iter).second = calib;//*iter = std::pair<std::string, CalibData>(tag, calib);
  else {
    m_calibrations[side].insert(std::pair<std::string, CalibData>(tag, calib));
  }
}

CalibData ZDCNLCalibration::GetCalibration(size_t side, std::string tag)
{
  CalibData null;

  auto iter = m_calibrations.at(side).find(tag);
  if (iter != m_calibrations.at(side).end()) {
    return iter->second;
  }
  else return null;
}

std::pair<float, float> ZDCNLCalibration::FindSNRange(size_t LBLow, size_t LBHigh, size_t side)
{
  static TH1D* snHist = new TH1D("ZDCNLCalibSNHist", "", 200, 0, 2000);
  snHist->Reset();

  LBEvtMap::const_iterator LBStart = m_LumiBlockEvtMap.lower_bound(LBLow);
  LBEvtMap::const_iterator LBEnd = m_LumiBlockEvtMap.upper_bound(LBHigh);

  // Select which trigger we're going to look at based on the side
  //
  const bool& trigger = (side == 1 ? L1_ZDC_C : L1_ZDC_A);

  for ( LBEvtMap::const_iterator iter = LBStart; iter != LBEnd; ++iter) {
    unsigned int entryStart = iter->second.first;
    unsigned int entryEnd = iter->second.second;

    for (unsigned int entry = entryStart; entry < entryEnd; entry++) {
      Long64_t nb = m_tree->GetEntry(entry);
      if (nb < 0) {
	std::cout << "Error reading entry " << entry << ", quitting" << std::endl;
	throw std::runtime_error("Error reading entry for ZDCNLCalibration");
      }
      
      if (!trigger) continue;
      
      int testMask = zdc_ZdcModuleMask >> side*4;
      if ((testMask & 0xf) != 0xf) continue;
      
      float sumAmp = zdc_ZdcAmp[side];
      snHist->Fill(sumAmp);
    }
  }

  int ibin = snHist->GetMaximumBin();
  double center = snHist->GetBinCenter(ibin);
  double maxCounts = snHist->GetBinContent(ibin);

  static TF1* gaussFunc = new TF1("gaussFunc", "gaus", 100, 2000);

  gaussFunc->SetParameters(maxCounts, center, 100);
  snHist->Fit("gaussFunc", "", "", center*0.7, center*1.3);

  double gaussMean = gaussFunc->GetParameter(1);
  double gaussWidth = gaussFunc->GetParameter(2);

  double cutLow = gaussMean -2.*gaussWidth;
  double cutHigh = gaussMean + 2*gaussWidth;

  if (m_debugLevel >= 0) {
    std::cout << "SN cut range = " << cutLow << " to " << cutHigh << std::endl;
  }

  return std::pair<float, float>(cutLow, cutHigh);
}

std::pair<std::pair<float, float>,std::pair<float, float>  > 
ZDCNLCalibration::FindSNTwoNRanges(size_t LBLow, size_t LBHigh, size_t side)
{
  static TH1D* sntnHist = new TH1D("ZDCNLCalibSNTNHist", "", 200, 0, 2000);
  sntnHist->Reset();

  LBEvtMap::const_iterator LBStart = m_LumiBlockEvtMap.lower_bound(LBLow);
  LBEvtMap::const_iterator LBEnd = m_LumiBlockEvtMap.upper_bound(LBHigh);

  // Select which trigger we're going to look at based on the side
  //
  const bool& trigger = (side == 1 ? L1_ZDC_C : L1_ZDC_A);

  for ( LBEvtMap::const_iterator iter = LBStart; iter != LBEnd; ++iter) {
    unsigned int entryStart = iter->second.first;
    unsigned int entryEnd = iter->second.second;

    for (unsigned int entry = entryStart; entry < entryEnd; ++entry) {
      Long64_t nb = m_tree->GetEntry(entry);
      if (nb < 0) {
	std::cout << "Error reading entry " << entry << ", quitting" << std::endl;
	throw std::runtime_error("Error reading entry for ZDCNLCalibration");
      }
      
      if (!trigger) continue;
      
      int testMask = zdc_ZdcModuleMask >> side*4;
      if ((testMask & 0xf) != 0xf) continue;
      
      float sumAmp = zdc_ZdcAmp[side];
      sntnHist->Fill(sumAmp);
    }
  }

  int ibin = sntnHist->GetMaximumBin();
  double center = sntnHist->GetBinCenter(ibin);
  double maxCounts = sntnHist->GetBinContent(ibin);

  static TF1* twogaussFunc = new TF1("twogaussFunc", "[0]*exp(-pow((x-[1])/[2],2)/2.) + [3]*exp(-pow((x-[4])/[5],2)/2.)", 100, 2000);

  twogaussFunc->SetParameters(maxCounts, center, 100, maxCounts/2, 2*center, 200);
  sntnHist->Fit("twogaussFunc", "", "", center*0.7, 2*center*1.3);

  double gaussMean1 = twogaussFunc->GetParameter(1);
  double gaussMean2 = twogaussFunc->GetParameter(4);

  double gaussWidth1 = std::abs(twogaussFunc->GetParameter(2));
  double gaussWidth2 = std::abs(twogaussFunc->GetParameter(5));

  double cutLow1 = gaussMean1 -2.*gaussWidth1;
  double cutHigh1 = gaussMean1 + 2*gaussWidth1
;
  double cutLow2 = gaussMean2 -2.*gaussWidth2;
  double cutHigh2 = gaussMean2 + 2*gaussWidth2;

  if (cutHigh1 > cutLow2) {
    double middle = 0.5*(cutHigh1 + cutLow2);
    cutHigh1 = middle;
    cutLow2 = middle;
  }

  if (m_debugLevel >= 0) {
    std::cout << "SN cut range = " << cutLow1 << " to " << cutHigh1 << std::endl;
    std::cout << "TN cut range = " << cutLow2 << " to " << cutHigh2 << std::endl;
  }

  return std::pair<std::pair<float, float>, std::pair<float, float> >(std::pair<float, float>(cutLow1, cutHigh1), 
								      std::pair<float, float>(cutLow2, cutHigh2));
}

void ZDCNLCalibration::Calibrate(size_t side, const std::string & calibInput, const std::string & calibOutput, 
				 size_t LBLow, size_t LBHigh, std::array<int, 4> maxPowerModule,
				 std::vector<std::pair<double, double> >  nNeutERange, 
				 bool excludeHE, float heSumThresh, float HEDeweight)
{
  std::for_each(maxPowerModule.begin(), maxPowerModule.end(), 
		[](int power) {
		  if (power == 0) {
		    std::cout << "Found max power = 0, quitting" << std::endl;
		    return;
		  }
		} );

  // Set up container for sums that we will use to calcualting the weights
  //
  size_t numWeights = std::accumulate(maxPowerModule.begin(), maxPowerModule.end(), 0);
  size_t arrayDim = 4*m_maxNLPower;

  size_t arrayDim2D = arrayDim*arrayDim;

  size_t numNeutMax = nNeutERange.size();

  std::vector<double> sumsHE(arrayDim, 0);
  std::vector<double> sums2DHE(arrayDim2D, 0);

  std::vector<std::vector<double> > sumsNeutVec(numNeutMax, std::vector<double>(arrayDim, 0));
  std::vector<std::vector<double> > sums2DNeutVec(numNeutMax, std::vector<double>(arrayDim2D, 0));

  LBEvtMap::const_iterator LBStart = m_LumiBlockEvtMap.lower_bound(LBLow);
  LBEvtMap::const_iterator LBEnd = m_LumiBlockEvtMap.upper_bound(LBHigh);

  // Select which trigger we're going to look at based on the side
  //
  const bool& triggerOpp = (side == 1 ? L1_ZDC_C : L1_ZDC_A);
  const bool& triggerSame = (side == 1 ? L1_ZDC_A : L1_ZDC_C);

  std::vector<int> numNeutEvent(numNeutMax, 0);
  int numHEEvent = 0;

  //
  //
  std::string calibName = (calibInput != "" ? calibInput : "default");
  CalibData calib = GetCalibration(side, calibName);

  for ( LBEvtMap::const_iterator iter = LBStart; iter != LBEnd; ++iter) {
    unsigned int entryStart = iter->second.first;
    unsigned int entryEnd = iter->second.second;

    for (unsigned int entry = entryStart; entry < entryEnd; ++entry) {
      Long64_t nb = m_tree->GetEntry(entry);
      if (nb < 0) {
	std::cout << "Error reading entry " << entry << ", quitting" << std::endl;
	throw std::runtime_error("Error reading entry for ZDCNLCalibration");
      }
      
      int testMask = zdc_ZdcModuleMask >> side*4;
      if ((testMask & 0xf) != 0xf) continue;
      
      double sumAmp = CalculateEnergy(zdc_ZdcModuleAmp[side], calib);
      
      if (triggerSame && sumAmp > heSumThresh && !excludeHE) {
	AddToSums(sumsHE, sums2DHE, zdc_ZdcModuleAmp[side]);
	numHEEvent++;
      }
      
      // For 1N we require the opposite side trigger
      //
      if (triggerOpp && sumAmp > nNeutERange[0].first && sumAmp < nNeutERange[0].second) {
	AddToSums(sumsNeutVec[0], sums2DNeutVec[0], zdc_ZdcModuleAmp[side]);
	numNeutEvent[0]++;
      }

      for (size_t iNNeut = 1; iNNeut < numNeutMax; iNNeut++) {
	if (sumAmp > nNeutERange[iNNeut].first && sumAmp < nNeutERange[iNNeut].second) {
	  AddToSums(sumsNeutVec[iNNeut], sums2DNeutVec[iNNeut], zdc_ZdcModuleAmp[side]);
	  numNeutEvent[iNNeut]++;
	}
      }
    }
  }

  if (m_debugLevel >= 0) {
    std::cout << "Statistics for side " << side << ", lb range = " << LBLow << ", " << LBHigh
	      << " : # high energy events = " << numHEEvent;

    for (size_t iNNeut = 0; iNNeut < numNeutMax; iNNeut++) {
      std::cout << ", # of " + std::to_string(iNNeut) + " N events = " << numNeutEvent[iNNeut];
    }

    std::cout << std::endl;
  }


  // Normalize the averages
  //
  for (size_t iNNeut = 0; iNNeut < numNeutMax; iNNeut++) {
    if (numNeutEvent[iNNeut] == 0) {
      std::cout << "Found no " + std::to_string(iNNeut) + "N events, quitting" << std::endl;
      return;
    }

    std::for_each(sumsNeutVec[iNNeut].begin(), sumsNeutVec[iNNeut].end(), [&](double& sum) {sum /= numNeutEvent[iNNeut];});
    std::for_each(sums2DNeutVec[iNNeut].begin(), sums2DNeutVec[iNNeut].end(), [&](double& sum) {sum /= numNeutEvent[iNNeut];});
  }

  if (!excludeHE) {
    if (numHEEvent == 0) {
      std::cout << "Found no high-energy events, quitting" << std::endl;
      return;
    }

    std::for_each(sumsHE.begin(), sumsHE.end(), [&](double& sum) {sum /= numHEEvent;});
    std::for_each(sums2DHE.begin(), sums2DHE.end(), [&](double& sum) {sum /= numHEEvent;});
  }


  // Now we have the averages, set up to do the minimization
  //
  size_t numFitParams = numWeights;
  TMatrixD minimMatrix(numFitParams, numFitParams);
  TVectorD minimVector(numFitParams);

  FillMinimizationData(minimMatrix, minimVector, maxPowerModule, HEDeweight, 
		       sumsNeutVec, sumsHE, sums2DNeutVec, sums2DHE);

  if (m_debugLevel >= 2) {
    std::cout << "Dumping matrix equation: " << std::endl;

    for (size_t index1 = 0; index1 < numFitParams; index1++) {
      std::cout << "| ";

      for (size_t index2 = 0; index2 < numFitParams; index2++) {
	std::cout << std::scientific << std::setw(9) << std::setprecision(2) << minimMatrix(index1, index2) << " ";
      }

      std::cout << "|  | w_" << index1 << " = | " << minimVector(index1) << std::endl;
    }
    std::cout << std::endl;
  }


  TDecompLU lu(minimMatrix);
  lu.Decompose();

  bool status = lu.Solve(minimVector);

  if (!status) {
    std::cout << "Minimization failed for side " << side << ", lb range " 
	      << LBLow << ", " << LBHigh << endl;
  }
  else {
    CalibData calibData;
    calibData.LBStart = LBLow;
    calibData.LBEnd = LBHigh;

     int weightIndex = 0;
     for (size_t module : {0, 1, 2, 3}) {
       calibData.weights[module].assign(maxPowerModule[module], 0);

       for (size_t power = 0; power < maxPowerModule[module]; power++) {
	 calibData.weights.at(module).at(power) = minimVector(weightIndex++);

	 if (m_debugLevel >= 0) {
	   std::cout << "Weight for module,power " << module << ", " << power << " = " <<  calibData.weights.at(module).at(power)
		     << std::endl;
	 }
       }
     }

     // Now store this calibration
     //
     AddCalibration(side, calibOutput, calibData);
  }
}

void ZDCNLCalibration::TestCalibration(int side, std::string name)
{
  std::string calibName = (name != "" ? name : "default");
  CalibData calib = GetCalibration(side, calibName);

  if (m_debugLevel >= 0) {
    std::cout << "Testing calibration for side " << side << "name: " << calibName << std::endl;
  }

  m_testCalibSNHist = new TH1D("testCalibSNHist" , "", 4000, 0, 200000);
  for (int module : {0, 1, 2, 3}) {
    std::string fracHistName = "testCalibHEFracHist_" + std::to_string(module);
    std::string energyHistName = "testCalibEnergyHist_" + std::to_string(module);

    m_testCalibHEFracHist[module] = new TH1D(fracHistName.c_str(), "", 200, 0., 1.);
    m_testCalibEnergyHist[module] = new TH1D(energyHistName.c_str(), "", 1000, 0., 100000);
  } 

  static std::array<float, 4> treeRawAmp;
  static std::array<float, 4> treeCalibAmp;
  static float treeRawSum, treeCalibSum, treeRawSumOpp;
  static unsigned int treeEventNumber;
  static bool passedSN, passedTN, passedHE;

  if (m_testTree != 0) delete m_testTree;

  m_testTree = new TTree("ZDCNLCalibTest", "ZDCNLCalibTest");
  m_testTree->SetDirectory(0);

  m_testTree->Branch("EventNumber", &treeEventNumber, "eventNumber/I");
  m_testTree->Branch("lumiBlock", &lumiBlock, "lumiBlock/I");
  m_testTree->Branch("L1_ZDC_A", &L1_ZDC_A, "L1_ZDC_A/b");
  m_testTree->Branch("L1_ZDC_C", &L1_ZDC_C, "L1_ZDC_C/b");
  m_testTree->Branch("L1_ZDC_A_C", &L1_ZDC_A_C, "L1_ZDC_A_C/b");

  m_testTree->Branch("RawSum", &treeRawSum, "RawSum/f");
  m_testTree->Branch("CalibSum", &treeCalibSum, "CalibSum/f");
  m_testTree->Branch("RawAmp", &treeRawAmp[0], "RawAmp[4]/f");
  m_testTree->Branch("CalibAmp", &treeCalibAmp[0], "CalibAmp[4]/f");
  m_testTree->Branch("RawSumOpp", &treeRawSumOpp, "RawSumOpp/f");

  LBEvtMap::const_iterator LBStart = m_LumiBlockEvtMap.lower_bound(calib.LBStart);
  LBEvtMap::const_iterator LBEnd = m_LumiBlockEvtMap.upper_bound(calib.LBEnd);

  int count = 0;
  for ( LBEvtMap::const_iterator iter = LBStart; iter != LBEnd; ++iter) {
    unsigned int entryStart = iter->second.first;
    unsigned int entryEnd = iter->second.second;

    for (unsigned int entry = entryStart; entry < entryEnd; ++entry) {
      Long64_t nb = m_tree->GetEntry(entry);
      if (nb < 0) {
	std::cout << "Error reading entry " << entry << ", quitting" << std::endl;
	throw std::runtime_error("Error reading entry for ZDCNLCalibration");
      }
      
      int testMask = zdc_ZdcModuleMask >> side*4;
      if ((testMask & 0xf) != 0xf) continue;
      
      double sumAmp = zdc_ZdcAmp[side];
      
      // Calculate the calibrated energy 
      //
      float sumCalibAmp = 0;
      float weightIndex = 0;
      
      std::array<double, 4> calibAmpModule = {0, 0, 0, 0};
      
      if (m_debugLevel > 2 && count < 20) {
	std::cout << "TestCalibration:: dumping calibration results for event " << count << std::endl;
      }
      
      for (size_t module : {0, 1, 2, 3}) {
	float amp = zdc_ZdcModuleAmp[side][module];
	float ampPow = amp;
	
	for (size_t power = 0; power < calib.weights[module].size(); power++) {
	  calibAmpModule[module] += calib.weights[module][power]*ampPow;
	  ampPow *= amp;
	}
	
	double energy = calibAmpModule[module];
	sumCalibAmp += energy;
	m_testCalibEnergyHist[module]->Fill(energy); 
	
	treeRawAmp[module] = amp;
	treeCalibAmp[module] = energy;
	
	if (m_debugLevel > 2 && count < 20) {
	  std::cout << "Module " << module << " : raw, calibrated energy = " << std::fixed << amp << ", " << energy << std::endl;
	}
      }
      
      if (m_debugLevel > 2 && count < 20) {
	std::cout << "Total: raw, calibrated energy = " << std::fixed << sumAmp << ", " << sumCalibAmp  << std::endl;
      }
      
      treeRawSum = sumAmp;
      treeRawSumOpp = (side == 0 ? zdc_ZdcAmp[1] : zdc_ZdcAmp[0]);
      treeCalibSum = sumCalibAmp;
      
      m_testTree->Fill();
      
      m_testCalibSNHist->Fill(sumCalibAmp);

      count++;
    }
  }
  
  m_haveTest = true;
}

void ZDCNLCalibration::FillMinimizationData(TMatrixD& minimMatrix, TVectorD& minimVector, 
					    std::array<int, 4> maxPowerModule, float HEDeweight, 
					    const std::vector<std::vector<double> >& sums1DVec, const std::vector<double>& sumsHE,
					    const std::vector<std::vector<double> >& sums2DVec, const std::vector<double>& sumsHE2D)
{
  size_t numWeights = minimVector.GetNrows();
  size_t numColumns = minimMatrix.GetNcols();
  size_t numRows = minimMatrix.GetNrows();
  size_t numNeutMax = sums1DVec.size();

  if (numRows != numWeights || numColumns != numWeights){
    throw std::runtime_error("ZDCNLCalibration::FillMinimizationData; incompatible row/col numbers");
  }

  //  if (m_debugLevel > 0) std::cout << "Filling single neutorn entries in matrix" << std::endl;

  double sumFracSqr = 0;
  for (size_t module : {0, 1, 2, 3}) {
    sumFracSqr += m_HEFraction.at(module)*m_HEFraction.at(module);
  }

  // Start with the single neutron contributions
  //
  {
    size_t index1 = 0;

    for (size_t module1 : {0, 1, 2, 3}) {
      for (size_t power1 = 0; power1 < maxPowerModule[module1]; power1++) {

	size_t index2 = 0;
	size_t sumIndexOffset = (module1*m_maxNLPower + power1)*4*m_maxNLPower;

	for (size_t module2 : {0, 1, 2, 3}) {
	  for (size_t power2 = 0; power2 < maxPowerModule[module2]; power2++) {

	    // The N neutron contributions
	    //
	    unsigned int sum2DIndex = sumIndexOffset + power2;
	    double element = 0;
	    std::for_each(sums2DVec.begin(), sums2DVec.end(), [&] (const std::vector<double>& sums) {element += 2*sums.at(sum2DIndex);});

	    // The TN contribution
	    //
	    //	    element += 2*sumsTN2D.at(sumIndexOffset + power2);
	    
	    // The HE fraction contribution
	    //
	    double sum = sumsHE2D.at(sumIndexOffset + power2);
	    sum *= HEDeweight;

	    if (module2 == module1) element += 2*sum;
	    element -= 2*m_HEFraction.at(module2)*sum;
	    element -= 2*m_HEFraction.at(module1)*sum;
	    element += 2*sumFracSqr*sum;
	    
	    minimMatrix(index1, index2) += element;

	    if (m_debugLevel > 2) {
	      std::cout << "Filling module 1, power 1 = " << module1 << ", " << power1 << ", module 2, power 2 = "
			<< module2 << ", " << power2 << " with sum at index " << sumIndexOffset + power2 
			<< ", value = " << minimMatrix(index1, index2) << std::endl;
	    }

	    index2++;
	  }

	  sumIndexOffset += m_maxNLPower;
	}
      
	size_t sum1DIndex = module1*m_maxNLPower + power1;

	minimVector[index1] = 0;
	for (unsigned int iNNeut = 0; iNNeut < numNeutMax; iNNeut++) {
	  minimVector[index1] += 2*((double) iNNeut + 1)*m_SNEnergy*sums1DVec[iNNeut].at(sum1DIndex);
	}

	if (m_debugLevel > 2) {
	  std::cout << "Filling vector module , power  = " << module1 << ", " << power1 << " with sum at index " << sum1DIndex
		    << ", value = " << minimVector[index1] << std::endl;
	}

	//	minimVector[index1] = 2*m_SNEnergy*sumsSN.at(sum1DIndex) + 2*2*m_SNEnergy*sumsTN.at(sum1DIndex);
	index1++;
      }
    }
  }

  // if (m_debugLevel > 0) std::cout << "Filling high energy entries in matrix" << std::endl;

  // {
    
  //   size_t sumIndexOffset = 0; 
  //   size_t index1 = 0;
    
  //   for (size_t module1 : {0, 1, 2, 3}) {
  //     for (size_t power1 = 0; power1 < maxPowerModule[module1]; power1++) {
  // 	size_t index2 = 0;

  // 	for (size_t module2 : {0, 1, 2, 3}) {
  // 	  for (size_t power2 = 0; power2 < maxPowerModule[module2]; power2++) {
	    
  // 	    double element = 0;
  // 	    double sum = sumsHE2D.at(sumIndexOffset + power2);
	    
  // 	    // Now assemble to terms contributing to this matrix element
  // 	    //
  // 	    if (module2 == module1) element += 2*sum;
  // 	    element -= 2*m_HEFraction.at(module2)*sum;
  // 	    element -= 2*m_HEFraction.at(module1)*sum;
  // 	    element += 2*sumFracSqr*sum;
	    
  // 	    minimMatrix(index1, index2) += m_HEDeweight*element;
  // 	    index2++;
  // 	  }

  // 	  sumIndexOffset += m_maxNLPower; 
  // 	}

  // 	index1++;
  //     }
  //   }
  // }

}

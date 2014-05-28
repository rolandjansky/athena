/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * TestBinsDiffByStrips()
{ 
  //Macro for testing and tuneing the BinsDiffByStrips algorithm. 

  //Algorithm red/green threhsolds for bins:
  const double binYellowThreshold = 3;
  const double binRedThreshold = 10;


  const double redCellFractionLow = 0.001; 
  const double redCellFractionHigh = 0.3;
  const double yellowCellFractionLow = 0.001;
  const double yellowCellFractionHigh = 0.3;
  
  const double fpk = -0.5;


  const double maxRedCellLimit = 100000000.; //Maximum possible value of power law distributed red cells;
  const int nbinx = 2000;
  const int nbiny = 500;

  //Settings for testing by varying iteration parameters (for optimization):
  const int nTestingIntervalsA = 4;
  const double testRangeLowA = 0.265;
  const double testRangeHighA = 0.295;
  const int nTestingIntervalsB = 16;
  const double testRangeLowB = 1.8;
  const double testRangeHighB = 2.2;

  const int nTestsPerInterval = 40;

  bool useOutlierFindingWithErrors = false;

  //Iteration parameters:
  double iterDeviationThreshold = 2.075;
  double iVarExponent = 0.275;
  double sbcf = 0.4;
  double cStop = 25;

  double paramA = testRangeLowA;
 
  TH2F * scorePlot = new TH2F("scorePlot","Test Score versus sbcf",nTestingIntervalsA,testRangeLowA,testRangeHighA,nTestingIntervalsB,testRangeLowB,testRangeHighB);
  
  for (int ka=1; ka<=nTestingIntervalsA; ka++){
    double paramB = testRangeLowB;

    //Decide which parameters to vary here:
    iVarExponent = paramA;

    for (int kb=1; kb<=nTestingIntervalsB; kb++){

      // and here:
      iterDeviationThreshold = paramB;

      double scoreSum = 0.;
      double scoreSum2 = 0.;

      for (int l=0; l<nTestsPerInterval; l++) {

	dqm_algorithms::BinsDiffByStrips * algorithm = new dqm_algorithms::BinsDiffByStrips();  
  
	//Build the test histogram:
	inputHistogram = new TH2F("inputHistogram","TProfile2D for testing BinsDiffByStrips",nbinx,0.,nbinx*1.,nbiny,0.,nbiny*1.);
	trueBinStatus =  new TH2F("trueBinStatus","Bad Bins Added to Input Histogram",
				  inputHistogram->GetNbinsX(),inputHistogram->GetXaxis()->GetXmin(),inputHistogram->GetXaxis()->GetXmax(),
				  inputHistogram->GetNbinsY(),inputHistogram->GetYaxis()->GetXmin(),inputHistogram->GetYaxis()->GetXmax());

	TRandom3 rand(0);
	int nTrueReds = 0;
	int nTrueYellows = 0;
	int nTrueGreens = 0;

      

	for(int i=1; i<=nbinx; i++) {
	  //Test the algorithms for a variety of bin numbers and fraction of red, yellow bins, 
	  double inputRedCellFraction = pow( ((pow(redCellFractionHigh,fpk) - pow(redCellFractionLow,fpk))*rand.Rndm() + pow(redCellFractionLow,fpk) ), 1/fpk);
	  double inputYellowCellFraction = pow( ((pow(yellowCellFractionHigh,fpk) - pow(yellowCellFractionLow,fpk))*rand.Rndm() + pow(yellowCellFractionLow,fpk) ), 1/fpk);
	  double filledBinFraction = pow(rand.Rndm(),2); //We do not fill all the bins in each slice, 

	  for(int j=1; j<=(nbiny*filledBinFraction); j++) {
	    int bin = inputHistogram->GetBin(i,j);

	    double binContent = 0;
	    double binError = 0;

	    double binVariance = 10;
	    double binMean = 50;

	    double minRedBin = binRedThreshold * binVariance;
	    double minYellowBin = binYellowThreshold * binVariance;

	    //Change the mean and statistics for red, yellow bins.
	    if ( rand.Rndm() < inputRedCellFraction ) {
	      //Red bins are added with a mean following an inverse squared power law distribution, with lower limit at binRedThreshold * variance:
	      binContent = binMean + 1 / ( (1/maxRedCellLimit - 1/minRedBin)*rand.Rndm() + 1/minRedBin );
	      binError = binContent * rand.Rndm() / 10; //Random fractional error of up to 10%
	      trueBinStatus->SetBinContent(bin,1.);
	      nTrueReds++;
	    }
	    else if ( rand.Rndm() < (inputYellowCellFraction /(1 - inputRedCellFraction)) ) {
	      //Yellow bins are added with a mean evenly distributed between binYellowThreshold and binRedThreshold;
	      binContent = binMean + minYellowBin + (minRedBin - minYellowBin) * rand.Rndm();
	      binError = binContent * rand.Rndm() / 10; //Random fractional error of up to 10%
	      trueBinStatus->SetBinContent(bin,2.);
	      nTrueYellows++;
	    }
	    else {
	      trueBinStatus->SetBinContent(bin,3.);
	      //Green bins are filled to simulate the result of binStat measurements of values with a true variance of binVariance:
	      int binStat = 100000; //Attempt to test algorithm over wide range of statistics:
	      if(binStat != 0){
		binContent = rand.Gaus(binMean,binVariance);
		if( useOutlierFindingWithErrors ) {
		  binError = binVariance;
		}
		else {
		  binError = binContent / sqrt(binStat);
		}
	      }
	      nTrueGreens++;
	    }
      
	    //Finally, smear the bin content by its error and add it to the histogram:
	    if(binError != 0){
	      //binContent = rand.Gaus(binContent,binError);
	      inputHistogram->SetBinContent(i,j,binContent);
	      inputHistogram->SetBinError(i,j,binError);
	    }
	  }
	}

	dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );
  
	//These are optional parameterms
      
	if(useOutlierFindingWithErrors) {
	  aconfig->addParameter("TestConsistencyWithErrors",1);
	}
      
	aconfig->addParameter("MinStat",10);
	aconfig->addParameter("PublishBins",1);
      
	aconfig->addParameter("nIterations", 100 );
	aconfig->addParameter("IterVariationExponent", iVarExponent);
	aconfig->addParameter("IterDeviationThresh", iterDeviationThreshold);
	aconfig->addParameter("IterScaleBiasCorrectionFactor", sbcf);
	aconfig->addParameter("IterBreakConstant", cStop);

	aconfig->addParameter("useStripsOfConstantY", 1.0 );
      
	// Threshold for number of bins N sigma aways from average
	aconfig->addGreenThreshold("MaxDeviation",binYellowThreshold);
	aconfig->addRedThreshold("MaxDeviation",binRedThreshold);
      
	dqm_core::Result * result = algorithm->execute( "test", *inputHistogram, *aconfig );

	if(result->status_ < 1) continue;
      
	TObjArray * resultList = (TObjArray*)result->getObject();
	TH2F* binwiseStatus = (TH2F*) resultList->First()->Clone();
 
	//outputInput = (TH2F*) resultList->At(1)->Clone();
	//normalizedBins = (TH2F*) resultList->At(2)->Clone();
	//sliceAverageHist = (TH1F*) resultList->At(3)->Clone();
	//sliceVarianceHist = (TH1F*) resultList->At(4)->Clone();
  
	//Loop over true bad bins and binwise status to determine efficiency and quality:
	int trueRedsFound = 0;
	int trueYellowsFound = 0;
	int trueGreensFound = 0;

	int greensFlaggedRed = 0;

	int greensFlaggedYellow = 0;
	int redsFlaggedYellow = 0;

	int redsFlaggedGreen = 0;
	int yellowsFlaggedGreen = 0;
	int yellowsFlaggedRed = 0;
  
	int nFlaggedUndefined = 0;
	int nFlaggedDisabled = 0;
  
	for(int i=1; i<=nbinx; i++) {
	  for(int j=1; j<=nbiny; j++) {
	    float testStatus = binwiseStatus->GetBinContent(i,j);
	    float trueStatus = trueBinStatus->GetBinContent(i,j);
      
	    switch (testStatus) {
	    case 1:
	      if(trueStatus == 1){
		trueRedsFound++;
	      }
	      else if(trueStatus == 3){
		greensFlaggedRed++;
	      }
	      else if(trueStatus == 2){
		yellowsFlaggedRed++;
	      }
	      break;
	    case 2:
	      if(trueStatus == 2){
		trueYellowsFound++;
	      }
	      else {
		if(trueStatus == 1){
		  redsFlaggedYellow++;
		}
		else if(trueStatus == 3){
		  greensFlaggedYellow;
		}
	      }
	      break;
	    case 3:
	      if(trueStatus == 1){
		redsFlaggedGreen++;
	      }
	      else if(trueStatus == 2){
		yellowsFlaggedGreen++;
	      }
	      else trueGreensFound++;
	      break;
	    case 0:
	      nFlaggedUndefined++;
	      break;
	    case -1:
	      nFlaggedDisabled++;
	      break;
	    }
	  }
	}

	double redTaggingEfficiency; 
	double redYellowTagginEfficiency;
	if (nTrueReds != 0){
	  redTaggingEfficiency = (1. * trueRedsFound) / nTrueReds;
	  redYellowTagginEfficiency = (1.0 *trueRedsFound + redsFlaggedYellow) / nTrueReds;
	}
	else {
	  redTaggingEfficiency = -1;
	  redYellowTagginEfficiency = -1;
	}

      


	double yellowTaggingEfficiency; 
	if (nTrueYellows != 0){
	  yellowTaggingEfficiency = (1. * trueYellowsFound) / nTrueYellows;
	}
	else {
	  yellowTaggingEfficiency = -1;
	}
      
	std::cout << "-------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "iVarExponent = " <<  iVarExponent << std::endl;
	std::cout << "iterDeviationThreshold = " << iterDeviationThreshold << std::endl;
	std::cout << "ScaleBiasCorrectionFactor = " << sbcf << std::endl;
	std::cout << "cStop = " << cStop << std::endl;

	std::cout << "Result =  " << result->status_ << std::endl;

	std::cout << std::endl;
	std::cout << "Of the " << nTrueReds << " red bins produced, " << trueRedsFound << " where flagged as red, giving an efficiency of: " << redTaggingEfficiency << std::endl;
	std::cout << trueRedsFound + redsFlaggedYellow << " red bins where flagged as red or yellow, yielding a total efficiency of:    " << redYellowTagginEfficiency << std::endl;
	std::cout << "Of the " << nTrueYellows << " yellow bins produced, " << trueYellowsFound << " where found, giving an efficiency of:      " << yellowTaggingEfficiency << std::endl;

	std::cout << std:: endl << redsFlaggedGreen << " red bins where flagged green, while" <<  std::endl;
	std::cout << greensFlaggedRed << " green bins where flagged red." << std::endl;

	std::cout << std:: endl << greensFlaggedYellow << " green bins and " << redsFlaggedYellow << " red bins where flagged as yellow" << std:: endl;
	std::cout << "while " << yellowsFlaggedGreen << " yellow bins where flagged as green and " << yellowsFlaggedRed << " yellow bins where flagged as red." << std::endl;
	std::cout << trueGreensFound << " bins where correctly identified as green." << std::endl;
	std::cout << nFlaggedUndefined << " bins where given no defined status, while " << nFlaggedDisabled << " bins where excluded from being tested." << std::endl;
  
	//A somewhat arbitrary score to summarize the level of sucess:
	double testScore = 0;
	if( nTrueReds != 0 ) {
	  testScore += ( ( 20. * trueRedsFound + 5. * redsFlaggedYellow - 2.0 * redsFlaggedGreen ) / nTrueReds );
	}
	if( nTrueYellows != 0 ) {
	  testScore += ( ( 2. * trueYellowsFound - 1.0 * yellowsFlaggedRed - 0.1 * yellowsFlaggedGreen ) / nTrueYellows );
	}
	if( nTrueGreens != 0 ) {
	  testScore += ( ( - 10000000. * greensFlaggedRed - 1000000. * greensFlaggedYellow + trueGreensFound ) / nTrueGreens );
	}
      
	std::cout << std::endl << "This corresponds to a score of " << testScore << " for the algorithm." << std::endl;
	std::cout << std::endl;
      
	scoreSum += testScore;
	scoreSum2 += pow(testScore,2);

	delete inputHistogram;
	delete trueBinStatus;
	delete binwiseStatus;
	//       delete outputInput;
	//       delete normalizedBins;
	//       delete sliceAverageHist;
	//       delete sliceVarianceHist;

	delete aconfig;
	delete algorithm;
	delete result;
      }

      double AverageScore = scoreSum/nTestsPerInterval;
      double AvgScoreError = sqrt( scoreSum2 /nTestsPerInterval - AverageScore * AverageScore );
      int thisBin = scorePlot->GetBin(ka,kb);
      scorePlot->SetBinContent(thisBin,AverageScore);
      scorePlot->SetBinError(thisBin,AvgScoreError);
      paramB += (testRangeHighB - testRangeLowB)/nTestingIntervalsB;
    }
    paramA += (testRangeHighA - testRangeLowA)/nTestingIntervalsA;
  }

  scorePlot->Draw();

 
  return scorePlot;
}

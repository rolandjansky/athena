/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class HistogramOperations

  A collection of static methods that act on Histograms
  Implement histogram comparisons here

  @author Ben Wynne bwynne@cern.ch
  @date 19-04-10
 */

#include "HistogramOperations.h"
#include "VectorOperations.h"
#include "TCanvas.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

//Merge histograms
TH1 * HistogramOperations::MergeHistograms( TH1 * firstInput, TH1 * secondInput )
{
	//firstInput->Sumw2();
	firstInput->Add( secondInput, 1.0 );
	return firstInput;
}

//Compare histograms naively
double HistogramOperations::BinwiseCompare( TH1 * NewInput, TH1 * OldInput, string Argument, bool UsePercent )
{
	int newX = NewInput->GetNbinsX();
	int newY = NewInput->GetNbinsY();
	int newZ = NewInput->GetNbinsZ();
	int oldX = OldInput->GetNbinsX();
	int oldY = OldInput->GetNbinsY();
	int oldZ = OldInput->GetNbinsZ();

	if ( newX == oldX && newY == oldY && newZ == oldZ )
	{
		//Parse the argument as the allowed percentage change in each bin value
		double allowedDiff = strtod( Argument.c_str(), nullptr );

		//Compare each bin
		double totalMismatch = 0.0;
		for ( int xIndex = 0; xIndex < newX; xIndex++ )
		{
			for ( int yIndex = 0; yIndex < newY; yIndex++ )
			{
				for ( int zIndex = 0; zIndex < newZ; zIndex++ )
				{
					//Retrieve bin content
					int binNumber = NewInput->GetBin( xIndex+1, yIndex+1, zIndex+1 );
					double newBinContent = NewInput->GetBinContent(binNumber);
					double oldBinContent = OldInput->GetBinContent(binNumber);

					//Check for rounding stupidness
					if ( ( newBinContent < 1.0E-8 && newBinContent != 0.0 ) || ( oldBinContent < 1.0E-8 && oldBinContent != 0.0 ) )
					{
						cerr << "WARNING: Stupid low values detected: " << newBinContent << " " << oldBinContent << endl;
					}

					//Compare
					double mismatch = 0.0;
					if(UsePercent) mismatch = std::abs( newBinContent - oldBinContent ) * 100.0 / newBinContent;
					else mismatch = std::abs( newBinContent - oldBinContent );
					
					//Add to the total mismatch score unless it's within the fuzz factor
					if ( mismatch > allowedDiff )
					{
					  totalMismatch += mismatch;
					}
				}
			}
		}

		//Warn if the bins do not match
		if ( totalMismatch > 0.0 )
		{
		  cerr << "WARNING: Mismatch in " << NewInput->GetName() << " of magnitude " << totalMismatch << endl;
		}
		return totalMismatch;
	}
	else
	{
		//Warn if the histograms do not have the same dimensions
		cerr << "WARNING: Not identical histogram dimensions for " << NewInput->GetName() << ": ";
		cerr << newX << " and " << oldX << ", ";
		cerr << newY << " and " << oldY << ", ";
		cerr << newZ << " and " << oldZ << endl;

		return 1.0;
	}
}

//Compare histograms naively
double HistogramOperations::BinwiseHackCompare( TH1 * NewInput, TH1 * OldInput, string Argument )
{
	//The argument should be a colon-separated list of the different allowed fluctuations for the x-bins
	vector<string> splitArgument = VectorOperations::SplitString( Argument, ':' );
	vector<double> binChange;
	for ( int argumentIndex = 0; argumentIndex < splitArgument.size(); argumentIndex++ )
	{
		binChange.push_back( strtod( splitArgument[argumentIndex].c_str(), nullptr ) );
	}

	int newX = NewInput->GetNbinsX();
	int newY = NewInput->GetNbinsY();
	int newZ = NewInput->GetNbinsZ();
	int oldX = OldInput->GetNbinsX();
	int oldY = OldInput->GetNbinsY();
	int oldZ = OldInput->GetNbinsZ();

	if ( newX != binChange.size() )
	{
		cerr << "WARNING: Argument " << Argument << " does not match number of x bins (" << newX << ")" << endl;
		return 1.0;
	}
	else if ( newX == oldX && newY == oldY && newZ == oldZ )
	{
		//Compare each bin
		double totalMismatch = 0.0;
		for ( int xIndex = 0; xIndex < newX; xIndex++ )
		{
			for ( int yIndex = 0; yIndex < newY; yIndex++ )
			{
				for ( int zIndex = 0; zIndex < newZ; zIndex++ )
				{
					//Retrieve bin content
					int binNumber = NewInput->GetBin( xIndex+1, yIndex+1, zIndex+1 );
					double newBinContent = NewInput->GetBinContent(binNumber);
					double oldBinContent = OldInput->GetBinContent(binNumber);

					//Check for rounding stupidness
					if ( ( newBinContent < 1.0E-8 && newBinContent != 0.0 ) || ( oldBinContent < 1.0E-8 && oldBinContent != 0.0 ) )
					{
						cerr << "WARNING: Stupid low values detected: " << newBinContent << " " << oldBinContent << endl;
					}

					//Compare
					double mismatch = std::abs( newBinContent - oldBinContent ) * 100.0 / newBinContent;

					//Add to the total mismatch score unless it's within the fuzz factor
					if ( mismatch > binChange[xIndex] )
					{
						totalMismatch += mismatch;
					}
				}
			}
		}

		//Warn if the bins do not match
		if ( totalMismatch > 0.0 )
		{
			cerr << "WARNING: Mismatch in " << NewInput->GetName() << " of magnitude " << totalMismatch << endl;
		}
		return totalMismatch;
	}
	else
	{
		//Warn if the histograms do not have the same dimensions
		cerr << "WARNING: Not identical histogram dimensions for " << NewInput->GetName() << ": ";
		cerr << newX << " and " << oldX << ", ";
		cerr << newY << " and " << oldY << ", ";
		cerr << newZ << " and " << oldZ << endl;

		return 1.0;
	}
}

//Perform a Kolmogorov test and warn if probability of match is less than threshold
double HistogramOperations::KolmogorovTest( TH1 * NewInput, TH1 * OldInput, string Argument )
{
	//Parse the argument as the minimum allowed probability of match
	double threshold = strtod( Argument.c_str(), nullptr );

	//Perform the Kolmogorov-Smirnov test
	NewInput->Sumw2();
	OldInput->Sumw2();
	double maxDist = NewInput->KolmogorovTest(OldInput,"M");

	//Warn if match probability is below threshold
	if ( maxDist > threshold )
	{
		cerr << "WARNING: Kolmogorov-Smirnov test has a maximum distance of " << maxDist << " for " << NewInput->GetName() << endl;
		return 1.0 - maxDist;
	}
	else
	{
		return 0.0;
	}
}

//Compare the mean values of two histograms with an allowed percentage change
double HistogramOperations::CompareMean( TH1 * NewInput, TH1 * OldInput, string Argument, bool UsePercent )
{
	double allowedDiff = 0.0;
	double result = 0.0;

	//Find out which comparison to do
	int comparison;
	if ( Argument == "reduce" )
	{
		comparison = 1;
	}
	else if ( Argument == "increase" )
	{
		comparison = 2;
	}
	else if ( Argument == "reduceAbs" )
	{
		comparison = 3;
	}
	else if ( Argument == "increaseAbs" )
	{
		comparison = 4;
	}
	else
	{
		comparison = 0;
		allowedDiff = strtod( Argument.c_str(), nullptr );
	}

	//Loop over all axes
	for ( int axisIndex = 1; axisIndex < 4; axisIndex++ )
	{
		double newMean = NewInput->GetMean(axisIndex);
		double oldMean = OldInput->GetMean(axisIndex);
		double seeChange = 0.0;
		if(UsePercent) seeChange = std::abs( newMean - oldMean ) * 100.0 / newMean;
		else seeChange = std::abs( newMean - oldMean );

		if ( comparison == 0 && seeChange > allowedDiff )
		{
		  string suffix = "";
		  if(UsePercent) suffix = "%";
		  cerr << "WARNING: Mean of " << NewInput->GetName() << " differs by " << seeChange << suffix << " on the " << axisIndex << " axis" << endl;
		  result += seeChange;
		}
		else if ( comparison == 1 && newMean > oldMean )
		{
		  cerr << "WARNING: Mean of " << NewInput->GetName() << " has increased on the " << axisIndex << " axis" << endl;
		  result += newMean - oldMean;
		}
		else if ( comparison == 2 && newMean < oldMean )
		{
		  cerr << "WARNING: Mean of " << NewInput->GetName() << " has reduced on the " << axisIndex << " axis" << endl;
		  result += oldMean - newMean;
		}
		else if ( comparison == 3 && std::abs(newMean) > std::abs(oldMean) )
		{
		  cerr << "WARNING: Absolute value of the mean of " << NewInput->GetName() << " has increased on the " << axisIndex << " axis" << endl;
		  result += oldMean - newMean;
		}
		else if ( comparison == 4 && std::abs(newMean) < std::abs(oldMean) )
		{
		  cerr << "WARNING: Absolute value of the mean of " << NewInput->GetName() << " has decreased on the " << axisIndex << " axis" << endl;
		  result += oldMean - newMean;
		}
	}
	
	return result;
}

//Compare the sigma values of two histograms with an allowed percentage change
double HistogramOperations::CompareSigma( TH1 * NewInput, TH1 * OldInput, string Argument, bool UsePercent )
{
	double allowedDiff = 0.0;
	double result = 0.0;

	//Find out which comparison to do
	int comparison;
	if ( Argument == "reduce" )
	{
		comparison = 1;
	}
	else if ( Argument == "increase" )
	{
		comparison = 2;
	}
	else
	{
		comparison = 0;
		allowedDiff = strtod( Argument.c_str(), nullptr );
	}

	//Loop over all axes
	for ( int axisIndex = 1; axisIndex < 4; axisIndex++ )
	{
		double newSigma = NewInput->GetRMS(axisIndex);
		double oldSigma = OldInput->GetRMS(axisIndex);
		double seeChange = 0.0;
		if(UsePercent) seeChange = std::abs( newSigma - oldSigma ) * 100.0 / newSigma;
		else seeChange = std::abs( newSigma - oldSigma );

		if ( comparison == 0 && seeChange > allowedDiff )
		{
		  string suffix = "";
		  if(UsePercent) suffix = "%";
		  cerr << "WARNING: Sigma of " << NewInput->GetName() << " differs by " << seeChange << suffix << " on the " << axisIndex << " axis" << endl;
		  result += seeChange;
		}
		else if ( comparison == 1 && newSigma > oldSigma )
		{
		  cerr << "WARNING: Sigma of " << NewInput->GetName() << " has increased on the " << axisIndex << " axis" << endl;
		  result += newSigma - oldSigma;
		}
		else if ( comparison == 2 && newSigma < oldSigma )
		{
		  cerr << "WARNING: Sigma of " << NewInput->GetName() << " has reduced on the " << axisIndex << " axis" << endl;
		  result += oldSigma - newSigma;
		}
	}

	return result;
}

//Compare the numbers of entries of the histograms
double HistogramOperations::CompareTotalEntries( TH1 * NewInput, TH1 * OldInput, string Argument )
{
	//Parse the argument as the allowed percentage change
	double percentChange = strtod( Argument.c_str(), nullptr );

	//Calculate the percentage change in number of entries
	double change = std::abs( NewInput->GetEntries() - OldInput->GetEntries() ) * 100.0 / NewInput->GetEntries();

	//Warn if outside allowed range
	if ( change > percentChange )
	{
		cerr << "WARNING: Number of entries in " << NewInput->GetName() << " has changed by " << change << "%" << endl;
		return change;
	}
	else
	{
		return 0.0;
	}
}

//Superimpose two histograms and save this plot - assumes a root file is already open
void HistogramOperations::FileOutput( TH1 * NewInput, TH1 * OldInput, string Information )
{

        TH1 * tempNew = (TH1*) NewInput->Clone();
	TH1 * tempOld = (TH1*) OldInput->Clone();
      
        //Make a canvas of hists with overlay
	string canvasTitle = (string)NewInput->GetName() + " " + Information;
	TCanvas * superimposedPlots = new TCanvas( NewInput->GetName(), canvasTitle.c_str() );

	if ( NewInput->GetDimension() == 1 )
	{
		//Check the x axis range
		double xMax = NewInput->GetXaxis()->GetXmax();
		double xMin = NewInput->GetXaxis()->GetXmin();
		if ( xMax < OldInput->GetXaxis()->GetXmax() )
		{
			xMax = OldInput->GetXaxis()->GetXmax();
		}
		if ( xMin > OldInput->GetXaxis()->GetXmin() )
		{
			xMin = OldInput->GetXaxis()->GetXmin();
		}
		NewInput->GetXaxis()->SetRangeUser( xMin, xMax );

		//Check the data will all be visible
		double yMax = NewInput->GetMaximum();
		double yMin = NewInput->GetMinimum();
		if ( yMax < OldInput->GetMaximum() )
		{
			yMax = OldInput->GetMaximum();
		}       
		if ( yMin > OldInput->GetMinimum() )
		{
			yMin = OldInput->GetMinimum();
		}

		double addRange = 0.1 * ( yMax - yMin );	
		NewInput->GetYaxis()->SetRangeUser( yMin - addRange, yMax + addRange );
	}

	//Plot both histograms on the canvas
	NewInput->Draw();
	OldInput->SetLineColor(kRed);
	OldInput->Draw("same");

	//Save the canvas
	superimposedPlots->Update();
	superimposedPlots->Write();

	//Make Canvas with difference between hists
	if( NewInput->GetNbinsX()==OldInput->GetNbinsX() &&
	    NewInput->GetNbinsY()==OldInput->GetNbinsY() &&
	    NewInput->GetNbinsZ()==OldInput->GetNbinsZ() )
	{
	  //Make a canvas
	  string canvasTitleDiff = (string)NewInput->GetName() + " " + Information + " diff";
	  string canvasNameDiff = (string)NewInput->GetName() + "_diff";
	  TCanvas * superimposedPlotsDiff = new TCanvas( canvasNameDiff.c_str(), canvasTitleDiff.c_str() );
	  
	  tempNew->Add(tempOld,-1.0);

	  //Plot both histogram on the canvas
	  if ( tempNew->GetDimension() == 2 ) tempNew->Draw("COLZ");
	  else tempNew->Draw();

	  //Save the canvas
	  superimposedPlotsDiff->Update();
	  superimposedPlotsDiff->Write();
	}

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PDFcreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "PDFcreator.h"

// Random number generators
#include "CLHEP/Random/RandFlat.h"

// ROOT
#include "TFile.h"
#include "TH2F.h"
#include "TAxis.h"
#include "TRandom.h"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
double ISF::PDFcreator::getRand(std::vector<double> inputParameters, bool discrete, double randMin, double randMax)
{
  // Since the histograms which hold the function's fit parameters are binned
  // we have to choose which bin we use for the current energy & eta regime.
  // It's very unlikely to exactly hit a bin's center, therefore we randomly
  // choose either the lower next or the upper next bin.

  int numInputPar = inputParameters.size();
  TAxis **axis = new TAxis*[numInputPar];
  Int_t *chosenBin = new Int_t[numInputPar];


  // get the axis from the first (could be any other too, but the first one
  // always exists) histogram (which holds the function's first fit parameter)
  axis[0] = m_par[0]->GetXaxis();
  axis[1] = m_par[0]->GetYaxis();

  // loop over all input inputParameters (e.g. eta & energy)
  for (int inputPar=0; inputPar<numInputPar; inputPar++) {
    // store the axis for the current inputParameter
    TAxis *curaxis = axis[inputPar];
    double curvalue = inputParameters[inputPar];

    // get the id of the bin corresponding to the current value
    // (use FindFixBin to avoid the axis from being rebinned)
    Int_t bin = curaxis->FindFixBin(curvalue);
    // get the center of the closest bin to the input inputParameter
    double closestCenter = curaxis->GetBinCenter(bin);
    // get the bins edge closest to the current value
    // and find out if the next closest bin has id bin+1 or bin-1
    double closestEdge = (curvalue <= closestCenter) ? curaxis->GetBinLowEdge(bin) : curaxis->GetBinUpEdge(bin);
    int binDelta = (curvalue <= closestCenter) ? -1 : +1;

    // Calculate the 'normalised distance' between the input-value,
    // the closest bin's center and this bin's closest edge
    // distance == 0 means that the input value lies exactly on its closest bin's center.
    // distance == 0.5 means that the input value lies exactly on the edge to the next bin
    double distance = (curvalue - closestCenter) / (closestEdge - closestCenter) / 2.;

    // now randomly choose if we take the closest bin or the one next to the closest one.
    // with this, we kind of linearly interpolate between two bins if an input value
    // betweeen these two bins does occure many times.
    double rand = CLHEP::RandFlat::shoot(m_randomEngine);
    chosenBin[inputPar] = ( rand >= distance ) ? bin : bin+binDelta;

    // check if we have chosen an over- or underflow-bin
    // if this has happened, choose the last bin still in range

    if ( chosenBin[inputPar] <= 0 )				chosenBin[inputPar] = 1;
    else if ( chosenBin[inputPar] > curaxis->GetNbins() )       chosenBin[inputPar] = curaxis->GetNbins();
  }

  // now get the bin number (since all histograms are binned
  // in exactly the same way it should be the same for all
  // of them)
  Int_t bin = 0;
  if (numInputPar == 1)             bin = m_par[0]->GetBin( chosenBin[0] );
  else if (numInputPar ==2 )        bin = m_par[0]->GetBin( chosenBin[0], chosenBin[1] );
  else if (numInputPar == 3)        bin = m_par[0]->GetBin( chosenBin[0], chosenBin[1], chosenBin[2] );
  // TODO: implement case of >3 input parameters

  // free some memory
  delete [] axis;

  // fill in the parameters into the distribution-function
  // (e.g. for the current energy & eta regime)
  for (int funcPar = 0; funcPar < m_pdf->GetNpar(); funcPar++) {
    double value = m_par[funcPar]->GetBinContent(bin);
    m_pdf->SetParameter(funcPar, value);
  }

  // now finally we can determine the random number from the given distribution
  //
  // set the minimum and maximum random values
  double xMin = ( (randMin != 0.) && (randMin <  m_randmax->GetBinContent(bin)) )
    ? randMin : m_randmin->GetBinContent(bin);
  double xMax = ( (randMax != 0.) && (randMax <= m_randmax->GetBinContent(bin)) )
    ? randMax : m_randmax->GetBinContent(bin);

  // free some more memory
  delete [] chosenBin;

  // (1.) for discrete distributions
  //       - this is implemented separately from the continuous case,
  //         since it significantly speeds up the creation of random
  //         numbers from a distribution with a high negative gradient
  //       - for this the sum( f(x_i) ) has to be normalized !
  if (discrete) {
    // get a flat random value between 0. and 1.
    double flat = CLHEP::RandFlat::shoot(m_randomEngine);

    double sum = 0.;

    // set the minimum x value
    xMin = lround(xMin);

    // now determine the corresponding value from the distribution
    for (int rand=xMin; rand<lround(xMax); rand++) {
      // sum up the probabilities for each current bin
      sum += m_pdf->Eval(rand);
      // if the sum gets larger than the random value
      //   -> the current 'rand' is the value from the distribution
      //   -> return this value
      if (sum >= flat) return ((double)rand);
    }

    // This point is reached if the the x value corresponding to the random
    // value is the maximum possible value in the probability distribution
    // or if the distribution is not normalized.
    //    -> return the maximum value
    return (double)m_pdf->GetXmax();
  }


  // (2.) for continuous distributions
  //      * algorithmus: rejection sampling
  else {
    // find max and min values in the PDF
    double yMin = m_pdf->GetMinimum(xMin, xMax);
    //if (yMin < 0.) yMin = 0.; // in case the PDF contains negative values
    double yMax = m_pdf->GetMaximum(xMin, xMax);

    // now get a random value rx with the right distribution
    //  - use the count variable just for backup to prevent
    //    an endless loop
    for (int count=0; count < 1e7; count++) {
      // first choose a random value within [yMin, yMax]
      double ry = CLHEP::RandFlat::shoot(m_randomEngine)*(yMax-yMin) + yMin;
      // then choose a random value within [xMin, xMax]
      double rx = CLHEP::RandFlat::shoot(m_randomEngine)*(xMax-xMin) + xMin;
      // calculate y=pdf(rx)
      double y = m_pdf->Eval(rx);
      // if ry<=pdf(rx) is fulfilled, we have our random value
      if ( ry <= y ) return rx;
    }
  }

  // this point will only be reached, if something went wrong
  return 0.;
}

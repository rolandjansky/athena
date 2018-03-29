/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MonitoringFile_MergeAlgs.cxx,v 1.4 2009-04-06 13:08:13 ponyisi Exp $
// **********************************************************************

#include "DataQualityUtils/MonitoringFile.h"

#include <cmath>
#include <iostream>

#include <TH1.h>
#include <TH2.h>
#include <TList.h>

namespace {
Bool_t IsBinOverflow(const TH1& hist, Int_t bin)
{
   // Return true if the bin is overflow.
   Int_t binx, biny, binz;
   hist.GetBinXYZ(bin, binx, biny, binz);
   Int_t dim = hist.GetDimension();

   if ( dim == 1 )
      return binx >= hist.GetNbinsX() + 1;
   else if ( dim == 2 )
      return (binx >= hist.GetNbinsX() + 1) || 
             (biny >= hist.GetNbinsY() + 1);
   else if ( dim == 3 )
      return (binx >= hist.GetNbinsX() + 1) || 
             (biny >= hist.GetNbinsY() + 1) ||
             (binz >= hist.GetNbinsZ() + 1);
   else
      return 0;
}

Bool_t IsBinUnderflow(const TH1& hist, Int_t bin)
{
   
   // Return true if the bin is overflow.
   Int_t binx, biny, binz;
   hist.GetBinXYZ(bin, binx, biny, binz);
   Int_t dim = hist.GetDimension();
   
   if ( dim == 1 )
      return (binx <= 0);
   else if ( dim == 2 )
      return (binx <= 0 || biny <= 0);
   else if ( dim == 3 )
      return (binx <= 0 || biny <= 0 || binz <= 0);
   else
      return 0;
}
}

namespace dqutils {

Int_t  MonitoringFile::getNumBins( const TH1& hist ) 
{
  // Return number of histogram bins in ROOT 1-dim projection scheme
  // Allow loops on multi-dim histograms without regard for dimensionality
  Int_t dim = hist.GetDimension();
  if (dim == 1) {
    return (hist.GetNbinsX()+2);
  }
  if (dim == 2) {
    return (hist.GetNbinsX()+2)*(hist.GetNbinsY()+2);
  }
  if (dim == 3) {
    return (hist.GetNbinsX()+2)*(hist.GetNbinsY()+2)*(hist.GetNbinsZ()+2);
  }
  return -1;
}

void MonitoringFile::merge_effAsPerCent( TH2& a, const TH2& b )
{
  // Auther: Benjamin Trocme
  // a and b are efficiency histogramming with percentage stored
  // den/num are a number of events
  // BinContent = n/d*100 
  // BinError   = (1/d2) * sqrt( d*n*(d-n) )

  // First extract the denominator
  // It is supposed to be the same for all bins
  // Have to do this in two steps to avoid problem
  // of empty bins in which the nb of events can not be extracted
  float denA = 0.;
  float denB = 0.;
  for (int ix = 1; ix <= a.GetNbinsX(); ix++){
    for (int iy = 1; iy <= a.GetNbinsY(); iy++){  
      // Extract ratio and associated errors
      // Warning : these are percentages!
      float efficiencyA = a.GetBinContent(ix,iy)/100.;
      float efficiencyB = b.GetBinContent(ix,iy)/100.;
      float efficiencyErrA = a.GetBinError(ix,iy)/100.;
      float efficiencyErrB = b.GetBinError(ix,iy)/100.;

      // Compute denominator ("nb of events")
      if (efficiencyErrA != 0 && efficiencyA != 0 && denA == 0) denA = efficiencyA*(1-efficiencyA)/efficiencyErrA/efficiencyErrA;
      if (efficiencyErrB != 0 && efficiencyB != 0 && denB == 0) denB = efficiencyB*(1-efficiencyB)/efficiencyErrB/efficiencyErrB;
    }
  }

  float denTot = denA + denB;
  const double nEntries = a.GetEntries() + b.GetEntries();

  for (int ix = 1; ix <= a.GetNbinsX(); ix++){
    for (int iy = 1; iy <= a.GetNbinsY(); iy++){  
      // Extract ratio and associated errors
      // Warning : these are percentages!
      float efficiencyA = a.GetBinContent(ix,iy)/100.;
      float efficiencyB = b.GetBinContent(ix,iy)/100.;
      //float efficiencyErrA = a.GetBinError(ix,iy)/100.;
      //float efficiencyErrB = b.GetBinError(ix,iy)/100.;

      // Compute numerator ("nb of good events") for each histo
      float numA = denA * efficiencyA;
      float numB = denB * efficiencyB;

      // Deduce the merged ratio and the associated error
      float numTot = numA + numB;
      float efficiencyTot = 0.;
      float efficiencyErrTot = 0.;

      if (denTot != 0.) efficiencyTot = numTot/denTot*100.;
      if (denTot != 0.) efficiencyErrTot = sqrt(numTot*denTot*(denTot-numTot))/denTot/denTot*100.;      

      a.SetBinContent(ix,iy,efficiencyTot);
      a.SetBinError(ix,iy,efficiencyErrTot);
    }
  }
  a.SetEntries( nEntries );
}

void MonitoringFile::merge_perBinEffPerCent(TH1& a, const TH1& b)
{
  // This code assume that the histogram content is the efficiency of a 
  // given cut or selection in each bin (e.g. the ratio of a distribution
  // after cut to the distribution before cut, bin by bin) and that these
  // are efficiencies in percent.
  //
  // It also assumes that the error we calculated in a specific way:
  // dEff = sqrt( eff*(1.-eff)/N )   [Eff= efficiency N = number of event in bin before cuts]
  // dEff = 1-0.159^(1/N) if Eff = 0 
  // dEff = 1-0.159^(1/N) if Eff = 1
  // dEff = 0 means no entries, the bin is ignored
  //

  static double OneSigOneSided = 0.159; // 0.5*(1-0.681) where 0.681 means 68%CL
  // Verify histogram compatibility
  if (a.GetDimension() != b.GetDimension()) {
    std::cerr<<"merge_perBinEffPerCent \""<< a.GetName() <<"\": attempt to merge histograms of different dimensionality" << std::endl;
    return;
  }

  Int_t ncells = getNumBins(a);

  if (ncells != getNumBins(b)) {
    std::cerr<<"merge_perBinEffPerCent \""<< a.GetName() <<"\": attempt to merge histograms of different sizes\n";
    return;
  }

  // do not attempt to automatically extend!
  a.SetCanExtend(TH1::kNoAxis);

  const double nEntries = a.GetEntries() + b.GetEntries();

  for( int bin = 0; bin < ncells; bin++ ) {
    if (IsBinUnderflow(a, bin) || IsBinOverflow(a, bin)) continue;
    float efficiencyA = a.GetBinContent(bin)/100.;
    float efficiencyB = b.GetBinContent(bin)/100.;
    float efficiencyErrA = a.GetBinError(bin)/100.;
    float efficiencyErrB = b.GetBinError(bin)/100.;

    float efficiencyTot = 0.;
    float efficiencyErrTot = 0.;
    if ( efficiencyErrA == 0. ) {
      efficiencyTot = efficiencyB;
      efficiencyErrTot = efficiencyErrB;
    }
    else {
      if ( efficiencyErrB == 0. ) {
        efficiencyTot = efficiencyA;
        efficiencyErrTot = efficiencyErrA;
      }
      else {
        float denomA =  0.;
        if ( efficiencyA == 0. ){
          denomA = std::log(OneSigOneSided)/std::log(1.-efficiencyErrA);
        }
        else {
          if ( efficiencyA > 0.99) {
            denomA = std::log(OneSigOneSided)/std::log(1.-efficiencyErrA);
          }
	  else {
            denomA = efficiencyA*(1.-efficiencyA)/(efficiencyErrA*efficiencyErrA);
          }
        }

        float denomB =  0.;
        if ( efficiencyB == 0. ){
          denomB = std::log(OneSigOneSided)/std::log(1.-efficiencyErrB);
        }
        else {
          if ( efficiencyB > 0.99) {
            denomB = std::log(OneSigOneSided)/std::log(1.-efficiencyErrB);
          }
	  else {
            denomB = efficiencyB*(1.-efficiencyB)/(efficiencyErrB*efficiencyErrB);
          }
        }

        float denom = denomA + denomB;
        efficiencyTot = (denomA*efficiencyA + denomB*efficiencyB)/denom;
        efficiencyErrTot = std::sqrt(efficiencyTot*(1.-efficiencyTot)/denom);
	if ( efficiencyTot == 0. ) efficiencyErrTot =  1.0-std::pow(OneSigOneSided,1.0/denom);
	if ( efficiencyTot > 0.99 ) efficiencyErrTot = 1.0-std::pow(OneSigOneSided,1.0/denom);
      }
    }
    a.SetBinContent(bin,efficiencyTot*100.);
    a.SetBinError(bin,efficiencyErrTot*100.);
  }
  a.SetEntries( nEntries );
}

void MonitoringFile::merge_effAsPerCentAlt( TH1& a, const TH1& b )
{
  // Author: Peter Onyisi, d'apres Benjamin Trocme
  // Variation of merge_effAsPerCent
  // a and b are efficiency histogramming with percentage stored
  // den/num are a number of events
  // BinContent = n/d*100
  // BinError   = (1/d2) * sqrt( d*n*(d-n) )

  // Verify histogram compatibility
  if (a.GetDimension() != b.GetDimension()) {
    std::cerr<<"merge_effAsPerCentAlt \"" << a.GetName() <<"\": attempt to merge histograms of different dimensionality\n";
    return;
  }

  Int_t ncells = getNumBins(a);

  if (ncells != getNumBins(b)) {
    std::cerr<<"merge_effAsPerCentAlt \""<< a.GetName() <<"\": attempt to merge histograms of different bin counts\n";
    return;
  }

  // do not attempt to automatically extend!
  a.SetCanExtend(TH1::kNoAxis);

  // First extract the denominator
  // It is supposed to be the same for all bins
  // Have to do this in two steps to avoid problem
  // of empty bins in which the nb of events can not be extracted

  float denA = 0.;
  float denB = 0.;
  for( int bin = 0; bin < ncells; bin++ ) {
    if (IsBinUnderflow(a, bin) || IsBinOverflow(a, bin)) continue;
    // Extract ratio and associated errors
    // Warning : these are percentages!
    float efficiencyA = a.GetBinContent(bin)/100.;
    float efficiencyB = b.GetBinContent(bin)/100.;
    float efficiencyErrA = a.GetBinError(bin)/100.;
    float efficiencyErrB = b.GetBinError(bin)/100.;

    // Compute denominator ("nb of events")
    if (efficiencyErrA != 0 && efficiencyA != 0 && denA == 0) denA = efficiencyA*(1-efficiencyA)/efficiencyErrA/efficiencyErrA;
    if (efficiencyErrB != 0 && efficiencyB != 0 && denB == 0) denB = efficiencyB*(1-efficiencyB)/efficiencyErrB/efficiencyErrB;
  }

  float denTot = denA + denB;
  const double nEntries = a.GetEntries() + b.GetEntries();

  for( int bin = 0; bin < ncells; bin++ ) {
    if (IsBinUnderflow(a, bin) || IsBinOverflow(a, bin)) continue;
    // Extract ratio and associated errors
    // Warning : these are percentages!
    float efficiencyA = a.GetBinContent(bin)/100.;
    float efficiencyB = b.GetBinContent(bin)/100.;
    //float efficiencyErrA = a.GetBinError(bin)/100.;
    //float efficiencyErrB = b.GetBinError(bin)/100.;

    // Compute numerator ("nb of good events") for each histo
    float numA = denA * efficiencyA;
    float numB = denB * efficiencyB;

    // Deduce the merged ratio and the associated error
    float numTot = numA + numB;
    float efficiencyTot = 0.;
    float efficiencyErrTot = 0.;

    if (denTot != 0.) efficiencyTot = numTot/denTot*100.;
    if (denTot != 0.) efficiencyErrTot = sqrt(numTot*denTot*(denTot-numTot))/denTot/denTot*100.;

    a.SetBinContent(bin,efficiencyTot);
    a.SetBinError(bin,efficiencyErrTot);
  }
  a.SetEntries( nEntries );
}


void MonitoringFile::merge_weightedAverage( TH1& a, const TH1& b )
{

  // Author: Tobias Golling

  if (a.GetDimension() != b.GetDimension()) {
    std::cerr<<"merge_weightedAverage \"" << a.GetName() <<"\": attempt to merge histograms of different dimensionality\n";
    return;
  }

  Int_t ncells = getNumBins(a);

  if (ncells != getNumBins(b)) {
    std::cerr<<"merge_weightedAverage \"" << a.GetName() <<"\": attempt to merge histograms of different sizes\n";
    return;
  }

  // do not attempt to automatically extend!
  a.SetCanExtend(TH1::kNoAxis);

  double nEntries = a.GetEntries();
  nEntries += b.GetEntries();

  // if ( !a.InheritsFrom("TH2") ) {
  for( int bin = 0; bin < ncells; bin++ ) {
    double y1 = a.GetBinContent(bin);
    double y2 = b.GetBinContent(bin);
    double e1 = a.GetBinError(bin);
    double e2 = b.GetBinError(bin);
    double w1 = 1., w2 = 1.;
    if (e1 > 0) w1 = 1./(e1*e1);
    if (e2 > 0) w2 = 1./(e2*e2);
      
    // case 1:
    if (e1 > 0 && e2 > 0){
      a.SetBinContent(bin, (w1*y1 + w2*y2)/(w1 + w2));
      a.SetBinError(bin, 1./sqrt(w1 + w2));
    }
    // case 2:
    else if (e2 > 0){
      a.SetBinContent(bin, y2);
      a.SetBinError(bin, e2);
    }
    // case 3:
    else if (e1 > 0){
      a.SetBinContent(bin, y1);
      a.SetBinError(bin, e1);
    }
    // case 4:
    else {
      a.SetBinContent(bin, (y1 + y2)/2.);
      a.SetBinError(bin, 0.);
    }
  }
  
  a.SetEntries( nEntries );

  /*
  } else if ( a.InheritsFrom("TH2") ) {

    try {

      merge_weightedAverage2D( dynamic_cast<TH2&>(a), dynamic_cast<const TH2&>(b) );

    } catch ( const std::bad_cast& err ) {
      // protect against dynamic cast failing

      return;

    }

  }
    */

}


void MonitoringFile::merge_weightedAverage2D( TH2& a, const TH2& b )
{
  // Author: Frank Berghaus
  for( int binx = 0; binx <= a.GetNbinsX()+1; binx++ ) {
    for( int biny = 0; biny <= a.GetNbinsY()+1; biny++ ) {

      int bin = a.GetBin(binx,biny);

      double y1 = a.GetBinContent(bin);
      double y2 = b.GetBinContent(bin);
      double e1 = a.GetBinError(bin);
      double e2 = b.GetBinError(bin);
      double w1 = 1., w2 = 1.;
      if (e1 > 0) w1 = 1./(e1*e1);
      if (e2 > 0) w2 = 1./(e2*e2);
      
      // case 1:
      if (e1 > 0 && e2 > 0){
	a.SetBinContent(bin, (w1*y1 + w2*y2)/(w1 + w2));
	a.SetBinError(bin, 1./sqrt(w1 + w2));
      }
      // case 2:
      else if (e2 > 0){
	a.SetBinContent(bin, y2);
	a.SetBinError(bin, e2);
      }
      // case 3:
      else if (e1 > 0){
	a.SetBinContent(bin, y1);
	a.SetBinError(bin, e1);
      }
      // case 4:
      else {
	a.SetBinContent(bin, (y1 + y2)/2.);
	a.SetBinError(bin, 0.);
      }
    }
  }
}


void MonitoringFile::merge_weightedEff( TH1& a, const TH1& b )
{
  // Author: Arely Cortes Gonzalez
  // This function adds two 1D efficiency histograms
  // weighting them by the number of entries.
  // The histograms need to have same binning.
  // Also, it can be used to add two normalized histograms,
  // keeping the properly weighted normalization.
  // The number of entries for the merged histogram
  // will be equal to the NumberEntries of 'a' + NumberEntries of 'b'


  // Getting weights based on number of entries.
  double entries_a = a.GetEntries();
  double entries_b = b.GetEntries();

  double weight_a = 0.0;
  double weight_b = 0.0;

  if (a.GetDimension() != b.GetDimension()) {
    std::cerr<<"merge_weightedEff \""<< a.GetName()<<"\": attempt to merge histograms of different dimensionality\n";
    return;
  }

  // Check whether the sumw2 are present - Added by B.Trocme
  bool sumw2 = (a.GetSumw2N() != 0) && (b.GetSumw2N() != 0);

  Int_t ncells = getNumBins(a);

  if (ncells != getNumBins(b)) {
    std::cerr<<"merge_weightedEff \""<< a.GetName() << "\": attempt to merge histograms of different sizes\n";
    return;
  }

  // do not attempt to automatically extend!
  a.SetCanExtend(TH1::kNoAxis);

  if (entries_a + entries_b > 0)
  {
    weight_a = entries_a / (entries_a + entries_b);
    weight_b = entries_b / (entries_a + entries_b);

    for( int bin = 0; bin < ncells; bin++ ) {
      double binContent_a = a.GetBinContent(bin);
      double binContent_b = b.GetBinContent(bin);

      //Error treatment added by Evan Wulf:
      double binError_a = a.GetBinError(bin);
      double binError_b = b.GetBinError(bin);

      //Filling the histogram with the new weighted values
      float weightedEff = binContent_a * weight_a + binContent_b * weight_b;
      a.SetBinContent(bin, weightedEff);

      //Set Errors:
      float weightedError = sqrt( pow(binError_a * weight_a,2) + pow(binError_b * weight_b,2) );
      a.SetBinError(bin, weightedError);
    }
  }
  // If the original histos did not contain sumw2, delete the sumw2 array created
  // by SetBinError. This may look dirty but this is the recommandation by R.Brun:
  // http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=1620&p=51674&hilit=sumw2#p51674
  // Added by B.Trocme
  if (!sumw2) (a.GetSumw2())->Set(0);
  //Resets number of entries of a:
  a.SetEntries(entries_a + entries_b);
}


void MonitoringFile::merge_Rebinned( TH1& a, TH1& b )
{
  // Author: Luca Fiorini
  // This method provide a correct summation for histograms that have different binning
  // e.g. the histograms with TH1::kCanRebin set to true
  // The method uses TH1::Merge as explained here:
  // http://root.cern.ch/root/html/TH1.html#TH1:Merge
  // The axis x may have different number
  // of bins and different limits, BUT the largest bin width must be
  // a multiple of the smallest bin width and the upper limit must also
  // be a multiple of the bin width.

  TList *list = new TList;
  list->Add(&b);
  a.Merge(list);

  delete list;

}


void MonitoringFile::merge_eventSample( TH2& a, const TH2& b )
{
  // Author: Peter Faulkner
  // Merge histograms containing, for example, event numbers of events
  // with particular types of errors.  Data is inserted/appended row-wise.
  int nbinsx = a.GetNbinsX();
  int nbinsy = a.GetNbinsY();
  if (b.GetNbinsX() != nbinsx || b.GetNbinsY() != nbinsy) return;
  double entries = a.GetEntries();
  for( int biny = 1; biny <= nbinsy; biny++ ) {
    for( int binx = 1; binx <= nbinsx; binx++ ) {
      double bVal = b.GetBinContent(binx, biny);
      if (bVal == 0) break;
      for( int binxa = 1; binxa <= nbinsx; binxa++ ) {
        double aVal = a.GetBinContent(binxa, biny);
	if (aVal == 0) {
	  a.SetBinContent(binxa, biny, bVal);
	  entries++;
	  break;
        } else if (bVal < aVal) {
	  for( int bx = nbinsx; bx > binxa; bx-- ) {
	    double v1 = a.GetBinContent(bx-1, biny);
	    if (v1 == 0) continue;
	    double v2 = a.GetBinContent(bx, biny);
	    if (v2 == 0) entries++;
	    a.SetBinContent(bx, biny, v1);
          }
	  a.SetBinContent(binxa, biny, bVal);
	  break;
        } else if (aVal == bVal) break;
      }
    }
  }
  a.SetEntries(entries);
}

void MonitoringFile::merge_RMS( TH1& a, const TH1& b ) {
  
  //Merge histograms where bins are filled with RMS type data: 
  // Add in quadrature, weighted by the number of events as
  // reconstructed from the errors.

  // Author: Evan Wulf

  if (a.GetDimension() != b.GetDimension()) {
    std::cerr<<"merge_RMS \""<< a.GetName() <<"\": attempt to merge histograms of different dimensionality\n";
    return;
  }

  Int_t ncells = getNumBins(a);

  if (ncells != getNumBins(b)) {
    std::cerr<<"merge_RMS \""<< a.GetName() <<"\": attempt to merge histograms of different sizes\n";
    return;
  }

  // do not attempt to automatically extend!
  a.SetCanExtend(TH1::kNoAxis);

  double nEntries = a.GetEntries();
  nEntries += b.GetEntries();

  for( int bin = 0; bin < ncells; bin++ ) {
    double rms1 = a.GetBinContent(bin);
    double rms2 = b.GetBinContent(bin);
    double e1 = a.GetBinError(bin);
    double e2 = b.GetBinError(bin);
    
    double n1 = 0;
    double n2 = 0;

    if( e1 != 0 ) {
      n1 = pow( rms1 / e1, 2) / 2;
    }
    if( e2 != 0 ) {
      n2 = pow( rms2 / e2, 2) / 2;
    }

    double ntot = n1 + n2;
    if( ntot <= 0 ) {
      a.SetBinContent( bin, sqrt( (rms1*rms1) + (rms2*rms2) ) );
      a.SetBinError( bin, sqrt( (e1*e1) + (e2*e2) ) );
    }
    else {
      double rmstot = sqrt( ( (pow(n1 * rms1,2) / (n1 - 1)) + (pow(n2 * rms2, 2) / (n2 - 1)) ) * (ntot - 1) / pow(ntot,2) );
      a.SetBinContent( bin, rmstot );
      a.SetBinError( bin, rmstot / sqrt( 2 * ntot ) );
    }
  }

  a.SetEntries(nEntries);
}

void MonitoringFile::merge_RMSpercentDeviation( TH1& a, const TH1& b ) {
  
  //Merge histograms where bins are filled with RMS type data which has
  // been normalized to a percent deviation by use of a reference, using
  // content = 100 * (RMS - reference) / reference = (RMS * 100 / reference) - 100 
  // error = RMSerror * 100 / reference.

  // Once constant term (100) is added back in, treatment is the same as with merge_RMS above:

  // Add in quadrature, weighted by the number of events as
  // reconstructed from the errors.

  // Author: Evan Wulf

  if (a.GetDimension() != b.GetDimension()) {
    std::cerr<<"merge_RMSpercentDeviation \"" << a.GetName() <<"\": attempt to merge histograms of different dimensionality\n";
    return;
  }

  Int_t ncells = getNumBins(a);

  if (ncells != getNumBins(b)) {
    std::cerr<<"merge_RMSpercentDeviation \"" << a.GetName() <<"\": attempt to merge histograms of different sizes\n";
    return;
  }

  double nEntries = a.GetEntries();
  nEntries += b.GetEntries();

  for( int bin = 0; bin < ncells; bin++ ) {
    double y1 = a.GetBinContent(bin) + 100;
    double y2 = b.GetBinContent(bin) + 100;
    double e1 = a.GetBinError(bin);
    double e2 = b.GetBinError(bin);
    
    double n1 = 0;
    double n2 = 0;

    if( e1 != 0 ) {
      n1 = pow( y1 / e1, 2) / 2;
    }
    if( e2 != 0 ) {
      n2 = pow( y2 / e2, 2) / 2;
    }

    double ntot = n1 + n2;
    if( ntot <= 0 ) {
      a.SetBinContent( bin, sqrt( (y1*y1) + (y2*y2) ) - 100 );
      a.SetBinError( bin, sqrt( (e1*e1) + (e2*e2) ) );
    }
    else {
      double ytot = sqrt( ( (pow(n1 * y1,2) / (n1 - 1)) + (pow(n2 * y2, 2) / (n2 - 1)) ) * (ntot - 1) / pow(ntot,2) );
      a.SetBinContent( bin, ytot - 100);
      a.SetBinError( bin, ytot / sqrt( 2 * ntot ) );
    }
  }

  a.SetEntries(nEntries);
}

void MonitoringFile::merge_lowerLB( TH1& a, const TH1& b ) {
  
  // Merge "status" histograms, i.e filled at start of run/LB.
  // The histogram title should contain the LB for which the histo was filled
  // such that strcmp can extract the histo of lower LB
  // Be careful to not format your title with %d but rather %4d. Otherwise, 
  // strcmp will return : 2>10
  // Example in : LArCalorimeter/LArMonTools/src/LArCoverage.cxx
  // Author: B.Trocme
  // 

  if (a.GetDimension() != b.GetDimension()) {
    std::cerr<<"merge_lowerLB \"" << a.GetName() <<"\": attempt to merge histograms of different dimensionality\n";
    return;
  }

  Int_t ncells = getNumBins(a);

  if (ncells != getNumBins(b)) {
    std::cerr<<"merge_lowerLB \"" << a.GetName() <<"\": attempt to merge histograms of different sizes\n";
    return;
  }

  // do not attempt to automatically extend!
  a.SetCanExtend(TH1::kNoAxis);

  if (strcmp(a.GetTitle(),b.GetTitle())>0){
    // The LB of histo a is greater than the LB of histo b
    // a is overwritten by b - Otherwise do nothing
    a.SetTitle(b.GetTitle());
    /*
    for( int bin = 0; bin < ncells; bin++ ) {
      a.SetBinContent(bin,b.GetBinContent(bin));
      a.SetBinError(bin,b.GetBinError(bin));
    }
    */
    a.Add(&a, &b, 0, 1);
    // If the original histo did not contain sumw2, delete the sumw2 array created
    // by SetBinError. This may look dirty but this is the recommandation by R.Brun:
    // http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=1620&p=51674&hilit=sumw2#p51674
    /*
    if ((b.GetSumw2N()) == 0) (a.GetSumw2())->Set(0);
    
    a.SetEntries(b.GetEntries()); */
  } 
  return;
}

}//end dqutils namespace

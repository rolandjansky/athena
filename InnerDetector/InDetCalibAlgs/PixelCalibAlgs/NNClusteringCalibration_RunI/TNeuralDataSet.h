/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Vassil Verguilov  11/05/2006

#ifndef __NEURALDATASET_
#define __NEURALDATASET_

//______________________________________________________________________________
//
// TNeuralData
//
// This class is the base class for Neural Network Input/Output patterns
// It contains methods for manupulating the data such as Randomize(),
// Normalize(), Shuffle() and others.
//
//______________________________________________________________________________
//

#include "TMatrixD.h"
#include "TVectorD.h"


class TNeuralDataSet
{
 public:
  TNeuralDataSet( Int_t aNumberOfPatterns = 0, Int_t aNumberOfUnits = 0 );
  virtual ~TNeuralDataSet( void );
  TNeuralDataSet(const TNeuralDataSet &) = delete;
  TNeuralDataSet & operator=(const TNeuralDataSet &) = delete;
  
  // Returns the number of the patterns in set
  Int_t GetPatternsCount( void ){ return mpData->GetNrows(); };
  // Returns the number of the units in pattern
  Int_t GetUnitsCount( void ){ return mpData->GetNcols(); };
  // Returns the data in cell defined by pattern number and unit index in the pattern
  Double_t GetData( const Int_t aPattern, const Int_t aIndex );
  // Change the data in cell defined by pattern number and unit index in the pattern
  void     SetData( const Int_t aPattern, const Int_t aIndex, Double_t aValue );

  Double_t GetEventWeight( const Int_t aPattern );
  void SetEventWeight( const Int_t aPattern, Double_t aValue );

  // Normalize all data
  void     Normalize( void );  
  // Randomizes the data
  void     Randomize( void );
  // Shuffles the patterns
  void     Shuffle( Int_t aSeed = 0 );

 private:  

  //data matrix
  TMatrixD * mpData;

  //vector (dim= entries)
  TVectorD * mpWeights;

  //vector (dim=variables)
  TVectorD * mpNormFactors;
  TVectorD * mpShiftFactors;

  ClassDef( TNeuralDataSet, 1 )

};

inline Double_t TNeuralDataSet::GetData( const Int_t aPattern, const Int_t aIndex )
{
  // Returns the value of cell in the set specified by Pattern number and Unit index
  return mpData->operator() ( aPattern, aIndex );
}
//______________________________________________________________________________
inline Double_t TNeuralDataSet::GetEventWeight( const Int_t aPattern )
{
  return mpWeights->operator() ( aPattern );
}
//______________________________________________________________________________



#endif

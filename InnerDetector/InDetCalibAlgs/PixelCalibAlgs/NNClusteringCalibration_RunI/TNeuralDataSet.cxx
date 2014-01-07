/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TNeuralDataSet.h"
#include "TRandom3.h"

ClassImp( TNeuralDataSet )
//______________________________________________________________________________
TNeuralDataSet::TNeuralDataSet( Int_t aNumberOfPatterns, Int_t aNumberOfUnits )
{
  // Default constructor
  mpData = new TMatrixD( aNumberOfPatterns, aNumberOfUnits ); 
  mpWeights = new TVectorD( aNumberOfPatterns);
  for (int i=0;i<mpWeights->GetNrows();i++){
    (*mpWeights)[i]=1.;
  }

  mpNormFactors = new TVectorD( aNumberOfUnits );
  mpShiftFactors = new TVectorD(  aNumberOfUnits);
}
//______________________________________________________________________________
TNeuralDataSet::~TNeuralDataSet()
{
  // Default destructor
  delete mpData;
  delete mpWeights;
  delete mpNormFactors;
  delete mpShiftFactors;
}
//______________________________________________________________________________
void TNeuralDataSet::SetEventWeight( const Int_t aPattern, Double_t aValue )
{
  mpWeights->operator() ( aPattern ) = aValue;
}
//______________________________________________________________________________
void TNeuralDataSet::SetData( const Int_t aPattern, const Int_t aIndex, Double_t aValue )
{
  // Changes the value of cell in the set specified by Pattern number and Unit index
  mpData->operator() ( aPattern, aIndex ) = aValue;
}
//______________________________________________________________________________
void TNeuralDataSet::Normalize( void )
{
  // Normalizes data
  Double_t fMin;
  Double_t fMax;
  Double_t tmp;

  for( UInt_t aUnitIndex = 0; aUnitIndex < this->GetUnitsCount(); aUnitIndex++ )
  {
    fMax = 0.0;
    fMin = 0.0; 
    for( UInt_t aPatternIndex = 0; aPatternIndex < this->GetPatternsCount(); aPatternIndex++ )
    {
	tmp = this->GetData( aPatternIndex, aUnitIndex );
	fMin = ( tmp < fMin )? tmp : fMin;
	fMax = ( tmp > fMax )? tmp : fMax;
    }

    for( UInt_t aPatternIndex = 0; aPatternIndex < this->GetPatternsCount(); aPatternIndex++ )
    {
      tmp = this->GetData( aPatternIndex, aUnitIndex );
      tmp = (tmp - fMin)/(fMax-fMin);
      this->SetData( aPatternIndex, aUnitIndex, tmp );
    }

    mpNormFactors->operator() (aUnitIndex) = 1./(fMax-fMin);
    mpShiftFactors->operator() (aUnitIndex) = -fMin/(fMax-fMin);

  }
}
//______________________________________________________________________________
void TNeuralDataSet::Randomize( void )
{
  // Randomizes the data
  TRandom3 r3;
  for( Int_t i = 0; i < this->GetPatternsCount(); i ++ )
    for( Int_t j = 0; j < this->GetUnitsCount(); j ++ ) 
    {
      this->SetData( i, j, r3.Gaus(0, 5) );  
    }
}
//______________________________________________________________________________
void TNeuralDataSet::Shuffle( Int_t aSeed )
{
  // Shuffles data
  TRandom3 Random( aSeed );
  Int_t j;
  Double_t tmp;
  Int_t a = this->GetPatternsCount() - 1;
  for ( Int_t i = 0; i < this->GetPatternsCount(); i++ )
  {
     j = ( Int_t ) ( Random.Rndm() * a );
     for( Int_t p = 0; p < this->GetUnitsCount(); p++ )
     {
	tmp = this->GetData( i, p );
	this->SetData( i, p, this->GetData( j, p ) );
	this->SetData( j, p, tmp );
     }
     tmp = this->GetEventWeight(i);
     this->SetEventWeight(i,this->GetEventWeight(j));
     this->SetEventWeight(j,tmp);
  }
}

//EOF

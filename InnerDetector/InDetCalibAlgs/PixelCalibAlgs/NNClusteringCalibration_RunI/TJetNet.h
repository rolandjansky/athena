/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 ROOT Interface to JETNET
 Author: Vassil Verguilov
 Created: 2005.02.22 14:07 EET
 Revisited and extended by: Giacinto Piacquadio (Freiburg University)
 (ATLAS - 18-02-2008)
*/



#ifndef __TJETNET_
#define __TJETNET_

//______________________________________________________________________________
//
// TJetNet
//
// This class is a ROOT wrapper for jetnet library.
//
//______________________________________________________________________________
//

//#define _DEBUG

#include "TROOT.h"
#include "TObject.h"
#include "Riostream.h"
#include "TNeuralDataSet.h"
#include "TF1.h"

class TTrainedNetwork;


//typedef ActivationFunction  TActivationFunction;

class TJetNet : public TObject
{
 public:
  
  TJetNet( void );

  TJetNet( Int_t aTestCount, Int_t aTrainCount, const Int_t aLayersCnt, const Int_t* aLayers );

  virtual ~TJetNet( void );
  
  void Print( void );
  
  Int_t GetTrainSetCnt( void ) const { return mTrainSetCnt; };
  Int_t GetTestSetCnt( void ) const { return mTestSetCnt; };
  Int_t GetInputDim( void ) const { return mpLayers[ 0 ]; };
  Int_t GetHiddenLayerDim( void ) const { return mHiddenLayerDim; };
  Int_t GetHiddenLayerSize(Int_t number) const { return mpLayers[ number ]; };
  Int_t GetOutputDim( void ) const { return mpLayers[ mLayerCount - 1 ]; };

  void SetInputTrainSet( Int_t aPatternInd, Int_t aInputInd, Double_t aValue );
  void SetOutputTrainSet( Int_t aPatternInd, Int_t aOutputInd, Double_t aValue );
  void SetInputTestSet( Int_t aPatternInd, Int_t aInputInd, Double_t aValue );
  void SetOutputTestSet( Int_t aPatternInd, Int_t aOutputInd, Double_t aValue );
  
  void SetEventWeightTrainSet( Int_t aPatternInd, Double_t aValue );
  void SetEventWeightTestSet( Int_t aPatternInd, Double_t aValue );

  Double_t GetInputTrainSet( Int_t aPatternInd, Int_t aInputInd );
  Double_t GetOutputTrainSet( Int_t aPatternInd, Int_t aOutputInd );
  Double_t GetInputTestSet( Int_t aPatternInd, Int_t aInputInd );
  Double_t GetOutputTestSet( Int_t aPatternInd, Int_t aOutputInd );
  
  Double_t GetEventWeightTrainSet( Int_t aPatternInd );
  Double_t GetEventWeightTestSet( Int_t aPatternInd );

  Double_t GetWeight( Int_t aLayerInd, Int_t aNodeInd, Int_t aConnectedNodeInd ) const; 
  Double_t GetThreshold( Int_t aLayerInd, Int_t aNodeInd) const;

  Int_t GetEpochs( void ) { return mEpochs; };
  void SetEpochs( const Int_t aEpochs ) { mEpochs = aEpochs; mCurrentEpoch = 0; };
  void Init( void );

  Double_t Train( void );
  Int_t Epoch( void );
  Double_t Test( void );
  Double_t TestBTAG( void );

  void Shuffle ( Bool_t aShuffleTrainSet = true, Bool_t aShuffleTestSet = true );
 
  void SaveDataAscii( TString aFileName = "jndata.dat" );
  void SaveDataRoot( TString aFileName = "jndata.root" );

  void LoadDataAscii( TString aFileName = "jndata.dat" );
  void LoadDataRoot( TString aFileName = "jndata.root" );
  
  void DumpToFile( TString aFileName = "fort.8" );
  void ReadFromFile( TString aFileName = "fort.8" );

  Double_t GetOutput( Int_t aIndex = 0 );
  void SetInputs( Int_t aIndex = 0, Double_t aValue = 0.0 );
  void Evaluate( Int_t aPattern );
  void Evaluate();

  void writeNetworkInfo(Int_t typeOfInfo = 0);

  Int_t GetLayerCount( void ){ return mLayerCount; };
  //Number of layers in NN
  Int_t GetUnitCount( Int_t aLayer );
  //Number of units in NN  

  void SelectiveFields( Int_t aLayerA, Int_t aNodeA1, Int_t aNodeA2, Int_t aNodeB1, Int_t aNodeB2, Int_t aSwitch = 0 );
  
  void SetUpdatesPerEpoch( Int_t aValue );
    void SetUpdatingProcedure( Int_t aValue );
    void SetErrorMeasure( Int_t aValue );
    void SetActivationFunction( Int_t aValue );
    void SetActivationFunction( Int_t aValue, Int_t layerN );
    void SetPatternsPerUpdate( Int_t aValue );
    void SetLearningRate( Double_t aValue );
    void SetMomentum( Double_t aValue );
    void SetInitialWeightsWidth( Double_t aValue );
    void SetLearningRateDecrease( Double_t aValue );
    void SetPottsUnits(Int_t aValue);
  

    Int_t GetUpdatesPerEpoch( void );
    Int_t GetUpdatingProcedure( void );
    Int_t GetErrorMeasure( void );
    Int_t GetActivationFunction( void ) const;
    Int_t GetActivationFunction( Int_t layerN) const;
    Int_t GetPatternsPerUpdate( void );
    Double_t GetLearningRate( void );
    Double_t GetMomentum( void );
    Double_t GetInitialWeightsWidth( void );
    Double_t GetLearningRateDecrease( void );
    Int_t GetPottsUnits();
    
    void LockInit( void ){ mInitLocked = kTRUE; };
    void UnlockInit( void ){ mInitLocked = kFALSE; };
    Int_t GetMSTJN( Int_t aIndex );
    Double_t GetPARJN( Int_t aIndex );
    void SetMSTJN( Int_t aIndex, Int_t aValue );
    void SetPARJN( Int_t aIndex, Double_t aValue );
  
    void Normalize( void );
    void Randomize( void );

    TTrainedNetwork* createTrainedNetwork() const;
    void readBackTrainedNetwork(const TTrainedNetwork*);

    enum TActivationFunction {
      afSigmoid = 1,
      afTanh    = 2,
      afExp     = 3,
      afLinear  = 4,
      afSigmoidEntropy = 5
    };

  void NormalizeOutput(bool isTrue=true);
    
private:
  
  void mSetWeight( Double_t weight,Int_t aLayerInd, Int_t aNodeInd, Int_t aConnectedNodeInd ); 
  void mSetThreshold( Double_t threshold, Int_t aLayerInd, Int_t aNodeInd);

  Int_t CopyFile( TString aSrcFile, TString aDestFile );
  void Reinitialize( void ); // Synchronizing the paramaters of the class object from JETNET parameters



  TActivationFunction menActFunction;

  Int_t  mLayerCount; // Number of Layers (including the input and output)
  Int_t* mpLayers; //! Array which contains the number of units in each layer 

  TNeuralDataSet* mpInputTrainSet;
  TNeuralDataSet* mpOutputTrainSet;
  TNeuralDataSet* mpInputTestSet;
  TNeuralDataSet* mpOutputTestSet;

  Int_t mTrainSetCnt, mTestSetCnt; // Size of Train and Test set

  Int_t mInputDim; // Number of the elements in intput layer
  Int_t mHiddenLayerDim; // Number of Hidden Layers
  Int_t mOutputDim; // Number of outputs
  Int_t mEpochs;    // Epochs
  Int_t mCurrentEpoch;    // Current epoch
  Bool_t mDebug; // Debug Flag
  Bool_t mIsInitialized;
  Bool_t mInitLocked;
  Bool_t mNormalizeOutput;


  ClassDef( TJetNet, 1 )
}; 

inline void TJetNet::SetOutputTestSet( Int_t aPatternInd, Int_t aOutputInd, Double_t aValue )
{
  // Changes the value of the cell corresponding to unit aInputInd in pattern aPatternInd into OUTPUT TEST set
  mpOutputTestSet->SetData( aPatternInd, aOutputInd, aValue );
}
//______________________________________________________________________________
inline Double_t TJetNet::GetInputTrainSet( Int_t aPatternInd, Int_t aInputInd )
{
  // Returns the value of the cell corresponding to unit aInputInd in pattern aPatternInd into INPUT TRAIN set
  return mpInputTrainSet->GetData( aPatternInd, aInputInd );
}
//______________________________________________________________________________
inline Double_t TJetNet::GetEventWeightTrainSet( Int_t aPatternInd )
{
  return mpInputTrainSet->GetEventWeight( aPatternInd);
}
//______________________________________________________________________________
inline Double_t TJetNet::GetEventWeightTestSet( Int_t aPatternInd )
{
  return mpInputTestSet->GetEventWeight( aPatternInd);
}

#endif


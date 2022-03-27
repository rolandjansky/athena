/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TJetNet.h"
#include "jetnet.h"
#include "TRandom3.h"
#include "TTimeStamp.h"
#include <unistd.h>
#include <cstdio>
#include <cmath>
#include "TTrainedNetwork.h"
#include "TFile.h"

#include "TH1F.h"

ClassImp( TJetNet )
 
//Constructors
//______________________________________________________________________________
TJetNet::TJetNet()
  : m_LayerCount(0),
    m_pLayers(0),
    m_pInputTrainSet(nullptr),
    m_pOutputTrainSet(nullptr),
    m_pInputTestSet(nullptr),
    m_pOutputTestSet(nullptr),
    m_TrainSetCnt(0),
    m_TestSetCnt(0),
    m_Epochs(0),
    m_CurrentEpoch(0),
    m_IsInitialized(kFalse),
    m_InitLocked(kFALSE),
    m_NormalizeOutput(false)
{
}
//______________________________________________________________________________
TJetNet::TJetNet( Int_t aTestCount, Int_t aTrainCount,
	 const Int_t aLayersCnt, const Int_t* aLayers )
#ifdef _DEBUG
  : m_Debug (kTRUE)
#else
  : m_Debug (kFALSE)
#endif
{
  // Creates neural network with aLayersCnt number of layers,
  // aTestCount number of patterns for the test set,
  // aTrainCount patterns for the train set.
  // aLayers contains the information for number of the units in the different layers

  int i;
  
  if( m_Debug ){ std::cout << "=====> Entering TJetNet::TJetNet(...)" << std::endl; }

  m_TestSetCnt  = aTestCount;
  m_TrainSetCnt = aTrainCount;
  m_LayerCount  = aLayersCnt; // Get the number of layers
  
  if( m_LayerCount > 0 )
  {  
    //Perform deep copy of the array holding Layers count
    m_pLayers = new Int_t[ m_LayerCount ];
    for( i = 0; i < m_LayerCount; ++i )
    {
      m_pLayers[ i ] = aLayers[ i ];
    }
  }

  m_InputDim = m_pLayers[ 0 ];
  m_OutputDim = m_pLayers[ m_LayerCount - 1 ];
  m_HiddenLayerDim = m_LayerCount-2;
  

  m_IsInitialized = kFALSE;
  m_InitLocked = kFALSE;

  m_pInputTrainSet  = new TNeuralDataSet( m_TrainSetCnt, GetInputDim() );
  m_pInputTestSet = new TNeuralDataSet( m_TestSetCnt, GetInputDim() );
  m_pOutputTrainSet = new TNeuralDataSet( m_TrainSetCnt, GetOutputDim() );
  m_pOutputTestSet = new TNeuralDataSet( m_TestSetCnt, GetOutputDim() );
  
  m_NormalizeOutput=false;

  m_enActFunction=afSigmoid;

  SetEpochs( -1 );

  if( m_Debug ){ std::cout << "=====> Leaving TJetNet::TJetNet(...)" << std::endl; }
}
//______________________________________________________________________________
TJetNet::~TJetNet( void )
{
  // Default destructor
  if( m_Debug ){ std::cout << "=====> Entering TJetNet::~TJetNet(...)" << std::endl; }
  delete [] m_pLayers;
  delete m_pInputTestSet;
  delete m_pInputTrainSet;
  delete m_pOutputTestSet;
  delete m_pOutputTrainSet;
  if( m_Debug ){ std::cout << "=====> Leaving TJetNet::~TJetNet(...)" << std::endl; }
}
//______________________________________________________________________________
//by Giacinto Piacquadio (18-02-2008)
TTrainedNetwork* TJetNet::createTrainedNetwork() const
{

  Int_t nInput=GetInputDim();
  Int_t nHidden=GetHiddenLayerDim();
  std::vector<Int_t> nHiddenLayerSize;
  //  Int_t* nHiddenLayerSize=new Int_t[nHidden];

  for (Int_t o=0;o<nHidden;++o)
  {
    nHiddenLayerSize.push_back(GetHiddenLayerSize(o+1));
  }
  Int_t nOutput=GetOutputDim();

  std::vector<TVectorD*> thresholdVectors;
  std::vector<TMatrixD*> weightMatrices;

  for (Int_t o=0;o<nHidden+1;++o)
  {
    int sizeActualLayer=(o<nHidden)?nHiddenLayerSize[o]:nOutput;
    int sizePreviousLayer=(o==0)?nInput:nHiddenLayerSize[o-1];
    thresholdVectors.push_back(new TVectorD(sizeActualLayer));
    weightMatrices.push_back(new TMatrixD(sizePreviousLayer,sizeActualLayer));
  }

  for (Int_t o=0;o<nHidden+1;++o)
  {

    if (m_Debug)
      if (o<nHidden)
      {
        cout << " Iterating on hidden layer n.: " << o << endl;
      }
      else
      {
        cout << " Considering output layer " << endl;
      }
    
    int sizeActualLayer=(o<nHidden)?nHiddenLayerSize[o]:nOutput;

    for (Int_t s=0;s<sizeActualLayer;++s)
    {
      if (o<nHidden)
      {
	if (m_Debug)
          cout << " To hidden node: " << s << endl;
      }
      else
      {
	if (m_Debug)
          cout << " To output node: " << s << endl;
      }
      if (o==0)
      {
	for (Int_t p=0;p<nInput;++p)
	{
	  if (m_Debug)
            cout << " W from inp nod: " << p << "weight: " <<
              GetWeight(o+1,s+1,p+1) << endl;
	  weightMatrices[o]->operator() (p,s) = GetWeight(o+1,s+1,p+1);
        }
      }
      else
      {
	for (Int_t p=0;p<nHiddenLayerSize[o-1];++p)
	{
	  if (m_Debug)
            cout << " W from lay : " << o-1 << " nd: " << 
              p << " weight: " <<
              GetWeight(o+1,s+1,p+1) << endl;
	  weightMatrices[o]->operator() (p,s)=GetWeight(o+1,s+1,p+1);
	}
      }
      if (m_Debug)
        cout << " Threshold for node " << s << " : " << 
          GetThreshold(o+1,s+1) << endl;
      thresholdVectors[o]->operator() (s) = GetThreshold(o+1,s+1);
    }
  }

  bool linearOutput=false;
  if (this->GetActivationFunction(nHidden)==4) 
  {
    //    cout << " Creating TTrainedNetwork with linear output function" << endl;
    linearOutput=true;
  }

  TTrainedNetwork* trainedNetwork=
    new TTrainedNetwork(nInput,
			nHidden,
			nOutput,
			nHiddenLayerSize,
			thresholdVectors,
			weightMatrices,
			m_enActFunction,
                        linearOutput,
                        m_NormalizeOutput);
						
  return trainedNetwork;

}
//______________________________________________________________________________
//by Giacinto Piacquadio (18-02-2008)
void TJetNet::readBackTrainedNetwork(const TTrainedNetwork* trainedNetwork)
{

  Int_t nInput=GetInputDim();
  Int_t nHidden=GetHiddenLayerDim();
  std::vector<Int_t> nHiddenLayerSize;

  if (trainedNetwork->getnHidden()!=nHidden)
  {
    cout << " Network doesn't match.. not loading.." << endl;
    return;
  }

  for (Int_t o=0;o<nHidden;++o)
  {
    nHiddenLayerSize.push_back(GetHiddenLayerSize(o+1));
    if (nHiddenLayerSize[o]!=trainedNetwork->getnHiddenLayerSize()[o])
    {
      cout << " Network doesn't match... not loading..." << endl;
      return;
    }
  }
  Int_t nOutput=GetOutputDim();

  if (trainedNetwork->getnInput()!=nInput)
  {
    cout << " Network doesn't match... not loading.." << endl;
    return;
  }


  if (trainedNetwork->getnOutput()!=nOutput)
  {
    cout << " Network doesn't match.. not loading.." << endl;
    return;
  }
  
  //OK, everything matches... can go on...
  
  std::vector<TVectorD*> thresholdVectors=trainedNetwork->getThresholdVectors();
  std::vector<TMatrixD*> weightMatrices=trainedNetwork->weightMatrices();
  //ownership remains of the TTrainedNetwork

  for (Int_t o=0;o<nHidden+1;++o)
  {
    int sizeActualLayer=(o<nHidden)?nHiddenLayerSize[o]:nOutput;
    int sizePreviousLayer=(o==0)?nInput:nHiddenLayerSize[o-1];
  
    for (Int_t s=0;s<sizeActualLayer;++s)
    {
      Double_t nodeValue=0.;
      if (o==0)
      {
	for (Int_t p=0;p<nInput;++p)
	{
	  mSetWeight(weightMatrices[o]->operator() (p,s),o+1,s+1,p+1);
        }
      }
      else
      {
	for (Int_t p=0;p<nHiddenLayerSize[o-1];++p)
	{
	  mSetWeight(weightMatrices[o]->operator() (p,s),o+1,s+1,p+1);
	}
      }
      mSetThreshold(thresholdVectors[o]->operator() (s),o+1,s+1);
    }
  }     
  if (trainedNetwork->getIfLinearOutput()==true)
  {
    cout << " Setting linear output function " << endl;
    this->SetActivationFunction(nHidden,4);
  }
  
  cout << " Successfully read back Trained Network " << endl;
}
//______________________________________________________________________________
  
void TJetNet::mSetWeight( Double_t weight,Int_t aLayerInd, Int_t aNodeInd, Int_t aConnectedNodeInd )
{
  JNINT1.W[ JNINDX( aLayerInd, aNodeInd, aConnectedNodeInd )-1 ]=weight;
}
//______________________________________________________________________________
void TJetNet::mSetThreshold( Double_t threshold, Int_t aLayerInd, Int_t aNodeInd)
{
  JNINT1.T[ JNINDX( aLayerInd, aNodeInd, 0 )-1 ]=threshold;
}
//______________________________________________________________________________
void TJetNet::Print( void )
{
  // Prints on the screen, information for the neural network
  Int_t i;

  std::cout << "TJetNet" << std::endl;
  std::cout << "Number of layers: " << m_LayerCount << std::endl;

  for( i = 0; i < m_LayerCount; i++ )
  {
    std::cout << "\t\tNumber of units in layer " << i << " : " <<  m_pLayers[ i ] << std::endl;
  }

  std::cout << "Epochs: " << GetEpochs() << std::endl;
  std::cout << "Updates Per Epoch: " << GetUpdatesPerEpoch() << std::endl;
  std::cout << "Updating Procedure: " << GetUpdatingProcedure() << std::endl;
  std::cout << "Error Measure: " << GetErrorMeasure() << std::endl;
  std::cout << "Patterns Per Update: " << GetPatternsPerUpdate() << std::endl;
  std::cout << "Learning Rate: " << GetLearningRate() << std::endl;
  std::cout << "Momentum: " << GetMomentum() << std::endl;
  std::cout << "Initial Weights Width: " << GetInitialWeightsWidth() << std::endl;
  std::cout << "Learning Rate Decrease: " << GetLearningRateDecrease() << std::endl;
  std::cout << "Activation Function: " << GetActivationFunction() << std::endl;
}
//______________________________________________________________________________
Double_t TJetNet::Test( void )
{
  // Initiate test cycle of the neural network
  Int_t NRight = 0;
  Double_t fMeanError = 0.0;
  Double_t *TMP;
  Int_t  NPatterns = GetTestSetCnt();
 

  for( Int_t iPattern = 0; iPattern < NPatterns; iPattern++ )
  {

    for( Int_t i = 0; i < GetInputDim(); i++ )
    {
      JNDAT1.OIN[ i ] = float ( GetInputTestSet( iPattern, i ) );
    }

    NWJNWGT.OWGT = GetEventWeightTestSet( iPattern );

    JNTEST();

    for( Int_t j = 0; j < GetOutputDim(); j++ )
    {
      fMeanError+= NWJNWGT.OWGT * 
        std::pow(JNDAT1.OUT[ j ]-float( GetOutputTestSet( iPattern, j )),2)/(float)GetOutputDim();
    }



    if( m_Debug ) std::cout << "Testing [ " << iPattern << " ] - "  << JNDAT1.OIN[ 0 ] 
                           << " => " << JNDAT1.OUT[ 0 ] << std::endl;

  }

  fMeanError/=2.*NPatterns;

  if (m_Debug)
    std::cout << " Test error: " << fMeanError << endl;

  return fMeanError;
}
//
Double_t TJetNet::TestBTAG( void )
{

  bool test=false;

  // Initiate test cycle of the neural network
  Int_t NRight = 0;
  Double_t fMeanError = 0.0;
  Double_t *TMP;
  Int_t  NPatterns = GetTestSetCnt();
  if (test)
  {
    NPatterns = GetTrainSetCnt();
  }
 
  vector<double> eff;
  eff.push_back(0.5);
  eff.push_back(0.6);
  eff.push_back(0.7);

  //test also the b-tagging performance directly during testing !!!
  vector<TH1F*> histoEfficienciesC;
  vector<TH1F*> histoEfficienciesL;
  TString histoEffStringC("histoEffC");
  TString histoEffStringL("histoEffL");
  for (int i=0;i<GetOutputDim();i++)
  {
    TString string1=histoEffStringC;
    string1+=i;
    TH1F* histo=new TH1F(string1,
                         string1,
                         20000,
                         -2,3);
    TString string2=histoEffStringL;
    string2+=i;
    TH1F* histo2=new TH1F(string2,
                          string2,
                          20000,
                          -2,3);
    histoEfficienciesC.push_back(histo);
    histoEfficienciesL.push_back(histo2);
  }

  for( Int_t iPattern = 0; iPattern < NPatterns; iPattern++ )
  {

    for( Int_t i = 0; i < GetInputDim(); i++ )
    {
      if (!test)
      {
        JNDAT1.OIN[ i ] = float ( GetInputTestSet( iPattern, i ) );
        NWJNWGT.OWGT = GetEventWeightTestSet( iPattern );
      }
      else
      {
        JNDAT1.OIN[ i ] = float (GetInputTrainSet( iPattern, i ) );
        NWJNWGT.OWGT = GetEventWeightTrainSet( iPattern );
      }

    }

    JNTEST();

    int active=0;
    for( Int_t j = 0; j < GetOutputDim(); j++ )
    {
      if (!test)
      {
        fMeanError+= NWJNWGT.OWGT * 
          std::pow(JNDAT1.OUT[ j ]-float( GetOutputTestSet( iPattern, j )),2)/(float)GetOutputDim();
      }
      else
      {
        fMeanError+= NWJNWGT.OWGT * 
          std::pow(JNDAT1.OUT[ j ]-float( GetOutputTrainSet( iPattern, j )),2)/(float)GetOutputDim();
      }
        

      //        std::cout << " j " <<  j << " is " << GetOutputTestSet( iPattern, j) << std::endl;

      if (!test)
      {
        if (fabs(float( GetOutputTestSet( iPattern, j)) - 1) < 1e-4)
        {
          active = j;
        }
      }
      else
      {
        if (fabs(float( GetOutputTrainSet( iPattern, j)) - 1) < 1e-4)
        {
          active = j;
        }
      }
    }

    //      if (m_Debug) std::cout << " active is: " << active << std::endl;

    //      if (m_Debug) std::cout << " filling histograms " << std::endl;

    if (JNDAT1.OUT[ 0 ] + JNDAT1.OUT[ 1 ] >= 0)
    {
      histoEfficienciesC[active]->Fill( JNDAT1.OUT[ 0 ] / 
                                        ( JNDAT1.OUT[ 0 ] + JNDAT1.OUT[ 1 ]),
                                        NWJNWGT.OWGT);
                                                                                
      //        if( m_Debug ) std::cout << "Filled: " << JNDAT1.OUT[ 0 ] / 
      //                         ( JNDAT1.OUT[ 0 ] + JNDAT1.OUT[ 2 ]) << std::endl;
      
    }
    else
    {
      std::cout << " Filled 0 " << std::endl;
      histoEfficienciesC[active]->Fill( 0 );
    }
      
        
    if (JNDAT1.OUT[ 0 ] + JNDAT1.OUT[ 2 ] >= 0)
    {
      histoEfficienciesL[active]->Fill( JNDAT1.OUT[ 0 ] / 
                                        ( JNDAT1.OUT[ 0 ] + JNDAT1.OUT[ 2 ]),
                                        NWJNWGT.OWGT);
      //        if( m_Debug ) std::cout << "Filled: " << JNDAT1.OUT[ 0 ] / 
      //                         ( JNDAT1.OUT[ 0 ] + JNDAT1.OUT[ 1 ]) << std::endl;
      
    }
    else
    {
      std::cout << " Filled 0 " << std::endl;
      histoEfficienciesL[active]->Fill( 0 );
    }

    if( m_Debug ) std::cout << "Testing [ " << iPattern << " ] - "  << JNDAT1.OIN[ 0 ] 
                           << " => " << JNDAT1.OUT[ 0 ] << std::endl;
      
  }// finish patterns
  
  if (m_Debug) std::cout << " Finished patterns... " << std::endl;
  
  TFile* newFile=new TFile("test.root","recreate");
  histoEfficienciesL[0]->Write();
  histoEfficienciesL[1]->Write();
  histoEfficienciesL[2]->Write();
  histoEfficienciesC[0]->Write();
  histoEfficienciesC[1]->Write();
  histoEfficienciesC[2]->Write();
  newFile->Write();
  newFile->Close();

  //for C-jet rejection
  
  for (int u=0;u<2;u++)
  {
    vector<TH1F*>* myVectorHistos;
    if (u==0) 
    {
      std::cout << "c-rej --> ";
      myVectorHistos=&histoEfficienciesC;
    }
    if (u==1)
    {
      std::cout << "l-rej --> ";
      myVectorHistos=&histoEfficienciesL;
    }
    
      
    if (m_Debug) std::cout << " 1 " << std::endl;
    
    Double_t allb=(*myVectorHistos)[0]->GetEntries();
    Double_t allc=(*myVectorHistos)[1]->GetEntries();
    Double_t allu=(*myVectorHistos)[2]->GetEntries();
    
    if (m_Debug) std::cout << " allb " << allb << std::endl;

    Double_t allbsofar=0;
    
    vector<int> binN_Eff;
    vector<bool> ok_eff;
    
    for (int r=0;r<eff.size();r++)
    {
      ok_eff.push_back(false);
      binN_Eff.push_back(0);
    }

    for (int s=0;s<(*myVectorHistos)[0]->GetNbinsX()+1;s++) {
      allbsofar+=(*myVectorHistos)[0]->GetBinContent((*myVectorHistos)[0]->GetNbinsX()+1-s);
      bool nothingMore(true);

 
      for (int r=0;r<eff.size();r++)
      {
        if (m_Debug) std::cout << " actual eff: " << allbsofar / allb << std::endl;

        if ((!ok_eff[r]) && allbsofar / allb > eff[r])
        {
          binN_Eff[r]=s;
          ok_eff[r]=true;
          if (m_Debug) std::cout << " bin: " << s << " eff: " << allbsofar / allb << std::endl;
          //          std::cout << " Cut value: " << (*myVectorHistos)[0]->GetBinCenter(s) << std::endl;
        }
        else if (allbsofar / allb <= eff[r])
        {
          nothingMore=false;
        }
      }
      if (nothingMore) break;
    }
    
    
    for (int r=0;r<eff.size();r++)
    {
      
      std::cout << " " << eff[r];

      std::cout << " check: " << (double)(*myVectorHistos)[0]->Integral((*myVectorHistos)[0]->GetNbinsX()-binN_Eff[r],
                                                                        (*myVectorHistos)[1]->GetNbinsX()+1)
        / (double)allb;
      
      double effc=(*myVectorHistos)[1]->Integral((*myVectorHistos)[0]->GetNbinsX()-binN_Eff[r],
                                                 (*myVectorHistos)[1]->GetNbinsX()+1);
      effc /= allc;
      double effl=(*myVectorHistos)[2]->Integral((*myVectorHistos)[0]->GetNbinsX()-binN_Eff[r],
                                                 (*myVectorHistos)[2]->GetNbinsX()+1);
      effl /= allu;
      
      if (effc!=0)
      {
        std::cout << " c: " << 1/effc;
      }
      if (effl!=0)
      {
        std::cout << " l: " << 1/effl;
      }
      
    }
    std::cout << std::endl;
  }
  
  for( Int_t j = 0; j < GetOutputDim(); j++ )
  {
    delete histoEfficienciesC[j];
    delete histoEfficienciesL[j];
  }
   

  fMeanError/=2.*NPatterns;

  if (m_Debug)
    std::cout << " Test error: " << fMeanError << endl;

  return fMeanError;
}


//______________________________________________________________________________
Double_t TJetNet::Train( void )
{
  // Initiate the train phase for the neural network
  Int_t NRight = 0;
  Double_t fMeanError = 0.0;
  Int_t  NPatterns = GetTrainSetCnt();

  //  cout << " NPatterns is: " << NPatterns << endl;

  Int_t inputDim=GetInputDim();
  Int_t outputDim=GetOutputDim();
  Int_t updatesPerEpoch=GetUpdatesPerEpoch();
  Int_t patternsPerUpdate=GetPatternsPerUpdate();
  
  if (updatesPerEpoch*patternsPerUpdate<1./2.*NPatterns) 
  {
    cout << "Using only: " << updatesPerEpoch*patternsPerUpdate << 
      " patterns on available: " << NPatterns << endl;
  } else if (updatesPerEpoch*patternsPerUpdate>NPatterns) 
  {
    cout << " Trying to use " << updatesPerEpoch*patternsPerUpdate << 
      " patterns, but available: " << NPatterns << endl;
    return -100;
  }
  
  for( Int_t iPattern = 0; iPattern < updatesPerEpoch*patternsPerUpdate;
       iPattern++ )
  {
    for( Int_t i = 0; i < inputDim; i++ )
    {
      JNDAT1.OIN[ i ] = float ( GetInputTrainSet( iPattern, i ) );
    }
    
    NWJNWGT.OWGT = GetEventWeightTrainSet( iPattern );
    
    for( Int_t j = 0; j < outputDim; j++ )
    {
      JNDAT1.OUT[ j ] = float ( GetOutputTrainSet( iPattern, j ) );
    }

    JNTRAL();
  }

  return GetPARJN(8);
}
//______________________________________________________________________________
void TJetNet::writeNetworkInfo(Int_t typeOfInfo)
{
  cout << " Invoking info of type: " << typeOfInfo << endl;
  JNSTAT(typeOfInfo);
}  
//______________________________________________________________________________
void TJetNet::Init( void )
{
  // Initializes the neuaral network
  Int_t i;
  JNDAT1.MSTJN[ 0 ] = m_LayerCount; // Set the number of layers

  // Set the number of nodes for each layer
  for( i = 0; i < m_LayerCount; i++ )
  {
    if ( m_Debug ) std::cout << "Layer " << i + 1 << " has " << m_pLayers[ i ] << " units." << std::endl; 
    JNDAT1.MSTJN[ 9 + i ] = m_pLayers[ i ]; 
  }
   
  cout << " calling JNINIT " << endl;
  JNINIT();

  if (m_NormalizeOutput)  
  {
    std::cout << " Setting to normalize output nodes: POTT nodes " << std::endl;
    SetPottsUnits(m_pLayers[m_LayerCount-1]); 
  }
  
  cout << " finishing calling JNINIT " << endl;
  m_IsInitialized = kTRUE;
}
//______________________________________________________________________________
Int_t TJetNet::Epoch( void )
{
  // Initiate one train/test step the network. 

  Double_t aTrain, aTest;
  if ( m_CurrentEpoch < m_Epochs )
  {
    m_CurrentEpoch++;
    aTrain = Train();

    //      if (m_CurrentEpoch%2)

    //      std::cout << " Calls to MSTJN: " << GetMSTJN(6) << 
    //	std::endl;

    if ( m_Debug ) 
    {


      std::cout << "[ " << m_CurrentEpoch << " ] Train: " << aTrain << std::endl;
    }
    if ( ( m_CurrentEpoch % 2 ) == 0 )
    {
      aTest = Test();
      //	  if ( m_Debug )
      std::cout << "[" << m_CurrentEpoch << "]: " << GetPARJN(8) << " ";
      std::cout << "Test: " << aTest << std::endl;
    }
  }
  return m_CurrentEpoch;
}
//______________________________________________________________________________
void TJetNet::SetInputTrainSet( Int_t aPatternInd, Int_t aInputInd, Double_t aValue )
{
  // Changes the value of the cell corresponding to unit aInputInd in pattern aPatternInd into INPUT TRAIN set
  m_pInputTrainSet->SetData( aPatternInd, aInputInd, aValue );
}
//______________________________________________________________________________
void TJetNet::SetOutputTrainSet( Int_t aPatternInd, Int_t aOutputInd, Double_t aValue )
{
  // Changes the value of the cell corresponding to unit aInputInd in pattern aPatternInd into OUTPUT TRAIN set
  m_pOutputTrainSet->SetData( aPatternInd, aOutputInd, aValue );
}
//______________________________________________________________________________
void TJetNet::SetInputTestSet( Int_t aPatternInd, Int_t aInputInd, Double_t aValue )
{
  // Changes the value of the cell corresponding to unit aInputInd in pattern aPatternInd into INPUT TEST set
  m_pInputTestSet->SetData( aPatternInd, aInputInd, aValue );
}
//______________________________________________________________________________
Double_t TJetNet::GetOutputTrainSet( Int_t aPatternInd, Int_t aOutputInd )
{
  // Returns the value of the cell corresponding to unit aInputInd in pattern aPatternInd into OUTPUT TRAIN set
  return m_pOutputTrainSet->GetData( aPatternInd, aOutputInd );
}
//______________________________________________________________________________
void TJetNet::SetEventWeightTrainSet( Int_t aPatternInd, Double_t aValue )
{
  m_pInputTrainSet->SetEventWeight(aPatternInd,aValue);
}
//______________________________________________________________________________

void TJetNet::SetEventWeightTestSet( Int_t aPatternInd, Double_t aValue )
{
  m_pInputTestSet->SetEventWeight(aPatternInd,aValue);
}
//______________________________________________________________________________
Double_t TJetNet::GetInputTestSet( Int_t aPatternInd, Int_t aInputInd )
{
  // Returns the value of the cell corresponding to unit aInputInd in pattern aPatternInd into INPUT TEST set
  return m_pInputTestSet->GetData( aPatternInd, aInputInd );
}
//______________________________________________________________________________
Double_t TJetNet::GetOutputTestSet( Int_t aPatternInd, Int_t aOutputInd )
{
  // Returns the value of the cell corresponding to unit aInputInd in pattern aPatternInd into OUTPUT TEST set
  return m_pOutputTestSet->GetData( aPatternInd, aOutputInd );
}
//______________________________________________________________________________
void  TJetNet::SaveDataAscii( TString aFileName )
{
  // Saves the Input/Output test and train data in plain text file
  ofstream out;
  int i, j;

  // Open ASCII file
  out.open( aFileName );

  //Write the number of layers, including the input and output
  out << m_LayerCount << std::endl;

  // Write into the file the number of units in input, hidden and output layers  
  for ( i = 0; i < m_LayerCount; i++ ) out << m_pLayers[ i ] << " ";
  out << std::endl;

  // Write the size of Train and Test sets 
  out << m_TrainSetCnt << " " << m_TestSetCnt << std::endl;

  // Dump the Train set : Input1 Input2 ... InputN Output1 Output2 ... OutputN
  for ( i = 0; i < m_TrainSetCnt; i++ )
  {
    out << GetInputTrainSet( i, 0 );
    for( j = 1; j < m_pLayers[ 0 ]; j++ ) out << " " << GetInputTrainSet( i, j );
    for( j = 0; j < m_pLayers[ m_LayerCount - 1 ]; j++ ) out << " " << GetOutputTrainSet( i, j );
    out << std::endl;
  }

 // Dump the Test set : Input1 Input2 ... InputN Output1 Output2 ... OutputN
  for ( i = 0; i < m_TestSetCnt; i++ )
  {
    out << GetInputTestSet( i, 0 );
    for( j = 1; j < m_pLayers[ 0 ]; j++ ) out << " " << GetInputTestSet( i, j );
    for( j = 0; j < m_pLayers[ m_LayerCount - 1 ]; j++ ) out << " " << GetOutputTestSet( i, j );
    out << std::endl;
  }
  // Close the file
  out.close();
}
//______________________________________________________________________________
void  TJetNet::LoadDataAscii( TString aFileName )
{
  // Loads the input/output test/train data from plain text file 
  ifstream in;
  int i, j, k, l, m;
  int aiParam[ 5 ];//iTrainCount, iTestCount, iInputDim, iHiddenDim, iOutputDim;
  Bool_t bFlag;
  Double_t tmp;
  Int_t iPatternLength;

  in.open( aFileName );
  bFlag = Bool_t( in.is_open() );
  if ( in )
  { 
    in >> m_LayerCount;
    if( m_Debug ){ std::cout << "Layers Count Set to " << m_LayerCount << std::endl;}
    i = 0;

    delete [] m_pLayers;
    m_pLayers = new Int_t[ m_LayerCount ];

    if( m_Debug ){ std::cout << "Updating the Layers Nodes Counters..." << std::endl; }
    while( ( i < m_LayerCount ) && ( !in.eof() ) )
    {
      in >> m_pLayers[ i ];
      if( m_Debug ){ std::cout << "Layer [ " << i + 1 << " ] has " << m_pLayers[ i ] << " units" << std::endl; }
      i++;
    }

    m_InputDim = m_pLayers[ 0 ];
    m_OutputDim = m_pLayers[ m_LayerCount - 1 ];
    m_HiddenLayerDim = m_LayerCount-2;

    //Get the patterns count per line 
    iPatternLength = m_InputDim + m_OutputDim;
    if( m_Debug ){ std::cout << "Patterns per line = " << iPatternLength << std::endl; } 
    in >> m_TrainSetCnt;
    if( m_Debug ){ std::cout << "Train Set has " << m_TrainSetCnt << " patterns." << std::endl; }
    in >> m_TestSetCnt;
    if( m_Debug ){ std::cout << "Test Set has " << m_TestSetCnt << " patterns." << std::endl; }
    
    delete m_pInputTestSet;
    delete m_pInputTrainSet;
    delete m_pOutputTestSet;
    delete m_pOutputTrainSet;
    
    m_pInputTrainSet  = new TNeuralDataSet( m_TrainSetCnt, GetInputDim() );
    m_pInputTestSet   = new TNeuralDataSet( m_TestSetCnt, GetInputDim() );
    m_pOutputTrainSet = new TNeuralDataSet( m_TrainSetCnt, GetOutputDim() );
    m_pOutputTestSet  = new TNeuralDataSet( m_TestSetCnt, GetOutputDim() );

    i = 0;
    j = 0;
    
    while( ( i < ( m_TrainSetCnt + m_TestSetCnt ) ) && ( !in.eof() ) )
    {
      j = 0;
      while( ( j < iPatternLength ) && ( !in.eof() ) )
      {
	if( i < m_TrainSetCnt )
	{
	  if( j < m_InputDim )
	  {
	    //Train Input Set
	    in >> tmp;
	    SetInputTrainSet( i, j, tmp );
	  }
	  else 
	  {
	    //Train Output Set 
	    m = j - m_InputDim;
	    in >> tmp;
	    SetOutputTrainSet( i, m, tmp );
	  }
	}
	else
	{
	  l = i - m_TrainSetCnt;
	  if( j < m_InputDim )
	    {
	      //Test Input Set
	      in >> tmp;
	      SetInputTestSet( l, j, tmp );
	    }
          else
	    {
	      //Test Output Set
	      m = j - m_InputDim;
	      in >> tmp;
	      SetOutputTestSet( l, m, tmp );
	    }

	}    
	j++;
      }
      i++;
    }
  }
  in.close();
}
//______________________________________________________________________________
void  TJetNet::SaveDataRoot( TString aFileName )
{
  // Saves the neural network in ROOT file
}
//______________________________________________________________________________
void  TJetNet::LoadDataRoot( TString aFileName )
{
  // Loads the neural network from ROOT file
}
//______________________________________________________________________________
void TJetNet::Evaluate(  )
{
  //evaluates directly the input provided through SetInputs()
  JNTEST();
}
//______________________________________________________________________________
void TJetNet::Evaluate( Int_t aPattern  )
{
  // Evaluates the network output form the input data specified by the Test Pattern
  for( Int_t i = 0; i < GetInputDim(); i++ )
  {
	JNDAT1.OIN[ i ] = float ( GetInputTestSet( aPattern, i ) );
  }
  JNTEST();
}
//______________________________________________________________________________
void TJetNet::SetInputs( Int_t aIndex, Double_t aValue )
{
  // Directly sets the inputs of the network 
  JNDAT1.OIN[ aIndex ] = float ( aValue );
}
//______________________________________________________________________________
Double_t TJetNet::GetOutput( Int_t aIndex )
{
  // Returns the output of the network 
  return Double_t ( JNDAT1.OUT[ aIndex ] );
}
//______________________________________________________________________________
void TJetNet::DumpToFile( TString aFileName )
{
  // Dumps the network data into JETNET specific format
  JNDUMP( -8 );
  std::cout << close( 8 ) << std::endl;
  rename( "./fort.8", aFileName );
}
//______________________________________________________________________________
void TJetNet::ReadFromFile( TString aFileName )
{
  // Loads the network from JETNET specific file
  rename( aFileName, "./fort.12" );
  JNREAD( -12 );
  Reinitialize();
  rename( "./fort.12", aFileName );
  //std::cout << close( 12 ) << std::endl;
}
//______________________________________________________________________________
Double_t TJetNet::GetWeight( Int_t aLayerInd, Int_t aNodeInd, Int_t aConnectedNodeInd ) const
{
  // Returns the node weight in specific Layer
  return Double_t ( JNINT1.W[ JNINDX( aLayerInd, aNodeInd, aConnectedNodeInd )-1 ] );
  //GP: ONE HAS TO PAY ATTENTION TO THIS STUPID -1!!!
}
//______________________________________________________________________________
Double_t TJetNet::GetThreshold( Int_t aLayerInd, Int_t aNodeInd) const
{
  //Returns the node threshold in the specific layer
  return Double_t ( JNINT1.T[ JNINDX( aLayerInd, aNodeInd, 0 )-1 ] );
  //GP: ONE HAS TO PAY ATTENTION TO THIS STUPID -1!!!
}
//______________________________________________________________________________
void TJetNet::SelectiveFields( Int_t aLayerA, Int_t aNodeA1, Int_t aNodeA2, Int_t aNodeB1, Int_t aNodeB2, Int_t aSwitch )
{
  // JetNet Selective Fields
  Int_t tmp, i1, i2, j1, j2;

  if( ( aLayerA > 0 ) && ( aLayerA < m_LayerCount ) )
  {
    i1 = TMath::Abs( aNodeA1 ); 
    i2 = TMath::Abs( aNodeA2 );
    j1 = TMath::Abs( aNodeB1 ); 
    j2 = TMath::Abs( aNodeB2 );

    if( i1 > i2 )
    {
      tmp = i1;
      i1 = i2;
      i2 = i1;
    }//if

    if( i1 > i2 )
    {
      tmp = i1;
      i1 = i2;
      i2 = i1;
    }//if

    if( ( i1 < m_pLayers[ aLayerA ] ) && ( i2 < m_pLayers[ aLayerA ] ) && 
	( j1 < m_pLayers[ aLayerA - 1 ] ) && ( j2 < m_pLayers[ aLayerA - 1 ] ) )
    {
      JNSEFI( aLayerA, i1, i2, j1, j2, aSwitch );
    }//if
  } //if
}
//______________________________________________________________________________
void TJetNet::Reinitialize( void )
{
  //Initializes the settings of the network
    Int_t i;
    
   m_LayerCount = JNDAT1.MSTJN[ 0 ]; // Set the number of layers
   
   delete [] m_pLayers;
   m_pLayers = new Int_t[ m_LayerCount ];
   
  // Set the number of nodes for each layer
  for( i = 0; i < m_LayerCount; i++ )
  {
    m_pLayers[ i ] = JNDAT1.MSTJN[ 9 + i ]; 
  }
  
  m_pInputTrainSet  = new TNeuralDataSet( m_TrainSetCnt, GetInputDim() );
  m_pInputTestSet   = new TNeuralDataSet( m_TestSetCnt, GetInputDim() );
  m_pOutputTrainSet = new TNeuralDataSet( m_TrainSetCnt, GetOutputDim() );
  m_pOutputTestSet  = new TNeuralDataSet( m_TestSetCnt, GetOutputDim() );

  m_InputDim = m_pLayers[ 0 ];
  m_OutputDim = m_pLayers[ m_LayerCount - 1 ];
  m_HiddenLayerDim = m_LayerCount-2;


}  
//______________________________________________________________________________
void TJetNet::Normalize( void )
{
  // Normilizes Inputs (both test and train)
  m_pInputTrainSet->Normalize();
  m_pInputTestSet->Normalize();
}
//______________________________________________________________________________
void TJetNet::Randomize( void )
{
  // Randomizes Inputs and Outputs of both train and test sets
  m_pInputTrainSet->Randomize();
  m_pInputTestSet->Randomize();
  m_pOutputTrainSet->Randomize();
  m_pOutputTestSet->Randomize();
}
//______________________________________________________________________________
Int_t TJetNet::GetUnitCount( Int_t aLayer )
{ 
  // Returns the number of the units in specfic layer
  if( ( aLayer > -1 ) && ( aLayer < m_LayerCount ) ) 
    return JNDAT1.MSTJN[ 9 + aLayer ]; 
}
//______________________________________________________________________________
void TJetNet::SetUpdatesPerEpoch( Int_t aValue )
{ 
  // Sets the number of the updates per epoch
  JNDAT1.MSTJN[ 8 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetUpdatingProcedure( Int_t aValue )
{  
  // Set specific weights update function
  JNDAT1.MSTJN[ 4 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetErrorMeasure( Int_t aValue )
{  
  JNDAT1.MSTJN[ 3 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetActivationFunction( Int_t aValue )
{ 
  // Set the kind of activation function used
  JNDAT1.MSTJN[ 2 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetActivationFunction( Int_t aValue, Int_t layerN )//layer 0 is first hidden layer
{ 
  // Set the kind of activation function used
  JNDAT2.IGFN[ layerN ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetPatternsPerUpdate( Int_t aValue )
{ 
  JNDAT1.MSTJN[ 1 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetLearningRate( Double_t aValue )
{ 
  // Change the Learning Rate
  JNDAT1.PARJN[ 0 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetMomentum( Double_t aValue )
{ 
  JNDAT1.PARJN[ 1 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetInitialWeightsWidth( Double_t aValue )
{ 
  JNDAT1.PARJN[ 3 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetLearningRateDecrease( Double_t aValue )
{ 
  JNDAT1.PARJN[ 10 ] = aValue; 
  if( !m_InitLocked ) this->Init();
}
//______________________________________________________________________________
void TJetNet::SetPottsUnits(Int_t aValue)
{
  JNINT2.IPOTT = aValue; 
} 
//_____________________________________________________________________________
void TJetNet::NormalizeOutput(bool isTrue)
{
  m_NormalizeOutput=isTrue;
}
//______________________________________________________________________________
Int_t TJetNet::GetUpdatesPerEpoch( void )
{ 
  return JNDAT1.MSTJN[ 8 ]; 
}
//______________________________________________________________________________
Int_t TJetNet::GetUpdatingProcedure( void )
{  
  return JNDAT1.MSTJN[ 3 ]; 
}
//______________________________________________________________________________
Int_t TJetNet::GetErrorMeasure( void )
{ 
  return JNDAT1.MSTJN[ 3 ]; 
}
//______________________________________________________________________________
Int_t TJetNet::GetActivationFunction( void ) const
{ 
  return JNDAT1.MSTJN[ 2 ]; 
}
//______________________________________________________________________________

Int_t TJetNet::GetActivationFunction( Int_t layerN ) const
{ 
  return JNDAT2.IGFN[ layerN ];
}
//______________________________________________________________________________
Int_t TJetNet::GetPatternsPerUpdate( void )
{ 
  return JNDAT1.MSTJN[ 1 ]; 
}
//______________________________________________________________________________
Double_t TJetNet::GetLearningRate( void )
{ 
  return JNDAT1.PARJN[ 0 ]; 
}
//______________________________________________________________________________
Double_t TJetNet::GetMomentum( void )
{ 
  return JNDAT1.PARJN[ 1 ]; 
}
//______________________________________________________________________________
Double_t TJetNet::GetInitialWeightsWidth( void )
{ 
  return JNDAT1.PARJN[ 3 ]; 
}
//______________________________________________________________________________
Double_t TJetNet::GetLearningRateDecrease( void )
{ 
  return JNDAT1.PARJN[ 10 ]; 
}
//______________________________________________________________________________
Int_t TJetNet::GetPottsUnits()
{
  return JNINT2.IPOTT;
} 
//______________________________________________________________________________
Int_t TJetNet::GetMSTJN( Int_t aIndex )
{
  return JNDAT1.MSTJN[ aIndex ]; 
}
//______________________________________________________________________________
Double_t TJetNet::GetPARJN( Int_t aIndex )
{
  return JNDAT1.PARJN[ aIndex ];
}
//______________________________________________________________________________
void TJetNet::SetMSTJN( Int_t aIndex, Int_t aValue )
{
  JNDAT1.MSTJN[ aIndex ] = aValue;
}
//______________________________________________________________________________
void TJetNet::SetPARJN( Int_t aIndex, Double_t aValue )
{
  JNDAT1.PARJN[ aIndex ] = aValue;
}
//______________________________________________________________________________
void TJetNet::Shuffle( Bool_t aShuffleTrainSet, Bool_t aShuffleTestSet )
{
  // Shuffles the train and/or test input/output sets
  TTimeStamp ts;
  Int_t Seed = ts.GetSec();
  if ( aShuffleTrainSet )
  {
    
    m_pInputTrainSet->Shuffle( Seed );
    m_pOutputTrainSet->Shuffle( Seed );
  }
  //Shuffle Test Set
  if ( aShuffleTestSet )
  {
    Seed = ts.GetSec();
    m_pInputTestSet->Shuffle( Seed );
    m_pOutputTestSet->Shuffle( Seed );
  }

  return;
} 


//EOF

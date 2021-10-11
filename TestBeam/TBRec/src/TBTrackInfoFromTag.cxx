/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TBTrackInfoFromTag.h"


// template functions for copying the contents of a 1- and 2-dim array into a vector
template <class type1, class type2> void copyArrToVec_1d(unsigned int size, type1 arr, type2 *vec)
{
	(*vec).clear();
	std::copy ( arr, arr+size, std::back_inserter(*vec) );
	return;
}

template <class type1, class type2> void copyArrToVec_2d(unsigned int size1, unsigned int size2, type1 arr, type2 *vec)
{
	(*vec).clear();
	(*vec).resize(size1);
	for(unsigned int cntr=0; cntr<size1; cntr++ )
		copyArrToVec_1d( size2,arr[cntr],&((*vec)[cntr]) );

	return;
}



TBTrackInfoFromTag::TBTrackInfoFromTag(const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name,pSvcLocator)
 {
  // job options
   declareProperty("SGrecordkey",     m_SGrecordkey="TBTrackReducedInfo");
   declareProperty("EventInfoSGKey",  m_SGkey1="ByteStreamEventInfo");
   declareProperty("InputRootFile",   m_inputRootFileName="./cbnt_RecExTB_Combined.root");

   rootFile1 = 0;
   rootFile2 = 0;
   tbTree = 0;
   tileRecTree = 0;

   m_max_nPixelHits = 0;
   m_max_nSctHits = 0;
   m_max_nTrtHitsTRT = 0;
   m_max1_Trt_HLTRT = 0;
   m_max2_Trt_HLTRT = 0;
   m_max_btdc1 = 0;
   m_max_GainC = 0;
   m_max_MuBack = 0;
   m_max_TfitC = 0;
   m_max_EfitC = 0;
   m_max_Chi2C = 0;
   m_max1_SampleC = 0;
   m_max2_SampleC = 0;
 }

TBTrackInfoFromTag::~TBTrackInfoFromTag()
{ }

StatusCode
TBTrackInfoFromTag::initialize()
{
  ///////////////////////
  // Allocate Services //
  ///////////////////////

	// initialize variables which store the max lenghts of the arrays
	// from the input root file to be read by the TBTrackInfoFromTag package
	m_max_nPixelHits = m_max_nSctHits = m_max_nTrtHitsTRT = 60;

	m_max1_Trt_HLTRT = 60;
	m_max2_Trt_HLTRT = 100;

	m_max_btdc1 = 16;
	m_max_GainC = 48;

	m_max_MuBack = 14;
	m_max_TfitC = m_max_EfitC = m_max_Chi2C = 48;

	m_max1_SampleC = 48;
	m_max2_SampleC = 9;

	// input root trees
	TTree * tree;

	ATH_MSG_DEBUG("Reading in file " << m_inputRootFileName << "  ...");

	ATH_MSG_DEBUG("\t- Getting TileRec_h1000 tree "); 
	rootFile1 = new TFile((TString)m_inputRootFileName);
	rootFile1->cd((TString)m_inputRootFileName + ":/TileRec");
	tree = (TTree*)gDirectory->Get("h1000");
	tileRecTree = new TileRec_h1000(tree);

	ATH_MSG_DEBUG("\t- Getting TB_tree tree "); 
	rootFile2 = new TFile((TString)m_inputRootFileName);
	rootFile2->cd((TString)m_inputRootFileName + ":/TB");
	tree = (TTree*)gDirectory->Get("tree");
	tbTree = new TB_tree(tree);

        return StatusCode::SUCCESS;
}

StatusCode
TBTrackInfoFromTag::execute()
{
	ATH_MSG_DEBUG("Executing TBTrackInfoFromTag...");

	const EventInfo * evtInfo;
	ATH_CHECK( evtStore()->retrieve(evtInfo,m_SGkey1) );

	int runNumber = evtInfo->event_ID()->run_number();
	uint64_t evtNumber = evtInfo->event_ID()->event_number();

	ATH_MSG_DEBUG("Run/Event numbers:\t " << runNumber << " / " << evtNumber); 

/*
	int returnFlag = 0;
	int evtFindFlag1, evtFindFlag2;
	evtFindFlag1 = tileRecTree->Loop(evtNumber);
	evtFindFlag2 = tbTree->Loop(evtNumber);

	if(evtFindFlag1 < 0) {
		ATH_MSG_DEBUG("There is no event number " << evtNumber << " in tileRecTree");
		returnFlag = 1;
	}
	if(evtFindFlag2 < 0) {
		ATH_MSG_DEBUG("There is no event number " << evtNumber << " in tbTree");
		returnFlag = 1;
	}

	if(returnFlag == 1)
	  return sc;

	tileRecTree->fChain->GetEntry(evtFindFlag1);
	tbTree->fChain->GetEntry(evtFindFlag1);

	ATH_MSG_DEBUG("EventNumber in EventInfo   =  " << evtNumber);
	ATH_MSG_DEBUG("EventNumber in tbTree      =  " << tbTree->Event);
	ATH_MSG_DEBUG("EventNumber in tileRecTree =  " << tileRecTree->Evt);
*/


	int evtFindFlag = tileRecTree->Loop(evtNumber);
	if(evtFindFlag < 0) {
          ATH_MSG_DEBUG("There is no event number " << evtNumber << " in tileRecTree");
	  return StatusCode::SUCCESS;
	}

	ATH_MSG_DEBUG("Get line number " << evtFindFlag << " from the trees");
	tileRecTree->fChain->GetEntry(evtFindFlag);
	tbTree->fChain->GetEntry(evtFindFlag);

	ATH_MSG_DEBUG("EventNumber in EventInfo   =  " << evtNumber);
	ATH_MSG_DEBUG("EventNumber in tileRecTree =  " << tileRecTree->Evt);
	ATH_MSG_DEBUG("EventNumber in tbTree      =  " << tbTree->Event);

	TBTrackInfo * trckInfo = new TBTrackInfo();

	// variables from TB_tree
	trckInfo->tb_Trigger         =  tbTree->Trigger;
	trckInfo->tb_trk_nTracks     =  tbTree->trk_nTracks;
	trckInfo->tb_trk_nTracksTRT  =  tbTree->trk_nTracksTRT;

	trckInfo->tb_sADC_S1         =  tbTree->sADC_S1;
	trckInfo->tb_sADC_muHalo     =  tbTree->sADC_muHalo;
	trckInfo->tb_sADC_muTag      =  tbTree->sADC_muTag;
	trckInfo->tb_sADC_C2         =  tbTree->sADC_C2;
	trckInfo->tb_sADC_TRTSci     =  tbTree->sADC_TRTSci;

	copyArrToVec_1d( m_max_nPixelHits,  tbTree->trk_nPixelHits,  &(trckInfo->tb_trk_nPixelHits) );
	copyArrToVec_1d( m_max_nSctHits,    tbTree->trk_nSctHits,    &(trckInfo->tb_trk_nSctHits) );
	copyArrToVec_1d( m_max_nTrtHitsTRT, tbTree->trk_nTrtHitsTRT, &(trckInfo->tb_trk_nTrtHitsTRT) );

	copyArrToVec_2d( m_max1_Trt_HLTRT,  m_max2_Trt_HLTRT, tbTree->trk_Trt_HLTRT, &(trckInfo->tb_trk_Trt_HLTRT) );

/*
	ATH_MSG_DEBUG("trk_nPixelHits:");
	ATH_MSG_DEBUG("m_max_nPixelHits " << m_max_nPixelHits );
	for(int i=0; i<60; i++){
		ATH_MSG_DEBUG("\t- trckInfo->tb_trk_nPixelHits[i] " << i << " = "<< trckInfo->tb_trk_nPixelHits[i] );
		ATH_MSG_DEBUG("\t- tbTree->trk_nSctHits[i] " << i << " = " << tbTree->trk_nSctHits[i] );
	}

	ATH_MSG_DEBUG("2- tb_trk_Trt_HLTRT:");
	ATH_MSG_DEBUG("m_max1_Trt_HLTRT ,  m_max2_Trt_HLTRT = " << m_max1_Trt_HLTRT << " , " << m_max2_Trt_HLTRT);
	for(int i=0; i<60; i++){
		ATH_MSG_DEBUG("\t- "   << i << ": ");
		for(int j=0; j<5; j++){
			ATH_MSG_DEBUG("\t\t- tbTree->tb_trk_Trt_HLTRT[i][j]   : " << j << " = " << tbTree->trk_Trt_HLTRT[i][j] );
			ATH_MSG_DEBUG("\t\t- trckInfo->tb_trk_Trt_HLTRT[i][j] : " << j << " = " << trckInfo->tb_trk_Trt_HLTRT[i][j] );
			ATH_MSG_DEBUG("");
		}
	}

*/

	// variables from TileRec_h1000
	trckInfo->tileRec_XchN1  =  tileRecTree->XchN1 ;
	trckInfo->tileRec_XchN2  =  tileRecTree->XchN2 ;
	trckInfo->tileRec_Xcha0  =  tileRecTree->Xcha0 ;
	trckInfo->tileRec_Xcha1  =  tileRecTree->Xcha1 ;

	copyArrToVec_1d( m_max_btdc1,  tileRecTree->btdc1,  &(trckInfo->tileRec_btdc1) );

	copyArrToVec_1d( m_max_GainC,  tileRecTree->GainC0,  &(trckInfo->tileRec_GainC0) );
	copyArrToVec_1d( m_max_GainC,  tileRecTree->GainC1,  &(trckInfo->tileRec_GainC1) );
	copyArrToVec_1d( m_max_GainC,  tileRecTree->GainC2,  &(trckInfo->tileRec_GainC2) );

	copyArrToVec_1d( m_max_MuBack, tileRecTree->MuBack,  &(trckInfo->tileRec_MuBack) );

	copyArrToVec_1d( m_max_TfitC,  tileRecTree->TfitC0,  &(trckInfo->tileRec_TfitC0) );
	copyArrToVec_1d( m_max_TfitC,  tileRecTree->TfitC1,  &(trckInfo->tileRec_TfitC1) );
	copyArrToVec_1d( m_max_TfitC,  tileRecTree->TfitC2,  &(trckInfo->tileRec_TfitC2) );

	copyArrToVec_1d( m_max_EfitC,  tileRecTree->EfitC0,  &(trckInfo->tileRec_EfitC0) );
	copyArrToVec_1d( m_max_EfitC,  tileRecTree->EfitC1,  &(trckInfo->tileRec_EfitC1) );
	copyArrToVec_1d( m_max_EfitC,  tileRecTree->EfitC2,  &(trckInfo->tileRec_EfitC2) );

	copyArrToVec_1d( m_max_Chi2C,  tileRecTree->Chi2C0,  &(trckInfo->tileRec_Chi2C0) );
	copyArrToVec_1d( m_max_Chi2C,  tileRecTree->Chi2C1,  &(trckInfo->tileRec_Chi2C1) );
	copyArrToVec_1d( m_max_Chi2C,  tileRecTree->Chi2C2,  &(trckInfo->tileRec_Chi2C2) );

	copyArrToVec_2d( m_max1_SampleC,  m_max2_SampleC,  tileRecTree->SampleC0,  &(trckInfo->tileRec_SampleC0) );
	copyArrToVec_2d( m_max1_SampleC,  m_max2_SampleC,  tileRecTree->SampleC1,  &(trckInfo->tileRec_SampleC1) );
	copyArrToVec_2d( m_max1_SampleC,  m_max2_SampleC,  tileRecTree->SampleC2,  &(trckInfo->tileRec_SampleC2) );


        ATH_CHECK( evtStore()->record(trckInfo,m_SGrecordkey) );
        ATH_MSG_DEBUG("\t- Recording TBTrackInfo in StorGate with key = " << m_SGrecordkey); 


        return StatusCode::SUCCESS;
}


StatusCode 
TBTrackInfoFromTag::finalize()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////

	delete rootFile1;
	delete rootFile2;
	delete tileRecTree;
	delete tbTree;

  return StatusCode::SUCCESS;
}

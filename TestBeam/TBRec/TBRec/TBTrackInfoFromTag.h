/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBTrackInfoFromTag_H
#define TBTrackInfoFromTag_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MWPC Reconstruction algorithm. Based on code for 2002 TestBeam            //
// (see LArCnv/LArHECTBCnv/hectb/Alg_mwpc.cxx)                               //
//  author : Pierre-Antoine Delsart                                          //
///////////////////////////////////////////////////////////////////////////////

// my cout color definitios
#define coutDefault		"\033[0m"
#define coutRed			"\033[31m"
#define coutGreen		"\033[32m"
#define coutBlue		"\033[34m"
#define coutPurple		"\033[35m"
#define coutUnderLine		"\033[4;30m"
#define coutWhiteOnBlack	"\33[40;37;1m"
#define MSG_DEBUG_RED(string)						log << MSG::DEBUG << coutRed << string << coutDefault << endreq;
#define MSG_DEBUG_GREEN(string)					log << MSG::DEBUG << coutGreen << string << coutDefault << endreq;
#define MSG_DEBUG_BLUE(string)					log << MSG::DEBUG << coutBlue << string << coutDefault << endreq;
#define MSG_DEBUG_PURPLE(string)				log << MSG::DEBUG << coutPurple << string << coutDefault << endreq;
#define MSG_DEBUG_WHITEONBLACK(string)	log << MSG::DEBUG << coutWhiteOnBlack << string << coutDefault << endreq;
#define MSG_ERROR_WHITEONBLACK(string)	log << MSG::ERROR << coutWhiteOnBlack << string << coutDefault << endreq;


#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

#include "EventInfo/EventInfo.h"
#include "TBEvent/TBTrackInfo.h"
#include "EventInfo/EventID.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

class StoreGateSvc;
class TROOT;
class TChain;
class TFile;
class TString;

#include "TBRec/TileRec_h1000.h"
#include "TBRec/TB_tree.h"


class TBTrackInfoFromTag : public AthAlgorithm
{



 public:

  TBTrackInfoFromTag(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBTrackInfoFromTag();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ////////////////
  // Properties //
  ////////////////

	
	std::string m_SGrecordkey, m_SGkey1;
	std::string m_inputRootFileName;

	unsigned int		m_max_nPixelHits, m_max_nSctHits, m_max_nTrtHitsTRT;
	unsigned int		m_max1_Trt_HLTRT, m_max2_Trt_HLTRT;
	unsigned int		m_max_btdc1, m_max_GainC;
	unsigned int		m_max_MuBack, m_max_TfitC, m_max_EfitC, m_max_Chi2C;
	unsigned int		m_max1_SampleC, m_max2_SampleC;

	TFile * rootFile1, * rootFile2;
	TileRec_h1000 * tileRecTree;
	TB_tree * tbTree;

};

CLASS_DEF( TBTrackInfoFromTag , 33008888 , 1 )


#endif

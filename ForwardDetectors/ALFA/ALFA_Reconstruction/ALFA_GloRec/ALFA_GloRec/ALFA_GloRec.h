/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRec_h
#define ALFA_GloRec_h


#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
//#include "typeinfo.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Kernel.h"
//#include "GaudiKernel/PropertyCallbackFunctor.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"


#include "ALFA_RawEv/ALFA_RawData.h"
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_RawEv/ALFA_ODDigitCollection.h"
#include "ALFA_GloRecEv/ALFA_GloRecEvent.h"
#include "ALFA_GloRecEv/ALFA_GloRecEvCollection.h"
#include "ALFA_GloRecEv/ALFA_GloRecODEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"
#include "ALFA_Geometry/ALFA_constants.h"

#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TString.h"
#include "TLine.h"
#include "TPaveStats.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH3F.h"
#include "THStack.h"
#include "TMath.h"
#include "TF1.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TApplication.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "ALFA_GloRec/AlfaLocalHits.h"

class StoreGateSvc;
class ActiveStoreSvc;


/////////////////////////////////////////////////////////
//
//       ALFA_GloRec class declaration
//
//////////////////////////////////////////////////////////
class ALFA_GloRec : public AthAlgorithm
{
public:
	ALFA_GloRec (const std::string& name, ISvcLocator* pSvcLocator);
	~ALFA_GloRec();

private:

    ////////////////////////////
	// a handle on Store Gate
    ////////////////////////////
	StoreGateSvc* m_storeGate;


    ////////////////////////////////
    //  general members
    ////////////////////////////////
    UInt_t m_nEvent;
	ALFA_GloRecEvCollection* m_pGloRecEvCollection;


    //////////////////////////////////////////////
    //  steerable members
    //////////////////////////////////////////////
    int m_iDataType;            
    std::string m_strTrackPathPatterns;
    TObjArray m_TObjArrTrackPathPatterns;
    std::string m_strGloRecAnalysisFile;
    std::string m_strGloRecCollectionName;   
    std::string m_strLocRecCorrCollectionName;
    std::string m_TruthCollectionName;   
        

    //////////////////////////////////////////////
    //  methods 
    //////////////////////////////////////////////
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    StatusCode Truth_info();
    void InitHistos();
    void FillTrackCandHistos(AlfaTrackCand * trkcand);
    void WriteHistos();
 

    /////////////////////////////////////////
    // generated vertices and momenta
    /////////////////////////////////////////
    double m_px_g_pos;
    double m_py_g_pos;
    double m_pz_g_pos;
    double m_x_g_pos;
    double m_y_g_pos;
    double m_z_g_pos;

    double m_px_g_neg;
    double m_py_g_neg;
    double m_pz_g_neg;
    double m_x_g_neg;
    double m_y_g_neg;
    double m_z_g_neg;

    ////////////////////////////////////////
    //  histograms
    ////////////////////////////////////////
    TH1F * m_th1_x_g;
    TH1F * m_th1_y_g;
    TH1F * m_th1_xslope_g;
    TH1F * m_th1_yslope_g;

	TH1F * m_th1_xnearuppotresiduals;
    TH1F * m_th1_ynearuppotresiduals;
  	TH1F * m_th1_xfaruppotresiduals;
    TH1F * m_th1_yfaruppotresiduals;
    TH1F * m_th1_xnearlwpotresiduals;
    TH1F * m_th1_ynearlwpotresiduals;
  	TH1F * m_th1_xfarlwpotresiduals;
    TH1F * m_th1_yfarlwpotresiduals;

    TH2F * m_th2_truexvsrecx;
    TH2F * m_th2_trueyvsrecy;
    TH2F * m_th2_truexslopevsrecxslope;
    TH2F * m_th2_trueyslopevsrecyslope;
    TH1F * m_th1_recxovertruex;
    TH1F * m_th1_recyovertruey;
    TH1F * m_th1_recxslopeovertruexslope;
    TH1F * m_th1_recyslopeovertrueyslope;
    TH1F * m_th1_recxminustruex;
    TH1F * m_th1_recyminustruey;
    TH1F * m_th1_recxslopeminustruexslope;
    TH1F * m_th1_recyslopeminustrueyslope;     

    TH2F * m_th2_extrapxvsrecxnearpot;   
    TH2F * m_th2_extrapyvsrecynearpot;
    TH1F * m_th1_recxoverextrapxnearpot;
    TH1F * m_th1_recyoverextrapynearpot;
    TH1F * m_th1_recxminusextrapxnearpot;
    TH1F * m_th1_recyminusextrapynearpot;

    TH2F * m_th2_extrapxvsrecxfarpot;   
    TH2F * m_th2_extrapyvsrecyfarpot;
    TH1F * m_th1_recxoverextrapxfarpot;
    TH1F * m_th1_recyoverextrapyfarpot;
    TH1F * m_th1_recxminusextrapxfarpot;
    TH1F * m_th1_recyminusextrapyfarpot;

};

#endif // ALFA_GloRec_h

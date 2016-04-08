/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
/// Filename : TileTBNtuple.h
/// Author   : Paulo da Silva
/// Created  : Oct, 2003
///
/// DESCRIPTION
/// 
///    To create TB beam Ntuple file from Digits container
///
/// Properties (JobOption Parameters):
///
///    TileDigitsContainer     string   key value of Digits in TDS 
///    TileBeamElemContainer   string   key of BeamElems in TDS
///    TileRawChannelContainerFlat      key of flat filtered RawChannels in TDS
///    TileRawChannelContainerFit       key of fit filtered RawChannels in TDS
///    CalibrateEnergy         bool     If calibration should be applied to energy
///    CalibMode               bool     If data is in calibration mode
///    NtupleLoc               string   pathname of ntuple file
///    NtupleID                int      ID of ntuple
///    BC1X1                   float    TDC Conv param
///    BC1X2                   float    TDC Conv param
///    BC1Y1                   float    TDC Conv param
///    BC1Y2                   float    TDC Conv param
///    BC1Z                    float    TDC Conv param
///    BC2X1                   float    TDC Conv param
///    BC2X2                   float    TDC Conv param
///    BC2Y1                   float    TDC Conv param
///    BC2Y2                   float    TDC Conv param
///    BC2Z                    float    TDC Conv param
///
/// BUGS:
///  
/// History:
///  
//  
//****************************************************************************
#ifndef TileTBNtuple_H
#define TileTBNtuple_H

// Gaudi includes
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TileConditions/TileCablingService.h"

class TileID;
class TileHWID;
class TileInfo;
class TileRawChannelBuilderFlatFilter;
class TileBeamInfoProvider;
class TileBeamElemContByteStreamCnv;
class TileCondToolEmscale;

#include "TTree.h"
#include <string>
#include <vector>
#include <map>

/* @class TileTBNtuple
 @brief Osolete Class to create Ntuples. Use TileTBAANtuple for rel> 13.0.10
 */

class TileTBNtuple: public AthAlgorithm {
  public:
    //Constructor
    TileTBNtuple(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileTBNtuple();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    StatusCode storeRawChannels(std::string cntID
                                , std::vector<NTuple::Array<float>*> *
                                , std::vector<NTuple::Array<float>*> *
                                , std::vector<NTuple::Array<float>*> *
                                , std::vector<NTuple::Array<float>*> *);

    StatusCode storeDigits();
    StatusCode storeBeamElements();
    StatusCode storeCells();

    StatusCode initList(void);
    StatusCode initNTuple(void);
    StatusCode connectFile(void);

    void getEta(void);

    inline int digiChannel2PMT(int fragType, int chan) {
      return (abs(m_cabling->channel2hole(fragType, chan)) - 1);
    }

    // If data should be put in calib mode
    bool m_calibMode;

    // The ntuple
    NTuple::Tuple* m_ntuplePtr;
    bool m_ntupleCreated;

    // event number
    int m_evtNr;

    // Triger items
    NTuple::Item<int> m_evTime;
    NTuple::Item<int> m_run;
    NTuple::Item<int> m_evt;
    NTuple::Item<int> m_trigType;

    // 0 - Beam, 1 neg eta, 2 pos eta
    NTuple::Array<int> m_l1ID;
    NTuple::Array<int> m_l1Type;
    NTuple::Array<int> m_evBCID;
    NTuple::Array<int> m_evType;

    NTuple::Array<int> m_frBCID;

    // Muon items
    NTuple::Item<float> m_muBackHit;
    NTuple::Item<float> m_muBackSum;
    NTuple::Array<float> m_muBack; // MUON/MuBack
    NTuple::Array<float> m_muCalib; // MUON/MuCalib

    // Ecal
    NTuple::Array<float> m_ecal;

    // laser items
    NTuple::Item<int> m_lasFlag;
    NTuple::Item<float> m_las0;
    NTuple::Item<float> m_las1;
    NTuple::Item<float> m_las2;
    NTuple::Item<float> m_las3;
    NTuple::Array<float> m_lasExtra;

    // pattern Unit in common beam crate
    NTuple::Item<int> m_commonPU;

    // Adder items
    NTuple::Matrix<int> m_adder;
    //NTuple::Array<int> m_addx;
    NTuple::Array<float> m_eneAdd;
    NTuple::Array<float> m_timeAdd;

    // status
    // NTuple::Matrix<float> m_status;

    // Cispar
    NTuple::Array<int> m_cispar;

    // TDC/BEAM Items
    NTuple::Item<int> m_s1cou;
    NTuple::Item<int> m_s2cou;
    NTuple::Item<int> m_s3cou;
    NTuple::Item<int> m_cher1;
    NTuple::Item<int> m_cher2;
    NTuple::Item<int> m_muTag;
    NTuple::Item<int> m_muHalo;
    NTuple::Item<int> m_muVeto;

    NTuple::Item<int> m_s2extra;
    NTuple::Item<int> m_s3extra;

    NTuple::Item<int> m_sc1;
    NTuple::Item<int> m_sc2;

    NTuple::Array<int> m_tof;
    NTuple::Array<int> m_btdc1;
    NTuple::Array<int> m_btdc2;

    NTuple::Item<float> m_xChN2;
    NTuple::Item<float> m_yChN2;
    NTuple::Item<float> m_xChN1;
    NTuple::Item<float> m_yChN1;
    NTuple::Item<float> m_xCha0;
    NTuple::Item<float> m_yCha0;

    NTuple::Item<float> m_xCha1;
    NTuple::Item<float> m_yCha1;
    NTuple::Item<float> m_xCha2;
    NTuple::Item<float> m_yCha2;
    NTuple::Item<float> m_xImp;
    NTuple::Item<float> m_yImp;

    // Digi/Energy items
    std::vector<NTuple::Item<int>*> m_evtVec;
    std::vector<NTuple::Array<int>*> m_bcidVec;
    std::vector<NTuple::Item<int>*> m_rodBCIDVec;
    std::vector<NTuple::Item<int>*> m_sizeVec;
    std::vector<NTuple::Array<int>*> m_dmuMaskVec;
    std::vector<NTuple::Array<int>*> m_slinkCRCVec;
    std::vector<NTuple::Array<int>*> m_gainVec;
    std::vector<NTuple::Array<int>*> m_errVec;
    std::vector<NTuple::Matrix<int>*> m_sampleVec;
    std::vector<NTuple::Array<int>*> m_feCRCVec; //we use shorts, because vector<bool> is bugged
    std::vector<NTuple::Array<int>*> m_rodCRCVec;

    std::vector<NTuple::Array<float>*> m_eneVec;
    std::vector<NTuple::Array<float>*> m_timeVec;
    std::vector<NTuple::Array<float>*> m_efitVec;
    std::vector<NTuple::Array<float>*> m_tfitVec;
    std::vector<NTuple::Array<float>*> m_pedfitVec;
    std::vector<NTuple::Array<float>*> m_chi2Vec;
    std::vector<NTuple::Array<float>*> m_eOptVec;
    std::vector<NTuple::Array<float>*> m_tOptVec;
    std::vector<NTuple::Array<float>*> m_pedOptVec;
    std::vector<NTuple::Array<float>*> m_chi2OptVec;
    std::vector<NTuple::Array<float>*> m_eDspVec;
    std::vector<NTuple::Array<float>*> m_tDspVec;
    std::vector<NTuple::Array<float>*> m_chi2DspVec;

    NTuple::Array<float> m_LarEne;
    NTuple::Array<float> m_BarEne;
    NTuple::Array<float> m_ExtEne;
    NTuple::Array<float> m_GapEne;

    NTuple::Array<int> m_coincTrig1;
    NTuple::Array<int> m_coincTrig2;
    NTuple::Array<int> m_coincTrig3;
    NTuple::Array<int> m_coincTrig4;
    NTuple::Array<int> m_coincTrig5;
    NTuple::Array<int> m_coincTrig6;
    NTuple::Array<int> m_coincTrig7;
    NTuple::Array<int> m_coincTrig8;

    NTuple::Item<int> m_coincFlag1;
    NTuple::Item<int> m_coincFlag2;
    NTuple::Item<int> m_coincFlag3;
    NTuple::Item<int> m_coincFlag4;
    NTuple::Item<int> m_coincFlag5;
    NTuple::Item<int> m_coincFlag6;
    NTuple::Item<int> m_coincFlag7;
    NTuple::Item<int> m_coincFlag8;

    std::vector<std::string> m_rosName; // name of arrays in ntuple for different ROSes
    std::vector<std::string> m_drawerList; // list of frag IDs in correct order
    std::vector<int> m_drawerType; // type of every drawer 1-4: B+, B-, EB+, EB-
    std::map<unsigned int, unsigned int, std::less<unsigned int> > m_drawerMap; // map for frag IDs -> index

    typedef std::map<unsigned int, unsigned int, std::less<unsigned int> >::iterator drawerMap_iterator;

    std::vector<std::string> m_beamFragList; // list of beam frag IDs to store in the ntuple
    bool m_beamIdList[32]; // list of beam frag IDs to store in the ntuple

    // Params for Beam TDC
    // Beam chambers -1 and -2
    float m_beamBN2X1;
    float m_beamBN2X2;
    float m_beamBN2Y1;
    float m_beamBN2Y2;
    float m_beamBN2Z;

    float m_beamBN1X1;
    float m_beamBN1X2;
    float m_beamBN1Y1;
    float m_beamBN1Y2;
    float m_beamBN1Z;

    // Beam chamber 0
    float m_beamBC0X1;
    float m_beamBC0X2;
    float m_beamBC0Y1;
    float m_beamBC0Y2;
    float m_beamBC0Z;

    // Beam chambers 1 and 2
    float m_beamBC1X1;
    float m_beamBC1X2;
    float m_beamBC1Y1;
    float m_beamBC1Y2;
    float m_beamBC1Z;

    float m_beamBC2X1;
    float m_beamBC2X2;
    float m_beamBC2Y1;
    float m_beamBC2Y2;
    float m_beamBC2Z;
    float m_beamBC2Zperiod1;
    float m_beamBC2Zperiod2;

    //run number
    int m_runNumber;
    float m_eta;
    float m_theta;
    std::string m_etaFileName;

    //inner radius of calo
    float m_radius;

    // Container Parameters
    std::string m_digitsContainer;
    std::string m_beamElemContainer;
    std::string m_flatRawChannelContainer;
    std::string m_fitRawChannelContainer;
    std::string m_fitcRawChannelContainer;
    std::string m_optRawChannelContainer;
    std::string m_dspRawChannelContainer;
    std::string m_laserObject;

    bool m_calibrateEnergyThisEvent;
    bool m_calibrateEnergy;
    bool m_useDspUnits;
    bool m_unpackAdder;
    bool m_completeNtuple;
    bool m_commitNtuple;
    bool m_bsInput;
    int m_finalUnit;

    // NTuple parameters
    std::string m_ntupleID;
    std::string m_ntupleLoc;
    Long64_t m_treeSize;
    int m_nSamples;
    unsigned int m_nDrawers;

    ToolHandle<TileRawChannelBuilderFlatFilter> m_adderFilterAlgTool;

    // Identifiers
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;

    const TileCablingService* m_cabling;

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool

    ToolHandle<TileBeamInfoProvider> m_beamInfo;

    TileBeamElemContByteStreamCnv* m_beamCnv;

    std::string m_currentNtupleLoc;
    int m_currentFileNum;
    int m_eventsPerFile;

    std::string m_streamName;
};

#endif

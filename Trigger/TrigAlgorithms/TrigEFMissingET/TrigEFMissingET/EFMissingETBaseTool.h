/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETBASETOOL_H
#define TRIGEFMISSINGET_EFMISSINGETBASETOOL_H

/********************************************************************

NAME:     EFMissingETBaseTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Rashid Djilkibaev (updated by Diego Casadei)
CREATED:  March 1, 2006     (March 2008)

PURPOSE:  Base Tool for all EFMissingET tools.  The derived
tools must implement the execute(...) method.

 ********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigEFMissingET/EFMissingETHelper.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include <vector>

class TrigMissingET;

struct FexType {
        enum Type {
          OTHER         = -1,
          TOPO          =  0,
          CELL          =  1,
          FEB           =  2,
          JET           =  3
         };
};

static const InterfaceID IID_EFMissingETBaseTool("EFMissingETBaseTool", 1, 0);

class EFMissingETBaseTool : public AthAlgTool
{
  public:
    /** constructor */
    EFMissingETBaseTool(const std::string& type,
        const std::string& name,
        const IInterface* parent);
    /** destructor */
    ~EFMissingETBaseTool();

    virtual StatusCode start();

    virtual StatusCode initialize();    //!< not used
    virtual StatusCode execute();       //!< not used
    virtual StatusCode finalize();      //!< not used

    virtual StatusCode execute(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const xAOD::CaloClusterContainer *caloCluster,
                               const xAOD::JetContainer *jets,
                               const xAOD::TrackParticleContainer *track,
                               const xAOD::VertexContainer *vertex,
                               const xAOD::MuonContainer *muon);

    /** save time for detector iDet at step iStep in timerTime */
    StatusCode getTime(int iDet, int iStep, float *timerTime);
    StatusCode getTimeEM(float *timerTime);   //!< save total time for EM det in timerTime
    StatusCode getTimeHEC(float *timerTime);  //!< save total time for HEC det in timerTim
    StatusCode getTimeTile(float *timerTime); //!< save total time for Tile det in timerTim
    StatusCode getTimeFCal(float *timerTime); //!< save total time for FCAL det in timerTim
    StatusCode getTimeRegionSelector(float *timerTime);  //!< total time spent by RegionSelector
    StatusCode getTimeLoadCollections(float *timerTime); //!< total time spent by LoadCollections
    StatusCode getTimeLoop(float *timerTime);            //!< total time spent looping on all channels

    static const InterfaceID& interfaceID() { return IID_EFMissingETBaseTool; }

    int getFexType() { return m_fextype; }

  protected:

    int m_fextype;    			  //!< Fex type

    ITrigTimerSvc* m_timersvc;    //!< Timer service
    TrigTimer*     m_timer[4][3]; //!< (EM, HEC, Tile, FCAL) x (RegionSelector, LoadCollections, loop)
    TrigTimer*     m_glob_timer;  //!< total time
    std::string    m_FexName;     //!< name of the parent Fex

    /** definition of the meaning for the component flag bits
     ** (see the constructor)
     **/
    unsigned short m_maskProcessing;     //!< Component is being processed
    unsigned short m_maskErrBSconv;      //!< Error detected in ByteStream conversion
    unsigned short m_maskErrMuon;        //!< Unexpected no. of input muons
    unsigned short m_maskErrFEB;         //!< Error detected in a FEB header
    unsigned short m_maskSkipped;        //!< Component has been skipped
    unsigned short m_maskCompBigMEtSEtRatio; //!< large value for |MET/SumET|
    unsigned short m_maskBadCompEnergy;  //!< Component energy too large (negative or positive)
    unsigned short m_maskBadEnergyRatio; //!< Bad component energy ratio

    unsigned short m_maskBadCellQuality;   //!< Bad cell quality detected
    unsigned short m_maskBadCellEnergy;    //!< Bad cell energy
    unsigned short m_maskBadCellTime;      //!< Bad cell time
    unsigned short m_maskNoisyEnergyRatio; //!< Bad value for BadSET/SET ratio
    unsigned short m_maskNoMuonTrack;      //!< Got muon without associated track

    unsigned short m_maskProcessed;        //!< Component has been processed
    unsigned short m_maskCompErrors;       //!< Error detected

    /** definition of the meaning for the global flag highest 16 bits
     ** (see the constructor)
     **/
    //    unsigned m_maskEMB_A_Missing;          //!< EMB_A absent in DetMask
    //    unsigned m_maskEMB_C_Missing;          //!< EMB_C absent in DetMask
    //    unsigned m_maskEME_A_Missing;          //!< EME_A absent in DetMask
    //    unsigned m_maskEME_C_Missing;          //!< EME_C absent in DetMask
    //    unsigned m_maskHEC_A_Missing;          //!< HEC_A absent in DetMask
    //    unsigned m_maskHEC_C_Missing;          //!< HEC_C absent in DetMask
    //    unsigned m_maskFCAL_A_Missing;         //!< FCAL_A absent in DetMask
    //    unsigned m_maskFCAL_C_Missing;         //!< FCAL_C absent in DetMask
    //    unsigned m_maskTileB_A_Missing;        //!< TileB_A absent in DetMask
    //    unsigned m_maskTileB_C_Missing;        //!< TileB_C absent in DetMask
    //    unsigned m_maskTileE_A_Missing;        //!< TileE_A absent in DetMask
    //    unsigned m_maskTileE_C_Missing;        //!< TileE_C absent in DetMask

    unsigned m_maskGlobNoisyEnergyRatio;   //!< Bad value for BadSET/SET ratio
    unsigned m_maskBadEMfraction;          //!< Bad value for EM/tot SumE ratio
    unsigned m_maskGlobBigMEtSEtRatio;     //!< large value for |MET/SumET|
    unsigned m_maskObjInCrack;             //!< High energy in crack scintillators
    unsigned m_maskGlobErrors;             //!< Error detected
};


#endif // TRIGEFMISSINGET_EFMISSINGETBASETOOL

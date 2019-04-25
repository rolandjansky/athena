/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEFMISSINGET_IMISSINGETTOOL_H
#define TRIGEFMISSINGET_IMISSINGETTOOL_H 1

/********************************************************************

NAME:     IMissingETTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET
AUTHOR:   Gabriel Gallardo
CREATED:  September 2018

PURPOSE:  Interface for MET trigger tools in AthenaMT

 ********************************************************************/


#include "GaudiKernel/IAlgTool.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"

#include "TrigEFMissingET/EFMissingETHelper.h"
#include "xAODTrigMissingET/TrigMissingET.h"

/**
 * @class Adds info the the missing ET object
 * @brief Specific implementations may use various input data types
 **/

class IMissingETTool : virtual public ::IAlgTool { 
  
public: 
  DeclareInterfaceID( IMissingETTool, 1, 0 );  
  virtual ~IMissingETTool(){}

  /**
   * a method to update the met object (and met helper object)
   **/
  virtual StatusCode update( xAOD::TrigMissingET *met,
			     TrigEFMissingEtHelper *metHelper,
           const EventContext& ctx ) const = 0;

protected:

  int m_fextype;            //!< Fex type

  ITrigTimerSvc* m_timersvc;    //!< Timer service
  TrigTimer*     m_timer[4][3]; //!< (EM, HEC, Tile, FCAL) x (RegionSelector, LoadCollections, loop)
  TrigTimer*     m_glob_timer;  //!< total time
  std::string    m_FexName;     //!< name of the parent Fex

  
 /** definition of the meaning for the component flag bits
  **/
 static const unsigned short m_maskProcessing         = 0x0001 ; // bit  0 Component is being processed
 static const unsigned short m_maskErrBSconv          = 0x0002 ; // bit  1 Error detected in ByteStream conversion
 static const unsigned short m_maskErrMuon            = 0x0004 ; // bit  2 Unexpected no. of input muons
 static const unsigned short m_maskErrFEB             = 0x0008 ; // bit  3 Error detected in a FEB header
 static const unsigned short m_maskSkipped            = 0x0010 ; // bit  4 Component has been skipped
 static const unsigned short m_maskCompBigMEtSEtRatio = 0x0020 ; // bit  5 large value for |MET/SumET|
 static const unsigned short m_maskBadCompEnergy      = 0x0040 ; // bit  6 Component energy too large (negative or positive)
 static const unsigned short m_maskBadEnergyRatio     = 0x0080 ; // bit  7 Bad component energy ratio

 static const unsigned short m_maskBadCellQuality     = 0x0100 ; // bit  8 Bad cell quality detected
 static const unsigned short m_maskBadCellEnergy      = 0x0200 ; // bit  9 Bad cell energy
 static const unsigned short m_maskBadCellTime        = 0x0400 ; // bit 10 Bad cell time
 static const unsigned short m_maskNoisyEnergyRatio   = 0x0800 ; // bit 11 Bad value for BadSET/SET ratio
 static const unsigned short m_maskNoMuonTrack        = 0x1000 ; // bit 12 Got muon without associated track
 // static const unsigned short m_spare               = 0x2000 ; // bit 13 Spare
 static const unsigned short m_maskProcessed          = 0x4000 ; // bit 14 Component has been processed
 static const unsigned short m_maskCompErrors         = 0x8000 ; // bit 15 Error detected

 /** definition of the meaning for the global flag highest 16 bits
  ** (see the constructor)
  **/
 //    static const unsigned m_maskEMB_A_Missing;          //!< EMB_A absent in DetMask
 //    static const unsigned m_maskEMB_C_Missing;          //!< EMB_C absent in DetMask
 //    static const unsigned m_maskEME_A_Missing;          //!< EME_A absent in DetMask
 //    static const unsigned m_maskEME_C_Missing;          //!< EME_C absent in DetMask
 //    static const unsigned m_maskHEC_A_Missing;          //!< HEC_A absent in DetMask
 //    static const unsigned m_maskHEC_C_Missing;          //!< HEC_C absent in DetMask
 //    static const unsigned m_maskFCAL_A_Missing;         //!< FCAL_A absent in DetMask
 //    static const unsigned m_maskFCAL_C_Missing;         //!< FCAL_C absent in DetMask
 //    static const unsigned m_maskTileB_A_Missing;        //!< TileB_A absent in DetMask
 //    static const unsigned m_maskTileB_C_Missing;        //!< TileB_C absent in DetMask
 //    static const unsigned m_maskTileE_A_Missing;        //!< TileE_A absent in DetMask
 //    static const unsigned m_maskTileE_C_Missing;        //!< TileE_C absent in DetMask

 /** definition of the meaning for the global flag bits (highest ones only) **/
 // spare                    = 0x00010000; // bit 16
 // spare                    = 0x00020000; // bit 17
 // spare                    = 0x00040000; // bit 18
 // spare                    = 0x00080000; // bit 19
 // spare                    = 0x00100000; // bit 20
 // spare                    = 0x00200000; // bit 21
 // spare                    = 0x00400000; // bit 22
 // spare                    = 0x00800000; // bit 23
 // spare                    = 0x01000000; // bit 24
 // spare                    = 0x02000000; // bit 25
 // spare                    = 0x04000000; // bit 26

 static const unsigned m_maskGlobNoisyEnergyRatio = 0x08000000; // bit 27 Bad value for BadSET/SET ratio
 static const unsigned m_maskBadEMfraction        = 0x10000000; // bit 28 Bad value for EM/tot SumE ratio
 static const unsigned m_maskGlobBigMEtSEtRatio   = 0x20000000; // bit 29 large value for |MET/SumET|
 static const unsigned m_maskObjInCrack           = 0x40000000; // bit 30 High energy in crack scintillators
 static const unsigned m_maskGlobErrors           = 0x80000000; // bit 31 Error detected 

}; 



#endif //> !TRIGEFMISSINGET_IMISSINGETTOOL_H

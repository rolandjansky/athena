/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyInterfaceAbstract_H
#define MuonboyInterfaceAbstract_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

class MboyPosDigiChoice;
class MboyVarByName;
class MboyAlineStore;
class MboyBlineStore;
class MboyIlineStore;
class MuonboyBFAbstract;
class MuonboyRTAbstract;
class MboyGenTrackStore;
class MuonRecDigiStore;
class MB_SegmentStore;
class MB_TrackStore;
class MB_Track;

class MbDRAT;
class MbProtoTrack;

class MbMuSc;

class MboyROAStore;

  /**
   @class MuonboyInterfaceAbstract

   This class is the base class interfacing Muonboy reconstruction

  @author samusog@cern.ch
  
  */

class MuonboyInterfaceAbstract{
public:
    MuonboyInterfaceAbstract();
    virtual ~MuonboyInterfaceAbstract();

public:
///////////////////////////////////

   /**Initialize reconstruction with B field only */
   virtual void initialize(
                           MuonboyBFAbstract* pMuonboyBFAbstract
                          ) = 0 ;
                          
   /**Initialize reconstruction with B field and Amdc geometry */
   virtual void initialize(
                           std::string AmdcString,
                           std::string AgddString,
                           MuonboyBFAbstract* pMuonboyBFAbstract
                          ) = 0 ;

   /**BackTrack */
   virtual MB_Track* MboyMuonBackTrack(MboyVarByName* pMboyVarByName, MB_Track* pMB_Track ) = 0 ;

   /**Do reconstruction */
   virtual int DoRec(
                     MboyVarByName*           pMboyVarByName,
                     MuonboyBFAbstract*       pMuonboyBFAbstract,
                     MuonboyRTAbstract*       pMuonboyRTAbstract,
                     const MboyPosDigiChoice* pMboyPosDigiChoice ,
                     MuonRecDigiStore*        pMuonRecDigiStore,
                     const MboyGenTrackStore* pMboyGenTrackStore,
                     MB_TrackStore*	      pMB_TrackStore,
                     MB_SegmentStore*	      pMB_SegmentStore,
                     MboyROAStore*	      pMboyROAExternalStore = 0                     
                    ) = 0 ;
   virtual int DoRec(
                     MboyVarByName*           pMboyVarByName,
                     MboyAlineStore*          pMboyAlineStore,
                     MboyBlineStore*          pMboyBlineStore,
                     MuonboyBFAbstract*       pMuonboyBFAbstract,
                     MuonboyRTAbstract*       pMuonboyRTAbstract,
                     const MboyPosDigiChoice* pMboyPosDigiChoice ,
                     MuonRecDigiStore*        pMuonRecDigiStore,
                     const MboyGenTrackStore* pMboyGenTrackStore,
                     MB_TrackStore*	      pMB_TrackStore,
                     MB_SegmentStore*	      pMB_SegmentStore,
                     MboyROAStore*	      pMboyROAExternalStore = 0
                    ) = 0 ;

   /**find track in the muon system  */
   virtual MbProtoTrack* find(
        MboyVarByName*           pMboyVarByName,
        MbProtoTrack*            pMbProtoTrack ,
        MbProtoTrack*            pTargetSegmentsProtoTrack 
                     ) = 0 ;
    /** refine */
   virtual MbProtoTrack* refine(
        MboyVarByName*           pMboyVarByName,
        MbProtoTrack*            pMbProtoTrack,
        const MuonRecDigiStore*  pMuonRecDigiStore,
        MB_TrackStore*           pMB_TrackStore
                                            ) = 0 ;
   virtual int SetAMbDRAT(
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
                     ) = 0 ;
   virtual int LoadEvent(
        MboyVarByName*           pMboyVarByName,
        MuonboyBFAbstract*       pMuonboyBFAbstract,
        MuonboyRTAbstract*       pMuonboyRTAbstract,
        const MboyPosDigiChoice* pMboyPosDigiChoice ,
        MuonRecDigiStore*        pMuonRecDigiStore
                     ) = 0 ;

   /**Collect matter*/
   virtual void DoCollectMatter(
                                MuonboyBFAbstract*   pMuonboyBFAbstract,
                                int AtCosmMatter,
                                double Xpos_in,
                                double Ypos_in,
                                double Zpos_in,
                                double Xmom_in,
                                double Ymom_in,
                                double Zmom_in,
                                double Charge,
                                std::vector<MbMuSc*>* pVectorOfMbMuSc
		               ) = 0 ;
		  
		  
   /**Set matiere from TagInfo */
   virtual void SetMatiereTagInfo(std::vector< std::pair<std::string,std::string> > TagInfoXML) = 0 ;
		  
   /**Set matiere from AGDD2GeoSwitches Data */
   virtual void SetMatiereAGDD2GeoSwitches( std::vector<std::string> AGDD2GeoSwitches) = 0 ;
		  	  
   /**Set Dead Channel Informations */
   virtual void SetDeadChannels( std::vector<std::string> MultiLayerOut) = 0 ;
		  	  
   /**Set Alignment Informations */
   virtual int SetAliLine(
                          MboyAlineStore* pMboyAlineStore,
                          MboyBlineStore* pMboyBlineStore, 
                          MboyIlineStore* pMboyIlineStore 
                         ) = 0 ;
		  	  
   /** Set Field Status data */
   virtual void SetFieldStatus(
                               int getToroidBarrelOn ,
                               int getToroidECTAOn   ,
                               int getToroidECTCOn   ,
                               int getSolenoidOn     ,
                               int getAllToroidOn    ,
                               int getFieldStatusOn 
                              ) = 0 ;
		  	  
   /**Set configuration */
   virtual void SetConfiguration( MboyVarByName*  pMboyVarByName) = 0 ;

   /**Set configuration variables */
   virtual void SetVariablesDefault(MboyVarByName*  pMboyVarByName) = 0 ;
		  
   /**Reset configuration variables */
   virtual void Reset() = 0 ;

   /**Get Agdd Xml variables */
   virtual void GetAgddXmlVarNb    (int& NbVar )                                   = 0 ;
   virtual void GetAgddXmlVarName  (int  IVar , std::string& NameVar )             = 0 ;
   virtual void GetAgddXmlVarValue (std::string  NameVar, int& Iok, double& Value) = 0 ;

};
#endif


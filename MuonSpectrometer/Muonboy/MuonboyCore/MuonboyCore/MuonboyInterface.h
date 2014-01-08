/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyInterface_H
#define MuonboyInterface_H

#include "MuonboyCore/MuonboyCoreMisc.h"

#include "MboyCoreEvt/MuonboyInterfaceAbstract.h"

class AandBlinesLoader;
class MuonboyCocoupMaker;
class MboyMuonBackTracker;
class MuonboyDoDRAT;
class MuonboyDoProtoTrack;
class MuonboyRecMaker;
class MuonboyCollectMatter;
class MuonboyConfigure;
class AgddXml;
class MuonboyExtROALoader;
class MuonboySetBFRT;

  /**
   @class MuonboyInterface

   this class is interfacing Muonboy reconstruction
   It derives from MuonboyInterfaceAbstract

  @author samusog@cern.ch
  
  */

class MuonboyInterface: public MuonboyInterfaceAbstract{
public:
    MuonboyInterface();
    virtual ~MuonboyInterface();

public:
///////////////////////////////////

   /**Initialize reconstruction with B field only */
   virtual void initialize(
                           MuonboyBFAbstract* pMuonboyBFAbstract
                          );
                          
   /**Initialize reconstruction with B field and Amdc geometry */
   virtual void initialize(
                           std::string AmdcString,
                           std::string AgddString,
                           MuonboyBFAbstract* pMuonboyBFAbstract
                          );

   /**BackTrack */
   virtual MB_Track* MboyMuonBackTrack(MboyVarByName* pMboyVarByName, MB_Track* pMB_Track )  ;

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
                    ) ;
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
                    );

   /**find track in the muon system  */
   virtual MbProtoTrack* find(
        MboyVarByName*           pMboyVarByName,
        MbProtoTrack*            pMbProtoTrack ,
        MbProtoTrack*            pTargetSegmentsProtoTrack 
                    );
   virtual MbProtoTrack* refine(
        MboyVarByName*           pMboyVarByName,
        MbProtoTrack*            pMbProtoTrack,
        const MuonRecDigiStore*  pMuonRecDigiStore,
        MB_TrackStore*           pMB_TrackStore
                    );
   virtual int SetAMbDRAT(
        MboyVarByName* pMboyVarByName,
        MbDRAT*        pMbDRAT
                    );
   virtual int LoadEvent(
        MboyVarByName*           pMboyVarByName,
        MuonboyBFAbstract*       pMuonboyBFAbstract,
        MuonboyRTAbstract*       pMuonboyRTAbstract,
        const MboyPosDigiChoice* pMboyPosDigiChoice ,
        MuonRecDigiStore*        pMuonRecDigiStore
                    );

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
		               );

   /**Set matiere from TagInfo */
   virtual void SetMatiereTagInfo(std::vector< std::pair<std::string,std::string> > TagInfoXML);
		  
   /**Set matiere from AGDD2GeoSwitches Data */
   virtual void SetMatiereAGDD2GeoSwitches( std::vector<std::string> AGDD2GeoSwitches) ;
		  	  
   /**Set Dead Channel Informations */
   virtual void SetDeadChannels( std::vector<std::string> MultiLayerOut) ;
		  	  
   /**Set Alignment Informations */
   virtual int SetAliLine(
                          MboyAlineStore* pMboyAlineStore,
                          MboyBlineStore* pMboyBlineStore, 
                          MboyIlineStore* pMboyIlineStore 
                         )  ;
		  	  
   /** Set Field Status data */
   virtual void SetFieldStatus(
                               int getToroidBarrelOn ,
                               int getToroidECTAOn   ,
                               int getToroidECTCOn   ,
                               int getSolenoidOn     ,
                               int getAllToroidOn    ,
                               int getFieldStatusOn 
                              ) ;
		  	  
   /**Set configuration */
   virtual void SetConfiguration( MboyVarByName*  pMboyVarByName) ;

   /**Set configuration variables */
   virtual void SetVariablesDefault(MboyVarByName*  pMboyVarByName);
		  
   /**Reset configuration variables */
   virtual void Reset();

   /**Get Agdd Xml variables */
   virtual void GetAgddXmlVarNb    (int& NbVar )                                   ;
   virtual void GetAgddXmlVarName  (int  IVar , std::string& NameVar )             ;
   virtual void GetAgddXmlVarValue (std::string  NameVar, int& Iok, double& Value) ;

private:
///////////////////////////////////
   AandBlinesLoader* p_AandBlinesLoader ; //!< Pointer on AandBlinesLoader

   MuonboyCocoupMaker* p_MuonboyCocoupMaker ; //!< Pointer on MuonboyCocoupMaker

   MboyMuonBackTracker* p_MboyMuonBackTracker ; //!< Pointer on MboyMuonBackTracker

   MuonboyDoDRAT* p_MuonboyDoDRAT ; //!< Pointer on MuonboyDoDRAT

   MuonboyDoProtoTrack* p_MuonboyDoProtoTrack ; //!< Pointer on MuonboyDoProtoTrack

   MuonboyRecMaker* p_MuonboyRecMaker ; //!< Pointer on MuonboyRecMaker

   MuonboyCollectMatter* p_MuonboyCollectMatter ; //!< Pointer on MuonboyCollectMatter

   MuonboyConfigure* p_MuonboyConfigure ; //!< Pointer on MuonboyConfigure

   AgddXml* p_AgddXml ; //!< Pointer on AgddXml

   MuonboyExtROALoader* p_MuonboyExtROALoader ; //!< Pointer on MuonboyExtROALoader

   MuonboySetBFRT* p_MuonboySetBFRT ; //!< Pointer on MuonboySetBFRT


};
#endif


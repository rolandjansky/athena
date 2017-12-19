/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmdcsimrecAthenaSvc_H
#define AmdcsimrecAthenaSvc_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"

class StoreGateSvc;

class ITagInfoMgr;
class IGeoModelSvc;

namespace MuonGM {
  class MuonDetectorManager ;
}

class AtlasDetectorID ;

#include "MuonAlignmentData/CorrContainer.h"

#include "AthenaKernel/IOVSvcDefs.h"

/////////////////////////////////////////////////////////////////////////////
#include "AmdcAth/AmdcAthMisc.h"

class AmdcAlineStore ;
class AmdcBlineStore ;
class AmdcIlineStore ;

#include "AmdcCore/Amdcsimrec.h"

template <class TYPE> class SvcFactory;

static const InterfaceID IID_IAmdcsimrecAthenaSvc(1001, 1, 0);

  /**
   @class AmdcsimrecAthenaSvc

   This class is the service needed to access Amdcsimrec functionnalities

  @author samusog@cern.ch
  
  */

class AmdcsimrecAthenaSvc : public AthService {
public:
///////////////////////////////////

   static const InterfaceID& interfaceID() { return IID_IAmdcsimrecAthenaSvc; }

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

   bool       InitializedSvc();
   bool       UsableSvc();
   StatusCode UpdatedSvc(IOVSVC_CALLBACK_ARGS);

   StatusCode initializeFromGeomodelCallback(IOVSVC_CALLBACK_ARGS);
   StatusCode SetAmdcABlineFromCoolCallback(IOVSVC_CALLBACK_ARGS);
   StatusCode initializeFromGeomodelSetAmdcABlineFromCoolCallback(IOVSVC_CALLBACK_ARGS);

   /**Get Amdcsimrec interface */
   Amdcsimrec* GetAmdcsimrec();

   /**Get the source of Amdcsimrec data */
   std::string GetNameOfTheSource();

   /**Get the name of used Amdcsimrec file */
   std::string GetNameOfTheFile();

   /**Get the location of used Amdcsimrec file */
   std::string GetLocationOfTheFile();

   /**Get the Amdc string */
   std::string GetAmdcString();

   /**Get the Agdd string */
   std::string GetAgddString();

   /**Get A and B lines collections*/
   StatusCode GetAmdcAliStores(
                               int&  AmdcABlinesStamp,
                               const AmdcAlineStore*& pAmdcAlineStore ,
                               const AmdcBlineStore*& pAmdcBlineStore ,
                               const AmdcIlineStore*& pAmdcIlineStore
                              );

   /**Set and Get TagInfoXML */
   StatusCode SetTagInfoXML(IOVSVC_CALLBACK_ARGS);
   int GetTagInfoXMLStamp();
   std::vector< std::pair<std::string,std::string> > GetTagInfoXML();

   /**Get AGDD2GeoSwitches data*/
   int GetAGDD2GeoSwitchesStamp();
   std::vector<std::string> GetAGDD2GeoSwitches();

   friend class SvcFactory<AmdcsimrecAthenaSvc>;

   // Standard Constructor
   AmdcsimrecAthenaSvc(const std::string& name, ISvcLocator* svc);

   // Standard Destructor
   virtual ~AmdcsimrecAthenaSvc();

private:
///////////////////////////////////

   bool m_IsUsable ; //!< Tell usuability state 

   bool m_IsInitialized ; //!< Tell initialisation state 

   /**Initialize from GeoModel  and Set A and B lines collections*/
   StatusCode regFcninitializeFromGeomodelSetAmdcABlineFromCool();

   /**Set A and B lines collections*/
   StatusCode regFcnSetAmdcABlineFromCool();
   StatusCode SetAmdcABlineFromCool();

   /**Initialize from an Ascii file */
   StatusCode initializeAscii();

   /**Set location of the Ascii file */
   StatusCode SetLocation(std::string NameFile, std::string& FileLocation);

   /**Initialize from GeoModel */
   StatusCode regFcninitializeFromGeomodel();
   StatusCode initializeFromGeomodel();

   /**Initialize from Oracle node*/
   StatusCode initializeFromOracleNode();

   /**Initialize from Oracle*/
   StatusCode initializeFromOracle();

   /**Post Geometry Loading sequence */
   StatusCode PostGeometryLoadingSequence();

   /**Produce Amdc and Agdd strings */
   StatusCode ProduceString();

   /**Set Geometry from strings */
   StatusCode SetFromString(std::string AmdcString,std::string AgddString);
  

    Amdcsimrec* p_Amdcsimrec ; //!< Pointer on Amdcsimrec interface

    std::string m_NameOfTheSource ; //!< Name of the source used

    std::string m_NameOfTheFile ; //!< Name of the file used

    std::string m_LocationOfTheFile ; //!< Location of the file used

    std::string m_AmdcString ; //!< Amdc string
    std::string m_AgddString ; //!< Agdd string

    int m_EmergencyOut ; //!< Optional stop at the end of initialisation

    int m_ControlALine ; //!< Optional modification of A lines
    int m_ControlILine ; //!< Optional modification of I lines

    int m_TruncateALine ; //!< Optional truncation of A lines

    int m_TruncatePLine ; //!< Optional truncation of P lines

    std::string m_OracleNodedetectorKey  ; //!< Name of Oracle node
    std::string m_OracleNodedetectorNode ; //!< Name of Oracle node

    std::string m_detectorKey  ; //!< Oracle node 
    std::string m_detectorNode ; //!< Oracle node

    int m_CtrlDumpCo      ; //!< Control of debug dump
    int m_CtrlFileStrings ; //!< Control of debug dump
    int m_CtrlFileOracle  ; //!< Control of debug dump

    int m_PrintLevel  ; //!< Control print level

    const IGeoModelSvc* p_IGeoModelSvc ; //!< Pointer On GeoModel

    
//Alignment
    StoreGateSvc*   p_detStore    ; //!< Pointer On StoreGateSvc
    const MuonGM::MuonDetectorManager*  p_MuonDetectorManager ; //!< Pointer On MuonDetectorManager

    AmdcAlineStore* p_AmdcAlineStoreInternal ;
    AmdcBlineStore* p_AmdcBlineStoreInternal ;
    AmdcIlineStore* p_AmdcIlineStoreInternal ;

    AmdcAlineStore* p_AmdcAlineStoreExternal ;
    AmdcBlineStore* p_AmdcBlineStoreExternal ;
    AmdcIlineStore* p_AmdcIlineStoreExternal ;

    StatusCode SetAliStoreInternal();
    StatusCode SetAmdcAlineStoreExternal();
    StatusCode SetAmdcBlineStoreExternal();
    StatusCode SetAmdcIlineStoreExternal();

    StatusCode GetALineMapContainer (const ALineMapContainer*& pALineMapContainer);
    StatusCode GetBLineMapContainer (const BLineMapContainer*& pBLineMapContainer);
    StatusCode GetCscInternalAlignmentMapContainer  (const CscInternalAlignmentMapContainer*& pCscInternalAlignmentMapContainer);

    void       CheckALineMapContainer(const ALineMapContainer*& pALineMapContainer);
    void       CheckBLineMapContainer(const BLineMapContainer*& pBLineMapContainer);
 
    StatusCode SetAmdcAlineStoreFromExternal(AmdcAlineStore* pAmdcAlineStore,int& LoadIer);
    StatusCode SetAmdcBlineStoreFromExternal(AmdcBlineStore* pAmdcBlineStore,int& LoadIer);
    StatusCode SetAmdcIlineStoreFromExternal(AmdcIlineStore* pAmdcIlineStore,int& LoadIer);
   
    int m_CompareInternalExternalALineContainer ; //!< Compare Int.Ext. A containers
    int m_ModifyALineContainer                  ; //!< Manage A external line
    int m_RmAlignCSC                            ; //!< Manange A line for CSC
    int m_SortAlignContainer                    ; //!< Sort Alignement Container

    int m_AlignmentSource    ; //!< Control Alignment Source
    int m_AlignmentCorr      ; //!< Control Alignment Corrections
    int m_AmdcABlinesStamp   ; //!< Volatility Status

    int m_UseMuonDetectorManagerForInternal ; //!< Control use of the MuonDetectorManager  for internal container

    int m_DontSetAmdcABlineFromCool  ; //!< All is the variable name


//Hard-Soft names correspondence
    int m_HardSoftCheck; //!< Optional Check of Hard/Soft schemes
    void TestHardSoftStuff();

//Xtomo check
    int m_XtomoCheck ; //!< Optional Check of Xtomo stuff
    void TestXtomoStuff();

//Tag Info 
    ITagInfoMgr*  p_ITagInfoMgr ; //!< Pointer On ITagInfoMgr
    int m_TagInfoXMLStamp   ; //!< Volatility Status
    std::vector< std::pair<std::string,std::string> > m_TagInfoXML  ; //!< Store infos from TagInfo

//AGDD2GeoSwitches Data
    int m_AGDD2GeoSwitchesStamp   ; //!< Volatility Status
    std::vector<std::string> m_AGDD2GeoSwitches  ; //!< Store infos from AGDD2GeoSwitches
 
//Load A and B lines for positions computation
    StatusCode LoadAlineStoreExternal();
    StatusCode LoadBlineStoreExternal();
    StatusCode LoadCscInternalAlignmentStoreExternal();
    
    const AtlasDetectorID* p_AtlasDetectorID ; //!< Pointer On AtlasDetectorID

};

#endif


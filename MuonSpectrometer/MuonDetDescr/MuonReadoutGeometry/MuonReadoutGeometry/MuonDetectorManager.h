/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDetectorManager_H
#define MuonDetectorManager_H

#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/MsgStream.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
// for nSW
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "MuonReadoutGeometry/GenericMDTCache.h"
#include "MuonReadoutGeometry/GenericRPCCache.h"
#include "MuonReadoutGeometry/GenericTGCCache.h"
#include "MuonReadoutGeometry/GenericCSCCache.h"
#include "MuonAlignmentData/CorrContainer.h"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#define optimRE true

typedef std::map<Identifier,ALinePar*>::const_iterator ciALineMap;
typedef std::map<Identifier,BLinePar*>::const_iterator ciBLineMap;
typedef std::map<Identifier,CscInternalAlignmentPar*>::const_iterator ciCscInternalAlignmentMap;
typedef std::map<Identifier,MdtAsBuiltPar*>::const_iterator ciMdtAsBuiltMap;

class IMessageSvc;

class Identifier;
class IdentifierHash;
class MdtIdHelper;

namespace MuonGM {

  class CscReadoutElement;
  class TgcReadoutElement;
  class RpcReadoutElement;
  class MdtReadoutElement;
  class MuonClusterReadoutElement;
  class MuonStation;
  // New Small Wheel
  class sTgcReadoutElement;
  class MMReadoutElement;
    
  class CscDetectorElement;
  class TgcDetectorElement;
  class RpcDetectorElement;
  class MdtDetectorElement;
  class TgcReadoutParams;

  /**
     The MuonDetectorManager stores the transient representation of the Muon Spectrometer geometry
     and provides access to all main blocks of information needed by generic clients of the 
     geometry:
     XxxReadoutElements, MuonStations, MuonIdHelpers, geometryVersion, etc. 
     More details below.
  */

  class MuonDetectorManager: public GeoVDetectorManager {

  public:
    
    // Constructor:
    MuonDetectorManager();

    // Destructor:
    ~MuonDetectorManager();
  
    // Gets the number of tree tops:  required:
    virtual unsigned int getNumTreeTops() const;
  
    // Gets the ith tree top:  required:
    virtual PVConstLink getTreeTop(unsigned int i) const;
  
    // Add a tree top:
    void addTreeTop( PVLink);

    // Add a XxxReadoutElement to the Collection
    void addMdtReadoutElement (const MdtReadoutElement*, Identifier);//!< store the MdtReadoutElement using as "key" the identifier
    void addRpcReadoutElement (const RpcReadoutElement*, Identifier);//!< store the RpcReadoutElement using as "key" the identifier
    void addTgcReadoutElement (const TgcReadoutElement*, Identifier);//!< store the TgcReadoutElement using as "key" the identifier
    void addCscReadoutElement (const CscReadoutElement*, Identifier);//!< store the CscReadoutElement using as "key" the identifier
    void addsTgcReadoutElement (const sTgcReadoutElement*, Identifier);//!< store the CscReadoutElement using as "key" the identifier
    void addMMReadoutElement (const MMReadoutElement*, Identifier);//!< store the CscReadoutElement using as "key" the identifier
    void addsTgcReadoutElement_withIdFields (const sTgcReadoutElement*, int iStname, int iStEta, int iStPhi, int imL);//!< store the sTgcReadoutElement using as "key" the identifier
    void addMMReadoutElement_withIdFields   (const MMReadoutElement*,   int iStname, int iStEta, int iStPhi, int imL);//!< store the MMReadoutElement using as "key" the identifier

    // storeTgcReadoutParams
    void storeTgcReadoutParams(TgcReadoutParams* x);
    
    // storeCscInternalAlignmentParams
    void storeCscInternalAlignmentParams(CscInternalAlignmentPar* x);

    void storeMdtAsBuiltParams(MdtAsBuiltPar* x);
    
    // access to Readout Elements
    const MdtReadoutElement* getMdtReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const RpcReadoutElement* getRpcReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const TgcReadoutElement* getTgcReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const CscReadoutElement* getCscReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    // New Small Wheel
    const MMReadoutElement*  getMMReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const sTgcReadoutElement* getsTgcReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)

    const MuonClusterReadoutElement* getMuonClusterReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)


    const MdtReadoutElement* getMdtReadoutElement(int i1, int i2, int i3, int i4) const;
    //!< access via internally defined array indices; not friendly to users

    // FIX: New Small Wheel ? No equivalent in .cxx file
    const sTgcReadoutElement* getsTgcReadoutElement(int i1, int i2, int i3, int i4) const;
    //!< access via internally defined array indices; not friendly to users
    const MMReadoutElement* getMMReadoutElement(int i1, int i2, int i3, int i4) const;
    //!< access via internally defined array indices; not friendly to users

    const TgcReadoutElement* getTgcReadoutElement(int i1, int i2, int i3) const;
    //!< access via internally defined array indices; not friendly to users

    const CscReadoutElement* getCscReadoutElement(int i1, int i2, int i3, int i4) const;
    //!< access via internally defined array indices; not friendly to users


    const MdtReadoutElement* getMdtRElement_fromIdFields(int i1, int i2, int i3, int i4) const;
    //!< access via extended identifier field (no unpacking)

    const sTgcReadoutElement* getsTgcRElement_fromIdFields(int i1, int i2, int i3, int i4) const;
    //!< access via extended identifier field (no unpacking)

    const MMReadoutElement* getMMRElement_fromIdFields(int i1, int i2, int i3, int i4) const;
    //!< access via extended identifier field (no unpacking)

    const TgcReadoutElement* getTgcRElement_fromIdFields(int i1, int i2, int i3) const;
    //!< access via extended identifier field (no unpacking)

    const CscReadoutElement* getCscRElement_fromIdFields(int i1, int i2, int i3, int i4) const;
    //!< access via extended identifier field (no unpacking)

    inline const MdtReadoutElement* getMdtReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id
    inline const RpcReadoutElement* getRpcReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id
    inline const TgcReadoutElement* getTgcReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id
    inline const CscReadoutElement* getCscReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id

    inline const MdtDetectorElement* getMdtDetectorElement(IdentifierHash id)  const;//!< access via data-collection hash id

    // access to Detector Elements (via hash id ) - NOTICE Detector Elements have the granularity of the EDM digit collections
    inline const RpcDetectorElement* getRpcDetectorElement(IdentifierHash id) const;     
    inline const TgcDetectorElement* getTgcDetectorElement(IdentifierHash id) const;
    inline const CscDetectorElement* getCscDetectorElement(IdentifierHash id) const;
    // TODO: New Small Wheel

    inline unsigned int nMuonStation() const; //!< Number of MuonStations

    inline unsigned int nMdtRE() const;//!< Number of Mdt ReadoutElements 
    inline unsigned int nsTgcRE() const;//!< Number of sTgc ReadoutElements 
    inline unsigned int nMMRE() const;//!< Number of MM ReadoutElements 
    inline unsigned int nCscRE() const;//!< Number of Csc ReadoutElements 
    inline unsigned int nRpcRE() const;//!< Number of Rpc ReadoutElements 
    inline unsigned int nTgcRE() const;//!< Number of Tgc ReadoutElements 

    inline unsigned int nMdtDE() const;//!< Number of Mdt DetectorElements 
    inline unsigned int nCscDE() const;//!< Number of Csc DetectorElements 
    inline unsigned int nRpcDE() const;//!< Number of Rpc DetectorElements 
    inline unsigned int nTgcDE() const;//!< Number of Tgc DetectorElements 
    // TODO: New Small Wheel
    
    // Geometry versioning    
    inline std::string geometryVersion() const; //!< it can be Rome-Initial or P03, or ... it's the name of the layout 
    inline void setGeometryVersion(std::string version);
    inline std::string get_DBMuonVersion() const;//!< the name of the MuonSpectrometer tag (in the geometry DB) actually accessed
    inline void set_DBMuonVersion(std::string );
    
    // Access to identifier helpers
    inline const MdtIdHelper*  mdtIdHelper() const;
    inline const CscIdHelper*  cscIdHelper() const;
    inline const RpcIdHelper*  rpcIdHelper() const;
    inline const TgcIdHelper*  tgcIdHelper() const;
    // New Small Wheel
    inline const sTgcIdHelper*  stgcIdHelper() const;
    inline const MmIdHelper*  mmIdHelper() const;
      
    // Setting the identifier helpers
    inline void set_mdtIdHelper(const MdtIdHelper* idh);
    inline void set_cscIdHelper(const CscIdHelper* idh);
    void set_rpcIdHelper(const RpcIdHelper* idh);
    inline void set_tgcIdHelper(const TgcIdHelper* idh);
    // New Small Wheel
    inline void set_stgcIdHelper(const sTgcIdHelper* idh);
    inline void set_mmIdHelper(const MmIdHelper* idh);    

    // Generic Technology descriptors
    inline void setGenericMdtDescriptor(const GenericMDTCache& mc);
    inline const GenericMDTCache* getGenericMdtDescriptor() const;
    inline void setGenericRpcDescriptor(const GenericRPCCache& rc);
    inline const GenericRPCCache* getGenericRpcDescriptor() const;
    inline void setGenericCscDescriptor(const GenericCSCCache& cc);
    inline const GenericCSCCache*  getGenericCscDescriptor() const;
    inline void setGenericTgcDescriptor(const GenericTGCCache& tc);
    inline const GenericTGCCache* getGenericTgcDescriptor() const;
    // TODO: New Small Wheel

    inline void setCachingFlag(int value);
    inline int cachingFlag() const;
    inline void setCacheFillingFlag(int value);
    inline int cacheFillingFlag() const;

    inline void setMinimalGeoFlag (int flag);
    inline int  MinimalGeoFlag () const;
    inline void setCutoutsFlag (int flag);
    inline int  IncludeCutoutsFlag () const;
    inline void setCutoutsBogFlag (int flag);
    inline int  IncludeCutoutsBogFlag () const;
    inline void setMdtDeformationFlag (int flag) {m_applyMdtDeformations = flag;}
    inline int  mdtDeformationFlag () const {return m_applyMdtDeformations;}
    inline void setMdtAsBuiltParamsFlag (int flag) {m_applyMdtAsBuiltParams = flag;}
    inline int  mdtAsBuiltParamsFlag () const {return m_applyMdtAsBuiltParams;}
    inline void setControlAlinesFlag (int flag) {m_controlAlines = flag;}
    inline int  controlAlinesFlag () const {return m_controlAlines;}
    inline void setApplyCscIntAlignment(bool x) {m_useCscIntAlign = x;}
    inline bool applyMdtDeformations () const {return (bool)m_applyMdtDeformations;}
    inline bool applyMdtAsBuiltParams () const {return (bool)m_applyMdtAsBuiltParams;}
    inline bool applyCscIntAlignment () const {return m_useCscIntAlign;}
    inline void setCscIlinesFlag (int flag) {m_controlCscIlines = flag;}
    inline int  CscIlinesFlag () const {return m_controlCscIlines;}
    inline void setCscFromGM(bool x) {m_useCscIlinesFromGM = x;}
    inline bool CscFromGM() const {return m_useCscIlinesFromGM;}
    
    enum readoutElementHashMax
      {    
        MdtRElMaxHash = 2500,
        CscRElMaxHash = 130,
        RpcRElMaxHash = 2600,
        TgcRElMaxHash = 1600
      };
    
    enum detElementHashMax
      {    
        MdtDetElMaxHash = 1200,
        CscDetElMaxHash = 65,
        RpcDetElMaxHash = 1300,
        TgcDetElMaxHash = 1600
      };
    
    
    
    enum MdtGMRanges
      {
        NMdtStatType     = 26, 
        NMdtStatEta      = 17,
        NMdtStEtaOffset  = 8,
        NMdtStatPhi      = 8,
        NMdtMultilayer   = 2
      };
    enum RpcGMRanges
      {
        NRpcStatType     = 12, // there are 12 station types where RPCs can be installed: BML/BMS/BOL/BOS/BMF/BOF/BOG/BME/BIR/BIM/BIL/BIS
        NRpcStatEta      = 17,
        NRpcStEtaOffset  = 8,
        NRpcStatPhi      = 8,
        NDoubletR        = 2,
        NDoubletZ        = 4
      }; // using some trick to save space: dbz=4 if rib's chambers and doubletphi=2;
    enum TgcGMRanges
      {
        NTgcStatType     = 8,
        NTgcStatTypeOff  = -41,
        NTgcStatEta      = 10,
        NTgcStEtaOffset  = 5,
        NTgcStatPhi      = 48
      };
    enum CscGMRanges
      {
        NCscStatType     = 2,
        NCscStatTypeOff  = -50,
        NCscStatEta      = 2,
        NCscStEtaOffset  = 1,
        NCscStatPhi      = 8,
        NCscChamberLayer = 2
      };    
    enum sTgcGMRanges
      {
        NsTgStatType     = 1, /// don't wont to have different array coloumns for different names // using this field to distinguish large=0 and small=1
        NsTgStatTypeOff  = 0, /// relevant only when couplig to identifiers 
        NsTgStatEta      = 6, /// 3 x 2 sides (-3,-2,-1 and 1,2,3) 
        NsTgStEtaOffset  = 3, /// starting from 0-5 
        NsTgStatPhi      = 16, // large and small sector together 
        NsTgChamberLayer = 2
      };    
    enum mmGMRanges
      {
        NMMcStatType     = 1, /// don't wont to have different array coloumns for different names
        NMMcStatTypeOff  = 0, /// relevant only when couplig to identifiers
        NMMcStatEta      = 4, /// 2 x 2 sides (-2,-1 and 1,2)
        NMMcStEtaOffset  = 2, /// starting from 0-3
        NMMcStatPhi      = 16, // large and small sector together
        NMMcChamberLayer = 2
      };

    // Add a MuonStation to the list
    void addMuonStation(MuonStation* mst); 
    MuonStation* getMuonStation (std::string stName, int eta, int phi) const; 
    //<! access to the MuonStation by StationName, Jzz, Jff (amdb indices!!!! not stationPhi and Eta)
    std::string muonStationKey(std::string stName, int statEtaIndex, int statPhiIndex) const;

    void clearCache() const;
    void refreshCache() const;
    void fillCache() const;

    void clearMdtCache() const;
    void clearRpcCache() const;
    void clearCscCache() const;
    void clearTgcCache() const;
    // New Small Wheel
    void clearsTgcCache() const;
    void clearMMCache() const;

    void refreshMdtCache() const;
    void refreshRpcCache() const;
    void refreshCscCache() const;
    void refreshTgcCache() const;
    // New Small Wheel
    void refreshsTgcCache() const;
    void refreshMMCache() const;

    void fillMdtCache() const;
    void fillRpcCache() const;
    void fillCscCache() const;
    void fillTgcCache() const;
    // New Small Wheel
    void fillsTgcCache() const;
    void fillMMCache() const;

    inline ALineMapContainer * ALineContainer() const;
    inline BLineMapContainer * BLineContainer() const;
    inline CscInternalAlignmentMapContainer * CscInternalAlignmentContainer() const;
    inline MdtAsBuiltMapContainer* MdtAsBuiltContainer() const;
    inline ciALineMap ALineMapBegin() const;
    inline ciBLineMap BLineMapBegin() const;
    inline ciALineMap ALineMapEnd() const;
    inline ciBLineMap BLineMapEnd() const;
    inline ciCscInternalAlignmentMap CscALineMapBegin() const;
    inline ciCscInternalAlignmentMap CscALineMapEnd() const;
    inline ciMdtAsBuiltMap MdtAsBuiltMapBegin() const;
    inline ciMdtAsBuiltMap MdtAsBuiltMapEnd() const;
    StatusCode updateAlignment(const ALineMapContainer* a) const; 
    StatusCode updateDeformations(const BLineMapContainer* a) const;
    StatusCode updateAsBuiltParams(const MdtAsBuiltMapContainer* a) const;
    StatusCode initCSCInternalAlignmentMap() const;
    StatusCode updateCSCInternalAlignmentMap(const CscInternalAlignmentMapContainer* cscIntAline) const;
    void initABlineContainers() const;

    // get Mdt AsBuilt parameters for chamber specified by Identifier
    MdtAsBuiltPar* getMdtAsBuiltParams(Identifier id);

    //!< provide a pointer to the msg svc to all readout geometry 
    inline IMessageSvc* msgSvc() const;

    int rpcStationName(const int stationIndex) const; // map the RPC station indices (0-NRpcStatType) back to the RpcIdHelper stationNames
    
  private:
    const RpcReadoutElement* getRpcReadoutElement(int i1, int i2, int i3, int i4, int i5) const;
    RpcReadoutElement* getRpcReadoutElement(int i1, int i2, int i3, int i4, int i5);
    const RpcReadoutElement* getRpcRElement_fromIdFields(int i1, int i2, int i3, int i4, int i5, int i6) const;

    unsigned int rpcStationTypeIdx(const int stationName) const; // map the RPC stationNames from the RpcIdHelper to 0-NRpcStatType
    enum RpcStatType {
        BML=0,
        BMS,
        BOL,
        BOS,
        BMF,
        BOF,
        BOG,
        BME,
        BIR,
        BIM,
        BIL,
        BIS,
        UNKNOWN
    };

    int m_cachingFlag;
    int m_cacheFillingFlag;
    int m_minimalgeo;
    int m_includeCutouts;
    int m_includeCutoutsBog;
    int m_controlAlines;
    int m_applyMdtDeformations;
    int m_applyMdtAsBuiltParams;
    bool m_useCscIntAlign;    
    int m_controlCscIlines;
    bool m_useCscIlinesFromGM;
    std::vector<PVLink> m_envelope;  // Tree-top...
    
    GenericCSCCache m_genericCSC;
    GenericMDTCache m_genericMDT;
    GenericRPCCache m_genericRPC;
    GenericTGCCache m_genericTGC;
    // TODO: New Small Wheel

    // Geometry versioning
    
    std::string m_geometryVersion;//generic name of the Layout
    std::string m_DBMuonVersion;  //name of the MuonVersion table-collection in Oracle

    // pointers to IdHelpers
    const MdtIdHelper* m_mdtIdHelper;
    const CscIdHelper* m_cscIdHelper;
    const RpcIdHelper* m_rpcIdHelper;
    const TgcIdHelper* m_tgcIdHelper;
    // New Small Wheel
    const sTgcIdHelper* m_stgcIdHelper;
    const MmIdHelper* m_mmIdHelper;

    // 115.6 kBytes.  
    const MdtReadoutElement*   m_mdtArray[NMdtStatType][NMdtStatEta][NMdtStatPhi][NMdtMultilayer];
    const CscReadoutElement*   m_cscArray[NCscStatType][NCscStatEta][NCscStatPhi][NCscChamberLayer];
    const RpcReadoutElement*   m_rpcArray[NRpcStatType][NRpcStatEta][NRpcStatPhi][NDoubletR][NDoubletZ];
    const TgcReadoutElement*   m_tgcArray[NTgcStatType][NTgcStatEta][NTgcStatPhi];
    const sTgcReadoutElement*  m_stgArray[NsTgStatType][NsTgStatEta][NsTgStatPhi][NsTgChamberLayer];
    const MMReadoutElement*    m_mmcArray[NMMcStatType][NMMcStatEta][NMMcStatPhi][NMMcChamberLayer];
    //
    const MdtReadoutElement *m_mdtArrayByHash[MdtRElMaxHash];
    const CscReadoutElement *m_cscArrayByHash[CscRElMaxHash];
    const RpcReadoutElement *m_rpcArrayByHash[RpcRElMaxHash];
    const TgcReadoutElement *m_tgcArrayByHash[TgcRElMaxHash];
    
    std::map< std::string, MuonStation * > m_MuonStationMap;
    
    unsigned int m_n_mdtRE;
    unsigned int m_n_cscRE;
    unsigned int m_n_rpcRE;
    unsigned int m_n_tgcRE;
    // New Small Wheel
    unsigned int m_n_stgRE;    
    unsigned int m_n_mmcRE;
    
    unsigned int m_n_mdtDE;
    unsigned int m_n_cscDE;
    unsigned int m_n_rpcDE;
    unsigned int m_n_tgcDE;
    // TODO: New Small Wheel

    // pointers to the XxxDetectorElements (with granularity a la EDM)
    std::vector<TgcReadoutParams*> m_TgcReadoutParamsVec;
    // vector of CSC Internal Alignment parameters (just for init purposes) filled from file or Oracle table (RDBReaderAccess) and then deleted by the factory; 
    
    MdtDetectorElement* m_mdtDEArray[MdtDetElMaxHash];
    RpcDetectorElement* m_rpcDEArray[RpcDetElMaxHash];
    TgcDetectorElement* m_tgcDEArray[TgcDetElMaxHash];
    CscDetectorElement* m_cscDEArray[CscDetElMaxHash];
    // TODO: New Small Wheel

    mutable  ALineMapContainer * m_aLineContainer;
    mutable  BLineMapContainer * m_bLineContainer;
    // CscInternalAlignmentMapContainer (pointers) will be created by RDBReaderAccess at the first attempt to store a CscInternalAlignmentPar -rot and transl parameters are held by the CSCredoutElements and the corresponding A-line is provided with this map (key Identifier) by the manager - the manager is responsible to delete the CscInternalAlignmentPar
    mutable  CscInternalAlignmentMapContainer * m_cscALineContainer;
    mutable  MdtAsBuiltMapContainer* m_AsBuiltParamsMap;
    /// RPC name caches
    std::map<int,int> m_rpcStatToIdx;
    std::map<int,int> m_rpcIdxToStat;

    //!< hold a pointer to the message svc to be used by all readout geometry 
    IMessageSvc* m_msgSvc;
    
  };

  IMessageSvc* MuonDetectorManager::msgSvc() const  {return m_msgSvc;}
    
  const MdtIdHelper*  MuonDetectorManager::mdtIdHelper() const {return m_mdtIdHelper;}
  const CscIdHelper*  MuonDetectorManager::cscIdHelper() const {return m_cscIdHelper;}
  const RpcIdHelper*  MuonDetectorManager::rpcIdHelper() const {return m_rpcIdHelper;}
  const TgcIdHelper*  MuonDetectorManager::tgcIdHelper() const {return m_tgcIdHelper;}
  // New Small Wheel
  const sTgcIdHelper*  MuonDetectorManager::stgcIdHelper() const {return m_stgcIdHelper;}
  const MmIdHelper*  MuonDetectorManager::mmIdHelper() const {return m_mmIdHelper;}

  void MuonDetectorManager::set_mdtIdHelper(const MdtIdHelper* idh) {m_mdtIdHelper = idh;}
  void MuonDetectorManager::set_cscIdHelper(const CscIdHelper* idh) {m_cscIdHelper = idh;}
  void MuonDetectorManager::set_tgcIdHelper(const TgcIdHelper* idh) {m_tgcIdHelper = idh;}
  // New Small Wheel
  void MuonDetectorManager::set_stgcIdHelper(const sTgcIdHelper* idh) {m_stgcIdHelper = idh;}
  void MuonDetectorManager::set_mmIdHelper(const MmIdHelper* idh) {m_mmIdHelper = idh;}

  void MuonDetectorManager::setGenericRpcDescriptor(const GenericRPCCache& rc)
  {m_genericRPC.stripSeparation = rc.stripSeparation;
    m_genericRPC.stripPanelThickness = rc.stripPanelThickness;
    m_genericRPC.rpcLayerThickness = rc.rpcLayerThickness;
    m_genericRPC.centralSupPanelThickness = rc.centralSupPanelThickness;
    m_genericRPC.GasGapThickness = rc.GasGapThickness;
    m_genericRPC.frontendBoardWidth = rc.frontendBoardWidth;}

  const GenericRPCCache* MuonDetectorManager::getGenericRpcDescriptor() const
    {return &m_genericRPC;}

  void MuonDetectorManager::setGenericMdtDescriptor(const GenericMDTCache& mc)
  {m_genericMDT.innerRadius = mc.innerRadius;
    m_genericMDT.outerRadius = mc.outerRadius;}

  const GenericMDTCache* MuonDetectorManager::getGenericMdtDescriptor() const
    {return &m_genericMDT;}

  void MuonDetectorManager::setGenericCscDescriptor(const GenericCSCCache& cc)
  {m_genericCSC.dummy1 = cc.dummy1;
    m_genericCSC.dummy2 = cc.dummy2;}

  const GenericCSCCache* MuonDetectorManager::getGenericCscDescriptor() const
    {return &m_genericCSC;}

  void MuonDetectorManager::setGenericTgcDescriptor(const GenericTGCCache& tc)
  {m_genericTGC.frame_h = tc.frame_h;
    m_genericTGC.frame_ab = tc.frame_ab;
    m_genericTGC.nlayers = tc.nlayers;
    for (unsigned int i = 0; i < (tc.materials).size(); i++) {
      m_genericTGC.materials[i] = tc.materials[i];
      m_genericTGC.positions[i] = tc.positions[i];
      m_genericTGC.tck[i] = tc.tck[i];
    }}

  const GenericTGCCache* MuonDetectorManager::getGenericTgcDescriptor() const
    {return &m_genericTGC;}


  void MuonDetectorManager::setMinimalGeoFlag (int flag) {m_minimalgeo = flag;}
  int MuonDetectorManager::MinimalGeoFlag () const {return m_minimalgeo;}
  void MuonDetectorManager::setCutoutsFlag (int flag) {m_includeCutouts = flag;}
  int MuonDetectorManager::IncludeCutoutsFlag () const {return m_includeCutouts;}
  void MuonDetectorManager::setCutoutsBogFlag (int flag) {m_includeCutoutsBog = flag;}
  int MuonDetectorManager::IncludeCutoutsBogFlag () const {return m_includeCutoutsBog;}

  std::string MuonDetectorManager::geometryVersion() const 
    {return m_geometryVersion;}

  void 
    MuonDetectorManager::setGeometryVersion(std::string version) 
  {m_geometryVersion = version;}

  std::string MuonDetectorManager::get_DBMuonVersion() const 
    {return m_DBMuonVersion;}

  void MuonDetectorManager::set_DBMuonVersion(std::string version)
  {m_DBMuonVersion = version;}  

  unsigned int MuonDetectorManager::nMuonStation() const {return m_MuonStationMap.size();} 
  unsigned int MuonDetectorManager::nMdtRE() const  {return m_n_mdtRE;}
  unsigned int MuonDetectorManager::nCscRE() const  {return m_n_cscRE;}    
  unsigned int MuonDetectorManager::nRpcRE() const  {return m_n_rpcRE;}    
  unsigned int MuonDetectorManager::nTgcRE() const  {return m_n_tgcRE;}    
  unsigned int MuonDetectorManager::nsTgcRE() const {return m_n_stgRE;}
  unsigned int MuonDetectorManager::nMMRE() const   {return m_n_mmcRE;}

  unsigned int MuonDetectorManager::nMdtDE() const  {return m_n_mdtDE;}
  unsigned int MuonDetectorManager::nCscDE() const  {return m_n_cscDE;}    
  unsigned int MuonDetectorManager::nRpcDE() const  {return m_n_rpcDE;}    
  unsigned int MuonDetectorManager::nTgcDE() const  {return m_n_tgcDE;}  
  // TODO: New Small Wheel  

  ALineMapContainer* 
    MuonDetectorManager::ALineContainer() const
    {return m_aLineContainer;}

  BLineMapContainer* 
    MuonDetectorManager::BLineContainer() const
    {return m_bLineContainer;}

  CscInternalAlignmentMapContainer*
    MuonDetectorManager::CscInternalAlignmentContainer() const
    {return  m_cscALineContainer;}

  MdtAsBuiltMapContainer* 
    MuonDetectorManager::MdtAsBuiltContainer() const
    {return m_AsBuiltParamsMap;}

  ciALineMap MuonDetectorManager::ALineMapBegin() const {return m_aLineContainer->begin();}
  ciBLineMap MuonDetectorManager::BLineMapBegin() const {return m_bLineContainer->begin();}
  ciALineMap MuonDetectorManager::ALineMapEnd() const  {return m_aLineContainer->end();}
  ciBLineMap MuonDetectorManager::BLineMapEnd() const  {return m_bLineContainer->end();}
  ciCscInternalAlignmentMap MuonDetectorManager::CscALineMapBegin() const {return  m_cscALineContainer->begin();}
  ciCscInternalAlignmentMap MuonDetectorManager::CscALineMapEnd() const {return  m_cscALineContainer->end();}
  ciMdtAsBuiltMap MuonDetectorManager::MdtAsBuiltMapBegin() const {return  m_AsBuiltParamsMap->begin();}
  ciMdtAsBuiltMap MuonDetectorManager::MdtAsBuiltMapEnd() const {return  m_AsBuiltParamsMap->end();}


  void MuonDetectorManager::setCacheFillingFlag(int value){m_cacheFillingFlag = value;}
  void MuonDetectorManager::setCachingFlag(int value){m_cachingFlag = value;}
  int  MuonDetectorManager::cacheFillingFlag() const {return m_cacheFillingFlag;}
  int  MuonDetectorManager::cachingFlag() const {return m_cachingFlag;}

  const MdtReadoutElement* 
    MuonDetectorManager::getMdtReadoutElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if ( id >= MdtRElMaxHash) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	  log <<MSG::WARNING<<" try to getMdtReadoutElement with hashId "
	      <<(unsigned int)id<<" outside range 0-"<<MdtRElMaxHash-1<<endmsg;
	  return 0;
	}
#endif 
      return m_mdtArrayByHash[id];
    }

  const RpcReadoutElement* 
    MuonDetectorManager::getRpcReadoutElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if (id >= RpcRElMaxHash) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	  log <<MSG::WARNING<<" try to getRpcReadoutElement with hashId "	
	      <<(unsigned int)id<<" outside range 0-"<<RpcRElMaxHash-1<<endmsg;
	  return 0;
	}
#endif 
      return  m_rpcArrayByHash[id];
    }

  const TgcReadoutElement* 
    MuonDetectorManager::getTgcReadoutElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if (id >= TgcRElMaxHash) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	  log <<MSG::WARNING<<" try to getTgcReadoutElement with hashId "
	      <<(unsigned int)id<<" outside range 0-"<<TgcRElMaxHash-1<<endmsg;
	  return 0;
	}
#endif 
      return m_tgcArrayByHash[id];
    }

  const CscReadoutElement* 
    MuonDetectorManager::getCscReadoutElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if (id >= CscRElMaxHash)
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	  log <<MSG::WARNING<<" try to getCscReadoutElement with hashId "
	      <<(unsigned int)id<<" outside range 0-"<<CscRElMaxHash-1<<endmsg;
	  return 0;
	}
#endif
      return m_cscArrayByHash[id];
    }
    
  const MdtDetectorElement* 
    MuonDetectorManager::getMdtDetectorElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if ( id >= MdtDetElMaxHash) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");	
	  log <<MSG::WARNING<<" try to getMdtDetectorElement with hashId "	
	      <<(unsigned int)id<<" outside range 0-"<<MdtDetElMaxHash-1<<endmsg;
	  return 0;
	}
#endif 
      return m_mdtDEArray[id];
    }

  const TgcDetectorElement*
    MuonDetectorManager::getTgcDetectorElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if ( id >= TgcDetElMaxHash) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	  log <<MSG::WARNING<<" try to getTgcDetectorElement with hashId "
	      <<(unsigned int)id<<" outside range 0-"<<TgcDetElMaxHash-1<<endmsg;
	  return 0;
	}
#endif 
      return m_tgcDEArray[id];
    }

  const CscDetectorElement*
    MuonDetectorManager::getCscDetectorElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if ( id >= CscDetElMaxHash) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	  log <<MSG::WARNING<<" try to getCscDetectorElement with hashId "
	      <<(unsigned int)id<<" outside range 0-"<<CscDetElMaxHash-1<<endmsg;
	  return 0;
	}
#endif 
      return m_cscDEArray[id];
    }

  const RpcDetectorElement*
    MuonDetectorManager::getRpcDetectorElement(IdentifierHash id) const
    {
#ifndef NDEBUG
      if ( id >= RpcDetElMaxHash) 
	{
	  MsgStream log(m_msgSvc, "MGM::MuonDetectorManager");
	  log <<MSG::WARNING<<" try to getRpcDetectorElement with hashId "
	      <<(unsigned int)id<<" outside range 0-"<<RpcDetElMaxHash-1<<endmsg;
	  return 0;
	}
#endif 
      return m_rpcDEArray[id];
    }
// TODO: New Small Wheel (missing all the above functions)
    
} // namespace MuonGM

#ifndef GAUDI_NEUTRAL
namespace MuonGM {
  class MuonDetectorManager;
}
CLASS_DEF(MuonGM::MuonDetectorManager, 4500, 1)
#endif

#endif

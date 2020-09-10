/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDetectorManager_H
#define MuonDetectorManager_H

#include "GaudiKernel/StatusCode.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
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

typedef ALineMapContainer::const_iterator ciALineMap;
typedef BLineMapContainer::const_iterator ciBLineMap;
typedef CscInternalAlignmentMapContainer::const_iterator ciCscInternalAlignmentMap;
typedef MdtAsBuiltMapContainer::const_iterator ciMdtAsBuiltMap;

namespace MuonGM {

  class CscReadoutElement;
  class TgcReadoutElement;
  class RpcReadoutElement;
  class MdtReadoutElement;
  class sTgcReadoutElement;
  class MMReadoutElement;
  class MuonClusterReadoutElement;
  class MuonStation;
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
    void addMdtReadoutElement (MdtReadoutElement*, Identifier);//!< store the MdtReadoutElement using as "key" the identifier
    void addRpcReadoutElement (RpcReadoutElement*, Identifier);//!< store the RpcReadoutElement using as "key" the identifier
    void addTgcReadoutElement (TgcReadoutElement*, Identifier);//!< store the TgcReadoutElement using as "key" the identifier
    void addCscReadoutElement (CscReadoutElement*, Identifier);//!< store the CscReadoutElement using as "key" the identifier
    void addsTgcReadoutElement (sTgcReadoutElement*, Identifier);//!< store the CscReadoutElement using as "key" the identifier
    void addMMReadoutElement (MMReadoutElement*, Identifier);//!< store the CscReadoutElement using as "key" the identifier
    void addsTgcReadoutElement_withIdFields (sTgcReadoutElement*, int isSmall, int stEta, int stPhi, int ml);//!< store the sTgcReadoutElement using as "key" the identifier
    void addMMReadoutElement_withIdFields   (MMReadoutElement*, int isSmall, int stEta, int stPhi, int ml);//!< store the MMReadoutElement using as "key" the identifier

    // storeTgcReadoutParams
    void storeTgcReadoutParams(std::unique_ptr<const TgcReadoutParams> x);

    // storeCscInternalAlignmentParams
    void storeCscInternalAlignmentParams(const CscInternalAlignmentPar& x);

    void storeMdtAsBuiltParams(const MdtAsBuiltPar& x);

    // access to Readout Elements
    const MdtReadoutElement* getMdtReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const RpcReadoutElement* getRpcReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const TgcReadoutElement* getTgcReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const CscReadoutElement* getCscReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const MMReadoutElement*  getMMReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)
    const sTgcReadoutElement* getsTgcReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)

    const MuonClusterReadoutElement* getMuonClusterReadoutElement(Identifier) const;//!< access via extended identifier (requires unpacking)

    const MdtReadoutElement* getMdtReadoutElement(int i1, int i2, int i3, int i4) const;
    MdtReadoutElement* getMdtReadoutElement(int i1, int i2, int i3, int i4);
    const sTgcReadoutElement* getsTgcReadoutElement(int i1, int i2, int i3, int i4) const;
    const MMReadoutElement* getMMReadoutElement(int i1, int i2, int i3, int i4) const;
    const TgcReadoutElement* getTgcReadoutElement(int i1, int i2, int i3) const;
    TgcReadoutElement* getTgcReadoutElement(int i1, int i2, int i3);
    const CscReadoutElement* getCscReadoutElement(int i1, int i2, int i3, int i4) const;
    CscReadoutElement* getCscReadoutElement(int i1, int i2, int i3, int i4);

    const MdtReadoutElement* getMdtRElement_fromIdFields(int i1, int i2, int i3, int i4) const;
    //!< access via extended identifier field (no unpacking)

    const sTgcReadoutElement* getsTgcRElement_fromIdFields(int isSmall, int stEta, int stPhi, int ml) const;
    //!< access via extended identifier field (no unpacking)

    const MMReadoutElement* getMMRElement_fromIdFields(int isSmall, int stEta, int stPhi, int ml) const;
    //!< access via extended identifier field (no unpacking)

    const TgcReadoutElement* getTgcRElement_fromIdFields(int i1, int i2, int i3) const;
    //!< access via extended identifier field (no unpacking)

    const CscReadoutElement* getCscRElement_fromIdFields(int i1, int i2, int i3, int i4) const;
    //!< access via extended identifier field (no unpacking)

    const MdtReadoutElement* getMdtReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id
    const RpcReadoutElement* getRpcReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id
    const TgcReadoutElement* getTgcReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id
    const CscReadoutElement* getCscReadoutElement(IdentifierHash id)  const;//!< access via detector-element hash id

    const MdtDetectorElement* getMdtDetectorElement(IdentifierHash id)  const;//!< access via data-collection hash id

    // access to Detector Elements (via hash id ) - NOTICE Detector Elements have the granularity of the EDM digit collections
    const RpcDetectorElement* getRpcDetectorElement(IdentifierHash id) const;
    const TgcDetectorElement* getTgcDetectorElement(IdentifierHash id) const;
    const CscDetectorElement* getCscDetectorElement(IdentifierHash id) const;

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

    // Geometry versioning
    inline std::string geometryVersion() const; //!< it can be Rome-Initial or P03, or ... it's the name of the layout
    inline void setGeometryVersion(std::string version);
    inline std::string get_DBMuonVersion() const;//!< the name of the MuonSpectrometer tag (in the geometry DB) actually accessed
    inline void set_DBMuonVersion(std::string );

    // Access to identifier helpers
    inline const MdtIdHelper* mdtIdHelper() const;
    inline const CscIdHelper* cscIdHelper() const;
    inline const RpcIdHelper* rpcIdHelper() const;
    inline const TgcIdHelper* tgcIdHelper() const;
    inline const sTgcIdHelper* stgcIdHelper() const;
    inline const MmIdHelper* mmIdHelper() const;

    // Setting the identifier helpers
    inline void set_mdtIdHelper(const MdtIdHelper* idh);
    inline void set_cscIdHelper(const CscIdHelper* idh);
    inline void set_rpcIdHelper(const RpcIdHelper* idh);
    inline void set_tgcIdHelper(const TgcIdHelper* idh);
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

    inline void setCachingFlag(int value);
    inline int cachingFlag() const;
    inline void setCacheFillingFlag(int value);
    inline int cacheFillingFlag() const;

    inline void setNSWABLinesAsciiSideA(const std::string& str);
    inline void setNSWABLinesAsciiSideC(const std::string& str);

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
        NsTgStatEta      = 6, /// 3 x 2 sides (-3,-2,-1 and 1,2,3)
        NsTgStEtaOffset  = 3, /// needed offest to map (-3,-2,-1,1,2,3) to (0,1,2,3,4,5)
        NsTgStatPhi      = 16, // large and small sector together
        NsTgChamberLayer = 2
      };
    enum mmGMRanges
      {
        NMMcStatEta      = 4, /// 2 x 2 sides (-2,-1 and 1,2)
        NMMcStEtaOffset  = 2, /// needed offest to map (-2,-1,1,2) to (0,1,2,3)
        NMMcStatPhi      = 16, // large and small sector together
        NMMcChamberLayer = 2
      };

    // Add a MuonStation to the list
    void addMuonStation(MuonStation* mst);
    const MuonStation* getMuonStation (std::string stName, int eta, int phi) const;
    MuonStation* getMuonStation (std::string stName, int eta, int phi);
    //<! access to the MuonStation by StationName, Jzz, Jff (amdb indices!!!! not stationPhi and Eta)
    std::string muonStationKey(std::string stName, int statEtaIndex, int statPhiIndex) const;

    void clearCache();
    void refreshCache();
    void fillCache();

    void clearMdtCache();
    void clearRpcCache();
    void clearCscCache();
    void clearTgcCache();
    void clearsTgcCache();
    void clearMMCache();

    void refreshMdtCache();
    void refreshRpcCache();
    void refreshCscCache();
    void refreshTgcCache();
    void refreshsTgcCache();
    void refreshMMCache();

    void fillMdtCache();
    void fillRpcCache();
    void fillCscCache();
    void fillTgcCache();
    void fillsTgcCache();
    void fillMMCache();

    inline const ALineMapContainer * ALineContainer() const;
    inline const BLineMapContainer * BLineContainer() const;
    inline const CscInternalAlignmentMapContainer * CscInternalAlignmentContainer() const;
    inline const MdtAsBuiltMapContainer* MdtAsBuiltContainer() const;
    inline ciALineMap ALineMapBegin() const;
    inline ciBLineMap BLineMapBegin() const;
    inline ciALineMap ALineMapEnd() const;
    inline ciBLineMap BLineMapEnd() const;
    inline ciCscInternalAlignmentMap CscALineMapBegin() const;
    inline ciCscInternalAlignmentMap CscALineMapEnd() const;
    inline ciMdtAsBuiltMap MdtAsBuiltMapBegin() const;
    inline ciMdtAsBuiltMap MdtAsBuiltMapEnd() const;
    StatusCode updateAlignment(const ALineMapContainer& a);
    StatusCode updateDeformations(const BLineMapContainer& a);
    StatusCode updateAsBuiltParams(const MdtAsBuiltMapContainer& a);
    StatusCode initCSCInternalAlignmentMap();
    StatusCode updateCSCInternalAlignmentMap(const CscInternalAlignmentMapContainer& cscIntAline);
    void initABlineContainers();

    // get Mdt AsBuilt parameters for chamber specified by Identifier
    const MdtAsBuiltPar* getMdtAsBuiltParams(Identifier id) const;

    int rpcStationName(const int stationIndex) const; // map the RPC station indices (0-NRpcStatType) back to the RpcIdHelper stationNames

  private:
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
    const RpcReadoutElement* getRpcReadoutElement(int i1, int i2, int i3, int i4, int i5) const;
    RpcReadoutElement* getRpcReadoutElement(int i1, int i2, int i3, int i4, int i5);
    const RpcReadoutElement* getRpcRElement_fromIdFields(int i1, int i2, int i3, int i4, int i5, int i6) const;

    void checkRpcReadoutElementIndices(int i1, int i2, int i3, int i4, int i5)const;
    void checkTgcReadoutElementIndices(int i1, int i2, int i3)const;
    void checkCscReadoutElementIndices(int i1, int i2, int i3, int i4)const;
    void checkMdtReadoutElementIndices(int i1, int i2, int i3, int i4)const;

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

    // Geometry versioning
    std::string m_geometryVersion;//generic name of the Layout
    std::string m_DBMuonVersion;  //name of the MuonVersion table-collection in Oracle

    std::string m_NSWABLinesAsciiSideA;
    std::string m_NSWABLinesAsciiSideC;

    // pointers to IdHelpers
    const MdtIdHelper* m_mdtIdHelper;
    const CscIdHelper* m_cscIdHelper;
    const RpcIdHelper* m_rpcIdHelper;
    const TgcIdHelper* m_tgcIdHelper;
    const sTgcIdHelper* m_stgcIdHelper;
    const MmIdHelper* m_mmIdHelper;

    // 115.6 kBytes.
    MdtReadoutElement*   m_mdtArray[NMdtStatType][NMdtStatEta][NMdtStatPhi][NMdtMultilayer];
    CscReadoutElement*   m_cscArray[NCscStatType][NCscStatEta][NCscStatPhi][NCscChamberLayer];
    RpcReadoutElement*   m_rpcArray[NRpcStatType][NRpcStatEta][NRpcStatPhi][NDoubletR][NDoubletZ];
    TgcReadoutElement*   m_tgcArray[NTgcStatType][NTgcStatEta][NTgcStatPhi];
    sTgcReadoutElement*  m_stgArray[NsTgStatEta][NsTgStatPhi][NsTgChamberLayer];
    MMReadoutElement*    m_mmcArray[NMMcStatEta][NMMcStatPhi][NMMcChamberLayer];
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
    unsigned int m_n_stgRE;
    unsigned int m_n_mmcRE;

    unsigned int m_n_mdtDE;
    unsigned int m_n_cscDE;
    unsigned int m_n_rpcDE;
    unsigned int m_n_tgcDE;

    // pointers to the XxxDetectorElements (with granularity a la EDM)
    std::vector<std::unique_ptr<const TgcReadoutParams> > m_TgcReadoutParamsVec;

    MdtDetectorElement* m_mdtDEArray[MdtDetElMaxHash];
    RpcDetectorElement* m_rpcDEArray[RpcDetElMaxHash];
    TgcDetectorElement* m_tgcDEArray[TgcDetElMaxHash];
    CscDetectorElement* m_cscDEArray[CscDetElMaxHash];

    ALineMapContainer m_aLineContainer;
    BLineMapContainer m_bLineContainer;
    CscInternalAlignmentMapContainer m_cscALineContainer;
    MdtAsBuiltMapContainer m_AsBuiltParamsMap;
    /// RPC name caches
    std::map<int,int> m_rpcStatToIdx;
    std::map<int,int> m_rpcIdxToStat;
    

  };

  const MdtIdHelper*  MuonDetectorManager::mdtIdHelper() const {return m_mdtIdHelper;}
  const CscIdHelper*  MuonDetectorManager::cscIdHelper() const {return m_cscIdHelper;}
  const RpcIdHelper*  MuonDetectorManager::rpcIdHelper() const {return m_rpcIdHelper;}
  const TgcIdHelper*  MuonDetectorManager::tgcIdHelper() const {return m_tgcIdHelper;}
  const sTgcIdHelper*  MuonDetectorManager::stgcIdHelper() const {return m_stgcIdHelper;}
  const MmIdHelper*  MuonDetectorManager::mmIdHelper() const {return m_mmIdHelper;}

  void MuonDetectorManager::set_mdtIdHelper(const MdtIdHelper* idh) {m_mdtIdHelper = idh;}
  void MuonDetectorManager::set_cscIdHelper(const CscIdHelper* idh) {m_cscIdHelper = idh;}
  void MuonDetectorManager::set_rpcIdHelper(const RpcIdHelper* idh) {m_rpcIdHelper = idh;}
  void MuonDetectorManager::set_tgcIdHelper(const TgcIdHelper* idh) {m_tgcIdHelper = idh;}
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

  const ALineMapContainer*
    MuonDetectorManager::ALineContainer() const
    {return &m_aLineContainer;}

  const BLineMapContainer*
    MuonDetectorManager::BLineContainer() const
    {return &m_bLineContainer;}

  const CscInternalAlignmentMapContainer*
    MuonDetectorManager::CscInternalAlignmentContainer() const
    {return  &m_cscALineContainer;}

  const MdtAsBuiltMapContainer*
    MuonDetectorManager::MdtAsBuiltContainer() const
    {return &m_AsBuiltParamsMap;}

  ciALineMap MuonDetectorManager::ALineMapBegin() const {return m_aLineContainer.begin();}
  ciBLineMap MuonDetectorManager::BLineMapBegin() const {return m_bLineContainer.begin();}
  ciALineMap MuonDetectorManager::ALineMapEnd() const  {return m_aLineContainer.end();}
  ciBLineMap MuonDetectorManager::BLineMapEnd() const  {return m_bLineContainer.end();}
  ciCscInternalAlignmentMap MuonDetectorManager::CscALineMapBegin() const {return  m_cscALineContainer.begin();}
  ciCscInternalAlignmentMap MuonDetectorManager::CscALineMapEnd() const {return  m_cscALineContainer.end();}
  ciMdtAsBuiltMap MuonDetectorManager::MdtAsBuiltMapBegin() const {return  m_AsBuiltParamsMap.begin();}
  ciMdtAsBuiltMap MuonDetectorManager::MdtAsBuiltMapEnd() const {return  m_AsBuiltParamsMap.end();}


  void MuonDetectorManager::setCacheFillingFlag(int value){m_cacheFillingFlag = value;}
  void MuonDetectorManager::setCachingFlag(int value){m_cachingFlag = value;}
  int  MuonDetectorManager::cacheFillingFlag() const {return m_cacheFillingFlag;}
  int  MuonDetectorManager::cachingFlag() const {return m_cachingFlag;}
  void MuonDetectorManager::setNSWABLinesAsciiSideA(const std::string& str) {m_NSWABLinesAsciiSideA = str;}
  void MuonDetectorManager::setNSWABLinesAsciiSideC(const std::string& str) {m_NSWABLinesAsciiSideC = str;}


} // namespace MuonGM

#ifndef GAUDI_NEUTRAL
namespace MuonGM {
  class MuonDetectorManager;
}
CLASS_DEF(MuonGM::MuonDetectorManager, 4500, 1)
CLASS_DEF(CondCont<MuonGM::MuonDetectorManager>, 205781622, 0)
#endif

#endif

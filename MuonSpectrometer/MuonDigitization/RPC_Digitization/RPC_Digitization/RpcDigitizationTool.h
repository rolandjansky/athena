/* -*- C++ -*- */

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_DIGITIZATIONTOOL_H
#define RPC_DIGITIZATIONTOOL_H
/** @class RpcDigitizationTool

    @section RPC_DigitizerDetails Class methods and properties


    In the initialize() method, the PileUpMerge and StoreGate services are initialized,
    and a pointer to an instance of the class MuonDetectorManager is retrieved from
    the detector store and used to obtain an rpcIdHelper.
    The ASCII file G4RPC_Digitizer.txt is read and its contents are used by the
    algorithm in order to simulate clusters.
    Random numbers are obtained in the code from a dedicated stream via
    AtRndmSvc, which is also initialized in the initialize() method.
    The execute() has responsibility for steering the digitization/cluster
    simulation process. A loop over the RPCHits is performed, converting each SimID to OID.
    The method PhysicalClusterSize
    is hence called, which creates a cluster of size 1 or two according to the impact point
    of the particle along the strip. The final size of the cluster is decided by the
    method TurnOnStrips.
    The last step in the creation of the digitization is the calculation of the
    propagation time of the electrical signal along the strip length. This is done in
    the PropagationTime method.
    In the hit collections coming from the RPCSensitiveDetector, it sometimes happen that
    many hits are produced by the same crossing particle, which are very close both in
    space and time. This is related to ionization and production of secondaries in the gas,
    and it is thus safe, and also recommended, to eliminate these multiple hits before
    proceeding to reconstruction. The execute() method provides this functionality
    using a dead time: once a hit is found on a given strip, every other hit coming from
    the same strip before the dead time is ignored.


*/


#include "PileUpTools/PileUpToolBase.h"

#include "AthenaKernel/IAthRNGSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "MuonSimEvent/RPCSimHit.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "HitManagement/TimedHitCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"
#include "MuonCondData/RpcCondDbData.h"

#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM

//Outputs
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include <vector>
#include <map>

class RpcHitIdHelper;
class RpcCondDbData;

class RpcIdHelper;
class ITagInfoMgr;

namespace MuonGM{
  class MuonDetectorManager;
}
namespace CLHEP{
  class HepRandomEngine;
}

class RpcDigitizationTool : public PileUpToolBase {

public:
  RpcDigitizationTool(const std::string& type, const std::string& name, const IInterface* pIID);

  /** Initialize */
  virtual StatusCode initialize() override final;

  /** When being run from PileUpToolsAlgs, this method is called at the start of
      the subevts loop. Not able to access SubEvents */
  virtual StatusCode prepareEvent(const EventContext& ctx, const unsigned int /*nInputEvents*/) override final;

  /** When being run from PileUpToolsAlgs, this method is called for each active
      bunch-crossing to process current SubEvents bunchXing is in ns */

  virtual StatusCode processBunchXing(
                                      int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents
                                      ) override final;


  /** When being run from PileUpToolsAlgs, this method is called at the end of
      the subevts loop. Not (necessarily) able to access SubEvents */
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

  /** alternative interface which uses the PileUpMergeSvc to obtain
      all the required SubEvents. */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

private:

  /** Get next event and extract collection of hit collections: */
  StatusCode getNextEvent(const EventContext& ctx);
  /** Digitization functionality shared with RPC_PileUpTool */
  StatusCode doDigitization(const EventContext& ctx, RpcDigitContainer* digitContainer, MuonSimDataCollection* sdoContainer);
  /** */
  StatusCode fillTagInfo()    const;
  /** */
  long long int PackMCTruth(float proptime, float tof, float posx, float posz);
  /** */
  void UnPackMCTruth(double theWord, float& proptime, float& tof, float& posy, float& posz);
  /** Read parameters for cluster simulation.
      This method reads the file specified by m_paraFile and
      uses the experimental distributions it contains to
      reproduce proper cluster sizes. */
  StatusCode readParameters();
  /** Cluster simulation: first step.
      The impact point of the particle across the strip is used
      to decide whether the cluster size should be 1 or 2 */
  std::vector<int> PhysicalClusterSize(const EventContext& ctx, const Identifier* id, const RPCSimHit* theHit, CLHEP::HepRandomEngine* rndmEngine);
  /** Cluster simulation: second step.
      Additional strips are turned on in order to reproduce the
      observed cluster size distribution */
  std::vector<int> TurnOnStrips(std::vector<int> pcs, const Identifier* id, CLHEP::HepRandomEngine* rndmEngine);
  /** Calculates the propagation time along the strip */
  double PropagationTime(const Identifier* id, const Amg::Vector3D pos);
  double PropagationTimeNew(const Identifier* id, const Amg::Vector3D globPos);
  /** Calculates the position of the hit wrt to the strip panel
      this transformation is needed since the impact point comes from the SD
      int he gas gap's reference frame. */
  Gaudi::Property<double> m_UncorrJitter {this, "UncorrJitter", 1.5 , "jitter uncorrelated between eta and phi"};
  Gaudi::Property<double> m_CorrJitter   {this, "CorrJitter", 0.0 , "jitter correlated between eta and phi"};
  Amg::Vector3D posInPanel(const Identifier* id, const Amg::Vector3D posInGap);
  /** adjust strip numbering according to standard OIDs **/
  int adjustStripNumber(const Identifier* id,int nstrip);
  /** Accounts for rotation of chambers.
      The impact point's coordinates are given by the RPCSensitiveDetector wrt
      the gas gap reference system but RPC chambers are placed in the spectrometer
      after a certain number of rotations. This method applies the necessary
      modifications to axis orientation, in order to obtain the correct strip number */
  Amg::Vector3D adjustPosition(const Identifier* id, const Amg::Vector3D hitPos);
  /** calculates the strip number and returns the position along the strip*/
  int findStripNumber(Amg::Vector3D gasGapPos, Identifier stripPanelId, double& posinstrip);

  //pile-up
  bool outsideWindow(double time) const;
  Gaudi::Property<double> m_timeWindowLowerOffset{this, "WindowLowerOffset", -100. , "digitization window lower limit"};
  Gaudi::Property<double> m_timeWindowUpperOffset{this, "WindowUpperOffset", +150. , "digitization window lower limit"};

  /** Average calibration methods and parameters */
  StatusCode  PrintCalibrationVector();
  /** Evaluate detection efficiency */
  StatusCode DetectionEfficiency(const EventContext& ctx, const Identifier* ideta, const Identifier* idphi, bool& undefinedPhiStripStatus, CLHEP::HepRandomEngine* rndmEngine, const HepMcParticleLink& trkParticle);
  double FCPEfficiency(const HepMC::GenParticle* genParticle);
  /** */
  int ClusterSizeEvaluation(const EventContext& ctx, const Identifier* id, float xstripnorm, CLHEP::HepRandomEngine* rndmEngine);
  /** CoolDB */
  StatusCode DumpRPCCalibFromCoolDB(const EventContext& ctx);

  const MuonGM::MuonDetectorManager*  m_GMmgr{};
  const RpcIdHelper*          m_idHelper{};
  RpcHitIdHelper*             m_muonHelper{};
  std::list<RPCSimHitCollection*> m_RPCHitCollList;
  TimedHitCollection<RPCSimHit>* m_thpcRPC{};
  SG::ReadCondHandleKey<RpcCondDbData> m_readKey{this, "ReadKey", "RpcCondDbData", "Key of RpcCondDbData"};
  std::map<Identifier,std::vector<MuonSimData::Deposit> > m_sdo_tmp_map;
  Gaudi::Property<int>            m_deadTime{this, "DeadTime", 100. , "dead time"};
  Gaudi::Property<bool> m_patch_for_rpc_time{this, "PatchForRpcTime", false, ""};
  Gaudi::Property<double>   m_rpc_time_shift{this, "PatchForRpcTimeShift", 12.5,
      "shift rpc digit time to match hardware time calibration: Zmumu muons are at the center of BC0, i.e. at 12.5ns+BC0shift w.r.t. RPC readout (BC0shift=2x3.125)"};
  Gaudi::Property<std::string>    m_paraFile{this, "Parameters", "G4RPC_Digitizer.txt",
      "ascii file with cluster simulation parameters"};
  std::vector<double> m_csPara; // cluster simulation parameters
  std::vector<double> m_rgausPara;
  std::vector<double> m_fgausPara;
  std::vector<double> m_constPara;
  double         m_cs3Para{0};
  std::vector<double> m_cs4Para;
  Gaudi::Property<bool> m_validationSetup{this, "ValidationSetup", false, ""};
  Gaudi::Property<bool> m_includePileUpTruth{this, "IncludePileUpTruth", true, "pileup truth veto"};

  Gaudi::Property<bool>  m_turnON_efficiency{this, "turnON_efficiency", true, ""};
  Gaudi::Property<bool>  m_kill_deadstrips{this, "KillDeadStrips", false, ""};//gabriele
  Gaudi::Property<bool>  m_applyEffThreshold{this, "ApplyEfficiencyThreshold", false, ""};//stefania
  Gaudi::Property<float> m_Minimum_efficiency{this, "Minimum_efficiency", 0.5, ""};//gabriele
  Gaudi::Property<bool>  m_turnON_clustersize{this, "turnON_clustersize", true, ""};
  Gaudi::Property<int>   m_testbeam_clustersize{this, "testbeam_clustersize", 1, ""};
  Gaudi::Property<int>   m_FirstClusterSizeInTail{this, "FirstClusterSizeInTail", 3, ""};

  Gaudi::Property< std::vector<float> > m_PhiAndEtaEff_A{this, "PhiAndEtaEff_A", {} , ""};
  Gaudi::Property< std::vector<float> > m_OnlyPhiEff_A  {this, "OnlyPhiEff_A"  , {} , ""};
  Gaudi::Property< std::vector<float> > m_OnlyEtaEff_A  {this, "OnlyEtaEff_A"  , {} , ""};
  Gaudi::Property< std::vector<float> > m_PhiAndEtaEff_C{this, "PhiAndEtaEff_C", {} , ""};
  Gaudi::Property< std::vector<float> > m_OnlyPhiEff_C  {this, "OnlyPhiEff_C"  , {} , ""};
  Gaudi::Property< std::vector<float> > m_OnlyEtaEff_C  {this, "OnlyEtaEff_C"  , {} , ""};

  Gaudi::Property< std::vector<double> > m_FracClusterSize1_A   {this, "FracClusterSize1_A"   , {} , ""};
  Gaudi::Property< std::vector<double> > m_FracClusterSize2_A   {this, "FracClusterSize2_A"   , {} , ""};
  Gaudi::Property< std::vector<double> > m_FracClusterSizeTail_A{this, "FracClusterSizeTail_A", {} , ""};
  Gaudi::Property< std::vector<double> > m_MeanClusterSizeTail_A{this, "MeanClusterSizeTail_A", {} , ""};

  Gaudi::Property< std::vector<double> > m_FracClusterSize1_C   {this, "FracClusterSize1_C"   , {} , ""};
  Gaudi::Property< std::vector<double> > m_FracClusterSize2_C   {this, "FracClusterSize2_C"   , {} , ""};
  Gaudi::Property< std::vector<double> > m_FracClusterSizeTail_C{this, "FracClusterSizeTail_C", {} , ""};
  Gaudi::Property< std::vector<double> > m_MeanClusterSizeTail_C{this, "MeanClusterSizeTail_C", {} , ""};

  std::vector<Identifier> m_DeadStripPanel;

  bool m_SetPhiOn{false};
  bool m_SetEtaOn{false};
  Gaudi::Property<bool> m_muonOnlySDOs{this, "MuonOnlySDOs", true, ""};
protected:
  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc",
      "Pile up service"};
  Gaudi::Property<bool> m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<RPCSimHitCollection> m_hitsContainerKey{this, "InputObjectName", "RPC_Hits",
      "name of the input object"};
  std::string m_inputHitCollectionName{""};
  SG::WriteHandleKey<RpcDigitContainer> m_outputDigitCollectionKey{this, "OutputObjectName", "RPC_DIGITS",
      "WriteHandleKey for Output RpcDigitContainer"}; // name of the output digits
  SG::WriteHandleKey<MuonSimDataCollection> m_outputSDO_CollectionKey{this, "OutputSDOName", "RPC_SDO",
      "WriteHandleKey for Output MuonSimDataCollection"}; // name of the output SDOs

  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};      // Random number service

  ITagInfoMgr *m_tagInfoMgr{}            ; // Tag Info Manager
  Gaudi::Property<std::string> m_RPC_TimeSchema{this, "RPC_TimeSchema", "RPC_TimeSchema",
      "Tag info name of Rpc Time Info"};
  Gaudi::Property<bool>        m_sdoAreOnlyDigits{this, "RPCSDOareRPCDigits", true,
      "decide is SDO deposits are saved for all G4 hits or only for those accepted as digits"};
  Gaudi::Property<bool>        m_PrintCalibrationVector      {this, "PrintCalibrationVector", false,
      "Printout Eff and CS average parameters"};
  Gaudi::Property<bool>        m_Efficiency_fromCOOL         {this, "Efficiency_fromCOOL", false,
      "Read efficiency from CoolDB"};
  Gaudi::Property<bool>        m_EfficiencyPatchForBMShighEta{this, "EfficiencyPatchForBMShighEta", false,
      "special patch to be true only when m_Efficiency_fromCOOL=true and /RPC/DQMF/ELEMENT_STATUS tag is RPCDQMFElementStatus_2012_Jaunuary_26"};
  Gaudi::Property<bool>        m_ClusterSize_fromCOOL        {this, "ClusterSize_fromCOOL", false,
      "Read cluster size from CoolDB"};
  Gaudi::Property<bool>        m_ClusterSize1_2uncorr        {this, "ClusterSize1_2uncorr", false,
      "Cluster size 1 and 2 not correlated to track position"};
  Gaudi::Property<bool>        m_BOG_BOF_DoubletR2_OFF       {this, "Force_BOG_BOF_DoubletR2_OFF", false,
      "Turn-off BOG and BOF with DoubletR=2"};
  Gaudi::Property<bool>        m_ignoreRunDepConfig          {this, "IgnoreRunDependentConfig", false,
      "true if we want to force the RUN1/RUN2 dependent options"};
  Gaudi::Property<bool>        m_PanelId_OFF_fromlist        {this, "PanelId_OFF_fromlist", false,
      "Turn-off PanelId from file m_FileName_DeadPanels"};
  Gaudi::Property<std::string> m_FileName_DeadPanels         {this, "FileName_DeadPanels", "PermanentDeadPanels.txt",
      "File with Dead panel PanelId list"};
  Gaudi::Property<bool>        m_PanelId_OK_fromlist         {this, "PanelId_OK_fromlist", false,
      "Turn-on PanelId from file m_FileName_GoodPanels"};
  Gaudi::Property<std::string> m_FileName_GoodPanels         {this, "FileName_GoodPanels", "PermanentGoodPanels.txt",
      " File with Good panel PanelId list"};

  std::map<Identifier , int> m_DeadPanel_fromlist;
  std::map<Identifier , int> m_GoodPanel_fromlist;

  Gaudi::Property<bool>      m_RPCInfoFromDb     {this, "RPCInfoFromDb", false, ""};
  Gaudi::Property<bool>      m_DumpFromDbFirst   {this, "DumpFromDbFirst", false, ""};
  Gaudi::Property<float>     m_CutMaxClusterSize {this, "CutMaxClusterSize", 5.0, ""};
  Gaudi::Property<int>       m_CutProjectedTracks{this, "CutProjectedTracks", 100, ""};

};

inline
bool RpcDigitizationTool::outsideWindow(double time) const {
  return time < m_timeWindowLowerOffset || time > m_timeWindowUpperOffset;
}
#endif // RpcDigitizationTool

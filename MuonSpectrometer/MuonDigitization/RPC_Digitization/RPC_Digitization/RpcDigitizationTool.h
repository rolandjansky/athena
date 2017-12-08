/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "MuonSimEvent/RPCSimHit.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "HitManagement/TimedHitCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM


#include <vector>
#include <map>

class RpcHitIdHelper;

class RpcDigitContainer;
class RpcIdHelper;
class PileUpMergeSvc;
class IAtRndmGenSvc;
class MsgStream;
class ITagInfoMgr;
 
class IRPCConditionsSvc;

namespace MuonGM{
  class MuonDetectorManager;
}
namespace CLHEP{
  class HepRandomEngine;
}

class RpcDigitizationTool : virtual public IMuonDigitizationTool, public PileUpToolBase {

public:
  RpcDigitizationTool(const std::string& type, const std::string& name, const IInterface* pIID);

  /** Initialize */
    virtual StatusCode initialize() override final;

  /** When being run from PileUpToolsAlgs, this method is called at the start of 
      the subevts loop. Not able to access SubEvents */
  virtual StatusCode prepareEvent(const unsigned int /*nInputEvents*/) override final;

  /** When being run from PileUpToolsAlgs, this method is called for each active 
      bunch-crossing to process current SubEvents bunchXing is in ns */

  virtual  StatusCode processBunchXing(
                                    int bunchXing,
                                    SubEventIterator bSubEvents,
                                    SubEventIterator eSubEvents
                                       ) override final;


  /** When being run from PileUpToolsAlgs, this method is called at the end of 
      the subevts loop. Not (necessarily) able to access SubEvents */
  virtual StatusCode mergeEvent() override final;

  /** alternative interface which uses the PileUpMergeSvc to obtain
  all the required SubEvents. */
  virtual StatusCode processAllSubEvents() override final;

  /** When being run from RPC_Digitizer, this method is called during
      the event loop. Just calls processAllSubEvents - leaving for
      back-compatibility (IMuonDigitizationTool) */
  virtual StatusCode digitize() override final;

  /** Finalize */
  virtual StatusCode finalize() override final;

  /** accessors */
  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used 

private:

  RpcDigitContainer*          m_digitContainer;
  MuonSimDataCollection*      m_sdoContainer;
  const MuonGM::MuonDetectorManager*  m_GMmgr;
  const RpcIdHelper*          m_idHelper;
  RpcHitIdHelper*             m_muonHelper;
  std::list<RPCSimHitCollection*> m_RPCHitCollList;
  TimedHitCollection<RPCSimHit>* m_thpcRPC;
  ServiceHandle<IRPCConditionsSvc> m_rSummarySvc;
  std::map<Identifier,std::vector<MuonSimData::Deposit> > m_sdo_tmp_map;
  int                         m_deadTime;
  bool                        m_patch_for_rpc_time;
  double                      m_rpc_time_shift    ;
  std::string    m_paraFile; // ascii file with cluster simulation parameters
  std::vector<double> m_csPara; // cluster simulation parameters
  std::vector<double> m_rgausPara;
  std::vector<double> m_fgausPara;
  std::vector<double> m_constPara;
  double         m_cs3Para;
  std::vector<double> m_cs4Para;
  bool                m_validationSetup;

  /** Get next event and extract collection of hit collections: */
  StatusCode getNextEvent();
  /** Digitization functionality shared with RPC_PileUpTool */
  StatusCode doDigitization();
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
  std::vector<int> PhysicalClusterSize(const Identifier* id, const RPCSimHit* theHit );
  /** Cluster simulation: second step.
      Additional strips are turned on in order to reproduce the
      observed cluster size distribution */
  std::vector<int> TurnOnStrips(std::vector<int> pcs, const Identifier* id);
  /** Calculates the propagation time along the strip */
  double PropagationTime(const Identifier* id, const Amg::Vector3D pos);
  double PropagationTimeNew(const Identifier* id, const Amg::Vector3D globPos);
  /** Calculates the position of the hit wrt to the strip panel
      this transformation is needed since the impact point comes from the SD
      int he gas gap's reference frame. */ 
  double m_UncorrJitter ; //time jitter uncorrelated between eta and phi
  double m_CorrJitter   ; //time jitter correlated between eta and phi
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
  double m_timeWindowLowerOffset;
  double m_timeWindowUpperOffset;

  /** Average calibration methods and parameters */
  StatusCode  PrintCalibrationVector();
  /** Evaluate detection efficiency */
  StatusCode DetectionEfficiency(const Identifier* ideta, const Identifier* idphi, bool& undefinedPhiStripStatus);
  /** */
  int ClusterSizeEvaluation(const Identifier* id, float xstripnorm);
  /** CoolDB */
  StatusCode DumpRPCCalibFromCoolDB();
        
  bool  m_turnON_efficiency      ;
  bool  m_kill_deadstrips        ;//gabriele
  bool  m_applyEffThreshold      ;//stefania
  float m_Minimum_efficiency     ;//gabriele
  bool  m_turnON_clustersize     ;
  int   m_testbeam_clustersize   ;
  int   m_FirstClusterSizeInTail ;
    
  std::vector<float> m_PhiAndEtaEff_A;
  std::vector<float> m_OnlyPhiEff_A  ; 
  std::vector<float> m_OnlyEtaEff_A  ;   
     
  std::vector<float> m_PhiAndEtaEff_C;
  std::vector<float> m_OnlyPhiEff_C  ; 
  std::vector<float> m_OnlyEtaEff_C  ;
          
  std::vector<double> m_FracClusterSize1_A   ;
  std::vector<double> m_FracClusterSize2_A   ;  
  std::vector<double> m_FracClusterSizeTail_A;   
  std::vector<double> m_MeanClusterSizeTail_A;
          
  std::vector<double> m_FracClusterSize1_C   ;
  std::vector<double> m_FracClusterSize2_C   ;  
  std::vector<double> m_FracClusterSizeTail_C;   
  std::vector<double> m_MeanClusterSizeTail_C;

  std::vector<Identifier> m_DeadStripPanel; 
        
  bool m_SetPhiOn ;    
  bool m_SetEtaOn ;
  bool m_muonOnlySDOs;
protected:  
  PileUpMergeSvc *m_mergeSvc; // Pile up service
  std::string m_inputHitCollectionName; // name of the input objects
  std::string m_outputDigitCollectionName; // name of the output digits
  std::string m_outputSDO_CollectionName; // name of the output SDOs

  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;      // Random number service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine

  ITagInfoMgr *m_tagInfoMgr            ; // Tag Info Manager
  std::string m_RPC_TimeSchema         ; // Tag info name of Rpc Time Info
  bool	      m_sdoAreOnlyDigits       ; // decide is SDO deposits are saved for all G4 hits or only for those accepted as digits
  bool	      m_PrintCalibrationVector ; // Printout Eff and CS average parameters
  bool	      m_Efficiency_fromCOOL    ; // Read efficiency from CoolDB
  bool	      m_EfficiencyPatchForBMShighEta    ; //  special patch to be true only when m_Efficiency_fromCOOL=true and /RPC/DQMF/ELEMENT_STATUS tag is RPCDQMFElementStatus_2012_Jaunuary_26
  bool	      m_ClusterSize_fromCOOL   ; // Read cluster size from CoolDB
  bool	      m_ClusterSize1_2uncorr   ; // Cluster size 1 and 2 not correlated to track position
  bool	      m_BOG_BOF_DoubletR2_OFF  ; // Turn-off BOG and BOF with DoubletR=2
  bool        m_ignoreRunDepConfig     ; // true if we want to force the RUN1/RUN2 dependent options 
  bool	      m_PanelId_OFF_fromlist   ; // Turn-off PanelId from file m_FileName_DeadPanels
  std::string m_FileName_DeadPanels    ; // File with Dead panel PanelId list 
  bool	      m_PanelId_OK_fromlist    ; // Turn-on PanelId from file m_FileName_GoodPanels
  std::string m_FileName_GoodPanels    ; // File with Good panel PanelId list 

  std::map<Identifier , int> m_DeadPanel_fromlist;
  std::map<Identifier , int> m_GoodPanel_fromlist;

  bool	      m_RPCInfoFromDb     ;
  bool	      m_DumpFromDbFirst   ; 
  float       m_CutMaxClusterSize ;       
  int	      m_CutProjectedTracks;

};

inline 
bool RpcDigitizationTool::outsideWindow(double time) const {
  return time < m_timeWindowLowerOffset || time > m_timeWindowUpperOffset;
}
#endif // RpcDigitizationTool

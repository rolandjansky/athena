#ifndef HTT_RAWNTUPLEWRAPPERALG_H
#define HTT_RAWNTUPLEWRAPPERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTInput/IHTTEventInputHeaderTool.h"
#include "TrigHTTObjects/HTTTruthTrack.h"

class TFile;
class TTree;
class HTTEventInputHeader;
class HTTHit;
class HTTTruthTrack;

class HTTRawNtupleWrapperAlg : public AthAlgorithm {
  
public:
  HTTRawNtupleWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTRawNtupleWrapperAlg () = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute()    override;
  virtual StatusCode finalize()   override;

private:
  // configuration parameters
  ToolHandle<IHTTEventInputHeaderTool>    m_hitInputTool  { this, "InputTool",  "HTTSGToRawHitsTool/HTTSGToRawHitsTool", "Input Tool" };
  StringProperty m_outpath     {this, "OutFileName", "httsim_smartwrapper.root", "output path"};
  BooleanProperty m_getOffline {this, "GetOffline", false, "flag to enable the offline tracking save"};


  // internal pointers
  HTTEventInputHeader* m_eventHeader = nullptr;
  int m_ntowers = 1;

  // Tree structure
  TFile *m_outfile = nullptr; // ROOT file descriptor
  TTree *m_hittree = nullptr; // TTree for the hit storage
  TTree *m_evtinfo = nullptr; /** TTree with general event information */
  TTree *m_trackstree = nullptr;
  TTree *m_offline_cluster_tree = nullptr;

   // Event Info: Add this to an header ?
  int m_run_number = 0; /** event's run number */
  int m_event_number = 0; /** event number */
  float m_averageInteractionsPerCrossing = 0.0F;
  float m_actualInteractionsPerCrossing = 0.0F;
  int m_LB = 0;
  int m_BCID = 0;
  unsigned int m_extendedLevel1ID = 0U;
  unsigned int m_level1TriggerType = 0U;
  std::vector<unsigned int> m_level1TriggerInfo;
  std::vector<HTTHit> *m_original_hits = nullptr; // variables related to the HTTHit storage
  std::vector<HTTTruthTrack> m_truth_tracks;
 //offline clusters
  std::vector<float>   *m_offline_locX = nullptr;
  std::vector<float>   *m_offline_locY = nullptr;
  std::vector<int>     *m_offline_isPixel = nullptr;
  std::vector<int>     *m_offline_isBarrel = nullptr;
  std::vector<int>     *m_offline_layer = nullptr;
  std::vector<int>     *m_offline_clustID = nullptr;
  std::vector<int>     *m_offline_trackNumber = nullptr;

  //offline tracks
  std::vector<float>   *m_offline_eta = nullptr;
  std::vector<float>   *m_offline_phi = nullptr;
  std::vector<float>   *m_offline_d0 = nullptr;
  std::vector<float>   *m_offline_z0 = nullptr;
  std::vector<float>   *m_offline_qoverpt = nullptr;
  std::vector<int>     *m_offline_barcode = nullptr;
  std::vector<float>   *m_offline_barcode_frac = nullptr;
};

#endif // HTTSGRORAWHITSWRAPPERALG_h

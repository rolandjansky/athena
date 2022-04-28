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
  HTTEventInputHeader* m_eventHeader;
  int m_ntowers = 1;

  // Tree structure
  TFile *m_outfile; // ROOT file descriptor
  TTree *m_hittree; // TTree for the hit storage
  TTree *m_evtinfo; /** TTree with general event information */
  TTree *m_trackstree;
  TTree *m_offline_cluster_tree;

   // Event Info: Add this to an header ?
  int m_run_number; /** event's run number */
  int m_event_number; /** event number */
  float m_averageInteractionsPerCrossing;
  float m_actualInteractionsPerCrossing;
  int m_LB;
  int m_BCID;
  unsigned int m_extendedLevel1ID;
  unsigned int m_level1TriggerType;
  std::vector<unsigned int> m_level1TriggerInfo;
  std::vector<HTTHit> *m_original_hits; // variables related to the HTTHit storage
  std::vector<HTTTruthTrack> m_truth_tracks;
 //offline clusters
  std::vector<float>   *m_offline_locX;
  std::vector<float>   *m_offline_locY;
  std::vector<int>     *m_offline_isPixel;
  std::vector<int>     *m_offline_isBarrel;
  std::vector<int>     *m_offline_layer;
  std::vector<int>     *m_offline_resAssociatedTrack;
  std::vector<int>     *m_offline_clustID;
  std::vector<int>     *m_offline_trackNumber;

  //offline tracks
  std::vector<float>   *m_offline_pt;
  std::vector<float>   *m_offline_eta;
  std::vector<float>   *m_offline_phi;
  std::vector<float>   *m_offline_d0;
  std::vector<float>   *m_offline_z0;
  std::vector<float>   *m_offline_q;
  std::vector<float>   *m_offline_qoverpt;
  std::vector<int>     *m_offline_barcode;
  std::vector<float>   *m_offline_barcode_frac;
};

#endif // HTTSGRORAWHITSWRAPPERALG_h

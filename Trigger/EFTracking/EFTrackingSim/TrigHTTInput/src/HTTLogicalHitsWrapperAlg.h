#ifndef HTT_LOGICALHITSWRAPPERALG_H
#define HTT_LOGICALHITSWRAPPERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"

#include "TrigHTTInput/IHTTEventInputHeaderTool.h"
#include "TrigHTTInput/IHTTEventOutputHeaderTool.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTInput/HTTRawToLogicalHitsTool.h"


class TH1F;
class TH2F;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;

class HTTLogicalHitsWrapperAlg : public AthAlgorithm {
public:
  HTTLogicalHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTLogicalHitsWrapperAlg () = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute()    override;
  StatusCode BookHistograms();


private:
 
  ToolHandle<IHTTEventInputHeaderTool>     m_hitInputTool    { this, "InputTool",  "HTTInputHeaderTool/HTTInputHeaderTool", "Input Tool" };
  ToolHandle<IHTTEventOutputHeaderTool>    m_writeOutputTool { this, "OutputTool", "HTTOutputHeaderTool/HTTOutputHeaderTool", "Output Tool" };
  ToolHandle<HTTClusteringToolI>           m_clusteringTool  { this, "ClusteringTool", "HTTClusteringTool/HTTClusteringTool", "HTT Clustering Tool" };
  ToolHandle<HTTRawToLogicalHitsTool>      m_hitMapTool      { this, "RawToLogicalHitsTool", "HTTRawToLogicalHitsTool/HTTRawToLogicalHitsTool", "Map Raw to Logical hit Tool" };

  Gaudi::Property<bool>  m_Clustering   {this, "Clustering", false, "flag to enable the clustering"};
  Gaudi::Property<bool>  m_Monitor      {this, "Monitor",    false, "flag to enable internal monitor"};
 

  // some histograms for internal monitor
  TH1F*   m_hits_barrelEC = nullptr;
  TH2F*   m_hits_r_vs_z = nullptr;

  TH1F*   m_nMissedHit = nullptr;
  TH1F*   m_nMissedHitFrac = nullptr;
  TH1F*   m_MissedHit_eta = nullptr ;
  TH1F*   m_MissedHit_phi = nullptr;
  TH1F*   m_MissedHit_type = nullptr;
  TH1F*   m_MissedHit_barrelEC = nullptr;
  TH1F*   m_MissedHit_layer = nullptr;

};

#endif // HTTSGRORAWHITSWRAPPERALG_h

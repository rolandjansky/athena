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
 
  // internal variables
  int m_ntowers;

  // some histograms for internal monitor
  TH1F*   m_hits_barrelEC;
  TH2F*   m_hits_r_vs_z;

  TH1F*   m_nMissedHit;
  TH1F*   m_nMissedHitFrac;
  TH1F*   m_MissedHit_eta ;
  TH1F*   m_MissedHit_phi;
  TH1F*   m_MissedHit_type;
  TH1F*   m_MissedHit_barrelEC;
  TH1F*   m_MissedHit_layer;

};

#endif // HTTSGRORAWHITSWRAPPERALG_h

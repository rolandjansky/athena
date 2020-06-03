/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"
#include "TGraph.h"
#include "TMath.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "StoreGate/ReadHandle.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "InDetGlobalPrimaryVertexMonTool.h"

InDetGlobalPrimaryVertexMonTool::InDetGlobalPrimaryVertexMonTool( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   // basic montoring
   m_hPvN(0),
   m_hPvNPriVtx(0),
   m_hPvNPileupVtx(0),
   m_hPvN_LB(0),
   m_hPvNaveMu(0),
   m_hPvX(0),
   m_hPvY(0),
   m_hPvZ(0),
   m_hPvErrX(0),
   m_hPvErrY(0),
   m_hPvErrZ(0),
   m_hPvChiSqDoF(0),
   m_hPvNTracks(0),
   m_hPvTrackPt(0),
   m_hPvTrackEta(0),
   // ennhanced montoring
   m_hVrt_XpullVsNtrkAverage_split(0),
   m_hVrt_YpullVsNtrkAverage_split(0),
   m_hVrt_ZpullVsNtrkAverage_split(0),

   m_hVrt_XpullVsPt2Average_split(0),
   m_hVrt_YpullVsPt2Average_split(0),
   m_hVrt_ZpullVsPt2Average_split(0),

   m_hVrt_Xerr_vs_ntrk(0),
   m_hVrt_Yerr_vs_ntrk(0),
   m_hVrt_Zerr_vs_ntrk(0),

   m_hVrt_Xerr_vs_pt2(0),
   m_hVrt_Yerr_vs_pt2(0),
   m_hVrt_Zerr_vs_pt2(0),
   m_hVrt_split_tag_ntrk(0),
   m_hVrt_split_probe_ntrk(0),
   m_hVrt_split_matched_tag_ntrk(0),
   m_hVrt_split_matched_probe_ntrk(0),
   m_hVrt_split_dist_tag(0),
   m_hVrt_split_dist_probe(0),
   m_histFolder("InDetGlobal/PrimaryVertex"),
   m_splitVertexTrkInvFraction(2),
   m_distanceSplitVxMatch(5.0),
   m_splitMatchingMetric(3),
   m_doEnhancedMonitoring(false)
{
  declareProperty("histFolder", m_histFolder);
  declareProperty("splitVertexTrkInvFraction", m_splitVertexTrkInvFraction, "inverse fraction to split tracks (1:N)");
  declareProperty("distanceSplitVertexMatch", m_distanceSplitVxMatch, "Distance for matching split-original Vertex in selection efficiency");
  declareProperty("splitMatchingMetric", m_splitMatchingMetric, "Determines which function to use to calculate matching between split vertices and original input vertex -- used in selection efficiency");
  declareProperty("doEnhancedMonitoring" , m_doEnhancedMonitoring, "turn on the enhanced vertex monitoring, it is triggered by the same InDetFlag that also triggers the creation of no beam constraint and split vertices");
}

InDetGlobalPrimaryVertexMonTool::~InDetGlobalPrimaryVertexMonTool() {}

StatusCode InDetGlobalPrimaryVertexMonTool::initialize() {
  StatusCode sc;
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  ATH_CHECK( m_vxContainerName.initialize() );
  ATH_CHECK( m_vxContainerNameWithoutBeamConstraint.initialize(m_doEnhancedMonitoring) );
  ATH_CHECK( m_vxContainerNameSplit.initialize(m_doEnhancedMonitoring) );

  return StatusCode::SUCCESS;
}

StatusCode InDetGlobalPrimaryVertexMonTool::bookHistogramsRecurrent() {

    MonGroup al_primaryvertex_shift( this, m_histFolder,  run, ATTRIB_UNMANAGED);
    MonGroup al_primaryvertex_expert( this, m_histFolder, run, ATTRIB_UNMANAGED);

    if( newRunFlag() ) {

    // basic monitoring
    m_hPvX          = makeAndRegisterTH1F(al_primaryvertex_shift,"pvX","Primary vertex: x;x (mm)",500,-5.,5.);
    m_hPvY          = makeAndRegisterTH1F(al_primaryvertex_shift,"pvY","Primary vertex: y;y (mm)",500,-5.,5.);
    m_hPvZ          = makeAndRegisterTH1F(al_primaryvertex_shift,"pvZ","Primary vertex: z;z (mm)",100,-200.,200.);
    m_hPvN          = makeAndRegisterTH1F(al_primaryvertex_shift,"pvN","Total number of vertices (primary and pile up);Total number of vertices",50,0.,50.);
    if ( al_primaryvertex_shift.regHist( m_hPvN_LB = TProfile_LW::create("pvN_LB", "Total number of vertices (primary and pile up) vs LB", 2000, 0.5, 2000.5 ) ).isFailure() )
	ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string("pvN_LB"));
    
    m_hPvNPriVtx    = makeAndRegisterTH1F(al_primaryvertex_expert,"pvNPriVtx","Number of primary vertices;Number of primary vertices",3,0.,3.);
    m_hPvNPileupVtx = makeAndRegisterTH1F(al_primaryvertex_expert,"pvNPileupVtx","Number of pileup vertices;Number of pile up vertices",40,0.,40.);
    if ( AthenaMonManager::environment() != AthenaMonManager::online )
    {
	m_hPvNaveMu     = makeAndRegisterTH2F(al_primaryvertex_shift,"m_hPvNaveMu","Number of vertices vs <#mu>", 50, 0.,50., 40, 0.,40.);
    }
    m_hPvErrX       = makeAndRegisterTH1F(al_primaryvertex_expert,"pvErrX","Primary vertex: #sigma_{x}; #sigma_{x} (mm)",100,0.,.25);
    m_hPvErrY       = makeAndRegisterTH1F(al_primaryvertex_expert,"pvErrY","Primary vertex: #sigma_{y}; #sigma_{y} (mm)",100,0.,.25);
    m_hPvErrZ       = makeAndRegisterTH1F(al_primaryvertex_expert,"pvErrZ","Primary vertex: #sigma_{z}; #sigma_{z} (mm)",100,0.,.25);
    m_hPvChiSqDoF   = makeAndRegisterTH1F(al_primaryvertex_shift,"pvChiSqDof","Primary vertex: #Chi^{2}/DoF of vertex fit;#Chi^{2}/DoF",100,0.,5.);
    m_hPvNTracks    = makeAndRegisterTH1F(al_primaryvertex_shift,"pvNTracks","Number of tracks in primary vertex;Number of tracks",300,0.,300.);
    m_hPvTrackPt    = makeAndRegisterTH1F(al_primaryvertex_shift,"pvTrackPt","Primary vertex: original track p_{t};p_{t} (GeV)",100,0.,20.);
    m_hPvTrackEta   = makeAndRegisterTH1F(al_primaryvertex_shift,"pvTrackEta","Primary vertex: original track #eta; #eta",100,-3.,3.);

    // enhanced monitoring
    if (m_doEnhancedMonitoring)
    {
      // to be certain that 0 tracks end up at 0. in histo (and 3 tracks at 3., aso. ...)
      float minBinNTrk(-0.5); float maxBinNTrk(149.5); unsigned int numBinsNTrk((unsigned int)(maxBinNTrk-minBinNTrk));
      float minBinSumPt2(0.); float maxBinSumPt2(20.); unsigned int numBinsSumPt2((unsigned int)( (maxBinSumPt2-minBinSumPt2) / 0.25 )); //bins size = 0.25

      m_hVrt_XpullVsNtrkAverage_split = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_XpullVsNtrkAverage_split", "Entries;N Tracks average;x pull value",numBinsNTrk, minBinNTrk, maxBinNTrk, 60, -6., 6.);
      m_hVrt_YpullVsNtrkAverage_split = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_YpullVsNtrkAverage_split", "Entries;N Tracks average;y pull value",numBinsNTrk, minBinNTrk, maxBinNTrk, 60, -6., 6.);
      m_hVrt_ZpullVsNtrkAverage_split = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_ZpullVsNtrkAverage_split", "Entries;N Tracks average;z pull value",numBinsNTrk, minBinNTrk, maxBinNTrk, 60, -6., 6.);
  
      m_hVrt_XpullVsPt2Average_split = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_XpullVsPt2Average_split", "Entries;#sqrt{#sum p_{T}^{2}} average [GeV];x pull value", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 60, -6., 6.);
      m_hVrt_YpullVsPt2Average_split = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_YpullVsPt2Average_split", "Entries;#sqrt{#sum p_{T}^{2}} average [GeV];y pull value", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 60, -6., 6.);
      m_hVrt_ZpullVsPt2Average_split = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_ZpullVsPt2Average_split", "Entries;#sqrt{#sum p_{T}^{2}} average [GeV];z pull value", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 60, -6., 6.);
  
      // errors of unconstrained vertices
      m_hVrt_Xerr_vs_ntrk = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_Xerr_vs_ntrk","Entries;Number of Tracks;x vertex Error [mm]",numBinsNTrk, minBinNTrk, maxBinNTrk, 200, 0., 1.);
      m_hVrt_Yerr_vs_ntrk = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_Yerr_vs_ntrk","Entries;Number of Tracks;y vertex Error [mm]",numBinsNTrk, minBinNTrk, maxBinNTrk, 200, 0., 1.);
      m_hVrt_Zerr_vs_ntrk = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_Zerr_vs_ntrk","Entries;Number of Tracks;z vertex Error [mm]",numBinsNTrk, minBinNTrk, maxBinNTrk, 400, 0., 2.);
  
      m_hVrt_Xerr_vs_pt2  = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_Xerr_vs_pt2","Entries;#sqrt{#sum p_{T}^{2}} [GeV];x vertex Error [mm]", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 200, 0., 1.);
      m_hVrt_Yerr_vs_pt2  = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_Yerr_vs_pt2","Entries;#sqrt{#sum p_{T}^{2}} [GeV];y vertex Error [mm]", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 200, 0., 1.);
      m_hVrt_Zerr_vs_pt2  = makeAndRegisterTH2F(al_primaryvertex_expert,"Vrt_Zerr_vs_pt2","Entries;#sqrt{#sum p_{T}^{2}} [GeV];z vertex Error [mm]", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 400, 0., 2.);
  
      // number of tag and probe split vertices reconstructed as a function of the number of tracks at the probe vertex
      m_hVrt_split_tag_ntrk = makeAndRegisterTH1F(al_primaryvertex_expert, "Vrt_split_tag_ntrk", "N. events with reconstructed tag;Number of tracks @ tag vertex", numBinsNTrk, minBinNTrk, maxBinNTrk);
      m_hVrt_split_probe_ntrk = makeAndRegisterTH1F(al_primaryvertex_expert, "Vrt_split_probe_ntrk", "N. events with both tag and probe reconstructed;Number of tracks @ probe vertex", numBinsNTrk, minBinNTrk, maxBinNTrk);
      // number of tag and probe split vertices reconstructed and matched to original non-BC vertex 
      // as a function of the number of tracks at the probe vertex
      m_hVrt_split_matched_tag_ntrk = makeAndRegisterTH1F(al_primaryvertex_expert, "Vrt_split_matched_tag_ntrk", "N. events with reconstructed tag;Number of tracks @ tag vertex", numBinsNTrk, minBinNTrk, maxBinNTrk);
      m_hVrt_split_matched_probe_ntrk = makeAndRegisterTH1F(al_primaryvertex_expert, "Vrt_split_matched_probe_ntrk", "N. events with both tag and probe reconstructed;Number of tracks @ probe vertex", numBinsNTrk, minBinNTrk, maxBinNTrk);
      // distance between split vertices and non-BC vertex
      m_hVrt_split_dist_tag   = makeAndRegisterTH1F(al_primaryvertex_expert, "Vrt_split_dist_tag", "Entries / 0.05mm;Distance of Tag split vertex from reconstructed", 100, 0., 20.);
      m_hVrt_split_dist_probe  = makeAndRegisterTH1F(al_primaryvertex_expert, "Vrt_split_dist_probe", "Entries / 0.05mm;Distance of Probe split vertex from reconstructed", 100, 0., 20.);

      // dummy registration for histos produced in the post processing (the DQ systems needs it like this ...)
      // resolution graphs
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "resolution_X_Ntrk");
      makeAndRegisterDummyTGraph(al_primaryvertex_expert, "scaleFactorX_Ntrk_RMS");
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "scaleFactor_X_Ntrk_Fit");
      
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "resolution_X_SumPt2");
      makeAndRegisterDummyTGraph(al_primaryvertex_expert, "scaleFactorX_SumPt2_RMS");
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "scaleFactor_X_SumPt2_Fit");
      
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "resolution_Y_Ntrk");
      makeAndRegisterDummyTGraph(al_primaryvertex_expert, "scaleFactorY_Ntrk_RMS");
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "scaleFactor_Y_Ntrk_Fit");
      
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "resolution_Y_SumPt2");
      makeAndRegisterDummyTGraph(al_primaryvertex_expert, "scaleFactorY_SumPt2_RMS");
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "scaleFactor_Y_SumPt2_Fit");
      
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "resolution_Z_Ntrk");
      makeAndRegisterDummyTGraph(al_primaryvertex_expert, "scaleFactorZ_Ntrk_RMS");
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "scaleFactor_Z_Ntrk_Fit");
      
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "resolution_Z_SumPt2");
      makeAndRegisterDummyTGraph(al_primaryvertex_expert, "scaleFactorZ_SumPt2_RMS");
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "scaleFactor_Z_SumPt2_Fit");
      
      // efficiency graphs
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "g_RecEff_M1");
      makeAndRegisterDummyTGraph(al_primaryvertex_shift, "g_SelEff_M1");
    
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode InDetGlobalPrimaryVertexMonTool::fillHistograms() {

  // Basic primary vertex monitoring
  SG::ReadHandle<xAOD::VertexContainer> vxContainer(m_vxContainerName);
  if (!vxContainer.isPresent()) {
    ATH_MSG_DEBUG ("StoreGate doesn't contain primary vertex container with key "+m_vxContainerName.key());
    return StatusCode::SUCCESS;
  }
  if (!vxContainer.isValid()) {
    ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key "+m_vxContainerName.key());
    return StatusCode::SUCCESS;
  }

  m_hPvN->Fill(vxContainer->size()-1);  // exclude dummy vertex
  m_hPvN_LB->Fill( m_manager->lumiBlockNumber(), vxContainer->size()-1);
  
  if ( AthenaMonManager::environment() != AthenaMonManager::online )
  {
      m_hPvNaveMu->Fill( lbAverageInteractionsPerCrossing(), vxContainer->size()-1 );
  }
  int nPriVtx = 0;
  int nPileupVtx = 0;
  for (xAOD::VertexContainer::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter)
  {
    // Count different types of vertices
      if ((*vxIter)->vertexType() == xAOD::VxType::PriVtx) nPriVtx++;
    if ((*vxIter)->vertexType() == xAOD::VxType::PileUp) nPileupVtx++;

    // Select primary vertex
    if ((*vxIter)->vertexType() != xAOD::VxType::PriVtx) continue;
    if ((*vxIter)->numberDoF() <= 0) continue;

    // fill basic vertex histograms
//     std::cout << (*vxIter)->recVertex().position().x() << std::endl;
    m_hPvX->Fill((*vxIter)->position().x());
    m_hPvY->Fill((*vxIter)->position().y());
    m_hPvZ->Fill((*vxIter)->position().z());
    m_hPvErrX->Fill( Amg::error( (*vxIter)->covariancePosition(), Trk::x) );
    m_hPvErrY->Fill( Amg::error( (*vxIter)->covariancePosition(), Trk::y) );
    m_hPvErrZ->Fill( Amg::error( (*vxIter)->covariancePosition(), Trk::z) );
    m_hPvChiSqDoF->Fill( (*vxIter)->chiSquared() / (*vxIter)->numberDoF() );


    auto & trackparticles = (*vxIter)->trackParticleLinks();
    
    // fill some track related histos
    m_hPvNTracks->Fill( trackparticles.size() );
    
    // Histograms on original tracks used for primary vertex
    for (auto trackparticle  : trackparticles )
    {
	const Trk::Perigee & measuredPerigee = (*trackparticle)->perigeeParameters();
	m_hPvTrackEta->Fill( measuredPerigee.eta() );
	m_hPvTrackPt->Fill(measuredPerigee.pT()/1000. );   // Histo is in GeV, not MeV
    }
  }
  m_hPvNPriVtx->Fill(nPriVtx);
  m_hPvNPileupVtx->Fill(nPileupVtx);

  if (m_doEnhancedMonitoring)
  {
    // Check for primary vertex with out beam constraint
    SG::ReadHandle<xAOD::VertexContainer> vxContainerWithoutBeamConstraint(m_vxContainerNameWithoutBeamConstraint);
    if (!vxContainerWithoutBeamConstraint.isPresent()) {
      ATH_MSG_DEBUG ("No primary vertex (done without beam constraint) with key "+m_vxContainerNameWithoutBeamConstraint.key()+" in storegate.");
      return StatusCode::SUCCESS;
    }
    if (!vxContainerWithoutBeamConstraint.isValid()) {
      ATH_MSG_DEBUG ("Could not retrieve primary vertex (done without beam constraint) with key "+m_vxContainerNameWithoutBeamConstraint.key());
      return StatusCode::SUCCESS;
    }
  
    SG::ReadHandle<xAOD::VertexContainer> vxContainerSplit(m_vxContainerNameSplit);
    if (!vxContainerSplit.isPresent()) {
      ATH_MSG_DEBUG ("No split primary vertex with key "+m_vxContainerNameSplit.key()+" in storegate.");
      return StatusCode::SUCCESS;
    }
    if (!vxContainerSplit.isValid()) {
      ATH_MSG_DEBUG ("Could not retrieve split primary vertex with key "+m_vxContainerNameSplit.key());
      return StatusCode::SUCCESS;
    }
  
    for (xAOD::VertexContainer::const_iterator vxIter = vxContainerWithoutBeamConstraint->begin(); vxIter != vxContainerWithoutBeamConstraint->end(); ++vxIter)
    {
  
      // Select primary vertex
      if ((*vxIter)->vertexType() == xAOD::VxType::NoVtx) continue;
      if ((*vxIter)->numberDoF() <= 0) continue;
  
      // fill some track related histos
  
      // Histograms on original tracks used for primary vertex
      if ((*vxIter)->vxTrackAtVertexAvailable())
      {
	std::vector<Trk::VxTrackAtVertex> vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();

        float sumpt2 = 0.;
        for (std::vector<Trk::VxTrackAtVertex>::iterator trkIter  = vxTrackAtVertex.begin();
	     trkIter != vxTrackAtVertex.end()  ; ++trkIter)
        {
          // pT at initial and perigeeAtVertex should be similar if not equal. measured perigee is stored in vertex (the other has to be retrieved from the linked Trk::Track(Particle))
          const Trk::Perigee* measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter).perigeeAtVertex());
          if (measuredPerigee == 0) measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter).initialPerigee());
          if (measuredPerigee!=0) sumpt2 += (measuredPerigee->pT()*measuredPerigee->pT())/1e6;
        }
  
        // for do enhanced vertexing
        m_hVrt_Xerr_vs_ntrk->Fill(vxTrackAtVertex.size(), Amg::error( (*vxIter)->covariancePosition(), Trk::x) );
	m_hVrt_Yerr_vs_ntrk->Fill(vxTrackAtVertex.size(), Amg::error( (*vxIter)->covariancePosition(), Trk::y) );
	m_hVrt_Zerr_vs_ntrk->Fill(vxTrackAtVertex.size(), Amg::error( (*vxIter)->covariancePosition(), Trk::z) );
  
        float sqrt_sumpt2(TMath::Sqrt(sumpt2));
        m_hVrt_Xerr_vs_pt2->Fill(sqrt_sumpt2, Amg::error( (*vxIter)->covariancePosition(), Trk::x) );
	m_hVrt_Yerr_vs_pt2->Fill(sqrt_sumpt2, Amg::error( (*vxIter)->covariancePosition(), Trk::y) );
	m_hVrt_Zerr_vs_pt2->Fill(sqrt_sumpt2, Amg::error( (*vxIter)->covariancePosition(), Trk::z) );
      }
    }
  
    if (vxContainerSplit->size() >= 3) // you need two split vertices and there is the dummy
    {
      const xAOD::Vertex* splitVxCandiate1 = vxContainerSplit->at(0);
      const xAOD::Vertex* splitVxCandiate2 = vxContainerSplit->at(1);
  
      // consider the first two split vertices for K-factor and reconstruction efficiency calculation
      if (splitVxCandiate1 != 0 and splitVxCandiate2 != 0)
      {
        // calculate reconstruction efficiency
        // we ask only one reconstructed vertex (no Beamspot-Constrained) and two split vertices
        const xAOD::Vertex* origVertexNoBC = vxContainerWithoutBeamConstraint->at(0);
        if ( vxContainerWithoutBeamConstraint->size() == 2 and  // 1 vtx + 1 dummy
            origVertexNoBC->vertexType() == xAOD::VxType::PriVtx and // good vertex
           
            vxContainerSplit->size() == 4 and // 2 split vertices, 2 dummy in this container
            splitVxCandiate1->vertexType() == xAOD::VxType::PriVtx) // tag vertex needs to be reconstructed
        {
          // calculate number of tracks at probe vertex -- needs to be discussed
          int nTrackAtOrigVertex = origVertexNoBC->vxTrackAtVertex().size();
          int nTracksForEff =  nTrackAtOrigVertex * (m_splitVertexTrkInvFraction - 1) / (m_splitVertexTrkInvFraction);
  
          // fill tag histogram (denominator of efficiency)
          m_hVrt_split_tag_ntrk->Fill(nTracksForEff);
          float distanceMatchTag = GetSplitMatchDistance(splitVxCandiate1, origVertexNoBC);
          m_hVrt_split_dist_tag->Fill(distanceMatchTag);
          if (distanceMatchTag <= m_distanceSplitVxMatch)
            m_hVrt_split_matched_tag_ntrk->Fill(nTracksForEff);
  
          // check if probe vertex was reconstructed
          if (splitVxCandiate2->vertexType() == xAOD::VxType::PriVtx) {
            m_hVrt_split_probe_ntrk->Fill(nTracksForEff);
            float distanceMatchProbe = GetSplitMatchDistance(splitVxCandiate2, origVertexNoBC);
            m_hVrt_split_dist_probe->Fill(distanceMatchProbe);
            if (distanceMatchProbe <= m_distanceSplitVxMatch && distanceMatchTag <= m_distanceSplitVxMatch)
              m_hVrt_split_matched_probe_ntrk->Fill(nTracksForEff);
          } // end: probe split vertex reconstructed
  
        } // end: tag split vertex reconstructed
  
        // calculate K-factors for resolution
        if (splitVxCandiate1->vertexType() == xAOD::VxType::PriVtx and splitVxCandiate2->vertexType() == xAOD::VxType::PriVtx)
        {
          float x_distance = splitVxCandiate1->position().x()-splitVxCandiate2->position().x();
          float y_distance = splitVxCandiate1->position().y()-splitVxCandiate2->position().y();
          float z_distance = splitVxCandiate1->position().z()-splitVxCandiate2->position().z();
  
          float x_error = TMath::Sqrt(TMath::Power( Amg::error( splitVxCandiate1->covariancePosition(), Trk::x ),2) +
				      TMath::Power( Amg::error( splitVxCandiate2->covariancePosition(), Trk::x ),2) );
	  float y_error = TMath::Sqrt(TMath::Power( Amg::error( splitVxCandiate1->covariancePosition(), Trk::y ),2) +
				      TMath::Power( Amg::error( splitVxCandiate2->covariancePosition(), Trk::y ),2) );
	  float z_error = TMath::Sqrt(TMath::Power( Amg::error( splitVxCandiate1->covariancePosition(), Trk::z ),2) +
				      TMath::Power( Amg::error( splitVxCandiate2->covariancePosition(), Trk::z ),2) );
  
          float x_split_pull = x_distance/x_error;
          float y_split_pull = y_distance/y_error;
          float z_split_pull = z_distance/z_error;
  
          const std::vector< Trk::VxTrackAtVertex > splitVxTrackAtVertexVector1 = splitVxCandiate1->vxTrackAtVertex();
          const std::vector< Trk::VxTrackAtVertex > splitVxTrackAtVertexVector2 = splitVxCandiate2->vxTrackAtVertex();
  
          int ntrk_even  = splitVxTrackAtVertexVector1.size();
          int ntrk_odd   = splitVxTrackAtVertexVector2.size();
          int ntrk_av    = (ntrk_odd + ntrk_even)/2;
  
          m_hVrt_XpullVsNtrkAverage_split->Fill(ntrk_av, x_split_pull);
          m_hVrt_YpullVsNtrkAverage_split->Fill(ntrk_av, y_split_pull);
          m_hVrt_ZpullVsNtrkAverage_split->Fill(ntrk_av, z_split_pull);
  
          //calculation of the  average sum of pt2
          float sum_pt2_1 = 0.;
          float sum_pt2_2 = 0.;
  
          for (std::vector<Trk::VxTrackAtVertex>::const_iterator trkIter  = splitVxTrackAtVertexVector1.begin();
              trkIter != splitVxTrackAtVertexVector1.end()  ; ++trkIter)
          {
            // pT at initial and perigeeAtVertex should be similar if not equal. measured perigee is stored in vertex (the other has to be retrieved from the linked Trk::Track(Particle))
            const Trk::Perigee* measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter).perigeeAtVertex());
            if (measuredPerigee == 0)  measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter).initialPerigee());
            if (measuredPerigee!=0)
            {
              //             std::cout << measuredPerigee->pT() << std::endl;
              float pT = measuredPerigee->pT()/1000.;
              sum_pt2_1 += TMath::Power(pT,2);
            }
          }
          float sqrt_sum_pt2_1(TMath::Sqrt(sum_pt2_1));
  
          for (std::vector<Trk::VxTrackAtVertex>::const_iterator trkIter  = splitVxTrackAtVertexVector2.begin();
              trkIter != splitVxTrackAtVertexVector2.end()  ; ++trkIter)
          {
            // pT at initial and perigeeAtVertex should be similar if not equal. measured perigee is stored in vertex (the other has to be retrieved from the linked Trk::Track(Particle))
            const Trk::Perigee* measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter).perigeeAtVertex());
            if (measuredPerigee == 0)  measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter).initialPerigee());
            if (measuredPerigee!=0)
            {
              float pT = measuredPerigee->pT()/1000.;
              sum_pt2_2 += TMath::Power(pT,2);
            }
          }
          float sqrt_sum_pt2_2(TMath::Sqrt(sum_pt2_2));
  
          float sqrt_sum_pt2_average((sqrt_sum_pt2_1+sqrt_sum_pt2_2)/2.);
          m_hVrt_XpullVsPt2Average_split->Fill(sqrt_sum_pt2_average, x_split_pull);
          m_hVrt_YpullVsPt2Average_split->Fill(sqrt_sum_pt2_average, y_split_pull);
          m_hVrt_ZpullVsPt2Average_split->Fill(sqrt_sum_pt2_average, z_split_pull);
        } // end loop: two succesfully reconstructed split vertices
      }
    } // end loop: at least 2 split vertices
  } // end if (m_doEnhancedMonitoring)
 
  return StatusCode::SUCCESS;
}

TH1F_LW* InDetGlobalPrimaryVertexMonTool::makeAndRegisterTH1F(MonGroup& mon,
                                                              const char* hName,
                                                              std::string hTitle,
                                                              int nBins,
                                                              float minX,
                                                              float maxX)
{
  TH1F_LW* h = TH1F_LW::create(hName,hTitle.c_str(),nBins,minX,maxX);
  //h->Sumw2();
  if (mon.regHist(h).isFailure()) {
    ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string(hName));
  }
  return h;
}

TH2F_LW* InDetGlobalPrimaryVertexMonTool::makeAndRegisterTH2F(MonGroup& mon,
    const char* hName, std::string hTitle,
    int nBinsX, float minX, float maxX,
    int nBinsY, float minY, float maxY) {
      TH2F_LW* h = TH2F_LW::create(hName,hTitle.c_str(),nBinsX,minX,maxX,nBinsY,minY,maxY);
  //h->Sumw2();
  //h->SetOption("colz");
      if (mon.regHist(h).isFailure()) {
        ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string(hName));
      }
      return h;
}

TH2F_LW* InDetGlobalPrimaryVertexMonTool::makeAndRegisterTH2FVariableXBins(MonGroup& mon,
    const char* hName, std::string hTitle,
    int nBinsX, double* xRange,           // range MUST be double, otherwise no compile ...
    int nBinsY, float minY, float maxY) {
      TH2F_LW* h = TH2F_LW::create(hName,hTitle.c_str(),nBinsX,xRange,nBinsY,minY,maxY);
  //h->Sumw2();
  //h->SetOption("colz");
      if (mon.regHist(h).isFailure()) {
        ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string(hName));
      }
      return h;
}

void InDetGlobalPrimaryVertexMonTool::makeAndRegisterDummyTGraph(MonGroup& mon, const char* hName)
{
      TGraph* h = new TGraph();
      h->SetName(hName);
      if (mon.regGraph(h).isFailure()) {
        ATH_MSG_WARNING ("Unable to book TGraph with name = " + std::string(hName));
      }
      return;
}


double InDetGlobalPrimaryVertexMonTool::GetSplitMatchDistance(const xAOD::Vertex* splitVx, const xAOD::Vertex* originalVx)
{
  // Get difference in position
  double dx = splitVx->position().x() - originalVx->position().x();
  double dy = splitVx->position().y() - originalVx->position().y();
  double dz = splitVx->position().z() - originalVx->position().z();
  // The error on the difference is partially correlated. Assume split vertex error as rough estimate.
  float dxError = Amg::error( splitVx->covariancePosition(), Trk::x);
  float dyError = Amg::error( splitVx->covariancePosition(), Trk::y);
  float dzError = Amg::error( splitVx->covariancePosition(), Trk::z);

  switch (m_splitMatchingMetric) {
  case 0:
    // dummy metric: returns zero distance
    return 0.0;
    break;
  case 1:
    // simple delta-z metric
    return TMath::Abs(dz);
    break;
  case 2:
    {
      // 3-D distance divided by the error (dominated by Delta z)
      double d = TMath::Sqrt(dx*dx+dy*dy+dz*dz);
      double dErr = TMath::Sqrt(TMath::Power(dx*dxError/d,2)+TMath::Power(dy*dyError/d,2)+TMath::Power(dz*dzError/d,2));
      return d/dErr;
      break;
    }
  case 3:
    {
      // quadratic sum of significance distances in the 3 directions. Expected RMS = 1
      return TMath::Sqrt(TMath::Power(dx/dxError,2)+TMath::Power(dy/dyError,2)+TMath::Power(dz/dzError,2));
      break;
    }
  default:
    break;
  }

  // choice not valid. Return high number and print error
  ATH_MSG_DEBUG("Invalid choice for the vertex selection efficiency metric.");
  return 999.0;

}

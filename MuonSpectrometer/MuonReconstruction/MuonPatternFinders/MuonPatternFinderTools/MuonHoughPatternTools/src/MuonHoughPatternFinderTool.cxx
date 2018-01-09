/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternTools/MuonHoughPatternFinderTool.h"
#include "MuonHoughPatternTools/MuonHoughPatternTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonHoughPatternEvent/MuonHoughHitContainer.h"

#include <map>
#include <set>
#include <vector>

#include "TH1F.h"
#include "TFile.h"

#include "GaudiKernel/IToolSvc.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "TrkSurfaces/Surface.h"

//#include "MuonGeoModel/MuonDetectorManager.h"
//#include "MuonGeoModel/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonPrepRawData/MdtDriftCircleStatus.h"
#include "MuonSegment/MuonSegmentCombination.h" // for csc's
#include "MuonSegment/MuonSegment.h" 
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/MatchDCWithLine.h"
#include "TrkDriftCircleMath/TangentToCircles.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "StoreGate/StoreGateSvc.h"

using namespace TrkDriftCircleMath;

namespace Muon {

  MuonHoughPatternFinderTool::MuonHoughPatternFinderTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p),
    m_muonHoughPatternTool("MuonHoughPatternTool"), 
    m_muonCombinePatternTool("MuonCombinePatternTool"), 
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"), 
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_hit_reweights(true),
    m_mdt_adc_cut(true), 
    m_mdt_adc_min(50), 
    m_mdt_tdc_cut(true), 
    m_count(0), 
    m_use_rpc(true),
    m_use_tgc(true),
    m_use_csc(true),
    m_use_mdt(true),
    m_weight_csc_on_segment(2.),
    m_showerskip(true),
    m_showerskipperc(0.3),
    m_use_histos(false),
    m_summary(false),
    m_recordAllOutput(false),
    m_cscAssoOutputLocation("MuonPatCscSegAssMap"), 
    m_phietahitassociation(0),
    m_CosmicPhiPatternsKey("CosmicPhiPatterns"),
    m_CosmicEtaPatternsKey("CosmicEtaPatterns"),
    m_COMBINED_PATTERNSKey("COMBINED_PATTERNS")
  {
    declareInterface<IMuonHoughPatternFinderTool>(this);
    
    declareProperty("muonHoughPatternTool",m_muonHoughPatternTool);
    declareProperty("muonCombinePatternTool",m_muonCombinePatternTool);

    declareProperty("HitReweights",m_hit_reweights);
    declareProperty("MDT_ADC_cut",m_mdt_adc_cut);
    declareProperty("MDT_TDC_cut",m_mdt_tdc_cut);

    declareProperty("MDT_ADC_value",m_mdt_adc_min);

    declareProperty("RPC",m_use_rpc);
    declareProperty("TGC",m_use_tgc);
    declareProperty("CSC",m_use_csc);
    declareProperty("MDT",m_use_mdt);

    declareProperty("ShowerSkipping",m_showerskip);
    declareProperty("ShowerSkipPercentage",m_showerskipperc);

    declareProperty("PatCscSegAssMapOutputLocation",m_cscAssoOutputLocation);//Not used
    declareProperty("UseHistos",m_use_histos);
    declareProperty("DoSummary",m_summary);
    declareProperty("RecordAll",m_recordAllOutput);

    declareProperty("CosmicPhiKey", m_CosmicPhiPatternsKey);
    declareProperty("CosmicEtaPatterns", m_CosmicEtaPatternsKey);
    declareProperty("COMBINED_PATTERNS", m_COMBINED_PATTERNSKey);
  }

  MuonHoughPatternFinderTool::~MuonHoughPatternFinderTool()
  {
  }

  StatusCode MuonHoughPatternFinderTool::initialize()
  {
    if (m_use_histos == true) {
      m_file = new TFile("Hough_histos.root","RECREATE");
      m_weighthistogram =  new TH1F("weighthisto","weighthisto",100,-0.5,2);
      m_weighthistogrammdt =  new TH1F("weighthistomdt","weighthistomdt",100,-0.3,2.2);
      m_weighthistogramrpc =  new TH1F("weighthistorpc","weighthistorpc",100,-0.3,2.2);
      m_weighthistogramcsc =  new TH1F("weighthistocsc","weighthistocsc",100,-0.3,2.2);
      m_weighthistogramtgc =  new TH1F("weighthistotgc","weighthistotgc",100,-0.3,2.2);
    }

    ATH_MSG_VERBOSE ("MuonHoughPatternFinderTool::Initializing");
    StatusCode sc = m_muonCombinePatternTool.retrieve();

    if(sc.isFailure())
      {
	ATH_MSG_FATAL ("Could not get MuonCombinePatternTool tool");
	return sc;
      }
    else
      {
	ATH_MSG_VERBOSE ("found Service MuonCombinePatternTool " << m_muonCombinePatternTool);
      }

    sc = m_muonHoughPatternTool.retrieve();

    if(sc.isFailure())
      {
	ATH_MSG_FATAL ("Could not get muonHoughPatternTool tool");
	return sc;
      }
    else
      {
	ATH_MSG_VERBOSE ("found Service muonHoughPatternTool: " << m_muonHoughPatternTool);
      }

    sc = m_idHelperTool.retrieve();
    if (sc.isSuccess()){
      ATH_MSG_DEBUG ("Retrieved " << m_idHelperTool);
    }else{ 
      ATH_MSG_FATAL ("Could not get " << m_idHelperTool);
      return sc;
    }

    sc = m_printer.retrieve();
    if (sc.isSuccess()){
      ATH_MSG_DEBUG ("Retrieved " << m_printer);
    }else{
      ATH_MSG_FATAL ("Could not get " << m_printer);
      return sc;
    }
  
    StoreGateSvc* detStore=0;
    sc = serviceLocator()->service("DetectorStore", detStore);

    if ( sc.isSuccess() ) 
      {
	sc = detStore->retrieve( m_detMgr );

	if ( sc.isFailure() ) {
          ATH_MSG_WARNING (" Cannot retrieve MuonDetDescrMgr ");
          return sc;
	} 
	else 
	  {
	    m_mdtIdHelper = m_detMgr->mdtIdHelper();
	    m_cscIdHelper = m_detMgr->cscIdHelper();    
	    m_rpcIdHelper = m_detMgr->rpcIdHelper();
	    m_tgcIdHelper = m_detMgr->tgcIdHelper();
	    ATH_MSG_DEBUG (" Retrieved IdHelpers: (mdt, csc, rpc and tgc) ");
	  }
      } 
    else 
      {
	ATH_MSG_WARNING (" MuonDetDescrMgr not found in DetectorStore ");
	return sc;
      }

    if ( m_hit_reweights ) {
      ATH_MSG_DEBUG ("Hit Reweighting " << m_hit_reweights);
    }

    m_phietahitassociation = new std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >;

    ATH_CHECK( m_CosmicPhiPatternsKey.initialize(m_recordAllOutput) );
    ATH_CHECK( m_CosmicEtaPatternsKey.initialize(m_recordAllOutput) );
    ATH_CHECK( m_COMBINED_PATTERNSKey.initialize(m_recordAllOutput) );

    ATH_MSG_VERBOSE ("End of Initializing");
    return StatusCode::SUCCESS; 
  }

  const MuonPatternCombinationCollection* MuonHoughPatternFinderTool::find( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
									    const std::vector<const CscPrepDataCollection*>& cscCols,  
									    const std::vector<const TgcPrepDataCollection*>& tgcCols,  
									    const std::vector<const RpcPrepDataCollection*>& rpcCols,  
									    const MuonSegmentCombinationCollection* cscSegmentCombis ) const {
    // read event_data:
    const MuonHoughHitContainer* hitcontainer = getAllHits( mdtCols, cscCols, tgcCols, rpcCols, cscSegmentCombis );

    // analyse data
    const MuonPatternCombinationCollection* patCombiCol = 0;
    if( hitcontainer ) {
      patCombiCol = analyse( *hitcontainer );
    }else{
      ATH_MSG_INFO (" No hit container created! ");
    }
    // clear hit container
    delete hitcontainer;
    hitcontainer=0;
    
    // ensure we always output a collection
    if( !patCombiCol ){
      ATH_MSG_DEBUG (" NO pattern combinations found, creating empty collection ");
      patCombiCol = new MuonPatternCombinationCollection();
    }

    // summary
    if (m_summary==true || this->msgLvl(MSG::DEBUG)) {
      if( patCombiCol->empty() ) msg() << MSG::DEBUG << " summarizing output: Combined pattern combination empty" << endmsg;
      else msg() << MSG::DEBUG << " summarizing Combined pattern combination output: " << std::endl 
		 << m_printer->print( *patCombiCol ) << endmsg;
    }

    // clean up tool for next call
    cleanUp();
    m_count++;
  
    ATH_MSG_VERBOSE ("execute(end) ");

    // return result
    return patCombiCol;
  } 

  void MuonHoughPatternFinderTool::cleanUp() const {
    //clear csc association map
    m_cschitsegassociation.clear();

    //clear etaphi association map
    m_phietahitassociation->clear();

    //clear stationmaps
    m_rpcmdtstationmap.clear();
    m_tgcmdtstationmap.clear();

  }

  StatusCode MuonHoughPatternFinderTool::finalize()
  {
    if (m_use_histos == true) {
      m_file->Write();
      m_file->Close();
      ATH_MSG_DEBUG ("MuonHoughPatternFinderTool:: delete rootfile");
      delete m_file;
      m_file=0;
      ATH_MSG_DEBUG ("MuonHoughPatternFinderTool::delete Histogram: ");
    }
    delete m_phietahitassociation;
    m_phietahitassociation = 0;
    ATH_MSG_VERBOSE ("finalize()");

    return StatusCode::SUCCESS;
  }

  const MuonPatternCombinationCollection* MuonHoughPatternFinderTool::analyse( const MuonHoughHitContainer& hitcontainer ) const {
    
    ATH_MSG_DEBUG ("size of event: " << hitcontainer.size());

    //  pass through hitcontainer (better still: preprawdata and only after make internal hitcontainer)
    m_muonHoughPatternTool->makePatterns(&hitcontainer);
      
    MuonPrdPatternCollection* phipatterns = m_muonHoughPatternTool->getPhiMuonPatterns();
    MuonPrdPatternCollection* etapatterns = m_muonHoughPatternTool->getEtaMuonPatterns();
    
    if (m_summary==true || this->msgLvl(MSG::DEBUG)) {
      if( phipatterns->empty() ) msg() << MSG::DEBUG << " summarizing input: Phi pattern combination empty" << endmsg;
      else msg() << MSG::DEBUG << " summarizing Phi pattern combination input: " << std::endl << m_printer->print( *phipatterns ) << endmsg;
      if( etapatterns->empty() ) msg() << MSG::DEBUG << " summarizing input: Eta pattern combination empty" << endmsg;
      else msg() << MSG::DEBUG << " summarizing Eta pattern combination input: " << std::endl << m_printer->print( *etapatterns ) << endmsg;
    }
    
    ATH_MSG_DEBUG ("writePatterns");
    ATH_MSG_DEBUG ("size: phi: " << phipatterns->size() << " eta: " << etapatterns->size());
   
    
    const MuonPrdPatternCollection* combinedpatterns = 0;
    const MuonPatternCombinationCollection* patterncombinations = 0;
    
    // make + write muonpatterncombinations
    if ( !etapatterns->empty() ) {
      
      m_muonCombinePatternTool->setPhiEtaHitAssMap(m_phietahitassociation);
      combinedpatterns = m_muonCombinePatternTool->combineEtaPhiPatterns(phipatterns,etapatterns);

    }

    if (combinedpatterns) {
      patterncombinations = m_muonCombinePatternTool->makePatternCombinations(combinedpatterns);
    }else{
      ATH_MSG_DEBUG ("No combined patterns, creating dummy.");
      combinedpatterns =  new MuonPrdPatternCollection();
    }

    record( phipatterns, m_CosmicPhiPatternsKey );
    record( etapatterns, m_CosmicEtaPatternsKey );
    record( combinedpatterns, m_COMBINED_PATTERNSKey );
    
    return patterncombinations;
  }



  const MuonHoughHitContainer* MuonHoughPatternFinderTool::getAllHits( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
								       const std::vector<const CscPrepDataCollection*>& cscCols,  
								       const std::vector<const TgcPrepDataCollection*>& tgcCols,  
								       const std::vector<const RpcPrepDataCollection*>& rpcCols,  
								       const MuonSegmentCombinationCollection* cscSegmentCombis ) const
  {
    ATH_MSG_VERBOSE ("getAllHits()");

    MuonHoughHitContainer* hitcontainer = new MuonHoughHitContainer;
    //reserve space for 5000 hits (arbitrary), this should gain some cpu/memory for background, but will lose for lower occupancy. If anyone knows a way to predict the number of muon hits, I'd like to hear it.
    hitcontainer->reserve(5000);

    bool use_csc_segments = cscSegmentCombis != 0;

    if (use_csc_segments == true && m_use_csc == true)
      {

  // m_csc_segments = cscSegmentCombis;
	MuonSegmentCombinationCollection::const_iterator msc = cscSegmentCombis->begin();
	MuonSegmentCombinationCollection::const_iterator msc_end = cscSegmentCombis->end();

	std::set<Identifier> csc_set; // set to make sure every csc hit is only passed to hitcontainer once
	std::pair<std::set<Identifier>::iterator,bool> csc_pair; 
	std::map <int, int> number_of_hits_per_layer; // map that connect layer number (1000*eta + 100*phi + 10*chamberlayer+ 2*wirelayer + eta/phi)

	std::vector<const Muon::CscClusterOnTrack*> csc_rots; // csc rots
	std::vector<const Muon::CscPrepData*> csc_prds;

	std::vector<int> layer_ids; // if 0 then prd already added

	csc_rots.reserve(400); // again arbitrary, atm (May 2008), the max number of csc segments is 50 (times 8 hits = 400)
	csc_prds.reserve(400);
	layer_ids.reserve(400);

	// two loops needed as number of hits per layer needs to be known

	for(; msc != msc_end; ++msc)
	  {
	    ATH_MSG_VERBOSE ("CSC combo segments loop, segmentcombo " << (*msc));
	    for (unsigned int ss=0; ss<(*msc)->numberOfStations(); ss++)
	      {
		const Muon::MuonSegmentCombination::SegmentVec* segmentsInCombo = (*msc)->stationSegments(ss);
          
		Muon::MuonSegmentCombination::SegmentVec::const_iterator ms = segmentsInCombo->begin();
		Muon::MuonSegmentCombination::SegmentVec::const_iterator ms_end = segmentsInCombo->end();
          
		for (; ms != ms_end; ++ms)
		  {
		    ATH_MSG_VERBOSE ("CSC segments loop, segment: " << (*ms));
		    std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> phi_set;
		    std::vector<const Trk::PrepRawData*> eta_vector;
		    std::pair<std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess>::iterator,bool> phi_pair;
          
		    int nRoTs = (*ms)->numberOfContainedROTs();
		    for(int i=0; i<nRoTs; ++i) 
		      {
			const Muon::CscClusterOnTrack* cscOnSeg = dynamic_cast<const Muon::CscClusterOnTrack*> ((*ms)->rioOnTrack(i));
			if (!cscOnSeg) {ATH_MSG_INFO ("Dynamic cast to CscClusterOnTrack failed!"); continue;}
			const Muon::CscPrepData* prd = dynamic_cast<const Muon::CscPrepData*> (cscOnSeg->prepRawData());
			if (!prd) {ATH_MSG_INFO ("Dynamic cast to CscPrepData failed!"); continue;}

			// hit-segment association, stored for use in downstream algorithms
			m_cschitsegassociation.insert(std::make_pair(prd,(*msc)));

			csc_rots.push_back(cscOnSeg);
			csc_prds.push_back(prd);

			Identifier id = prd->identify();
			bool channel_type = m_cscIdHelper->measuresPhi(id);
			csc_pair = csc_set.insert(id);
			if (csc_pair.second == false) {
			  ATH_MSG_DEBUG (" CSC hit was already added, weight set to 0");
			  layer_ids.push_back(0);
			}
			else {
			  const int layer_id = 1000*m_cscIdHelper->stationEta(id) + 100*m_cscIdHelper->stationPhi(id) + 10*m_cscIdHelper->chamberLayer(id) + 2*m_cscIdHelper->wireLayer(id) + channel_type;
			  ATH_MSG_DEBUG ("csc layer_id: " << layer_id);
			  ++number_of_hits_per_layer[layer_id];
			  layer_ids.push_back(layer_id);
			}
			
			if (channel_type == true) { //phi hit
			  phi_pair = phi_set.insert(prd);
			  if (phi_pair.second == false) {
			    ATH_MSG_INFO (" CSC phi hit was already added");
			  }
			}
			else { //eta hit
			  eta_vector.push_back(prd);
			}
		      } // rots
		    // add hit association from segment to map: 
		    if (!phi_set.empty()){
		      ATH_MSG_VERBOSE ("Number of Phi Csc hits in segment: " << phi_set.size());
		      std::vector <const Trk::PrepRawData*>::iterator vec_it = eta_vector.begin();
		      for (;vec_it!=eta_vector.end(); ++vec_it) {
			m_phietahitassociation->insert(std::make_pair(*vec_it,phi_set));
		      }
		    }
		  }
	      }
	  }
	
	for(unsigned int i=0; i<csc_rots.size(); i++) 
	  {
	    const Muon::CscPrepData* prd = csc_prds[i];
	    
	    const Amg::Vector3D globalpos = csc_rots[i]->globalPosition();
	    double hitx=globalpos.x();
	    double hity=globalpos.y();
	    double hitz=globalpos.z();
	    
	    bool channel_type = m_cscIdHelper->measuresPhi(csc_rots[i]->identify());

	    double weight = 0.;
	    if (layer_ids[i] != 0) { // not yet added
	      double number_of_hits = (double) number_of_hits_per_layer[layer_ids[i]];
	      weight = m_weight_csc_on_segment / (0.75*std::sqrt(number_of_hits)+0.25*number_of_hits);
	    }
	    
	    ATH_MSG_DEBUG (m_printer->print( *prd ) << " weight " << weight);
	    MuonHoughHit* hit = new MuonHoughHit(hitx,hity,hitz,channel_type,MuonHough::CSC,1.,weight,prd);
	    
	    hitcontainer->addHit(hit);
	    if (m_use_histos == true) {
	      m_weighthistogram->Fill(weight);
	      m_weighthistogramcsc->Fill(weight);
	    }
	  }
      } // use_csc_segments
    // taken and modified from DetectorDescription/GeoModel/HitDisplay/src/HitDisplaySystem.cxx
    
    if (m_use_rpc == true)
      {
	std::vector< const RpcPrepDataCollection*>::const_iterator it = rpcCols.begin();
	std::vector< const RpcPrepDataCollection*>::const_iterator it_end = rpcCols.end();
	for( ;it!=it_end;++it ){
	  Muon::RpcPrepDataCollection::const_iterator cit_begin = (*it)->begin();
	  Muon::RpcPrepDataCollection::const_iterator cit_end = (*it)->end();
	  addRpcCollection(cit_begin,cit_end,hitcontainer);
	}
      }
    
    if (m_use_tgc == true)
      {
	std::vector< const TgcPrepDataCollection*>::const_iterator it = tgcCols.begin();
	std::vector< const TgcPrepDataCollection*>::const_iterator it_end = tgcCols.end();
	for( ;it!=it_end;++it ){
	  Muon::TgcPrepDataCollection::const_iterator cit_begin = (*it)->begin();
	  Muon::TgcPrepDataCollection::const_iterator cit_end = (*it)->end();
	  addTgcCollection(cit_begin,cit_end,hitcontainer);
	}
      }

    if (m_use_mdt == true) 
      {
	std::vector< const MdtPrepDataCollection*>::const_iterator it = mdtCols.begin();
	std::vector< const MdtPrepDataCollection*>::const_iterator it_end = mdtCols.end();
	for( ;it!=it_end;++it ){
	  Muon::MdtPrepDataCollection::const_iterator cit_begin = (*it)->begin();
	  Muon::MdtPrepDataCollection::const_iterator cit_end = (*it)->end();
	  addMdtCollection(cit_begin,cit_end,hitcontainer);
	}
      }

    if (m_use_csc == true && use_csc_segments == false)
      {
	std::vector< const CscPrepDataCollection*>::const_iterator it = cscCols.begin();
	std::vector< const CscPrepDataCollection*>::const_iterator it_end = cscCols.end();
	for( ;it!=it_end;++it ){
	  Muon::CscPrepDataCollection::const_iterator cit_begin = (*it)->begin();
	  Muon::CscPrepDataCollection::const_iterator cit_end = (*it)->end();
	  addCscCollection(cit_begin,cit_end,hitcontainer);
	}
      }
  
     
    // if statement for verbose:

    if (this->msgLvl (MSG::VERBOSE)) {
    
      ATH_MSG_VERBOSE ("MuonHoughPatternFinderTool::getAllHits() saving "
                       << hitcontainer->size()<<" converted hits");
    
      for(unsigned int i=0;i<hitcontainer->size();i++)
	{
	  ATH_MSG_VERBOSE (" hit " << hitcontainer->getHit(i)->getWhichDetector()
                           << " (" << hitcontainer->getHit(i)->getHitx() << "," << hitcontainer->getHit(i)->getHity()
                           << "," << hitcontainer->getHit(i)->getHitz() << ") " << " weight: " << hitcontainer->getHit(i)->getWeight() << " measures phi: " << hitcontainer->getHit(i)->getMeasuresPhi());
	}
    
      ATH_MSG_VERBOSE ("MuonHoughPatternFinderTool::getAllHits() saving " << m_phietahitassociation->size() 
                       << "associated hits ");
    }

    return hitcontainer;
  
  } // getAllHits

  void MuonHoughPatternFinderTool::record( const MuonPrdPatternCollection* patCol, const  SG::WriteHandleKey<MuonPrdPatternCollection> &key ) const {
    
    if( !patCol ) {
      ATH_MSG_WARNING ("Zero pointer, could not save patterns!!! ");
      return;
    }

    // check whether we are writing patterns to storegate, if not delete pattern
    if( !m_recordAllOutput ){
      ATH_MSG_DEBUG ("Deleted patterns: " << patCol->size() << "  at " << key.key());

      // since patCol Datavector, it owns (by defaults its elements)
      delete patCol;
    }
    else {
      SG::WriteHandle<MuonPrdPatternCollection> handle(key);
      StatusCode sc = handle.record(std::unique_ptr<MuonPrdPatternCollection>(const_cast<MuonPrdPatternCollection*> (patCol)));
      if ( sc.isFailure() ){
	ATH_MSG_WARNING ("Could not save patterns at " << key.key());
      }
      else{
	ATH_MSG_DEBUG ("Saved patterns: " << patCol->size() << "  at " << key.key());
      }
    }
  }

  bool MuonHoughPatternFinderTool::cut() const
  {
    if (1)
      {
	//msg() << MSG::VERBOSE << "Event through Cut()" << endmsg;
	return true;
      }

    else {
      //msg() << MSG::VERBOSE << "Event not through Cut()" << endmsg;
      return false;
    }
  }

  void MuonHoughPatternFinderTool::addRpcCollection(Muon::RpcPrepDataCollection::const_iterator cit_begin, Muon::RpcPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer) const
  {
    if (cit_begin == cit_end) return;
    Muon::RpcPrepDataCollection::const_iterator cit = cit_begin;   
    std::map <int, int> number_of_hits_per_layer;
    std::set <int> layers; // different layer definition between the two!! 
      
    int size_begin = hitcontainer->size();

    if (m_hit_reweights) // reweight  hits, according to Peter's new algorithm
      {
	std::vector <double> strips(2*m_rpcIdHelper->stripMax()+2); // filled strips, to determine whether it was no noise rpc hit (confirmation of ((neighbouring) layer))
      
	for( ; cit!=cit_end;++cit ) {
	  const Muon::RpcPrepData* prd = *cit;
	  const bool channel_type = m_rpcIdHelper->measuresPhi(prd->identify());
	  const Identifier id = prd->identify();
	  int strip = m_rpcIdHelper->strip(id); // strip between 1 and 99!!
	  if (channel_type == true) {strip += m_rpcIdHelper->stripMax();}
	  strips[strip]+=1.;
	  strips[strip+1]+=0.5;
	  strips[strip-1]+=0.5;
	}
	cit = cit_begin;   
	for( ; cit!=cit_end;++cit ) 
	  {
	    const Muon::RpcPrepData* prd = *cit;
	    Identifier id = prd->identify();
	    const bool channel_type = m_rpcIdHelper->measuresPhi(prd->identify());
	    int strip = m_rpcIdHelper->strip(id); // strip between 1 and 99!!
	    if (channel_type == true) {strip += m_rpcIdHelper->stripMax();}

	    if (strips[strip] > 1) {
        
	      const int doubletR = m_rpcIdHelper->doubletR(id);
	      const int doubletPhi = m_rpcIdHelper->doubletPhi(id);
	      const int doubletZ = m_rpcIdHelper->doubletZ(id);
	      const int gasGap = m_rpcIdHelper->gasGap(id);
	      int layer_number = (gasGap - 1) * 12  + (doubletR-1) * 6 + (doubletPhi -1) * 3 + (doubletZ -1); // layer_number ranges from 0..35
	      if (channel_type) layer_number = layer_number + 36;

	      ++number_of_hits_per_layer[layer_number];
        
	      int layer =  2*(doubletR-1) + (gasGap-1); // layer ranges from 0..8
	      if (channel_type) layer = layer + 4;
        
	      layers.insert(layer);
	      ATH_MSG_VERBOSE ("layer_number: " << layer_number << " doubletR: " << doubletR 
                               << " doubletZ: " << doubletZ << " doubletPhi " << doubletPhi
                               << " gasGap " << gasGap << " layer " << layer);
	    }
	  }
      }

    std::map<const Identifier,std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> > gasgapphimap; // map between gasgapidentifier and phi hits
    std::map<const Identifier,std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >::iterator gg_it;
    std::pair<std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess>::iterator,bool> gg_insert;

    cit = cit_begin;
      
    for( ; cit!=cit_end;++cit ){
      const Muon::RpcPrepData* prd = *cit;
    
      const Amg::Vector3D globalpos = prd->globalPosition();
      const Identifier id = prd->identify();
      const double hitx=globalpos.x();
      const double hity=globalpos.y();
      const double hitz=globalpos.z();
    
      const bool channel_type = m_rpcIdHelper->measuresPhi(id);
    
      // gasgapmap
    
      if (channel_type == true) // phi hit
	{
	  const Identifier gasGapId = m_idHelperTool->gasGapId(id);
	  gg_it = gasgapphimap.find(gasGapId);
	  if (gg_it==gasgapphimap.end()){ // gasgapid not yet in map
	    std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> hitset;
	    hitset.insert(prd);
	    gasgapphimap.insert(std::make_pair(gasGapId,hitset));
	  }
	  else { // gasgapid already in set
	    gg_insert = (*gg_it).second.insert(prd);
	    if (gg_insert.second == false) {
	      ATH_MSG_DEBUG ("WARNING::RPC hit already in set? ");
	    }
	  }
	}
    
      double weight = 1.;
      double prob = 1.;
    
      if (m_hit_reweights)
	{
	  if (layers.size()<=1) {
	    weight = 0.;
	    prob = 0.;
	  }
	  else {
          
	    const int doubletZ = m_rpcIdHelper->doubletZ(id);
	    const int doubletPhi = m_rpcIdHelper->doubletPhi(id);
	    const int doubletR = m_rpcIdHelper->doubletR(id);
	    const int gasGap = m_rpcIdHelper->gasGap(id);     
	    int layer_number = (gasGap - 1) * 12  + (doubletR-1) * 6 + (doubletPhi -1) * 3 + (doubletZ -1); // layer_number ranges from 0..35
	    if (channel_type) layer_number = layer_number + 36;
    
	    double number_of_hits = (double) number_of_hits_per_layer[layer_number];
          
	    if (number_of_hits > 0) {    
//	      weight = 1. / (0.75*std::sqrt(number_of_hits)+0.25*number_of_hits);
	      weight = 1. / (0.25*std::sqrt(number_of_hits)+0.75*number_of_hits);
              if( layers.size() == 2) weight = weight/2.;
	    } else {
	      weight = 0.;
	      prob = 0.;
	    }
	  }
	}
      MuonHoughHit* hit = new MuonHoughHit(hitx,hity,hitz,channel_type,MuonHough::RPC,prob,weight,prd);
      hitcontainer->addHit(hit);
      ATH_MSG_DEBUG (m_printer->print( *prd ) << " NEW weight " << weight);

      if (m_use_histos == true) {
	m_weighthistogram->Fill(weight);
	m_weighthistogramrpc->Fill(weight);
      }
    }

    int size_end = hitcontainer->size();

    updateRpcMdtStationMap((*cit_begin)->identify(),size_begin,size_end);
      
    // extract preprawdata from gasgapmap // might not be fastest way (filling immidiately saves this second loop)
      
    cit = cit_begin;
      
    for( ; cit!=cit_end;++cit ){
      const Muon::RpcPrepData* prd = *cit;
      const Identifier id = prd->identify();
      if (m_rpcIdHelper->measuresPhi(id) == false) { // eta hit
      
	const Identifier gasGapId = m_idHelperTool->gasGapId(id);
      
	gg_it = gasgapphimap.find(gasGapId);
	if (gg_it!=gasgapphimap.end()) {
	  m_phietahitassociation->insert(std::make_pair(prd,(*gg_it).second));
	}
      }
    }

  }

  void MuonHoughPatternFinderTool::addTgcCollection(Muon::TgcPrepDataCollection::const_iterator cit_begin, Muon::TgcPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer) const
  {
    if (cit_begin == cit_end) return;
    Muon::TgcPrepDataCollection::const_iterator cit = cit_begin;
    std::map <int, int> number_of_hits_per_layer;
    std::set <int> layers; // different layer definition between the two!! 

    int size_begin = hitcontainer->size();

    if (m_hit_reweights)
      {
	std::vector <double> channels(2*m_tgcIdHelper->channelMax()+2); // filled strips, to determine whether it was no noise rpc hit (confirmation of ((neighbouring) layer))
	for( ; cit!=cit_end;++cit )
	  {
	    const Muon::TgcPrepData* prd = *cit;
	    Identifier id = prd->identify();
	    //bool channel_type = prd->channelType(); 
	    bool channel_type = m_tgcIdHelper->isStrip(id); // like measuresPhi()
	    int channel = m_tgcIdHelper->channel(id); // between 1 and 135!
	    if (channel_type==true) {channel+=m_tgcIdHelper->channelMax();} 
	    channels[channel]+=1.;
	    channels[channel+1]+=0.55;
	    channels[channel-1]+=0.55;
	  }
	cit = cit_begin;  
	for( ; cit!=cit_end;++cit )
	  {
	    const Muon::TgcPrepData* prd = *cit;
	    const Identifier id = prd->identify();
	    const bool channel_type = m_tgcIdHelper->isStrip(id); // like measuresPhi()
	    int channel = m_tgcIdHelper->channel(id); // between 1 and 135!
	    if (channel_type==true) {channel+=m_tgcIdHelper->channelMax();} 

	    if (channels[channel]>1) {

	      const int gasgap = m_tgcIdHelper->gasGap(id);
	      int layer_number = (gasgap-1); // layer_number ranges from 0..5
	      if (channel_type==true) {layer_number = layer_number + 3;}
        
	      ++number_of_hits_per_layer[layer_number];
        
	      const int layer = layer_number; // layer ranges from 0..5

	      layers.insert(layer);
	      ATH_MSG_VERBOSE ("gasgap: " << gasgap << " layer: " << layer_number);
	    }
	  }
      }
      
    std::map<const Identifier,std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> > gasgapphimap; // map between gasgapidentifier and phi hits
    std::map<const Identifier,std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >::iterator gg_it;
    std::pair<std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess>::iterator,bool> gg_insert;
      
    cit = cit_begin;  
    for( ;cit!=cit_end;++cit )
      {
	const Muon::TgcPrepData* prd = *cit;
	Amg::Vector3D globalpos = prd->globalPosition();
	Identifier id = prd->identify();
	double hitx=globalpos.x();
	double hity=globalpos.y();
	double hitz=globalpos.z();
      
	bool channel_type = m_tgcIdHelper->isStrip(id); // like measuresPhi()
      
	int big_number = 250000;
      
	if (std::isnan(hitx) || std::abs(hitx)>big_number || std::abs(hity) >big_number || std::abs(hitz) > big_number) // to avoid crashing with TGC hits
	  {
	    ATH_MSG_WARNING ("TGC hit not physical: hitx: " << hitx << " hity: " << hity << " hitz: " << hitz);
	  }
	else 
	  {
	    if (channel_type == true) // phi hit
	      {
		const Identifier gasGapId = m_idHelperTool->gasGapId(id);
		gg_it = gasgapphimap.find(gasGapId);
		if (gg_it==gasgapphimap.end()){ // gasgapid not yet in map
		  std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> hitset;
		  hitset.insert(prd);
		  gasgapphimap.insert(std::make_pair(gasGapId,hitset));
		}
		else { // gasgapid already in set
		  gg_insert = (*gg_it).second.insert(prd);
		  if (gg_insert.second == false) {
		    ATH_MSG_DEBUG ("WARNING::TGC hit already in set? ");
		  }
		}
	      }
	    double weight = 1.;
	    double prob = 1.;
	    if (m_hit_reweights==true)
	      {
		if (layers.size() <= 1) {
		  weight = 0.;
		  prob = 0.;
		}
		else {

		  const int gasgap = m_tgcIdHelper->gasGap(id);
		  int layer_number = (gasgap-1); // layer_number ranges from 0..1/2
		  if (channel_type==true) layer_number = layer_number + 3 ;
		  double number_of_hits = (double) number_of_hits_per_layer[layer_number];
		  if (number_of_hits > 0) {    
//		    weight =  1. / (0.75*std::sqrt(number_of_hits)+0.25*number_of_hits);
		    weight =  1. / (0.25*std::sqrt(number_of_hits)+0.75*number_of_hits);
                    if( layers.size() == 2) weight = weight/2.;
		  } else {
		    weight = 0.;
		    prob = 0.;
		  }
		}
	      }
	    MuonHoughHit* hit = new MuonHoughHit(hitx,hity,hitz,channel_type,MuonHough::TGC,prob,weight,prd); //getPrd
	    hitcontainer->addHit(hit);
	    ATH_MSG_DEBUG (m_printer->print( *prd ) << " NEW weight " << weight);
	    if (m_use_histos == true) {
	      m_weighthistogram->Fill(weight);
	      m_weighthistogramtgc->Fill(weight);
	    }
	  }
      }

    int size_end = hitcontainer->size();
    updateTgcMdtStationMap((*cit_begin)->identify(),size_begin,size_end);
      
    // extract preprawdata from gasgapmap // might not be fastest way (filling immidiately saves this second loop)
      
    cit = cit_begin;
  
    for( ; cit!=cit_end;++cit ){
      const Muon::TgcPrepData* prd = *cit;
      const Identifier id = prd->identify();
      if (m_tgcIdHelper->isStrip(id) == false) { // eta hit
      
	const Identifier gasGapId = m_idHelperTool->gasGapId(id);
      
	gg_it = gasgapphimap.find(gasGapId);
	if (gg_it!=gasgapphimap.end()) {
	  m_phietahitassociation->insert(std::make_pair(prd,(*gg_it).second));
	}
      }
    }

  }

  void MuonHoughPatternFinderTool::addMdtCollection(Muon::MdtPrepDataCollection::const_iterator cit_begin, Muon::MdtPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer) const
  {
    if (cit_begin == cit_end) return;

    const int size = std::distance(cit_begin,cit_end);

    if (m_showerskip == true) { 
      const Muon::MdtPrepData* mdt = (*cit_begin);
      const MuonGM::MdtReadoutElement* detEl = mdt->detectorElement();
      unsigned int channels = 2 * detEl->getNLayers() * detEl->getNtubesperlayer(); // Factor 2 for number of multilayers, should be changed when only 1 detector element per chamber (the chambers with only 1 multilayer have a twice less severe cut (for convenience))
      double occupancy = (double)size / (double)channels;

      ATH_MSG_DEBUG (" size: " << size << " channels: " << channels << " occupancy: " << occupancy);

      // if more than m_showerskipperc (default 30%) of all hits in the chamber is hit then all weights to 0
      // only done for large chambers (more than 50 hits) 
      if (occupancy > m_showerskipperc && size > 50) {

	ATH_MSG_DEBUG ("Chamber skipped! Too high occupancy (>" << m_showerskipperc << "%): " << occupancy << " association to pattern still possible");

	Muon::MdtPrepDataCollection::const_iterator cit = cit_begin;   
	for( ; cit!=cit_end;++cit ) // first
	  {
	    const Muon::MdtPrepData* mdt = (*cit);
      
	    if (m_mdt_tdc_cut == true && mdt->status()!=Muon::MdtStatusDriftTime) continue;
	    
	    if ((m_mdt_adc_cut == true && (mdt->adc() > m_mdt_adc_min)) || m_mdt_adc_cut == false)
	      {
		Identifier id = mdt->identify();
		const Trk::Surface& surface = mdt->detectorElement()->surface(id);
		const Amg::Vector3D& globalpos = surface.center();
		double hitx = globalpos.x();
		double hity = globalpos.y();
		double hitz = globalpos.z();
	      
		MuonHoughHit* hit = new MuonHoughHit(hitx,hity,hitz,false,MuonHough::MDT,0.,0.,mdt); //getPrd
		ATH_MSG_DEBUG (m_printer->print( *mdt ));
		hitcontainer->addHit(hit);
	      }
	  }
	return;
      }
    }

    std::map <int, int> number_of_hits_per_layer;
    std::map <int, int> number_of_hots_per_layer; // number of trigger confirmed or hits on segment within layer (key)

    std::vector<double> hitx;
    std::vector<double> hity;
    std::vector<double> hitz;
    std::vector<double> radius;
    std::vector<double> errradius;
    std::vector<double> weights;
    std::vector<double> prob; // artificial probability that hit belongs to true muon
    std::vector<int> multilayer;
    std::vector<int> tubelayer;
    std::vector<int> tubes;
    //   std::vector<double> w1;
    //   std::vector<double> w2;
    std::vector<int> onsegment; // non-zero if on segment, int indicates how many hits in same layer are on segment (used in weighting)
    std::vector<double> psi;
    std::vector<double> weight_trigger;
    std::vector<int> tr_confirmation; // 1 if confirmation from trigger hits else 0
    std::vector<Identifier> ids;
    std::vector<int> layers;
    std::vector<const Muon::MdtPrepData*> prd;

    hitx.reserve(size);
    hity.reserve(size);
    hitz.reserve(size);
    radius.reserve(size);
    errradius.reserve(size);
    weights.reserve(size);
    prob.reserve(size);
    multilayer.reserve(size);
    tubelayer.reserve(size);
    tubes.reserve(size);
    onsegment.reserve(size);
    psi.reserve(size);
    weight_trigger.reserve(size);
    tr_confirmation.reserve(size);
    ids.reserve(size);
    layers.reserve(size);
    prd.reserve(size);
		   
    std::vector <double> tubecount(m_mdtIdHelper->tubeMax()+2);

    Muon::MdtPrepDataCollection::const_iterator cit = cit_begin;   
    for( ; cit!=cit_end;++cit ) // first
      {
	const Muon::MdtPrepData* mdt = (*cit);
      
	if (m_mdt_tdc_cut == true && mdt->status()!=Muon::MdtStatusDriftTime) continue;
      
	if ((m_mdt_adc_cut == true && (mdt->adc() > m_mdt_adc_min)) || m_mdt_adc_cut == false)
	  {
	    Identifier id = mdt->identify();
      
	    prd.push_back(mdt);
	    const Trk::Surface& surface = mdt->detectorElement()->surface(id);
	    const Amg::Vector3D& globalpos = surface.center();
	    hitx.push_back(globalpos.x());
	    hity.push_back(globalpos.y());
	    hitz.push_back(globalpos.z());
	    radius.push_back(mdt->localPosition()[0]);
	    errradius.push_back(Amg::error(mdt->localCovariance(),0));
	    weights.push_back(1.);
	    prob.push_back(1.);
	    const int tube = m_mdtIdHelper->tube(id);
	    tubes.push_back(tube);
	    onsegment.push_back(0);
	    psi.push_back(0.);
	    weight_trigger.push_back(0.);
	    tr_confirmation.push_back(0); 
	    ids.push_back(id);

	    const int multi_layer = m_mdtIdHelper->multilayer(id);
	    const int tube_layer = m_mdtIdHelper->tubeLayer(id);
	    multilayer.push_back(multi_layer);
	    tubelayer.push_back(tube_layer);

	    int layer_number = (multi_layer-1)*m_mdtIdHelper->tubeLayerMax() + (tube_layer-1); // layer_number ranges from 0..5/7
	    layers.push_back(layer_number);

	    tubecount[tube]+=1.;
	    tubecount[tube-1]+=0.5;
	    tubecount[tube+1]+=0.5;

	    ATH_MSG_VERBOSE (" layer_number: " << layer_number << " multi_layer: " << multi_layer
                             << " tube_layer: " << tube_layer);

	  } // adc cut
      }

    const unsigned int prdsize = prd.size();

    if (prdsize == 0) return;

    if (false == m_hit_reweights) {
      for (unsigned int i=0; i<prdsize; i++) {
      
	MuonHoughHit* hit = new MuonHoughHit(hitx[i],hity[i],hitz[i],false,MuonHough::MDT,1.,1.,prd[i]); //getPrd
	ATH_MSG_DEBUG (m_printer->print( *prd[i] ));
	hitcontainer->addHit(hit);
      }
      return;
    }

    double tubem = *(std::max_element(tubecount.begin(),tubecount.end()));

    if (tubem < 2.01) // allweights 0
      {
	ATH_MSG_VERBOSE (" TOO SMALL tubem : " << tubem);
	for (unsigned int i=0; i<prdsize; i++) {
	  MuonHoughHit* hit = new MuonHoughHit(hitx[i],hity[i],hitz[i],false,MuonHough::MDT,0.,0.,prd[i]); //getPrd
	  ATH_MSG_DEBUG (m_printer->print( *prd[i] ) << " weight " << 0 << " adc: " << prd[i]->adc());
	  hitcontainer->addHit(hit);
	  if (m_use_histos == true) {
	    m_weighthistogram->Fill(0);
	    m_weighthistogrammdt->Fill(0);
	  }
	  //msg() << MSG::DEBUG << "Hit accepted" << endmsg;
    
	} // collection
	return;
      }

    // fast segment search:

    for (unsigned int i=0; i<prdsize; i++) {

      if (tubecount[tubes[i]] > 1 ) ++number_of_hits_per_layer[layers[i]];
      //    if (tubecount[i] > tubem ) tubem = tubecount[i];

      // KILL 1 hit cases
      if (tubecount[tubes[i]] <= 1 ) prob[i] = 0;
    } // end hit loop i
  
    int ml1 = 0;
    int ml2 = 0;

    std::map <int, int>::iterator map_it=number_of_hits_per_layer.begin();
    for (; map_it!=number_of_hits_per_layer.end(); ++map_it) {

      if ((*map_it).first >= m_mdtIdHelper->tubeLayerMax()) {ml1++;}
      else { ml2++;}
    }

    if (ml1 + ml2 < 2.01) // allweights = 0
      {
	ATH_MSG_VERBOSE (" TOO SMALL ml1 + ml2 : " << ml1 << " ml2 " << ml2);
	for (unsigned int i=0; i<prdsize; i++) {
	  MuonHoughHit* hit = new MuonHoughHit(hitx[i],hity[i],hitz[i],false,MuonHough::MDT,0.,0.,prd[i]); //getPrd
	  ATH_MSG_DEBUG (m_printer->print( *prd[i] ) << " weight " << 0);
	  hitcontainer->addHit(hit);
	  if (m_use_histos == true) {
	    m_weighthistogram->Fill(0);
	    m_weighthistogrammdt->Fill(0);
	  }
	} // collection
	return;
      }

    bool barrel = m_mdtIdHelper->isBarrel(ids[0]);

    DCVec dcs;
    dcs.reserve(prdsize);
    for (unsigned int i=0; i<prdsize; i++) {
      if (prob[i] < 0.01) continue;
      double global_radius = std::sqrt(hitx[i]*hitx[i]+hity[i]*hity[i]); // global radius
      const TrkDriftCircleMath::LocPos lpos(global_radius,hitz[i]); // global coordinates
      // create identifier
      TrkDriftCircleMath::MdtId mdtid( barrel, multilayer[i]-1, tubelayer[i]-1 ,tubes[i]-1 );
      // create new DriftCircircleMath::DriftCircle::DriftState
      TrkDriftCircleMath::DriftCircle dc(lpos, radius[i], errradius[i], TrkDriftCircleMath::DriftCircle::InTime , mdtid, i);
      dcs.push_back( dc );
    }
    
    bool seg_found = true;
    while (true == seg_found) {
    
      std::vector <int> sel(dcs.size());
      double angleDif=0.;
    
      fastSegmentFinder(dcs,ml1,ml2,angleDif,sel);
   
      if (ml1+ml2 >= 2.1) {
	int removed_hits = 0; // keeps track of number of removed hits
	for (unsigned int i=0; i<sel.size(); i++) {

	  if (sel[i] != 0) {
	    
	    unsigned int j = (unsigned int) dcs[i-removed_hits].index(); // index of position in prd vec
	    onsegment[j]=1; 
	    psi[j] = angleDif;

	    ++number_of_hots_per_layer[layers[j]];
	    
	    // remove hit from dcs container for next iteration!!
	    dcs.erase(dcs.begin()+i-removed_hits);
	    removed_hits++;
	  }
	}
      }
      else {seg_found = false;}
    }

    // trigger confirmation checks:

    int stationcode = stationCode(ids[0]);

    // rpc:

    std::map<int,std::vector<std::pair<int, int> > >::const_iterator stationmap_it = m_rpcmdtstationmap.find(stationcode);

    if (stationmap_it!=m_rpcmdtstationmap.end()) {

      const std::vector<std::pair<int, int> > &stationhits = (*stationmap_it).second;

      // stationloop
      for (unsigned int i=0; i<stationhits.size(); i++) {
	// rpc hit loop
	for (int j=stationhits[i].first; j<stationhits[i].second; j++) {
    
	  const MuonHoughHit* rpchit = hitcontainer->getHit(j);
	  if (rpchit->getWeight() < 0.01) continue;
    
	  const double rpcx = hitcontainer->getHitx(j);
	  const double rpcy = hitcontainer->getHity(j);
	  const double rpcz = hitcontainer->getHitz(j);
	  const double rpc_radius = std::sqrt(rpcx*rpcx+rpcy*rpcy);
	  const double rpc_rz_ratio = rpc_radius / rpcz;
	  const double rpc_inv_rz_ratio = 1./rpc_rz_ratio;

	  for (unsigned int k = 0; k < prdsize; k++) {   
	    // Mdt hit loop 
	    double dis = 0.;
	    if (true == barrel) {
	      dis = hitz[k]-std::sqrt(hitx[k]*hitx[k]+hity[k]*hity[k]) * rpc_inv_rz_ratio;
	    }
	    else { // can that happen?
	      dis = std::sqrt(hitx[k]*hitx[k]+hity[k]*hity[k]) - rpc_rz_ratio*hitz[k];
	    }
      
	    if (weight_trigger[k] < 0.1) {weight_trigger[k] = 1.;}
      
	    if(std::abs(dis) < 250.) {
	      double wnew = 1.5 + (250. - std::abs(dis))/251.;
	      if(wnew > weight_trigger[k]) {weight_trigger[k] = wnew;}
	    }
	  }
	}
      }
    }

    // tgc:

    stationmap_it = m_tgcmdtstationmap.find(stationcode);
  
    if (stationmap_it!=m_tgcmdtstationmap.end()) {

      const std::vector<std::pair<int, int> > &stationhits = (*stationmap_it).second;

      // stationloop
      for (unsigned int i=0; i<stationhits.size(); i++) {
	// tgc hit loop
	for (int j=stationhits[i].first; j<stationhits[i].second; j++) {
    
	  const MuonHoughHit* tgchit = hitcontainer->getHit(j);
	  if (tgchit) {
	    if (tgchit->getWeight() < 0.01) continue;

	    const double tgcx = hitcontainer->getHitx(j);
	    const double tgcy = hitcontainer->getHity(j);
	    const double tgcz = hitcontainer->getHitz(j);
	    const double tgc_rz_ratio = std::sqrt(tgcx*tgcx+tgcy*tgcy)/tgcz;
      
	    for (unsigned int k = 0; k < prdsize; k++) {   
	      // Mdt hit loop 
	      if (weight_trigger[k] < 0.1) weight_trigger[k] = 3.;
	      double dis = std::sqrt(hitx[k]*hitx[k]+hity[k]*hity[k]) - tgc_rz_ratio*hitz[k]; // only endcap extrapolation
	      if(std::abs(dis) < 250.) {
		double wnew = 3.5 + (250.-std::abs(dis))/251.;
		if(wnew > weight_trigger[k]) weight_trigger[k] = wnew;
	      }
	    }
	  }
	}
      } 
    }

    // define trigger confirmation:

    for (unsigned int i=0; i<prdsize; i++) {

      // for MDTs require trigger chamber confirmation
      //                  or segment with selected hits 
      
      if (weight_trigger[i] > 1.5 && weight_trigger[i] < 2.55 ) tr_confirmation[i] = 1;
      if (weight_trigger[i] > 3.5 && weight_trigger[i] < 4.55 ) tr_confirmation[i] = 1;
      
      // add confirmed hits to hots layer count:
      if (tr_confirmation[i] == 1 && onsegment[i] == 0) { // else already added
	++number_of_hots_per_layer[layers[i]];
      }
    }

    // calculate final weights:

    for (unsigned int i=0; i<prdsize; i++) {
      if (prob[i] < 0.01) {weights[i] = 0; continue;} // throw away hits that are not significant

      // correct for several number of hits in layer:
      map_it = number_of_hits_per_layer.find(layers[i]);
      if (map_it!=number_of_hits_per_layer.end()) {
	int layerhits = (*map_it).second;
	double layer_weight = 1./(0.25*layerhits + 0.75*std::sqrt(layerhits)); 

	if (0==tr_confirmation[i] && onsegment[i] == 0) {
	  // downweighting for non-confirmed hits:
	  prob[i] = prob[i] - 0.2;
	  if (prob[i] < 0 ) prob[i] = 0.;
	
	  // correct for several number of hits in layer:
	  weights[i] = prob[i] * layer_weight;
	}
      
	else {
	  // Correct probabilities for hits on segment or confirmed by RPC/TGC 
	  double rej = 1. / (1. - layer_weight + 0.10); 
	  double rej0 = 1.; // irrevelant value
	  
	  if (onsegment[i] != 0 && tr_confirmation[i] !=0) {rej0 = 30;}
	  else if (onsegment[i] != 0) {rej0 = 1.75/(psi[i] + 0.05);}  // 1.75 = 5*0.35
	  else if (tr_confirmation[i] !=0) {rej0 = 8;}
	
	  double rej_total = rej*rej0;
	  prob[i] = rej_total / (1. + rej_total);  
	
	  // correct for several number of confirmed hits in layer:
	  map_it = number_of_hots_per_layer.find(layers[i]);
	  if (map_it!=number_of_hots_per_layer.end()) {
	    int layerhits_conf = (*map_it).second;
	    weights[i] = prob[i]/(0.25*layerhits_conf + 0.75*std::sqrt(layerhits_conf)); 
	  }
	  else {
	    ATH_MSG_INFO ("Entry not in map! This should not happen");
	    weights[i] = prob[i];
	  }
	}
      }
      else {
	ATH_MSG_INFO ("Entry not in map! This should not happen");
	weights[i] = prob[i];
      }
    }
    
    // and finally add hits to container:

    for (unsigned int i=0; i<prdsize; i++) {

      MuonHoughHit* hit = new MuonHoughHit(hitx[i],hity[i],hitz[i],false,MuonHough::MDT,prob[i],weights[i],prd[i]); //getPrd
      ATH_MSG_DEBUG (m_printer->print( *prd[i] ) << " trigger weight " <<  weight_trigger[i] << " on segment " << onsegment[i] << " psi " << psi[i] << " prob " << prob[i] << " weight " << weights[i]);
      hitcontainer->addHit(hit);
      if (m_use_histos == true) {
	m_weighthistogram->Fill(weights[i]);
	m_weighthistogrammdt->Fill(weights[i]);
      }
      //msg() << MSG::DEBUG << "Hit accepted" << endmsg;
    
    } // collection
  }

  void MuonHoughPatternFinderTool::addCscCollection(Muon::CscPrepDataCollection::const_iterator cit_begin, Muon::CscPrepDataCollection::const_iterator cit_end, MuonHoughHitContainer* hitcontainer)const
  {
    if (cit_begin == cit_end) return;
    std::map <int, int> number_of_hits_per_layer;
    if (m_hit_reweights) // reweight  hits, according to Niels' and Peters new algorithm
      {
	Muon::CscPrepDataCollection::const_iterator cit = cit_begin;
	for( ; cit!=cit_end;++cit )
	  {
	    const Muon::CscPrepData* prd = (*cit);
	    Identifier id = prd->identify();
      
	    bool channel_type = m_cscIdHelper->measuresPhi(prd->identify());
      
	    const int chamber_layer = m_cscIdHelper->chamberLayer(id);
	    const int chamber_layer_max = m_cscIdHelper->chamberLayerMax(id);
	    const int wire_layer = m_cscIdHelper->wireLayer(id);
	    int layer_number = (chamber_layer-1) + chamber_layer_max *(wire_layer -1); // layer_number ranges from 0..7
	    if (channel_type) layer_number = layer_number + 8 ;

	    ++number_of_hits_per_layer[layer_number];
      
	    ATH_MSG_DEBUG ("chamber_layer: " << chamber_layer << " chamber_layer_max: " << chamber_layer_max << " wire_layer: " << wire_layer);
      
	  }
      }

    std::map<const Identifier,std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> > gasgapphimap; // map between gasgapidentifier and phi hits
    std::map<const Identifier,std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >::iterator gg_it;
    std::pair<std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess>::iterator,bool> gg_insert;

    Muon::CscPrepDataCollection::const_iterator cit = cit_begin;  
    for( ;cit!=cit_end;++cit )
      {
	const Muon::CscPrepData* prd = *cit;
	Amg::Vector3D globalpos = prd->globalPosition();
	Identifier id = prd->identify();
	double hitx=globalpos.x();
	double hity=globalpos.y();
	double hitz=globalpos.z();
      
	bool channel_type = m_cscIdHelper->measuresPhi(id);

	if (channel_type == true) // phi hit
	  {
	    const Identifier gasGapId = m_idHelperTool->gasGapId(id);
	    gg_it = gasgapphimap.find(gasGapId);
	    if (gg_it==gasgapphimap.end()){ // gasgapid not yet in map
	      std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> hitset;
	      hitset.insert(prd);
	      gasgapphimap.insert(std::make_pair(gasGapId,hitset));
	    }
	    else { // gasgapid already in set
	      gg_insert = (*gg_it).second.insert(prd);
	      if (gg_insert.second == false) {
		ATH_MSG_DEBUG ("WARNING::CSC hit already in set? ");
	      }
	    }
	  }
	double weight = 1.;
	if (m_hit_reweights)
	  {
	    const int chamber_layer = m_cscIdHelper->chamberLayer(id);
	    const int chamber_layer_max = m_cscIdHelper->chamberLayerMax(id);
	    const int wire_layer = m_cscIdHelper->wireLayer(id);
	    int layer_number = (chamber_layer-1) + chamber_layer_max *(wire_layer -1); // layer_number ranges from 0..7
	    if (channel_type) layer_number = layer_number + 8 ;
	    double weight_factor_csc = 1.;
	    double number_of_hits = (double) number_of_hits_per_layer[layer_number];
	    weight = weight_factor_csc / std::sqrt(number_of_hits);
      
	    ATH_MSG_DEBUG ("CSC weight: " << weight);
	  }
      
	MuonHoughHit* hit = new MuonHoughHit(hitx,hity,hitz,channel_type,MuonHough::CSC,1.,weight,prd); //getPrd
	hitcontainer->addHit(hit);
	ATH_MSG_DEBUG (m_printer->print( *prd ) << " weight " << weight);
	if (m_use_histos == true) {
	  m_weighthistogram->Fill(weight);
	  m_weighthistogramcsc->Fill(weight);
	}
      }
    // extract preprawdata from gasgapmap // might not be fastest way (filling immidiately saves this second loop)

    cit = cit_begin;
  
    for( ; cit!=cit_end;++cit ){
      const Muon::CscPrepData* prd = *cit;
      const Identifier id = prd->identify();
      if (m_cscIdHelper->measuresPhi(id) == false) { // eta hit
      
	const Identifier gasGapId = m_idHelperTool->gasGapId(id);
      
	gg_it = gasgapphimap.find(gasGapId);
	if (gg_it!=gasgapphimap.end()) {
	  m_phietahitassociation->insert(std::make_pair(prd,(*gg_it).second));
	}
      }
    }
  }

  void MuonHoughPatternFinderTool::updateRpcMdtStationMap(const Identifier rpcid, const int hit_begin, const int hit_end) const
  {
    //  input is a RPC identifier, begin container and end container
    //  rpcmdtstationmap is updated
    //
    // called once per rpc collection/station
  
    std::string st = m_rpcIdHelper->stationNameString(m_rpcIdHelper->stationName(rpcid));
    ATH_MSG_VERBOSE ("updateRpcMdtStationMap" <<  st);
    if (st[0] != 'B') return; // no rpc for endcap chambers

    std::map<int,std::vector<std::pair<int, int> > >::iterator it;
    int stationcode = stationCode(rpcid);

    // store station code 
  
    addToStationMap(m_rpcmdtstationmap,it,stationcode,hit_begin,hit_end);

    int idphi = m_rpcIdHelper->stationPhi(rpcid);
    int ideta = m_rpcIdHelper->stationEta(rpcid);
   
    int idphi1 = idphi - 1;
    if (idphi1 == 0) idphi1 = 8;
    int idphi2 = idphi + 1;
    if (idphi2 >  8) idphi2 = 1;
 
    std::string station;
    if (st == "BOL") {
      station = "BOS";
    } 
    else if (st == "BOS") {
      station = "BOL";
    } 
    else if (st == "BMS") {
      station = "BML";
    } 
    else if (st == "BML") {
      station = "BMS";
    }  
    else return;
 
    // store Neighbouring station codes 
    int stationNameMDT = m_mdtIdHelper->stationNameIndex(station);

    stationcode = stationCode(stationNameMDT,idphi1,ideta);
    addToStationMap(m_rpcmdtstationmap,it,stationcode,hit_begin,hit_end);

    stationcode = stationCode(stationNameMDT,idphi2,ideta);
    addToStationMap(m_rpcmdtstationmap,it,stationcode,hit_begin,hit_end);

    //  Also look into Inner station
    
    if (st == "BMS") {
      station = "BIS";
    }
    else if (st == "BML") {
      station = "BIL";
    }
    else return;
 	  	 
    stationNameMDT = m_mdtIdHelper->stationNameIndex(station);
    stationcode = stationCode(stationNameMDT,idphi,ideta);
    addToStationMap(m_rpcmdtstationmap,it,stationcode,hit_begin,hit_end);
  }

  void MuonHoughPatternFinderTool::updateTgcMdtStationMap(const Identifier tgcid, int hit_begin, int hit_end) const
  {
    //  input is a TGC identifier, begin container and end container
    //  tgcmdtstationmap is updated
    //
    // called once per tgc collection/station
    std::string st = m_tgcIdHelper->stationNameString(m_tgcIdHelper->stationName(tgcid));
    if (st[0] != 'T') return; 

    std::vector <int> T31(5); T31[0] = 2; T31[1] = 3; T31[2] = 3; T31[3] = 4; T31[4] = 4; 
    std::vector <int> T32(5); T32[0] = 3; T32[1] = 4; T32[2] = 4; T32[3] = 5; T32[4] = 5;
    std::vector <int> T11(5); T11[0] = 2; T11[1] = 3; T11[2] = 4; T11[3] = 4; T11[4] = 4; 
    std::vector <int> T12(5); T12[0] = 3; T12[1] = 4; T12[2] = 5; T12[3] = 5; T12[4] = 5;
 
    std::map<int,std::vector<std::pair<int, int> > >::iterator it;
 
    //Determine station phi in MDT 
  
    int modphiTGC = 48; 
    if (st[2]=='F') modphiTGC = 24;
    if (st[1]=='4') modphiTGC = 24;
  
    int idphi = m_tgcIdHelper->stationPhi(tgcid);
    int ideta = m_tgcIdHelper->stationEta(tgcid);
    int index = abs(ideta)-1;
    int idphi1MDT = 1 + int (8.*(idphi+1)/modphiTGC);
    int idphi2MDT = 1 + int (8.*(idphi-1)/modphiTGC);
    if (idphi1MDT > 8) idphi1MDT = 1;
    if (idphi2MDT > 8) idphi2MDT = 1;
  
    int sign = 1;
    if (ideta < 0) sign = -1;
  
    //Determine two station etas  in MDT 
  
    int ideta1MDT = 0; 
    int ideta2MDT = 0; 
    if (st[2]=='F') {
      ideta1MDT = sign*1; 
      ideta2MDT = sign*2; 
    }
    if (st[2]=='E') {
      if (st[1]=='4') {
	// T4 
	ideta1MDT = sign*4; 
	ideta2MDT = sign*5; 
      } else if (st[1] == '3') {
	// T3 
	ideta1MDT = sign*T31[index]; 
	ideta2MDT = sign*T32[index]; 
      } else {
	// T1 or T2
	ideta1MDT = sign*T11[index]; 
	ideta2MDT = sign*T12[index]; 
      }
    }
    std::string station1 = "EML";
    std::string station2 = "EMS";
    if (st[1] == '4') {
      station1 = "EIL";
      station2 = "EIS";
    } 
    int stationNameMDT1 = m_mdtIdHelper->stationNameIndex(station1);
    int stationNameMDT2 = m_mdtIdHelper->stationNameIndex(station2);

    int stationcode = stationCode(tgcid);

    // store station Inner and Middle codes
 	  	 
    stationcode = stationCode(stationNameMDT1,idphi1MDT,ideta1MDT);
    addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
    stationcode = stationCode(stationNameMDT2,idphi1MDT,ideta1MDT);
    addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
    if(ideta1MDT != ideta2MDT) {
      stationcode = stationCode(stationNameMDT1,idphi1MDT,ideta2MDT);
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
      stationcode = stationCode(stationNameMDT2,idphi1MDT,ideta2MDT);
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
    }
    if(idphi1MDT != idphi2MDT) {
      stationcode = stationCode(stationNameMDT1,idphi2MDT,ideta1MDT);
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
      stationcode = stationCode(stationNameMDT2,idphi2MDT,ideta1MDT);
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
      if(ideta1MDT != ideta2MDT) {
	stationcode = stationCode(stationNameMDT1,idphi2MDT,ideta2MDT);
	addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
	stationcode = stationCode(stationNameMDT2,idphi2MDT,ideta2MDT);
	addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
      }
    }
    // Store corresponding Outer stations
 	  	 
    if (station1 == "EMS") {
      station1 = "EOS";
    }
    if (station2 == "EML") {
      station2 = "EOL";
    } else return;
 	  	 
    stationNameMDT1 = m_mdtIdHelper->stationNameIndex(station1);
    stationNameMDT2 = m_mdtIdHelper->stationNameIndex(station2);

    stationcode = stationCode(stationNameMDT1,idphi1MDT,ideta1MDT);
    addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);  
    stationcode = stationCode(stationNameMDT2,idphi1MDT,ideta1MDT);
    addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
    if(ideta1MDT != ideta2MDT) {
      stationcode = stationCode(stationNameMDT1,idphi1MDT,ideta2MDT); 
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);  
      stationcode = stationCode(stationNameMDT2,idphi1MDT,ideta2MDT); 
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);  
    }
    if(idphi1MDT != idphi2MDT) {  
      stationcode = stationCode(stationNameMDT1,idphi2MDT,ideta1MDT);
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
      stationcode = stationCode(stationNameMDT2,idphi2MDT,ideta1MDT);
      addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);

      if(ideta1MDT != ideta2MDT) {
	stationcode = stationCode(stationNameMDT1,idphi2MDT,ideta2MDT);
	addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
	stationcode = stationCode(stationNameMDT2,idphi2MDT,ideta2MDT);
	addToStationMap(m_tgcmdtstationmap,it,stationcode,hit_begin,hit_end);
      }
    }
  }

  int MuonHoughPatternFinderTool::stationCode(Identifier id)const
  {
    return 10000000*m_mdtIdHelper->stationName(id) + 100000*m_mdtIdHelper->stationPhi(id) + 1000*(m_mdtIdHelper->stationEta(id)+10);
  }

  int MuonHoughPatternFinderTool::stationCode(int stationname,int phi,int eta)const
  {
    return 10000000*stationname + 100000*phi + 1000*(eta+10);
  }

  void MuonHoughPatternFinderTool::addToStationMap(std::map<int,std::vector<std::pair<int, int> > > &stationmap, std::map<int,std::vector<std::pair<int, int> > >::iterator &it, int &stationcode, const int &hit_begin, const int &hit_end) const
  {
    it = stationmap.find(stationcode);
    if (it==stationmap.end()){
      std::vector<std::pair<int, int> > dummyvec;
      dummyvec.push_back(std::make_pair(hit_begin,hit_end));
      stationmap[stationcode]=dummyvec;
    }
    else {
      (*it).second.push_back(std::make_pair(hit_begin,hit_end));
    }
  }

  void MuonHoughPatternFinderTool::fastSegmentFinder(TrkDriftCircleMath::DCVec& dcs,int &nl1, int &nl2, double &angleDif, std::vector <int> &sel)const 
  {
    //
    // Input:  vector of driftcircles per chamber 
    // Output: nl1 = segment hits in multilayer 1 and nl2 = segment hits in multilayer 2
    //       : sel(1:dcs.size)  = 0 NOT selected  = 1 on segment
    //
    // Method: constructs the tangent lines to all driftcircle combinations and counts hits in a road of 1.5 mm
    //         segment = combination with most hits
    //         uses TrkDriftCircleMath software 
    //
  
    // Layers with more than 10 hits are skipped as seed, if all layers have more than 10 hits, no fits are tried
  
    nl1 = 0;
    nl2 = 0;
    angleDif = -1.;
    if (dcs.empty() == true) return;
  
    DCCit it_end = dcs.end();
    DCCit it1 = dcs.begin();   
    std::map <int, DCVec > layerHits; // map between layer and driftcircles
    std::map <int , int> dcsId; // map between 'idnumber' and position

    std::map <int, DCVec >::iterator map_it;    
    int nhits = 0;
    for( ;it1!=it_end;++it1, nhits++ ) {
      sel[nhits] = 0;
      int isort = 100*(4*(it1->id().ml()) + it1->id().lay()) + it1->id().tube();
      dcsId[isort] = nhits;
      int ilay = 4*(it1->id().ml()) + it1->id().lay();
      ATH_MSG_VERBOSE (" ilay " << ilay << " isort " << isort);

      map_it = layerHits.find(ilay);
      if (map_it != layerHits.end()) {
	(*map_it).second.push_back(*it1);
      } else {
	DCVec dcl;
	dcl.reserve(dcs.size());
	dcl.push_back(*it1);
	layerHits[ilay] = dcl;
      }
    }
  
    unsigned int nHits = 0; // is maximalized
    unsigned int nHitsLine = 0;
    unsigned int nPassedTubes = 0;
    double roadWidth = 1.5; 
    TrkDriftCircleMath::DCOnTrackVec hitsOnLineSel;
    TrkDriftCircleMath::TangentToCircles tanCreator;
    TrkDriftCircleMath::MatchDCWithLine matchWithLine;  
    bool stop = false;
    for( int i = 0; i < 8; i++ ) {
      if (layerHits.count(i) != 1) continue;
      DCVec& dci = layerHits[i];
      if (dci.size() > 10) continue;
      DCCit iti = dci.begin();
      DCCit iti_end = dci.end();
      for( ;iti!=iti_end;++iti ) {
	// One seed selected
	float tubeRadius=14.6;
	if((*iti).rot()){ //if no access to rot, can't do anything here
	  tubeRadius = (*iti).rot()->detectorElement()->innerTubeRadius();
	}
	for( int j = 7; j > i; j-- ) {
	  if (layerHits.count(j) != 1) continue;    
	  DCVec& dcj = layerHits[j];
	  if (dcj.size() > 10) continue;
	  DCCit itj = dcj.begin();
	  DCCit itj_end = dcj.end();
	  for( ;itj!=itj_end;++itj ) {
	    // Second seed selected
	    double hitx = (*itj).x();
	    double hity = (*itj).y();
	    double norm = std::sqrt(hitx*hitx + hity*hity);
	    double cphi = hitx/norm;
	    double sphi = hity/norm; 
	    TrkDriftCircleMath::TangentToCircles::LineVec& lines = tanCreator.tangentLines( *iti, *itj );
	    for( TrkDriftCircleMath::TangentToCircles::LineVec::const_iterator lit = lines.begin(); lit!=lines.end(); ++lit ){
	      double coshit = std::cos((*lit).phi());
	      double sinhit = std::sin((*lit).phi());
	      double cospsi = coshit*cphi + sinhit*sphi;
	      if( cospsi > 1. )  cospsi = 1.;
	      else if(cospsi < -1. ) cospsi = -1.;
	      double psi = std::acos(cospsi);
	      if (psi > 0.3) continue;
	      matchWithLine.set( *lit, roadWidth, TrkDriftCircleMath::MatchDCWithLine::Road,tubeRadius );
	      const TrkDriftCircleMath::DCOnTrackVec& hitsOnLine = matchWithLine.match( dcs );
	      unsigned int matchedHits = matchWithLine.hitsOnTrack();
	      ATH_MSG_VERBOSE (" Summary nHits " << matchedHits << " nl1 " << matchWithLine.hitsMl1() 
                               << " nl2 " << matchWithLine.hitsMl2());
	      if (matchedHits > nHits || (matchedHits ==  nHits && psi < angleDif )) {
		int dnl = std::abs(static_cast<int>(matchWithLine.hitsMl1())-
                                   static_cast<int>(matchWithLine.hitsMl2()));
                ATH_MSG_DEBUG (" matchWithLine.hitsOnTrack() >  nHits old " << nHits << " new: " << matchedHits);
                ATH_MSG_DEBUG (" dnl " << dnl << " old dnl " << std::abs(nl1-nl2));
                ATH_MSG_DEBUG (" hit cos phi " << cphi << " line " << coshit << " sin phi " << sphi << " line " << sinhit << " psi " << psi);
          
		// update of variables:
		nHits = matchedHits;
		nl1 = matchWithLine.hitsMl1();
		nl2 = matchWithLine.hitsMl2(); 
		nHitsLine = hitsOnLine.size();
		nPassedTubes = matchWithLine.passedTubes();
		hitsOnLineSel = hitsOnLine;  
		angleDif = psi;
	      }

	      ATH_MSG_VERBOSE (" Select nHits " << nHits << " nl1 " << nl1 << " nl2 " << nl2);
	      if (nHits >= dcs.size()) stop = true;
	    } // end lines
	    if (stop) break;
	  } //end itj
	  if (stop) break; 
	} //end j     
	if (stop) break; 
      } // end iti 
      if (stop) break; 
    } // end i

    ATH_MSG_DEBUG (" Fast segment finder Max Layers hit " << dcs.size() << " nHitsLine - nHits " << nHitsLine-nl1-nl2 << " passed Tubes -nHits " << nPassedTubes-nl1-nl2 << " nl1 " << nl1 << " nl2 " << nl2  << " angleDif " << angleDif);
  
    TrkDriftCircleMath::DCOnTrackIt itt = hitsOnLineSel.begin();
    TrkDriftCircleMath::DCOnTrackIt itt_end = hitsOnLineSel.end();
    int i = 0;
    for( ;itt!=itt_end;++itt,i++ ) {
      int isort = 100*(4*(itt->id().ml()) + itt->id().lay()) + itt->id().tube();
      if (dcsId.count(isort) == 1) {
	int dcsIndex = dcsId[isort];
	sel[dcsIndex] = 1;

	ATH_MSG_DEBUG (" Selected Hit index " << dcsIndex << " MultiLayer " << itt->id().ml() << " layer " <<  itt->id().lay() << " tube " << itt->id().tube());
      } 
      else {
	ATH_MSG_WARNING (" ALARM fastSegmentFinder hit NOT found " << i << " isort " << isort);
      }
    }
  }
}

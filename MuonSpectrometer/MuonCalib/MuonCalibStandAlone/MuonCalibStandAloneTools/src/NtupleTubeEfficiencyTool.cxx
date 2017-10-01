/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 10.04.2007, AUTHOR: STEFFEN KAISER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS NtupleTubeEfficiencyTool ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

// Athena //
#include "MuonIdHelpers/MdtIdHelper.h"

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MdtCalibInterfaces/IMdtSegmentFitter.h"

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

// NtupleTubeEfficiencyTool //
#include "MuonCalibStandAloneTools/NtupleTubeEfficiencyTool.h"


//this
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"


//root
#include "TFile.h"
#include "TH1.h"
#include "TNtuple.h"
#include "TString.h"
#include "TDirectory.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
namespace MuonCalib {

//*****************************************************************************

// constructor

NtupleTubeEfficiencyTool::NtupleTubeEfficiencyTool( const std::string& t, 
                                                    const std::string& n, 
                                                    const IInterface* p) : 
    AthAlgTool(t, n, p), m_reg_sel_svc("RegionSelectionSvc", n) {
    
    declareInterface< NtupleCalibrationTool >(this);	
    declareProperty("RegionSelectionSvc", m_reg_sel_svc);

    m_nb_hits = 5;
    declareProperty("nSegmentHits", m_nb_hits);

    m_road_width = 2.0; 
    declareProperty("roadWidth", m_road_width);

    m_debug = false;
    declareProperty("Debug", m_debug);
    
    m_file_name = string("TubeEfficiencies");
    declareProperty("fileName", m_file_name);

    m_chi2_cut = 10.0;
    declareProperty("Chi2Cut", m_chi2_cut);

    m_exclude_layer = false;
    declareProperty("excludeLayer", m_exclude_layer);

}
 
//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

StatusCode NtupleTubeEfficiencyTool::initialize() {
	
    ATH_MSG_INFO( "Initializing NtupleTubeEfficiencyTool" );

    //-----------------------------//
    //-- Get the StoreGate Stuff --//
    //-----------------------------//

    ATH_CHECK( detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );

    ATH_CHECK( detStore()->retrieve( m_detMgr ) );

    //retrieve fixed id tool   
    std::string idToFixedIdToolType("MuonCalib::IdToFixedIdTool");
    std::string idToFixedIdToolName("MuonCalib_IdToFixedIdTool");
    
    ATH_CHECK( toolSvc()->retrieveTool(idToFixedIdToolType, idToFixedIdToolName, m_id_tool) ); 

    //get region selection service
    ATH_CHECK( m_reg_sel_svc.retrieve() );

    //----------------------------------//
    //-- Create Root Files and Histos --//
    //----------------------------------//

    TDirectory* workdir = gDirectory;
    m_tfile = new TFile(TString(m_file_name)+".root", "RECREATE");

    if(m_debug){
    
        //m_tfile_debug = new TFile(TString(m_file_name).ReplaceAll(".root","_debug.root"), "RECREATE");
        m_tfile_debug = new TFile(TString(m_file_name)+"_debug.root", "RECREATE");
    
        ATH_MSG_INFO( "NtupleTubeEfficiencyTool: Debug mode is switched on" );
        
        m_hit_ntuple = new TNtuple("hit_ntuple", "MDT HIT NTUPLE",
                                   "event_nb:station:eta:phi:ml:layer:tube:w_x:w_y:w_z:x:t:r:d");
    }
	 
        
    m_qfitter = NULL;
    
    m_nb_trigger = 0;

    m_nb_multilayers = -1;
    m_nb_layers      = -1;
    m_nb_tubes       = -1;

    if(m_debug){
        m_tfile_debug->cd();
    }
    else{
        workdir->cd();
    }

    //control histos

    m_h_chi2 = new TH1F("h_chi2", "", 500, 0, 100);  

    m_h_distance     = new TH1F("h_distance", 
			      "distance between track and wire", 
			      150, 0, 15);   
    m_h_nb_hit_tubes = new TH1I("h_nb_hit_tubes", 
			      "nb of hit tubes in exluded layer", 
			      51, 0, 50); 
 

    //efficiency and fakerate histos (per tube)

    m_h_efficiency = vector< vector< vector<TH1F*> > >(2); //up to two multilayers 
    
    for (unsigned int k=0; k<m_h_efficiency.size(); k++) {
    
	m_h_efficiency[k] = vector< vector<TH1F*> >(4); // up to four layers per multilayer
	for (unsigned int l=0; l<m_h_efficiency[k].size(); l++) {
        
	    m_h_efficiency[k][l] = vector<TH1F*>(72); // up to 72 tubes per layer
	    for (unsigned int m=0; m<m_h_efficiency[k][l].size(); m++) {
	
		m_h_efficiency[k][l][m] 
		    = new TH1F( Form("efficiency_%i_%i_%i",k+1,l+1,m+1), 
				"Efficiency", 2, -0.5, 1.5) ;
            }
	}
    } 

   
    m_h_fakerate = vector< vector< vector<TH1F*> > >(2); //up to two multilayers 
    
    for (unsigned int k=0; k<m_h_fakerate.size(); k++) {
    
	m_h_fakerate[k] = vector< vector<TH1F*> >(4); // up to four layers per multilayer
	for (unsigned int l=0; l<m_h_fakerate[k].size(); l++) {
        
	    m_h_fakerate[k][l] = vector<TH1F*>(72); // up to 72 tubes per layer
	    for (unsigned int m=0; m<m_h_fakerate[k][l].size(); m++) {
	
		m_h_fakerate[k][l][m] 
		    = new TH1F( Form("fakerate_%i_%i_%i",k+1,l+1,m+1), 
				"Fakerate", 2, -0.5, 1.5) ;

	    }
	}
    } 
    
    //efficiency and fakerate histos (per layer)

    m_tfile->cd();

    m_h_tube_efficiency  = vector< vector<TH1F*> >(2); //up to two multilayers 
    for (unsigned int k=0; k<m_h_tube_efficiency.size(); k++) {
        
        m_h_tube_efficiency[k] = vector<TH1F*>(4); // up to four layers per multilayer
	for (unsigned int l=0; l<m_h_tube_efficiency[k].size(); l++) {
            
            m_h_tube_efficiency[k][l] 
                = new TH1F( Form("tube_efficiency_%i_%i",k+1,l+1), 
                            "Tube Efficiency", 72, 0.5, 72.5) ;
            
            m_h_tube_efficiency[k][l]->GetXaxis()->SetTitle("Tube");
            m_h_tube_efficiency[k][l]->GetYaxis()->SetTitle("Efficiency");
            m_h_tube_efficiency[k][l]->SetMarkerStyle(8);
        }
    }

    m_h_tube_fakerate  = vector< vector<TH1F*> >(2); //up to two multilayers 
    for (unsigned int k=0; k<m_h_tube_fakerate.size(); k++) {
        
        m_h_tube_fakerate[k] = vector<TH1F*>(4); // up to four layers per multilayer
	for (unsigned int l=0; l<m_h_tube_fakerate[k].size(); l++) {
            
            m_h_tube_fakerate[k][l] 
                = new TH1F( Form("tube_fakerate_%i_%i",k+1,l+1), 
                            "Tube Fakerate", 72, 0.5, 72.5) ;
            
            m_h_tube_fakerate[k][l]->GetXaxis()->SetTitle("Tube");
            m_h_tube_fakerate[k][l]->GetYaxis()->SetTitle("Fakerate");
            m_h_tube_fakerate[k][l]->SetMarkerStyle(8);

        }
    }
    

    //entry histos (per layer)

    m_h_tube_entries_efficiency = vector< vector<TH1F*> >(2); //up to two multilayers 
    for (unsigned int k=0; k<m_h_tube_entries_efficiency.size(); k++) {
        
        m_h_tube_entries_efficiency[k] = vector<TH1F*>(4); // up to four layers per multilayer
	for (unsigned int l=0; l<m_h_tube_entries_efficiency[k].size(); l++) {
            
            m_h_tube_entries_efficiency[k][l] 
                = new TH1F( Form("tube_entries_efficiency_%i_%i",k+1,l+1), 
                            "Tube Entries Efficiency", 72, 0.5, 72.5) ;
            
            m_h_tube_entries_efficiency[k][l]->GetXaxis()->SetTitle("Tube");
            m_h_tube_entries_efficiency[k][l]->GetYaxis()->SetTitle("Entries");
        }
    }

    m_h_tube_entries_fakerate = vector< vector<TH1F*> >(2); //up to two multilayers 
    for (unsigned int k=0; k<m_h_tube_entries_fakerate.size(); k++) {
        
        m_h_tube_entries_fakerate[k] = vector<TH1F*>(4); // up to four layers per multilayer
	for (unsigned int l=0; l<m_h_tube_entries_fakerate[k].size(); l++) {
            
            m_h_tube_entries_fakerate[k][l] 
                = new TH1F( Form("tube_entries_fakerate_%i_%i",k+1,l+1), 
                            "Tube Entries Fakerate", 72, 0.5, 72.5) ;
            
            m_h_tube_entries_fakerate[k][l]->GetXaxis()->SetTitle("Tube");
            m_h_tube_entries_fakerate[k][l]->GetYaxis()->SetTitle("Entries");
        }
    }


    //efficiency and fakerate histos (per chamber)

    m_h_layer_efficiency   = new TH1F("layer_efficiency", "Layer Efficiency", 8, 0.5, 8.5);
    m_h_chamber_efficiency = new TH1F("chamber_efficiency", "Chamber Efficiency", 3, -0.5, 2.5);
    
    m_h_layer_efficiency->GetXaxis()->SetTitle("Layer");
    m_h_layer_efficiency->GetYaxis()->SetTitle("Efficiency");
    m_h_layer_efficiency->SetMarkerStyle(8);
    
    m_h_chamber_efficiency->GetYaxis()->SetTitle("Efficiency"); 
    m_h_chamber_efficiency->SetMarkerStyle(8);

    return StatusCode::SUCCESS;

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::

StatusCode NtupleTubeEfficiencyTool::finalize(void) {

    ATH_MSG_INFO( "Finalizing NtupleTubeEfficiencyTool" );
   
    m_tfile->Write();
    if(m_debug){
        m_tfile_debug->Write();
    }
    
    return StatusCode::SUCCESS;

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD handleEvent ::
//::::::::::::::::::::::::

StatusCode 
NtupleTubeEfficiencyTool::handleEvent( const MuonCalibEvent & event, 
                                       int eventnumber,
                                       const std::vector<MuonCalibSegment *> &segments, 
                                       unsigned int position) {

    if(segments.size()<=position) return StatusCode::SUCCESS;

    MuonCalibSegment segment(*segments[position]);

    //---------------//
    //-- Variables --//
    //---------------//

    m_nb_trigger++;

    if(m_qfitter==NULL){ 
        m_qfitter = new QuasianalyticLineReconstruction();
    }

    
    int nb_hits;
        
    // station identifiers //
  
    MuonFixedId Mid((segment.mdtHOT()[0])->identify());
     
    //this has to be set in order to get the m_detMgr->getMdtReadoutElement() method working correctly
    //otherwise also for the first multilayer, the second is returned
    Mid.setMdtMultilayer(1);

    Identifier station_id = m_id_tool->fixedIdToId(Mid);
  

    if(m_nb_multilayers<0){  
	m_nb_multilayers = m_mdtIdHelper->numberOfMultilayers(station_id);
    }

    
    //----------//
    //-- Hits --//
    //----------//
    
    if(segment.mdtHitsOnTrack()<m_nb_hits){
        //if(segment.mdtHitsOnTrack()!=m_nb_hits){
        return StatusCode::SUCCESS;
    }

    if(m_debug){
       
        for (unsigned int k=0; k<segment.mdtHitsOnTrack(); k++) {
            
            MuonFixedId id((segment.mdtHOT()[k])->identify());
            
            m_hit_ntuple->Fill(eventnumber,
                               id.stationName(), id.eta(), id.phi(),
                               id.mdtMultilayer(), id.mdtTubeLayer(),
                               id.mdtTube(),
                               (segment.mdtHOT()[k])->localPosition().x(),
                               (segment.mdtHOT()[k])->localPosition().y(),
                               (segment.mdtHOT()[k])->localPosition().z(),
                               (segment.mdtHOT()[k])->distanceToReadout(),
                               (segment.mdtHOT()[k])->driftTime(),
                               (segment.mdtHOT()[k])->driftRadius(),
                               (segment.mdtHOT()[k])->signedDistanceToTrack());
            
        }
    }
    
    //----------------//
    //-- Efficiency --//
    //----------------//

    m_qfitter->setRoadWidth(m_road_width); //0.65
    m_qfitter->switchOnRefit();
    //m_qfitter->fit(segment);

    //loop over multilayers
    for (int multilayer=1; multilayer<m_nb_multilayers+1; multilayer++) {

	const MuonGM::MdtReadoutElement* MdtRoEl = 
	    m_detMgr->getMdtReadoutElement( m_mdtIdHelper->channelID(station_id,multilayer,1,1) );
    
	if(m_nb_layers<0)  m_nb_layers = MdtRoEl->getNLayers();
	if(m_nb_tubes<0)   m_nb_tubes  = MdtRoEl->getNtubesperlayer(); 
    
	//loop over layers
	for (int layer=1; layer<m_nb_layers+1; layer++) {

            nb_hits = 0;
            
            // hit selection vector for refits 
            IMdtSegmentFitter::HitSelection 
                hit_selection = IMdtSegmentFitter::HitSelection(segment.mdtHitsOnTrack());

        
	    //exclude layer: layer in multilayer: multilayer //
            
            for (unsigned int l=0; l<segment.mdtHitsOnTrack(); l++) {
		
                MuonFixedId id((segment.mdtHOT()[l])->identify());
		
                if (id.mdtMultilayer() == multilayer &&
                    id.mdtTubeLayer()  == layer) {
		    
                    hit_selection[l] = 1;
                
                } else {
		
                    hit_selection[l] = 0;
		    nb_hits = nb_hits+1;
		}
	    }

            //cout << "event: " << eventnumber << ", hits: " << nb_hits << ", segment: " << segment.mdtHitsOnTrack() << endl;
            
	    //perform the track fit //
	    //if (nb_hits<m_nb_hits) {
            //   continue;
            //}
            
            

            if(m_exclude_layer){
                
                m_h_chi2->Fill(m_qfitter->chi2PerDegreesOfFreedom());

                if (!m_qfitter->fit(segment, hit_selection)) {
                    continue;
                }
                if (m_qfitter->numberOfTrackHits()<3) {
                    continue;
                }
                //if (m_qfitter->numberOfTrackHits()<(m_nb_hits-1) {
                //  continue;
                //}
                if(m_qfitter->chi2PerDegreesOfFreedom()>m_chi2_cut){ 
                    continue;
                }
		
            }else{
                
                m_h_chi2->Fill(segment.chi2());

                if(segment.chi2()>m_chi2_cut){ 
                    continue;
                }
            }
	    
            // find tube which has been traversed by the track //
    
	    MTStraightLine track;
            
            if(m_exclude_layer){
                track = m_qfitter->track();
            }
            else{
                track = MTStraightLine(segment.position(),segment.direction(),
                                       Amg::Vector3D(0,0,0), Amg::Vector3D(0,0,0)
                                       );
            }

	    std::vector<int> traversed_tube(0);
    
	    for (int k=0; k<m_nb_tubes; k++) {

		Amg::Vector3D TubePos = 
		    MdtRoEl->GlobalToAmdbLRSCoords(MdtRoEl->tubePos(multilayer,layer,k+1));
	
		Amg::Vector3D tube_position  = Amg::Vector3D(TubePos.x(), TubePos.y(), TubePos.z());
		Amg::Vector3D tube_direction = Amg::Vector3D(1,0,0);
	
		MTStraightLine tube = MTStraightLine( tube_position, tube_direction,
						      Amg::Vector3D(0,0,0), Amg::Vector3D(0,0,0)
						      );
                
                //debug: check geometry
                //cout << "AMDpos: " << tube_position 
                //   << ", mypos: " << wire_position[multilayer-1][layer-1][k] << endl;
                
		double distance = TMath::Abs(track.signDistFrom(tube));
	
                if ( distance < (MdtRoEl->innerTubeRadius()) ){
		    traversed_tube.push_back(k+1);
		    m_h_distance->Fill(distance);
		}
	    }

	    m_h_nb_hit_tubes->Fill(traversed_tube.size());
    

	    // check whether the traversed tubes give a hit //
    
	    //collect raw hits in the excluded layer //
	    const MuonCalibRawHitCollection *raw_hits(event.rawHitCollection());
    
	    std::vector<int> hit_tube(0);

	    for (MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it=
		     raw_hits->rawMdtHitCollectionBegin();
		 it!=raw_hits->rawMdtHitCollectionEnd(); ++it) {
	
		MuonCalibRawMdtHit *hit = *it;
                
		if (m_reg_sel_svc->isInRegion(hit->identify()) &&
		    (hit->identify()).mdtMultilayer() == multilayer &&
		    (hit->identify()).mdtTubeLayer()  == layer) {
                    
		    hit_tube.push_back( (hit->identify()).mdtTube() );
                    
		}
	    }

	    // Efficiencies //

	    //loop over traversed tubes
	    for (unsigned int k=0; k<traversed_tube.size(); k++) {
	
		int hit_flag = 0;
	
		//loop over hit tubes
		for (unsigned int j=0; j<hit_tube.size(); j++) {
	    
		    if(traversed_tube[k]==hit_tube[j]){
			hit_flag = 1;
                        break;
                    }
		}
    
		m_h_efficiency[multilayer-1][layer-1][ traversed_tube[k]-1 ]->Fill(hit_flag);
	    }

            
            // Fakerate //

	    //loop over hit tubes
	    for (unsigned int k=0; k<hit_tube.size(); k++) {
		
		int fake_flag = 0;
		
		//loop over traversed tubes
		for (unsigned int j=0; j<traversed_tube.size(); j++) {
		    
		    if(hit_tube[k]!=traversed_tube[j]){
			fake_flag = 1;
		    }
		}
		
		m_h_fakerate[multilayer-1][layer-1][ hit_tube[k]-1 ]->Fill(fake_flag);
	    }

	
	} //end of loop over layers
   
    } //end of loop over multilayers
    
    return StatusCode::SUCCESS;

}

//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD analyseSegments ::
//::::::::::::::::::::::::::::

StatusCode 
NtupleTubeEfficiencyTool::analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/) {

    std::string outfilename = m_file_name +  ".txt";
    
    ofstream outfile(outfilename.c_str());
    outfile << "ml \t ly \t tube \t efficiency \t error \t fakerate \t ferror" << endl;

    //initialize summary variables
   
    int    chamber_ntubes;
    double chamber_efficiency;
    double chamber_error;
    double chamber_entries;

    std::vector<int>    multilayer_ntubes(m_nb_multilayers);
    std::vector<double> multilayer_efficiency(m_nb_multilayers);
    std::vector<double> multilayer_error(m_nb_multilayers);
    std::vector<double> multilayer_entries(m_nb_multilayers);

    std::vector<std::vector<int> >     layer_ntubes(m_nb_multilayers, std::vector<int>(m_nb_layers));
    std::vector<std::vector<double> >  layer_efficiency(m_nb_multilayers, std::vector<double>(m_nb_layers));
    std::vector<std::vector<double> >  layer_error(m_nb_multilayers, std::vector<double>(m_nb_layers));
    std::vector<std::vector<double> >  layer_entries(m_nb_multilayers, std::vector<double>(m_nb_layers));

    chamber_ntubes     = 0;
    chamber_efficiency = 0.;
    chamber_error      = 0.;
    chamber_entries    = 0;
  
    for (int k=0; k<m_nb_multilayers; k++) {
        
        multilayer_ntubes[k]     = 0;
        multilayer_efficiency[k] = 0.;
        multilayer_error[k]      = 0.;
        multilayer_entries[k]    = 0;

        for (int l=0; l<m_nb_layers; l++) {
            layer_ntubes[k][l]     = 0;
            layer_efficiency[k][l] = 0.;
            layer_error[k][l]      = 0.;
            layer_entries[k][l]    = 0;
        }
    }

    //calculate and fill tube efficiencies

    //loop over multilayers
    for (int k=0; k<m_nb_multilayers; k++) {
	
	//loop over layers
	for (int l=0; l<m_nb_layers; l++) {
	    
            //loop over tubes
	    for (int m=0; m<m_nb_tubes; m++) {
		
                //calculate efficiency, fakerate and errors

		double efficiency = m_h_efficiency[k][l][m]->GetMean();
		double entries    = m_h_efficiency[k][l][m]->GetEntries();
		double error      = 0.; 
	
		if(entries!=0){
		    error  = ( 1/sqrt(entries) ) * sqrt( efficiency*(1-efficiency) );
		}
		
                double fakerate = m_h_fakerate[k][l][m]->GetMean() / (double)m_nb_trigger;
		double fentries = m_h_fakerate[k][l][m]->GetEntries();
		double ferror   = 0; 
		
                if(fentries!=0){
		    ferror  = ( 1/sqrt(fentries) ) * sqrt( fakerate*(1-fakerate) );
		}
           
     
                //fill histos
                
                TString title = "Tube Efficiency ";
                title += TString(m_reg_sel_svc->GetRegionSelection());
                title += Form(" (ml%i, ly%i)",k+1,l+1); 
                
                m_h_tube_efficiency[k][l]->SetTitle(title);
                m_h_tube_fakerate[k][l]->SetTitle(title.ReplaceAll("Efficiency","Fakerate"));

                m_h_tube_efficiency[k][l]->SetBinContent(m+1,efficiency);
                m_h_tube_efficiency[k][l]->SetBinError(m+1,error);

                m_h_tube_fakerate[k][l]->SetBinContent(m+1,fakerate);
                m_h_tube_fakerate[k][l]->SetBinError(m+1,ferror);

                m_h_tube_efficiency[k][l]->GetXaxis()->SetRange(1,m_nb_tubes);
                m_h_tube_fakerate[k][l]->GetXaxis()->SetRange(1,m_nb_tubes);


                //write text file

                outfile << k+1 << "\t" << l+1 << "\t" << m+1 << "\t" 
			<< Form("%1.3f",efficiency) << "\t"
			<< Form("%1.3f",error) << "\t"
			<< Form("%1.8f",fakerate) << "\t"
			<< Form("%1.3f",ferror) << endl;
             

                //entry histos
                
                TString entry_title = "Tube Entries (Efficiency) ";
                entry_title += TString(m_reg_sel_svc->GetRegionSelection());
                entry_title += Form(" (ml%i, ly%i)",k+1,l+1); 
 
                m_h_tube_entries_efficiency[k][l]->SetTitle(entry_title);
                m_h_tube_entries_fakerate[k][l]->SetTitle(entry_title.ReplaceAll("Efficiency","Fakerate"));

                m_h_tube_entries_efficiency[k][l]->SetBinContent(m+1,entries);
                m_h_tube_entries_fakerate[k][l]->SetBinContent(m+1,fentries);
                
                m_h_tube_entries_efficiency[k][l]->GetXaxis()->SetRange(1,m_nb_tubes);
                m_h_tube_entries_fakerate[k][l]->GetXaxis()->SetRange(1,m_nb_tubes);
                

                //summarized efficiencies
                
                if(error>0){

                    layer_ntubes[k][l]++; 
                    layer_efficiency[k][l] += 1./pow(error,2)*efficiency;
                    layer_error[k][l]      += 1./pow(error,2);
                    layer_entries[k][l]    += entries;
                    
                    multilayer_ntubes[k]++; 
                    multilayer_efficiency[k] += 1./pow(error,2)*efficiency;
                    multilayer_error[k]      += 1./pow(error,2);
                    multilayer_entries[k]    += entries;
                    
                    chamber_ntubes++; 
                    chamber_efficiency += 1./pow(error,2)*efficiency;
                    chamber_error      += 1./pow(error,2);
                    chamber_entries    += entries;
                    
                }
            }
	}
    }

    
    //fill summary histos
    
    TString layer_title = "Layer Efficiency ";
    layer_title += TString(m_reg_sel_svc->GetRegionSelection());
        
    m_h_layer_efficiency->SetTitle(layer_title);
    m_h_layer_efficiency->GetXaxis()->SetRange(1,m_nb_multilayers*m_nb_layers);

    TString chamber_title = "Efficiency ";
    chamber_title += TString(m_reg_sel_svc->GetRegionSelection());
   
    m_h_chamber_efficiency->SetTitle(chamber_title);
    m_h_chamber_efficiency->GetXaxis()->SetRange(1,m_nb_multilayers+1);

    int layer_bin = 1;
    int chamber_bin = 2;

    for (int k=0; k<m_nb_multilayers; k++) {
        
        for (int l=0; l<m_nb_layers; l++) {
            
            double layer_eff = 0.;
            double layer_err = 0.;
            
            if(layer_error[k][l]>0){
                layer_eff = layer_efficiency[k][l]/layer_error[k][l];
                layer_err = sqrt(1./layer_error[k][l]);
            }
            
            m_h_layer_efficiency->SetBinContent(layer_bin, layer_eff);
            m_h_layer_efficiency->SetBinError(layer_bin, layer_err);
            
            layer_bin++;
        }
        
        double ml_eff = 0.;
        double ml_err = 0.;

        if(multilayer_error[k]>0){
            ml_eff = multilayer_efficiency[k]/multilayer_error[k];
            ml_err = sqrt(1./multilayer_error[k]);
        }

        m_h_chamber_efficiency->SetBinContent(chamber_bin, ml_eff);
        m_h_chamber_efficiency->SetBinError(chamber_bin, ml_err);
        m_h_chamber_efficiency->GetXaxis()->SetBinLabel(chamber_bin, TString(Form("ml%i",k+1)).Data());

        chamber_bin++;
    }

    double ch_eff = 0.;
    double ch_err = 0.;

    if(chamber_error>0){
        ch_eff = chamber_efficiency/chamber_error;
        ch_err = sqrt(1./chamber_error);
    }
    
    m_h_chamber_efficiency->SetBinContent(1, ch_eff);
    m_h_chamber_efficiency->SetBinError(1, ch_err);
    m_h_chamber_efficiency->GetXaxis()->SetBinLabel(1,"chamber");
    m_h_chamber_efficiency->LabelsOption("d");


    //delete not used histos

    for (int k=0; k<2; k++) {      //multilayers
        
        for (int l=0; l<4; l++) {  //layers
            
            if(k>m_nb_multilayers-1 || l>m_nb_layers-1){
                delete m_h_tube_efficiency[k][l];
                delete m_h_tube_fakerate[k][l];
                delete m_h_tube_entries_efficiency[k][l];
                delete m_h_tube_entries_fakerate[k][l];
            }
        }
    }


    return StatusCode::SUCCESS;

}

}

    
//HepGeom::Point3D<double> tubePos = ml2->tubePos(1,2,1);
//HepGeom::Point3D<double> tubePos = ml2->localTubePos(2,1,1);
//cout << "tubePos: " << tubePos.y() << endl;
   
//HepGeom::Point3D<double> tubePosLoc = ml2->GlobalToAmdbLRSCoords(tubePos);
//HepGeom::Point3D<double> tubePosLoc = ml2->AmdbLRSToGlobalCoords(tubePos);
//HepGeom::Point3D<double> tubePosLoc = ml2->tubeFrame_localROPos(2,1,1);

//     cout << "k \t wirepos(x) \t wirepos(y) \t wirepos(z) \t tubepos(x) \t tubepos(y) \t tubepos(z)" << endl;
//     for (int k=0; k<n_tubesly; k++) {
	
// 	//HepGeom::Point3D<double> LTubePos = ml2->localTubePos(2,1,k+1);
// 	HepGeom::Point3D<double> LTubePos = ml2->GlobalToAmdbLRSCoords(ml2->tubePos(1,2,k+1));

// 	cout << k << "\t" 
// 	     << wire[k].positionVector().x() <<  "\t"
// 	     << wire[k].positionVector().y() <<  "\t" 
// 	     << wire[k].positionVector().z() <<  "\t || \t" 
// 	     << LTubePos.x() << "\t"
// 	     << LTubePos.y() << "\t"
// 	     << LTubePos.z() << endl;
//     }


   //hardcoded wire positions for BML_3_1
    //  Amg::Vector3D wire_position[2][3][72];
    //      for (int k=0; k<72; k++) {
    //          wire_position[0][0][k]  = Amg::Vector3D(0, 30.035*(k+1)-15.018,    169.516998); 
    //          wire_position[0][1][k]  = Amg::Vector3D(0, 30.035*(k+1)+0.0000023,  195.528992); 
    //          wire_position[0][2][k]  = Amg::Vector3D(0, 30.035*(k+1)-15.018001, 221.540009); 
    //          wire_position[1][0][k]  = Amg::Vector3D(0, 30.035*(k+1)-15.018000, 568.576965); 
    //          wire_position[1][1][k]  = Amg::Vector3D(0, 30.035*(k+1)+0.000004,  594.588989);
    //          wire_position[1][2][k]  = Amg::Vector3D(0, 30.035*(k+1)-15.018000, 620.599976);
    //      }

  //reset histo names

   //  if( !(TString(m_h_tube_efficiency[0][0]->GetName()))
//         .Contains(TString(m_cal_region->regionId()))){
               
//         for (unsigned int k=0; k<m_h_tube_efficiency.size(); k++) {
                      
//             for (unsigned int l=0; l<m_h_tube_efficiency[k].size(); l++) {
                
//                 m_h_tube_efficiency[k][l]->SetName( TString(m_cal_region->regionId()) + "_" +
//                                                    TString(m_h_tube_efficiency[k][l]->GetName()) );
                
                
//             }
//         }
        
//         for (unsigned int k=0; k<m_h_tube_fakerate.size(); k++) {
            
//             for (unsigned int l=0; l<m_h_tube_fakerate[k].size(); l++) {
                
//                 m_h_tube_fakerate[k][l]->SetName( TString(m_cal_region->regionId()) + "_" +
//                                                  TString(m_h_tube_fakerate[k][l]->GetName()) );
                
//             }
//         }
//     }

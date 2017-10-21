/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 19.10.2007, AUTHOR: STEFFEN KAISER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS NtupleDisplay ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>

// Athena //
#include "MuonIdHelpers/MdtIdHelper.h"

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
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
#include "MdtCalibFitters/StraightPatRec.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

// NtupleDisplayTool //
#include "MuonCalibStandAloneTools/NtupleDisplayTool.h"


//this
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"


//root
#include "TCanvas.h"
#include "TControlBar.h"
#include "TApplication.h"
#include "TLine.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TMath.h"
#include "TLatex.h"

// STL //
#include <string>

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
namespace MuonCalib {

//*****************************************************************************

// constructor

NtupleDisplayTool::NtupleDisplayTool( const std::string& t, 
                                                    const std::string& n, 
                                                    const IInterface* p) : 
    AthAlgTool(t, n, p),
    m_reg_sel_svc("RegionSelectionSvc", n){
    
    declareInterface< NtupleCalibrationTool >(this);	
    declareProperty("RegionSelectionSvc", m_reg_sel_svc);

    //-----------------//
    //-- Job Options --//
    //-----------------//

    m_fitter_name = string("QuasianalyticLineReconstruction");
    declareProperty("fitterName", m_fitter_name);

    m_nb_hits = 5;
    declareProperty("nSegmentHits", m_nb_hits);

    m_road_width = 2.0; 
    declareProperty("roadWidth", m_road_width);

    m_adc_cut = 50.;
    declareProperty("adcCut", m_adc_cut);

    m_chosen_event = -1;
    declareProperty("displayEvent", m_chosen_event);

    m_scalefactor = 1;
    declareProperty("scaleDisplay", m_scalefactor);

}
 
//:::::::::::::::::::::::
//:: METHOD initialize ::
//:::::::::::::::::::::::

StatusCode NtupleDisplayTool::initialize() {
	
    ATH_MSG_INFO( "Initializing NtupleDisplayTool" );

    
    //----------------------//
    //-- Set the Graphics --//
    //----------------------//
    
    m_root = new TApplication("m_root", 0, 0, NULL, 0);
    m_canvas = NULL;
    m_canvas_leg = NULL;
    
    m_bar = new TControlBar("vertical","Display Control");
    m_bar->AddButton("    Next Event    ","gApplication->Terminate()", "Go to the next event");
    m_bar->AddButton("Exit",".qqqqqqq", "Exit athena");


    //-----------------------------//
    //-- Get the StoreGate Stuff --//
    //-----------------------------//

    ATH_CHECK( detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );

    ATH_CHECK( detStore()->retrieve( m_detMgr ) );

    //retrieve fixed id tool   
    std::string idToFixedIdToolType("MuonCalib::IdToFixedIdTool");
    std::string idToFixedIdToolName("MuonCalib_IdToFixedIdTool");
    
    ATH_CHECK( toolSvc()->retrieveTool(idToFixedIdToolType, idToFixedIdToolName, m_id_tool) ); 

    //---------------//
    //-- Variables --//
    //---------------//
        
    m_qfitter = NULL;
    
    m_nb_multilayers = -1;
    m_nb_layers      = -1;
    m_nb_tubes       = -1;
    
    //vector of colors to be used
    m_colors = vector<int>(5);
    m_colors[0] = 3;  //segment hit
    m_colors[1] = 14; //raw hit
    m_colors[2] = 1;  //drift radius>14.6
    m_colors[3] = 2;  //adc<m_adc_cut
    m_colors[4] = 0;  //drift time<0

    // tube circles
    m_tube_circle = vector< vector< vector<TubeCircle*> > >(2); //up to two multilayers 
        
    for (unsigned int k=0; k<m_tube_circle.size(); k++) {
    
	m_tube_circle[k] = vector< vector<TubeCircle*> >(4); // up to four layers per multilayer
	for (unsigned int l=0; l<m_tube_circle[k].size(); l++) {
        
	    m_tube_circle[k][l] = vector<TubeCircle*>(72); // up to 72 tubes per layer
	    for (unsigned int m=0; m<m_tube_circle[k][l].size(); m++) {
	
		m_tube_circle[k][l][m] = new TubeCircle(m_colors,m_adc_cut);
                
	    }
	}
    } 

//get region selection service
	ATH_CHECK( m_reg_sel_svc.retrieve() );
   
    return StatusCode::SUCCESS;

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::

StatusCode NtupleDisplayTool::finalize(void) {

    ATH_MSG_INFO( "Finalizing NtupleDisplayTool" );
    
    return StatusCode::SUCCESS;

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD handleEvent ::
//::::::::::::::::::::::::

StatusCode 
NtupleDisplayTool::handleEvent( const MuonCalibEvent & event, 
                                       int eventnumber,
                                       const std::vector<MuonCalibSegment *> &segments, 
                                       unsigned int position) {

    if(segments.size()<=position) return StatusCode::SUCCESS;

    MuonCalibSegment segment(*segments[position]);

    //if displaying only one event 
    if(m_chosen_event!=-1){
        if(eventnumber!=m_chosen_event){
            return StatusCode::SUCCESS;
        }
    }


    //---------------//
    //-- VARIABLES --//
    //---------------//
    
    if(m_qfitter==NULL){
       if (m_fitter_name==string("QuasianalyticLineReconstruction")) {
        m_qfitter = new QuasianalyticLineReconstruction();
       }
       if (m_fitter_name==string("StraightPatRec")) {
        m_qfitter = new StraightPatRec();
       }
       if (m_fitter_name!=string("QuasianalyticLineReconstruction") &&
           m_fitter_name!=string("StraightPatRec")) {
           ATH_MSG_FATAL( "Unknown track fitter!" );
           return StatusCode::FAILURE;
       }
    }

            
    // station identifiers //
  
    MuonFixedId Mid((segment.mdtHOT()[0])->identify());
     
    //this has to be set in order to get the m_detMgr->getMdtReadoutElement() method 
    //working correctly, otherwise also for the first multilayer, the second is returned 
    Mid.setMdtMultilayer(1);
    
    Identifier station_id = m_id_tool->fixedIdToId(Mid);
    NtupleStationId st_id((segment.mdtHOT()[0])->identify());
    st_id.SetMultilayer(0);
    if (!st_id.InitializeGeometry(m_mdtIdHelper, m_detMgr))
    	return StatusCode::SUCCESS;

    if(m_nb_multilayers<0){  
 	m_nb_multilayers = m_mdtIdHelper->numberOfMultilayers(station_id);
    }


    //----------------------------//
    //-- Determine the Geometry --//
    //----------------------------//

    double y_min(1e6);
    double z_min(1e6);
    double y_max(-1e6);
    double z_max(-1e6);
    
    if(m_tube_circle_lin.size()<1){
     
        //loop over multilayers
        for (int multilayer=1; multilayer<m_nb_multilayers+1; multilayer++) {
            
            const MuonGM::MdtReadoutElement* MdtRoEl = 
                m_detMgr->getMdtReadoutElement( m_mdtIdHelper->channelID(station_id,multilayer,1,1) );
             
            //loop over layers
            for (int layer=st_id.LayerMin(multilayer-1); layer<=st_id.LayerMax(multilayer-1); layer++) {
                 
                for (int k=st_id.TubeMin(multilayer-1); k<=st_id.TubeMax(multilayer-1); k++) {
                     
                    Amg::Vector3D TubePos = 
                        MdtRoEl->GlobalToAmdbLRSCoords(MdtRoEl->tubePos(multilayer,layer,k));
                                          
                    //ATH_MSG_INFO( "ml: " << multilayer << ", " << TubePos );
                    
                    //set the tube position
//              ATH_MSG_INFO( multilayer   <<" "<< layer<<" "<<k );
			m_tube_circle[multilayer-1][layer-1][k-1] ->setPosition(TubePos);
			m_tube_circle[multilayer-1][layer-1][k-1]->multilayer= multilayer;
			m_tube_circle[multilayer-1][layer-1][k-1]->layer= layer;
			m_tube_circle[multilayer-1][layer-1][k-1]->tube= k;
			
                    	m_tube_circle_lin.push_back( m_tube_circle[multilayer-1][layer-1][k-1]);
                    
                    //determine the size of the chamber
                    if(TubePos.y()<y_min){
                        y_min = TubePos.y();
                    }
                    if(TubePos.z()<z_min){
                        z_min = TubePos.z();
                    }
                    if(TubePos.y()>y_max){
                        y_max = TubePos.y();
                    }
                    if(TubePos.z()>z_max){
                        z_max = TubePos.z();
                    }
               
                } //for: tube
                 
            } //for: layers
             
        } //for: multilayers
      
    } //if: size<1
       
    //set the display canvas
    if(m_canvas==NULL){
        
        double scale(1);
        if(m_nb_tubes<60){
            scale = 0.85;
        }
        else{
            scale = 0.56;
        }
        scale *= m_scalefactor;
        
        int canvas_size_x =  TMath::Nint((y_max - y_min)*scale); //1200x500 72er 0.56, 0.71 //48er 0.85, 1.08
        int canvas_size_y =  TMath::Nint((z_max - z_min)*scale*1.2); 
        m_canvas = new TCanvas("mdt_display", 
                               "MDT Chamber Display" ,
                               canvas_size_x,canvas_size_y);
        m_canvas->Range(y_min-20, z_min-20.0, y_max+20.0, z_max+20.0);
        m_canvas->SetFillColor(0);
        m_canvas->Clear();
        m_canvas->Update();
    
        //draw the control bar
        m_bar->Show();
    }
    

    //------------------//
    //-- Segment Hits --//
    //------------------//

    //check if segment has at least m_nb_hits hits (set in the job options)
    if(segment.mdtHitsOnTrack()<m_nb_hits){
        return StatusCode::SUCCESS;
    }
   
    for (unsigned int k=0; k<segment.mdtHitsOnTrack(); k++) {
            
        MuonFixedId id((segment.mdtHOT()[k])->identify());
            
        int ml(id.mdtMultilayer());
        int ly(id.mdtTubeLayer());
        int tu(id.mdtTube());
        double radius((segment.mdtHOT()[k])->driftRadius());
        double time((segment.mdtHOT()[k])->driftTime());
        double adc((segment.mdtHOT()[k])->adcCount());

        m_tube_circle[ml-1][ly-1][tu-1]->setSegmentHit(radius, time, adc);

    }


    //--------------//
    //-- Raw Hits --//
    //--------------//
    
    const MuonCalibRawHitCollection *raw_hits(event.rawHitCollection());
    
    for (MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it=
             raw_hits->rawMdtHitCollectionBegin();
         it!=raw_hits->rawMdtHitCollectionEnd(); ++it) {
	
        MuonCalibRawMdtHit *hit = *it;
        MuonFixedId id(hit->identify());
        
        if (m_reg_sel_svc->isInRegion(hit->identify())){

            int ml(id.mdtMultilayer());
            int ly(id.mdtTubeLayer());
            int tu(id.mdtTube());
            double radius(hit->driftRadius());
            double time(hit->driftTime());
            double adc(hit->adcCount());

            m_tube_circle[ml-1][ly-1][tu-1]->setRawHit(radius, time, adc);
            
        }
    }


    //-----------//
    //-- Track --//
    //-----------//

    //get track from segment
    MTStraightLine track = MTStraightLine(segment.position(),segment.direction(),
                                          Amg::Vector3D(0,0,0), Amg::Vector3D(0,0,0)
                                          );
    
    double y1, z1, y2, z2;
    z1 = z_min-15;
    z2 = z_max+15;
    y1 = track.a_x2()*z1+track.b_x2();
    y2 = track.a_x2()*z2+track.b_x2();
    
    TLine* line = new TLine(y1, z1, y2, z2);
    line->SetLineWidth(2);
    line->SetLineColor(m_colors.at(0));
    
    //refit segment 
    bool fit_success(false);
    if(m_qfitter)
    	{
	    m_qfitter->setRoadWidth(m_road_width); //0.65
	    m_qfitter->switchOnRefit();
    
    
	    fit_success = m_qfitter->fit(segment);
        }
    MTStraightLine track_refit(segment.position(),segment.direction(),
                                          Amg::Vector3D(0,0,0), Amg::Vector3D(0,0,0));

    y1 = track_refit.a_x2()*z1+track_refit.b_x2();
    y2 = track_refit.a_x2()*z2+track_refit.b_x2();
       
    TLine* line_refit = new TLine(y1, z1, y2, z2);
    line_refit->SetLineWidth(2);
    line_refit->SetLineColor(4);


    //------------------//
    //-- Build Legend --//
    //------------------//

    if(m_canvas_leg==NULL){

        
        int c_x_max = 220;
        int c_y_max = 260;
        
        double x_max = 1.6*c_x_max;
        double y_max = 1.6*c_y_max;

        m_canvas_leg = new TCanvas("mdt_display_legend", "Display Legend",c_x_max,c_y_max);
        m_canvas_leg->Range(0, 0, x_max, y_max);
        m_canvas_leg->SetFillColor(0);
        m_canvas_leg->Clear();
        m_canvas_leg->Update();
        //TLegend* leg = new TLegend(0,0,1,1);
        
        int offset     = 40;
        int leftspace  = 40;
        int tleftspace = 70;
        double tsize   = 0.065;
         
        TubeCircle* circle1 = new TubeCircle(m_colors,m_adc_cut);
        circle1->setPosition(Amg::Vector3D(0,leftspace,y_max-offset));
        circle1->setSegmentHit(11, 1, 2*m_adc_cut);
        circle1->Draw();
        
        TubeCircle* circle2 = new TubeCircle(m_colors,m_adc_cut);
        circle2->setPosition(Amg::Vector3D(0,leftspace,y_max-2*offset));
        circle2->setRawHit(11, 1, 2*m_adc_cut);
        circle2->Draw();
         
        TubeCircle* circle3 = new TubeCircle(m_colors,m_adc_cut);
        circle3->setPosition(Amg::Vector3D(0,leftspace,y_max-4*offset));
        circle3->setSegmentHit(20, 1, 2*m_adc_cut);
        circle3->Draw();

        TubeCircle* circle4 = new TubeCircle(m_colors,m_adc_cut);
        circle4->setPosition(Amg::Vector3D(0,leftspace,y_max-5*offset));
        circle4->setSegmentHit(14.6, -1, 2*m_adc_cut);
        circle4->Draw();

        TubeCircle* circle5 = new TubeCircle(m_colors,m_adc_cut);
        circle5->setPosition(Amg::Vector3D(0,leftspace,y_max-6*offset));
        circle5->setSegmentHit(14.6, 1, 0.5*m_adc_cut);
        circle5->Draw();
        m_canvas_leg->Update();
       
        TLine* line_leg = new TLine(leftspace-15,y_max-8*offset,leftspace+15,y_max-8*offset);
        line_leg->SetLineWidth(2);
        line_leg->SetLineColor(m_colors.at(0)); 
        line_leg->Draw();
           
        TLine* line_refit_leg = new TLine(leftspace-15,y_max-9*offset,leftspace+15,y_max-9*offset);
        line_refit_leg->SetLineWidth(2);
        line_refit_leg->SetLineColor(line_refit->GetLineColor()); 
        line_refit_leg->Draw();

        TLine* line1_leg = new TLine(0.5*leftspace,y_max-3*offset,x_max-0.5*leftspace,y_max-3*offset);
        line1_leg->SetLineWidth(1);
        line1_leg->SetLineColor(16); 
        line1_leg->SetLineStyle(2); 
        line1_leg->Draw();
 
        TLine* line2_leg = new TLine(0.5*leftspace,y_max-7*offset,x_max-0.5*leftspace,y_max-7*offset);
        line2_leg->SetLineWidth(1);
        line2_leg->SetLineColor(16); 
        line2_leg->SetLineStyle(2); 
        line2_leg->Draw();
         
         
        TPaveText* text1 = new TPaveText(tleftspace, y_max-offset-15, x_max, y_max-offset+15);
        text1->SetFillColor(0); // text is black on white
        text1->SetTextSize(tsize);
        text1->SetTextAlign(12);
        text1->SetBorderSize(0);
        text1->AddText("Segment Hit");
        text1->Draw();

        TPaveText* text2 = new TPaveText(tleftspace, y_max-2*offset-15, x_max, y_max-2*offset+15);
        text2->SetFillColor(0); 
        text2->SetTextSize(tsize); 
        text2->SetTextAlign(12);
        text2->SetBorderSize(0);
        text2->AddText("Raw Hit");
        text2->Draw();

        TPaveText* text3 = new TPaveText(tleftspace, y_max-4*offset-15, x_max, y_max-4*offset+15);
        text3->SetFillColor(0); 
        text3->SetTextSize(tsize); 
        text3->SetTextAlign(12);
        text3->SetBorderSize(0);
        text3->AddText("Drift Radius > 14.6 mm");
        text3->Draw();

        TPaveText* text4 = new TPaveText(tleftspace, y_max-5*offset-15, x_max, y_max-5*offset+15);
        text4->SetFillColor(0); 
        text4->SetTextSize(tsize); 
        text4->SetTextAlign(12);
        text4->SetBorderSize(0);
        text4->AddText("Drift Time < 0");
        text4->Draw();

        TPaveText* text5 = new TPaveText(tleftspace, y_max-6*offset-15, x_max, y_max-6*offset+15);
        text5->SetFillColor(0); 
        text5->SetTextSize(tsize); 
        text5->SetTextAlign(12);
        text5->SetBorderSize(0);
        text5->AddText(Form("ADC < %.1f",m_adc_cut));
        text5->Draw();
         
        TPaveText* text6 = new TPaveText(tleftspace, y_max-8*offset-15, x_max, y_max-8*offset+15);
        text6->SetFillColor(0); 
        text6->SetTextSize(tsize); 
        text6->SetTextAlign(12);
        text6->SetBorderSize(0);
        text6->AddText("Track (Segment)");
        text6->Draw();

        TPaveText* text7 = new TPaveText(tleftspace, y_max-9*offset-15, x_max, y_max-9*offset+15);
        text7->SetFillColor(0); 
        text7->SetTextSize(tsize); 
        text7->SetTextAlign(12);
        text7->SetBorderSize(0);
        text7->AddText("Refitted Track");
        text7->Draw();

    }

    m_canvas_leg->Update();
    m_canvas->cd();


    //---------------------//
    //-- Draw Everything --//
    //---------------------//

    ATH_MSG_INFO( "Event: " << eventnumber );

    for (unsigned int k=0; k<m_tube_circle_lin.size(); k++) {
        m_tube_circle_lin[k]->Draw();
	if(m_tube_circle_lin[k]->tube==1)
		{
		std::ostringstream ostr;
		ostr << m_tube_circle_lin[k]->multilayer << "/" << m_tube_circle_lin[k]->layer << "/" <<m_tube_circle_lin[k]->tube;
		TLatex *txt=new TLatex( m_tube_circle_lin[k]->GetX1() - 2* m_tube_circle_lin[k]->GetR1(), m_tube_circle_lin[k]->GetY1(), ostr.str().c_str());
//	txt->SetTextSize(1);
		txt->Draw();
		ATH_MSG_INFO( ostr.str() );
		}

    }
    line->Draw();

    if(fit_success){
        //if(fit_success && m_qfitter->numberOfTrackHits()>=m_nb_hits){ 
        line_refit->Draw();
    }
    else{
        ATH_MSG_WARNING( "refit not successfull!" );
    }

    m_canvas->Update();
    m_root->Run(true);  //run the TApplication

    //old control mechanism
    //if(m_chosen_event!=-1){
    //return StatusCode::SUCCESS;
    //}
     
    // ATH_MSG_INFO( "Event: " << eventnumber << " -> Press Return to continue to the next event! (q for exit)" );
    //char c = getchar();
   
    //if(c==char('q')){
    // exit(0);
    //}


    //------------------//
    //-- Reset Canvas --//
    //------------------//

    //reset tube circles
    for (unsigned int k=0; k<m_tube_circle_lin.size(); k++) {
        m_tube_circle_lin[k]->reset(); 
    }
    m_canvas->Clear();

    
    return StatusCode::SUCCESS;

}


//*****************************************************************************

//::::::::::::::::::::::::::::
//:: METHOD analyseSegments ::
//::::::::::::::::::::::::::::

StatusCode 
NtupleDisplayTool::analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/) {
    
    return StatusCode::SUCCESS;

}

//*****************************************************************************

//::::::::::::::::::::::
//:: CLASS TubeCircle ::
//::::::::::::::::::::::

TubeCircle::TubeCircle(void) : TEllipse(){
    
    layer=0; multilayer=0; tube=0;
    
    this->reset();
    this->setColors(vector<int>(5,1));
    this->setAdcCut(50.);
    
    
}

TubeCircle::TubeCircle(vector<int> colors, double adc_cut) : TEllipse(){
     layer=0; multilayer=0; tube=0;
   
    this->reset();
    this->setColors(colors);
    this->setAdcCut(adc_cut);
}


TubeCircle::TubeCircle(Amg::Vector3D TubePos, vector<int> colors, double adc_cut) : TEllipse(){
   
    layer=0; multilayer=0; tube=0;
    this->reset();
    this->setPosition(TubePos);
    this->setColors(colors);
    this->setAdcCut(adc_cut);

}

void 
TubeCircle::reset(void){
    
    m_used_flag=0;
    m_rawhit_flag=0;
    m_segmenthit_flag=0;

    this->SetR1(14.6);
    this->SetR2(14.6);
    this->SetPhimin(0.0);
    this->SetPhimax(360.0);
    this->SetFillColor(0);
    this->SetLineColor(1);
    this->SetLineWidth(2);

}

void 
TubeCircle::setPosition(Amg::Vector3D TubePos){

    this->SetX1(TubePos.y());
    this->SetY1(TubePos.z());

}

void
TubeCircle::setColors(std::vector<int> colors){

    m_colors = colors;
}

void
TubeCircle::setAdcCut(double adc_cut){

    m_adc_cut = adc_cut;
}


void 
TubeCircle::setUsedFlag(bool flag){
    
    m_used_flag = flag;
}

bool
TubeCircle::isUsed(void){
    
    return m_used_flag;
}   

void 
TubeCircle::setSegmentHit(double radius, double time, double adc){

    m_segmenthit_flag = true;
    this->setRadius(radius, time, adc, 0);

}

bool 
TubeCircle::hasSegmentHit(void){

    return m_segmenthit_flag;
}

void 
TubeCircle::setRawHit(double radius, double time, double adc){

    m_rawhit_flag = true;

    if(!this->hasSegmentHit()){
        this->setRadius(radius, time, adc, 1);
    }
}

bool 
TubeCircle::hasRawHit(void){

    return m_rawhit_flag;
}

void 
TubeCircle::setRadius(double radius, double time, double adc, int hit){
    
    if(adc<m_adc_cut){
        this->SetFillColor(m_colors.at(3));
        this->SetLineColor(m_colors.at(hit));
    }
    else if(time<0){ 
        this->SetFillColor(m_colors.at(4));
        this->SetLineColor(m_colors.at(hit));
    }
    else if(radius>14.6){
        this->SetFillColor(m_colors.at(hit));
        this->SetLineColor(m_colors.at(2));
    }
    else{
        this->SetR1(radius);
        this->SetR2(radius);
        this->SetLineWidth(0);
        this->SetLineColor(m_colors.at(hit));
        this->SetFillColor(m_colors.at(hit));
    }
    
    //set minimum visible radius
    if(radius<2){
        this->SetR1(2);
        this->SetR2(2);
    }

}

}


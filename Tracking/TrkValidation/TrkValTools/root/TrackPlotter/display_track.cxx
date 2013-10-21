/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "display_track.h"

//#include "PlotValNtuple/Display.h"
#include "ShapeCreator.h"

#include <vector>
#include <iostream>
#include <sstream>


#include "TArc.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TText.h"
#include "TMath.h"

/////////////////////////////////////
/// base class: displayTracks
/////////////////////////////////////
displayTracks::displayTracks() {

    m_trackGraphics = new std::vector<TObject*>();
    m_rioGraphics = new std::vector<TObject*>();
    m_geometryGraphics = new std::vector<TObject*>();
    m_competingRotGraphics = new std::vector<TObject*>();

};

displayTracks::~displayTracks() {
    clear();
    delete m_trackGraphics;
    m_trackGraphics = 0;
    delete m_rioGraphics;
    m_rioGraphics = 0;
    delete m_geometryGraphics;
    m_geometryGraphics = 0;
    delete m_competingRotGraphics;
    m_competingRotGraphics = 0;
}

std::vector<TObject*>* displayTracks::getTrackGraphics() {
    return m_trackGraphics;
}
std::vector<TObject*>* displayTracks::getRIOGraphics() {
    return m_rioGraphics;
}
std::vector<TObject*>* displayTracks::getGeometryGraphics() {
    return m_geometryGraphics;
}
std::vector<TObject*>* displayTracks::getCompetingROTsGraphics() {
    return m_competingRotGraphics;
}

void displayTracks::clearTrackGraphics() {
    while(m_trackGraphics->size()!=0) {
        delete m_trackGraphics->back();
        m_trackGraphics->pop_back();
    }
}
void displayTracks::clearRIOGraphics() {
    while(m_rioGraphics->size()!=0) {
        delete m_rioGraphics->back();
        m_rioGraphics->pop_back();
    }
}

void displayTracks::clearGeometryGraphics() {
    while(m_geometryGraphics->size()!=0) {
        delete m_geometryGraphics->back();
        m_geometryGraphics->pop_back();
    }
}
void displayTracks::clearCompetingROTsGraphics() {
    while(m_competingRotGraphics->size()!=0) {
        delete m_competingRotGraphics->back();
        m_competingRotGraphics->pop_back();
    }
}

void displayTracks::clear() {
    clearTrackGraphics();
    clearRIOGraphics();
    clearGeometryGraphics();
    clearCompetingROTsGraphics();
}


displayBarrelXY::displayBarrelXY() :
displayTracks() {

    //ShapeCreator::setProjection(0);
    m_shapeCreator.setProjection(0);

};

/////////////////////////////////////
/// displayBarrelXY
/////////////////////////////////////

void displayBarrelXY::displayTrackState( GlobalPosition& trkPosition, GlobalDirection& trkDirection) {
    int color(2);
    //if( pt[i] == 1) color = 4;
    // length of track parameters in display (mm)
    double tp_len = 2*10.;
    double tp_r   = 0.1;

    // draw circle at track position
    TArc* trackpp = m_shapeCreator.createArc( trkPosition, tp_r );
    trackpp->SetLineColor( color );
    //m_arcs.push_back(trackpp);
    TObject* trackppObj = trackpp;
    m_trackGraphics->push_back(trackppObj);

    //m_display->addArc( trackpp );

    // draw line in track direction
    TLine* trackp = m_shapeCreator.createLine( trkPosition, trkDirection, tp_len );
    trackp->SetLineWidth( 2 );
    trackp->SetLineColor( color );
    //m_display->addLine( trackp );
    //m_lines.push_back(trackp);
    TObject* trackpObj = trackp;
    m_trackGraphics->push_back(trackpObj);

    // draw line perp. to track direction
    TLine* trackp_o = m_shapeCreator.createOrthogonalLine( trkPosition, trkDirection, tp_len );
    trackp_o->SetLineColor( color );
    //m_display->addLine( trackp_o );
    //m_lines.push_back(trackp_o);
    TObject* trackp_oObj = trackp_o;
    m_trackGraphics->push_back(trackp_oObj);

}

void displayBarrelXY::displayTRT_MeasurementOnTrack( GlobalPosition& strawPosition, double driftRadius, GlobalDirection& trkDirection, bool isOutlier, double) {
    displayTRT_Measurement(strawPosition, driftRadius, true, isOutlier);
    if (!isOutlier) {
      // display marker for left/right solution
      GlobalDirection dir_o(trkDirection.y(), -trkDirection.x(), 0.);
      dir_o *= (trkDirection.z()<0 ? -1 : 1);

      // use an arrow as marker: fine for higher zoom, with less zoom looks strange
      //TArrow* marker = ShapeCreator::createArrow(strawPosition, dir_o, driftRadius);
      TPolyLine* marker = m_shapeCreator.createTriangle(strawPosition, dir_o,
                                                        0.45*driftRadius+(driftRadius<0?-0.3:0.3));
      marker->SetLineColor( 311 );
      marker->SetLineWidth( 2 );
      //m_display->addLine( marker_o );
      //m_lines.push_back(marker_o);
      TObject* markerObj = marker;
      m_trackGraphics->push_back(markerObj);
    }
}

void displayBarrelXY::displayTRT_Measurement( GlobalPosition& strawPosition, double driftRadius,
                                              bool onTrack, bool outlierOnTrack, double) {
    // tube radius
    double tube_radius = 2.;

    std::vector<TObject*>* geometry=0;
    bool backside = strawPosition.z() < 0.;
    int color_hit  = 4; // blue
    int color_tube = 1; // black
    if (onTrack && !outlierOnTrack ) {
        color_hit = backside ? 303 : 4;  // dark/full blue
        geometry = m_trackGraphics;
    } else if (onTrack && outlierOnTrack) {
        color_hit = backside ? 302 : 3;  // dark/full green
        geometry = m_trackGraphics;
    } else {
        color_hit = backside ? 306 : 6;  // dark/full magenta
        geometry = m_rioGraphics;
    }
    // draw TRT tube
    TArc* tube = m_shapeCreator.createArc( strawPosition, tube_radius );
    tube->SetLineColor( color_tube );
    if (!onTrack && backside) tube->SetFillColor( 310 );
    //m_display->addArc( tube );
    //m_arcs.push_back(tube);
    TObject* tubeObj = tube;
    geometry->push_back(tubeObj);

    // draw little flag to distinguish z<0 / z>0 in XY view
    double x_flag = backside ? -1.6 : 1.6;
    GlobalPosition gp_flag(strawPosition.x()+1.6,strawPosition.y()+x_flag,strawPosition.z());
    TArc* tubeFlag =  m_shapeCreator.createArc( gp_flag,0.25 );
    tubeFlag->SetLineColor( color_hit );
    tubeFlag->SetFillColor( color_hit );
    TObject* flagObj = tubeFlag;
    geometry->push_back(flagObj);

    // draw drift circle or full NODRIFTTIME tube
    if (driftRadius != 0.0) {
      TArc* hit = m_shapeCreator.createArc( strawPosition, driftRadius );
      hit->SetLineColor( color_hit );
      hit->SetLineWidth( 2     );
      //m_display->addArc( hit );
      //m_arcs.push_back(hit);
      TObject* hitObj = hit;
      geometry->push_back(hitObj);
    } else {
      TArc* hit = m_shapeCreator.createArc( strawPosition, 1.15 ); // tube used w/o drift time -> 4mm/sqrt(12) error
      hit->SetLineColor( color_hit );
      hit->SetFillColor( color_hit );
      hit->SetLineWidth( 2     );
      TObject* hitObj = hit;
      geometry->push_back(hitObj);
    }

    // draw circle at straw (wire) position
    TArc* tube_c = m_shapeCreator.createArc( strawPosition, 0.05 );
    tube_c->SetLineColor( 1 );
    tube_c->SetFillColor( 1 );
    //m_display->addArc( tube_c );
    //m_arcs.push_back(tube_c);
    TObject* tube_cObj = tube_c;
    geometry->push_back(tube_cObj);
}
void displayBarrelXY::displayTRT_Tubes() {}

void displayBarrelXY::displaySiliconMeasurement( GlobalPosition& measurementPosition, bool onTrack) {
    // marker radius
    double hit_r   = 0.15;
    std::vector<TObject*>* geometry=0;

    int color = 6;
    if (onTrack) {
        color = 4;
        geometry = m_trackGraphics;
    } else {
        color = 6;
        geometry = m_rioGraphics;
    }

    // draw measurement position
    TArc* hit = m_shapeCreator.createArc( measurementPosition, hit_r );
    hit->SetLineColor( color );
    hit->SetFillColor( color );
    //m_display->addArc( hit );
    //m_arcs.push_back(hit);
    TObject* hitObj = hit;
    geometry->push_back(hitObj);

}

void displayBarrelXY::displayCompetingTRT_ROT( std::vector<GlobalPosition>& strawPositions, std::vector<double> driftRadii, std::vector<double> assgnProbs, GlobalDirection& trkDirection) {

    GlobalDirection dir_o(trkDirection.y(), -trkDirection.x(), 0.);
    dir_o *= (trkDirection.z()<0 ? -1 : 1);
    char assgnProbText[50];

    // write the value of the assignment prob near the measurement
    sprintf(assgnProbText,"%e", assgnProbs[0]);
    TText* text = m_shapeCreator.createText(strawPositions[0]+(driftRadii[0]*2.)*dir_o.Unit(), assgnProbText);
    //TText* text = m_shapeCreator.createText(strawPositions[0], assgnProbText);
    // align left if direction points left and align right if direction points right
    text->SetTextAlign((dir_o.x()*driftRadii[0])>0 ? 12 : 32);
    TObject* textObj = text;
    m_competingRotGraphics->push_back(textObj);

    // loop over remaining competing measurements
    for (unsigned int i = 1; i < strawPositions.size(); i++) {
        // draw drift circle
        //TArc* hit = m_shapeCreator.createArc( strawPosition, driftRadius );
        //hit->SetLineColor( color );
        //TObject* hitObj = hit;
        //m_competingRotGraphics->push_back(hitObj);

        // write the value of the assignment prob near the measurement
        sprintf(assgnProbText,"%e", assgnProbs[i]);
        TText* text = m_shapeCreator.createText(strawPositions[i]+(driftRadii[i]*2.)*dir_o.Unit(), assgnProbText);
        //TText* text = m_shapeCreator.createText(strawPositions[i], assgnProbText);
        // align left if direction points left and align right if direction points right
        text->SetTextAlign((dir_o.x()*driftRadii[i])>0 ? 12 : 32);
        TObject* textObj = text;
        m_competingRotGraphics->push_back(textObj);

        if (strawPositions[i] != strawPositions[i-1]) {
            // we have another straw: draw line between straws
            TLine* connection = m_shapeCreator.createLine(strawPositions[i-1], strawPositions[i-1]);
            connection->SetLineColor(16); // gray
            TObject* connectionObj = connection;
            m_competingRotGraphics->push_back(connectionObj);
        }
    }
}


/////////////////////////////////////
/// displayEndcapPhiZ
/////////////////////////////////////

displayEndcapPhiZ::displayEndcapPhiZ() :
displayTracks() {

    //ShapeCreator::setProjection(0);
    m_shapeCreator.setProjection(3);

};


void displayEndcapPhiZ::displayTrackState( GlobalPosition& trkPosition, GlobalDirection& trkDirection) {
    int color(2);
    //if( pt[i] == 1) color = 4;
    // length of track parameters in display (mm)
    double tp_len = 2*10.;
    double tp_r   = 0.1;

    // draw circle at track position
    TArc* trackpp = m_shapeCreator.createArc( trkPosition, tp_r );
    trackpp->SetLineColor( color );
    //m_arcs.push_back(trackpp);
    TObject* trackppObj = trackpp;
    m_trackGraphics->push_back(trackppObj);
    //    if (trackpp) {
    //        std::cout << "set line color of the circle "<<trackpp<< " to "<< color  << std::endl;
    //    }
    //    std::cout << "display: "<<m_display << std::endl;

    //m_display->addArc( trackpp );

    // draw line in track direction
    TLine* trackp = m_shapeCreator.createLine( trkPosition, trkDirection, tp_len );
    trackp->SetLineWidth( 2 );
    trackp->SetLineColor( color );
    //m_display->addLine( trackp );
    //m_lines.push_back(trackp);
    TObject* trackpObj = trackp;
    m_trackGraphics->push_back(trackpObj);

    // draw line perp. to track direction
    TLine* trackp_o = m_shapeCreator.createOrthogonalLine( trkPosition, trkDirection, tp_len );
    trackp_o->SetLineColor( color );
    //m_display->addLine( trackp_o );
    //m_lines.push_back(trackp_o);
    TObject* trackp_oObj = trackp_o;
    m_trackGraphics->push_back(trackp_oObj);

//     TVector3 endOfDirPointer = trkPosition + trkDirection;
// 
//     TEllipse* endOfDir = m_shapeCreator.createEllipse( endOfDirPointer, 0.05, atan(0.05 / trkPosition.Perp()) * 1000. );
//     endOfDir->SetLineColor( color );
//     endOfDir->SetFillColor( color );
//     //m_display->addArc( tube_c );
//     //m_arcs.push_back(tube_c);
//     TObject* endOfDirObj = endOfDir;
//     m_trackGraphics->push_back(endOfDirObj);

}

void displayEndcapPhiZ::displayTRT_MeasurementOnTrack( GlobalPosition& strawPosition, double driftRadius, GlobalDirection& trkDirection, bool isOutlier, double trackRadius) {
    displayTRT_Measurement(strawPosition, driftRadius, true, isOutlier, trackRadius);
/*    // display marker for left/right solution
    GlobalDirection dir_o(trkDirection.y(), -trkDirection.x(), 0.);
    dir_o *= (trkDirection.z()<0 ? -1 : 1);

    // use an arrow as marker: fine for higher zoom, with less zoom looks strange
    //TArrow* marker = ShapeCreator::createArrow(strawPosition, dir_o, driftRadius);
    TPolyLine* marker = ShapeCreator::createTriangle(strawPosition, dir_o, driftRadius);
    marker->SetLineColor( 3 );
    //marker->SetLineWidth( 2 );
    //m_display->addLine( marker_o );
    //m_lines.push_back(marker_o);
    TObject* markerObj = marker;
    m_trackGraphics->push_back(markerObj);*/
  
}

void displayEndcapPhiZ::displayTRT_Measurement( GlobalPosition& strawPosition, double driftRadius, bool onTrack,
                                                bool outlierOnTrack, double trackRadius) {
    // tube radius
    const double tube_radius = 2.;
    const double radiusOfStrawCenters = 823.9;
    //const double phiScale = 180. / TMath::Pi(); // use deg
    const double phiScale = 1000.; // use mrad
    //const double tube_radiusInPhi = tube_radius / radiusOfStrawCenters * phiScale;
    double Rho = radiusOfStrawCenters;
    if (onTrack && (trackRadius > 0.)) {
        Rho = trackRadius;
    }
    //std::cout << "  rho=" << Rho << ", driftradius=" << driftRadius << std::endl;
    std::vector<TObject*>* geometry=0;
    int color_hit  = 4; // blue
    int color_tube = 1; // black
    if (onTrack && !outlierOnTrack ) {
        color_hit = 4;  // full blue
        geometry = m_trackGraphics;
    } else if (onTrack && outlierOnTrack) {
        color_hit = 3;  // full green
        geometry = m_trackGraphics;
    } else {
        color_hit = 6;  // full magenta
        geometry = m_rioGraphics;
    }

    // draw drift circle or full NODRIFTTIME tube
    if (driftRadius != 0.0) {
      TEllipse* hit = m_shapeCreator.createEllipse( strawPosition, driftRadius, atan(driftRadius / Rho) * phiScale );
      hit->SetLineColor( color_hit );
      hit->SetLineWidth( 2     );
      //m_display->addArc( hit );
      //m_arcs.push_back(hit);
      TObject* hitObj = hit;
      geometry->push_back(hitObj);
    } else {
      // tube used w/o drift time -> 4mm/sqrt(12) error
      TEllipse* hit = m_shapeCreator.createEllipse( strawPosition, 1.15, atan(1.15 / Rho) * phiScale );
      hit->SetLineColor( color_hit );
      hit->SetFillColor( color_hit );
      hit->SetLineWidth( 2     );
      TObject* hitObj = hit;
      geometry->push_back(hitObj);
    }

    // draw TRT tube
    TEllipse* tube = m_shapeCreator.createEllipse( strawPosition, tube_radius, atan(tube_radius / Rho) * phiScale );
    tube->SetLineColor( color_tube );
    //m_display->addArc( tube );
    //m_arcs.push_back(tube);
    TObject* tubeObj = tube;
    geometry->push_back(tubeObj);

    // draw circle at straw (wire) position
    TEllipse* tube_c = m_shapeCreator.createEllipse( strawPosition, 0.05, atan(0.05 / Rho) * phiScale );
    tube_c->SetLineColor( color_tube );
    tube_c->SetFillColor( color_tube );
    //m_display->addArc( tube_c );
    //m_arcs.push_back(tube_c);
    TObject* tube_cObj = tube_c;
    geometry->push_back(tube_cObj);
    
    double l = sqrt(strawPosition.y()*strawPosition.y() + strawPosition.x()*strawPosition.x());
    TVector3 maxRadiusInPhiPosition1(Rho*strawPosition.x()/l - driftRadius * strawPosition.y()/l, 
                                     Rho*strawPosition.y()/l + driftRadius * strawPosition.x()/l,
                                     strawPosition.z());
    
    TEllipse* maxRad1 = m_shapeCreator.createEllipse( maxRadiusInPhiPosition1, 0.05, atan(0.05 / Rho) * phiScale );
    maxRad1->SetLineColor( color_hit );
    maxRad1->SetFillColor( color_hit );
    //m_display->addArc( tube_c );
    //m_arcs.push_back(tube_c);
    TObject* maxRad1Obj = maxRad1;
    geometry->push_back(maxRad1Obj);
    TVector3 maxRadiusInPhiPosition2(Rho*strawPosition.x()/l + driftRadius * strawPosition.y()/l, 
                                     Rho*strawPosition.y()/l - driftRadius * strawPosition.x()/l,
                                     strawPosition.z());
    
    TEllipse* maxRad2 = m_shapeCreator.createEllipse( maxRadiusInPhiPosition2, 0.05, atan(0.05 / Rho) * phiScale );
    maxRad2->SetLineColor( color_hit );
    maxRad2->SetFillColor( color_hit );
    //m_display->addArc( tube_c );
    //m_arcs.push_back(tube_c);
    TObject* maxRad2Obj = maxRad2;
    geometry->push_back(maxRad2Obj);
}
void displayEndcapPhiZ::displayTRT_Tubes() {}

void displayEndcapPhiZ::displaySiliconMeasurement( GlobalPosition& measurementPosition, bool onTrack) {
    // marker radius
    double hit_r   = 0.1;
    std::vector<TObject*>* geometry=0;

    int color = 6;
    if (onTrack) {
        color = 4;
        geometry = m_trackGraphics;
    } else {
        color = 6;
        geometry = m_rioGraphics;
    }

    // draw measurement position
    TArc* hit = m_shapeCreator.createArc( measurementPosition, hit_r );
    hit->SetLineColor( color );
    hit->SetFillColor( color );
    //m_display->addArc( hit );
    //m_arcs.push_back(hit);
    TObject* hitObj = hit;
    geometry->push_back(hitObj);

}

void displayEndcapPhiZ::displayCompetingTRT_ROT( std::vector<GlobalPosition>& strawPositions, std::vector<double> driftRadii, std::vector<double> assgnProbs, GlobalDirection& trkDirection) {

/*    GlobalDirection dir_o(trkDirection.y(), -trkDirection.x(), 0.);
    dir_o *= (trkDirection.z()<0 ? -1 : 1);
    char assgnProbText[50];

    // write the value of the assignment prob near the measurement
    sprintf(assgnProbText,"%e", assgnProbs[0]);
    TText* text = ShapeCreator::createText(strawPositions[0]+(driftRadii[0]*2.)*dir_o.Unit(), assgnProbText);
    //TText* text = ShapeCreator::createText(strawPositions[0], assgnProbText);
    // align left if direction points left and align right if direction points right
    text->SetTextAlign((dir_o.x()*driftRadii[0])>0 ? 12 : 32);
    TObject* textObj = text;
    m_competingRotGraphics->push_back(textObj);

    // loop over remaining competing measurements
    for (unsigned int i = 1; i < strawPositions.size(); i++) {
        // draw drift circle
        //TArc* hit = ShapeCreator::createArc( strawPosition, driftRadius );
        //hit->SetLineColor( color );
        //TObject* hitObj = hit;
        //m_competingRotGraphics->push_back(hitObj);

        // write the value of the assignment prob near the measurement
        sprintf(assgnProbText,"%e", assgnProbs[i]);
        TText* text = ShapeCreator::createText(strawPositions[i]+(driftRadii[i]*2.)*dir_o.Unit(), assgnProbText);
        //TText* text = ShapeCreator::createText(strawPositions[i], assgnProbText);
        // align left if direction points left and align right if direction points right
        text->SetTextAlign((dir_o.x()*driftRadii[i])>0 ? 12 : 32);
        TObject* textObj = text;
        m_competingRotGraphics->push_back(textObj);

        if (strawPositions[i] != strawPositions[i-1]) {
            // we have another straw: draw line between straws
            TLine* connection = ShapeCreator::createLine(strawPositions[i-1], strawPositions[i-1]);
            connection->SetLineColor(16); // gray
            TObject* connectionObj = connection;
            m_competingRotGraphics->push_back(connectionObj);
        }
    }*/
}



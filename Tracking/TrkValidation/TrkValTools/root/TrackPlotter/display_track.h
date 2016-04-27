/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DISPLAY_TRACK_H
#define DISPLAY_TRACK_H

//#include "PlotValNtuple/Display.h"
//#include "CLHEP/Vector/ThreeVector.h"
#include <vector>
#include "TVector3.h"
#include "ShapeCreator.h"

typedef TVector3 GlobalPosition;
typedef TVector3 GlobalDirection;

//class Display;
//namespace CLHEP {
//class Hep3Vector;
//}

//class TLine;
//class TArc;
//class TBox;
class TObject;
//class ShapeCreator;

class displayTracks {
public:
    displayTracks();
    virtual ~displayTracks();
    /** create the graphic objects for a single track state */
    virtual void displayTrackState( GlobalPosition& trkPosition, GlobalDirection& trkDirection) = 0;
    /** create the graphic objects for a single TRT measurement on track */
    virtual void displayTRT_MeasurementOnTrack( GlobalPosition& strawPosition, double driftRadius,
                                                GlobalDirection& trkDirection, bool isOutlier,
                                                double trackRadius = 0.) = 0;
    /** create the graphic objects for a single TRT measurement */
    virtual void displayTRT_Measurement( GlobalPosition& strawPosition, double driftRadius,
                                         bool onTrack, bool outlierOnTrack, double trackRadius = 0.) = 0;
    virtual void displayTRT_Tubes() = 0;
    /** create the graphic objects for a single Silicon measurement */
    virtual void displaySiliconMeasurement( GlobalPosition& measurementPosition, bool onTrack) = 0;
    /** create the graphic objects for a single CompetingTRT_DriftCirclesOnTrack */
    virtual void displayCompetingTRT_ROT( std::vector<GlobalPosition>& strawPositions, std::vector<double> driftRadii, std::vector<double> assgnProbs, GlobalDirection& trkDirection)=0;
    /** clear all graphics vectors */
    virtual void clear();
    
    virtual void clearTrackGraphics();
    virtual void clearRIOGraphics();
    virtual void clearGeometryGraphics();
    virtual void clearCompetingROTsGraphics();

    //void draw();
    //void print(std::string name){m_display->print(name);}
    //std::vector<TLine*>& getLines();
    //std::vector<TArc*>& getArcs();
    //std::vector<TBox*>& getBoxes();
    virtual std::vector<TObject*>* getTrackGraphics();
    virtual std::vector<TObject*>* getRIOGraphics();
    virtual std::vector<TObject*>* getGeometryGraphics();
    virtual std::vector<TObject*>* getCompetingROTsGraphics();
protected:
    //Display* displayDriftTubeBarrelTrackSection( DriftTubeBarrelTrackSection& section );
    //void getRange( const std::vector<GlobalPosition>&, GlobalPosition& ll, GlobalPosition& rh);
    //GlobalPosition m_lowerLeft;
    //GlobalPosition m_upperRight;
    //Display* m_display;
    //std::vector<TLine*> m_lines;
    //std::vector<TArc*>  m_arcs;
    //std::vector<TBox*>  m_boxes;
    std::vector<TObject*>* m_trackGraphics;
    std::vector<TObject*>* m_rioGraphics;
    std::vector<TObject*>* m_geometryGraphics;
    std::vector<TObject*>* m_competingRotGraphics;
    
    ShapeCreator m_shapeCreator;
};



class displayBarrelXY : public displayTracks {
public:
    //displayBarrelXY(GlobalPosition& lowerLeft, GlobalPosition& upperRight );
    displayBarrelXY();
    //~displayBarrelXY();
    /** create the graphic objects for a single track state */
    virtual void displayTrackState( GlobalPosition& trkPosition, GlobalDirection& trkDirection);
    /** create the graphic objects for a single TRT measurement on track */
    virtual void displayTRT_MeasurementOnTrack( GlobalPosition& strawPosition, double driftRadius,
                                                GlobalDirection& trkDirection, bool isOutlier,
                                                double = 0.);
    /** create the graphic objects for a single TRT measurement */
    virtual void displayTRT_Measurement( GlobalPosition& strawPosition, double driftRadius,
                                         bool onTrack, bool outlierOnTrack, double = 0.);
    virtual void displayTRT_Tubes();
    /** create the graphic objects for a single Silicon measurement */
    virtual void displaySiliconMeasurement( GlobalPosition& measurementPosition, bool onTrack);
    /** create the graphic objects for a single CompetingTRT_DriftCirclesOnTrack */
    virtual void displayCompetingTRT_ROT( std::vector<GlobalPosition>& strawPositions, std::vector<double> driftRadii, std::vector<double> assgnProbs, GlobalDirection& trkDirection);

private:

};

class displayEndcapPhiZ : public displayTracks {
public:
    //displayBarrelXY(GlobalPosition& lowerLeft, GlobalPosition& upperRight );
    displayEndcapPhiZ();
    //~displayEndcapPhiZ();
    /** create the graphic objects for a single track state */
    virtual void displayTrackState( GlobalPosition& trkPosition, GlobalDirection& trkDirection);
    /** create the graphic objects for a single TRT measurement on track */
    virtual void displayTRT_MeasurementOnTrack( GlobalPosition& strawPosition, double driftRadius,
                                                GlobalDirection& trkDirection, bool isOutlier,
                                                double trackRadius = 0.);
    /** create the graphic objects for a single TRT measurement */
    virtual void displayTRT_Measurement( GlobalPosition& strawPosition, double driftRadius,
                                         bool onTrack, bool outlierOnTrack, double trackRadius = 0.);
    virtual void displayTRT_Tubes();
    /** create the graphic objects for a single Silicon measurement */
    virtual void displaySiliconMeasurement( GlobalPosition& measurementPosition, bool onTrack);
    /** create the graphic objects for a single CompetingTRT_DriftCirclesOnTrack */
    virtual void displayCompetingTRT_ROT( std::vector<GlobalPosition>& strawPositions, std::vector<double> driftRadii, std::vector<double> assgnProbs, GlobalDirection& trkDirection);

//     std::vector<TObject*>* getTrackGraphics() {return m_trackGraphics;}
//     std::vector<TObject*>* getRIOGraphics() {return m_rioGraphics;}
//     std::vector<TObject*>* getGeometryGraphics() {return m_geometryGraphics;}
//     std::vector<TObject*>* getCompetingROTsGraphics() {return m_competingRotGraphics;}
private:
    
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLOTVALNTUPLE_H
#define PLOTVALNTUPLE_H

//#include "PlotValNtuple/TrackData.h"
//#include "CLHEP/Vector/ThreeVector.h"
#include <vector>

#include "display_track.h"
#include "TrackDataNtupleTree.h"
#include "TrkValidationNtupleRIOs.h"

//typedef Hep3Vector GlobalPosition;
//typedef Hep3Vector GlobalDirection;

// class Display;
// namespace CLHEP{
// class Hep3Vector;
// }

enum DisplayMode { BarrelXY, EndCapPhiZ };

class TObject;

class PlotValNtuple {
public:
    PlotValNtuple( TrackDataNtupleTree &trackNtuple, TrkValidationNtupleRIOs *rioNtuple, DisplayMode mode=BarrelXY);

    void setNextTrack(int& eventNumber, int& trackNumber, int& InterationNumber);
    void setPrevTrack(int& eventNumber, int& trackNumber, int& InterationNumber);

    void setNextEvent(int& eventNumber);
    void setPrevEvent(int& eventNumber);

    void setEvent(int eventNumber, int InterationNumber);
    void setTrack(int eventNumber, int trackNumber, int InterationNumber);

    void setMode(DisplayMode mode) {
        m_mode = mode;
    }

    //    /** plot a single track */
    //std::vector<TObject*>* plotBarrelXY_SingleTrack(TrackDataNtupleTree &, Long64_t entry=0);
    //    std::vector<TObject*>& plotBarrelXY_SingleTrack(TrackDataNtupleTree&, Long64_t entry);
    //displayBarrelXY* plotBarrelXY_SingleTrack(TrackDataNtupleTree &, Long64_t entry=0);
    //    /** plot a whole event, return the index of the last entry belonging to the event by reference */
    //    std::vector<TObject*>& plotBarrelXY_Event(TrackDataNtupleTree&, Long64_t entry, Long64_t &lastEntryOfEvent);

    //    /** plot the RIOs */
    //    std::vector<TObject*>& plotBarrelXY_RIOs(TrkValidationNtupleRIOs&, int eventNumber);

    //    /** plot CompetingRIOsOnTrack for a single track */
    //    //  std::vector<TObject*>& plotBarrelXY_SingleTrackCompROTs(TrackDataNtupleTree&, Long64_t entry);
    //    /** plot CompetingRIOsOnTrack for whole event, return the index of the last entry belonging to the event by reference */
    //    //  std::vector<TObject*>& plotBarrelXY_EventCompROTs(TrackDataNtupleTree&, Long64_t entry, Long64_t &lastEntryOfEvent);

    std::vector<TObject*>* getTrackGraphics() {
        switch (m_mode) {
        case BarrelXY:
            return m_barrelXY.getTrackGraphics();
            break;
        case EndCapPhiZ:
            return m_endcapPhiZ.getTrackGraphics();
            break;
        }
        return 0;
    }
    std::vector<TObject*>* getRIOGraphics() {
        switch (m_mode) {
        case BarrelXY:
            return m_barrelXY.getRIOGraphics();
            break;
        case EndCapPhiZ:
            return m_endcapPhiZ.getRIOGraphics();
            break;
        }
        return 0;
    }
    std::vector<TObject*>* getGeometryGraphics() {
        switch (m_mode) {
        case BarrelXY:
            return m_barrelXY.getGeometryGraphics();
            break;
        case EndCapPhiZ:
            return m_endcapPhiZ.getGeometryGraphics();
            break;
        }
        return 0;
    }
    std::vector<TObject*>* getCompetingROTsGraphics() {
        switch (m_mode) {
        case BarrelXY:
            return m_barrelXY.getCompetingROTsGraphics();
            break;
        case EndCapPhiZ:
            return m_endcapPhiZ.getCompetingROTsGraphics();
            break;
        }
        return 0;
    }


private:
    /** plot one track */
    //std::vector<TObject*>* plotBarrelXY_SingleTrack(TrackDataNtupleTree &, Long64_t entry=0);
    void plotTrack();
    void plotRIOs(int eventNumber);
    //  /** get the range of the track */
    //  void getBarrelXY_Range(TrackDataNtupleTree&, GlobalPosition& lowerLeft, GlobalPosition& upperRight);

    //Display* displayDriftTubeBarrelTrackSection( DriftTubeBarrelTrackSection& section );
    //void getRange( const std::vector<GlobalPosition>&, GlobalPosition& ll, GlobalPosition& rh);
    //    GlobalPosition m_lowerLeft;
    //    GlobalPosition m_upperRight;

    Long64_t m_trackNtupleEntry;
    //Long64_t m_rioNtupleEntry;
    TrackDataNtupleTree& m_trackNtuple;
    TrkValidationNtupleRIOs* m_rioNtuple;

    displayBarrelXY m_barrelXY;
    displayEndcapPhiZ m_endcapPhiZ;
    DisplayMode m_mode;
};


#endif // PLOTVALNTUPLE_H

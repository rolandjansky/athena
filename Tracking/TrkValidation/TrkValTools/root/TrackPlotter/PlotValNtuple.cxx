/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PlotValNtuple.h"
#include <iostream>
#include <math.h>

PlotValNtuple::PlotValNtuple( TrackDataNtupleTree &trackNtuple, TrkValidationNtupleRIOs *rioNtuple, DisplayMode mode) :
    m_trackNtupleEntry(0),
    //m_rioNtupleEntry(0),
    m_trackNtuple(trackNtuple),
    m_rioNtuple(rioNtuple),
    m_mode(mode) {
    //std::cout<<"calling cstr from PlotValNtuple"<< std::endl;
    // init the ntuples to avoid problems
    //(*m_trackNtuple.Init();
    m_trackNtuple.GetEntry(m_trackNtupleEntry);
    if (m_rioNtuple)
        m_rioNtuple->GetEntry(0);
}

void PlotValNtuple::setNextTrack(int& eventNumber, int& trackNumber, int& InterationNumber){
    // in case of troubles: set numbers to current entry
    eventNumber = int(m_trackNtuple.EventNumber);
    trackNumber = int(m_trackNtuple.TrackID);
    InterationNumber = int(m_trackNtuple.IterationIndex);
    
    Long64_t nentries = m_trackNtuple.fChain->GetEntriesFast();
    if (m_trackNtupleEntry+1 >= nentries)
        return;
    
    // get the entry:
    if (m_trackNtuple.GetEntry(m_trackNtupleEntry++)==0){
        m_trackNtupleEntry--;
        return;
    }
    // clear the track objects:
    m_barrelXY.clearTrackGraphics();
    m_barrelXY.clearCompetingROTsGraphics();
    m_endcapPhiZ.clearTrackGraphics();
    m_endcapPhiZ.clearCompetingROTsGraphics();
    // get the new track graphics
    plotTrack();
    // do we have a new event?
    if (eventNumber != int(m_trackNtuple.EventNumber)){
        // clear also the RIO graphics
        //m_barrelXY.clearRIOGraphics();
        plotRIOs(int(m_trackNtuple.EventNumber));
        
    }
    // set the numbers to the new values:    
    eventNumber = int(m_trackNtuple.EventNumber);
    trackNumber = int(m_trackNtuple.TrackID);
    InterationNumber = int(m_trackNtuple.IterationIndex);
}

void PlotValNtuple::setPrevTrack(int& eventNumber, int& trackNumber, int& InterationNumber){
    // in case of troubles: set numbers to current entry
    eventNumber = int(m_trackNtuple.EventNumber);
    trackNumber = int(m_trackNtuple.TrackID);
    InterationNumber = int(m_trackNtuple.IterationIndex);
    
    if (m_trackNtupleEntry <= 0)
        return;
    
    // get the entry:
    if (m_trackNtuple.GetEntry(m_trackNtupleEntry--)==0){
        m_trackNtupleEntry++;
        return;
    }
    // clear the track objects:
    m_barrelXY.clearTrackGraphics();
    m_barrelXY.clearCompetingROTsGraphics();
    m_endcapPhiZ.clearTrackGraphics();
    m_endcapPhiZ.clearCompetingROTsGraphics();
    // get the new track graphics
    plotTrack();
    // do we have a new event?
    if (eventNumber != int(m_trackNtuple.EventNumber)){
        // clear also the RIO graphics
        //m_barrelXY.clearRIOGraphics();
        plotRIOs(int(m_trackNtuple.EventNumber));
        
    }
    // set the numbers to the new values:    
    eventNumber = int(m_trackNtuple.EventNumber);
    trackNumber = int(m_trackNtuple.TrackID);
    InterationNumber = int(m_trackNtuple.IterationIndex);
}

void PlotValNtuple::setNextEvent(int& eventNumber){
    // in case of troubles: set numbers to current entry
    eventNumber = int(m_trackNtuple.EventNumber);
    //InterationNumber = int(m_trackNtuple.IterationIndex);
    
    Long64_t nentries = m_trackNtuple.fChain->GetEntriesFast();
    if (m_trackNtupleEntry+1 >= nentries)
        return;
    
    // get the first entry of the event:
    if (m_trackNtuple.GetEntry(++m_trackNtupleEntry)==0){
        m_trackNtupleEntry--;
        return;
    }
    // get the eventNumber
    eventNumber = int(m_trackNtuple.EventNumber);
    //InterationNumber = int(m_trackNtuple.IterationIndex);
    
    // clear the track and rio objects:
    m_barrelXY.clearTrackGraphics();
    m_barrelXY.clearCompetingROTsGraphics();
    m_endcapPhiZ.clearTrackGraphics();
    m_endcapPhiZ.clearCompetingROTsGraphics();
    //m_barrelXY.clearRIOGraphics();
    // get the new RIO graphics
    plotRIOs(eventNumber);
    // get the new track graphics
    for (; m_trackNtupleEntry < nentries; m_trackNtupleEntry++) {
        m_trackNtuple.GetEntry(m_trackNtupleEntry);
        std::cout << "Entry " << m_trackNtupleEntry << " has eventNumber  " << m_trackNtuple.EventNumber<< std::endl;
        if (int(m_trackNtuple.EventNumber) != eventNumber) break;
        plotTrack();
    }
    // set entry number to last entry of the event
    m_trackNtupleEntry--;
    m_trackNtuple.GetEntry(m_trackNtupleEntry);
}

void PlotValNtuple::setPrevEvent(int& eventNumber){
    // in case of troubles: set numbers to current entry
    eventNumber = int(m_trackNtuple.EventNumber);
    //InterationNumber = int(m_trackNtuple.IterationIndex);
    
    if (m_trackNtupleEntry <= 0)
        return;
    
    // get the last entry of the previous event:
    for (; m_trackNtupleEntry >= 0; m_trackNtupleEntry--) {
        m_trackNtuple.GetEntry(m_trackNtupleEntry);
        std::cout << "Entry " << m_trackNtupleEntry << " has eventNumber  " << m_trackNtuple.EventNumber<< std::endl;
        if (int(m_trackNtuple.EventNumber) != eventNumber) break;
    }
    if (int(m_trackNtuple.EventNumber) == eventNumber)
        return;
    Long64_t lastEntryOfEvent = m_trackNtupleEntry;
    
    // get the eventNumber
    eventNumber = int(m_trackNtuple.EventNumber);
    //InterationNumber = int(m_trackNtuple.IterationIndex);
    
    // clear the track and rio objects:
    m_barrelXY.clearTrackGraphics();
    m_barrelXY.clearCompetingROTsGraphics();
    m_endcapPhiZ.clearTrackGraphics();
    m_endcapPhiZ.clearCompetingROTsGraphics();
    //m_barrelXY.clearRIOGraphics();
    // get the new RIO graphics
    plotRIOs(eventNumber);
    // get the new track graphics
    for (; m_trackNtupleEntry >=0; m_trackNtupleEntry--) {
        m_trackNtuple.GetEntry(m_trackNtupleEntry);
        std::cout << "Entry " << m_trackNtupleEntry << " has eventNumber  " << m_trackNtuple.EventNumber<< std::endl;
        if (int(m_trackNtuple.EventNumber) != eventNumber) break;
        plotTrack();
    }
    // set entry number to last entry of the event
    m_trackNtupleEntry = lastEntryOfEvent;
    m_trackNtuple.GetEntry(m_trackNtupleEntry);
}

void PlotValNtuple::setEvent(int eventNumber, int InterationNumber){}
void PlotValNtuple::setTrack(int eventNumber, int trackNumber, int InterationNumber){}


// std::vector<TObject*>& PlotValNtuple::plotBarrelXY_SingleTrack(TrackDataNtupleTree &trackNtuple, Long64_t entry) {
//     // clear the vector of graphic objects
//     m_barrelXY.clear();
//     // check for number of entries
//     Long64_t nentries = trackNtuple.fChain->GetEntriesFast();
//     if (entry > nentries)
//         return m_barrelXY.getTrackGraphics();
//     
//     // get the entry:
//     if (trackNtuple.GetEntry(entry)==0)
//         return m_barrelXY.getTrackGraphics();
// 
//     plotBarrelXY_Track(trackNtuple);
//     return m_barrelXY.getTrackGraphics();
// }


void PlotValNtuple::plotTrack() {


    
    std::cout << "*** Track: Processing run " << m_trackNtuple.RunNumber
              << ", event " << m_trackNtuple.EventNumber
              << ", track " << m_trackNtuple.TrackID
              << ", iteration " << m_trackNtuple.IterationIndex << std::endl;

        
    // determine the range of the track
    //GlobalPosition lowerLeft;
    //GlobalPosition upperRight;
    //getBarrelXY_Range(trackNtuple, lowerLeft, upperRight);

    // get the display for barrel in xy projection
    //displayBarrelXY* barrelDisplay = new displayBarrelXY(lowerLeft, upperRight);
    //displayBarrelXY barrelDisplay;

    // loop over the track states
    std::cout << "Entry has " << m_trackNtuple.nHits << " track states  "<< std::endl;
    for( int i=1; i<m_trackNtuple.nHits; ++i) {
        // distinguish barrel and endcap modules:
        bool barrel = true;
        if (fabs( (*m_trackNtuple.HitZ)[i] )>800.) {
            // end cap!
            barrel = false;
        }

/*        if (fabs(trackNtuple.DetectorType)[i])!=3)
            continue;            */
        // draw the track states
        GlobalPosition trkPos((*m_trackNtuple.TrackX)[i], (*m_trackNtuple.TrackY)[i], (*m_trackNtuple.TrackZ)[i]);
        GlobalDirection trkMom((*m_trackNtuple.TrackMomentumX)[i], (*m_trackNtuple.TrackMomentumY)[i], (*m_trackNtuple.TrackMomentumZ)[i]);
        GlobalDirection trkDir(trkMom.Unit());
//        std::cout << "  Track state with global position " << trkPos << " and direction  "<< trkDir<< std::endl;
// WL        m_barrelXY.displayTrackState(trkPos, trkMom);
        // draw the measurements
        GlobalPosition *measPos=0;
        //std::cout << "  detector type: " << (*m_trackNtuple.DetectorType)[i] << std::endl;
        switch ((*m_trackNtuple.DetectorType)[i]) {
        case 1:
        case 2:
            // Pixel or SCT hit
            measPos = new GlobalPosition((*m_trackNtuple.HitX)[i], (*m_trackNtuple.HitY)[i], (*m_trackNtuple.HitZ)[i]);
            //std::cout << "  silicon measurement at " << *measPos << std::endl;
            m_barrelXY.displaySiliconMeasurement(*measPos, true);
            m_endcapPhiZ.displaySiliconMeasurement(*measPos, true);
            break;
        case 3:
            measPos = new GlobalPosition((*m_trackNtuple.HitSurfaceX)[i], (*m_trackNtuple.HitSurfaceY)[i], (*m_trackNtuple.HitSurfaceZ)[i]);
            //std::cout << "  TRT measurement at " << *measPos << " with driftradius  "<< (*m_trackNtuple.HitLocal1)[i] << std::endl;
            // if ((*m_trackNtuple.outlierFlag)[i] > 0) std::cout << " ## outlier on ntuple." << std::endl;
            if (barrel) {
                m_barrelXY.displayTRT_MeasurementOnTrack(*measPos,
                                                        (*m_trackNtuple.HitLocal1)[i],
                                                        trkDir,
                                                        ((*m_trackNtuple.outlierFlag)[i] > 0));
            }else{
                m_endcapPhiZ.displayTRT_MeasurementOnTrack(*measPos,
                        (*m_trackNtuple.HitLocal1)[i],
                        trkDir,
                        ((*m_trackNtuple.outlierFlag)[i] > 0), trkPos.Perp());
            }
            //std::cout << "Track Pos: (X, Y, Z)=(" << trkPos.x() << ","  << trkPos.y() << "," << trkPos.z() << ") => (R, Phi) = (" << trkPos.Perp() << ","  << trkPos.Phi() << "),  driftradius: " << (*m_trackNtuple.HitLocal1)[i] << std::endl;
            //std::cout << "straw Pos: (X, Y, Z)=(" << measPos->x() << ","  << measPos->y() << "," << measPos->z() << ") => (R, Phi) = (" << measPos->Perp() << ","  << measPos->Phi() << ")"  << std::endl;
            //std::cout << "Track Dir: (X, Y, Z)=(" << trkDir.x() << ","  << trkDir.y() << "," << trkDir.z() << ") => (R, Phi) = (" << trkDir.Perp() << ","  << trkDir.Phi() << ")"  << std::endl;
            // do we have competing ROT info?
            //std::cout << "  containes " << (*m_trackNtuple.CompROTnContainedROTs)[i] << " ROTs" << std::endl;
            if (m_trackNtuple.CompROTnContainedROTs && (*m_trackNtuple.CompROTnContainedROTs)[i] > 0) {
                std::vector<GlobalPosition> strawPositions;
                std::vector<double> driftRadii;
                std::vector<double> assgnProbs;
                //std::cout << "   containes " << (*m_trackNtuple.CompROTnContainedROTs << " ROTs  "<< std::endl;
/*                for( int rotIndex=0; rotIndex<(*m_trackNtuple.CompROTnContainedROTs)[i]; ++rotIndex) {
                    GlobalPosition measPos((*m_trackNtuple.CompROTcontainedROTsHitSurfaceX)[i][rotIndex], (*m_trackNtuple.CompROTcontainedROTsHitSurfaceY)[i][rotIndex], (*m_trackNtuple.CompROTcontainedROTsHitSurfaceZ)[i][rotIndex]);
        //            std::cout << "  TRT measurement at " << *measPos << " with driftradius  "<< trackNtuple.HitLocal1)[i] << std::endl;
                    strawPositions.push_back(measPos);
                    driftRadii.push_back((*m_trackNtuple.CompROTcontainedROTsHitLocal1)[i][rotIndex]);
                    assgnProbs.push_back((*m_trackNtuple.CompROTcontainedROTsAssgnProbs)[i][rotIndex]);
                }*/
                if (barrel){
                    m_barrelXY.displayCompetingTRT_ROT( strawPositions, driftRadii, assgnProbs, trkDir);
                }else{
                    m_endcapPhiZ.displayCompetingTRT_ROT( strawPositions, driftRadii, assgnProbs, trkDir);
                }
            }
            break;
        }
        m_barrelXY.displayTrackState(trkPos, trkDir);
        m_endcapPhiZ.displayTrackState(trkPos, trkDir);
        delete measPos;
    }
    //barrelDisplay->draw();
    
    //return barrelDisplay.getGraphicObjects();
}

// std::vector<TObject*>& PlotValNtuple::plotBarrelXY_Event(TrackDataNtupleTree& tree,
//                                                          Long64_t entry,
//                                                          Long64_t &lastEntryOfEvent) {
//     // clear the vector of graphic objects
//     m_barrelXY.clear();
//     // check for number of entries
//     Long64_t nentries = tree.fChain->GetEntriesFast();
//     if (entry > nentries)
//         return m_barrelXY.getTrackGraphics();
//             
//     // get the first entry
//     if (tree.GetEntry(entry)==0)
//         return m_barrelXY.getTrackGraphics();
//     // plot the first entry:
//     plotBarrelXY_Track(tree);
//     
//     Int_t eventNumber = tree.EventNumber;
//     Long64_t i = entry+1;
//     for (; i < nentries; i++) {
//         tree.GetEntry(i);
//         if (tree.EventNumber != eventNumber) break;
//         plotBarrelXY_Track(tree);
//     }
//     // jump back to first entry of the event
//     tree.GetEntry(entry);
//     lastEntryOfEvent = i-1;
//     
//     return m_barrelXY.getTrackGraphics();
// }

void PlotValNtuple::plotRIOs(int eventNumber){
    if (!m_rioNtuple) return;

    // clear the vector of graphic objects
    m_barrelXY.clearRIOGraphics();
    m_endcapPhiZ.clearRIOGraphics();
    // check for number of entries
    Long64_t nentries = m_rioNtuple->fChain->GetEntriesFast();
                
    // get the entry with the eventNumber
    
    for (int i=0; i < nentries; i++) {
        m_rioNtuple->GetEntry(i);
        if (int(m_rioNtuple->TRT_EventNumber) != eventNumber) continue;    
        std::cout << "Entry has " << m_rioNtuple->TRT_nRIOs << " RIOs  "<< std::endl;
        for( int rioIndex=0; rioIndex<m_rioNtuple->TRT_nRIOs; ++rioIndex) {
            // cut for barrel modules:
            if (fabs((*m_rioNtuple->TRT_SurfaceZ)[rioIndex])>800.) {
                // endcap:
                GlobalPosition strawPos((*m_rioNtuple->TRT_SurfaceX)[rioIndex], (*m_rioNtuple->TRT_SurfaceY)[rioIndex], (*m_rioNtuple->TRT_SurfaceZ)[rioIndex]);
                m_endcapPhiZ.displayTRT_Measurement( strawPos, (*m_rioNtuple->TRT_DriftRadius)[rioIndex], false, false);
            }else {
                // barrel:
                //std::cout << "Z: " << tree.TRT_SurfaceZ[rioIndex]<< std::endl;
                GlobalPosition strawPos((*m_rioNtuple->TRT_SurfaceX)[rioIndex], (*m_rioNtuple->TRT_SurfaceY)[rioIndex], (*m_rioNtuple->TRT_SurfaceZ)[rioIndex]);
                m_barrelXY.displayTRT_Measurement( strawPos, (*m_rioNtuple->TRT_DriftRadius)[rioIndex], false, false);
            }
        }
    
    }
    //std::cout << "return RIO graphic objects"<< std::endl;
    //return m_barrelXY_RIOs.getRIOGraphics();
}

// std::vector<TObject*>& PlotValNtuple::plotBarrelXY_SingleTrackCompROTs(TrackDataNtupleTree&, Long64_t entry){
//     
// }
// 
// std::vector<TObject*>& PlotValNtuple::plotBarrelXY_EventCompROTs(TrackDataNtupleTree&, Long64_t entry, Long64_t &lastEntryOfEvent){
// 
// }

// void PlotValNtuple::plotBarrelXY_Track(TrackDataNtupleTree &trackNtuple) {
// 
// 
//     
//     std::cout << "*** BarrelXY_CompetingROTs: Processing run " << trackNtuple.RunNumber
//               << ", event " << trackNtuple.EventNumber
//               << ", track " << trackNtuple.TrackID
//               << ", iteration " << trackNtuple.IterationIndex << std::endl;
// 
//         
//     // loop over the track states
//     std::cout << "Entry has " << trackNtuple.nHits << " track states  "<< std::endl;
//     for( int i=1; i<trackNtuple.nHits; ++i) {
//         // cut for barrel modules:
//         if (fabs(trackNtuple.TrackZ)[i])>800.)
//             continue;
// /*        if (fabs(trackNtuple.DetectorType)[i])!=3)
//             continue;            */
//         // draw the track states
//         GlobalPosition trkPos(trackNtuple.TrackX)[i], trackNtuple.TrackY)[i], trackNtuple.TrackZ)[i]);
//         GlobalDirection trkMom(trackNtuple.TrackMomentumX)[i], trackNtuple.TrackMomentumY)[i], trackNtuple.TrackMomentumZ)[i]);
// //        GlobalDirection trkDir(trkMom.Unit());
// //        std::cout << "  Track state with global position " << trkPos << " and direction  "<< trkDir<< std::endl;
//         m_barrelXY.displayTrackState(trkPos, trkMom);
//         // draw the measurements
//         GlobalPosition *measPos=0;
// //        std::cout << "  detector type: " << trackNtuple.DetectorType)[i] << std::endl;
//         switch (trackNtuple.DetectorType)[i]) {
//         case 1:
//         case 2:
//             measPos = new GlobalPosition(trackNtuple.HitX)[i], trackNtuple.HitY)[i], trackNtuple.HitZ)[i]);
// //            std::cout << "  silicon measurement at " << *measPos << std::endl;
//             m_barrelXY.displaySiliconMeasurement(*measPos);
//             break;
//         case 3:
//             measPos = new GlobalPosition(trackNtuple.HitSurfaceX)[i], trackNtuple.HitSurfaceY)[i], trackNtuple.HitSurfaceZ)[i]);
// //            std::cout << "  TRT measurement at " << *measPos << " with driftradius  "<< trackNtuple.HitLocal1)[i] << std::endl;
//             m_barrelXY.displayTRT_MeasurementOnTrack(*measPos,
//                                                         trackNtuple.HitLocal1)[i],
//                                                         trkMom);
//             break;
//         }
//         delete measPos;
//     }
//     //barrelDisplay->draw();
//     
//     //return barrelDisplay.getGraphicObjects();
// }


// void PlotValNtuple::getBarrelXY_Range(TrackDataNtupleTree& ntupleTree, GlobalPosition& lowerLeft, GlobalPosition& upperRight) {
//     // void display_track::calculateRange( const std::vector<GlobalPosition>& tp, GlobalPosition& ll,
//     //                  GlobalPosition& rh){
//     //
//     // loop over hits find min and max
//     double minx = 2000.;
//     double miny = 2000.;
//     double maxx = -2000.;
//     double maxy = -2000.;
// 
//     for( int i=0; i<ntupleTree.nHits; ++i) {
//         // cut for barrel modules:
//         if (fabs(ntupleTree.TrackZ)[i])>800.)
//             continue;
//         if (ntupleTree.DetectorType)[i] != 3)
//             continue;
//         if( ntupleTree.TrackX)[i] < minx )
//             minx =  ntupleTree.TrackX)[i];
//         if( ntupleTree.TrackY)[i] < miny )
//             miny =  ntupleTree.TrackY)[i];
//         if( ntupleTree.TrackX)[i] > maxx )
//             maxx =  ntupleTree.TrackX)[i];
//         if( ntupleTree.TrackY)[i] > maxy )
//             maxy =  ntupleTree.TrackY)[i];
//     }
// 
//     double offset = 10.;
//     // lower left
//     lowerLeft.setX(minx-offset);
//     lowerLeft.setY(miny-offset);
// 
//     // upper right
//     upperRight.setX(maxx+offset);
//     upperRight.setY(maxy+offset);
// 
// //    std::cout << " calculated range " << lowerLeft << "   " << upperRight << std::endl;
// 
// }

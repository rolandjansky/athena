/** @file RoI.h */

#ifndef TrigInDetAnalysisUtils_RoI_H
#define TrigInDetAnalysisUtils_RoI_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/Track.h"



class RoI {
  
	//    friend class AnalysisConfig;

 public:

                RoI(TIDARoiDescriptor* roiInfo) : m_roiInfo(roiInfo) { }

		// Accessor methods
		const TIDARoiDescriptor* descriptor() const {return  m_roiInfo;}

		const std::vector<TrigInDetAnalysis::Track*>& testTracks()      const {return m_testTracks;}
		const std::vector<TrigInDetAnalysis::Track*>& referenceTracks() const {return m_referenceTracks;}
		const std::vector<TrigInDetAnalysis::Track*>& selectionTracks() const {return m_selectionTracks;}

		//  protected:

		// Accessor methods
		void testTracks(std::vector<TrigInDetAnalysis::Track*>& tracks)      {m_testTracks = tracks;}
		void referenceTracks(std::vector<TrigInDetAnalysis::Track*>& tracks) {m_referenceTracks = tracks;}
		void selectionTracks(std::vector<TrigInDetAnalysis::Track*>& tracks) {m_selectionTracks = tracks;}
		void tracks(std::vector<TrigInDetAnalysis::Track*>& tracks, unsigned int index) {
			switch(index) {
				case 0: testTracks(tracks); break;
				case 1: referenceTracks(tracks); break;
				case 2: selectionTracks(tracks); break;
			}
		}

	protected:

		// RoI information
		TIDARoiDescriptor* m_roiInfo;

		// Track vectors
		std::vector<TrigInDetAnalysis::Track*> m_testTracks;
		std::vector<TrigInDetAnalysis::Track*> m_referenceTracks;
		std::vector<TrigInDetAnalysis::Track*> m_selectionTracks;

};

#endif // TrigInDetAnalysisUtils_RoI_H

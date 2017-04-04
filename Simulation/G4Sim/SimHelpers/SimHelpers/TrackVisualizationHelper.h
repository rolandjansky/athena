/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackVisualizationHelper_H
#define TrackVisualizationHelper_H

class TrackVisualizationHelper {
public:
	TrackVisualizationHelper():
		trackVisualizationScheme(0),visualizeTracks(false) {}
	virtual ~TrackVisualizationHelper() {;} // Null op
	virtual bool VisualizeTracks() {return visualizeTracks;}
	virtual int  TrackVisualizationScheme() {return trackVisualizationScheme;}
	virtual void SetTrackVisualizationScheme(int i)
	{
		trackVisualizationScheme=i;
	}
	virtual void SetVisualizeTracks(bool t) {visualizeTracks=t;}
private:
	int trackVisualizationScheme;
	bool visualizeTracks;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackVisualizationHelper_H
#define TrackVisualizationHelper_H

class TrackVisualizationHelper {
public:
	TrackVisualizationHelper():
		m_trackVisualizationScheme(0),m_visualizeTracks(false) {}
	virtual ~TrackVisualizationHelper() {;} // Null op
	virtual bool VisualizeTracks() {return m_visualizeTracks;}
	virtual int  TrackVisualizationScheme() {return m_trackVisualizationScheme;}
	virtual void SetTrackVisualizationScheme(int i)
	{
		m_trackVisualizationScheme=i;
	}
	virtual void SetVisualizeTracks(bool t) {m_visualizeTracks=t;}
private:
	int m_trackVisualizationScheme;
	bool m_visualizeTracks;
};

#endif

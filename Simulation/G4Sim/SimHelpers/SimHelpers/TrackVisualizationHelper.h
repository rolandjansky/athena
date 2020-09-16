/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackVisualizationHelper_H
#define TrackVisualizationHelper_H

class TrackVisualizationHelper {
public:
  TrackVisualizationHelper():
    m_trackVisualizationScheme(0),m_visualizeTracks(false) {}
  virtual ~TrackVisualizationHelper() {;} // Null op
  virtual bool visualizeTracks() const {return m_visualizeTracks;}
  virtual int  trackVisualizationScheme() const {return m_trackVisualizationScheme;}
  virtual void setTrackVisualizationScheme(int i)
  {
    m_trackVisualizationScheme=i;
  }
  virtual void setVisualizeTracks(bool t) {m_visualizeTracks=t;}
private:
  int m_trackVisualizationScheme;
  bool m_visualizeTracks;
};

#endif

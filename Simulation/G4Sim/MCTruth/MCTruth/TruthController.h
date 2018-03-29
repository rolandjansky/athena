/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthController_H
#define TruthController_H

class TrackVisualizationHelper;

class TruthController {
public:
  static TruthController *getTruthController();
  void setVisualizationHelper(TrackVisualizationHelper *);
  TrackVisualizationHelper* getVisualizationHelper() const
  {
    return m_theVisHelper;
  }
private:
  // this is a singleton
  static TruthController *s_thePointer;
  TruthController();
  TruthController(const TruthController&) {}
  TruthController& operator= (const TruthController&);
  ~TruthController();
  TrackVisualizationHelper *m_theVisHelper;
};

#endif

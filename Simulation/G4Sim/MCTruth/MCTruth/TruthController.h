/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthController_H
#define TruthController_H

class TrackVisualizationHelper;

class TruthController {
public:
  static const TruthController *getTruthController();
  void setVisualizationHelper(TrackVisualizationHelper *);
  const TrackVisualizationHelper* getVisualizationHelper() const
  {
    return m_theVisHelper;
  }
private:
  // this is a singleton
  TruthController();
  TruthController(const TruthController&) = delete;
  TruthController& operator= (const TruthController&) = delete;
  ~TruthController();
  TrackVisualizationHelper *m_theVisHelper;
};

#endif

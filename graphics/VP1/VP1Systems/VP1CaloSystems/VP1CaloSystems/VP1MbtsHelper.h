/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1MBTSHELPER_H
#define VP1MBTSHELPER_H

#include "VP1Base/VP1Interval.h"
#include <vector>
#include <string>
#include <QObject>

class StoreGateSvc;
class SoSeparator;
class SoNode;
class VP1CaloCellController;

class VP1MbtsHelper : public QObject
{
  Q_OBJECT

 public:
  VP1MbtsHelper(bool outline);
  ~VP1MbtsHelper();

  void systemcreate(StoreGateSvc* detstore);
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator* root);
  void systemerase();

  void refreshGraph(const VP1Interval& interval);

  void setController( VP1CaloCellController* );

  // Selection feedback
  std::vector<std::string> userPickedNode(SoNode* pickedNode);

  public slots:
    void selectionUpdated(const VP1Interval& interval);
    void outlineUpdate(const bool& outline);
    void clipVolumeRadiusChanged(double radius);

 private:
  class Clockwork;
  Clockwork* _clockwork;

  bool m_outline;
  double m_clipRadius;
  bool m_run2Geo;     // RUN2 geometry: the readout granularity of MBTS2 changes from 8 to 4
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSYSTEMDISPLAY_H
#define TRACKSYSTEMDISPLAY_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : TrackSystemDisplay
 *
 * @brief   : Temp. Version of Track Display (Header; for testing purposes only)
 *
 * @author  : Manuel Proissl    <mproissl@cern.ch> - University of Edinburgh
 * @author  : Thomas Kittelmann <thomas.kittelmann@cern.ch>
 ***********************************************************************************/

//Includes
#include "VP1Base/IVP13DSystemSimple.h"
#include <iostream>
#include <string>
#include <vector>
#include <QList>
#include <stdio.h>
#include <map>

//Classes
class SoSwitch;
namespace Trk { class Track; }
class QTextBrowser;

class TrackSystemDisplay : public IVP13DSystemSimple
{
  Q_OBJECT

public:
  TrackSystemDisplay();
  virtual ~TrackSystemDisplay(){};
  void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root);
  void userPickedNode(SoNode* pickedNode, SoPath * pickedPath);
  void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);
  QWidget * buildController();
  void printTrackInfo(QString title, QList<QString> paraname, QList<QString> paravalue);

private slots:
  void ptCutChanged(double);
  void updateSelectionMode(bool single);
/*
protected:
  class Imp;
  Imp * d;
*/
private:
  std::map<SoNode*,Trk::Track *> m_nodeToTrack;
  std::map<SoSwitch*,double> m_switchToPt;
  
  double m_ptcut;
  SoCooperativeSelection * m_multiselection;
  
  QTextBrowser * m_trackInfoDisplay;
};
#endif

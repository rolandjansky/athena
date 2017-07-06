/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKTYPES_H
#define TRACKTYPES_H

#include <QString>

class TrackType {
public:
  //NB: The order listed here is the order presented in the GUI (see TrackCollWidget::sortSections(..))
  enum Type {
    Unknown, TrkTrack, TrackParticle, FatrasTruthTrack, RefittedTrack, TrkSegment, LV2Track, TruthTrack, SimulationTrack, AnalysisObject, xAOD
  };

  static qint32 typeToInt(const Type&);
  static Type intToType(const qint32&);

  static QString typeToString(const Type&);
  static Type stringToType(const QString&str, bool & status);

  //For getting a pretty more human-readable string for the section headers:
  static QString typeToSectionHeader(const Type&);
  static Type sectionHeaderToType(const QString&, bool & status);
  static QString typeToSectionHeaderToolTip(const Type&);

private:
  TrackType();
};


#endif

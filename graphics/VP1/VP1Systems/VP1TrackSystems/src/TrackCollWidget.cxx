/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollWidget                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollWidget.h"
#include "VP1TrackSystems/TrackTypes.h"

//____________________________________________________________________
class TrackCollWidget::Imp {
public:
  static TrackCollWidget * theclass;
  static bool sectionSorting(const QString &s1, const QString &s2)
  {
    //We sort by the order of the types in the enum.
    bool ok1, ok2;
    TrackType::Type type1 = TrackType::sectionHeaderToType(s1, ok1);
    TrackType::Type type2 = TrackType::sectionHeaderToType(s2, ok2);
    if (!ok1&&theclass)
      theclass->message("Could not decode section header to type: '"+s1+"'");
    if (!ok2&&theclass)
      theclass->message("Could not decode section header to type: '"+s2+"'");
    return static_cast<int>(type1) < static_cast<int>(type2);
  }
};

TrackCollWidget * TrackCollWidget::Imp::theclass = nullptr;

//____________________________________________________________________
void TrackCollWidget::sortSections(QList<QString>& sections)
{
  Imp::theclass = this;//I guess this is not really threadsafe...
  std::sort(sections.begin(),sections.end(),Imp::sectionSorting);
  Imp::theclass = nullptr;
}

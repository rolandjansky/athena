/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AODCollWidget                     //
//                                                            //
//  Author: edward.moyse@cern.ch                              //
//  Initial version: June 2015                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "AODCollWidget.h"

#include "VP1Base/VP1Msg.h"

//____________________________________________________________________
class AODCollWidget::Imp {
public:
  static AODCollWidget * theclass;
  static bool sectionSorting(const QString &s1, const QString &s2)
  {
      VP1Msg::messageDebug("AODCollWidget::Imp::sectionSorting() - s1: " + s1 + ", s2: " + s2);
	  return true; //FIXME! Don't really care about this for the moment.
  }
};

AODCollWidget * AODCollWidget::Imp::theclass = 0;

//____________________________________________________________________
void AODCollWidget::sortSections(QList<QString>& sections)
{
  Imp::theclass = this;//I guess this is not really threadsafe...
  std::sort(sections.begin(),sections.end(),Imp::sectionSorting);
  Imp::theclass = 0;
}

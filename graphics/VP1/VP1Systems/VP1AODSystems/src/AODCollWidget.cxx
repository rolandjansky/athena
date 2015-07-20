/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//____________________________________________________________________
class AODCollWidget::Imp {
public:
  static AODCollWidget * theclass;
  static bool sectionSorting(const QString &s1, const QString &s2)
  {
	  return true; //FIXME! Don't really care about this for the moment.
  }
};

AODCollWidget * AODCollWidget::Imp::theclass = 0;

//____________________________________________________________________
void AODCollWidget::sortSections(QList<QString>& sections)
{
  Imp::theclass = this;//I guess this is not really threadsafe...
  qSort(sections.begin(),sections.end(),Imp::sectionSorting);
  Imp::theclass = 0;
}

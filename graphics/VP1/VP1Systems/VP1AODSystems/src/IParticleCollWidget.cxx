/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollWidget                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "IParticleCollWidget.h"

//____________________________________________________________________
class IParticleCollWidget::Imp {
public:
  static IParticleCollWidget * theclass;
  static bool sectionSorting(const QString &s1, const QString &s2)
  {
	  return true; //FIXME! Don't really care about this for the moment.
  }
};

IParticleCollWidget * IParticleCollWidget::Imp::theclass = 0;

//____________________________________________________________________
void IParticleCollWidget::sortSections(QList<QString>& sections)
{
  Imp::theclass = this;//I guess this is not really threadsafe...
  qSort(sections.begin(),sections.end(),Imp::sectionSorting);
  Imp::theclass = 0;
}

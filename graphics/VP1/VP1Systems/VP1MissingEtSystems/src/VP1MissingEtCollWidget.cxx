/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1MissingEtSystems/VP1MissingEtCollWidget.h"

VP1MissingEtCollWidget::VP1MissingEtCollWidget(QWidget * parent)
:VP1CollectionWidget(parent){

}

VP1MissingEtCollWidget::~VP1MissingEtCollWidget(){

}


void VP1MissingEtCollWidget::sortSections(QList<QString>& sections){
 //Order: standard -> truth -> ... -> calo
 ensureFirst("*truth*",sections);
 ensureFirst("*standard*",sections);
 ensureLast("*calo*",sections);
}

void VP1MissingEtCollWidget::setShape(bool /*shape*/){

}


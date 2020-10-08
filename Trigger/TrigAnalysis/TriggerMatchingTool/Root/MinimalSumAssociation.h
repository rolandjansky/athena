// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMATCHINGTOOL_MINIMALSUMASSOCIATION_H
#define TRIGGERMATCHINGTOOL_MINIMALSUMASSOCIATION_H

#include "TriggerMatchingTool/IAssociationStrategy.h"
#include "AsgMessaging/AsgMessaging.h"


class MinimalSumAssociation : public IAssociationStrategy, public asg::AsgMessaging {
public:
  MinimalSumAssociation();
  index_assignment_t associate(const std::vector<std::vector<double> >& matrix);
};

#endif

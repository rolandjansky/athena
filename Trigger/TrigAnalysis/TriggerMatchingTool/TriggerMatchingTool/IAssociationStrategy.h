// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMATCHINGTOOL_IASSOCIATIONSTRATEGY_H
#define TRIGGERMATCHINGTOOL_IASSOCIATIONSTRATEGY_H

#include <map>
#include <vector>

class IAssociationStrategy {
public:
  virtual ~IAssociationStrategy(){}
  typedef std::map<unsigned int,unsigned int> index_assignment_t;
  virtual index_assignment_t associate(const std::vector<std::vector<double> >& matrix) = 0;
};

#endif

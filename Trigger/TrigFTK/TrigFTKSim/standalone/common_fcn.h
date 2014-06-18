/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMON_FCN_H
#define COMMON_FCN_H

#include <string>
#include <istream>

void common_fcn_init(int,char *[]);

std::istream& replace_getline(std::istream&, std::string&);

void replace_ids_with_tag(std::string&, const std::string&, std::string &tag, int);

void replace_ids(std::string&, const std::string&, int);

#endif // COMMON_FCN_H

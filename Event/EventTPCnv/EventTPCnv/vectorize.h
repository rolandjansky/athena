/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VECTORIZE_H
#define VECTORIZE_H
#include <string>
#include <vector>

void strToUI( const std::string &m, std::vector<unsigned int> &v);
void UITostr(std::string &n, std::vector<unsigned int>::const_iterator &i, bool bugcompat);
void bitmapToUI( const std::vector<bool> &m, std::vector<unsigned int> &v);
void UITobitmap(std::vector<bool> &n, std::vector<unsigned int>::const_iterator &i,
                bool bugcompat);

#endif

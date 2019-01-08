/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// Class to create a string of elements, n perline
// where n is settable.

#ifndef JETREC_LINEFORMATTER_H
#define JETREC_LINEFORMATTER_H

#include <sstream>
#include <ios>
#include <vector>

class LineFormatter {
public:
  LineFormatter(std::size_t perline):m_perline(perline){
  }
  template<typename T>
  std::string operator()(const std::vector<T>& v){
    std::size_t count{0};
    std::size_t linecount{0};
    std::ostringstream oss{"", std::ios::ate};
    oss << linecount << '|' << " ";
    for (const auto& e : v){
      if (count == m_perline){
        count = 0;
        linecount += m_perline;
        oss << '\n'<<linecount<< '|' << " ";
      }
      oss << e << " ";
      ++count;
    }
    // if(count != 0){oss << '\n';}
    return oss.str();
  }

private:
  std::size_t m_perline;
};

#endif

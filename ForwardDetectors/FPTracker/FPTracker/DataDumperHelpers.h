/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_DATADUMPERHELPERS_H
#define FPTRACKER_DATADUMPERHELPERS_H
#include <sstream>
#include <cstddef>
#include <algorithm>

namespace FPTracker{
  class LineFormater{
  public:
    LineFormater(int numPerLine): m_numPerLine(0), m_maxPerLine(numPerLine), m_ost(new std::ostringstream()){}
    template<class T>
    void operator()(T t){
      if (m_numPerLine == m_maxPerLine){
	m_numPerLine = 0;
	(*m_ost)<<'\n';
      }
      (*m_ost)<<t<<" ";
      ++m_numPerLine;
    }
    std::string toString() const {return m_ost->str();}
  private:
    int m_numPerLine;
    int m_maxPerLine;
    std::ostringstream* m_ost;
  };
  
  template<class Array1D>
    std::string array1DToString(const Array1D& arr){
    
    std::ostringstream dump;
    LineFormater lf = std::for_each(arr.begin(), arr.end(), LineFormater(10));
    dump<<lf.toString()+'\n';
    return dump.str();
  }


  
  template<class Array2D>
  std::string array2DToString(const Array2D& arr){
    
    std::ostringstream dump;
    for(std::size_t is=0; is<arr.size(); ++is){
      dump << "side "<<is<<'\n';
      
      LineFormater lf = std::for_each(arr[is].begin(), arr[is].end(), LineFormater(10));
      dump<<lf.toString()+'\n';
    }
    return dump.str();
  }

  template<class Array3D>
  std::string array3DToString(const Array3D& arr){
    
    std::ostringstream dump;
    for(std::size_t is=0; is<arr.size(); ++is){
      dump << "side "<<is<<'\n';
      for (std::size_t ig=0; ig<arr[is].size(); ++ig){

	dump << "geom "<<ig<<'\n';
	LineFormater lf = std::for_each(arr[is][ig].begin(), arr[is][ig].end(), LineFormater(10));
	dump<<lf.toString()+'\n';

      }
    }
    return dump.str();
  }
}

#endif

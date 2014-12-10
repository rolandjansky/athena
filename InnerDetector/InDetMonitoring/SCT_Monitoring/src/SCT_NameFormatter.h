/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_NameFormatter.h
 * Contains string formatting utility functions for use in SCT_Monitoring
 * @ author shaun roe
**/
#ifndef SCT_NameFormatter_H
#define SCT_NameFormatter_H

#include <string>
#include "boost/lexical_cast.hpp"

namespace SCT_Monitoring{
  ///format an element index (e.g. in looping through barrels, this goes from 0->(2*(nbarrels)) - 1 into layer and side
  ///for use in both the histogram title and its name
  class LayerSideFormatter{
  private:
    const unsigned int m_element;
    const std::string m_layerStr;
    const std::string m_sideStr;
    unsigned int m_elementforDisk = 1;//30.11.2014

  public:
    LayerSideFormatter(const unsigned int i): m_element(i),m_layerStr(boost::lexical_cast<std::string>(i/2)), m_sideStr(boost::lexical_cast<std::string>(i%2)){
      //nop
    }
  LayerSideFormatter(const unsigned int i,const unsigned int m): m_element(i),m_layerStr(boost::lexical_cast<std::string>(i/2)), 
    m_sideStr(boost::lexical_cast<std::string>(i%2)),m_elementforDisk(m){//30.11.2014
      //nop
    }
    std::string layer() const{
      return m_layerStr;
    }
    std::string side() const{
      return m_sideStr; 
    }
    std::string layerPlus1() const{
      return boost::lexical_cast<std::string>((m_element/2) + 1);
    }
    std::string title() const{
      if(m_elementforDisk==1){//30.11.2014
	return std::string("Layer ")+m_layerStr+std::string(" Side ")+m_sideStr;
      }
      else {
	return std::string("Disk ")+m_layerStr+std::string(" Side ")+m_sideStr;
      }
    }
    std::string name(const std::string& delimiter="_") const{
      return m_layerStr+delimiter+m_sideStr;
    }
  };

}//end of SCT_Monitoring namespace

#endif


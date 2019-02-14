/* emacs: this is -*- c++ -*- */
/**
 **     @file    DrawLabel.h
 **
 **     @author  mark sutton
 **     @date    Sun 11 Nov 2012 01:18:14 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  DRAWLABEL_H
#define  DRAWLABEL_H

#include <iostream>
#include <string>

#include "TLatex.h"



class DrawLabel {

public:

  DrawLabel(double x, double y, const std::string& s, int colour=kBlack, double size=0.033 ) : 
    m_text(s), m_colour(colour), m_x(x), m_y(y), m_size(size)
  {
    //    std::cout << "DrawLabel::DrawLabel() " << m_text << std::endl;
    Draw(); 
  }
  
  virtual ~DrawLabel() { } 

  void Draw() const { 
      TLatex* tt = new TLatex();
      tt->SetNDC();
      tt->SetTextColor(m_colour);
      tt->SetTextSize(m_size);
      tt->DrawLatex(m_x, m_y, m_text.c_str() );
  }

  std::string& text()       { return m_text; } 
  std::string  text() const { return m_text; } 

private:

  std::string m_text;
  int         m_colour;

  double      m_x;
  double      m_y;

  double      m_size;

};

inline std::ostream& operator<<( std::ostream& s, const DrawLabel& _d ) { 
  return s << _d.text();
}


#endif  // DRAWLABEL_H 











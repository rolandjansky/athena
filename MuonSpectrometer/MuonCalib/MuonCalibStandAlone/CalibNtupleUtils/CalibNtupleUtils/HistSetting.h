/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HISTSETTING_H
#define HISTSETTING_H

class TAxis;
class TH1;

#include <string>
/**
@ class AxisSetting
Class customizing TAxis settings such as labelsize/offset, titlesize/offset.
@author Niels.Van.Eldik@cern.ch
*/
class AxisSetting {
 public:
  AxisSetting( std::string title = "" );                             //!< constructor (not applying any title)
 
  void setLabel( double labelSize = -1., double labelOffset = -1. ); //!< sets label size/offset
  void setTitle( double titleSize = -1., double titleOffset = -1. ); //!< sets title size/offset
  void applySetting( TAxis* axis ) const;                            //!< applies the settings to axis

 private:
  std::string m_title;    //!< axis title
  double m_labelOffset;   //!< label offset
  double m_labelSize;     //!< label size
  double m_titleOffset;   //!< title offset
  double m_titleSize;     //!< title size
};


/**
   @class HistSetting
   Helper class in order to customize HistHandler with your favorite LabelSize/Offset, TitleSize/Offset etc.
   @author Niels.Van.Eldik
 */
class HistSetting {
 public:
  HistSetting(std::string xtitle = "", bool doStat = false, const AxisSetting* xsettings=0, const AxisSetting* ysetting=0  ); //!< constructor
  void applySetting( TH1* h ) const; //!< Applies the setting to histogram h (title, stats, axissettings)

 private:
  std::string m_title;            //!< title of the histogram
  bool m_doStat;                  //!< flag switching on/off the statistics panel of the histogram
  const AxisSetting* m_xsetting;  //!< settings of the x Axis
  const AxisSetting* m_ysetting;  //!< settings of the y Axis
};



#endif

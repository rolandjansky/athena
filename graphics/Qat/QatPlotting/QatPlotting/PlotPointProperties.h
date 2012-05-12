#ifndef _PLOTPOINTPROPERTIES_H_
#define _PLOTPOINTPROPERTIES_H_
#include <QtGui/QPen>
#include <QtGui/QBrush>

class PlotPointProperties {

 public:


  QPen   pen;
  QBrush brush;
   enum SymbolStyle {CIRCLE, 
		    SQUARE,
		    TRIANGLE_U, 
		    TRIANGLE_L}                     symbolStyle;

  int                                               symbolSize;

  // Construct Default:
  PlotPointProperties():
    symbolStyle(CIRCLE),
    symbolSize(5){}

};

#endif

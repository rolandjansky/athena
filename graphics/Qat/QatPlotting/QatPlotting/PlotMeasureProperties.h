#ifndef _PLOTMEASUREPROPERTIES_H_
#define _PLOTMEASUREPROPERTIES_H_
#include <QtGui/QPen>
#include <QtGui/QBrush>

class PlotMeasureProperties {

 public:


  QPen                                              pen;
  QBrush                                            brush;
  enum SymbolStyle {CIRCLE, 
		    SQUARE,
		    TRIANGLE_U, 
		    TRIANGLE_L}                     symbolStyle;

  int                                               symbolSize;

  // Construct Default:
  PlotMeasureProperties(): 
    symbolStyle(CIRCLE),
    symbolSize(5){
  }

};

#endif

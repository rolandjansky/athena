#ifndef _PLOTRESIDUAL1DPROPERTIES_H_
#define _PLOTRESIDUAL1DPROPERTIES_H_
#include <QtGui/QPen>

class PlotResidual1DProperties {

 public:


  QPen                                              pen;

  enum SymbolStyle {CIRCLE, 
		    SQUARE,
		    TRIANGLE_U, 
		    TRIANGLE_L}                     symbolStyle;

  int                                               symbolSize;

  // Construct Default:
  PlotResidual1DProperties(): 
    symbolStyle(CIRCLE),
    symbolSize(5) 
      {}

};

#endif

#ifndef _PLOTHIST1DPROPERTIES_H_
#define _PLOTHIST1DPROPERTIES_H_
#include <QtGui/QPen>
#include <QtGui/QBrush>

class PlotHist1DProperties {

 public:


  QPen                                              pen;
  QBrush                                            brush;

  enum PlotStyle {LINES, SYMBOLS}                   plotStyle;


  enum SymbolStyle {CIRCLE, 
		    SQUARE,
		    TRIANGLE_U, 
		    TRIANGLE_L}                     symbolStyle;

  int                                               symbolSize;

  // Construct Default:
  PlotHist1DProperties(): 
    plotStyle(LINES),
    symbolStyle(CIRCLE),
    symbolSize(5) 
      {}

};

#endif

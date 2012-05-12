#ifndef _PLOTPROFILEPROPERTIES_H_
#define _PLOTPROFILEPROPERTIES_H_
#include <QtGui/QPen>
#include <QtGui/QBrush>

class PlotProfileProperties {

 public:


  QPen                                              pen;
  QBrush                                            brush;
  enum SymbolStyle {CIRCLE, 
		    SQUARE,
		    TRIANGLE_U, 
		    TRIANGLE_L}                     symbolStyle;

  int                                               symbolSize;
  int                                               errorBarSize;
  bool                                              drawSymbol;

  // Construct Default:
  PlotProfileProperties(): 
    symbolStyle(CIRCLE),
    symbolSize(5),
    errorBarSize(symbolSize),
    drawSymbol(true){
  }

};

#endif

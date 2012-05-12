#ifndef _PLOTFUNCTION1DPROPERTIES_H_
#define _PLOTFUNCTION1DPROPERTIES_H_
#include <QtGui/QPen>
#include <QtGui/QBrush>
class PlotFunction1DProperties {

 public:

  QPen   pen;
  QBrush brush;
  double baseLine;


  // Construct Default:
  PlotFunction1DProperties(): 
    baseLine(0.0){}

};

#endif

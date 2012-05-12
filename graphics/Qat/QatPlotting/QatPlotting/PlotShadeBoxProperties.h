#ifndef _PLOTSHADEBOXPROPERTIES_H_
#define _PLOTSHADEBOXPROPERTIES_H_
#include <QtGui/QPen>
#include <QtGui/QBrush>

class PlotShadeBoxProperties {

 public:


  QPen                                              pen;
  QBrush                                            brush;
  double                                            horizontalSize;

  // Construct Default:
  PlotShadeBoxProperties(): 
    horizontalSize(1.0)
    {
    }

};

#endif

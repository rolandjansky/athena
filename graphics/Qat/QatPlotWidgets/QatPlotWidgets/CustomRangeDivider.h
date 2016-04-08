#ifndef _CUSTOMRANGEDIVIDER_H_
#include "QatPlotWidgets/AbsRangeDivider.h"
#include "QatPlotWidgets/RangeDivision.h"
#include <vector>
class CustomRangeDivider : public AbsRangeDivider {

 public:
  // Constructor:
  CustomRangeDivider();

  // Destructor:
  ~CustomRangeDivider();

  // Insert a subdivision.  The label is cloned & the clone managed.
  void add (double x, QTextDocument *label=NULL);

  // Get the number of subdivision:
  virtual int getNumSubdivisions() const;

  // Get the location of each subdivision:
  virtual const RangeDivision & getSubdivision ( int i) const;

  // Three methods to set the range.  The calling the first method once
  // is more efficient than calling the last methods twice!
  virtual void setRange(double min, double max);
  virtual void setMin(double min);
  virtual void setMax(double max);

 // Get the validity of each subdivision:
  virtual bool isValid(int i) const;

 private:

  double _min;
  double _max;
  std::vector<RangeDivision> _subdivision;

};
#endif

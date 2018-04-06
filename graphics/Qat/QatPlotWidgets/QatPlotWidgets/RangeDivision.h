#ifndef QATPLOTWIDGETS_RANGEDIVISION_H
#define QATPLOTWIDGETS_RANGEDIVISION_H
#include <QtGui/QTextDocument>

class RangeDivision {
  
 public:

  // Constructor:
  RangeDivision(double value);

  // Copy Constructor:
  RangeDivision(const RangeDivision & right);

  // Assignment operator:
  RangeDivision & operator=(const RangeDivision & right);

  // Destructor;
  ~RangeDivision();


  // Value Accessors:
  const double & x() const;
  
  // Text Accessor.
  QTextDocument *label() const;

  // Set Document (deep copy):
  void setLabel(QTextDocument *doc);

 private:

  double m_x;
  QTextDocument *m_label;
 

};
#endif

#pragma once

#include <iostream>
#include "SGTools/CLASS_DEF.h"


class ShiftCondObj {
 
public: 
 
  ShiftCondObj():m_val(0) {};
  ShiftCondObj(int i): m_val(i) {};
  virtual ~ShiftCondObj(){};

  void val(double i) { m_val = i; }
  double val() const { return m_val; }

private:
  double m_val;
};

inline std::ostream& operator<<(std::ostream& ost, const ShiftCondObj& rhs) {
  ost << rhs.val(); 
  return ost;
}


//using the macros below we can assign an identifier (and a version) 
//to the type ShiftConfObj
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( ShiftCondObj , 174332583 , 1 )
CLASS_DEF( CondCont<ShiftCondObj> , 80691078 , 1 )


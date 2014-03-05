// Check.cxx

#include "AsgTools/Check.h"

#include <iostream>

using std::cout;
using std::endl;

namespace {

bool show(bool aval =true, bool set =false) {
  static bool val = false;
  if ( set ) val = aval;
  return val;
}

}  // end unnamed namespace

//**********************************************************************

void asg::Check::passed() {
  if ( ! showpass() ) return;
  cout << "***************** ASG check passed ********************" << endl;
}

//**********************************************************************

void asg::Check::failed() {
  cout << "***************** ASG check failed ********************" << endl;
}

//**********************************************************************

void asg::Check::setshowpass(bool val) {
  show(val, true);
}

//**********************************************************************

bool asg::Check::showpass() {
  return show();
}

//**********************************************************************

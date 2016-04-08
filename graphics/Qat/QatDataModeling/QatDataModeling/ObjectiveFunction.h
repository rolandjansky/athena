#ifndef ObjectiveFunction_h_
#define ObjectiveFunction_h_
class ObjectiveFunction {

  // An objective (ie  function to be minimized):

 public:


  // Its constructor
  ObjectiveFunction();

  // Its destructor
  virtual ~ObjectiveFunction();

  // Its function call operator:
  virtual double operator() () const=0;

};
#endif


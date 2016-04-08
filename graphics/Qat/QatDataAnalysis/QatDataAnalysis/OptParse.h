#ifndef _OPTPARSE_H_
#define _OPTPARSE_H_
class HistogramManager;
#include <string>
#include <vector>


// 
struct HIOZeroToOne {

  //---------------Simple Struct, few public variables--------------//
  HistogramManager *output;                                         //
  bool              verbose;                                        //
  void optParse(int & argc, char ** & argv);                        //
  //---------------End importatant part-----------------------------//

  HIOZeroToOne();
  ~HIOZeroToOne();


  private:

  HIOZeroToOne & operator =( const HIOZeroToOne &);
  HIOZeroToOne(const HIOZeroToOne &);
  class Clockwork;
  Clockwork *c;

};

// 
struct HIOOneToOne {

  //---------------Simple Struct, few public variables--------------//
  const HistogramManager *input;                                    //
  HistogramManager *output;                                         //
  bool              verbose;                                        //
  void optParse(int & argc, char ** & argv);                        //
  //---------------End importatant part-----------------------------//

  HIOOneToOne();
  ~HIOOneToOne();


  private:

  HIOOneToOne & operator =( const HIOOneToOne &);
  HIOOneToOne(const HIOOneToOne &);
  class Clockwork;
  Clockwork *c;

};

// 
struct HIOOneToZero {

  //---------------Simple Struct, few public variables--------------//
  const HistogramManager *input;                                    //
  bool              verbose;                                        //
  void optParse(int & argc, char ** & argv);                        //
  //---------------End importatant part-----------------------------//

  HIOOneToZero();
  ~HIOOneToZero();


  private:

  HIOOneToZero & operator =( const HIOOneToZero &);
  HIOOneToZero(const HIOOneToZero &);
  class Clockwork;
  Clockwork *c;

};

// 
struct HIONToOne {

  //---------------Simple Struct, few public variables--------------//
  std::vector<const HistogramManager *> input;                      //
  HistogramManager *output;                                         //
  bool              verbose;                                        //
  void optParse(int & argc, char ** & argv);                        //
  //---------------End importatant part-----------------------------//

  HIONToOne();
  ~HIONToOne();


  private:

  HIONToOne & operator =( const HIONToOne &);
  HIONToOne(const HIONToOne &);

  class Clockwork;
  Clockwork *c;

};
// 
struct HIONToZero {

  //---------------Simple Struct, few public variables--------------//
  std::vector<const HistogramManager *> input;                      //
  bool              verbose;                                        //
  void optParse(int & argc, char ** & argv);                        //
  //---------------End importatant part-----------------------------//

  HIONToZero();
  ~HIONToZero();


  private:

  HIONToZero & operator =( const HIONToZero &);
  HIONToZero(const HIONToZero &);
  class Clockwork;
  Clockwork *c;

};


class NumericInput {
  
 public:

  NumericInput();
  ~NumericInput();

  // Declare a parameter (name, doc, value)
  void declare(const std::string & name, const std::string & doc, double val);

  // Then override with user input:
  void optParse(int & argc, char ** & argv);

  // Print the list of parameters:
  std::string usage() const;

  // Get the value of a parameter, by name:
  double getByName  (const std::string & name) const;

 private:
 
  NumericInput & operator =( const NumericInput &);
  NumericInput(const NumericInput &);

  class Clockwork;
  Clockwork *c;

};



#endif

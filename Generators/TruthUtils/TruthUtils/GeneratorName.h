/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHUTILS_GENERATORNAME_H
#define TRUTHUTILS_GENERATORNAME_H

#include <iosfwd>
#include <string>

/// @file GeneratorName.h
/// @brief Enum and tools for decoding generator information from the process code -- yuck!
/// @todo This thing is a mess that completely breaks down with LHE files and new gens. REMOVE!


/// @note Sherpa cannot be used with capital letters as it interferes with the namespace name from external Sherpa package.
/// @note New generators need to be added in genames.inc too EML 12.12.2008
/// @note PYTHIA instead of a special number 1010000000 is now 110000000 (the special number may be used when we run out of other numbers) EML 19.01.2009
enum GeneratorName {
  /*
  PYTHIA          = 1000000,
  HERWIG          = 2000000,
  ISAJET          = 3000000,
  SINGLE          = 4000000,
  HIJING          = 5000000,
  PHOJET          = 6000000,
  Sherpa          = 7000000,
  CASCADE         = 8000000,
  JIMMY           = 9000000,
  HYDJET          = 10000000,
  */
  // first generators - new scheme
  PYTHIA          = 110000000,
  HERWIG          = 20000000,
  ISAJET          = 30000000,
  SINGLE          = 40000000,
  HIJING          = 50000000,
  PHOJET          = 60000000,
  Sherpa          = 70000000,
  CASCADE         = 80000000,
  JIMMY           = 90000000,
  HYDJET          = 100000000,
  // note 110000000 used for PYTHIA
  EPOS            = 120000000,

  // second generators
  COMPHEP         = 100000,
  //  CHARYBDIS   = 150000,
  USER            = 200000,
  ACERMC          = 300000,
  ALPGEN          = 400000,
  MADGRAPH        = 500000,
  MADCUP          = 600000,
  TOPREX          = 700000,
  LHAEXT          = 800000,
  MCATNLO         = 900000,
  // new second generators - new scheme
  CHARYBDIS       = 1001000000,
  HORACE          = 1001100000,
  LHEF            = 1001200000,
  MATCHIG         = 1001300000,
  HVGEN           = 1001400000,
  EXOGRAVITON     = 1001500000,
  PYTHIABC        = 1001600000,
  PROTOS          = 1001700000,
  GRAVADD         = 1001800000,
  HELAC           = 1001900000,
  PYTHIASGLUON    = 1002000000,

  // third generators
  TAUOLA          = 10000,
  PHOTOS          = 20000,
  TAUOLA_PHOTOS   = 30000,
  UNKNOWN         = -999
};


GeneratorName  generator_name(const int& index);
GeneratorName  first_generator(const int& index);
GeneratorName  second_generator(const int& index);
GeneratorName  third_generator(const int& index);

int            generator_process(int& index);

void           GeneratorName_print(int& id);
std::string    generator_string(const int& index);
int            generator_int(std::string& name);

std::ostream &operator<< (std::ostream& lhs, GeneratorName rhs);
bool isNewGenCoding(const int& index);


#endif // GENERATORMODULES_GENERATORNAME_H

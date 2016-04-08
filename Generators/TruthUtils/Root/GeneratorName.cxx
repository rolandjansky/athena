/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthUtils/GeneratorName.h"
#include <iostream>
#include <algorithm>

/// @todo This scheme doesn't work anymore...

GeneratorName generator_name (const int& index) {
  switch (index) {
    //new scheme for first generators
  case  110000000 : return PYTHIA;
  case   20000000 : return HERWIG;
  case   30000000 : return ISAJET;
  case   40000000 : return SINGLE;
  case   50000000 : return HIJING;
  case   60000000 : return PHOJET;
  case   70000000 : return Sherpa;
  case   80000000 : return CASCADE;
  case   90000000 : return JIMMY;
  case  100000000 : return HYDJET;
    // note 110000000 used for PYTHIA
  case  120000000 : return EPOS;
  case 100000   : return COMPHEP;
    //  case 150000   : return CHARYBDIS;
  case 200000   : return USER;
  case 300000   : return ACERMC;
  case 400000   : return ALPGEN;
  case 500000   : return MADGRAPH;
  case 600000   : return MADCUP;
  case 700000   : return TOPREX;
  case 800000   : return LHAEXT;
  case 900000   : return MCATNLO;
    // new second generators in a new scheme
  case 1001000000 : return CHARYBDIS;
  case 1001100000 : return HORACE;
  case 1001200000 : return LHEF;
  case 1001300000 : return MATCHIG;
  case 1001400000 : return HVGEN;
  case 1001500000 : return EXOGRAVITON;
  case 1001600000 : return PYTHIABC;
  case 1001700000 : return PROTOS;
  case 1001800000 : return GRAVADD;
  case 1001900000 : return HELAC;
  case 1002000000 : return PYTHIASGLUON;
    // third generators
  case 10000    : return TAUOLA;
  case 20000    : return PHOTOS;
  case 30000    : return TAUOLA_PHOTOS;
  }
  return UNKNOWN;
}


GeneratorName first_generator(const int& index) {
  int gen1;
  if (isNewGenCoding(index)) {
    gen1 = 10000000*(index/10000000);
    if (gen1 >= 1000000000) gen1=gen1-1000000000;
  } else{
    // Extract the first generator int representation (/1000000 *1000000)
    // and convert to a new scheme (*10)
    gen1 = 10000000*(index/1000000);
    //special treatment for PYTHIA, as the coding is not standard
    if (gen1 == 10000000) gen1=110000000;
  }
  return generator_name(gen1);
}

GeneratorName second_generator(const int& index) {
  int gen1,gen2;
  if (isNewGenCoding(index)){
    gen1 = first_generator(index);
    gen2 = index - gen1;
    gen2 = 100000*(gen2/100000);
    //  if (gen2==100000 && (gen1==2000000 || gen1==9000000)) gen2 += 50000;
  } else{
    gen1 = 1000000*(index/1000000);
    gen2 = index - gen1;
    gen2 = 100000*(gen2/100000);
  }
  return generator_name(gen2);
}


GeneratorName third_generator(const int& index) {
  int gen1,gen2,gen3;
  if (isNewGenCoding(index)){
    gen1 = first_generator(index);
    gen2 = second_generator(index);
    gen3 = index - gen1 - gen2;
    gen3 = 10000*(gen3/10000);
  } else{
    gen1 = 1000000*(index/1000000);
    gen2 = index - gen1;
    gen2 = 100000*(gen2/100000);
    gen3 = index - gen1 - gen2;
    gen3 = 10000*(gen3/10000);
  }
  return generator_name(gen3);
}


int generator_process(int& index) { return index % 10000; }

void GeneratorName_print(int& id) {
  std::cout << first_generator(id) << " "
            << second_generator(id) << " "
            << third_generator(id) << " + "
            << generator_process(id);
}

std::string generator_string(const int& index) {
  switch (index) {
  case 110000000  : return "PYTHIA";
  case 20000000  : return "HERWIG";
  case 30000000  : return "ISAJET";
  case 40000000  : return "SINGLE";
  case 50000000  : return "HIJING";
  case 60000000  : return "PHOJET";
  case 70000000  : return "Sherpa";
  case 80000000  : return "CASCADE";
  case 90000000  : return "JIMMY";
  case 100000000 : return "HYDJET";
  case 120000000 : return "EPOS";
  case 100000   : return "COMPHEP";
    //  case 150000   : return "CHARYBDIS";
  case 200000   : return "USER";
  case 300000   : return "ACERMC";
  case 400000   : return "ALPGEN";
  case 500000   : return "MADGRAPH";
  case 600000   : return "MADCUP";
  case 700000   : return "TOPREX";
  case 800000   : return "LHAEXT";
  case 900000   : return "MCATNLO";
  case 1001000000   : return "CHARYBDIS";
  case 1001100000   : return "HORACE";
  case 1001200000   : return "LHEF";
  case 1001300000   : return "MATCHIG";
  case 1001400000   : return "HVGEN";
  case 1001500000   : return "EXOGRAVITON";
  case 1001600000   : return "PYTHIABC";
  case 1001700000   : return "PROTOS";
  case 1001800000   : return "GRAVADD";
  case 1001900000   : return "HELAC";
  case 1002000000   : return "PYTHIASGLUON";
  case 10000    : return "TAUOLA";
  case 20000    : return "PHOTOS";
  case 30000    : return "TAUOLA_PHOTOS";
  }
  return "";
}

int generator_int (std::string& name) {
  /// @note Sherpa cannot be written with capital letters as it interferes with the namespace name from external Sherpa package.  EML 11.12.2008
  if (name == "sherpa") name = "Sherpa";
  else transform(name.begin(), name.end(), name.begin(), toupper);
  if ( name == "PYTHIA" )        return 110000000;
  if ( name == "HERWIG" )        return 20000000;
  if ( name == "ISAJET" )        return 30000000;
  if ( name == "SINGLE" )        return 40000000;
  if ( name == "HIJING" )        return 50000000;
  if ( name == "PHOJET" )        return 60000000;
  if ( name == "Sherpa" )        return 70000000;
  if ( name == "CASCADE" )       return 80000000;
  if ( name == "JIMMY" )         return 90000000;
  if ( name == "HYDJET" )        return 100000000;
  if ( name == "EPOS" )          return 120000000;
  if ( name == "COMPHEP" )       return 100000;
  //  if ( name == "CHARYBDIS" )     return 150000;
  if ( name == "USER" )          return 200000;
  if ( name == "ACERMC" )        return 300000;
  if ( name == "ALPGEN" )        return 400000;
  if ( name == "MADGRAPH" )      return 500000;
  if ( name == "MADCUP" )        return 600000;
  if ( name == "TOPREX" )        return 700000;
  if ( name == "LHAEXT" )        return 800000;
  if ( name == "MCATNLO" )       return 900000;
  if ( name == "CHARYBDIS" )     return 1001000000;
  if (name == "HORACE" )        return 1001100000;
  if (name == "LHEF")           return 1001200000;
  if (name == "MATCHIG")        return 1001300000;
  if (name == "HVGEN")          return 1001400000;
  if (name == "EXOGRAVITON")    return 1001500000;
  if (name == "PYTHIABC")       return 1001600000;
  if (name == "PROTOS")         return 1001700000;
  if (name == "GRAVADD")        return 1001800000;
  if (name == "HELAC")          return 1001900000;
  if (name == "PYTHIASGLUON")   return 1002000000;
  if ( name == "TAUOLA" )        return 10000;
  if ( name == "PHOTOS" )        return 20000;
  if ( name == "TAUOLA_PHOTOS" ) return 30000;
  return -999;
}

std::ostream& operator<<(std::ostream& lhs, GeneratorName rhs) {
  lhs << generator_string(rhs);
  return lhs;
}

bool isNewGenCoding(const int& index) {
  if (index < 20000000) return false;
  else return true;
}

#ifndef ANA_ALGORITHM_LINKDEF_H
#define ANA_ALGORITHM_LINKDEF_H

// Some common definitions:
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

// Declare the class(es) to generate dictionaries for:
#pragma link C++ class EL::AnaAlgorithm+;
#ifdef ROOTCORE
#pragma link C++ class EL::AnaAlgorithmConfig+;
#endif

#endif // ANA_ALGORITHM_LINKDEF_H

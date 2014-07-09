/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  testLogicParser.cpp
  -------------------
  Test decoding of a logic expression.
*/
#include <iostream>
#include <cstdlib>
#include "TrigConfStorage/LogicExpression.h"

using namespace std;

void usage(const std::string& cmd) {
  cout << "Usage: " << cmd << " <logicExpression>" << endl;
  cout << "Symbols: AND                 : " 
       << TrigConf::LogicExpression::kAND << endl;
  cout << "         OR                  : " 
       << TrigConf::LogicExpression::kOR << endl;
  cout << "         NOT                 : " 
       << TrigConf::LogicExpression::kNOT << endl;
  cout << "         Start of a sub-logic: " 
       << TrigConf::LogicExpression::kOPEN << endl;
  cout << "         End of a sub-logic  : " 
       << TrigConf::LogicExpression::kCLOSE << endl;
  cout << "         Logic element name  : " << "[a-zA-Z0-9_]+" << endl;
  cout << "Example: " << cmd << " \"((a&b)|!c)\"" << endl;
  cout << "         " << cmd 
       << " \"(a|b|(c&d&e&f1&\\!((ab|abd|\\!(dddd)))))\"" << endl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    usage(argv[0]);
    exit(0);
  }
  std::string logic = argv[1];
  TrigConf::LogicExpression le(std::cout);
  int n;
  if ( (n = le.parse(logic)) > 0) {
    cout << endl;
    cout << "Print logic" << endl;
    cout << endl;
    cout << "Input expression:          " << logic << endl;
    cout << "Pre-normalized expression: " << le.logicRep() << endl;
    le.normalize();
    cout << "Normalized expression:     " << le.logicRep() << endl;
    cout << endl;
    cout << "Total number of elements : " << le.totalNumberOfElements() <<endl;
    le.print();
  } else {
    cout << "Error while parsing string " << logic << endl;
  }
  return 0;
}


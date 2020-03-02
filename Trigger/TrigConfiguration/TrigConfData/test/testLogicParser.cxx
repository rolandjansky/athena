#include "TrigConfData/LogicParser.h"

#include <vector>
#include <string>

using namespace std;

template< typename T>
std::vector<bool>
testPatterns(const std::vector<std::string> & patterns,
             const std::map<std::string,T> & state)
{
   TrigConf::LogicParser p;
   std::vector<bool> result;

   for( auto & s : patterns) {
      p.parse(s)->print();
      cout << endl;
   }
   cout << "inputs:" << endl << "------" << endl;
   for(auto & entry : state) {
      cout << entry.first << " ==> " << entry.second << endl;
   }

   cout << endl << "outputs:" << endl << "-------" << endl;
   try {
      for(auto & s : patterns) {
         auto logic = p.parse(s);
         bool passed = logic->evaluate(state);
         cout << logic->expression() << "  =>  " << passed << endl;
         result.push_back(passed);
      }
   } catch (TrigConf::LogicParsingException & ex) {
      std::cerr << "Error in evaluating logic. " << ex.msg() << endl;
      throw;
   }
   return result;
}



int main() {

   cout << endl;
   cout << "=============" << endl;
   cout << " Test boolean" << endl;
   cout << "=============" << endl << endl;
   std::map<std::string,bool> state {
      {"EM3", true},
      {"MU6", false},
      {"MU20", true},
      {"TAU6", true},
      {"J100", false},
      {"BGRP0", true},
      {"BGRP1", true}
   };

   bool success = true;

   vector<string> tests;
   tests.emplace_back("EM3&(MU6|MU20)");      // true
   tests.emplace_back("EM3 & !(MU6|MU20)");   // false
   tests.emplace_back("EM3 & (MU6 & MU20)");  // false
   tests.emplace_back("EM3 & (!MU6 & MU20)");  // false
   tests.emplace_back("!EM3 | (MU6 & MU20)"); // true
   tests.emplace_back("!EM3 | MU6 | !MU20"); // false
   tests.emplace_back("((EM3&MU6)|(TAU6&J100))&BGRP0&BGRP1)"); // false

   auto result1 = testPatterns(tests, state);

   success = success && (result1 == std::vector<bool>{1,0,0,1,0,0,0});

   // test 2: give multiplicity
   cout << endl;
   cout << "==================" << endl;
   cout << " Test mulitplicity" << endl;
   cout << "==================" << endl << endl;

   std::map<std::string,unsigned int> multiplicity {
      {"EM3", 2},
      {"XE30", 1},
      {"XE50", 0},
      {"MU20", 1},
      {"RNDM0", 1},
      {"EM18VHI", 1},
      {"TOPO_300INVM9999-AJ30s6-AJ20s6", 1},
   };

   vector<string> multiplicityTests;
   multiplicityTests.emplace_back("MU20[x1]&XE30[x1]");
   multiplicityTests.emplace_back("MU20[x1]&XE50[x1]");
   multiplicityTests.emplace_back("RNDM0");
   multiplicityTests.emplace_back("EM3");
   multiplicityTests.emplace_back("EM3[x1]");
   multiplicityTests.emplace_back("EM3[x2]");
   multiplicityTests.emplace_back("EM3[x3]");
   multiplicityTests.emplace_back("EM3[x1] & TOPO_300INVM9999-AJ30s6-AJ20s6[x1]");
   multiplicityTests.emplace_back("EM18VHI[x1] & RNDM0");


   auto result2 = testPatterns(multiplicityTests, multiplicity);

   success = success && (result2 == std::vector<bool>{1,0,1,1,1,1,0,1,1});

   cout << "Success? " << (success ? "yes" : "no" ) << endl;
   return success ? 0 : 1;
}


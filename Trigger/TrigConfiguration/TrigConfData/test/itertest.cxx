#include "TrigConfData/ConstIter.h"

#include <vector>
#include <iostream>

using namespace std;

class TestCollection {
public:

  TestCollection(const vector<vector<int>> & vi) :
    m_data(vi)
  {}
  ~TestCollection() {}

  using const_iterator = TrigConf::ConstIter< vector<vector<int>>, vector<int> >;

  const_iterator begin() const {
    return {m_data, 0};
  }
  const_iterator end() const {
    return {m_data, m_data.size()};
   }

 private:
   vector<vector<int>> m_data;

 };


bool inner_loop ( int, const std::vector<int> & v,
		  const std::vector<int> & refV
		  )
{
  bool allGood = true;
  unsigned int refIdx(0);
  for(int ii :  v) {
    bool match = (ii == refV[refIdx++]);
    allGood &= match;
    cout << "      inner_loop " << ii << " -> " << (match ? "ok" : "mismatch  ======> FAIL") << endl;
  }
  return allGood;
}

bool test1 ( const TrigConf::ConstIter< vector<vector<int>>, std::vector<int>>& it,
	     const std::vector<int> & refV )
{
   bool allGood = true;
   cout << "In test1 with reference : ";
   for( int i: refV ) cout << i << " ";
   cout << endl;

   unsigned int refIdx(0);
   for ( int i : *it) {
     bool match = i == refV[refIdx++];
      cout << "   outer loop " << i << " -> " << (match ? "ok" : "mismatch  =========> FAIL") << endl;
      allGood &= match;
      allGood &= inner_loop (i, *it, refV);
   }
   return allGood;
}

bool test2 ( const TrigConf::ConstIter< vector<vector<int>>, std::vector<int>>& it,
	     const std::vector<int> & refV )
{
   bool allGood = true;
   cout << "In test2 with reference : ";
   for( int i: refV ) cout << i << " ";
   cout << endl;

   unsigned int size = refV.size();

   unsigned int refIdx(0);
   auto end = (*it).end(); // create end first, so at least begin is pointing to a valid iterator in the following loop
   for ( vector<int>::const_iterator ii = (*it).begin(); ii != end; ++ii) {

      int v = *ii;
      if ( refIdx>=size ) {
         cout << "Not stopping correctly, begin() and end() pointing to different collections.  ===> FAIL" << endl;
         allGood = false;
         break;
      }
      bool match = (v == refV[refIdx++]); 
      cout << "   loop " << v << " -> " << (match ? "ok" : "mismatch  =========> FAIL") << endl;
      allGood &= match;
   }
   return allGood;
}


int main() {

   bool successTest1 ( true );
   bool successTest2 ( true );
   bool successTest3 ( true );
   bool success ( true );

   vector<vector<int>> v = { {1, 2, 3, 4}, 
			     {5, 6}, 
			     {7, 8, 9, 10, 11}};

   // test 1
   auto iter1 = TrigConf::ConstIter< vector<vector<int>>, vector<int> > { v, 0 };
   successTest1 &= test1(  iter1, { 1, 2, 3, 4});
   successTest1 &= test1(++iter1, { 5, 6});
   successTest1 &= test1(++iter1, { 7, 8, 9, 10, 11});

   // test 2
   auto iter2 = TrigConf::ConstIter< vector<vector<int>>, vector<int> > { v, 0 };
   successTest2 = test2( iter2, v[0] );

   // test 3
   cout << "In test3" << endl;
   const TestCollection c(v);
   unsigned int idx(0);
   for( auto x : c ) {
      successTest3 &= inner_loop (0, x, v[idx++]);
   }

   cout << "Test iterator validity duration different loops          ... " << ( successTest1 ? "passed" : "failed" ) << endl;
   cout << "Test iterator validity duration same loop                ... " << ( successTest2 ? "passed" : "failed" ) << endl;
   cout << "Test iterator validity in range-based loop               ... " << ( successTest3 ? "passed" : "failed" ) << endl;

   success &= successTest1;
   success &= successTest2;
   success &= successTest3;
   return success ? 0 : 1;
}

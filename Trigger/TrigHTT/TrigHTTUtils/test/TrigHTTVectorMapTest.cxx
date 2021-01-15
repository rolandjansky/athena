/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TrigHTTVectorMapTest.cxx: Unit tests for HTTVectorMap
 *
 * Author: Riley Xu
 * Email: rixu@cern.ch
 * Creation Date: 2019-06-03
 */



#include <string>
#include <iostream>

#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTUtils/HTTVectorMap.h"

using std::cout;
using std::string;
using std::vector;

void test_iterate()
{
    HTTVectorMap<string,int> map;
    HTT_ASSERT_THROW(map.empty());

    map.assign({"a", "b", "c", "d"},
               {1  , 2  , 3  , 4  });

    HTT_ASSERT_THROW(!map.empty());
    HTT_ASSERT_THROW(map.size() == 4);

    int i = 1;
    for (auto it = map.begin(); it != map.end(); it++)
    {
        HTT_ASSERT_THROW(*it == i++);
    }
    i = 1;
    for (int val : map)
    {
        HTT_ASSERT_THROW(val == i++);
    }
    i = 1;
    for (int j = 0; j < 4; j++)
    {
        HTT_ASSERT_THROW(map[j] == i++);
        map[j] = j;
    }
    i = 0;
    for (int j = 0; j < 4; j++)
    {
        HTT_ASSERT_THROW(map[j] == i++);
    }
}

void test_sort()
{
    HTTVectorMap<string,int> map(
            {"r", "i", "x", "u"},
            {114, 105, 120, 117}
    );

    vector<string> srt_keys({"i", "r", "u", "x"});
    vector<int> srt_vals({105, 114, 117, 120});

    vector<string> const & keys = map.getKeys();
    vector<int> const & vals = map.getData();
    for (int i = 0; i < 4; i++)
    {
        HTT_ASSERT_THROW(vals[i] == srt_vals[i]);
        HTT_ASSERT_THROW(keys[i] == srt_keys[i]);
    }
}


void test_search()
{
    HTTVectorMap<string,int> map(
            {"r", "i", "x", "u"},
            {114, 105, 120, 117}
    );

    map.assign({"a", "b", "c", "d"},
               {1  , 2  , 3  , 4  });

    HTT_ASSERT_THROW(map.find("u") == map.end());
    HTT_ASSERT_THROW(map.find("c") == map.begin() + 2);
    HTT_ASSERT_THROW(*(map.find("c")) == 3);

    map.assign({"a", "b", "b", "b", "c"},
               {1  , 2  , 3  , 4  , 5  });

    HTT_ASSERT_THROW(map.lower_bound("b") == map.begin() + 1);
    HTT_ASSERT_THROW(map.upper_bound("b") == map.begin() + 4);
}

void test_getKeyIndex()
{
    HTTVectorMap<string,int> map(
            {"a", "b", "c", "d"},
            {1  , 2  , 3  , 4  }
    );

    HTT_ASSERT_THROW(map.getKeyIndex("b") == 1);
    HTT_ASSERT_THROW(map.getKeyIndex("d") == 3);
    HTT_ASSERT_THROW(map.getKeyIndex("x") == map.size());
}

int main(int, char**)
{
    try {
        test_iterate();
        test_sort();
        test_search();
    } catch (std::runtime_error const & e) {
        cout << "TrigHTTVectorMapTest failed: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

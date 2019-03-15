/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/LArFCAL_SuperCell_ID_test.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Unit test for LArFCAL_SuperCell_ID.
 */

#undef NDEBUG

#include "CaloIdentifier/LArFCAL_SuperCell_ID.h"
#include "CxxUtils/make_unique.h"
#include "boost/foreach.hpp"
#include <iostream>
#include <string>
#include <vector>


#include "larfcal_id_test_common.cxx"


class LArFCAL_SuperCell_ID_Test
  : public LArFCAL_SuperCell_ID, public ILArFCAL_ID_Test
{
public:
  virtual int get_lar_field_value() const override { return lar_field_value(); }
  virtual int get_lar_fcal_field_value() const override { return lar_fcal_field_value(); }
};


void test_basic (const LArFCAL_SuperCell_ID& idhelper)
{
  std::cout << "test_basic\n";

  basic_print_id (idhelper, idhelper.channel_id (2, 1, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 2, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (2, 3, 1, 1));

  basic_print_id (idhelper, idhelper.channel_id (-2, 1, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (-2, 2, 1, 1));
  basic_print_id (idhelper, idhelper.channel_id (-2, 3, 1, 1));
}


int main()
{
  std::unique_ptr<IdDictParser> parser = make_parser();
  std::unique_ptr<LArFCAL_SuperCell_ID> idhelper = make_helper<LArFCAL_SuperCell_ID_Test>(*parser);
  try {
    test_basic (*idhelper);
    test_connected (*idhelper, true);
    test_exceptions (*idhelper);
  }
  catch(LArID_Exception & except){
    std::cout << "Unexpected exception: " << (std::string) except << std::endl ;
  }
  return 0;
}

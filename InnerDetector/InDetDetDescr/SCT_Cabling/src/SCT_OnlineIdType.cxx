/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_OnlineIdType.h"

//STL
#include <set>

namespace SCT_Cabling{
  DataSource
  onlineIdType(const unsigned int rodId){
    const unsigned int rod(rodId & 0xFFFFFF);
    typedef std::set<unsigned int> UintSet;
    // set of rod ids used in data and in montecarlo
    const UintSet commonSet={0x210000,  0x210001,  0x210002,  0x210003,  0x210004,  0x210005,  0x210006,
        0x210007,  0x210008,  0x210009,  0x21000a,  0x220000,  0x220001,  0x220002,  0x220003,  0x220004,
        0x220005,  0x220006,  0x220007,  0x220008,  0x220009,  0x22000a,  0x230000,  0x230001,  0x230002,
        0x230003,  0x230004,  0x230005,  0x230006,  0x230007,  0x230008,  0x230009,  0x23000a,  0x240000,
        0x240001,  0x240002,  0x240003,  0x240004,  0x240005,  0x240006,  0x240007,  0x240008,  0x240009,
        0x24000a,  0x24000b};
    const UintSet::const_iterator notFoundInCommon(commonSet.end());
    //
    // set of rods unique to usage in the database-derived cabling
    const UintSet dbSet={0x210100,  0x210101,  0x210102,  0x210103,  0x210104,  0x210105,  0x210106,
       0x210107,  0x210108,  0x210109,  0x21010a,  0x220100,  0x220101,  0x220102,  0x220103,  0x220104,
       0x220105,  0x220106,  0x220107,  0x220108,  0x220109,  0x22010a,  0x230100,  0x230101,  0x230102,
       0x230103,  0x230104,  0x230105,  0x230106,  0x230107,  0x230108,  0x230109,  0x23010a,  0x23010b,
       0x240100,  0x240101,  0x240102,  0x240103,  0x240104,  0x240105,  0x240106,  0x240107,  0x240108,
       0x240109,  0x24010a};
    const UintSet::const_iterator notFoundInDb(dbSet.end());
    //
    // set of rod Ids which are unique to usage in old montecarlo files with cabling derived from text file
    const UintSet mcSet={ 0x21000b,  0x21000c,  0x21000d,  0x21000e,  0x21000f,  0x210010,  0x210011,  0x210012,  0x210013,
        0x210014,  0x210015,  0x22000b,  0x22000c,  0x22000d,  0x22000e,  0x22000f,  0x220010,  0x220011,
        0x220012,  0x220013,  0x220014,  0x220015,  0x23000b,  0x23000c,  0x23000d,  0x23000e,  0x23000f,
        0x230010,  0x230011,  0x230012,  0x230013,  0x230014,  0x24000c,  0x24000d,  0x24000e,  0x24000f,
        0x240010,  0x240011,  0x240012,  0x240013,  0x240014};
    const UintSet::const_iterator notFoundInMc(mcSet.end());
    DataSource result(ROD_INVALID);
    if (commonSet.find(rod) == notFoundInCommon){
      //the rod is not among the rods which are common to db and mc, so look at those unique to db
      if (dbSet.find(rod) == notFoundInDb){
        //the rod is not in common or the db list, so look in mc list
        if (mcSet.find(rod) == notFoundInMc){
          //didnt find the rod anywhere
          result=ROD_INVALID;
        } else {
          //found the rod in mc
          result=ROD_MC_ONLY;
        }
      } else {
        //rod found in those unique to db
        result=ROD_DB_ONLY;
      }
    }else{
      //rod was in the common list of rods
      result=ROD_VALID;
    }
    return result;
  }//end of function definition
}//end of namespace


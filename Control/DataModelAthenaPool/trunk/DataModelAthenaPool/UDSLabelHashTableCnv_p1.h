/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef UDSLABELHASHTABLECNV_P1
#define UDSLABELHASHTABLECNV_P1

class UDSLabelHashTable;
class UDSLabelHashTable_p1;

/** @class UDSLabelHashTableCnv_p1
 *  @brief T/P converter class for UDSLabelHashTable used by UserDataStore
 */

class UDSLabelHashTableCnv_p1 {
 public:
  void transToPers(const UDSLabelHashTable& trans, UDSLabelHashTable_p1& pers);
  void persToTrans(const UDSLabelHashTable_p1& pers, UDSLabelHashTable& trans);
};

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAGRESULTENUM_H
#define TAGRESULTENUM_H

namespace TagResult
{
  enum TypeEnum
  {
    UNKNOWN=0, // not tagged yet
    passMpassD2_2Var,
    passMfailD2_2Var,
    failMpassD2_2Var,
    failMfailD2_2Var
  };
  inline int enumToInt(const TypeEnum type)
  {
    switch (type)
      {
      case passMpassD2_2Var: return 1;
      case passMfailD2_2Var: return 2;
      case failMpassD2_2Var: return 3;
      case failMfailD2_2Var: return 4;
      default:               return 0;
      }
  }
  inline TypeEnum intToEnum(const int type)
  {
    if ( type==1 ){
      return passMpassD2_2Var;
    }else if ( type==2 ){
      return passMfailD2_2Var;
    }else if ( type==3 ){
      return failMpassD2_2Var;
    }else if ( type==4 ){
      return failMfailD2_2Var;
    }
    
    return UNKNOWN;
  }
}

#endif

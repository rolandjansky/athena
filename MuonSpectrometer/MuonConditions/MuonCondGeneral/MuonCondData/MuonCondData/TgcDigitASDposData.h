/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDIGITASDPOSDATA_H
#define TGCDIGITASDPOSDATA_H

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *               Contents of TGC_Digitization_AsdPosition.db                   *
 *  ==================================================================         *
 *                    |1|2|3|4|5|6|7|8|9|10|11|12|13|                          *
 *                                                                             *
 *             1  -- station number(unsigned short) -> 41~48                   *
 *             2  -- station eta(unsigned short) -> 1~5                        *
 *             3  -- station phi(short) -> BW is -99, EI is 1~21, FI is 1~24   *
 *            4~5 -- strip ASD position coordinate(float)                      *
 *           6~13 -- Wire ASD position coordinate(float)                       *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class TgcDigitASDposData
{
public:
  TgcDigitASDposData();
  virtual ~TgcDigitASDposData() = default;

  enum {
    N_WIREASDPOS=8,
    N_STRIPASDPOS=2,
    N_CHANNELINPUT_TOASD=16
  };

  std::vector<unsigned short> stationNum;
  std::vector<unsigned short> stationEta;
  std::vector<short> stationPhi;
  std::vector<std::vector<float>> asdPos;
};
CLASS_DEF(TgcDigitASDposData, 54799429, 1)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TgcDigitASDposData>, 50515203, 1)

#endif // TGCDIGITASDPOSDATA_H

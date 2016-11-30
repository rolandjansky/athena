/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataFormatter.hh"

void
DataFormatter::Calc()
{
  std::cout << this->bd_index << " " << this->bd_t_in << " DataFormatter Calc() start" << std::endl;

  double fw_in, ew_in, fw_out, ew_out;
  double primary_fw_in, primary_ew_in, primary_fw_out, primary_ew_out;

  // ** fw_in ** //
  fw_in=0;
  primary_fw_in=0;
  // ** ew_in ** //
  ew_in=0;
  primary_ew_in=0;
  // ** fw_out ** //
  fw_out=0;
  primary_fw_out=0;
  // ** ew_out ** //
  ew_out=0;
  primary_ew_out=0;
    
  // **** Set iotime of DO***** //
//   this->iotime[bd_index].SetFwIn(fw_in);
//   this->iotime[bd_index].SetFwOut(fw_out);
//   this->iotime[bd_index].SetEwIn(ew_in);
//   this->iotime[bd_index].SetEwOut(ew_out);
//   this->iotime[bd_index].SetPriFwIn(primary_fw_in);
//   this->iotime[bd_index].SetPriFwOut(primary_fw_out);
//   this->iotime[bd_index].SetPriEwIn(primary_ew_in);
//   this->iotime[bd_index].SetPriEwOut(primary_ew_out);

  
  
}

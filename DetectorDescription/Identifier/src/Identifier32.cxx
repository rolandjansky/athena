/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "Identifier/Identifier32.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>

#include <iostream>
#include <iomanip>


//-----------------------------------------------
std::string Identifier32::getString() const
{
  std::string result;
  char temp[20];

  sprintf (temp, "0x%x", (unsigned int)m_id);
  result += temp;
  return (result);
}

//-----------------------------------------------
void Identifier32::show () const
{
    const Identifier32& me = *this;
    std::cout << me.getString();
}



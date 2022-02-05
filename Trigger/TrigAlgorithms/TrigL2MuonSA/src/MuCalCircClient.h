/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUCALCIRCCLIENT_H
#define  TRIGL2MUONSA_MUCALCIRCCLIENT_H

#include <string>
#include <exception>

#include "circ/Circ.h"
#include "circ/Circservice.h"

#include "MuCalDecode/CalibEvent.h"
#include "MuCalDecode/CalibUti.h"

namespace TrigL2MuonSA 
{  
  class Circexception: public std::exception
  {
    const char *what () const noexcept
    {
      return "Error in opening output buffer via Circ client";
    }
  };

  class MuCalCircClient
  {
   public:
    MuCalCircClient (unsigned short port, std::string &buffer_name, unsigned int buffer_size);
    virtual ~MuCalCircClient ();

    bool dumpToCirc (LVL2_MUON_CALIBRATION::CalibEvent &event) const;

   private:
    int m_cid;
    unsigned short m_port;
    std::string    m_bufferName;
    unsigned int   m_bufferSize;
  };
} // namespace TrigL2MuonSA

#endif  // 

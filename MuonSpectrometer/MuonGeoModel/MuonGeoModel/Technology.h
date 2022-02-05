/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Technology_H
#define Technology_H

#include <string>

// abstract base class for MDT, RPC etc inner structure
// a placeholder
namespace MuonGM {
    class MYSQL;

    class Technology {
      protected:
        std::string m_name;

      public:
        double thickness;
        Technology(MYSQL& mysql, std::string s);
        virtual ~Technology();
        std::string GetName();
    };
} // namespace MuonGM

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCONLINEDBENTRY_H
#define RPCONLINEDBENTRY_H

#include <string>
#include <iostream>
#include <sstream>
#include <stdint.h>

namespace MuonCalib{

    /**
      @class RpcOnlineDBEntry

      @author andrea.di.simone@cern.ch

      @brief representation of an online db entry (masked chans) in rpc cond db.

      RPC online (masks) db entry
     */

    class RpcOnlineDBEntry
    {
        public:
            RpcOnlineDBEntry(){};
            RpcOnlineDBEntry(uint64_t theID, uint64_t mask1, uint64_t mask2, uint64_t mask3);

	    void getColumns(std::string &mask1, std::string &mask2,std::string &mask3) const;  

            ~RpcOnlineDBEntry() {}

	    uint64_t getID() const;

        private:
	    uint64_t m_mask1, m_mask2,m_mask3,m_id;
    };

}

#endif

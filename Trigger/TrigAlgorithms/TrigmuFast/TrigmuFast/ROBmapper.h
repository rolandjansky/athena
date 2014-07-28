/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_ROB_MAPPER
#define MUON_ROB_MAPPER


#include <vector>
#include <string>
#include <stdint.h>


namespace MUON {


class ROBmapper {


    private:
    uint32_t (*m_robMap)[3][32][2];
    bool is_atlas;

    public:
    ROBmapper(std::string filename);
    ~ROBmapper();
    
    void lookup(unsigned int subsystem, unsigned int sector,unsigned int roi,
                std::vector<uint32_t>& ids) const;

};


}



#endif

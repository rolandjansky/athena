/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_ROI_MAPPER
#define MUON_ROI_MAPPER


#include <list>
#include <stdint.h>
#include <cstring>
#include <iosfwd>


namespace MUON {

struct ROImap {

    unsigned short int SYS;
    unsigned short int SUBSYS;
    unsigned short int SEC;
    unsigned short int sys;
    unsigned short int subsys;
    unsigned short int sec;
                   int th;
    float              phi;    
    
    ROImap()
    {
        memset(this, 0, sizeof(*this));
	this->th = -1;
    }
};


class ROImapper {


    private:
    std::list<ROImap> m_roiMap;

    public:
    ROImapper(std::string filename);
    ~ROImapper();
    
    const std::list<ROImap> roiMap(void) const;
    void remap(unsigned short int& system, unsigned short int& subsystem, 
               unsigned short int& sector, float& eta, float& phi, 
	       int th ) const;

    friend std::ostream& ::MUON::operator<< (std::ostream& stream,const ROImapper& map);

};

std::ostream& operator<< (std::ostream& stream,const ROImapper& map);

inline const std::list<ROImap> ROImapper::roiMap() const { return m_roiMap; }

}



#endif

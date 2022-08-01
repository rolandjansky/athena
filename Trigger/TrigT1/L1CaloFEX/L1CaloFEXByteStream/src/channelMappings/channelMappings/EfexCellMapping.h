#ifndef EFEX_CELL_MAPPING_H
#define EFEX_CELL_MAPPING_H

#include "channelMappings/L1CaloDetectorRegion.h"
#include "channelMappings/EfexCSVTables.h"
#include "channelMappings/EfexHardwareInfo.h"

#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <memory>
class EfexCellMapping {

    public:
        enum roiType { Roi };
        // Constructor for input data
        EfexCellMapping (int crate,
        int efexnumber, int fpga, int quad, int mgtchannel, int dataword){
            init( crate,  efexnumber,  fpga,  quad,  mgtchannel,  dataword);
        };
        //Constructor for output MGT scheme (0 to 48):
        EfexCellMapping (int crate,
        int efexnumber, int fpga, int mgtchannelout, int dataword);
        // Constructor for TOB positions
        EfexCellMapping ( roiType t, int crate,
        int efexnumber, int fpga, int eta, int phi );
        // Constructor for TOB positions with seed
        EfexCellMapping ( roiType t, int crate,
        int efexnumber, int fpga, int eta, int phi, int seed );
        // From full HW info returns an L1CaloDetectorRegion object which 
        // gives exact tower/supercell + depth
        L1CaloDetectorRegion getDetectorRegion() const;
        EfexHardwareInfo getHardwareInfo() const;
        std::string getLatomeID() const;
        //Dirty additions for Steve TODO remove later
        int m_fibrenumber{-1};
        int m_inputconnector{-1};
    private:
        void init(int crate, int efexnumber,int fpga, int quad, int mgtchannel , int dataword);
        std::string findModuleCords(int crate, int efexnumber) const;
        L1CaloDetectorRegion m_region;
        EfexHardwareInfo m_hwinfo;
        EfexCSVTables& m_tables{ EfexCSVTables::getInstance() };
        std::string m_latomeid;
};

#endif // EFEX_CELL_MAPPING_H

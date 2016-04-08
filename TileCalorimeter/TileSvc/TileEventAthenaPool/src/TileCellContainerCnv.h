/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileCellContainerCnv_H
#define TileCellContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "TileEvent/TileCellContainer.h"

class TileTBID;
class StoreGateSvc;
class CaloDetDescrElement;
class MbtsDetDescrManager;
typedef T_AthenaPoolCustCnv<TileCellContainer,TileCellVec> TileCellContainerCnvBase;

class TileCellContainerCnv:public TileCellContainerCnvBase
{

   friend class CnvFactory<TileCellContainerCnv >;
public:
    TileCellContainerCnv(ISvcLocator* svcloc);
    virtual ~TileCellContainerCnv();

    /// initialization
    virtual StatusCode initialize();

    virtual StatusCode transToPers(TileCellContainer* obj, 
				   TileCellVec*& persObj) ;
    virtual StatusCode persToTrans(TileCellContainer*& transObj,
				   TileCellVec* obj) ;

private:
    // vector of Collections.
    std::map<std::string,TileCellVec> m_vecCellAll;
    StoreGateSvc* m_storeGate; 
    const TileTBID* m_tileTBID;
    const MbtsDetDescrManager* m_mbtsMgr;

    int m_version;

    static const int NSIDE = 2;
    static const int NPHI  = 8;
    static const int NETA  = 2;
    static const int NCELLMBTS = NSIDE*NPHI*NETA;

    inline int cell_index(int side, int phi, int eta) const { return (side*NPHI+phi)*NETA+eta; }
    void initIdToIndex();
  
    Identifier m_id[NCELLMBTS];
    CaloDetDescrElement * m_dde[NCELLMBTS];
    int m_gainIndex[17];
    int m_gain[8];

    inline int round32(double x) { 
      if (x<-2147483647.) return -0x7FFFFFFF;
      else if (x>2147483647.) return 0x7FFFFFFF;
      else return (int)lround(x);
    }

    inline int round16(double x) { 
      if (x<-32767.) return -0x7FFF;
      else if (x>32767.) return 0x7FFF;
      else return (int)lround(x);
    }
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL1_RAWDATACONTAINERCNV_P2_H
#define PIXEL1_RAWDATACONTAINERCNV_P2_H

// Pixel1_RawDataContainerCnv_p2, T/P separation of Pixel Raw data
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetRawData/PixelRDO_Container.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetEventAthenaPool/InDetRawDataContainer_p2.h"
class PixelID;


// We do NOT use T_AthenaPoolTPCnvIDCont because we do all of the
// conversion in the .cxx. Same as for the LAr Raw Channels
// Note that this is used for a container of Pixel Raw Data
// that containes only SCR1_RawData concrete types
// PixelRDO_ContainerCnv.cxx delegates the conversion to this
// converter if the objects are of the correct type


class Pixel1RawDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase<PixelRDO_Container, InDetRawDataContainer_p2>
{
private:
    const PixelID *m_pixId;
public:
    Pixel1RawDataContainerCnv_p2() : m_pixId(0) {};
  
    virtual void	persToTrans(const InDetRawDataContainer_p2* persCont,
                                    PixelRDO_Container* transCont,
                                    MsgStream &log) ;
    virtual void	transToPers(const PixelRDO_Container* transCont,
                                    InDetRawDataContainer_p2* persCont,
                                    MsgStream &log) ;
    virtual PixelRDO_Container* createTransient(const InDetRawDataContainer_p2* persObj, MsgStream& log) ;


    // ID helper can't be used in the constructor, need initialize()
    void initialize(const PixelID *idhelper) { m_pixId = idhelper; }
  
};

#endif

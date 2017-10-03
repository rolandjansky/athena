/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calo Trigger Tower Manager converter package
 --------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTMgrDetDescrCnv.h,v 1.7 2008-12-14 01:56:04 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef CALOTTDETDESCRCNV_CALOTTMGRDETDESCRCNV_H
# define CALOTTDETDESCRCNV_CALOTTMGRDETDESCRCNV_H

#include "DetDescrCnvSvc/DetDescrConverter.h"


/**
 *  @brief This class is a converter for the CaloTTDescrManager which is
 *  stored in the detector store. 
 *
 *  This class derives from  DetDescrConverter which is a converter of the DetDescrCnvSvc. 
 *  This converter creates a manager object and adds descriptors and
 *  detector elements to the manager. This objects are either created
 *  or accessed from the detector store.
 *
 *  @warning The hadronic part of the FCAL calorimeter calorimeter is constructed
 *   with 4 channels in eta. These 4 channels actually correspond to 2 channels in eta
 *   FOR EACH of the hadronic samplings, i.e. FCAL2 and FCAL3. Therefore the geometry
 *   of these channels is wrong.
 */

class CaloTTMgrDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<CaloTTMgrDetDescrCnv>;

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long  storageType();
    static const CLID& classID();

protected:
    CaloTTMgrDetDescrCnv(ISvcLocator* svcloc);

private:

};


#endif // CALOTTDETDESCRCNV_CALOTTMGRDETDESCRCNV_H

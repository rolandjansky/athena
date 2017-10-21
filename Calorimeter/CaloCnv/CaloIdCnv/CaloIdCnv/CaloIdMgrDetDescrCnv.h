/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calo Identifier Manager converter package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloIdMgrDetDescrCnv.h,v 1.7 2009-02-10 14:09:21 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef CALOIDMGRDETDESCRCNV_CALOIDMGRDETDESCRCNV_H
#define CALOIDMGRDETDESCRCNV_CALOIDMGRDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"


/**
 *  @brief This class is a converter for the CaloIdManager which is
 *  stored in the detector store. 
 *
 *  This class derives from
 *  DetDescrConverter which is a converter of the DetDescrCnvSvc. This
 *  converter creates a manager object and adds descriptors and
 *  detector elements to the manager. This objects are either created
 *  or accessed from the detector store.
 *
 * @author RD Schaffer
 * @author maintained by F. Ledroit
 */

class CaloIdMgrDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<CaloIdMgrDetDescrCnv>;

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    /** Storage type (used by CnvFactory) */
    static long storageType();
    /** class ID (used by CnvFactory) */
    static const CLID& classID();

protected:
    CaloIdMgrDetDescrCnv(ISvcLocator* svcloc);

private:

};


#endif // CALOIDMGRDETDESCRCNV_CALOIDMGRDETDESCRCNV_H

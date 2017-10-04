/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LAr Identifier converter package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: LArIdMgrDetDescrCnv.h,v 1.5 2009-02-11 04:50:25 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef LARIDMGRDETDESCRCNV_LARIDMGRDETDESCRCNV_H
# define LARIDMGRDETDESCRCNV_LARIDMGRDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 *  This class is a converter for the LArDetDescrManager which is
 *  stored in the detector store. This class derives from
 *  DetDescrConverter which is a converter of the DetDescrCnvSvc. This
 *  converter creates a manager object and adds descriptors and
 *  detector elements to the manager. This objects are either created
 *  or accessed from the detector store.
 */

class LArIdMgrDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<LArIdMgrDetDescrCnv>;

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

protected:
    LArIdMgrDetDescrCnv(ISvcLocator* svcloc);

private:

};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // LARIDMGRDETDESCRCNV_LARIDMGRDETDESCRCNV_H

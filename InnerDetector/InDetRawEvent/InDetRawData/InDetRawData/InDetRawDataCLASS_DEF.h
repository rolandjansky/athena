/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetRawDataCLASS_DEF.h
//   Header file for class InDetRawDataCLASS_DEF
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the CLASS_DEF for Containers and Collections
///////////////////////////////////////////////////////////////////
// Version 1.0 25/09/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef INDETRAWDATA_INDETRAWDATACLASS_DEF_H
#define INDETRAWDATA_INDETRAWDATACLASS_DEF_H

// Include all headers here - just the containers and collections are
// enough
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/TRT_RDO_Container.h"

#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/TRT_RDO_Collection.h"

namespace {
    // Need this to read old data to create the dict entries
    DataVector<InDetRawDataCollection<TRT_LoLumRawData> > type1;
    DataVector<InDetRawDataCollection<SCT1_RawData> >     type2;
    DataVector<InDetRawDataCollection<Pixel1RawData> >    type3;
    DataVector<SCT1_RawData>                              type4;
    InDetRawDataCollection< Pixel1RawData >               type5;
    InDetRawDataCollection< SCT1_RawData >                type6;
    InDetRawDataCollection< SCT_RDORawData >              type7;
    InDetRawDataCollection< TRT_LoLumRawData >            type8;
    InDetRawDataCollection< TRT_RDORawData >              type9;

}


#endif // INDETRAWDATA_INDETRAWDATACLASS_DEF_H

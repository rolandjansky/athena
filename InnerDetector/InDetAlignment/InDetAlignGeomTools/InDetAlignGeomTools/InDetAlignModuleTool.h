/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGEOMTOOLS_INDETALIGNMODULETOOL_H
#define INDETALIGNGEOMTOOLS_INDETALIGNMODULETOOL_H

#include "TrkAlignGenTools/AlignModuleTool.h"

/**
   @file InDetAlignModuleTool.h
   @class InDetAlignModuleTool

   @brief Tool derived from AlignModuleTool which implements the method for
   returning a sub-TrkDetElementBase structure identifier Hash. This is only needed
   for TRT where the structure is a single straw.

   @author Daniel Kollar <daniel.kollar@cern.ch>
   @date 02/12/2009
*/

class TRT_ID;

namespace Trk {
  class AlignTSOS;
}

namespace InDet {

  class InDetAlignModuleTool :  public Trk::AlignModuleTool {

  public:

    InDetAlignModuleTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~InDetAlignModuleTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Returns identifier hash for sub-TrkDetElementBase structure based on detector type.
        In the inner detector it is only implemented for TRT where the structure is a single straw */
    virtual int subDetElementIDHash(Identifier id) const;

    /** allows to set detector specific properties of AlignTSOS.
        Fan-out angle is set here for SCT measurements in the endcaps. */
    virtual void setMeasurementProperties(Trk::AlignTSOS * atsos) const;

  private:

    const TRT_ID * m_trtHelper;         //!< TRT id helper

  };
  
}

#endif // INDETALIGNGEOMTOOLS_INDETALIGNMODULETOOL_H

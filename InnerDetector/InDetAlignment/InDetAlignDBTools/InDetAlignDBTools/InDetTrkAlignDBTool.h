/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNDBTOOLS_INDETTRKALIGNDBTOOL_H
#define INDETALIGNDBTOOLS_INDETTRKALIGNDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/ITrkAlignDBTool.h"

/**
  @file InDetTrkAlignDBTool.h 
  @class InDetTrkAlignDBTool 

  @brief Tool for writing Silicon and TRT alignment constants into oracle DB or ASCII files.

  @author Daniel Kollar <daniel.kollar@cern.ch>
  @date 08/09/2009
*/

class TFile;

namespace InDet
{

  class InDetTrkAlignDBTool : virtual public Trk::ITrkAlignDBTool, public AthAlgTool {

  public:

    /** constructor */
    InDetTrkAlignDBTool(const std::string & type, const std::string & name, const IInterface * parent);

    /** destructor */
    virtual ~InDetTrkAlignDBTool();

    /** initialize method */
    virtual StatusCode initialize();

    /** finalize method */
    virtual StatusCode finalize();

    /** Writes alignment parameters */
    void writeAlignPar();

  private:

    /** updates alignment constants in the DB */
    void updateDB();

    ToolHandle<Trk::ITrkAlignDBTool>      m_siDBtool;
    ToolHandle<Trk::ITrkAlignDBTool>      m_trtDBtool;

  }; // end class

} // end namespace

#endif // INDETALIGNDBTOOLS_INDETTRKALIGNDBTOOL_H

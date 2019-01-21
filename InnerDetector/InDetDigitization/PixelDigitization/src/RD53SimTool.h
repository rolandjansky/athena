/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_RD53SimTool_H
#define PIXELDIGITIZATION_RD53SimTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "FrontEndSimTool.h"

class RD53SimTool:public FrontEndSimTool {

  public:
    RD53SimTool( const std::string& type, const std::string& name,const IInterface* parent);

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~RD53SimTool();
    virtual void process(SiChargedDiodeCollection &chargedDiodes,PixelRDO_Collection &rdoCollection, CLHEP::HepRandomEngine *rndmEngine);

  private:
    RD53SimTool();

};

#endif // PIXELDIGITIZATION_RD53SimTool_H

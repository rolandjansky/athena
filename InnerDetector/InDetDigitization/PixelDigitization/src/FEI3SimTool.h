/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_FEI3SimTool_H
#define PIXELDIGITIZATION_FEI3SimTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "FrontEndSimTool.h"

class FEI3SimTool:public FrontEndSimTool {

  public:
    FEI3SimTool( const std::string& type, const std::string& name,const IInterface* parent);

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~FEI3SimTool();
    virtual void process(SiChargedDiodeCollection &chargedDiodes,PixelRDO_Collection &rdoCollection);

  private:
    FEI3SimTool();

    std::vector<bool> m_BarrelHitDuplication;
    std::vector<bool> m_EndcapHitDuplication;
    std::vector<int>  m_BarrelSmallHitToT;
    std::vector<int>  m_EndcapSmallHitToT;

    int m_timingTune;

    int relativeBunch2009(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge) const;

    int relativeBunch2015(const SiTotalCharge &totalCharge, int barrel_ec, int layer_disk, int moduleID) const;

};

#endif // PIXELDIGITIZATION_FEI3SimTool_H

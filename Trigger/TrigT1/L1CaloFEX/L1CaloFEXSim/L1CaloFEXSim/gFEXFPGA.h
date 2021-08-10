/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXFPGA - Defines FPGA tools
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef gFEXFPGA_H
#define gFEXFPGA_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXToolInterfaces/IgFEXFPGA.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXToolInterfaces/IgFEXJetAlgo.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"

#include <vector>

namespace LVL1 {

  class gFEXFPGA : public AthAlgTool, virtual public IgFEXFPGA {

  public:
    /** Constructors */
    gFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** Destructor */
    virtual ~gFEXFPGA();

    virtual StatusCode init(int id) override ;
    virtual void reset() override ;
    virtual int getID() const override {return m_fpgaId;}

    virtual void SetTowersAndCells_SG(gTowersCentral) override ;
    virtual void SetTowersAndCells_SG(gTowersForward) override ;

    virtual void GetEnergyMatrix(gTowersCentral &) const override ;
    virtual void GetEnergyMatrix(gTowersForward &) const override ;


    /** Internal data */
  private:

    int m_fpgaId = -1;

    gTowersCentral m_gTowersIDs_central;
    gTowersForward m_gTowersIDs_forward;


    SG::ReadHandleKey<LVL1::gTowerContainer> m_gFEXFPGA_gTowerContainerKey {this, "MyGTowers", "gTowerContainer", "Input container for gTowers"};


  };

} // end of namespace


#endif

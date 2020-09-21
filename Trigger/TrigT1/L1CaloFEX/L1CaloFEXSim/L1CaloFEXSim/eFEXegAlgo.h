/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXegAlgo.h  -  
//                              -------------------
//     begin                : 24 02 2020
//     email                : antonio.jacques.costa@cern.ch ulla.blumenschein@cern.ch tong.qiu@cern.ch
//  ***************************************************************************/


#ifndef eFEXegAlgo_H
#define eFEXegAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IeFEXegAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/eFEXegTOB.h"
#include "L1CaloFEXSim/eTowerContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The eFEXegAlgo class calculates the egamma TOB variables: Reta, Rhad and Wstot
  */
  
  class eFEXegAlgo : public AthAlgTool, virtual public IeFEXegAlgo {

  public:
    /** Constructors */
    eFEXegAlgo(const std::string& type, const std::string& name, const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    
    /** Destructor */
    virtual ~eFEXegAlgo();

    virtual StatusCode safetyTest() override;
    virtual void setup(int inputTable[3][3]) override; 

    virtual std::vector<unsigned int> getReta() override;
    virtual void getRhad(std::vector<unsigned int> & ) override;
    virtual void getWstot(std::vector<unsigned int> & ) override;
    virtual void getRealPhi(float & phi) override;
    virtual void getRealEta(float & eta) override;
    virtual std::unique_ptr<eFEXegTOB> geteFEXegTOB() override;
    virtual unsigned int getET() override;
    virtual void getWindowET(int layer, int jPhi, int SCID, unsigned int &) override;
    virtual bool haveSeed() override {return m_haveseed;};
  
    virtual void getCoreEMTowerET(unsigned int & et) override;
    virtual void getCoreHADTowerET(unsigned int & et) override;
  private:
    virtual void setSeed() override;
    bool m_seed_UnD = false; 
    unsigned int m_seedID = 999;
    int m_eFEXegAlgoTowerID[3][3];
    bool m_haveseed;

    SG::ReadHandleKey<LVL1::eTowerContainer> m_eFEXegAlgo_eTowerContainerKey {this, "MyETowers", "eTowerContainer", "Input container for eTowers"};

  };
  
} // end of namespace

//CLASS_DEF( LVL1::eFEXegAlgo, 32202260 , 1 )

#endif

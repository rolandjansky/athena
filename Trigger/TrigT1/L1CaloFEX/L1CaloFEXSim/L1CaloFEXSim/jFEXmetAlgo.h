/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXmetAlgo - Algorithm for MET Algorithm in jFEX
//                              -------------------
//     begin                : 14 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef jFEXmetAlgo_H
#define jFEXmetAlgo_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXmetAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jFEXmetTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h" 
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"


namespace LVL1 {

  class jFEXmetAlgo : public AthAlgTool, virtual public IjFEXmetAlgo{

  public:
    /** Constructors **/
    jFEXmetAlgo(const std::string& type, const std::string& name, const IInterface* parent);
   
    /** standard Athena-Algorithm method **/
    virtual StatusCode initialize() override;
    virtual StatusCode reset() override;

    /** Destructor **/
    virtual ~jFEXmetAlgo();

    virtual StatusCode safetyTest() override;
    virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) override;
    virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) override;

    virtual void buildBarrelmet()  override;
    virtual void buildFWDmet()  override;
    virtual int GetMetXComponent()  override;
    virtual int GetMetYComponent()  override;
    virtual int getTTowerET(unsigned int TTID ) override; 
    virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map)  override;
    
    virtual std::unique_ptr<jFEXmetTOB> getmetTOBs() override;
    
protected:

  private:
        SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        std::vector<std::vector<int>> m_FPGA;
        std::vector<std::vector<int>> m_FPGA_phi02;
        std::vector<std::vector<int>> m_FPGA_fcal;
        std::vector<int> m_met;
        std::vector<float> m_met_angle;
        std::vector<int> m_met_Xcoord;
        std::vector<int> m_met_Ycoord;
        int m_Totalmet_Xcoord;
        int m_Totalmet_Ycoord;
        
        virtual void buildMetXComponent();
        virtual void buildMetYComponent();        
        
        std::unordered_map<int,std::vector<int> > m_map_Etvalues;
  };



}//end of namespace


//CLASS_DEF( LVL1::jFEXmetAlgo , 121222945 , 1 )

#endif

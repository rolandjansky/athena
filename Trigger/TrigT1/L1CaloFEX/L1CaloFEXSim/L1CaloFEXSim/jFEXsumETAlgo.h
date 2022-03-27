/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXsumETAlgo - Algorithm for Sum Et Algorithm in jFEX
//                              -------------------
//     begin                : 12 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef jFEXsumETAlgo_H
#define jFEXsumETAlgo_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXsumETAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jFEXsumETTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h" 
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"


namespace LVL1 {

  class jFEXsumETAlgo : public AthAlgTool, virtual public IjFEXsumETAlgo{

  public:
    /** Constructors **/
    jFEXsumETAlgo(const std::string& type, const std::string& name, const IInterface* parent);
   
    /** standard Athena-Algorithm method **/
    virtual StatusCode initialize() override;
    virtual StatusCode reset() override;

    /** Destructor **/
    virtual ~jFEXsumETAlgo();

    virtual StatusCode safetyTest() override;
    virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) override;
    virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) override;
    virtual int getTTowerET(unsigned int TTID=0) override; // arguments 2,2 to get the central TT from m_TTwindow[5][5]

    virtual void buildBarrelSumET()  override;
    virtual void buildFWDSumET()  override;
    virtual int getETlowerEta(uint bin)  override;
    virtual int getETupperEta(uint bin)  override;   
    virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map)  override;
    
    virtual std::unique_ptr<jFEXsumETTOB> getsumETTOBs() override;
    
protected:

  private:
        SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        std::vector<std::vector<int>> m_FPGA;
        std::vector<std::vector<int>> m_FPGA_phi02;
        std::vector<std::vector<int>> m_FPGA_fcal;
        std::vector<int> m_SumET;
        int m_SumlowEta =0;
        int m_SumhighEta=0;
        
        std::unordered_map<int,std::vector<int> > m_map_Etvalues;
        
        
  };



}//end of namespace


//CLASS_DEF( LVL1::jFEXsumETAlgo , 121222945 , 1 )

#endif

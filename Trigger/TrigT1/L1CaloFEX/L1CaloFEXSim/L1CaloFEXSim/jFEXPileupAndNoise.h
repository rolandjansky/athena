/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXPileupAndNoise - Algorithm for Pileup and Noise in jFEX
//                              -------------------
//     begin                : 24 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef jFEXPileupAndNoise_H
#define jFEXPileupAndNoise_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXToolInterfaces/IjFEXPileupAndNoise.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h" 
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/StoreGateSvc.h" 
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"


namespace LVL1 {

  class jFEXPileupAndNoise : public AthAlgTool, virtual public IjFEXPileupAndNoise{

  public:
    /** Constructors **/
    jFEXPileupAndNoise(const std::string& type, const std::string& name, const IInterface* parent);
   
    /** standard Athena-Algorithm method **/
    virtual StatusCode initialize() override;
    virtual StatusCode safetyTest() override;
    virtual StatusCode reset() override;

    virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) override;
    virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) override;    
    
    virtual std::unordered_map<int,std::vector<int> > GetEt_values() override;
    virtual std::unordered_map<int,std::vector<int> > Get_EM_Et_values() override;
    virtual std::unordered_map<int,std::vector<int> > Get_HAD_Et_values() override;
    
    /** Destructor **/
    virtual ~jFEXPileupAndNoise();

    
    virtual std::vector<float> CalculatePileup()      override;
    virtual void ApplyPileup2Jets(bool b) override;
    virtual void ApplyPileup2Met(bool b)  override;
    virtual void ApplyNoise2Jets(bool b)  override;
    virtual void ApplyNoise2Met(bool b)   override;

    
protected:

  private:
        SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        SG::ReadHandle<jTowerContainer> m_jTowerContainer;
        
        int m_FPGA_central[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]={{0}};
        int m_FPGA_forward[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]={{0}};
        
        int m_FPGA_ET_central_EM[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]={{0}};
        int m_FPGA_ET_central_HAD[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]={{0}};
        int m_FPGA_ET_forward_EM[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]={{0}};
        int m_FPGA_ET_forward_HAD[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]={{0}};
        
        

        //rho variables for the pileup
        float m_rho_EM   = 0; //for eta < 3.2
        float m_rho_HAD1 = 0; //for eta < 1.5
        float m_rho_HAD2 = 0; //for 1.5 < eta < 1.6
        float m_rho_HAD3 = 0; //for 1.6 < eta < 3.2
        float m_rho_FCAL = 0; //for eta > 3.1
        
        //TT counters
        int m_count_rho_EM   = 0;
        int m_count_rho_HAD1 = 0;
        int m_count_rho_HAD2 = 0;
        int m_count_rho_HAD3 = 0;
        int m_count_rho_FCAL = 0;
        
        //boolean flags
        bool m_is_FWD      = false; // 0 for central, 1 for forward
        bool m_apply_pileup2jets  = false; 
        bool m_apply_pileup2met   = false;
        bool m_apply_noise2jets   = false;
        bool m_apply_noise2met    = false;
        
        void reset_conters();
        void SubtractPileup();
        void ApplyNoiseCuts(std::unordered_map<int,std::vector<int> > & map_Etvalues, int layer);


        // SG information
        int getTTowerEta  (unsigned int TTID ); 
        int getTTowerET   (unsigned int TTID ); 
        int getET_EM      (unsigned int TTID ); 
        int getET_HAD     (unsigned int TTID ); 
        float getTTArea_EM  (unsigned int TTID ); 
        float getTTArea_HAD (unsigned int TTID ); 
        
        //Noise values applied
        // It should be 0 GeV and 1 GeV in firmware LSB scale (bitwise is using MeV right now, CHANGE IF NEEDED!)
        int m_et_low  = 0;
        int m_et_high = 1000.;
        
        
        std::unordered_map<int,std::vector<int> > m_map_Etvalues_EM;
        std::unordered_map<int,std::vector<int> > m_map_Etvalues_HAD;
        
  };



}//end of namespace


CLASS_DEF( LVL1::jFEXPileupAndNoise , 585629 , 1 )

#endif

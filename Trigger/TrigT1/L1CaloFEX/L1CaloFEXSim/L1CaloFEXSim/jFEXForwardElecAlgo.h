
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

//***************************************************************************
//		jFEXForwardElecAlgo - Algorithm for Forward Electron Algorithm in jFEX
//                              -------------------
//     begin                : 16 11 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef jFEXForwardElecAlgo_H
#define jFEXForwardElecAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXForwardElecAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXForwardElecTOB.h"
#include "L1CaloFEXSim/jFEXForwardElecInfo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"


namespace LVL1 {

  class jFEXForwardElecAlgo : public AthAlgTool, virtual public IjFEXForwardElecAlgo {

  public:
    /** Constructors **/
    jFEXForwardElecAlgo(const std::string& type, const std::string& name, const IInterface* parent);
    
    /** standard Athena-Algorithm method **/
    virtual StatusCode initialize() override;
    
    /** Destructor **/
    virtual ~jFEXForwardElecAlgo();
    
    /** Standard methods **/
    virtual StatusCode safetyTest() override;
    virtual StatusCode reset() override;
   
    virtual void setup(int inputTable[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width], int jfex, int fpga ) override;
    
    //virtual std::array<float,4> getEtaPhiEt(uint) override;
    virtual std::array<float,2> getEtaPhi(uint) override;
    virtual std::array<uint,2> getEtEmHad(uint) override;

    virtual std::unordered_map<uint, jFEXForwardElecInfo> ClusterTTIDLists() override;
    virtual std::unordered_map<uint, jFEXForwardElecInfo> LocalMaxima() override;
    virtual std::unordered_map<uint, jFEXForwardElecInfo> calculateEDM() override;

    virtual bool isNeighbour(float aeta, int deltaNeta, float deltaPhi) override;  
    
	
    // Need to add the seeding, local maxima, clustering, calculation of etiso, etem , ethad etc 
    virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > etmapEM,std::unordered_map<int,std::vector<int> > etmapHAD) override;

    
    
    
  private:
    
    
    SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "jTower input container"};
    SG::ReadHandle<jTowerContainer> m_jTowerContainer;
    std::unordered_map<int,std::vector<int> > m_map_Etvalues_EM;
    std::unordered_map<int,std::vector<int> > m_map_Etvalues_HAD;
    int m_jFEXalgoTowerID[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width];
    int m_lowerEM_eta;
    int m_upperEM_eta;
    int m_jfex;
    int m_fpga;
    static constexpr float m_2PI = 2*M_PI;
    static constexpr float m_TT_Size_phi = M_PI/32;
    const int m_Edge_dR2 = std::round( (std::pow(2*M_PI/32,2)) * 1e5  );
    const int m_Edge_dR3 = std::round( (std::pow(3*M_PI/32,2)) * 1e5  );
    const int m_Edge_dR4 = std::round( (std::pow(4*M_PI/32,2)) * 1e5  );
        
        
  };
  
}//end of namespace

CLASS_DEF( LVL1::jFEXForwardElecAlgo, 71453331, 1 )

#endif


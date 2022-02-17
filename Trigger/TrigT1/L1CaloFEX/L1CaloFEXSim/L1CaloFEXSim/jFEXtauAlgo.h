/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXtauAlgo - Algorithm for Tau Algorithm in jFEX
//                              -------------------
//     begin                : 18 02 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef jFEXtauAlgo_H
#define jFEXtauAlgo_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXtauAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jFEXtauTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h" 
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/StoreGateSvc.h" 


namespace LVL1 {

  class jFEXtauAlgo : public AthAlgTool, virtual public IjFEXtauAlgo{

  public:
    /** Constructors **/
    jFEXtauAlgo(const std::string& type, const std::string& name, const IInterface* parent);
   
    /** standard Athena-Algorithm method **/
    virtual StatusCode initialize() override;

    /** Destructor **/
    virtual ~jFEXtauAlgo();

    virtual StatusCode safetyTest() override;
    virtual void setup(int TTwindow[5][5], int seed[3][3]) override;
    virtual int getTTowerET(unsigned int TTID ) override; 
    virtual int getRealPhi(unsigned int TTID  ) override; 
    virtual int getRealEta(unsigned int TTID  ) override;  

    virtual void buildSeeds() override; 
    virtual bool isSeedLocalMaxima() override;
    virtual void setFirstEtRing(int First_ETring[])  override;
    virtual int getClusterEt() override;
    virtual int getIsLocalMaxima() override;
    virtual int getFirstEtRing()  override;
    virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map)  override;
    
    
    virtual std::unique_ptr<jFEXtauTOB> getTauTOBs(int mphi, int meta) override;

    
// virtual jFEXtauAlgo* getTauTOBs() override;
//LVL1::jFEXtauAlgoTOB * LVL1::jFEXtauAlgo::getTauTOB()
    
protected:

  private:
        SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        
        int realValue(int ID, int eta);
        int m_SeedIDs[3][3]={{0}};
        int m_SeedConditions_ET[3][3]={{0}};
        int m_SeedCluster_ET[3][3]={{0}};
        int m_TTwindow[5][5]={{0}};
        int m_ClusterEt = 0;
        int m_TauIsolation = 0;
        //int m_tauClusterIDs[4][5];
	    bool m_seedSet=false;
	    bool m_isLocalMaxima=false;
          
        std::unordered_map<int,std::vector<int> > m_map_Etvalues;

        struct color {
            std::string RED      ="\033[1;31m";
            std::string ORANGE   ="\033[1;38;5;208m";
            std::string YELLOW   ="\033[1;33m";
            std::string GREEN    ="\033[1;32m";
            std::string BLUE     ="\033[1;34m";
            std::string PURPLE   ="\033[1;35m";
            std::string END      ="\033[0m";
            std::string B_BLUE   ="\033[1;44m";
            std::string B_PURPLE ="\033[1;45m";
            std::string B_ORANGE ="\033[1;48;5;208;30m";
            std::string B_GRAY   ="\033[1;100m";
            std::string B_RED    ="\033[1;41m";
            std::string B_GREEN  ="\033[1;42m";
        } m_color;


        
  };



}//end of namespace


CLASS_DEF( LVL1::jFEXtauAlgo , 121222945 , 1 )

#endif

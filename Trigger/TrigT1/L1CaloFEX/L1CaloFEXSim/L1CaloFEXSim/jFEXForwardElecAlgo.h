/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
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

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"


namespace LVL1 {

class jFEXForwardElecAlgo : public AthAlgTool, virtual public IjFEXForwardElecAlgo {

    public:
        /** Constructors **/
        jFEXForwardElecAlgo(const std::string& type, const std::string& name, const IInterface* parent);

        /** standard Athena-Algorithm method **/
        virtual StatusCode initialize() override;

        /** Destructor **/
        virtual ~jFEXForwardElecAlgo();

        virtual StatusCode safetyTest() override;
        virtual StatusCode reset() override;
        virtual void setup() override;


        virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map_EM,std::unordered_map<int,std::vector<int> > et_map_HAD)  override;

    protected:

    private:

        SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        SG::ReadHandle<jTowerContainer> m_jTowerContainer;
        std::unordered_map<int,std::vector<int> > m_map_Etvalues_EM;
        std::unordered_map<int,std::vector<int> > m_map_Etvalues_HAD;
        
        int getTTowerET_EM(unsigned int TTID );
        int getTTowerET_HAD(unsigned int TTID );
        int getTTowerTotalET(unsigned int TTID );
        int getPhi(unsigned int TTID  );
        int getEta(unsigned int TTID  );        
        

};

}//end of namespace

CLASS_DEF( LVL1::jFEXForwardElecAlgo, 71453331, 1 )

#endif

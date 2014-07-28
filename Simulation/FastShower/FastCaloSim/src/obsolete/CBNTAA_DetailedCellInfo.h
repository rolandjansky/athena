/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CBNTAA_DETAILEDCELLINFO_H
#define CBNTAA_DETAILEDCELLINFO_H

// 
// CBNTAA_DetailedCellInfo: 
//     CBNT Algorithm class accessing detailed cell Information
//		for the parametrization
//     Retrieve CaloCell container, and make ntuple
//
// ***** Algorithm properties (name defined in cxx file): 
// *****     NtupleID:         Ntuple ID number 
// *****     CellColl:         Cell Container name

#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include "FastCaloSim/FastShowerCellBuilderTool.h"
#include "HepPDT/ParticleDataTable.hh"

//#include "GaudiKernel/NTuple.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


class StoreGateSvc; 
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;
class CaloCalibrationHitContainer;
//class ICaloBadChanTool;


class CBNTAA_DetailedCellInfo: public CBNT_AthenaAwareBase
{
public:    

    CBNTAA_DetailedCellInfo(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~CBNTAA_DetailedCellInfo();
    StatusCode CBNT_initialize();
    StatusCode CBNT_execute();
    StatusCode CBNT_finalize();
    StatusCode CBNT_clear();

private: 

    std::vector<std::string> m_CalibrationContainerNames;
    std::vector<std::string> m_CalibrationContainerInactiveNames;
    std::string     m_cellsName;     //Cell container
    int     m_maxNCells ; //Maximum number of cells in ntuple
    std::vector<unsigned int> m_caloNums ; // which calo to treat
    float m_cellEnergyThreshold ; // threshold on cell energy for ntuple
                                // if negative symmetric
    bool m_saveDetInfo;
    bool m_saveTimeInfo;

    bool m_caloSelection ;
    bool m_saveQInfo;
    bool m_saveId;
    bool m_savePosition;

    const CaloDetDescrManager* m_caloDDM;
//  ToolHandle<ICaloBadChanTool> m_pb_tool;
    ToolHandle<ICaloCellMakerTool>  m_FastShowerCellBuilderTool;
    //ToolHandle<FastShowerCellBuilderTool> m_FastShowerCellBuilderTool;

    TruthHelper::GenAccessIO*                   m_gentesIO;
 
    HepPDT::ParticleDataTable*     m_particleDataTable;


// variables to be in ntuple.  

    long   m_nhit; 
    float  m_eCell;  


    long   m_nCells; 
    std::vector<float> * m_eCells; 
//######################
    std::vector<float> * EM_energy;
    std::vector<float> * NonEM_energy;
    std::vector<float> * Invisible_energy;
    std::vector<float> * Escaped_energy;

    std::vector<float> * EMInactive_energy;
    std::vector<float> * NonEMInactive_energy;
    std::vector<float> * InvisibleInactive_energy;
    std::vector<float> * EscapedInactive_energy;

    std::vector<long> * Layer;
    std::vector<long> * CellID;
//######################
    std::vector<float> * m_etaCells;  
    std::vector<float> * m_phiCells;  
    std::vector<float> * m_xCells;
    std::vector<float> * m_yCells; 
    std::vector<float> * m_zCells;
    std::vector<long>  * m_detCells;
    std::vector<float> * m_timeCells;
    std::vector<long>  * m_qCells;
    std::vector<float> * m_fitQCells;
    std::vector<long> * m_gainCells;
    std::vector< unsigned long >* m_offId;

//######################
   float  dcalosurf;
   float etacalosurf;
   float phicalosurf;

   std::vector<float> * ldcalolayercenter;
   std::vector<float> * letacalolayercenter;
   std::vector<float> * lphicalolayercenter;
   std::vector<bool>  * lok;

    StoreGateSvc* m_storeGate; 

    std::string m_suffix;        
    std::string m_cellsoutfile;

    std::string                    m_mcLocation;

    


};

#endif






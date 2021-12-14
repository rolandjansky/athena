/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARHITTOCELL_H
#define LARROD_LARHITTOCELL_H
// +======================================================================+
// +                                                                      +
// + Author ........: Denis Oliveira Damazio                              +
// + Institute ......: BNL                                                +
// + Creation date .: 29/04/2021                                          +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloConditions/CaloNoiseSigmaDiff.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "LArDigitization/LArHitEMap.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArElecCalib/ILArfSampl.h"
#include "CaloDetDescr/CaloDetDescrManager.h"


class CaloSuperCellDetDescrManager;
class ICaloSuperCellIDTool;
class CaloDetDescrElement;
class CaloCell;
class CaloIdManager;
class LArOnline_SuperCellID;

/**
 * @brief Make a dummy supercell @c LArRawChannelContainer from a
 *        @c CaloCallContainer.
 */
class LArHITtoCell : public AthReentrantAlgorithm
{
public:
  /// Standard Gaudi algorithm constructor.
  LArHITtoCell(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArHITtoCell() {};

  
  /// Standard Gaudi initialize method.
  StatusCode initialize(); 

  /// Algorithm execute method.
  StatusCode execute(const EventContext& context) const;

private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"ScCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};

  /// Property: Fraction of Energy Sampled (conditions input).
  SG::ReadCondHandleKey<ILArfSampl> m_fracSKey
     {this, "FracSKey", "LArfSamplSC", "SG Key of fSamplS conditions object"};

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey
    {this, "CaloDetDescrManager", "CaloDetDescrManager", "SG Key for CaloDetDescrManager in the Condition Store" };

  SG::ReadCondHandleKey<CaloSuperCellDetDescrManager>  m_caloSuperCellMgrKey
    {this, "CaloSuperCellDetDescrManager", "CaloSuperCellDetDescrManager", "SG key of the resulting CaloSuperCellDetDescrManager" };

  /// Property: Offline / supercell mapping tool.
  ToolHandle<ICaloSuperCellIDTool>     m_scidtool
	{ this, "SCIDTool", "CaloSuperCellIDTool" ,
		"Offline / supercell mapping tool."};

  /** hit map */
  SG::ReadHandleKey<LArHitEMap> m_hitMapKey{this,"LArHitEMapKey","LArHitEMap"};

  /// Property Output Container
  SG::WriteHandleKey<CaloCellContainer> m_cellKey{this,"CaloCellKey","SCellHITs",
      "SG key of the output CaloCellContainer"};

  /// if is SuperCell
  Gaudi::Property<bool> m_isSC { this, "IsSuperCell",true,
	"Is Super Cell" };

  /// Entry point for calorimeter ID helpers.
  const CaloIdManager* m_calo_id_manager;

  /** pointer to the offline TT helper */
  const CaloCell_SuperCell_ID*     m_scHelper = nullptr;
  /** pointer to the offline id helper  */
  const CaloCell_ID*               m_OflHelper = nullptr;


    template <class T>
  const T* retrieve(const EventContext& context, SG::ReadCondHandleKey<T> handleKey) const {
        SG::ReadCondHandle<T> handle( handleKey, context);
        if ( not handle.isValid() ) {
                ATH_MSG_ERROR("could not retrieve : " << handle.key() );
                return nullptr;
        } else return handle.cptr();
  }


};


#endif // not LARROD_LARHITTOCELL_H

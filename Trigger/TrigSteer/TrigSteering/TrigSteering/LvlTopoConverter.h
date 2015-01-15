/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : LvlTopoConverter
 *
 * @brief this tool initializes the HLT from LVL1 objects
 *
 * @author Ingrid Deigaard    <ingrid.deigaard@cern.ch>       Nikhef, NL
 *
 * File and Version Information:
 * $Id: LvlTopoConverter.h 629914 2014-11-20 17:23:34Z stelzer $
 **********************************************************************************/

#ifndef TrigSteering_LvlTopoConverter
#define TrigSteering_LvlTopoConverter


#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "DataModel/DataVector.h"

#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JetElement_ClassDEF.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoAlgorithms/ClusterSortingAlg.h"
#include "L1TopoAlgorithms/DeltaPhiLeading.h"
#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOB.h"

#include "L1TopoSimulation/IInputTOBConverter.h"

// namespace LVL1 {
//    class IInputTOBConverter;
// }

namespace HLT {

   class SteeringChain;

   static const InterfaceID IID_LvlTopoConverter("HLT::ILvlTopoConverter", 1, 0); //!< std Gaudi interface

   class ILvlTopoConverter : public virtual ILvlConverter {
   public:

      using ILvlConverter::hltExecute;

      /** @brief execute LvlConverter for one event
          @param activeChains reference to vector holding active chains; to be filled by the method !
      */
      virtual ErrorCode hltExecute(const std::vector<HLT::SteeringChain*>& activeCchains,TCS::TopoInputEvent & inpEvt) = 0;

      static const InterfaceID& interfaceID() { return IID_LvlConverter; } //!< Gaudi interface method
   };



   class LvlTopoConverter : public LvlConverter, virtual public ILvlTopoConverter {
   public:

      LvlTopoConverter(const std::string& name, const std::string& type,
                       const IInterface* parent); //!< std Gaudi constructor
      ~LvlTopoConverter() {} //!< destructor

      ErrorCode hltInitialize(); //!< HLT initialize -> get handle to services, read-in trigger configuration for L1 and HLT; prepare some lookup maps
      ErrorCode hltFinalize();   //!< HLT finalize   -> do nothing here
      ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& chains); //!< HLT execute -> see class description
      ErrorCode hltExecute(const std::vector<HLT::SteeringChain*>& chains,TCS::TopoInputEvent & inputEvent);

      static const InterfaceID& interfaceID() { return IID_LvlTopoConverter; } //!< Gaudi interface method

   private:

      StatusCode FillInputEvent(TCS::TopoInputEvent & inputEvent);

      //properties:
      //ServiceHandle<StoreGateSvc> m_storeGate;
      //ToolHandle<LVL1::IL1JetElementTools> m_JetElementTool;
      ToolHandle<LVL1::IInputTOBConverter> m_emtauInputProvider;
      ToolHandle<LVL1::IInputTOBConverter> m_jetInputProvider;
      ToolHandle<LVL1::IInputTOBConverter> m_energyInputProvider;
      ToolHandle<LVL1::IInputTOBConverter> m_muonInputProvider;

//       std::string m_triggerTowerLocation;
//       std::string m_jetElementLocation;

   };
} // end namespace


#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOSELECTORTOOL_H
#define EVENTINFOSELECTORTOOL_H

/** @file EventInfoSelectorTool.h
 *  @brief This file contains the class definition for the EventInfoSelectorTool class.
 *  $Id: EventInfoSelectorTool.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "AthenaKernel/IAthenaSelectorTool.h"
//#include "AthenaBaseComps/AthAlgTool.h"
#include "EventInfoCnvTool.h"

#include <string>

/** @class EventInfoSelectorTool
 *  @brief This class provides an example for reading with a ISelectorTool to veto events on AttributeList.
 **/
namespace xAODMaker {

   class EventInfoSelectorTool : public xAODMaker::EventInfoCnvTool, virtual public IAthenaSelectorTool {
   public: // Constructor and Destructor
      /// Standard Service Constructor
      EventInfoSelectorTool(const std::string& type, const std::string& name, const IInterface* parent);
      /// Destructor
      virtual ~EventInfoSelectorTool();

   public:
      /// IAthenaSelectorTool Interface method implementations:
      virtual StatusCode initialize();
      virtual StatusCode postInitialize();
      virtual StatusCode preNext() const;
      virtual StatusCode postNext() const;
      virtual StatusCode preFinalize();
      virtual StatusCode finalize();
   private:
      /// StoreGate key for the input object
      std::string m_aodKey;
      /// StoreGate key for the output object
      std::string m_xaodKey;

   };

   inline StatusCode EventInfoSelectorTool::initialize() {return EventInfoCnvTool::initialize();}
   inline StatusCode EventInfoSelectorTool::postInitialize() {return StatusCode::SUCCESS;}
   inline StatusCode EventInfoSelectorTool::preFinalize() {return StatusCode::SUCCESS;}
   inline StatusCode EventInfoSelectorTool::finalize() {return StatusCode::SUCCESS;}
}

#endif

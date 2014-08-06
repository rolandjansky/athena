// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CTP_RDOFillerTool.h 274305 2010-01-21 14:13:53Z krasznaa $
#ifndef TRIGGERD3PDMAKER_CTP_RDOFILLERTOOL_H
#define TRIGGERD3PDMAKER_CTP_RDOFILLERTOOL_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "TrigT1Result/CTP_RDO.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for CTP_RDO objects
    *
    *         This filler tool can be used to save the properties of CTP_RDO
    *         objects into the output D3PD. The CTP_RDO object is the readout
    *         data of the Central Trigger Processor. This data is only
    *         available at the ESD (and RAW data) level.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 274305 $
    * $Date: 2010-01-21 15:13:53 +0100 (Thu, 21 Jan 2010) $
    */
   class CTP_RDOFillerTool : public BlockFillerTool< CTP_RDO > {

   public:
      /// Regular AlgTool constructor
      CTP_RDOFillerTool( const std::string& type, const std::string& name,
                         const IInterface* parent );

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill( const CTP_RDO& rdo );

   private:
      uint32_t* m_timeSec;
      uint32_t* m_timeNanoSec;
      short*    m_numberOfBunches;
      short*    m_l1AcceptBunchPosition;
      std::vector< uint32_t >* m_pitWords;
      std::vector< uint32_t >* m_tbpWords;
      std::vector< uint32_t >* m_tapWords;
      std::vector< uint32_t >* m_tavWords;

   }; // class CTP_RDOFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_CTP_RDOFILLERTOOL_H

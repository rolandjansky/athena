// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZdcDigitsFillerTool.h 470583 2011-11-25 10:33:45Z krasznaa $
/**
  * @file HeavyIonD3PDMaker/FillerTool.h
  * @author Yujiao Chen
  * @author Marco Leite <leite@cern.ch>
  * @date August 2010
  * @brief Block filler tool for ZDC information.
  */
#ifndef FORWARDDETECTORSD3PDMAKER_ZDCDIGITSFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_ZDCDIGITSFILLERTOOL_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "ZdcEvent/ZdcDigits.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// Forward declaration(s):
class ZdcID;

namespace D3PD {

   // Convenience type definition:
   typedef D3PD::BlockFillerTool< ZdcDigits >
   ZdcDigitsFillerBase;

   class ZdcDigitsFillerTool : public ZdcDigitsFillerBase {

   public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type   The name of the tool type.
       * @param name   The tool name.
       * @param parent The tool's Gaudi parent.
       */
      ZdcDigitsFillerTool( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

      virtual StatusCode initialize();
      virtual StatusCode book();
      virtual StatusCode fill( const ZdcDigits& p );

   private:
      void clearData();

      const ZdcID* m_zdcID;

      //These hold all information: different gains, delays and methods
      std::vector< int >* m_dig_g0d0;
      std::vector< int >* m_dig_g0d1;
      std::vector< int >* m_dig_g1d0;
      std::vector< int >* m_dig_g1d1;

      unsigned int  *m_id;

      int            *m_module;
      int            *m_side;
      int            *m_type;
      int            *m_channel;

   }; // class ZdcDigitsFillerTool

} // namespace D3PD

#endif // FORWARDDETECTORSD3PDMAKER_ZDCDIGITSFILLERTOOL_H

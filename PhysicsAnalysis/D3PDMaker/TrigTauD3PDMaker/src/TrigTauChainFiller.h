/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUD3PDMAKER_TRIGTAUCHAINFILLER_H
#define TRIGTAUD3PDMAKER_TRIGTAUCHAINFILLER_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace TrigTauTool { class TrigReadNavTool; }

namespace D3PD {

  /**
   * @brief filler tool adds map of chainnames to rois(roiwords) passed per chain
   *
   * @author E. Ptacek
   * @date Mar, 2010
   */
  class TrigTauChainFiller
    : public BlockFillerTool<void>
    {
    public:
      /**
       * @brief Standard Gaudi tool constructor.
       * @param type The name of the tool type.
       * @param name The tool name.
       * @param parent The tool's Gaudi parent.
       */
      TrigTauChainFiller (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
      
      
      virtual StatusCode initialize();

      /// Book variables for this block.
      virtual StatusCode book();
      
      
      /**
       * @brief Fill one block --- type-safe version.
       *
       * This is called once per event?.  
       */
      virtual StatusCode fill ();
      
      
    private:
      ToolHandle<TrigTauTool::TrigReadNavTool> m_trigRnt;

      // filled to the ntuple

      ///map of chain names to ROIWords for Trigger
      std::vector<std::string>* m_chainRoImapChains; 
      std::vector< std::vector<unsigned int> >* m_chainRoImapRoIwords;
    };

} // namespace D3PD

#endif // TRIGTAUD3PDMAKER_TRIGTAUCHAINFILLER_H

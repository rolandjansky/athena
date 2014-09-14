/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_NAVIGATION_SLIMMING_H
#define TRIG_NAVIGATION_SLIMMING_H

#include <set>
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IAlgTool.h"


#include "TrigNavTools/TrigNavigationSlimmingTool.h"


namespace HLT {

  /**
   * @brief The TrigNavigationSlimming class is an athena algorithm used to slim
   *        the trigger Navigation structure in various ways
   *
   * @author Ben Smith <bcsmith@fas.harvard.edu> - Harvard University
   *
   * This algorithm allows the user to slim the trigger Navigation structure,
   * which is important in creating derived datasets, as the structure can grow
   * very large (> 30k per event).
   *
   * There are two main ways slimming can take place:
   * 
   * (a) By stream, group, or chain name.  By specifying the stream, group, or chain
   *     name in the job options file, specific parts of the structure can be removed
   *     or included at will.
   *
   * (b) By trigger element attributes.  It is possible to remove all intermediate
   *     (i.e., non-terminal or RoI) nodes, all ghost nodes, or all featureless nodes
   *
   */

  class HLTResult;

  class TrigNavigationSlimming : public AthAlgorithm {

    public:

      TrigNavigationSlimming(const std::string& name, ISvcLocator *pSvcLocator);
      ~TrigNavigationSlimming() { ; };

      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();

      
      
    protected:


      // needed for stream slimming
      //      void setResult(HLT::HLTResult *result) { m_result = result; };

      // needed tools
      ToolHandle<HLT::TrigNavigationSlimmingTool> m_slimmingTool;
      //      ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
      //      ServiceHandle<IThinningSvc> m_thinningSvc;

      // needed services
      //StoreGateSvc *m_storeGate;

      // internal flags




  private:

      
  }; // end of class TrigNavigationSlimming

} // end of namespace HLT

#endif

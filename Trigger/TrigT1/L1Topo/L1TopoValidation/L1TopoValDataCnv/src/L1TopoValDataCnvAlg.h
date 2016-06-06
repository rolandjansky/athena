// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGERCNV_L1TOPOVALDATACNVALG_H
#define XAODTRIGGERCNV_L1TOPOVALDATACNVALG_H

// System include(s):
#include <string>
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// Local include(s):

namespace xAODMaker {

   /**
    *  @short Algorithm for creating L1TopoValData objects of
    *         type xAOD::TrigComposite
    *
    *         This algorithm can be used to fill the L1TopoValData objects 
    *         form RAW data
    *
    * @author Werner Wiedenmann <Werner.Wiedenmann@cern.ch>
    *
    */
   class L1TopoValDataCnvAlg : public AthAlgorithm {

   public:
     /// Regular Algorithm constructor
     L1TopoValDataCnvAlg( const std::string& name, ISvcLocator* svcLoc );

     /// Function initialising the algorithm
     virtual StatusCode initialize();
     /// Function executing the algorithm
     virtual StatusCode execute();

   private:
     /// StoreGate key of the HLT result object
     std::string m_hltResultKey;
     /// StoreGate key for the output object
     std::string m_xaodKey;
     /// Flag for writing TAP words to output
     bool m_writeTAP;
     /// Flag for writing TAV words to output
     bool m_writeTAV;
     /// vector for simulated L1Topo result
     std::vector<int> m_simulatedTopoResult;
     /// vector for CTP L1Topo TIP words
     std::vector<int> m_ctpTopoTIPWords;
     /// vector for CTP L1Topo TBP words
     std::vector<int> m_ctpTopoTBPWords;
     /// vector for CTP L1Topo TAP words
     std::vector<int> m_ctpTopoTAPWords;
     /// vector for CTP L1Topo TAV words
     std::vector<int> m_ctpTopoTAVWords;

   }; // class L1TopoValDataCnvAlg
} // namespace xAODMaker

#endif // XAODTRIGGERCNV_L1TOPOVALDATACNVALG_H

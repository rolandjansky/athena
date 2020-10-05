/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_EDM_AUXCHECKER_H
#define TRIG_EDM_AUXCHECKER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "xAODCore/AuxContainerBase.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>


class TrigEDMAuxChecker : public AthAlgorithm  {

 public:

   TrigEDMAuxChecker(const std::string& name, ISvcLocator* pSvcLocator);
   ~TrigEDMAuxChecker();

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();



 private:


   void dumpDecorators(const xAOD::AuxContainerBase *,const std::string&);
   std::vector<std::string> m_auxList;
   std::map< std::string,std::vector<std::string> > m_auxmap;
   std::map< std::string,std::vector<std::string> > m_dynauxmap;
   int m_count;

};

#endif // TRIG_EDM_AUXCHECKER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMuonChamberT0s_H
#define MboyMuonChamberT0s_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
 
#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////////////////////////
#include "MuonboyAth/MuonboyAthMisc.h"

namespace Muon {
  class  ChamberT0s;
}
namespace AdjT0 {
  class  IAdjustableT0Tool;
}
class MdtIdHelper ;

  /**
   @class MboyMuonChamberT0s

   Save T0s found by Mboy

  @author samusog@cern.ch
  
  */

class MboyMuonChamberT0s:public AthAlgorithm {
public:
    MboyMuonChamberT0s(const std::string& name, ISvcLocator* pSvcLocator);
   ~MboyMuonChamberT0s();
   
public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////
//Data

   std::vector<Identifier> m_VectorOfIdentifiers ;
   
   void DumpIt(const Muon::ChamberT0s* pChamberT0s);
   void DumpItAfterStorage() ;
   
   std::string m_MuonChamberT0sLocation ; //!< Location of the MuonChamberT0s

   int m_DoDbg ; //!< Control debug

   const MdtIdHelper* p_MdtIdHelper ; //!< IdHelper

   ToolHandle< AdjT0::IAdjustableT0Tool >            p_IAdjustableT0Tool                                    ; //!< Pointer On IAdjustableT0Tool


};

#endif


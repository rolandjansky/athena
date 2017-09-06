/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKCachedBankGenAlgo_h
#define FTKCachedBankGenAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////

class FTKCachedBankGenAlgo: public AthAlgorithm {
 public:
  FTKCachedBankGenAlgo(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKCachedBankGenAlgo();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode RunCachedBankGenerator();
   

protected:
   std::string m_pmap_path;
   std::string m_rmap_path;
   std::string m_modulelut_path =""; // ModuleLUT, required only for HWMODEID=2
   std::string m_ssmapTSP_path;
   std::string m_ssmapAM_path;

   // Path of Bank
   std::string m_in_bank_path;
   std::string m_in_cache_path;
   std::string m_out_ccache_path;
   std::string m_out_pcache_path;

   std::string m_sectordefHW0;
   std::string m_sectordefHW2;

   int m_hwmodeid_default; // HWMODEID
   int m_hwmodeid_tspcc; // HWMODEID for sector-ordered and ccache format
   int m_hwmodeid_dc; // HWMODEID for pcache format
   int m_curreg; //! Region of interest
   int m_nSubReg; //! number of subregions
   int m_iSubReg; //! current subregion
   int m_verbosity; // for debugging
   long m_maxpatt; //! max. number of patterns, -1 if no cut is employed
   int m_IBLMode; //! IBLMode (0: noIBL, 1: with IBL, 2: 3DIBL)
   bool m_ITkMode; //! ITkMode (0: current geometry, 1: with ITk geometry)
   std::string m_WhereToRun; //! in which athena step the pattern generation should be run
   int m_flatFormat;

   std::string m_partitionList;
   int m_nSectorMax;
   int m_nLamb;

   int m_ndcMax;
   std::vector<int> m_ndcMaxPlane;
   int m_wildcardPenalty;
   std::vector<int> m_wildcardPenaltyPlane;

   std::string m_badmap_path;
   std::string m_badmap_path2;

private:

};

#endif // FTKCachedBankGenAlgo_h

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKPattBankGenRootAlgo_h
#define FTKPattBankGenRootAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigFTKSim/FTKLogging.h"
#include "TrigFTKBankGen/FTKPattGenRoot.h"

/////////////////////////////////////////////////////////////////////////////

class FTKPattGenRootAlgo: public AthAlgorithm , public FTKLogger {
 public:
  FTKPattGenRootAlgo(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKPattGenRootAlgo();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

protected:
   virtual void PostMessage(void);

   // Handle to the IAtRndmGenSvc
   ServiceHandle<IAtRndmGenSvc> m_rndmSvc;

   // // HepRandomEngine is an abstract class defining the interface for each random engine.
   //CLHEP::HepRandomEngine* m_pEng;
   
   std::string m_ssmap_path;
   //std::string m_ssmaptsp_path;
   std::string m_pmap_path;
   std::string m_rmap_path;
   std::string m_modulelut_path;
   std::string m_slices_path;
   std::string m_fitconstants_path;
   std::string m_sectors_path;
   // std::string m_dbghit_path;

   int m_curreg = 0; //! Region of interest
   int m_overlap = 2; //! overlap removal: 0=disabled; 1=barrel-only, 2=everywhere
   int m_HWMODEID = 0; //! (SS)HWMODEID (0 or 2)
   int m_IBLMode; // flag to change the IBLMode
   bool m_fixEndcapL0;
   bool m_ITkMode; // flag to use ITk geometry

   std::string m_rndStreamName;

   u_int64_t m_trials; //! Number of trials of the trackInvertion function

   // sector parameters
   double m_phi_min;
   double m_phi_max;
   double m_c_min;
   double m_c_max;
   double m_d0_min;
   double m_d0_max;
   double m_z0_min;
   double m_z0_max;
   double m_eta_min;
   double m_eta_max;
   double m_d0_alpha;

   // beam spot
   double m_beamspotX,m_beamspotY;
   
   FTKSectorSlice* m_sectorslice  ; //! FTKSectorSlice reads the slices file
   std::string m_OutputFile; //! output file to store patterns in

   int m_keep7of8;
   double m_tolerance7of8;

   int m_sectorSelection;

#ifdef DEBUG_BEAMSPOT
   ToolHandle<Trk::IPropagator> *m_propagator;
#endif
   FTKPattGenRoot *m_pattgen;
   uint64_t m_totalPatterns;
   uint64_t m_totalThrown;

   std::string m_ModuleGeometryFile;

private:

};

#endif // FTKPattGenRootAlgo_h

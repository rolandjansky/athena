/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArTBH6BeamInfo_H 
#define LArTBH6BeamInfo_H 

#include "GaudiKernel/Algorithm.h"

#include "Identifier/Identifier.h"

#include <string>

class StoreGateSvc;

class LArTBH6BeamInfo : public Algorithm {

public:

// Constructor
   LArTBH6BeamInfo(const std::string& name, ISvcLocator* pSvcLocator);

// Destructor
   virtual ~LArTBH6BeamInfo();

// Gaudi
   StatusCode initialize();
   StatusCode execute(); 
   StatusCode finalize();

private:

   typedef std::vector<double> dVect;

  ////////////////////////////////////////////////////////////////////////
  /// \brief Fit data to the function u = a1 + a2*w. and determines
  ///        intercept, slope, residual for each BPC, and chi2 on fit
  /////////////////////////////////////////////////////////////////////////
  bool fitVect(const dVect &vec_x, const dVect &vec_xz, const dVect &vec_ex,
	       double &a1, double &a2, double &chi2, dVect &residual);

   std::vector<std::string> m_HitsCollNames;
   bool                     m_Primary;
   int			    m_pcode;

   StoreGateSvc*            m_StoreGate;
   float                    m_cryoX;
   int 			    m_numEv;

};
#endif

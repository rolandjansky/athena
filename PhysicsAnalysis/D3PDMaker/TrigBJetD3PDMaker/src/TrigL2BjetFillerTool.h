// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __DP3D_TrigL2BjetFillerTool_h__
#define __DP3D_TrigL2BjetFillerTool_h__

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// EDM include(s):
#include "TrigParticle/TrigL2Bjet.h"

// Additional include(s):
#include "InDetBeamSpotService/IBeamCondSvc.h"

namespace D3PD {

   class TrigL2BjetFillerTool : public D3PD::BlockFillerTool< TrigL2Bjet > {

   public:
      TrigL2BjetFillerTool( const std::string &type, const std::string &name,
                            const IInterface *parent );

      StatusCode initialize();
      StatusCode book();
      StatusCode fill( const TrigL2Bjet &x );

   private:
      ServiceHandle< IBeamCondSvc > m_iBeamCondSvc;
      bool m_beamCondAvailable;

      bool mWriteBasics;
      bool mWriteHypoVars;
      bool mWriteDetails;

      int *m_roiId;
      float *m_prmVtx;
      float *m_xcomb;
      float *m_xIP1d;
      float *m_xIP2d;
      float *m_xIP3d;
      float *m_xChi2;
      float *m_xSv;

      float *m_xmvtx;
      float *m_xevtx;
      float *m_xnvtx;

      bool *m_valid;

      float *m_BSx;
      float *m_BSy;
      float *m_BSz;

      float *m_sBSx;
      float *m_sBSy;
      float *m_sBSz;
      float *m_sBSxy;

      float *m_BTiltXZ;
      float *m_BTiltYZ;
    
      int *m_BSstatus;

   }; // class TrigL2BjetFillerTool

} // namespace D3PD

#endif // __DP3D_TrigL2BjetFillerTool_h__

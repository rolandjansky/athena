// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __DP3D_TrigEFBjetFillerTool_h__
#define __DP3D_TrigEFBjetFillerTool_h__

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// EDM include(s):
#include "TrigParticle/TrigEFBjet.h"

namespace D3PD {

   class TrigEFBjetFillerTool : public D3PD::BlockFillerTool< TrigEFBjet > {

   public:
      TrigEFBjetFillerTool( const std::string &type, const std::string &name,
                            const IInterface *parent );

      StatusCode book();
      StatusCode fill( const TrigEFBjet &x );

   private:
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

   }; // class TrigEFBjetFillerTool

} // namespace D3PD

#endif // __DP3D_TrigEFBjetFillerTool_h__

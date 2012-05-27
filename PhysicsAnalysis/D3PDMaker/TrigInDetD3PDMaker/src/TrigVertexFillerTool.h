// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigVertexFillerTool.h 340544 2011-01-18 12:51:04Z krasznaa $
#ifndef __TRIGINDETD3PDMAKER_TRIGVERTEXFILLERTOOL_H__
#define __TRIGINDETD3PDMAKER_TRIGVERTEXFILLERTOOL_H__

// EDM include(s):
#include "TrigInDetEvent/TrigVertex.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool saving information about TrigVertex objects (LVL2)
    *
    *
    * @author Dmitry Emeliyanov
    */
   class TrigVertexFillerTool : public BlockFillerTool< TrigVertex > {

   public:
      /// Regular AlgTool constructor
      TrigVertexFillerTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigVertex& );

   private:
      bool m_writeBasicInfo; ///< Write basic information about the object
      bool m_writeHypoInfo; ///< Write the variables needed to re-do the hypo cuts
      bool m_writeDetailedInfo; ///< Write the most detailed information

      int*   m_algId; ///< Vertex author
      float* m_chi2; ///< Chi2 of vertex fit
      int*   m_nDoF; ///< Number of DOF of vertex fit

      float* m_x; ///< vertex coordinate
      float* m_y; ///< vertex coordinate
      float* m_z; ///< vertex coordinate
      float* m_mass; ///< invariant mass estimate
      float* m_massVariance; ///< variance of invariant mass estimate
      std::vector< float >* m_covariance; ///< Covariance of vertex parameters

      float* m_energyFraction; ///< energy ratio E(secondary vertex)/E(jet)
      int*   m_nTwoTracksSecVtx; ///< number of 2-track vertices

   }; // class TrigVertexFillerTool

} // namespace D3PD

#endif // __TRIGINDETD3PDMAKER_TRIGVERTEXFILLERTOOL_H__

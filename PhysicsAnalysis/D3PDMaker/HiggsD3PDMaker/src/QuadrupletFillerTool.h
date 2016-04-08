// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: QuadrupletFillerTool.h 485019 2012-02-24 16:16:04Z krasznaa $
/**
 * @file HiggsD3PDMaker/src/HiggsQuadrupletFillerTool.h
 * @author Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @date Aug, 2009
 * @brief Block filler tool for a four-momentum.
 */
#ifndef HIGGSD3PDMAKER_QuadrupletFillerTool_H
#define HIGGSD3PDMAKER_QuadrupletFillerTool_H

// EDM include(s):
#include "HSG2Event/Quadruplet.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @brief Block filler for the Higgs quadruplet
    *
    *         Block filler tool for the information stored directly in
    *         HSG2::Quadruplet objects.
    *
    * @author Kirill Prokofiev <Kirll.Prokofiev@cern.ch>
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 485019 $
    * $Date: 2012-02-24 17:16:04 +0100 (Fri, 24 Feb 2012) $
    */
   class QuadrupletFillerTool : public BlockFillerTool< HSG2::Quadruplet > {

   public:
      /// Standard Gaudi tool constructor
      QuadrupletFillerTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

      /// Book variables for this block.
      virtual StatusCode book();
      /// Fill the variables of this block
      virtual StatusCode fill( const HSG2::Quadruplet& q );

   private:
      unsigned int* m_fs; ///< Quadruplet final state type

   }; // class QuadrupletFillerTool

} // namespace D3PD

#endif // HIGGSD3PDMAKER_QuadrupletFillerTool_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef CALOD3PDMAKER_CALOCELLFILTERALG_H
#define CALOD3PDMAKER_CALOCELLFILTERALG_H

// STL include(s):
#include <string>
#include <vector>

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration(s):
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;
class ICaloNoiseTool;

/**
 *  @short Algorithm to filter CaloCellContainer
 *
 * @author Hong Ma March 2010
 *
 * $Revision$
 * $Date$
 */
class CaloCellFilterAlg : public AthAlgorithm {

public:    
   /// Regular Gaudi Algorithm constructor
   CaloCellFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

   /// Function executed before the event loop starts
   virtual StatusCode initialize();
   /// Function executed for each event
   virtual StatusCode execute();

private:

   std::string m_cellsName; ///< Cell container
   std::string m_outputCellsName; ///< output Cell container
   int         m_maxNCells; ///< Maximum number of cells to accept
   std::vector< unsigned int > m_caloNums; ///< which calo to treat
   float m_cellEnergyThreshold; ///< threshold on cell energy for ntuple
                                // if negative symmetric
   float m_sigmaCut; ///< cut on abs(E)/sigmaNoise (no cut if <0 )
   std::vector< unsigned int > m_caloSamplings; ///< which calo samplings to treat

   const LArEM_ID   *m_emid;
   const LArFCAL_ID *m_fcalid;
   const LArHEC_ID  *m_hecid;
   const TileID     *m_tileid;    

   ToolHandle< ICaloNoiseTool > m_noise_tool;

   bool m_caloSelection;
   bool m_caloSamplingSelection;

}; // class CaloCellFilterAlg

#endif // CALOD3PDMAKER_CALOCELLFILTERALG_H

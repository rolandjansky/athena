/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIGlobalEt.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HIGLOBALET_H
#define HIGLOBALET_H

#include "HIGlobal/HICaloUtil.h"
// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

class CaloCellContainer;

// No NameSpace selected 

/** @class HIGlobalEt
 *   @author  Adam Trzupek <Adam.Trzupek@ifj.edu.pl>
 *   @author  Dominik Derendarz <Dominik.Derendarz@ifj.edu.pl>
 *   @date 26 June 2009.
 *   @brief This calss is the algorithm for global Et reconstruction. 
 */  

class HIGlobalEt : public AthAlgorithm
  {
  public:

     /** Standard Athena-Algorithm Constructor */
     HIGlobalEt(const std::string& name, ISvcLocator* pSvcLocator);
     /** Default Destructor */
     ~HIGlobalEt();
     /** Function which retrieve cell Et from CaloCellContainer */ 
     float GetEt(const CaloCellContainer * cCell);   
     /** Function which retrieve cell Et from CaloCellContainer and multiplicate it by proper calibration factor depending on cell's eta */
     float* GetEtvsEta(const CaloCellContainer * cCell);
           
     /** standard Athena-Algorithm method */
     StatusCode          initialize();
     /** standard Athena-Algorithm method */
     StatusCode          execute();
     /** standard Athena-Algorithm method */
     StatusCode          finalize();

  private:
    HICaloUtil m_correction;
  }; 

#endif 

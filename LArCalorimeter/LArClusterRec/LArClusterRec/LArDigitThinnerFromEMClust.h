/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArDigitsThinner
 * @brief Algorithm to copy a selected subset of LArDigits
 * 
 */


#ifndef LARCLUSTERREC_LArDigitThinnerFromEMClust_H
#define LARCLUSTERREC_LArDigitThinnerFromEMClust_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"

#include <vector>
#include <string>

class HWIdentifier;
class Identifier;

class LArDigitThinnerFromEMClust : public AthAlgorithm
{
 public:
  
  LArDigitThinnerFromEMClust(const std::string& name,
			     ISvcLocator* pSvcLocator);

  ~LArDigitThinnerFromEMClust();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 
  private:
    
    StatusCode getCells();

    ToolHandle<LArCablingService> m_larCablingSvc; 
    const LArOnlineID* m_onlineID;

    std::string m_inputContainerName, m_outputContainerName;
    std::string m_clusterContainerName;

    std::vector<bool> m_listCells;
    size_t m_nchannels;

    unsigned int m_nevent;
    unsigned int m_ncells;

};

#endif

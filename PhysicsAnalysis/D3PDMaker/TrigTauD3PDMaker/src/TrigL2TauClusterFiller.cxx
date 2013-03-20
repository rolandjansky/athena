/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TrigL2TauClusterFiller.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Block filler tool for a trigtaucluster object.
 */


#include "TrigL2TauClusterFiller.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/BasicTypes.h"

namespace D3PD
{
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrigL2TauClusterFiller::TrigL2TauClusterFiller(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
    : BlockFillerTool<TrigTauCluster> (type, name, parent)
  {
    book().ignore(); // Avoid coverity warnings.
  }
  
  /**
   * @brief Book variables for this block.
   */
  StatusCode TrigL2TauClusterFiller::book()
  {
    CHECK( addVariable ("quality",             m_quality)  );
    CHECK( addVariable ("EMenergy",            m_EMenergy)  );
    CHECK( addVariable ("HADenergy",           m_HADenergy)  );
    CHECK( addVariable ("eta",                 m_eta)  );
    CHECK( addVariable ("phi",                 m_phi)  );    
    CHECK( addVariable ("CaloRadius",          m_CaloRadius)  );
    CHECK( addVariable ("EMRadius3S",          m_EMRadius3S) );
    CHECK( addVariable ("CoreFrac",            m_CoreFrac) );
    CHECK( addVariable ("EMFrac",              m_EMFrac) );    
    CHECK( addVariable ("HADRadius",           m_HADRadius)  );
    CHECK( addVariable ("IsoFrac",             m_IsoFrac)  );    
    CHECK( addVariable ("numTotCells",         m_numTotCells)  );
    CHECK( addVariable ("stripWidth",          m_stripWidth)  );
    CHECK( addVariable ("stripWidthOffline",   m_stripWidthOffline)  );
    
    CHECK( addVariable ("EMenergyWide",        m_EMenergyWide)  );
    CHECK( addVariable ("EMenergyNarrow",      m_EMenergyNarrow)  );
    CHECK( addVariable ("EMenergyMedium",      m_EMenergyMedium)  );

    CHECK( addVariable ("HADenergyWide",       m_HADenergyWide)  );
    CHECK( addVariable ("HADenergyNarrow",     m_HADenergyNarrow)  );
    CHECK( addVariable ("HADenergyMedium",     m_HADenergyMedium)  );
    
    CHECK( addVariable ("HADRadiusSamp",       m_HADRadiusSamp)  );
    CHECK( addVariable ("EMRadiusSamp",        m_EMRadiusSamp)  );
    
    CHECK( addVariable ("etNarrow",            m_etNarrow) );
    CHECK( addVariable ("etWide",              m_etWide) );
    CHECK( addVariable ("etMedium",            m_etMedium) );
    
    return StatusCode::SUCCESS;
  }
  
  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode TrigL2TauClusterFiller::fill(const TrigTauCluster& p)
  {
    /// add vars
    *m_quality           = p.quality();
    *m_EMenergy          = p.EMenergy();
    *m_HADenergy         = p.HADenergy();
    *m_eta               = p.eta();
    *m_phi               = p.phi();
    *m_CaloRadius        = p.CaloRadius();
    *m_EMRadius3S        = p.EMRadius3S();
    *m_HADRadius         = p.HadRadius();
    *m_CoreFrac          = p.CoreFrac();
    *m_EMFrac            = p.EMFrac();
    *m_IsoFrac           = p.IsoFrac();
    *m_numTotCells       = p.numTotCells();
    *m_stripWidth        = p.stripWidth();
    *m_stripWidthOffline = p.stripWidthOffline();
       
    const TrigTauClusterDetails* clusterDetails = p.clusterDetails();

    for(uint k=0; k<NEMSAMPLES; k++)
      {	
	m_EMenergyNarrow -> push_back(p.EMenergyNarrow(k));
	m_EMenergyMedium -> push_back(p.EMenergyMedium(k));
	m_EMenergyWide   -> push_back(p.EMenergyWide(k));
	
	if(clusterDetails)	  
	  m_EMRadiusSamp -> push_back(clusterDetails->EMRadius(k));	
      }
    
    for(uint k=0; k<NHADSAMPLES; k++)
      {	
	m_HADenergyNarrow -> push_back(p.HADenergyNarrow(k));
	m_HADenergyMedium -> push_back(p.HADenergyMedium(k));
	m_HADenergyWide   -> push_back(p.HADenergyWide(k));

	if(clusterDetails)
	  m_HADRadiusSamp->push_back(clusterDetails->HADRadius(k));
      }
    
    if(clusterDetails)
      {
	float caloeta = p.eta();
	*m_etNarrow     = (getEMEnergy(clusterDetails, 0) + getHADEnergy(clusterDetails, 0) )/cosh(caloeta);
	*m_etMedium     = (getEMEnergy(clusterDetails, 1) + getHADEnergy(clusterDetails, 1) )/cosh(caloeta);
	*m_etWide       = (getEMEnergy(clusterDetails, 2) + getHADEnergy(clusterDetails, 2) )/cosh(caloeta);
      }
    return StatusCode::SUCCESS;
  }
  
  
  float TrigL2TauClusterFiller::getEMEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice)
  {
    double EMEnergy;
    switch (widthChoice)
      {
      case 0:
	EMEnergy = (clusterDetails->EMenergyNarrow(0)+
		    clusterDetails->EMenergyNarrow(1)+
		    clusterDetails->EMenergyNarrow(2)+
		    clusterDetails->EMenergyNarrow(3));
	break;
      case 1:
	EMEnergy = (clusterDetails->EMenergyMedium(0)+
		    clusterDetails->EMenergyMedium(1)+
		    clusterDetails->EMenergyMedium(2)+
		    clusterDetails->EMenergyMedium(3));
	break;
      case 2:
	EMEnergy = (clusterDetails->EMenergyWide(0)+
		    clusterDetails->EMenergyWide(1)+
		    clusterDetails->EMenergyWide(2)+
		    clusterDetails->EMenergyWide(3));
	break;       
      default:
	EMEnergy = (clusterDetails->EMenergyNarrow(0)+
		    clusterDetails->EMenergyNarrow(1)+
		    clusterDetails->EMenergyNarrow(2)+
		    clusterDetails->EMenergyNarrow(3));
	
      }  
    return EMEnergy;
}
  
float TrigL2TauClusterFiller::getHADEnergy(const TrigTauClusterDetails* clusterDetails, int widthChoice)
{
  double HADEnergy;
  switch (widthChoice)
  {
    case 0:
       HADEnergy = (clusterDetails->HADenergyNarrow(0)+
                    clusterDetails->HADenergyNarrow(1)+
                    clusterDetails->HADenergyNarrow(2));
       break;
    case 1:
       HADEnergy = (clusterDetails->HADenergyMedium(0)+
                    clusterDetails->HADenergyMedium(1)+
                    clusterDetails->HADenergyMedium(2));
       break;
    case 2:
       HADEnergy = (clusterDetails->HADenergyWide(0)+
                    clusterDetails->HADenergyWide(1)+
                    clusterDetails->HADenergyWide(2));
       break;       
    default:
       HADEnergy = (clusterDetails->HADenergyNarrow(0)+
                    clusterDetails->HADenergyNarrow(1)+
                    clusterDetails->HADenergyNarrow(2));

  }  
  
  return HADEnergy;
}

} // namespace D3PD

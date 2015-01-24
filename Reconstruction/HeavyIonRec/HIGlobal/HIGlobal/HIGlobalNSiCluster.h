/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIGlobalNSiCluster.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HIGLOBALNSICLUSTER_H
#define HIGLOBALNSICLUSTER_H

#include "AthenaBaseComps/AthAlgorithm.h"



// No NameSpace selected 

/** @class HIGlobalNSiCluster

    This is for the Doxygen-Documentation.  
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.
    
    @author  Krzysztof Wozniak <Krzysztpf.Wozniak@ifj.edu.pl>
*/  

class HIGlobalNSiCluster : public AthAlgorithm
  {
  public:

     /** Standard Athena-Algorithm Constructor */
     HIGlobalNSiCluster(const std::string& name, ISvcLocator* pSvcLocator);
     /** Default Destructor */
     ~HIGlobalNSiCluster();

     /** standard Athena-Algorithm method */
     StatusCode          initialize();
     /** standard Athena-Algorithm method */
     StatusCode          execute();
     /** standard Athena-Algorithm method */
     StatusCode          finalize();

  private:
    
  int m_nDet;

  int m_eventNumber;
  int m_numberOfEvents;

  double m_r;
  double m_phi;
  double m_z;
  long m_rowWidth;
  long m_colWidth;
  double m_phiWidth;
  double m_etaWidth;
  double m_errPhi;
  double m_errZ;
  }; 

#endif 

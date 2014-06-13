/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIGlobalNSiCluster.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HIGLOBALNSICLUSTER_H
#define HIGLOBALNSICLUSTER_H

#include "HIGlobal/HIGlobalNSiCluster.h"
#include "HIGlobal/EtaPhiBins.h"

#include "StoreGate/StoreGateSvc.h"

#include <fstream>

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include "InDetIdentifier/PixelID.h"


#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetPrepRawData/SiWidth.h"

#include <string>

#include "TMath.h"

#include <iostream>
using std::cout;using std::endl;


//#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiCluster.h"



class AtlasDetectorID;
class Identifier;
class StoreGateSvc;

// No NameSpace selected 

/** @class HIGlobalNSiCluster

    This is for the Doxygen-Documentation.  
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.
    
    @author  Krzysztof Wozniak <Krzysztpf.Wozniak@ifj.edu.pl>
*/  

class HIGlobalNSiCluster : public Algorithm
  {

  typedef InDet::SiClusterContainer SiClusterContainer;
  typedef InDet::SiClusterCollection SiClusterCollection;
  typedef InDet::SiCluster SiCluster;

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
    
    /** class member version of retrieving MsgStream */
    mutable MsgStream                 m_log;

    /** class member version of retrieving StoreGate */
    StoreGateSvc*  m_sgSvc;

    /** member variables for algorithm properties: */
    // int/double/bool  m_propertyName;
  
  std::string m_SiClustersName;

  int m_nDet;

  const DataHandle<SiClusterContainer> m_clusters;
//  wrong:   const DataHandle<SiClusterCollection> m_clusters;
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

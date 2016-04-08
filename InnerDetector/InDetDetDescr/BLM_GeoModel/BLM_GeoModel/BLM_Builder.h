/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BLM_Builder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BLMBUILDER_H
#define BLMBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GeoModelInterfaces/IGeoSubDetTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class AtlasDetectorID;
class Identifier;
class GeoVPhysVol;
class StoreGateSvc;

namespace InDetDD
{

  /** @class BLMBuilder 

  Beam Loss Monitor Builder
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */  

  class BLM_Builder : virtual public IGeoSubDetTool, public AthAlgTool
    {
    public:
      BLM_Builder(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~BLM_Builder ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
       /** build the BCM geometry */
      virtual StatusCode build(GeoVPhysVol* parent);
  
      /** For alignment */      
      // Register callback function on ConDB object
      virtual StatusCode registerCallback( StoreGateSvc* detStore );

      // Callback function itself
      virtual StatusCode align(IOVSVC_CALLBACK_ARGS);

    private:

      /** member variables for algorithm properties: */
      std::vector<double> m_module0;
      std::vector<double> m_moduleI;
      std::vector<double> m_moduleII;
      std::vector<double> m_moduleIII;
      std::vector<double> m_moduleIV;
      std::vector<double> m_moduleV;
      std::vector<double> m_moduleVI;
      std::vector<double> m_moduleVII;
      std::vector<double> m_moduleVIII;
      std::vector<double> m_moduleIX;
      std::vector<double> m_moduleX;
      std::vector<double> m_moduleXI;
      unsigned int m_moduleon;
      bool m_blmon;
      bool m_BDparameters;
    }; 
} // end of namespace

#endif 

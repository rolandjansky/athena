/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_ALGOJIVEXML_H
#define JIVEXML_ALGOJIVEXML_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

//Forward declarations
class StoreGateSvc;

namespace JiveXML{
  class IDataRetriever; 
  class IFormatTool;
  class IStreamTool;
}  

namespace JiveXML {

  /**
   * Converts Reconstructed Event Data objects into XML files
   * which can be read by, for example, the Atlantis graphics package.
   *
   * @author N. Konstantinidis
   * @author S. Boeser
   */
  class AlgoJiveXML : public AthAlgorithm {

    public:
   
      //Constructor
      AlgoJiveXML (const std::string& name, ISvcLocator* pSvcLocator);

      //Default Athena algorithm methods
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

    private:

      /** @name Configurable options */  
      //@{
      //Whether default server stream shall be created
      bool m_onlineMode;
      //Whether events shall be written to a file
      bool m_writeToFile;
      //Whether XML geometry information shall be generated
      bool m_writeGeometry;
      // Geometry version from Athena
      std::string m_geometryVersionIn;
      // Athena release version
      std::string m_AtlasRelease;
      //@}


      /** List of dataTypes to be retrieved and written to xml */
      std::vector<std::string> m_dataTypes;

      /** List of geometry writer tools to be called */
      std::vector<std::string> m_GeoWriterNames;

    protected:

      /** 
       * The list of DataRetrievers. This is initialised using the list of names
       * supplied by the jobOptions. DataRetrievers are AlgTools residing in the
       * corresponding sub-detector packages (e.g. TrackRetriever in InDetJiveXML).
       **/
      ToolHandleArray<JiveXML::IDataRetriever> m_DataRetrievers;
      
      /** 
       * Handle to the formatting tool, which is passed on to
       * the data retrievers and converts the data into XML
       **/
      ToolHandle<JiveXML::IFormatTool> m_FormatTool;

      /**
       * List of streaming tools that pass the formatted XML text
       * into a file, to a server, etc.
       **/
      ToolHandleArray<JiveXML::IStreamTool> m_StreamTools;

  };

}//namespace
#endif 

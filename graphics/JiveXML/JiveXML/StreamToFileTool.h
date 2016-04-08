/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__STREAMTOFILETOOL_H
#define JIVEXML__STREAMTOFILETOOL_H

//IAlgTool interface
#include "JiveXML/IStreamTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace JiveXML {
  
  /**
   * Stream JiveXML event information into a file. Currently only
   * one file per event is supported, this will hopefully be changed soon.
   *
   * @author Sebastian Boeser
   */
  class StreamToFileTool : virtual public IStreamTool, public AthAlgTool{

    public:
      
      /// Constructor
      StreamToFileTool( const std::string&, const std::string&, const IInterface*);

      /// Initialize
      virtual StatusCode initialize();
      
      /// Finalize
      virtual StatusCode finalize();

      /// Stream one event
      virtual StatusCode StreamEvent( const unsigned long EventNumber, const unsigned int RunNumber, const std::ostringstream* EventBuffer ) ;

    private:

      /** Prefix put in front of file name */
      std::string m_FileNamePrefix;
      
      /** Suffix put at the end of the file name (including type)*/
      std::string m_FileNameSuffix;
      
      /** Generate a file name */
      std::string MakeFileName( const unsigned long EventNumber, const unsigned int RunNumber ) const;
      
      /** Creates a new output stream to write XML to */
      StatusCode NewFile( const unsigned long EventNumber, const unsigned  int RunNumber, std::ofstream *& outFile ) const ;

      /** Closes output stream */
      StatusCode CloseFile( std::ofstream *& outFile ) const ;
      
  };

} //Namespace

#endif

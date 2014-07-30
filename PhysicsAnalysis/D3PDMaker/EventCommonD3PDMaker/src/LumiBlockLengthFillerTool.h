// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockLengthFillerTool.h 542661 2013-03-27 00:35:13Z ssnyder $
#ifndef EVENTCOMMOND3PDMAKER_LUMIBLOCKLENGTHFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_LUMIBLOCKLENGTHFILLERTOOL_H

// Gaudi/Athena include(s):
#include "AthenaKernel/IOVSvcDefs.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving the time length of the current lumiblock
    *
    *         While most of the luminosity blocks have a fixed length
    *         (1 minute), many factors can make luminosity blocks longer or
    *         shorter during data taking. Some analyses need to know the
    *         exact length of the luminosity blocks, this tool is meant
    *         for them.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 542661 $
    * $Date: 2013-03-27 01:35:13 +0100 (Wed, 27 Mar 2013) $
    */
   class LumiBlockLengthFillerTool : public BlockFillerTool< void > {

   public:
      /// Regular Gaudi AlgTool constructor
      LumiBlockLengthFillerTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

      /// Regular AlgTool initialization
      virtual StatusCode initialize();

      /// Book variables for the block
      virtual StatusCode book();
      /// Fill the variables of this block
      virtual StatusCode fill();

   private:
      /// Function calculating the length of the current lumiblock
      StatusCode update( IOVSVC_CALLBACK_ARGS );

      std::string m_folderName; ///< Name of the COOL folder to read from

      float* m_lbLength; ///< Variable with the luminosity block length
      float m_lbLengthTransient; ///< Transient store for the value

   }; // class LumiBlockLengthFillerTool

} // namespace D3PD

#endif // EVENTCOMMOND3PDMAKER_LUMIBLOCKLENGTHFILLERTOOL_H

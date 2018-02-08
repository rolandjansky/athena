/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataUpdater.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATAUPDATER_H
#define ANALYSISCALIBRATIONDATAUPDATER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

class AtlasDetectorID;
class Identifier;

class TFile;
class TObject;

namespace Analysis 
{

  /** @class CalibrationDataUpdater

      This algorithm is meant to be used in a stand-alone fashion, to
      update a ROOT file containing results to be entered in COOL with fresh
      (but possibly partial) information. Alternatively, it can be used to
      change the default algorithm that is used (if multiple alternatives are
      available).
      
      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  class CalibrationDataUpdater : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       CalibrationDataUpdater(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~CalibrationDataUpdater();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method (unused) */
       StatusCode          execute();
       /** standard Athena-Algorithm method (unused) */
       StatusCode          finalize();

    private:
      
      /** auxiliary functions */
      void setDefaultResults(std::string Name, TFile* fOut) const;
      void copyResults(const std::string& from, const std::string& to,
		       TFile* fIn, TFile* fOut) const;
      bool isValidName(std::string Name) const;
      void writeOutput(TObject* in, std::string Name, TFile* fOut) const;

      //--------------- properties ------------------

      /** input ROOT file containing new results */
      std::string m_inputRootFile;
      
      /** ROOT file to be entered in COOL */
      std::string m_DbRootFile;

      /** names of input and output parametrisations */
      std::vector<std::string> m_paramNames;

      /** flag if these results are to be used as defaults (they will anyway
	  if no other results exist for the particular combination of jet collection /
	  tagging algorithm / operating point) */
      bool m_overrideDefaults;

    }; 
} // end of namespace

#endif 

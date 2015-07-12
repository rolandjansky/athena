/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class SimpleNoiseToolFromTextFile
    @brief Tool to provide noise from numbers read in a text file
    @author Rolf Seuster
*/

#ifndef CALOTOOLS_SIMPLENOISETOOLFROMTEXTFILE_H
#define CALOTOOLS_SIMPLENOISETOOLFROMTEXTFILE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "CaloIdentifier/CaloGain.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "Identifier/Identifier.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <map>

class CaloCell;
class CaloDetDescrElement;

class StoreGateSvc;

class SimpleNoiseToolFromTextFile : public AthAlgTool,
				    virtual public ICalorimeterNoiseTool
{
 public:
  
  SimpleNoiseToolFromTextFile(const std::string& type, const std::string& name,
		  const IInterface* parent);
  
  virtual ~SimpleNoiseToolFromTextFile();
  
  StatusCode initialize();

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);
  
  float getNoise(const CaloCell* aCell, CalorimeterNoiseType type=JOBOPTION);
  float getNoise(const CaloDetDescrElement* caloDDE, CalorimeterNoiseType type=JOBOPTION);

  bool isEOverNSigma(const CaloCell* caloCell, float sigmaCut , 
                     CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                     CalorimeterNoiseType type=JOBOPTION );

  float getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type=JOBOPTION);

  float getEffectiveSigma(const CaloCell* caloCell,
                     CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                     CalorimeterNoiseType type=JOBOPTION );


  
 private:
  // access calo detector managers and calo identifier helper
  // only needed for debug printout
  const CaloDetDescrManager* m_caloDDM;
  const CaloCell_ID* m_caloIDH;
  
  // Identifier keyed map of calo cell noise
  std::map<Identifier, float> m_cellNoise;
  
  // Properties
  std::string m_cellNoiseFileName;
  double m_cellNoiseUnits;           // CLHEP units of cell noise in noise file
  float m_cellNoiseDefault;          // noise value to return if cell noise not in noise file
  bool m_cellNoiseDefaultWarning;    // if true, print a MSG::WARNING if noise default is used
  
  float getNoiseHelper(Identifier id);
};
#endif

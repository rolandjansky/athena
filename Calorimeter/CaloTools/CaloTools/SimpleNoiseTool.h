/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class SimpleNoiseTool
    @brief Tool to provide noise from hardcoded numbers
    @author Rolf Seuster
*/

#ifndef CALOTOOLS_SIMPLENOISETOOL_H
#define CALOTOOLS_SIMPLENOISETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "CaloGeoHelpers/CaloSampling.h"

#include "CaloIdentifier/CaloGain.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "CaloInterface/ICalorimeterNoiseTool.h"

#include <map>
#include <string>

class CaloCell;

class StoreGateSvc;

class SimpleNoiseTool : public AthAlgTool,
			virtual public ICalorimeterNoiseTool
{
 public:

  typedef std::pair<CaloSampling::CaloSample,CaloGain::CaloGain> index_t;
  typedef std::map<index_t,double>                               store_t;
  
  SimpleNoiseTool(const std::string& type, const std::string& name,
		  const IInterface* parent);
  
  virtual ~SimpleNoiseTool();
  
  StatusCode initialize();
  
  float getNoise(const CaloCell* aCell, CalorimeterNoiseType type=JOBOPTION);
  float getNoise(const CaloDetDescrElement *caloCell, CalorimeterNoiseType type=JOBOPTION);

  bool isEOverNSigma(const CaloCell* caloCell, float sigmaCut , 
                     CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                     CalorimeterNoiseType type=JOBOPTION );

  float getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type=JOBOPTION);

  float getEffectiveSigma(const CaloCell* caloCell,
                     CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                     CalorimeterNoiseType type=JOBOPTION );


  
};
#endif

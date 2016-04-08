/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForDetEnvelopeTool_h 
#define ForDetEnvelopeTool_h 1

#include "GeoModelUtilities/GeoModelTool.h"
class ForDetEnvelopeManager;

class ForDetEnvelopeTool : public GeoModelTool 
{
 public:

  // Standard Constructor
  ForDetEnvelopeTool( const std::string& type, const std::string& name, const IInterface* parent );

  // Standard Destructor
  virtual ~ForDetEnvelopeTool();

  virtual StatusCode create( StoreGateSvc* detStore );
  virtual StatusCode clear(StoreGateSvc* detStore);
 private:
  const ForDetEnvelopeManager* m_manager;
};

#endif 

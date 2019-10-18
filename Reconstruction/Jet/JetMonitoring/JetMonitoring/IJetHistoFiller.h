/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETHISTOFILLER_H
#define IJETHISTOFILLER_H

#include "xAODJet/JetContainer.h"

#include "GaudiKernel/IAlgTool.h"

class JetMonitoringAlg;


static const InterfaceID IID_IJetHistoFiller("IJetHistoFiller", 1 , 0); 
////////////////////////////////////////////////////
/// \class IJetHistoFiller
///
/// Interface for tools in charge of filling 1 histo (or several closely related ones)
/// 
/// the main function processJetContainer() takes a JetMonitoringAlg as input so the implementation
/// can retrieve the monitoring tools from it.
class IJetHistoFiller : virtual public IAlgTool                         
{
public:
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IJetHistoFiller; }

  virtual ~IJetHistoFiller(){}
  
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const = 0;  

  
};


#endif

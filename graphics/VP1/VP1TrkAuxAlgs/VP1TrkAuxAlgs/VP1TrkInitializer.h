/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
// Header file for class VP1TrkInitializer                    //
//                                                            //
// This algorithm has been introduced in order to solve       //
// tool creation problem. Various tools have to be created    //
// at the initialize() phase, so that they can register       //
// callback functions in time and also initialize the         //
// tracking geometry service                                  //
//                                                            //
// Initial version August 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TRKAUXALGS_VP1TRKINITIALIZER
#define VP1TRKAUXALGS_VP1TRKINITIALIZER

#include "AthenaBaseComps/AthAlgorithm.h"
#include <QStringList>

class IToolSvc;

class VP1TrkInitializer: public AthAlgorithm
{
 public:
  VP1TrkInitializer(const std::string& name, ISvcLocator* pSvcLocator);
  ~VP1TrkInitializer();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  IToolSvc* m_toolSvc;

  // Properties for forcing Tool initialization
  bool m_forceExtrapolatorTools;
  bool m_forceFitterTools;

  // Initialize tools - needed to register callbacks at initialize() stage
  template <typename T> void initTools(QStringList& toolTypes, QString env);
};

#endif

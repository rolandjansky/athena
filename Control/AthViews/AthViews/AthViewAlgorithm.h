///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthViewAlgorithm.h 
// Header file for class AthViewAlgorithm
// Author: B Wynne, based on S Binet's AthAlgorithm
/////////////////////////////////////////////////////////////////// 
#ifndef ATHVIEWS_ATHVIEWALGORITHM_H
#define ATHVIEWS_ATHVIEWALGORITHM_H 1


// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthViews/View.h"

class AthViewAlgorithm : public AthAlgorithm
{
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  // Copy constructor: 

  /// Constructor with parameters: 
  AthViewAlgorithm(const std::string& name, 
               ISvcLocator* pSvcLocator,
               const std::string& version=PACKAGE_VERSION);

  /// Destructor: 
  virtual ~AthViewAlgorithm();

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  virtual StatusCode sysExecute(const EventContext&) override;
#else
  virtual StatusCode sysExecute() override;
#endif
  virtual StatusCode sysInitialize() override;


  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  SG::View * eventView(const EventContext& ctx);

  /// callback for output level property 
  void msg_update_handler(Property& outputLevel);

  //flags for whether the algorithm can only be run in a view (or not)
  bool m_require_view;
  bool m_require_not_view;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private:

  /// Default constructor: 
  AthViewAlgorithm(); //> not implemented
  AthViewAlgorithm (const AthViewAlgorithm& ); //> not implemented
  AthViewAlgorithm& operator= (const AthViewAlgorithm&); //> not implemented
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 



#endif //> !ATHVIEWS_ATHVIEWALGORITHM_H

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthHistogramTool.h 
// Header file for class AthHistogramTool
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHHISTOGRAMTOOL_H
#define ATHENABASECOMPS_ATHHISTOGRAMTOOL_H 1

// STL includes
#include <string>

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "GaudiKernel/ServiceHandle.h"


// Some needed forward declarations
class ITHistSvc;



class AthHistogramTool
  : public ::AthAlgTool,
    public ::AthHistogramming
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// Constructor with parameters: 
  AthHistogramTool( const std::string& type,
                    const std::string& name, 
                    const IInterface* parent );


  /// Destructor:
  virtual ~AthHistogramTool();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief The standard @c THistSvc (for writing histograms and TTrees 
   *  and more to a root file)
   * Returns (kind of) a pointer to the @c THistSvc
   */
  const ServiceHandle<ITHistSvc>& histSvc() const;


  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Initialization method invoked by the framework. This method is responsible
   *  for any bookkeeping of initialization required by the framework itself.
   *  It will in turn invoke the initialize() method of the derived algorithm,
   *  and of any sub-algorithms which it creates.
   */
  virtual StatusCode sysInitialize();



  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
protected:
  
  /// Pointer to the THistSvc (event store by default)
  ServiceHandle<ITHistSvc> m_histSvc;


  /// Name of the ROOT output stream (file)
  std::string m_prefix;

  /// Name of the ROOT directory
  std::string m_rootDir;

  /// The prefix for the histogram THx name
  std::string m_histNamePrefix;

  /// The postfix for the histogram THx name
  std::string m_histNamePostfix;

  /// The prefix for the histogram THx title
  std::string m_histTitlePrefix;

  /// The postfix for the histogram THx title
  std::string m_histTitlePostfix;


}; 


/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

// For the THistSvc
inline const ServiceHandle<ITHistSvc>& AthHistogramTool::histSvc() const 
{
  return m_histSvc;
}


#endif //> !ATHENABASECOMPS_ATHHISTOGRAMTOOL_H

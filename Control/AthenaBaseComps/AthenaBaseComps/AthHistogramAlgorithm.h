///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @class AthHistogramAlgorithm.h 
/// Header file for AthHistogramAlgorithm
/// @author: Karsten Koeneke (karsten.koeneke@cern.ch)
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHHISTOGRAMALGORITHM_H
#define ATHENABASECOMPS_ATHHISTOGRAMALGORITHM_H 1

// STL includes
#include <string>

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "GaudiKernel/ServiceHandle.h"


// Some needed forward declarations
class ITHistSvc;




class AthHistogramAlgorithm
  : public ::AthAlgorithm,
    public ::AthHistogramming
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// Constructor with parameters: 
  AthHistogramAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AthHistogramAlgorithm(); 


  /** Initialization method invoked by the framework. This method is responsible
   *  for any bookkeeping of initialization required by the framework itself.
   *  It will in turn invoke the initialize() method of the derived algorithm,
   *  and of any sub-algorithms which it creates.
   */
  virtual StatusCode sysInitialize();



  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief The standard @c THistSvc (for writing histograms and TTrees 
   *  and more to a root file)
   * Returns (kind of) a pointer to the @c THistSvc
   */
  const ServiceHandle<ITHistSvc>& histSvc() const;




  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
private:
  /// Default constructor: 
  ///AthHistogramAlgorithm();

  

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// a handle on the Hist/TTree registration service
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

// I/O operators
//////////////////////

// For the THistSvc
inline const ServiceHandle<ITHistSvc>& AthHistogramAlgorithm::histSvc() const 
{
  return m_histSvc;
}


#endif //> !ATHENABASECOMPS_ATHHISTOGRAMALGORITHM_H

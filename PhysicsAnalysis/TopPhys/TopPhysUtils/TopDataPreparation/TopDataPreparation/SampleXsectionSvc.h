/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLEXSECTIONSVC_H
#define SAMPLEXSECTIONSVC_H

#include <string>

#include "TopDataPreparation/SampleXsection.h"

/** @class SampleXsectionSvc
 **    
 ** @author W. H. Bell <W.Bell@cern.ch>
 **
 ** @brief Singleton class providing access to SampleXsection
 **
 ** The files associated with SampleXsection are read once when the unique instance is created.
 ** This happens on the first call to 
 ** SampleXsection::svc(std::string inputFile),
 ** which also return a pointer to the instance. Subsequently it is sufficient
 ** to call SampleXsectionSvc::svc() without any arguments.
 */
class SampleXsectionSvc {
 public:
  /** Return a pointer to the unique instance, initialize if necessary.
   *  @param inputFile name of the input file.
   *  @return pointer to SampleXsectionSvc instance, NULL on failure.
  */
  static SampleXsectionSvc *svc(std::string inputFile);

  /** Return a pointer to the unique instance, or NULL if not initialized.
   *  @return pointer to SampleXsectionSvc instance. */
  static SampleXsectionSvc *svc() { return s_instance; }

  /** Provide access to the WhichPeriod object held by the instance.
   *  @return pointer to the initialized WhichPeriod object. */
  const SampleXsection* sampleXsection() const { return m_sampleXsection; }

  
 private:

  /** Singleton pattern: Private constructor. */
  SampleXsectionSvc(std::string inputFile="");
  
  /** Singleton pattern: Private destructor. */
  ~SampleXsectionSvc();
  
  /** Singleton pattern: Copy constructor is private and undefined. */
  SampleXsectionSvc(const SampleXsectionSvc &);

  /** Singleton pattern: Assignment operator is private and undefined. */
  SampleXsectionSvc &operator=(const SampleXsectionSvc &);
  
  /** A pointer to carry the address of the unique instance. */
  static SampleXsectionSvc *s_instance;
  
  /** A member function to setup the WhichPeriod object. */
  int initialize(void);

  /** Input file. */
  std::string m_inputFile;

  /** The SampleXsection instance. */
  SampleXsection *m_sampleXsection;
};

#endif



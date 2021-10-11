/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// -*- C++ -*-
//
// powhegLesHouchesReader.h is - (c) Silvia Ferrario Ravasio and Tomas Jezo
// inspired by LesHouchesFileReader.h which is a part of ThePEG
//
#ifndef THEPEG_powhegLesHouchesFileReader_H
#define THEPEG_powhegLesHouchesFileReader_H
// This is the declaration of the powhegLesHouchesFileReader class.

#include "ThePEG/LesHouches/LesHouchesReader.h"
#include "Herwig7_i/PowhegLesHouchesFileReader.fh"
#include "ThePEG/PDT/Decayer.h"
#include "ThePEG/Utilities/CFileLineReader.h"
#include <string>
#include <stdio.h>


namespace ThePEG {


/**
 * powhegLesHouchesFileReader is an base class to be used for objects which
 * reads event files from matrix element generators. It inherits from
 * LesHouchesReader and extends it by defining a file handle to be
 * read from, which is opened and closed by the open() and close()
 * functions. Note that the file handle is a standard C filehandle and
 * not a C++ stream. This is because there is no standard way in C++
 * to connect a pipe to a stream for reading eg. gzipped files. This
 * class is able to read plain event files conforming to the Les
 * Houches Event File accord.
 *
 * @see \ref powhegLesHouchesFileReaderInterfaces "Th1e interfaces"
 * defined for powhegLesHouchesFileReader.
 * @see Event
 * @see LesHouchesReader
 */
class powhegLesHouchesFileReader: public LesHouchesReader {

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * Default constructor.
   */
  powhegLesHouchesFileReader() : m_neve(0), m_ieve(0), m_theQNumbers(false),
			   m_theIncludeFxFxTags(false),
			   m_theIncludeCentral(false) {}

  /**
   * Copy-constructor. Note that a file which is opened in the object
   * copied from will have to be reopened in this.
   */
  powhegLesHouchesFileReader(const powhegLesHouchesFileReader &);

  /**
   * Destructor.
   */
  virtual ~powhegLesHouchesFileReader();
  //@}

public:

  /** @name Virtual functions specified by the LesHouchesReader base class. */
  //@{
  /**
   * Initialize. This function is called by the LesHouchesEventHandler
   * to which this object is assigned.
   */
  virtual void initialize(LesHouchesEventHandler & eh);

  /**
   * Open a file with events. Derived classes should overwrite it and
   * first calling it before reading in the run information into the
   * corresponding protected variables.
   */
  virtual void open();

  /**
   * Close the file from which events have been read.
   */
  virtual void close();
 

  /**
   * Read the next event from the file or stream into the
   * corresponding protected variables. Return false if there is no
   * more events or if this was not a LHF event file.
   */
  virtual bool doReadEvent();
  //@}

  /**
   * Return the name of the file from where to read events.
   */
  string filename() const { return m_theFileName; }

  /** 
   * Return the optional weights information string ("Names")
   */

  virtual vector<string> optWeightsNamesFunc();
  

public:

  /** @name Functions used by the persistent I/O system. */
  //@{
  /**
   * Function used to write out object persistently.
   * @param os the persistent output stream written to.
   */
  void persistentOutput(PersistentOStream & os) const;

  /**
   * Function used to read in object persistently.
   * @param is the persistent input stream read from.
   * @param version the version number of the object when written.
   */
  void persistentInput(PersistentIStream & is, int version);
  //@}

  /**
   * Standard Init function used to initialize the interfaces.
   */
  static void Init();


  /** 
   * Erases all occurences of a substring from a string 
   */
  
  void erase_substr(std::string& subject, const std::string& search);


protected:

  /** @name Clone Methods. */
  //@{
  /**
   * Make a simple clone of this object.
   * @return a pointer to the new object.
   */
  virtual IBPtr clone() const;

  /** Make a clone of this object, possibly modifying the cloned object
   * to make it sane.
   * @return a pointer to the new object.
   */
  virtual IBPtr fullclone() const;
  //@}

  /** @name Standard (and non-standard) Interfaced functions. */
  //@{
  /**
   * Initialize this object after the setup phase before saving an
   * EventGenerator to disk.
   * @throws InitException if object could not be initialized properly.
   */
  virtual void doinit();

  /**
   * Return true if this object needs to be initialized before all
   * other objects because it needs to extract PDFs from the event file.
   */
  virtual bool preInitialize() const;
  //@

protected:

  /**
   * The wrapper around the C FILE stream from which to read
   */
  CFileLineReader m_cfile;

protected:

  /**
   * The number of events in this file.
   */
  long m_neve;

  /**
   * The current event number.
   */
  long m_ieve;

  /**
   * If the file is a standard Les Houches formatted file (LHF) this
   * is its version number. If empty, this is not a Les Houches
   * formatted file
   */
  string m_LHFVersion;

  /**
   * If LHF. All lines (since the last open() or readEvent()) outside
   * the header, init and event tags.
   */
  string m_outsideBlock;

  /**
   * If LHF. All lines from the header block.
   */
  string m_headerBlock;

  /**
   * If LHF. Additional comments found in the init block.
   */
  string m_initComments;

  /**
   * If LHF. Map of attributes (name-value pairs) found in the init
   * tag.
   */
  map<string,string> m_initAttributes;

  /**
   * If LHF. Additional comments found with the last read event.
   */
  string m_eventComments;

  /**
   * If LHF. Map of attributes (name-value pairs) found in the last
   * event tag.
   */
  map<string,string> m_eventAttributes;

private:

  /**
   * The name of the file from where to read events.
   */
  string m_theFileName;

  /**
   *  Whether or not to search for QNUMBERS stuff
   */
  bool m_theQNumbers;

  /**
   * Include/Read FxFx tags
   */
  bool m_theIncludeFxFxTags;

  /**
   * Include central weight (for backup use)
   */
  bool m_theIncludeCentral;

  /**
   *  Decayer for any decay modes read from the file
   */
  DecayerPtr m_theDecayer;
  
  /**
   * Further information on the weights
   */
  map<string,string> m_scalemap;

  /**
   * Temporary holder for optional weights
   */
  
  map<string,double> m_optionalWeightsTemp;

  map<string,string> m_optionalWeightsLabel;


private:

  /**
   * Describe an abstract base class with persistent data.
   */
  static ClassDescription<powhegLesHouchesFileReader> m_initpowhegLesHouchesFileReader;

  /**
   * Private and non-existent assignment operator.
   */
  powhegLesHouchesFileReader & operator=(const powhegLesHouchesFileReader &);

public:

  /** @cond EXCEPTIONCLASSES */
  /** Exception class used by powhegLesHouchesFileReader if reading the file
   *  fails. */
  class powhegLesHouchesFileError: public Exception {};
  /** @endcond */

};

}


#include "ThePEG/Utilities/ClassTraits.h"

namespace ThePEG {

/** @cond TRAITSPECIALIZATIONS */

/**
 * This template specialization informs ThePEG about the
 * base class of powhegLesHouchesFileReader.
 */
template <>
struct BaseClassTrait<powhegLesHouchesFileReader,1>: public ClassTraitsType {
  /** Typedef of the base class of powhegLesHouchesFileReader. */
  typedef LesHouchesReader NthBase;
};

/**
 * This template specialization informs ThePEG about the name of the
 * powhegLesHouchesFileReader class and the shared object where it is
 * defined.
 */
template <>
struct ClassTraits<powhegLesHouchesFileReader>
  : public ClassTraitsBase<powhegLesHouchesFileReader> {
  /**
   * Return the class name.
   */
  static string className() { return "ThePEG::powhegLesHouchesFileReader"; }
  /**
   * Return the name of the shared library to be loaded to get access
   * to the powhegLesHouchesFileReader class and every other class it uses
   * (except the base class).
   */
  static string library() { return "libpowhegHerwig.so"; }

};

/** @endcond */

}

#endif /* THEPEG_powhegLesHouchesFileReader_H */


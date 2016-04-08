///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthSequencer.h
// Header file for class AthSequencer
// Author: S.Binet<binet@cern.ch>
// Stolen from Gaudi repository
///////////////////////////////////////////////////////////////////
#ifndef GAUDISEQUENCER_ATHSEQUENCER_H
#define GAUDISEQUENCER_ATHSEQUENCER_H

// Include files
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"

#include "AthenaKernel/AlgorithmTimer.h"

#include <setjmp.h>
#include <signal.h>

/**
 ** ClassName: AthSequencer
 **
 ** Description: An AthSequencer is essentially a list of Algorithms and is responsible
 **              for their management. Note that Sequences may themselves contain other
 **              Sequences. The default execute( ) implementation loops over the
 **              members of the sequence, calling their execute( ) methods. However, this
 **              can be modified if a member is disabled, has already been executed, or a
 **              member indicates that it's filter fails. The the former two cases the
 **             execution of the member is bypassed. In the latter case, the loop is
 **             terminated and the AthSequencer assumes the same filtered state as the
 **             last member.
 **/
class AthSequencer : public AthAlgorithm 
{
public:

  /**
   ** Constructor(s)
   **/
  AthSequencer( const std::string& name, // The path object's name
                ISvcLocator* svcloc      // A pointer to a service location service
                );

  /**
   ** Destructor
   **/
  virtual ~AthSequencer( );

  /*****************************
   ** Public Function Members **
   *****************************/

  /**
   ** Initialization of a sequencer. Typically things like histogram creation,
   ** setting up of data structures etc, should be done here. If a sequence
   ** has properties specified in the job options file, they will be set to
   ** the requested values BEFORE the initialize() method is invoked.
   **/
  virtual StatusCode initialize( );
  
  /**
   ** AthSequencer Reinitialization.
   **/
  virtual StatusCode reinitialize( );
  
  /**
   ** The actions to be performed by the sequencer on an event. This method
   ** is invoked once per event.
   **/
  virtual StatusCode execute( );
  
  /**
   ** AthSequencer finalization.
   **/
  virtual StatusCode finalize( );
  
  /**
   ** AthSequencer beginRun.
   **/
  virtual StatusCode beginRun( );

  /**
   ** AthSequencer endRun.
   **/
  virtual StatusCode endRun( );

  /** Start (from INITIALIZED to RUNNING). @c IStateful::start
   */
  virtual StatusCode start ();
  
  /** Stop (from RUNNING to INITIALIZED). 
   */
  virtual StatusCode stop ();

  /**
   ** Reset the AthSequencer executed state for the current event.
   **/
  void resetExecuted( );

  /**
   ** Has the StopOverride mode been set?
   **/
  virtual bool isStopOverride( ) const;
  
  /**
   ** Append an algorithm to the sequencer.
   **/
  StatusCode append( Algorithm* pAlgorithm );

  /**
   ** Create a algorithm and append it to the sequencer. A call to this method
   ** creates a child algorithm object. Note that the returned pointer is
   ** to Algorithm (as opposed to IAlgorithm), and thus the methods of
   ** IProperty are also available for the direct setting of the algorithm's
   ** properties. Using this mechanism instead of creating algorithms
   ** directly via the new operator is preferred since then the framework
   ** may take care of all of the necessary book-keeping.
   **/
  StatusCode createAndAppend(
                             const std::string& type,  // The concrete algorithm class of the algorithm
                             const std::string& name,  // The name to be given to the algorithm
                             Algorithm*& pAlgorithm    // Set to point to the newly created algorithm object
                             );

  /**
   ** Remove the specified algorithm from the sequencer
   **/
  StatusCode remove( Algorithm* pAlgorithm );
  StatusCode remove( const std::string& name );

  /// Decode Member Name list
  StatusCode decodeMemberNames( );

  /// "Members" property handler
  void       membershipHandler( Property& theProp );

  /// Decode branch member naem list
  StatusCode decodeBranchMemberNames( );

  /// "BranchMembers" propertry handler
  void       branchMembershipHandler( Property& theProp );

protected:

  /**
   ** Append an algorithm to the sequencer.
   **/
  StatusCode append( Algorithm* pAlgorithm,
                     std::vector<Algorithm*>* theAlgs );

  /**
   ** Create a algorithm and append it to the sequencer. A call to this method
   ** creates a child algorithm object. Note that the returned pointer is
   ** to Algorithm (as opposed to IAlgorithm), and thus the methods of
   ** IProperty are also available for the direct setting of the algorithm's
   ** properties. Using this mechanism instead of creating algorithms
   ** directly via the new operator is preferred since then the framework
   ** may take care of all of the necessary book-keeping.
   **/
  StatusCode createAndAppend(
                             const std::string& type,  // The concrete algorithm class of the algorithm
                             const std::string& name,  // The name to be given to the algorithm
                             Algorithm*& pAlgorithm,    // Set to point to the newly created algorithm object
                             std::vector<Algorithm*>* theAlgs
                             );

  /**
   ** Decode algorithm names, creating or appending algorithms as appropriate
   **/
  StatusCode decodeNames( StringArrayProperty& theNames,
                          std::vector<Algorithm*>* theAlgs );

  /**
   ** Remove the specified algorithm from the sequencer
   **/

  StatusCode remove( const std::string& algname, std::vector<Algorithm*>* theAlgs );

private:

  /******************************
   ** Private Function Members **
   ******************************/

  /**
   ** Private Copy constructor: NO COPY ALLOWED
   **/
  AthSequencer( const AthSequencer& a );

  /**
   ** Private asignment operator: NO ASSIGNMENT ALLOWED
   **/
  AthSequencer& operator=( const AthSequencer& rhs );

  /// Run one algorithm.
  /// Broken out to avoid warnings related to longjmp.
  StatusCode executeAlgorithm (Algorithm* theAlgorithm,
                               volatile bool& all_good,
                               volatile bool& caughtfpe);

  /**************************
   ** Private Data Members **
   **************************/
  
  /// Member names (of the form '<cppType>/<instanceName>')
  StringArrayProperty m_names;

  /// Stop on filter failure Override flag
  BooleanProperty m_stopOverride;
  
  /// set optional algorithm / sequence time outs
  double m_timeout;
  int m_timeoutMilliseconds;
  
  /// timer will abort job once timeout for any algorithm or sequence is reached
  Athena::AlgorithmTimer m_abortTimer;
  
private:
  bool m_continueEventloopOnFPE;
  
  /*********************************************/
  /* global variables needed to handle signals */
  /*********************************************/
  
  static sigjmp_buf s_fpe_landing_zone;                /* jmp point for SIGFPE handler */
  static siginfo_t s_fpe_info;                     /* state storage for SIGFPE handler */
  static struct sigaction s_oldHandler;
  
  static void fpe_trap_enable();
  static void fpe_callback(int, siginfo_t*, void*);
  static bool prepareCatchAndEnableFPE();
  void cleanupAfterFPE(siginfo_t*);
  static void uninstallFPESignalHandler();
  
  static const size_t s_maxArraySize = 50;
  
  static void *s_array[s_maxArraySize];
  static size_t s_curArraySize;
  
  static int m_FPEexcepts;
  static bool m_installedSignalHandler;
  
};

#endif //GAUDISEQUENCER_ATHSEQUENCER_H


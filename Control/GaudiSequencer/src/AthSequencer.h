///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthSequencer.h
// Header file for class AthSequencer
// Author: S.Binet<binet@cern.ch>
// Stolen from Gaudi repository
///////////////////////////////////////////////////////////////////
#ifndef GAUDISEQUENCER_ATHSEQUENCER_H
#define GAUDISEQUENCER_ATHSEQUENCER_H

// Include files
#include "AthenaBaseComps/AthCommonDataStore.h"
#include "AthenaBaseComps/AthCommonMsg.h"

#include "Gaudi/Property.h"
#include "Gaudi/Sequence.h"

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
class AthSequencer
  : public AthCommonDataStore<AthCommonMsg<Gaudi::Sequence>>
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
  virtual StatusCode initialize( ) override;
  
  /**
   ** AthSequencer Reinitialization.
   **/
  virtual StatusCode reinitialize( ) override;
  
  /**
   ** The actions to be performed by the sequencer on an event. This method
   ** is invoked once per event.
   **/
  virtual StatusCode execute( const EventContext& ctx ) const override;

  /** Start (from INITIALIZED to RUNNING). @c IStateful::start
   */
  virtual StatusCode start () override;
  
  /** Stop (from RUNNING to INITIALIZED). 
   */
  virtual StatusCode stop () override;

  /**
   ** Reset the AthSequencer executed state for the current event.
   **/
  virtual void resetExecuted( const EventContext& ctx ) const;

  /**
   ** Has the StopOverride mode been set?
   **/
  virtual bool isStopOverride( ) const;
  
  /**
   ** Append an algorithm to the sequencer.
   **/
  StatusCode append( Gaudi::Algorithm* pAlgorithm );

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
                             Gaudi::Algorithm*& pAlgorithm    // Set to point to the newly created algorithm object
                             );

  /**
   ** Remove the specified algorithm from the sequencer
   **/
  StatusCode remove( Gaudi::Algorithm* pAlgorithm );
  StatusCode remove( const std::string& name );

  /// Decode Member Name list
  StatusCode decodeMemberNames( );

  /// "Members" property handler
  void       membershipHandler( Gaudi::Details::PropertyBase& theProp );

  /// Decode branch member naem list
  StatusCode decodeBranchMemberNames( );

  /// "BranchMembers" propertry handler
  void       branchMembershipHandler( Gaudi::Details::PropertyBase& theProp );

protected:

  /**
   ** Append an algorithm to the sequencer.
   **/
  StatusCode append( Gaudi::Algorithm* pAlgorithm,
                     std::vector<Gaudi::Algorithm*>* theAlgs );

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
                             Gaudi::Algorithm*& pAlgorithm,    // Set to point to the newly created algorithm object
                             std::vector<Gaudi::Algorithm*>* theAlgs
                             );

  /**
   ** Decode algorithm names, creating or appending algorithms as appropriate
   **/
  StatusCode decodeNames( Gaudi::Property<std::vector<std::string>>& theNames,
                          std::vector<Gaudi::Algorithm*>* theAlgs );

  /**
   ** Remove the specified algorithm from the sequencer
   **/

  StatusCode remove( const std::string& algname, std::vector<Gaudi::Algorithm*>* theAlgs );

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
  StatusCode executeAlgorithm (Gaudi::Algorithm* theAlgorithm,
                               const EventContext& ctx,
                               volatile bool& all_good,
                               volatile bool& caughtfpe) const;

  /**************************
   ** Private Data Members **
   **************************/
  
  /// Member names (of the form '<cppType>/<instanceName>')
  Gaudi::Property<std::vector<std::string>> m_names{this, "Members",{},
      "Algorithm names (of the form '<cppType>/<instanceName>')","SubAlgorithm"};

  // Atomic sequencer (don't unroll in MT)
  Gaudi::Property<bool> m_atomic;

  // Invert logic of Sequence
  Gaudi::Property<bool> m_modeOR;
  // Ignore filterPassed() decisions of member algs
  Gaudi::Property<bool> m_ignoreFilter;
  /// Stop on filter failure Override flag (normally stop if alg filter fails)
  Gaudi::Property<bool> m_stopOverride;
  /// a "Concurrent" or "Sequential" sequence. "Sequential" enforces
  /// strict ordering in MT. "Concurrent" allows scheduler re-ordering 
  /// for data flow (default)
  Gaudi::Property<bool> m_sequential;
  
  /// set optional algorithm / sequence time outs
  double m_timeout;
  int m_timeoutMilliseconds;
  
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
  void cleanupAfterFPE(siginfo_t*) const;
  static void uninstallFPESignalHandler();
  
  static const size_t s_maxArraySize = 50;
  
  static void *s_array[s_maxArraySize];
  static size_t s_curArraySize;
  
  static int m_FPEexcepts;
  static bool m_installedSignalHandler;
  
};

#endif //GAUDISEQUENCER_ATHSEQUENCER_H


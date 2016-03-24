//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file eformat/ROBFragmentNoTemplates.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: salnikov $
 * $Revision: 726499 $
 * $Date: 2016-02-26 17:52:24 +0100 (Fri, 26 Feb 2016) $
 *
 * @brief Defines the ROB fragment entity as described in the Event Format
 * note.
 */

#ifndef OFFLINE_EFORMAT_V40_ROBFRAGMENT_H
#define OFFLINE_EFORMAT_V40_ROBFRAGMENT_H

#include <vector>

#include "v40_Header.h"
#include "eformat/blob.h"
#include "eformat/Version.h"
#include "eformat/Problem.h"

namespace offline_eformat { namespace v40 {

  /**
   * Describes how to access the contents of a subdetector fragment, as
   * prescribed by the event format note.
   */
  class ROBFragment : public Header {

  public: //interface

   /**
     * To build a fragment given the containing buffer. I need to know
     * where the fragment starts in order to do that.
     *
     * @param it The exact position where this fragment should start.
     */
    ROBFragment (const uint32_t* it);

    /**
     * Copy constructor
     *
     * @param other The fragment to be copied
     */
    ROBFragment (const ROBFragment& other);

    /**
     * Builds an empty, otherwise useless ROBFragment
     */
    ROBFragment ();

    /**
     * Constructor that only works with T=<real pointer>. This constructor is
     * pretty much like the one with the pointer, but it will fetch the pointer
     * from the u32 blob. It is only used in the python bindings.
     *
     * @param blob The blob which contains the block of data this fragment is
     * in.
     */
    ROBFragment (const eformat::helper::u32list& blob);

    /**
     * Constructor that only works with T=<real pointer>. This constructor is
     * pretty much like the one with the pointer, but it will fetch the pointer
     * from the u32 blob. It is only used in the python bindings.
     *
     * @param blob The blob which contains the block of data this fragment is
     * in.
     */
    ROBFragment (const eformat::helper::u32slice& blob);
    
    /**
     * Destructor virtualisation
     */
    virtual ~ROBFragment();

    /**
     * Assignment
     *
     * @param other The fragment to be copied
     */
    ROBFragment& operator= (const ROBFragment& other);

    /**
     * Manual re-assignment
     *
     * @param it The position pointing the first word of this fragment
     */
    ROBFragment& assign (const uint32_t* it);

    
    /**
     * Says if the ROB fragment is valid. This method will throw exceptions.
     */
    bool check_rob (const uint16_t version=eformat::MAJOR_V40_VERSION) const;
    
    /**
     * Says if the ROB fragment is valid. Does not throw exceptions.
     */
    bool check_rob_noex (const uint16_t version=eformat::MAJOR_V40_VERSION) const;
        
    /**
     * Says if the ROB fragment is valid. Exception behaviour is controllable.
     */
    bool check_rob_impl 
    (const uint16_t version=eformat::MAJOR_V40_VERSION,
     const bool exc = true) const;
    
    /**
     * Says if the underlying ROD fragment is valid. Does not throw exceptions.
     */
    bool check_rod_noex
    (const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;

    /**
     * Says if the underlying ROD fragment is valid. Throws exceptions. 
     */
    bool check_rod
    (const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;
        
    /**
     * Says if the underlying ROD fragmentis valid. 
     * Exception behaviour is controllable.
     */
    bool check_rod_impl 
    (const uint16_t rod_version=eformat::MAJOR_V31_VERSION,
     const bool exc = true) const;

    /**
     * Says if the fragment is valid. This method will throw exceptions.
     */
    bool check(const uint16_t version=eformat::MAJOR_V40_VERSION,
      const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;

    /**
     * Says if the fragment is valid. This method does not throw exceptions.
     */
    bool check_noex (const uint16_t version=eformat::MAJOR_V40_VERSION,
      const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;


    /**
     * Says if the fragment is valid. Exception behaviour is controllable.
     */
    bool check_impl 
    (const uint16_t version=eformat::MAJOR_V40_VERSION,
     const uint16_t rod_version=eformat::MAJOR_V31_VERSION,
     const bool exc = true) const;

    /**
     * Tells which problemS the current fragment has.
     */
    void problems (std::vector<eformat::FragmentProblem>& p,
      const uint16_t version=eformat::MAJOR_V40_VERSION,
      const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;

    /**
     * Tells which problemS the current ROB fragment has.
     */
    void rob_problems (std::vector<eformat::FragmentProblem>& p,
      const uint16_t version=eformat::MAJOR_V40_VERSION) const;

    /**
     * Tells which problemS the underlying ROD fragment has.
     */
    void rod_problems (std::vector<eformat::FragmentProblem>& p,
      const uint16_t version=eformat::MAJOR_V31_VERSION) const;

    /**
     * Gets the source_id of the ROB fragment itself 
     */
    uint32_t rob_source_id () const; 

    /**
     * Sets the given pointer to the ROD fragment start
     *
     * @param it The pointer to be set to the ROD fragment start
     */
    const uint32_t* rod_start() const;
    inline void rod_start (const uint32_t*& it) const //legacy
    { it = rod_start(); }

    /**
     * Returns the fragment type.
     */
    uint32_t rod_marker() const;

    /**
     * Returns the total fragment size
     */
    uint32_t rod_fragment_size_word() const;

    /**
     * Returns the size, in words, of the current header. That does @b
     * not include the trailer.
     */
    uint32_t rod_header_size_word() const;

    /**
     * Returns the size, in words, of the trailer
     */
    uint32_t rod_trailer_size_word() const;

    /**
     * Returns the formatting version for the ROD fragment.
     */
    uint32_t rod_version() const;

    /**
     * Returns the source identifier of the ROD fragment.
     */
    uint32_t rod_source_id() const;

    /**
     * Returns the current run number.
     */
    uint32_t rod_run_no() const;

    /**
     * Returns the lvl1 identifier
     */
    uint32_t rod_lvl1_id() const;

    /**
     * Returns the bunch crossing identifier
     */
    uint32_t rod_bc_id() const;

    /**
     * Returns the lvl1 trigger type
     */
    uint32_t rod_lvl1_trigger_type() const;

    /**
     * Returns the detector event type
     */
    uint32_t rod_detev_type() const;

    /**
     * Returns the number of status words available
     */
    uint32_t rod_nstatus () const;
    
    /**
     * Returns the status words, as an iterator to the status words
     * available.
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    const uint32_t* rod_status() const;
    inline void rod_status (const uint32_t*& it) const //legacy
    { it = rod_status(); }

    /**
     * Returns the number of data words available
     */
    uint32_t rod_ndata () const; 

    /**
     * Returns a pointer to the first data word
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    const uint32_t* rod_data() const;
    void rod_data (const uint32_t*& it) const //legacy
    { it = rod_data(); }

    /**
     * Returns the status block position. A value of <tt>zero</tt> indicates
     * that the status block preceeds the data block. A value of <tt>one</tt>
     * means the contrary.
     */
    uint32_t rod_status_position () const;

    /**
     * Returns the number of children available.
     */
    virtual uint32_t nchildren () const;

    /**
     * Returns the nth child fragment. If the nth fragment doesn't exist, the
     * behaviour is undefined.
     *
     * @param p A preallocated pointer you should provide.
     * @param n The fragment position, starting at zero, of the child fragment
     * you would like to get.
     */
    const uint32_t* child(size_t n) const;
    virtual inline void child (const uint32_t*& p, size_t n) const //legacy
    { p = child(n); }

    /**
     * Returns all the children of this fragment. The input to this method is a
     * valid set of iterators to existing, pre-allocated pointers
     *
     * @param p A pointer to a set of preallocated TPointer's to set to the
     * position of the children of this fragment.
     * @param max The maximum number of children, p can point to.
     *
     * @return The number of children found on this fragment
     */
    virtual uint32_t children (const uint32_t** p, size_t max) const;

  private: //representation

    const uint32_t* m_start; ///< my one-after-the-last word

  };

}}

#endif /* OFFLINE_EFORMAT_V40_ROBFRAGMENT_H */

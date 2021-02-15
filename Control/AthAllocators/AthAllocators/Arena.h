// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Arena.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/Arena.h
 * @author scott snyder
 * @date May 2007
 * @brief Collection of memory allocators with a common lifetime,
 *        plus subsystem summary.
 *
 * The Arena classes provide a framework for memory allocation.
 * It supports the following general features:
 *
 *  - Segregated storage.  Objects of identical type are grouped together.
 *    Memory is allocated from the system in large blocks and carved
 *    up into the individual objects.  This allows eliminating malloc
 *    overhead and it helps with reducing problems due to heap fragmentation.
 *
 *  - Objects may be `cached': the object constructor is run only when
 *    the memory is first allocated.  If it is freed and then reallocated
 *    by the application, the destructor/constructor calls may be eliminated.
 *
 *  - Memory allocations of similar lifetimes may be grouped together
 *    in ``Arenas''.  All memory in a given Arena may be freed at once.
 *
 *  - We attempt to make individual object allocation and deallocation
 *    very fast.  The code for the `common' case can be entirely inlined,
 *    and should compile to a handful of instructions.
 *
 *  - We keep statistics about the amount of memory allocated
 *    per type per Arena.
 *
 * First, a bit of terminology; this also summarizes the major components
 * of the library.  This will be followed by more detailed descriptions.
 * See also the individual class headers.
 *
 *   - An @em element is an individual piece of memory that the application
 *     requests.
 *
 *   - A @em block is a piece of memory allocated from the system
 *     It will usually be divided up into individual elements.
 *
 *   - An @em Allocator is the fundamental memory allocator, managing
 *     storage for a single type of object.  Memory allocated from the
 *     system associated with a single Allocator.  The Allocator
 *     classes themselves do not depend on the types being allocated;
 *     however, functions operating on these objects, such as constructors
 *     and destructors, may be given to the Allocator to be called
 *     at an appropriate time.  Multiple Allocator implementations
 *     may be available, implementing different strategies.
 *
 *   - An @em Arena is a group of Allocators, which share a common lifetime.
 *     The memory allocated by all Allocators in an Arena may be freed
 *     in a single operation.
 *
 *   - An @em ArenaHeader represents a group of Arenas.  One Arena
 *     is considered the `current' Arena; it is the one from which
 *     memory allocations will be made.  Arenas within the group
 *     may be made current in a stack-like manner.
 *
 *   - A @em Handle is the interface the application uses to allocate memory.
 *     A Handle is templated on the type being allocated as well as on the
 *     underlying Allocator.  When a Handle is constructed, it is associated
 *     with the Allocator associated with the Arena that is current at that time
 *     (a new Allocator is automatically created if required).  Therefore,
 *     a Handle should not be passed between threads, and Handle objects
 *     should not exist across any point where the current store/Arena
 *     may be changed.
 *
 *     A Handle also holds a lock on its associated allocator.
 *     Therefore, if you try to create two handle instances referencing the
 *     same allocator (i.e, same type and same thread), you'll get a deadlock.
 *     
 *     Multiple Handle implementations may be available, implementing
 *     different strategies for initializing the elements.
 *
 * Here are some more details about these components.  For full details,
 * see the individual class headers.
 *
 * An @em Allocator is the fundamental allocator for a single type.
 * Allocator instances generally request memory from the system
 * in big blocks and then divide it up into individual elements.
 * The memory allocated from the system is generally not returned
 * to the system unless explicitly requested; elements not currently
 * in use are kept in a pool for fast allocation.  An Allocator
 * class does not depend on the type being allocated.  Instead, the
 * necessary information is passed to the Allocator on creation
 * in a parameters structure.  This includes the element size,
 * as well as three function pointers:
 *
 *   - The constructor function is called when an element is first
 *     allocated from the system.
 *   - The destructor function is called just before an element's
 *     memory is released back to the system.
 *   - The clear function is called after the application frees
 *     an element.
 *
 * Any of these may be null, in which case the corresponding call is skipped.
 *
 * An Allocator has a name, which is used both to identify it in the
 * Allocator registry when Allocators are created on demand and
 * in memory statistics reports.
 * An Allocator must support these operations:
 *
 *   - @c allocate, to allocate a new element.
 *   - @c reset, to free all allocated elements.  The memory will generally
 *     be retained by the Allocator to be reused again quickly.
 *   - @c erase, to free all allocated elements, and return all
 *     allocated memory to the system.
 *   - @c reserve, to adjust the amount of current unused memory which
 *     the Allocator keeps in its pool.  If the amount of memory requested
 *     is greater than what is currently available, the new memory will
 *     usually be allocated in a single block.  If the amount of memory
 *     requested is less than what is currently available, free blocks
 *     will be returned to the system, if possible.
 *   - @c name to return the Allocator's name, and @c stats to return
 *     the current statistics for the Allocator.
 *
 * There are some additional operations which an Allocator may optionally
 * implement:
 *
 *   - @c free, to free an individual element.
 *   - @c resetTo, to free all elements that were allocated after
 *     a given element, as well as the element itself.
 *   - An iterator, which iterates over all allocated blocks.
 *
 * Two @c Allocator implementations are currently available in the library:
 *
 *  - @c ArenaPoolAllocator: Allocates elements in a stack-like manner.
 *    Implements the @c resetTo operation and an iterator, but does
 *    not implement @c free.
 *
 *  - @c ArenaHeapAllocator: An Allocator that allows elements to be
 *    individually freed.  Implements @c free, but not @c resetTo
 *    nor an iterator.  This Allocator requires maintaining
 *    a pointer with each free element.  By default, this pointer
 *    is kept outside the element, increasing the effective size
 *    per element.  However, if part of the element may be overwritten
 *    while it is free, then the allocator may be configured
 *    to have this pointer overlap part of the element.
 *
 * @c Allocator objects are grouped into @c Arenas.  Each @c Arena
 * contains a vector of @c Allocator objects.  Each distinct @c Allocator
 * type is assigned an index into this vector; these indices are
 * globally unique.  An @c Arena is associated with a @c ArenaHeader,
 * which maintains a notion of the `current' @c Arena; the @c ArenaHeader
 * holds a reference to the @c Allocator vector of the current @c Arena.
 * An @c Arena has a @c makeCurrent operation to nominate it as the
 * current @c Arena for its @c ArenaHeader.  A helper class @c Arena::Push
 * is provided to change the current @c Arena in a stack-like manner.
 * An @c Arena also has operations to @c reset or @c erase all of its
 * @c Allocators, as well as for summing statistics over them all.
 * An @c Arena also has a name, which is used in printing statistics reports.
 *
 * The object that the application uses to allocate memory is provided
 * by the @c Handle classes.  These are templated on the type being
 * allocated as well as on the underlying @c Allocator class.
 * A @c Handle is created by passing in the @c Arena (or @c ArenaHeader)
 * with which it is created, as well as any optional creation
 * parameters for the @c Allocator.  The first time a given type
 * is seen, it is assigned an index the the @c Arena @c Allocator
 * vector.  When a @c Handle is created, it will look up the proper
 * @c Allocator instance in the current @c Arena, creating it if needed.
 * The @c Handle will then forward operations to the underlying
 * @c Allocator.  The library provides two @c Handle implementations:
 *
 *  - @c ArenaHandle: When this @c Handle is used, the element
 *    constructor/destructor are expected to be called every time
 *    an element is allocated/freed by the application.  The @c allocate
 *    method returns a @c void*; it is expected that this will then
 *    be used in a placement new.  The destructor will be called
 *    by the library when elements are freed.
 *
 *  - @ ArenaCachingHandle: This @c Handle allows `caching' already-constructed
 *    objects, such that the constructor is run only when the element's
 *    memory is first allocated from the system, and the destructor
 *    is run only when the element's memory is released back to the system.
 *    The @c allocate method thus returns an already-initialized @c T*.
 *    An optional @c clear function may be called when the element
 *    is freed by the application.
 *
 * An example of the basic usage might be something like this.
 *
 *@code
 *    class Example { ...
 *      SG::Arena m_arena;
 *      SG::ArenaCachingHandle<MyObj, SG::ArenaPoolAllocator> m_handle;
 *      ...};
 *
 *    Example::Example()
 *      // Associates with the default ArenaHeader.
 *      : m_arena ("myarena") { ...
 *      ...}
 *
 *    ret Example::execute() {  ...
 *      SG::Arena::Push push (m_arena);
 *      SG::ArenaCachingHandle<MyObj, SG::ArenaPoolAllocator> handle;
 *      MyObj* obj = handle.allocate();
 *      ...}
 *
 *    ret Example::newEvent() { ...
 *      m_arena.reset();
 *      ... }
 @endcode
 *
 * See also the unit tests for the Handle classes.
 */


#ifndef ATLALLOCATORS_ARENA_H
#define ATLALLOCATORS_ARENA_H


#include "AthAllocators/ArenaBase.h"
#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaAllocatorCreator.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include <cstdlib>
#include <string>
#include <ostream>


namespace SG {


/**
 * @brief Collection of memory allocators with a common lifetime,
 *
 * See the file-level comments for a full description.
 */
class Arena
  : public SG::ArenaBase
{
public:
  /**
   * @brief Constructor.
   * @param name The name of this @c Arena; to use in reports.
   * @param header The header with which this @c Arena is associated.
   *        If defaulted, the global default @c ArenaHeader will be used.
   */
  Arena (const std::string& name, ArenaHeader* header = 0);


  /**
   * @brief Destructor.
   */
  ~Arena();


  /**
   * @brief Return the @c ArenaHeader with which this @c Arena is associated.
   */
  ArenaHeader* header();


  /**
   * @brief Make this @c Arena the current one for its @c ArenaHeader.
   * @returns The previously current Arena.
   */
  ArenaBase* makeCurrent();


  /**
   * @brief Helper class for making @c Arena instances current
   *        in a stack-like manner.
   */
  class Push
  {
  public:

    /**
     * @brief Constructor.  Make @c a current.
     * @param a The @c Arena to make current.
     */
    Push (Arena& a);


    /**
     * @brief Destructor.  Undoes the effect of the constructor.
     */
    ~Push();


  private:
    /// The @c ArenaHeader for the stack we're managing.
    ArenaHeader* m_header;

    /// The previously-current arena.
    ArenaBase* m_prev;
  };


private:
  /// The @c ArenaHeader with which we're associated.
  ArenaHeader* m_header;
};


} // namespace SG


#endif // not ATLALLOCATORS_ARENA_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TConvertingBranchElement.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief A variant of @c TBranchElement that can call converters
 *        when reading objects in split mode.
 *
 * This class adds the ability to call converters on classes being 
 * read in split mode.  It is implemented as a derived class
 * of @c TBranchElement.  In order to use it, you must first
 * register your converters with @c TConverterRegistry
 * (and call @c TConvertingBranchElement::Initialize, but normally
 * the registry class will take care of that).
 *
 * A few notes:
 *  - The @c Initialize method will change the @c New() method of the
 *    @c TBranchElement class to point at our new method.  Thus,
 *    when we read a @c TBranchElement from a file, we'll actually
 *    make an instance of this class instead.  (@c TBranchElement
 *    instances that are created explicitly, for writing, are not
 *    affected by this.)
 *
 *  - We make some changes in the contents of the @c TBranchElement
 *    if a conversion is happening.  However, the old values are saved,
 *    so that when the branch gets written, the original values
 *    are restored.
 *
 * TODO:
 * - Alternate name in file.
 * - Emulated persistent class.
 * - Rewriting the same tree doesn't really work.
 *   In the rewritten file, we get the _new_ streamerinfo's
 *   for the classes with conversions.
 */


#ifndef TCONVERTINGBRANCHELEMENT_H
#define TCONVERTINGBRANCHELEMENT_H

#if defined(__clang__)
# pragma clang diagnostic push
# if __has_warning("-Wformat-pedantic")
#  pragma clang diagnostic ignored "-Wformat-pedantic"
# endif
#endif
#include "TBranchElement.h"
#if defined(__clang__)
# pragma clang diagnostic pop
#endif

#include <atomic>

class TVirtualConverter;

/**
 * @brief A variant of @c TBranchElement that can call converters
 *        when reading objects in split mode.
 */
class TConvertingBranchElement
#ifndef __REFLEX__
#ifndef __CLING__
  : public TBranchElement
  // Need to hide the base class from reflex and cling so that it doesn't
  // insist on generating the ClassDef functions.
#endif
#endif
{
public:
  // --- Public methods ---
  /// Constructor.
  TConvertingBranchElement();

  /// Destructor.
  virtual ~TConvertingBranchElement();

  /**
   * @brief Set up to allow for conversions in split mode.
   */
  static void Initialize();

  /**
   * @brief Read all branches into the previously-declared object.
   * @param entry The ntuple row to read.
   * @param getall If true, force reading all branches, even those
   *               previously deactivated.
   *
   * If entry = 0, then use current entry number + 1.
   * If entry < 0, then reset entry number to 0.
   *
   * Returns the number of bytes read from the input buffer.
   * If entry does not exist, then returns 0.
   * If an I/O error occurs, then returns -1.
   */
  virtual Int_t GetEntry(Long64_t entry, Int_t getall);

  // Need to hide this from cling; otherwise, genreflex will try to
  // generate an implementation for this.
#ifndef __CLING__
  /**
   * @brief Read or write this object.
   * @param R__b The Root buffer.
   */
  virtual void Streamer(TBuffer& R__b);
#endif

  /**
   * @brief Set the address of the object to use for I/O.
   * @param add Object address.
   */
  virtual void SetAddress(void* add);

  /**
   * @brief Reset branch addresses and maybe delete the object.
   */
  virtual void ResetAddress();


  /**
   * @brief Set the deletion flag.
   * @param flag If true, try to delete the branch object on branch deletion.
   */
  static void SetDoDel (bool flag);



protected:
  /**
   * @brief Initialize the TStreamerInfo pointer.
   * (Overridden internal method.)
   */
  virtual void InitInfo();

  /**
   * @brief Initialize data member offsets.
   * (Overridden internal method.)
   */
  virtual void InitializeOffsets();

  /**
   * @brief Read leaves into I/O buffers for this branch.
   * @param b Root I/O buffer.
   */
  void ReadLeavesCollectionConverting(TBuffer& b);

  void ReadLeavesMemberBranchCountConverting (TBuffer& b);


private:
  /// Flag used to mark dummy nodes created by @c BuildConvertedElisions.
  enum {
    kIsDummy      = BIT(20)
  };

  /**
   * @brief new() method for this object.
   * @param p Address for placement new, or 0.
   * @return Address of the new object.
   *
   * This is installed as the @c New method in @c TBranchElement's @c TClass.
   * Thus, when we read from a file a @c TBranchElement, we actually
   * make an instance of this class.
   */
  static void* new_TConvertingBranchElement (void* p);

  /**
   * @brief Add dummy nodes if needed to recover the correct tree structure.
   */
  void BuildConvertedElisions();

  /**
   * @brief Check to see if we need to worry about conversions for this branch.
   *        Set up member variables if so.
   */
  void CheckForConversion();

  /**
   * @brief Recursively reset the type field of containers in conversions.
   */
  void ConvResetType();

  /**
   * @branch Read in the subbranches of this branch.
   * @param entry The entry number to read.
   * @param getall If true, read all branches, even if disabled.
   * @param dont_reset If true, don't reset the @c TBuffer read pointer.
   * @return The number of bytes read.
   */
  Int_t ReadSubBranches(Long64_t entry,
                        Int_t getall,
                        bool dont_reset);

  TVirtualConverter *fConv;       //! Conversion for this branch.
  TClass            *fConvClass;  //! Class for conversion.
  char              *fConvObject; //! Pointer to tmp obj used for conversion.
  TObjArray         *fConvOrigBranches; //! Saved branch list.  If we change
                                        //  the branch list, we store the
                                        //  original one here, so that
                                        //  when we stream, we use the
                                        //  original.
  Int_t              fConvOrigType;     //! Saved branch type.  The original
                                        //  type is stored here, so that if
                                        //  we stream, we can save the original
                                        //  type.  This is set to -1 before
                                        //  it's initialized.
  bool               fConvContainerFlag; //! True if we're doing a container
                                         //  conversion.
  bool               fConvDontReset;     //! Flag that the next read should
                                         //  not reset the buffer pointer.
                                         //  See GetEntry().
  static std::atomic<bool> fgDoDel;      //! If true, try to delete the
                                         //  branch object when the branch
                                         //  is deleted.


  TConvertingBranchElement& operator= (const TConvertingBranchElement&);
  TConvertingBranchElement (const TConvertingBranchElement&);
};

#endif


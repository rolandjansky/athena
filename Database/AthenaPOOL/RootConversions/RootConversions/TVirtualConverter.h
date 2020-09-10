// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TVirtualConverter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Base class for Root converters.
 *
 * This header defines the base class @c TVirtualConverter,
 * to be used for conversions between different versions
 * of a class when reading data.
 *
 * We need to keep track of a couple different classes here.
 * The `transient' class is the class that we want to read.
 * It must have the same name in the persistent store
 * (this restriction may be removed later).
 * The `shape' of the persistent version of the class may,
 * however, differ from the transient version.  We thus
 * identify a second, `persistent' class which gives the shape
 * of the class that's expected in the persistent store.
 * (In the future, we may remove the limitation that this be
 * an actual class known to root and allow root `emulated' classes
 * to be used.)
 * The converter must then know how to initialize a transient
 * object from a persistent one.
 *
 * Three pieces of information are needed to properly register the converter.
 * First, the name of the transient class.
 * Second, the name of the persistent class.
 * Third, the root checksum for the persistent class shape we want
 * to read.  If this is not supplied, then one will be calculated
 * based on the name of the transient class and the contents of the
 * persistent class.  This is usually what's needed; however, if you're
 * relying on automatic schema evolution to read the persistent class
 * (i.e., what you've declared in your program isn't exactly the
 * same as the stored class), then you'll need to specify the checksum
 * explicitly.
 *
 * To make this a little more concrete, say you have:
 *
 *@code
 *  struct T { int a; };
 @endcode
 *
 * that you've saved to root.  You later change this to:
 *
 *@code
 *  struct T { double a2; };
 @endcode
 *
 * Now, root's automatic schema evolution won't be able to read this
 * properly, because it doesn't have any way to connect a2 to a.
 * You can still read the data with a converter.  Make an additional
 * class
 *
 *@code
 *  struct T_old { int a; };
 @endcode
 *
 * Then make a converter class.  The easiest thing to do is to use
 * the template class @c TVirtualConverter_T:
 *
 *@code
 *  class T_Conv
 *    : public TVirtualConverter_T<T, T_old>
 *  {
 *  public:
 *    void Convert (T* transobj, const T_old* persobj)
 *    { transobj->a2 = persobj->a; }
 *  };
 @endcode
 *
 * Here, the class names and the checksum are all provided by default.
 * If you need to override them, they can be specified to the base class
 * constructor.
 *
 * In order to perform the conversion, a temporary instance of the
 * persistent object must be created.  By default, this is done
 * via @c TClass::New, and then later destroyed using @c TClass::Destructor.
 * This can be changed by overriding the @c NewPersObj and @c DeletePersObjs
 * methods of @c TVirtualConverter.
 *
 * Once a converter instance has been created, it should be registered
 * with @c TConverterRegistry.
 */

#ifndef TVIRTUALCONVERTER_H
#define TVIRTUALCONVERTER_H

#include <string>
#include <typeinfo>
#include "Rtypes.h"

class TClass;
class TBuffer;


/**
 * @brief Base class for converters for Root schema evolution.
 *
 * Derive from this and define the @c Convert method to do the conversion.
 * You must specify both the transient and persistent classes
 * in the constructor.  The checksum must also be specified if the
 * actual shape in the file is different than the provided persistent class.
 */
class TVirtualConverter
{
public:
  /**
   * @brief Constructor, with checksum and class pointers.
   * @param checksum   The Root checksum of the persistent class we'll convert.
   * @param trans_cls  The transient class to which we will be converting.
   * @param pers_cls   The persistent class from which we will be converting.
   */
  TVirtualConverter (UInt_t checksum,
                     TClass* trans_cls,
                     TClass* pers_cls);

  /**
   * @brief Constructor, with class pointers.
   * @param trans_cls  The transient class to which we will be converting.
   * @param pers_cls   The persistent class from which we will be converting.
   *
   * The checksum will be calculated automatically from the provided classes.
   */
  TVirtualConverter (TClass* trans_cls,
                     TClass* pers_cls);

  /**
   * @brief Constructor, with checksum and class names.
   * @param checksum   The Root checksum of the persistent class we'll convert.
   * @param trans_cls  The name of the transient class to which
   *                   we will be converting.
   * @param pers_cls   The name of the persistent class from which
   *                   we will be converting.
   */
  TVirtualConverter (UInt_t checksum,
                     const std::string& trans_cls,
                     const std::string& pers_cls);

  /**
   * @brief Constructor, with class names.
   * @param trans_cls  The name of the transient class to which
   *                   we will be converting.
   * @param pers_cls   The name of the persistent class from which
   *                   we will be converting.
   *
   * The checksum will be calculated automatically from the provided classes.
   */
  TVirtualConverter (const std::string& trans_cls,
                     const std::string& pers_cls);

  /**
   * @brief Destructor.
   */
  virtual ~TVirtualConverter();

  /**
   * @brief Return the transient class, to which we're converting.
   */
  TClass* GetTransClass ();

  /**
   * @brief Return the persistent class, from which we're converting.
   */
  TClass* GetPersClass();

  /**
   * @brief Return the checksum of the class from which we're converting.
   */
  int GetCheckSum() const;

  /**
   * @brief Create a new instance of the persistent class.
   *
   * The provided implementation of this uses @c TClass::New, but
   * you can override it if need be.
   */
  virtual void* NewPersObj ();

  /**
   * @brief Destroy an instance of the persistent class.
   * @param persobj Pointer to the instance to destroy.
   *
   * The provided implementation of this uses @c TClass::Destructor, but
   * you can override it if need be.
   */
  virtual void DeletePersObj (void* persobj);
  
  /**
   * @brief Do the conversion.
   * @param transobj Pointer to an instance of the transient class.
   * @param persobj Pointer to an instance of the persistent class.
   *
   * An implementation of this must be provided in derived classes.
   * It should initialize the object at @c transobj from the
   * one at @c persobj.
   */
  virtual void ConvertVoid (void* transobj, const void* persobj) = 0;

  /**
   * @brief Read object in non-split mode.
   * @param b       The source of the data.
   * @param obj     The object into which we're reading.
   *                (This will be the transient object.)
   * @param version The version number of the class.
   * @param R__s    The starting position in the buffer.
   * @param R__c    The number of bytes for this object in the buffer.
   *
   * You normally shouldn't have to worry about this, but it's exposed
   * here so that it can be overridden if needed.
   * It's only used when reading in non-split mode.
   */
  virtual void ReadBuffer(TBuffer& b, void* obj, Version_t version,
                          UInt_t R__S, UInt_t R__c);

  /**
   * @brief Helper to convert a class name to a Root class pointer.
   * @param name  The name of the class to convert.
   *
   * Returns 0 on failure.
   */
  static TClass* ToClass (const std::string& name);

  /**
   * @brief Helper to convert a @c type_info to a Root class pointer.
   * @param name  The @c type_info of the class to convert.
   *
   * Returns 0 on failure.
   */
  static TClass* ToClass (const std::type_info& id);


protected:
  // For compatibility with @c T_AthenaRootConverter only.
  // Allows specifying the constructor arguments after the constructor
  // itself is called.
  TVirtualConverter ();
  void SetCtorInfo (UInt_t checksum,
                    TClass* trans_cls,
                    TClass* pers_cls);


private:
  /// Helper to allow for automatic schema evolution for persistent classes.
  void CheckStreamInfos();

  /// Helper to check that the classes have been defined properly.
  void CheckClasses();

  /// The declared transient class.
  TClass* fTransClass;

  /// The declared persistent class.
  TClass* fPersClass;

  /// The declared checksum.
  UInt_t fCheckSum;

  /// Used by @c CheckStreamInfos: the length of the streamerinfo list
  /// that we saw on the last @c CheckStreamInfos call.
  int fOldLast;


protected:
  /// The temporary persistent object instance, used for conversions
  /// (in non-split mode).
  void* fPersObj;
};


/**
 * @brief Easy-to-use templated class for defining converters.
 *
 * The classes for the conversion are given by the @c TTrans
 * and @c TPers template arguments.  In the simple case, you shouldn't
 * need to give any arguments to the constructor.
 */
template <class TTrans, class TPers>
class TVirtualConverter_T
  : public TVirtualConverter
{
public:
  /**
   * @brief Constructor, with class pointers.
   * @param trans_cls  The transient class to which we will be converting.
   * @param pers_cls   The persistent class from which we will be converting.
   *
   * The checksum will be calculated automatically from the provided classes.
   *
   * If either of the class pointers is defaulted to 0, the class pointer
   * will be computed from the appropriate template parameter.
   */
  TVirtualConverter_T (TClass* trans_cls = 0,
                       TClass* pers_cls = 0);

  /**
   * @brief Constructor, with checksum and class pointers.
   * @param checksum   The Root checksum of the persistent class we'll convert.
   * @param trans_cls  The transient class to which we will be converting.
   * @param pers_cls   The persistent class from which we will be converting.
   *
   * If either of the class pointers is defaulted to 0, the class pointer
   * will be computed from the appropriate template parameter.
   */
  TVirtualConverter_T (UInt_t checksum,
                       TClass* trans_cls = 0,
                       TClass* pers_cls = 0);

  /**
   * @brief Do the conversion.
   * @param transobj Pointer to an instance of the transient class.
   * @param persobj Pointer to an instance of the persistent class.
   *
   * This implementation simply calls the @c Convert below
   * with the appropriate casting.
   */
  virtual void ConvertVoid (void* transobj, const void* persobj);

  /**
   * @brief Do the conversion.
   * @param transobj Pointer to an instance of the transient class.
   * @param persobj Pointer to an instance of the persistent class.
   *
   * An implementation of this must be provided in derived classes.
   * It should initialize the object at @c transobj from the
   * one at @c persobj.
   */
  virtual void Convert (TTrans* transobj, const TPers* persobj) = 0;
};


#include "RootConversions/TVirtualConverter.icc"


#endif // not TVIRTUALCONVERTER_H

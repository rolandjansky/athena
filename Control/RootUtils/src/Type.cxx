/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RootUtils/src/Type.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Wrapper for ROOT types.
 */


#include "RootUtils/Type.h"
#include "TError.h"
#include "TROOT.h"
#include "TInterpreter.h"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <cassert>


namespace RootUtils {


/**
 * @brief Wrapper for ROOT types.
 *
 * Using the ROOT interfaces to manipulate objects of generic type
 * is rather awkward, because you have do to completely different things
 * depending on whether the type of object you're dealing with is a class
 * or a fundamental type.  This wrapper allows treating these two cases
 * in a uniform way.  For now, we don't try to handle other kinds of types
 * (pointers, etc).
 */
Type::Type ()
  : m_cls(0),
    m_type(kNoType_t),
    m_ti(0),
    m_size(0),
    m_assign(new TMethodCall()),
    m_assignInitialized(false),
    m_tsAssign(TMCDeleter::destroy),
    m_defElt(0)
{
}


/**
 * @brief Construct for a class type.
 * @param cls The class of the object on which we're operating.
 */
Type::Type (TClass* cls)
  : m_cls(0),
    m_type(kNoType_t),
    m_ti(0),
    m_size(0),
    m_assign(new TMethodCall()),
    m_assignInitialized(false),
    m_tsAssign(TMCDeleter::destroy),
    m_defElt(0)
{
  init (cls);
}


/**
 * @brief Construct for a fundamental type.
 * @param type The ROOT type code.
 */
Type::Type (EDataType type)
  : m_cls(0),
    m_type(kNoType_t),
    m_ti(0),
    m_size(0),
    m_assign(new TMethodCall()),
    m_assignInitialized(false),
    m_tsAssign(TMCDeleter::destroy),
    m_defElt(0)
{
  init (type);
}


/**
 * @brief Construct from a type name.
 * @param typname The name of the type.
 *
 * @c typname may be either the name of a fundamental type,
 * or the name of a class known to ROOT.  An exception will be
 * thrown if the name is not recognized.
 */
Type::Type (const std::string& typname)
  : m_cls(0),
    m_type(kNoType_t),
    m_ti(0),
    m_size(0),
    m_assign(new TMethodCall()),
    m_assignInitialized(false),
    m_tsAssign(TMCDeleter::destroy),
    m_defElt(0)
{
  init (typname);
}


/**
 * @brief Copy constructor.
 * @param other Object to be copied.
 */
Type::Type (const Type& other)
  : m_cls (other.m_cls),
    m_type (other.m_type),
    m_ti (other.m_ti),
    m_size (other.m_size),
    m_assign (std::unique_ptr<TMethodCall, TMCDeleter>(new TMethodCall(*(other.m_assign)))),
    m_assignInitialized (false),
    m_tsAssign(TMCDeleter::destroy),
    m_defElt (0)
{
  // Don't copy m_tsAssign.

  if (m_cls)
    m_defElt = create();
}


/**
 * @brief Assignment.
 * @param other Object to be copied.
 */
Type& Type::operator= (const Type& other)
{
  if (this != &other) {
    m_cls = other.m_cls;
    m_type = other.m_type;
    m_ti = other.m_ti;
    m_size = other.m_size;
    *m_assign = *(other.m_assign);
    m_assignInitialized = false;
    if (m_defElt)
      destroy (m_defElt);
    if (m_cls)
      m_defElt = create();
    else
      m_defElt = 0;

    // Don't copy m_tsAssign.
    // Before we call using it, we'll check that the function still matches
    // m_assign.
  }
  return *this;
}


/**
 * @brief Destructor.
 */
Type::~Type()
{
  if (m_defElt)
    destroy (m_defElt);
}


/**
 * @brief Initialize from a type name.
 * @param typname The name of the type.
 *
 * The @c Type object must have been just default-constructed.
 *
 * @c typname may be either the name of a fundamental type,
 * or the name of a class known to ROOT.  An exception will be
 * thrown if the name is not recognized.
 */
void Type::init (const std::string& typname)
{
  // Make sure root's table of types is initialized.
  // May be needed for root 6.
  gROOT->GetListOfTypes();

  assert (m_cls == 0 && m_type == kNoType_t);

  TClass* cls = TClass::GetClass (typname.c_str());
  if (cls) {
    init (cls);
    return;
  }

  for (int i = 0; i < kNumDataTypes; ++i) {
    EDataType type = static_cast<EDataType>(i);
    TDataType* dt = TDataType::GetDataType(type);
    if (dt && typname == dt->GetTypeName()) {
      init (type);
      return;
    }
  }

  EDataType type = kNoType_t;
  // Check for some other synonyms.
  if (typname == "uint32_t") {
    if (sizeof (unsigned int) == 4)
      type = kUInt_t;
  }
  else if (typname == "int32_t") {
    if (sizeof (unsigned int) == 4)
      type = kInt_t;
  }
  else if (typname == "uint16_t") {
    if (sizeof (unsigned short) == 2)
      type = kUShort_t;
  }
  else if (typname == "int16_t") {
    if (sizeof (short) == 2)
      type = kShort_t;
  }
  else if (typname == "uint8_t") {
    type = kUChar_t;
  }
  else if (typname == "int8_t") {
    type = kChar_t;
  }
  else if (typname == "uint64_t") {
    if (sizeof(unsigned long) == 8)
      type = kULong_t;
    else if (sizeof(unsigned long long) == 8)
      type = kULong64_t;

  }
  else if (typname == "int64_t") {
    if (sizeof(long) == 8)
      type = kLong_t;
    else if (sizeof(long long) == 8)
      type = kLong64_t;
  }

  if (type != kNoType_t) {
    init (type);
    return;
  }

  throw std::runtime_error (std::string ("RootUtils::Type: Can't find type `") +
                            typname + "'.");
}


/**
 * @brief Initialize for a class type.
 * @param cls The class of the object on which we're operating.
 *
 * The @c Type object must have been just default-constructed.
 */
void Type::init (TClass* cls)
{
  assert (m_cls == 0 && m_type == kNoType_t);
  m_cls = cls;
  m_type = kNoType_t;
  m_size = cls->Size();
  m_defElt = create();
  m_ti = cls->GetTypeInfo();
}


/**
 * @brief Initialize for a fundamental data type..
 * @param type The ROOT type code.
 *
 * The @c Type object must have been just default-constructed.
 */
void Type::init (EDataType type)
{
  // Make sure root's table of types is initialized.
  // May be needed for root 6.
  gROOT->GetListOfTypes();

  assert (m_cls == 0 && m_type == kNoType_t);
  TDataType* dt = TDataType::GetDataType (type);
  if (!dt) {
    throw std::runtime_error ("Unknown data type");
  }

  m_cls = 0;
  m_type = type;
  m_size = dt->Size();
  m_defElt = 0;

  switch (type) {
#define TYPE(CODE, TYP, IOTYP) case CODE: m_ti = &typeid(TYP); break
#include "Types.def"
#undef TYPE
  default: break;
  }
}


/**
 * @brief Create an instance of the object.
 *
 * Class objects will be default-constructed; fundamental types
 * will be zero-filled.
 */
void* Type::create() const
{
  if (m_cls)
    return m_cls->New();
  void* p = new char[m_size];
  clear (p);
  return p;
}


/**
 * @brief Destroy an instance of the object (and free memory).
 * @param p Pointer to the object to be destroyed and freed.
 */
void Type::destroy (void* p) const
{
  if (p) {
    if (m_cls)
      m_cls->Destructor (p);
    else
      delete [] (reinterpret_cast<char*> (p));
  }
}


/**
 * @brief Return the name of this type.
 */
std::string Type::getTypeName() const
{
  if (m_type != kNoType_t)
    return TDataType::GetTypeName (m_type);

  if (m_cls)
    return m_cls->GetName();

  return "";
}


/**
 * @brief Return the ROOT class for the described type.
 *
 * Returns 0 if this is for a fundamental type.
 */
TClass* Type::getClass() const
{
  return m_cls;
}


/**
 * @brief Return the ROOT data type code for the described type.
 *
 * Returns @c kNoType_t if this is for a class type.
 */
EDataType Type::getDataType() const
{
  return m_type;
}


/**
 * @brief Return the @c type_info for the described type.
 */
const std::type_info* Type::getTypeInfo() const
{
  return m_ti;
}


/**
 * @brief Return the size in bytes of an instance of the described type.
 */
size_t Type::getSize() const
{
  return m_size;
}


/**
 * @brief Copy a range of objects.
 * @param dst Pointer to the start of the first object for the destination.
 * @param src Pointer to the start of the first object for the copy source.
 * @param n Numer of objects to copy.
 *
 * This function properly handles overlapping ranges.
 */
void Type::copyRange (void* dst, const void* src, size_t n) const
{
  if (m_cls) {
    if (dst > src && reinterpret_cast<unsigned long>(src) + n*m_size > reinterpret_cast<unsigned long>(dst))
    {
      for (size_t i = n-1; i < n; --i) {
        assign (dst, i, src, i);
      }
    }
    else
    {
      for (size_t i = 0; i < n; ++i) {
        assign (dst, i, src, i);
      }
    }
  }
  else {
    memmove (dst, src, n * m_size);
  }
}


/**
 * @brief Clear a range of objects.
 * @param dst Pointer to the start of the first object to clear.
 * @param n Number of objects to clear.
 *
 * Objects of class type are cleared by assigning to them
 * a default-constructed object.  Other objects types are cleared
 * by filling with 0.
 */
void Type::clearRange (void* dst, size_t n) const
{
  if (m_cls) {
    for (size_t i = 0; i < n; ++i) {
      clear (dst, i);
    }
  }
  else {
    memset (dst, 0, n * m_size);
  }
}


/**
 * @brief Clear an object in a vector.
 * @param dst Pointer to the start of the vector's data.
 * @param index Index of the object in the vector.
 */
void Type::clear (void* dst, size_t index) const
{
  dst = reinterpret_cast<void*>(reinterpret_cast<unsigned long>(dst) + m_size* index);
  assign (dst, m_defElt);
}


/**
 * @brief Clear an object.
 * @param dst Pointer to the object.
 */
void Type::clear (void* dst) const
{
  assign (dst, m_defElt);
}


/**
 * @brief Copy an object within vectors.
 * @param dst Pointer to the start of the destination vector's data.
 * @param dst_index Index of destination object in the vector.
 * @param src Pointer to the start of the source vector's data.
 * @param src_index Index of source object in the vector.
 *
 * @c dst and @ src can be either the same or different.
 */
void Type::assign (void* dst,        size_t dst_index,
                   const void* src,  size_t src_index) const
{
  dst = reinterpret_cast<void*>(reinterpret_cast<unsigned long>(dst) + m_size* dst_index);
  src = reinterpret_cast<const void*>(reinterpret_cast<unsigned long>(src) + m_size * src_index);
  assign (dst, src);
}


/**
 * @brief Copy an object.
 * @param dst Destination for the copy.
 * @param src Source for the copy.
 *
 * The copy will be done using either @c m_tsAssign or @c memcpy,
 * depending on whether or not the object has class type.
 * If the payload does not have class type and @c src is null,
 * then the destination element will be filled with 0's.
 */
void Type::assign (void* dst, const void* src) const
{
  if (m_cls && checkAssign()) {
    m_tsAssign->ResetParam();
    m_tsAssign->SetParam (reinterpret_cast<Long_t>(src));
    m_tsAssign->Execute (dst);
  }
  else {
    if (src)
      memcpy (dst, src, m_size);
    else
      memset (dst, 0, m_size);
  }
}


/**
 * @brief Swap an object between vectors.
 * @param a Pointer to the start of the first vector's data.
 * @param aindex Index of the object in the first vector.
 * @param b Pointer to the start of the second vector's data.
 * @param bindex Index of the object in the second vector.
 *
 * @c a and @ b can be either the same or different.
 */
void Type::swap (void* a, size_t a_index,
                 void* b, size_t b_index) const
{
  a = reinterpret_cast<void*>(reinterpret_cast<unsigned long>(a) + m_size*a_index);
  b = reinterpret_cast<void*>(reinterpret_cast<unsigned long>(b) + m_size*b_index);
  swap (a, b);
}


/**
 * @brief Swap two objects.
 * @param a Pointer to the first object.
 * @param b Pointer to the second object.
 */
void Type::swap (void* a, void* b) const
{
  if (m_cls && m_assign->IsValid()) {
    void* tmp = create();
    assign (tmp, a);
    assign (a, b);
    assign (b, tmp);
    destroy (tmp);
  }
  else {
    std::vector<char> tmp (m_size);
    memcpy (tmp.data(), a, m_size);
    memcpy (a, b, m_size);
    memcpy (b, tmp.data(), m_size);
  }
}


/**
 * @brief Initialize an object from a string.
 * @param p Pointer to the object to initialize.
 * @param s String from which to initialize.
 *
 * Only works for basic types and `std::string`.
 */
void Type::fromString (void* p, const std::string& s) const
{
  std::istringstream is (s);
  switch (m_type) {
#define TYPE(CODE, TYP, IOTYP) case CODE: { IOTYP tmp=0; is >> tmp; *reinterpret_cast<TYP*>(p) = tmp; } return
#include "Types.def"
#undef TYPE
  default: break;
  }

  if (m_cls == TClass::GetClass ("string")) {
    *reinterpret_cast<std::string*>(p) = s;
    return;
  }

  throw std::runtime_error
    (std::string ("RootUtils::Type::fromString: Can't convert objects of type `" +
                  getTypeName() + "'."));
}


/**
 * @brief See if @c m_assign is initialized.
 * If not, try to initialize it now,  and copy to the thread-specific variable.
 * Returns true on success.
 */
bool Type::checkAssign() const
{
  // Fail if this isn't a class type.
  if (m_cls == 0) return false;

  if (!m_assignInitialized) {
    // Not initialized ... try to do so now.  First take the lock.
    std::lock_guard<std::mutex> lock (m_assignMutex);
    if (!m_assignInitialized) {
      std::string proto = "const ";
      proto += m_cls->GetName();
      proto += "&";
      m_assign->InitWithPrototype (m_cls, "operator=", proto.c_str());
      if (!m_assign->IsValid()) {
        ::Warning ("RootUtils::Type",
                   "Can't get assignent op for type `%s'.",
                   m_cls->GetName());
      }
      m_assignInitialized = true;
    }
  }

  if (!m_assign->IsValid()) return false;

  if (m_tsAssign.get() == 0 || m_tsAssign->GetMethod() != m_assign->GetMethod())
    m_tsAssign.reset (new TMethodCall (*m_assign));

  return true;
}


void Type::TMCDeleter::destroy(TMethodCall* ptr)
{
  // Don't try to run the TMethodCall destructor if gCling is gone.
  TInterpreter* cling = gCling;
  if (cling) {
    delete ptr;
  }
}


void Type::TMCDeleter::operator() (void* ptr)
{
  // Use the static function for the heavy lifting.
  destroy(static_cast<TMethodCall*>(ptr));
}


} // namespace RootUtils

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//  Package    : pool/StorageSvc (The POOL project)
//
//  Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTDB_DBARRAY_H
#define POOL_ROOTDB_DBARRAY_H

/*
 *  pool namespace declaration
 */
namespace pool {
  /** @class DbArray DbArray.h StorageSvc/DbArray.h
    *
    * Class describing arbitrary arrays
    *
    * @author  M.Frank
    * @date    1/8/2002
    * @version 1.0
    */
  template<class T> class DbArray  {
  private:
    /// Objects of this type cannot be copied
    DbArray(const DbArray&): m_size(0), m_buffer(0){         }
  public:
    /// Size of buffer
    int    m_size;
    /// Buffer with object content
    T     *m_buffer;

    /// Standard Constructor
    DbArray() : m_size(0), m_buffer(0)  {                  }
    /// Standard Destructor
    virtual ~DbArray()                  {                  }
    /// Allocate an object buffer
    void allocate(int siz, bool do_delete)  {
      if ( do_delete ) delete [] m_buffer;
      m_buffer = new T[siz];
      m_size = siz;
    }
    /// Release data buffer with or without delete
    void release(bool do_delete)      {
      if ( do_delete ) delete [] m_buffer;
      m_buffer    = 0;
      m_size      = 0;
    }
  };
}
struct CharDbArray : public pool::DbArray<char> { public: CharDbArray() {}; virtual ~CharDbArray() {} }; 
// Avoid name collision with UCharDbArray in RootCnv.
struct UCharDbArrayAthena : public pool::DbArray<unsigned char> { public: UCharDbArrayAthena() {}; virtual ~UCharDbArrayAthena() {} }; 
struct ShortDbArray : public pool::DbArray<short> { public: ShortDbArray() {}; virtual ~ShortDbArray() {} }; 
struct UShortDbArray : public pool::DbArray<unsigned short> { public: UShortDbArray() {}; virtual ~UShortDbArray() {} }; 
struct IntDbArray : public pool::DbArray<int> { public: IntDbArray() {}; virtual ~IntDbArray() {} }; 
struct UIntDbArray : public pool::DbArray<unsigned int> { public: UIntDbArray() {}; virtual ~UIntDbArray() {} }; 
struct LongDbArray : public pool::DbArray<long> { public: LongDbArray() {}; virtual ~LongDbArray() {} }; 
struct ULongDbArray : public pool::DbArray<unsigned long> { public: ULongDbArray() {}; virtual ~ULongDbArray() {} }; 
struct FloatDbArray : public pool::DbArray<float> { public: FloatDbArray() {}; virtual ~FloatDbArray() {} }; 
struct DoubleDbArray : public pool::DbArray<double> { public: DoubleDbArray() {}; virtual ~DoubleDbArray() {} }; 

#endif // POOL_ROOTDB_DbArray_H

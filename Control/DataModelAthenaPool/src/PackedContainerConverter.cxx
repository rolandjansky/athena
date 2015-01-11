/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelAthenaPool/src/PackedContainerConverter.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Allow converting std::vector to SG::PackedContainer.
 */


#include "DataModelAthenaPool/PackedContainerConverter.h"
#include "AthContainers/PackedContainer.h"
#include "RootConversions/TConverterRegistry.h"
#include "TMemberStreamer.h"
#include "TClass.h"
#include "TROOT.h"
#include <vector>
#include <string>
#include <cassert>


namespace DataModelAthenaPool {


/**
 * @brief Move from one vector to another.
 * @param src Source vector.  Destroyed by the move.
 * @param dst Destination vector.
 *
 * If the types of @c src and @c dst are the same, then just do a move.
 * Otherwise, copy.
 */
template <class T>
void vectorMove (std::vector<T>& src, std::vector<T>& dst)
{
  dst.swap (src);
}
template <class T, class U>
void vectorMove (std::vector<T>& src, std::vector<U>& dst)
{
  dst.assign (src.begin(), src.end());
}


/**
 * @brief Converter from @c std::vector<T> to @c SG::PackedContainer<U>.
 */
template <class T, class U=T>
class PackedContainerConverter
  : public TMemberStreamer
{
public:
  /**
   * @brief Constructor.
   * @param tname The name of the vector element type T.
   */
  PackedContainerConverter (const char* tname);


  /**
   * @brief Run the streamer.
   * @param b Buffer from which to read.
   * @param pmember Pointer to the object into which to read.
   * @param size Number of instances to read.
   */
  virtual void operator() (TBuffer& b, void* pmember, Int_t size=0);


private:
  /// Hold the source class.
  TClass* m_cl;
};


/**
 * @brief Constructor.
 * @param tname The name of the vector element type T.
 */
template <class T, class U>
PackedContainerConverter<T, U>::PackedContainerConverter (const char* tname)
{
  std::string vname = "vector<";
  vname += tname;
  if (vname[vname.size()-1] == '>')
    vname += ' ';
  vname += '>';
  m_cl = gROOT->GetClass (vname.c_str());
}


/**
 * @brief Run the streamer.
 * @param b Buffer from which to read.
 * @param pmember Pointer to the object into which to read.
 * @param size Number of instances to read.
 */
template <class T, class U>
void PackedContainerConverter<T, U>::operator() (TBuffer& b,
                                                 void* pmember, 
                                                 Int_t size /*=0*/)
{
  // This only works for reading!
  assert (b.IsReading());

  // The transient object.
  SG::PackedContainer<U>* obj =
    reinterpret_cast<SG::PackedContainer<U>*> (pmember);

  // We'll read into this object.
  std::vector<T> tmp;

  while (size-- > 0) {
    // Read into tmp and move data to *obj.
    tmp.clear();
    m_cl->Streamer (&tmp, b);
    vectorMove (tmp, *obj);
    ++obj;
  }
}


/**
 * @brief Install converters for supported instantiations.
 */
void installPackedContainerConverters()
{
#define CONVERTER(SRC, DST)                                     \
  do {                                                          \
    TConverterRegistry::Instance()->AddStreamerConverter        \
      ("vector<" #SRC ">", "SG::PackedContainer<" #DST ">",     \
       new PackedContainerConverter<SRC, DST> ( #SRC));         \
  } while (0)

#define CONVERTER1(T) CONVERTER(T,T)

  CONVERTER1(char);
  CONVERTER1(unsigned char);
  CONVERTER1(short);
  CONVERTER1(unsigned short);
  CONVERTER1(int);
  CONVERTER1(unsigned int);
  CONVERTER1(float);
  CONVERTER1(double);

  CONVERTER1(std::vector<char>);
  CONVERTER1(std::vector<unsigned char>);
  CONVERTER1(std::vector<short>);
  CONVERTER1(std::vector<unsigned short>);
  CONVERTER1(std::vector<int>);
  CONVERTER1(std::vector<unsigned int>);
  CONVERTER1(std::vector<float>);
  CONVERTER1(std::vector<double>);

  CONVERTER1(std::vector<std::vector<char> >);
  CONVERTER1(std::vector<std::vector<unsigned char> >);
  CONVERTER1(std::vector<std::vector<short> >);
  CONVERTER1(std::vector<std::vector<unsigned short> >);
  CONVERTER1(std::vector<std::vector<int> >);
  CONVERTER1(std::vector<std::vector<unsigned int> >);
  CONVERTER1(std::vector<std::vector<float> >);
  CONVERTER1(std::vector<std::vector<double> >);
}


} // namespace DataModelAthenaPool

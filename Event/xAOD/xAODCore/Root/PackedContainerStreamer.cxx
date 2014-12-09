/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODCore/Root/PackedContainerStreamer.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Streamer code for I/O with SG::PackedContainer.
 */


#include "AthContainers/PackedContainer.h"
#include "AthContainers/PackedConverter.h"
#include "AthContainers/ClassName.h"
#include "TClassStreamer.h"
#include "TMemberStreamer.h"
#include "TError.h"
#include "TROOT.h"
#include <cassert>


namespace xAOD {


/**
 * @brief Write a set of packed parameters to a buffer.
 * @param b Buffer to which to write.
 * @param param Parameters to write.
 */
inline
void writePackedParameters( TBuffer& b, const SG::PackedParameters& parms ) {
   b << parms.nbits();
   b << parms.flags();
   if (parms.isFloat()) {
      b << parms.nmantissa();
      if (parms.hasScale())
         b << parms.scale();
   }
}


/**
 * @brief Read a set of packed parameters from a buffer.
 * @param b Buffer from which to write.
 */
inline
SG::PackedParameters readPackedParameters( TBuffer& b ) {
   uint8_t nbits;
   b >> nbits;

   uint8_t flags;
   b >> flags;

   SG::PackedParameters parms (nbits, flags);
   if (parms.isFloat()) {
      uint8_t nmantissa;
      b >> nmantissa;
      parms.setNmantissa (nmantissa);
      if (parms.hasScale()) {
         float scale;
         b >> scale;
         parms.setScale (scale);
      }
   }
   return parms;
}


/**
 * @brief Streamer for reading/writing SG::PackedContainer instances.
 */
template <class T>
class PackedContainerStreamer
  : public TClassStreamer
{
public:
  /**
   * @brief Constructor.
   */
  PackedContainerStreamer();


  /**
   * @brief Run the streamer.
   * @param b Buffer from/to which to read/write.
   * @param objp Object instance.
   */
  virtual void operator()(TBuffer &b, void *objp);


private:
  /// Name of the class we read/write (for error messages).
  std::string m_className;
};


/**
 * @brief constructor.
 */
template <class T>
PackedContainerStreamer<T>::PackedContainerStreamer()
  : m_className( ClassName< SG::PackedContainer< T > >::name() )
{
}


/**
 * @brief Run the streamer.
 * @param b Buffer from/to which to read/write.
 * @param objp Object instance.
 */
template <class T>
void PackedContainerStreamer<T>::operator() ( TBuffer& b, void* objp ) {
   SG::PackedContainer<T>* cont =
     reinterpret_cast<SG::PackedContainer<T>*> (objp);

   if (b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = b.ReadVersion(&R__s, &R__c);
      if (R__v != 1) {
         Error ("PackedContainerStreamer",
                "Bad version %d for object of type %s (expected 1)",
                R__v, m_className.c_str());
      }

      cont->setParms (readPackedParameters (b));

      uint32_t nelt;
      b >> nelt;

      SG::PackedConverter cnv (cont->parms());
      cnv.read (nelt, *cont, b);
    
      b.CheckByteCount(R__s, R__c, m_className.c_str());
   }
   else {
      UInt_t R__c = b.Length();
      b.SetBufferOffset (R__c + sizeof(UInt_t));
      b << Version_t(1);
      writePackedParameters (b, cont->parms());

      uint32_t nelt = cont->size();
      b << nelt;

      SG::PackedConverter cnv (cont->parms());
      cnv.write (cont->size(), *cont, b);

      b.SetByteCount(R__c, kTRUE);
   }
}


/**
 * @brief Install streamers for supported instantiations.
 */
class InstallPackedContainerStreamer
{
public:
  InstallPackedContainerStreamer();
};


InstallPackedContainerStreamer::InstallPackedContainerStreamer()
{
  ROOT::GetROOT();

#define STREAMER(TYPE)                                                  \
  do {                                                                  \
    TClass* cl = TClass::GetClass ("SG::PackedContainer<" #TYPE " >");  \
    if (cl) {                                                           \
      cl->AdoptStreamer (new PackedContainerStreamer<TYPE>);            \
    }                                                                   \
  } while (0)

  STREAMER(char);
  STREAMER(unsigned char);
  STREAMER(short);
  STREAMER(unsigned short);
  STREAMER(int);
  STREAMER(unsigned int);
  STREAMER(float);
  STREAMER(double);

  // A couple definitions are commented out here.  This is because
  // some xAOD packages generate dictionaries for vectors of those types.
  // This leads to a circular library dependency at runtime.
  //   (via the xAOD packages calling AddDVProxy on their DV types which
  //    in turn tries to load the dummy DV type from this library).
  // These can be restored once the xAOD packages are cleaned up.

  STREAMER(std::vector<char>);
  //STREAMER(std::vector<unsigned char>);
  STREAMER(std::vector<short>);
  //STREAMER(std::vector<unsigned short>);
  STREAMER(std::vector<int>);
  STREAMER(std::vector<unsigned int>);
  STREAMER(std::vector<float>);
  STREAMER(std::vector<double>);

  STREAMER(std::vector<std::vector<char> >);
  STREAMER(std::vector<std::vector<unsigned char> >);
  STREAMER(std::vector<std::vector<short> >);
  //STREAMER(std::vector<std::vector<unsigned short> >);
  //STREAMER(std::vector<std::vector<int> >);
  //STREAMER(std::vector<std::vector<unsigned int> >);
  //STREAMER(std::vector<std::vector<float> >);
  STREAMER(std::vector<std::vector<double> >);
}


InstallPackedContainerStreamer packedContainerStreamerInstance;


} // namespace xAOD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ELStreamer.cxx,v 1.3 2008-04-15 18:53:35 ssnyder Exp $
/**
 * @file AthenaROOTAccess/src/ELStreamer.cxx
 * @author scott snyder
 * @date July 2007
 * @brief Call the required hooks for reading @c ElementLink classes.
 */


#include "AthenaROOTAccess/ELStreamer.h"
#include "TClass.h"
#include "TROOT.h"


namespace AthenaROOTAccess {


// Used for root bug workaround in copy ctor; see below.
int offset_offs = -1;


/**
 * @brief Constructor.
 * @param cl The class into which we should hook.
 *
 * This will hook the streamer into the given class.
 */
ELStreamer::ELStreamer (TClass* cl)
  : m_cl (cl),
    m_metht (cl, "toTransient", ""),
    m_meths (cl, "source", ""),
    m_old (m_cl->GetStreamer())
{
  // We need to make a copy of the old streamer; the class will delete
  // the original when the new one is adopted.
  if (m_old) m_old = m_old->Generate();

  // Hook in this streamer.
  m_cl->AdoptStreamer (this);
}


/**
 * @brief Copy constructor.
 * @param other The object to copy.
 */
ELStreamer::ELStreamer (const ELStreamer& other)
  : TClassStreamer (other),
    m_cl (other.m_cl),
    m_metht (other.m_metht),
    m_meths (other.m_meths),
    m_old (other.m_old ? other.m_old->Generate() : 0)
{
  // Work around ROOT bug: TMethodCall copy ctor doesn't copy fOffset!
  // ??? This is obsolete with 5.18?
  if (offset_offs == -1) {
    TClass* cl = gROOT->GetClass ("TMethodCall");
    offset_offs = cl->GetDataMemberOffset ("fOffset");
  }
  *reinterpret_cast<long*>(reinterpret_cast<char*>(&this->m_metht) + offset_offs)=
    *reinterpret_cast<const long*>(reinterpret_cast<const char*>(&other.m_metht) + offset_offs);
  *reinterpret_cast<long*>(reinterpret_cast<char*>(&this->m_meths) + offset_offs)=
    *reinterpret_cast<const long*>(reinterpret_cast<const char*>(&other.m_meths) + offset_offs);
}


/**
 * @brief Destructor.
 */
ELStreamer::~ELStreamer()
{
  delete m_old;
}


/**
 * @brief Clone this object.
 */
TClassStreamer* ELStreamer::Generate() const
{
  return new ELStreamer (*this);
}


/**
 * @brief Run the streamer.
 * @param b The buffer from which the object is being read.
 * @param objp Pointer to the object.
 */
void ELStreamer::operator()(TBuffer &b, void *objp)
{
  // If there was a previous streamer, call that.
  if (m_old)
    (*m_old)(b, objp);
  else {
    // Otherwise, we need to call the generic TClass methods.
    if (b.IsReading())
      m_cl->ReadBuffer (b, objp);
    else
      m_cl->WriteBuffer (b, objp);
  }

  // If we're reading, call toTransient() and source().
  if (b.IsReading()) {
    m_metht.Execute (objp);
    m_meths.Execute (objp);
  }
}


} // namespace AthenaROOTAccess

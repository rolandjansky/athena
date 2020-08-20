/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DataModelAthenaPool/src/DataVectorConvert.cxx
 * @author scott snyder
 * @date Oct 2007, from earlier code of Nov 2005
 * @brief Provide backwards compatibility for reading @c DataVector classes.
 */


#include "DataModelAthenaPool/DataVectorConvert.h"
#include "TClass.h"
#include "TBuffer.h"
#include "RootUtils/TClassEditRootUtils.h"
#include "CxxUtils/checker_macros.h"
#include "TMemberStreamer.h"
#include "TStreamerElement.h"
#include "TStreamerInfo.h"
#include "TRealData.h"
#include "TDataMember.h"
#include "TMethodCall.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <mutex>


namespace {


//============================================================================
// Custom streamer class for converting vectors.
//


/**
 * @brief Custom streamer class for reading @c DataVector's.
 *
 * This is actually only used for the @c vector part of the @c DataVector.
 * It allows reading a vector of pointers and performing pointer conversions.
 * I.e., we have a @c vector<A*>, but what's written is @c vector<B*>.
 * The constructor is called with three root classes, corresponding
 * to @c vector<B*>, @c B (@c from), and @c A (@c to).
 */
class DataVectorConvertStreamer
  : public TMemberStreamer
{
public:
  /**
   * @brief Constructor.
   * @param cl The @c vector class from which we're converting
   *           (@c vector<B*> in the above).
   * @param from The class contained in the vector from which
   *             we're converting (@c B in the above).
   * @param to The class contained in the vector to which
   *           we're converting (@c A in the above).
   */
  DataVectorConvertStreamer (TClass* cl, TClass* from, TClass* to);


  /**
   * @brief Run the streamer.
   * @param b Buffer from which to read.
   * @param pmember Pointer to the object into which to read.
   * @param size Number of instances to read.
   */
  virtual void operator() (TBuffer& b, void* pmember, Int_t size=0);


  /**
   * @brief Convert pointer @c from from a pointer-to-FROM to a pointer-to-TO.
   * @param from The pointer to convert.
   *
   * Will return 0 if the conversion is not possible.
   */
  void* convert_ptr (void* from);


private:
  /// The class to which we're converting.
  TClass* m_cl;

  /// The contained in the vector from which we're converting.
  TClass* m_from;

  /// The contained in the vector to which we're converting.
  TClass* m_to;
};


/**
 * @brief Constructor.
 * @param cl The @c vector class from which we're converting
 *           (@c vector<B*> in the above).
 * @param from The class contained in the vector from which
 *             we're converting (@c B in the above).
 * @param to The class contained in the vector to which
 *           we're converting (@c A in the above).
 */
DataVectorConvertStreamer::DataVectorConvertStreamer (TClass* cl,
                                                      TClass* from,
                                                      TClass* to)
  : m_cl (cl),
    m_from (from),
    m_to (to)
{
}


/**
 * @brief Run the streamer.
 * @param b Buffer from which to read.
 * @param pmember Pointer to the object into which to read.
 * @param size Number of instances to read.
 */
void
DataVectorConvertStreamer::operator() (TBuffer& b,
                                       void* pmember,
                                       Int_t size)
{
  // The object to which we're reading, cast to a vector.
  // Note that the code here assumes that std::vector<X*> has
  // the same layout for any X.  This should be the case for any
  // sane implementation, but it's not guaranteed.  If this causes
  // a problem, this could be done by first reading into a temporary
  // and then copying.
  std::vector<void*>* obj = (std::vector<void*>*)pmember;

  // Loop over instances.  In our application, @c size should really
  // only be 1.
  while (size-- > 0) {
    // Read in the object.  The pointers will still need to be converted.
    m_cl->Streamer (obj, b);

    // Loop over each pointer and convert.
    std::vector<void*>& v = *obj;
    size_t len = v.size();
    for (unsigned i=0; i < len; i++)
      v[i] = convert_ptr (v[i]);

    // Move to the next instance.
    ++obj;
  }
}


/**
 * @brief Convert pointer @c from from a pointer-to-FROM to a pointer-to-TO.
 * @param from The pointer to convert.
 *
 * Will return 0 if the conversion is not possible.
 */
void* DataVectorConvertStreamer::convert_ptr (void* from)
{
  // Don't need to do anything if source and dest classes are the
  // same, or if this is a null pointer.
  if (m_from == m_to || from == 0)
    return from;

  // We used to to the conversion like this.
  // But this doesn't work in root6 if virtual derivation is used.
#if 0
  // We do the conversion by finding the actual (most-derived) class
  // of this instance, then looking up the base class offsets of the
  // source and destination classes.  In principle, if the source
  // and destination classes are related by non-virtual inheritance,
  // then this can reduce to a constant that we could compute at initialization
  // time.  However, i was unable to get information from root on the
  // inheritance type (virtual vs. non-virtual), so that optimization
  // is not done.
  TClass* cl_actual = m_from->GetActualClass (from);
  if (!cl_actual)
    return 0;
  int offs1 = cl_actual->GetBaseClassOffset (m_from);
  int offs2 = cl_actual->GetBaseClassOffset (m_to);
  if (offs1 < 0 || offs2 < 0)
    return 0;
  return (char*)from - offs1 + offs2;
#endif

  // So instead, just call to the interpreter.
  // Slow, but doesn't matter so much here since this is just
  // for backwards compatibility.
  std::ostringstream os;
  os << "dynamic_cast<" << m_to->GetName() << "*>((" << m_from->GetName() << "*)" << from << ")";
  std::string line = os.str();
  Long_t ret = gInterpreter->Calc (line.c_str());
  return reinterpret_cast<void*> (ret);
}


//============================================================================
// Helper functions for installing the conversion.
//


/**
 * @brief Look up a class with Root.
 * @param logfn The debug logging callback.
 * @param dv_class The @c DataVector class we're working on,
 *                 for logging.
 * @param name The name of the class we want to find.  A trailing *
 *             will be stripped off before using it.
 * @param inner If true, then what we really look up is the contained
 *              class of an STL container.  This transformation is done
 *              before stripping the *.  So, if @c name is
 *              @c vector<A*>, then if @c inner is @c true, we'll
 *              look up @c A.
 *
 * Returns 0 on failure.
 */
TClass* find_class (RootUtils::ILogger* logfn,
                    TClass* dv_class,
                    const std::string& name,
                    bool inner = false)
{
  std::string name2 = name;
  if (inner)
    name2 = TClassEdit::ShortType (name2.c_str(), TClassEdit::kInnerClass);
  name2 = TClassEdit::ShortType (name2.c_str(), TClassEdit::kDropTrailStar);
  TClass* cls = gROOT->GetClass (name2.c_str());
  if (cls == 0 && logfn) {
    std::ostringstream ss;
    ss << dv_class->GetName() << ": Can't find class " << name2;
    logfn->debug (ss.str().c_str());
  }
  return cls;
}


/**
 * @brief Get data member offset within DataVector.
 * @param dv_class The top-level DataVector class.
 * @param dm The member for which we want to find the offset.
 *
 * Returns -1 on failure.
 *
 * We can't just use dv_class->GetRealData(); that doesn't work
 * in root6 if there's virtual derivation.
 */
int get_member_offset (TClass* dv_class, TDataMember* dm)
{
  TMethodCall meth;
  meth.InitWithPrototype (dv_class, "baseOffset", "const std::type_info&");
  if (!meth.IsValid()) {
    return -1;
  }
  TClass* mcls = dm->GetClass();
  meth.ResetParam();
  meth.SetParam (reinterpret_cast<Long_t> (mcls->GetTypeInfo()));
  Long_t ret = 0;
  meth.Execute (ret);
  TRealData* rd = mcls->GetRealData (dm->GetName());
  if (rd)
    return rd->GetThisOffset() + ret;
  return -1;
}


/**
 * @brief Adjust the streamer info for a single @c DataVector class
 *        for reading old data.
 * @param logfn The debug logging callback.
 * @param dv_class The @c DataVector class we're working on.
 * @param si1 The @c TStreamInfo object for one of the old shapes
 *            of the class.  This is the one that we're going to fix up.
 * @param sie0 The @c TStreamerElement for the @c m_pCont field
 *             for the in-memory version of the class.
 * @param sie1 The @c TStreamerElement for the @c m_pCont field
 *             for the version of the class we're trying to read
 *             (from @c si1).
 */
void diddle_dv_streaminfo (RootUtils::ILogger* logfn,
                           TClass* dv_class,
                           TStreamerInfo* si1,
                           TStreamerElement* sie0,
                           TStreamerElement* sie1)
{
  // This is the type from which we need to convert.
  // Give up if it ain't a vector.
  std::string vec_name = sie1->GetTypeName();
  if (std::abs(TClassEdit::IsSTLCont (vec_name.c_str(), 0)) !=
      TClassEdit::kVector)
  {
    if (logfn) {
      std::ostringstream ss;
      ss << dv_class->GetName() << ": Type to be converted is not a vector: "
         << vec_name;
      logfn->debug (ss.str().c_str());
    }
    return;
  }

  // Extract the inner classes from the source and target vector names,
  // as well as the source vector itself.  Give up if any of them
  // aren't known to root.
  TClass* cl_from = find_class (logfn, dv_class, vec_name, true);
  TClass* cl_to = find_class (logfn, dv_class, sie0->GetTypeName(), true);
  TClass* vcls = find_class (logfn, dv_class, vec_name);
  if (cl_from == 0 || cl_to == 0 || vcls == 0)
    return;

  // Fix up the streaminfo.  Change the type name to what's expected,
  // and install a custom streamer.
  sie1->SetTypeName (sie0->GetTypeName());
  sie1->TStreamerElement::SetStreamer
    (new DataVectorConvertStreamer (vcls, cl_from, cl_to));

  // Now we have to make sure the offsets are correct.
  TIter next (si1->GetElements());
  while (TStreamerElement* elem = dynamic_cast<TStreamerElement*> (next())) {
    elem->SetNewType (elem->GetType());
    TRealData* rd = dv_class->GetRealData (elem->GetName());
    if (rd) {
      // We used to do this; however this doesn't work with
      // root6 if there's virtual derivation (root6 will quietly
      // return garbage in such a case).  We have to get the offsets
      // in a more roundabout way.
      //elem->SetOffset (rd->GetThisOffset());
      int offs = get_member_offset (dv_class, rd->GetDataMember());
      if (offs >= 0)
        elem->SetOffset (offs);
      else
        elem->SetNewType (-2);
    }
    else
      elem->SetNewType (-2);
  }

  // Rebuild all the internal tables.
  si1->Compile();
}


/**
 * @brief Examine a single @c DataVector class to see if it needs
 *        to be adjusted for backwards compatibility.
 * @param logfn The debug logging callback.
 * @param dv_class The @c DataVector class we're working on.
 *
 * If any of the shapes of @c dv_class don't match its primary shape,
 * then call @c diddle_dv_streaminfo.
 */
void test_dv (RootUtils::ILogger* logfn,
              TClass* dv_class)
{
  if (logfn) {
    std::ostringstream ss;
    ss << "Testing class " << dv_class->GetName()
       << " for possible back-compatibility conversions.";
    logfn->debug (ss.str().c_str());
  }

  // Get the array of TStreamerInfos.
  const TObjArray* infos = dv_class->GetStreamerInfos();
  if (infos == 0) {
    if (logfn) {
      std::ostringstream ss;
      ss << dv_class->GetName() << ": Can't find TStreamerInfo list.";
      logfn->debug (ss.str().c_str());
    }
    return;
  }

  // If no streaminfos, this class isn't involved in persistency.  Skip it.
  if (infos->GetEntries() == 0)
    return;

  // Get the class version, and thence the primary class shape.
  int dv_vers = dv_class->GetClassVersion(); 
  TVirtualStreamerInfo* si0 = dv_class->GetStreamerInfo();
  if (si0 == 0) {
    if (logfn) {
      std::ostringstream ss;
      ss << dv_class->GetName() << ": Can't find primary shape.";
      logfn->debug (ss.str().c_str());
    }
    return;
  }

  // Look up the m_pCont field in the primary shape.
  Int_t offset;
  TStreamerElement* sie0 = si0->GetStreamerElement ("m_pCont", offset);
  if (sie0 == 0 ||
      std::abs(TClassEdit::IsSTLCont (sie0->GetTypeName(), 0)) !=
      TClassEdit::kVector)
  {
    if (logfn) {
      std::ostringstream ss;
      ss << dv_class->GetName() << ": m_pCont not present, or not a vector.";
      logfn->debug (ss.str().c_str());
    }
    return;
  }

  // Go through all other shapes.
  // Look up m_pCont in each.
  // If the type differs from that in the primary shape, then we'll
  // need to fix up the streamer info.
  Int_t nent = infos->GetEntriesFast();
  for (Int_t i = 0; i < nent; i++) {
    if (i == dv_vers) continue;
    TStreamerInfo* si1 = dynamic_cast<TStreamerInfo*>(infos->At (i));
    if (si1 != 0) {
      TStreamerElement* sie1 = si1->GetStreamerElement ("m_pCont", offset);
      if (sie1 != 0 && strcmp (sie0->GetTypeName(), sie1->GetTypeName()) != 0){
        if (logfn) {
          std::ostringstream ss;
          ss << dv_class->GetName()
             << ": SI slot " << i << " has m_pCont with type "
             << sie1->GetTypeName() << "; needs adjustment.";
          logfn->debug (ss.str().c_str());
        }
        diddle_dv_streaminfo (logfn, dv_class, si1, sie0, sie1);
      }
    }
  }
}


} // anonymous namespace


namespace DataModelAthenaPool {


/**
 * @brief Scan all known @c DataVector classes and fix up the Root
 *        data for any that need conversion.
 * @param logfn Callback for logging debug messages.
 *              Null for no logging.
 *
 *        It's ok to call this more than once to look for any new classes
 *        (we remember which classes we've already processed to avoid
 *        doing them again).
 */
void DataVectorConvert::initialize (RootUtils::ILogger* logfn /* = 0*/)
{
  static std::mutex m;
  std::lock_guard<std::mutex> lock (m);

  // List of classes we've already processed.
  static std::vector<TClass*> s_seen ATLAS_THREAD_SAFE;

  TIter next (gROOT->GetListOfClasses());
  while (TClass* cls = dynamic_cast<TClass*> (next())) {
    if (std::strncmp (cls->GetName(), "DataVector<", 11) == 0) {
      if (std::find (s_seen.begin(), s_seen.end(), cls) == s_seen.end()) {
        test_dv (logfn, cls);
        s_seen.push_back (cls);
      }
    }
  }
}


} // namespace DataModelAthenaPool

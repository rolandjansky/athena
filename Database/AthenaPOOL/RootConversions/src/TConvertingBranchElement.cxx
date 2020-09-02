/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TConvertingBranchElement.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief A variant of @c TBranchElement that can call converters
 *        when reading objects in split mode.
 */


#include "RootConversions/TConvertingBranchElement.h"
#include "RootConversions/TConverterRegistry.h"
#include "RootConversions/TVirtualConverter.h"
#include "TClonesArray.h"
#include "TStreamerElement.h"
#include "TFile.h"
#include "TTree.h"
#include "TVirtualCollectionProxy.h"
#include "TVirtualCollectionIterators.h"
#include "TBranchRef.h"
#include "RootUtils/TClassEditRootUtils.h"
#include "TBasket.h"
#include "TError.h"
#include "TStreamerInfo.h"
#include "TBufferFile.h"
#include "TVirtualArray.h"
#include "TROOT.h"
#include <cassert>
#include <cstdlib>


bool TConvertingBranchElement::fgDoDel = false;


namespace {


/**
 * @brief Temporarily change the object referred to by a collection proxy.
 *
 * This is the same as @c TVirtualCOllectionProxy::TPushPop,
 * except that it doesn't crash if the proxy pointer is null.
 */
class TPushPopSafe
{
public:
  TVirtualCollectionProxy* fProxy;
  TPushPopSafe (TVirtualCollectionProxy* proxy, void* objectstart);
  inline ~TPushPopSafe();
};


/**
 * @brief Constructor
 * @param proxy The collection proxy to change.  May be null.
 * @param objectstart The new object to which the proxy should refer.
 *
 * Changes @c proxy so that it now refers to @c objectstart.
 * The old reference is remembered and will be restored
 * when this object is destroyed.
 * If the @c proxy is 0, this does nothing.
 */
inline TPushPopSafe::TPushPopSafe (TVirtualCollectionProxy* proxy,
                                   void* objectstart)
  : fProxy (proxy)
{
  if (fProxy)
    fProxy->PushProxy (objectstart);
}


/**
 * @brief Destructor.
 *
 * Restore the proxy's previously-referenced object.
 * If the proxy pointer was null, this does nothing.
 */
inline TPushPopSafe::~TPushPopSafe()
{
  if (fProxy)
    fProxy->PopProxy();
}


/**
 * @brief Remove a prefix from a string.
 * @param str    The string to be modified.
 * @param prefix The prefix.
 *
 * If @c str starts with @c prefix, then that substring is removed
 * from the start of @c str (in-place).  Otherwise, @c str is not modified.
 */
void RemovePrefix(TString& str, const char* prefix)
{
  if (str.Length() && prefix && strlen(prefix)) {
    if (!str.Index(prefix)) {
      str.Remove(0, strlen(prefix));
    }
  }
}


/**
 * @brief Return the base name of a branch, from its full name.
 * @param fullname The full name of the branch.
 *
 * Given a full branch name like A.B.C[10], this function will
 * return the base name `C'.
 */
std::string BasenameOfBranch (const std::string& fullname)
{
  std::string s = fullname;
  size_t pos = s.rfind('.');
  if (pos != std::string::npos) {
    s = s.substr(pos+1);
  }
  while ((pos = s.rfind('[')) != std::string::npos) {
    s = s.substr(0, pos);
  }
  return s;
}


/**
 * @brief Find the @c TStreamerInfo for a class from a @c TFile.
 * @param cl The class for which we want the @c TStreamerInfo.
 * @parma br The branch in which the class is being read.
 *
 * Find the @c TStreamerInfo for a class by looking directly at the
 * list from the file from which it's being read (not relying on the
 * class members).
 */
TStreamerInfo* GetStreamerInfoFromFile (const TClass* cl, const TBranch* br)
{
  // Find the file holding this branch.
  // (Can't rely on fInfo having being set up yet.)
  TDirectory* dir = br->GetDirectory();
  if (!dir) return 0;
  TFile* file = dir->GetFile();
  if (!file) return 0;
  if (file->GetSeekInfo() == 0) return 0;

  // Find the streamerinfo for this class.
  const TList* silist = file->GetStreamerInfoCache();
  TListIter li (silist);
  TStreamerInfo* info;
  while ((info = dynamic_cast<TStreamerInfo*> (li.Next())) != 0) {
    if (strcmp (info->GetName(), cl->GetName()) == 0)
      break;
  }

  return info;
}


/**
 * @brief Test to see if @c cl or anything it contains needs a conversion.
 * @param cl The class to test.
 * @param br The branch in which it is contained.
 *
 * Test to see if the class @c cl, or any classes that it contains,
 * requires running a conversion on input.
 */
bool DoesClassNeedConv (const TClass* cl, const TBranch* br)
{
  if (!cl) return false;
  TStreamerInfo* info = GetStreamerInfoFromFile (cl, br);
  if (!info) return false;

  // Does this class have a conversion?
  if (TConverterRegistry::Instance()->GetConverter (cl->GetName(),
                                                    info->GetCheckSum()))
    return true;

  // Do any contained classes have a conversion?
  TObjArray* elems = info->GetElements();
  int ndata = elems->GetEntriesFast();
  for (int i = 0; i < ndata; ++i) {
    TStreamerElement* elt =
      reinterpret_cast<TStreamerElement*> (elems->At (i));
    TClass* bcl = elt->GetClass();
    // Note: we can have bcl==cl for a STL container.
    // For example, the TStreamerInfo for vector<int> has a single
    // TStreamerSTL element with a class that points
    // back to vector<int>.
    if (bcl && bcl != cl && DoesClassNeedConv (bcl, br))
      return true;
  }
  return false;
}


/**
 * @brief Does the class given by @c elt have a field named @c name?
 * @param elt  @c TStreamerElement giving the class to test.
 * @param name The name for which to test.
 * @param br   The parent @c TBranch we're reading.
 *
 * The class represented by @c elt is retrieved.
 * We then test to see if that class contains a data member called @c name,
 * either directly or as a member of a base class.
 */
bool BaseHasField1 (TStreamerBase* elt,
                    const TString& name,
                    const TBranch* br)
{
  // Find the TStreamerInfo for the class we're testing.
  TClass* cl = elt->GetClassPointer();
  if (!cl) return false;
  TStreamerInfo* info = GetStreamerInfoFromFile (cl, br);
  if (!info) return false;

  // Go through each element looking for the name.
  // Recursively check base classes.
  TObjArray* elems = info->GetElements();
  size_t ndata = elems->GetEntriesFast();
  for (size_t ielt = 0; ielt < ndata; ielt++) {
    TStreamerElement* selt =
      reinterpret_cast<TStreamerElement*> (elems->At(ielt));
    if (TStreamerBase* belt = dynamic_cast<TStreamerBase*> (selt)) {
      if (BaseHasField1 (belt, name, br))
        return true;
    }
    else {
      if (name == selt->GetName())
        return true;
    }
  }
  return false;
}


/**
 * @brief Does the class given by @c elt have a field with a given name?
 * @param elt  @c TStreamerElement giving the class to test.
 * @param bname The name for which to search (see below).
 * @param namedot Prefix to remove from @c bname.
 * @param br   The parent @c TBranch we're reading.
 *
 * The name for which we search is given by the first dotted element
 * in @c bname after the prefix @c namedot is removed.  For example,
 * if @c bname is `a.b[10].c' and @c namedot is `a.', then we look
 * for the name `b'.
 */
bool BaseHasField (TStreamerBase* elt,
                   TString bname,
                   const TString& namedot,
                   const TBranch* br)
{
  if (!elt) return false;
  RemovePrefix (bname, namedot.Data());
  Ssiz_t i = bname.Index ("[");
  if (i != kNPOS)
    bname.Remove (i);
  i = bname.Index (".");
  if (i != kNPOS)
    bname.Remove (i);
  return BaseHasField1 (elt, bname, br);
}


   struct R__PushCache {
      TBufferFile &fBuffer;
      TVirtualArray *fOnfileObject;

      R__PushCache(TBufferFile &b, TVirtualArray *in, UInt_t size) : fBuffer(b), fOnfileObject(in) {
         if (fOnfileObject) {
            fOnfileObject->SetSize(size);
            fBuffer.PushDataCache( fOnfileObject );
         }
      }
      ~R__PushCache() {
         if (fOnfileObject) fBuffer.PopDataCache();
      }
   };


} // anonymous namespace


/**
 * @brief Constructor.
 */
TConvertingBranchElement::TConvertingBranchElement()
: fConv(0)
, fConvClass(0)
, fConvObject(0)
, fConvOrigBranches(0)
, fConvOrigType(-1)
, fConvContainerFlag(false)
, fConvDontReset(false)
{
}


/**
 * @brief Constructor.
 *
 * Get rid of any temporary objects which we made.
 */
TConvertingBranchElement::~TConvertingBranchElement()
{
  // If we made a temporary object instance for conversions, delete it.
  if (fConv && fConvObject)
    fConv->DeletePersObj (fConvObject);

  // If we contain dummy branches, delete them here.
  // In that case, we should also delete the saved original branch list.
  if (fConvOrigBranches) {
    Int_t nbranches = fBranches.GetEntriesFast();
    for (Int_t i = 0; i < nbranches; i++) {
      if (fBranches[i]->TestBit (kIsDummy))
        delete fBranches[i];
    }
    fBranches = *fConvOrigBranches;
    delete fConvOrigBranches;
  }

  // If we're a dummy ourselves, we should delete the branch list.
  if (TestBit (kIsDummy))
    fBranches.Clear();
  
  // Try to delete the object, if requested.
  if (fgDoDel)
    ResetAddress();
}


/**
 * @brief Set up to allow for conversions in split mode.
 */
void TConvertingBranchElement::Initialize()
{
#if 0
  static bool initialized = false;
  if (initialized)
    return;
  initialized = true;

  TClass* cl = gROOT->GetClass ("TBranchElement", true);
  if (!cl) {
    ::Error ("TConvertingBranchElement",
             "Can't find TClass for TBranchElement");
    return;
  }

  // Change the @c New() method for @c TBranchElement to make
  // an instance of this class instead.
  cl->SetNew (TConvertingBranchElement::new_TConvertingBranchElement);
#endif
}


/**
 * @brief new() method for this object.
 * @param p Address for placement new, or 0.
 * @return Address of the new object.
 *
 * This is installed as the @c New method in @c TBranchElement's @c TClass.
 * Thus, when we read from a file a @c TBranchElement, we actually
 * make an instance of this class.
 */
void* TConvertingBranchElement::new_TConvertingBranchElement (void *p)
{
  if (p)
    return new (p) TConvertingBranchElement;
  return new TConvertingBranchElement;
}


/**
 * @brief Add dummy nodes if needed to recover the correct tree structure.
 *
 * Given a @c TBranchElement tree structure, Root will elide some of the
 * intermediate tree nodes.  For example, given:
 *
 *   a
 *   a.b
 *   a.b.c
 *   a.b.c.d1
 *   a.b.c.d2
 *
 * Root will elide the `c' node --- so one has node `a', with
 * child `a.b', with children `a.b.c.d1' and `a.b.c.d2'.
 *
 * This greatly complicates the process of running conversions,
 * if `c' has a conversion.  So, what we do is if we determine that
 * a conversion is needed, we modify the tree to reinsert the elided
 * nodes.  These nodes will have the kIsDummy flag set.  Further,
 * if we do that, we remember the original branch list from the parent
 * node (`a.b' in the above example), so that if the structure is written,
 * we can write the original, elided structure.
 */
void TConvertingBranchElement::BuildConvertedElisions()
{
  // Can't do anything if we can't find the @c TStreamerInfo.
  if (!fInfo) return;

  // The class of this branch's element.
  TClass* topclass = 0;
  if (fID < 0)
    topclass = gROOT->GetClass (GetClassName());
  else {
    std::string s = BasenameOfBranch (GetName());
    int offset = 0;
    TStreamerElement* elt = fInfo->GetStreamerElement(s.c_str(), offset);
    if (elt)
      topclass = elt->GetClass();
  }
  if (!topclass) return;

  // Here's the plan.
  // We start by adding all branches to the @c branches list.
  // Then we walk through the members of this class (using the
  // @c TStreamerInfo).
  // If this member is contained in the branch list, then we
  // remove it from @c branches and add it to @c noconv_branches
  // (it wasn't elided).
  // If it's not in the branch list, then it was elided.
  // We look then to see if the corresponding class (or anything
  // it contains) requires a conversion.  If so, then we build
  // a dummy node for it, and add the dummy to @c conv_dummies.
  // We then look through @c branches, find any that actually
  // belong to the new dummy node, remove them from @c branches,
  // and add them to the dummy.
  // If no conversion is needed, then we don't need to do anything
  // at this point.
  //
  // When we're done scanning all the members of the class, we take
  // anything remaining in @c branches and move to @c noconv_branches
  // (these correspond to branches with elided nodes, but with
  // no conversions required.)  Then, if we made any dummy nodes,
  // then we replace our branch list with the concatenation of the
  // dummy list and the @c noconv_branches list (saving the original
  // branch list so that we can restore it later, if needed).
  std::vector<TBranch*> conv_dummies;
  std::vector<TBranch*> noconv_branches;
  std::vector<TBranch*> branches;

  // To start with, add all our child branches to @c branches.
  Int_t nbranches = fBranches.GetEntriesFast();
  branches.reserve (nbranches);
  for (Int_t i=0; i < nbranches; i++) {
    TBranch* b = dynamic_cast<TBranchElement*> (fBranches[i]);
    if (b)
      branches.push_back (b);
  }

  // Get the @c TStreamerInfo for this class.
  // If this class is a container, then we actually want
  // to get the @c TStreamerInfo for the contained class.
  TStreamerInfo* info = GetStreamerInfoFromFile (topclass, this);
  if (!info && fType == 4) {
    TVirtualCollectionProxy* proxy = topclass->GetCollectionProxy();
    if (!proxy) return;
    topclass = proxy->GetValueClass();
    if (!topclass) return;
    info = GetStreamerInfoFromFile (topclass, this);
  }
  else if (fType == 3) {
    topclass = gROOT->GetClass (fClonesName);
    if (!topclass) return;
    info = GetStreamerInfoFromFile (topclass, this);
  }
  if (!info) return;

  // Now walk through all elements in the @c TStreamerInfo.
  TObjArray* elems = info->GetElements();
  size_t ndata = elems->GetEntriesFast();
  for (size_t ielt = 0; ielt < ndata; ++ielt) {
    // The element.
    TStreamerElement* elt =
      reinterpret_cast<TStreamerElement*> (elems->At(ielt));

    // See if there's a branch corresponding to this element.
    // If so, add it to the noconv list.
    bool found = false;
    for (unsigned i=0; i < branches.size(); i++) {
      TBranchElement* b = dynamic_cast<TBranchElement*>(branches[i]);

      // Test branch @b to see if corresponds to the SI element @ elt.
      if (b && gROOT->GetClass (b->GetClassName()) == topclass) {
        if (dynamic_cast<TStreamerBase*> (elt) != 0 && b->GetType() == 1) {
          // For a base class, the test above is sufficient.
          found = true;
        }
        else {
          // Test the name.
          std::string s = BasenameOfBranch (b->GetName());
          if (s == elt->GetName())
            found = true;
        }

        if (found) {
          // We found a branch matching the element.
          // Move the branch from @c branches to @c noconv_branches,
          // and exit the loop over branches.
          noconv_branches.push_back (b);
          branches.erase (branches.begin()+i);
          break;
        }
      }
    }

    if (!found) {
      // We didn't find a branch corresponding to this element.
      // Maybe this is an elided class.
      // If this is a class type, then see if it or anything
      // it contains has a conversion.  If so, then build a dummy
      // node for it.
      TClass* cl = elt->GetClass();
      if (cl && DoesClassNeedConv (cl, this)) {
        // It needs a conversion.  Make a dummy node.
        TClass* clparent = gROOT->GetClass (info->GetName());
        TConvertingBranchElement* dum = new TConvertingBranchElement;
        dum->SetBit (kIsDummy);
        TString name;
        if (fID < 0)
          name = elt->GetName();
        else {
          name = GetName();
          if (fType == 1) {
            Ssiz_t i = name.Length()-1;
            while (i >= 0 && name[i] != '.')
              --i;
            name.Remove (i+1);
          }
          else
            name += ".";
          name += elt->GetName();
        }
        dum->SetName (name);
        dum->SetTitle (name);
        TString namedot = name + ".";
        dum->SetParentClass (clparent);
        dum->SetClassName (info->GetName());
        dum->fCheckSum = info->GetCheckSum();
        dum->fClassVersion = topclass->GetClassVersion();
        dum->fID = ielt;
        dum->fTree = this->fTree;
        dum->fDirectory = this->fDirectory;
        dum->fBranchClass = topclass;
        if (dynamic_cast<TStreamerBase*> (elt) != 0) {
          dum->fType = 1;
          Ssiz_t i = namedot.Length()-2;
          while (i >= 0 && namedot[i] != '.')
            --i;
          namedot.Remove (i+1);
        }
        else
          dum->fType = 2;

        // Add the dummy node to @c conv_dummies.
        conv_dummies.push_back (dum);
        
        // Find all branches that are a part of this class and
        // move them from @c branches to the dummy node
        for (unsigned i=0; i < branches.size(); i++) {
          TString bname = branches[i]->GetName();
          if (bname.Index (namedot) == 0 &&
              (dum->fType == 2 ||
               BaseHasField (dynamic_cast<TStreamerBase*>(elt),
                             bname,
                             namedot,
                             this)))
          {
            dum->GetListOfBranches()->Add (branches[i]);
            if (TConvertingBranchElement* be =
                dynamic_cast<TConvertingBranchElement*> (branches[i]))
            {
              be->fParentClass = cl;
            }
            branches.erase (branches.begin()+i);
            --i;
          }
        }
      }
    }
    // End of loop over elements.
  }

  // Any remaining branches go to @c noconv_branches.
  for (unsigned int i=0; i < branches.size(); i++)
    noconv_branches.push_back (branches[i]);

  if (conv_dummies.size() > 0) {
    // We made some dummies.
    // Replace our branch list with the concatenation
    // of the dummy list (@c conv_dummies) and the branches
    // that don't need special processing (@c noconv_branches --- because
    // they either did not have an elision or they didn't require
    // a conversion).
    fConvOrigBranches = new TObjArray;
    *fConvOrigBranches = fBranches;
    fBranches.Clear();
    for (unsigned int i=0; i < conv_dummies.size(); i++)
      fBranches.Add (conv_dummies[i]);
    for (unsigned int i=0; i < noconv_branches.size(); i++)
      fBranches.Add (noconv_branches[i]);
  }
}


/**
 * @brief Check to see if we need to worry about conversions for this branch.
 *        Set up member variables if so.
 */
void TConvertingBranchElement::CheckForConversion()
{
  // Need @c TStreamerInfo in order to do anything.
  if (!fInfo) return;

  // Restore any elided tree nodes if there's a conversion.
  BuildConvertedElisions();

  // See if this element is a class.
  const char* classname = 0;
  int checksum = 0;

  if (fID < 0) {
    // Special case for top-level branch.
    classname = GetClassName();
    checksum = fCheckSum;
  }
  else {
    // Otherwise, we need to look at the streamerinfo element.
    std::string s = BasenameOfBranch (GetName());
    int offset = 0;
    TStreamerElement* elt = fInfo->GetStreamerElement(s.c_str(), offset);
    if (elt) {
      TClass* cl = elt->GetClass();
      if (cl) {
        classname = cl->GetName();
        TStreamerInfo* info = GetStreamerInfoFromFile (cl, this);
        if (info)
          checksum = info->GetCheckSum();
      }
    }
  }

  if (classname) {
    // Ok, this branch represents a class.
    // See if there's a conversion for it.
    TVirtualConverter* conv =
      TConverterRegistry::Instance()->GetConverter (classname, checksum);

    // If there's not a conversion and this class is a container,
    // then we need to see if something in the container needs a conversion.
    if (!conv && (fType == 4 || fType == 3)) {
      // Find the contained class.
      TClass* contclass = gROOT->GetClass (fClonesName.Data());
      if (contclass) {
        TStreamerInfo* info = GetStreamerInfoFromFile (contclass, this);
        if (info) {
          // Found the contained class with its @c TStreamerInfo.
          // See if the contained class has a conversion.
          conv =
            TConverterRegistry::Instance()->GetConverter (fClonesName.Data(),
                                                          info->GetCheckSum());

          // If the contained class, or anything it contains, has a conversion,
          // then we need to deal with conversions when reading this container.
          if (conv || DoesClassNeedConv (contclass, this)) {
            fConvContainerFlag = true;
            ConvResetType();
          }
        }
      }
    }

    if (conv) {
      TClass* convclass = conv->GetPersClass();
      if (convclass) {
        // We have a conversion!  Remember the converter and persistent
        // class, and create the temporary persistent class instance.
        fConv = conv;
        fConvClass = convclass;
        fConvObject = (char*)conv->NewPersObj();
      }
    }
  }

  // Now see if the class containing this element has a conversion.
  if (fID > -1) {
    TVirtualConverter* conv =
      TConverterRegistry::Instance()->GetConverter (GetClassName(),
                                                    fCheckSum);
    if (conv) {
      TClass* convclass = conv->GetPersClass();
      if (convclass) {
        // The class containing this class has a conversion.
        // Change the @c TStreamerInfo to correspond to the
        // appropriate persistent class; also change the parent class
        // pointer.
        Bool_t optim = TStreamerInfo::CanOptimize();
        TStreamerInfo::Optimize(kFALSE);
        TStreamerInfo* info =
          dynamic_cast<TStreamerInfo*> (convclass->GetStreamerInfo (0));
        if (info)
          fInfo = info;
        TStreamerInfo::Optimize(optim);
        fParentClass = convclass;
      }
    }
  }
}


/**
 * @brief Initialize the TStreamerInfo pointer.
 * (Overridden internal method.)
 *
 * This gets called by @c GetInfo if the @c fInfo pointer is null.
 * It should initialize that pointer.
 *
 * Here is where we check for a conversion.
 */
void TConvertingBranchElement::InitInfo()
{
  // Do the standard stuff.
  TBranchElement::InitInfo();

  //  Check for a conversion.
  if (fInfo) {
    const_cast<TConvertingBranchElement*>(this)->CheckForConversion();

    // Re-find the fID.
    const_cast<TConvertingBranchElement*>(this)->fInit = kFALSE;
    TBranchElement::InitInfo();
  }

  // Change ReadLeaves implementation if needed.
  // The base class implementation works here except for the case
  // of an STL associative container.  We need to change the code
  // for that to notice @c fConvDontReset and to use @c ReadSubBranches.
  if (fType == 4)
    fReadLeaves = (ReadLeaves_t)&TConvertingBranchElement::ReadLeavesCollectionConverting;
  else if (fType == 2 && (fConvOrigType == 41 || fConvOrigType == 31))
    fReadLeaves = (ReadLeaves_t)&TConvertingBranchElement::ReadLeavesMemberBranchCountConverting;
}


/**
 * @brief Recursively reset the type field of container elts in conversions.
 *
 * Walk recursively through all children of this node.
 * Any of them that have a @c fType corresponding to a member field
 * within a container (31 or 41) are reset to 2 (normal member).
 */
void TConvertingBranchElement::ConvResetType()
{
  Int_t nbranches = fBranches.GetEntriesFast();
  for (Int_t i=0; i < nbranches; i++) {
    TConvertingBranchElement* b =
      dynamic_cast<TConvertingBranchElement*> (fBranches[i]);
    if (b) {
      if (b->fType == 41 || b->fType == 31) {
        b->fConvOrigType = b->fType;
        b->fType = 2;
      }
      b->ConvResetType();
    }
  }
}


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
 *
 * Some notes about conversions.
 * If this branch has a conversion, then rather than reading directly
 * into the target address, we instead read into a temporary object
 * of the persistent type (@c fConvObject), and then call the converter
 * to copy from the persistent to the transient object.
 * We override @c SetAddress so that this will get set correctly
 * for children of converted classes.
 *
 * Note that the need to call the converter from the branch node corresponding
 * to the class with the conversion is why we have problems with elided
 * tree nodes --- and the reason for @c BuildConvertedElisions.
 *
 * Containers are more complicated.
 * Suppose we have something like:
 *
 *  struct A { int a1; int a2; };
 *  struct B { int b1; A b2; };
 *  struct C { std::vector<B> c1; };
 *
 * and we're working on the vector<B>.
 *
 * The way Root handles this is that we have three branches: c1.b1,
 * c1.b1.a1, c1.b1.a2 holding the container data.  In addition, the
 * container branch c1 will hold the count of the number of elements
 * in the container.  What Root does is:
 *  - First read the element count from branch c1.  This will be stored
 *    in the c1 @c TBranchElement.  Its children point to it to get
 *    the number of elements.
 *  - We then loop over the branches.
 *  - We read all elements at once for each branch, via
 *    @c TBranchInfo::ReadBufferSTL.  We get the number of elements
 *    by looking in the parent container branch.  Note that for the
 *    children of the container, the address set via @c SetAddress
 *    is not the address of the object itself, but rather the address
 *    of a collection proxy.  The proxy is then associated with
 *    the actual container being read.
 *
 * Now, when we try to do this with conversions, we run into the problem
 * that we need the complete object in order to run the conversion.
 * One way of doing this would be to buffer the complete collection
 * of persistent objects, then loop over them calling the converter.
 * I wanted to avoid having to do that for large collections, so what's
 * done here is somewhat more complicated.  The idea is to change
 * the order of the loop over the branches and the loop over the
 * elements, so that we make one complete object at a time.
 * In a little more detail:
 *
 *  - We change the type of the children of the containers from 31/41
 *    (member field of container) to 2 (normal member).
 *
 *  - When reading the collection branch, we read the number of elements,
 *    then loop over each element.  For each element, we loop over
 *    each branch, calling @c GetEntry.  But first, we call @c SetAddress
 *    to pass in the address of the temporary persistent object
 *    (if a conversion is being done for the object directly
 *    contained by the container) or the address of the object
 *    in the container (if the conversion is further down).
 *
 *  - A complication is that normally when @c GetEntry is called
 *    for a member in a container, it (via @c ReadLeaves) resets
 *    the @c TBuffer to point at the first element in the collection.
 *    Because of how we've rearranged the loops, we want this to happen
 *    only on the first time.  To accomplish this, a `dont_reset'
 *    flag is introduced, to tell @c GetEntry not to call
 *    @c ReadLeaves.  It would be most natural to do this by adding
 *    a parameter to @c GetEntry.  However, at this point, we do
 *    not want to change the @c GetEntry interface.  So, instead
 *    of passing this as an argument, it's passed in a member
 *    variable, @c fConvDontReset.
 *
 * All of this is handled by the following methods:
 *  @c GetEntry
 *  @c ReadLeaves
 *  @c ReadSubBranches
 */
Int_t TConvertingBranchElement::GetEntry(Long64_t entry, Int_t getall)
{
  // The first part of this just copies the base class @c GetEntry.
  // Differences from the base class implementation will be pointed out below.

  // Remember which entry we are reading.
  fReadEntry = entry;

  // If our tree has a branch ref, make it remember the entry and
  // this branch.  This allows a TRef::GetObject() call done during
  // the following I/O operation, for example in a custom streamer,
  // to search for the referenced object in the proper element of the
  // proper branch.
  TBranchRef* bref = fTree->GetBranchRef();
  if (bref) {
    bref->SetParent(this, fBranchID);
    bref->SetRequestedEntry(entry);
  }

  Int_t nbytes = 0;

  SetupAddresses();

  Int_t nbranches = fBranches.GetEntriesFast();
  if (nbranches) {
    // -- Branch has daughters.
    // One must always read the branch counter.
    // In the case when one reads consecutively twice the same entry,
    // the user may have cleared the TClonesArray between the GetEntry calls.
    if ((fType == 3) || (fType == 4)) {
      Int_t nb = TBranch::GetEntry(entry, getall);
      if (nb < 0) {
        return nb;
      }
      nbytes += nb;
    }

    // Here's a difference from the @c TBranchElement base class.
    // First, we need to pick up the current state of the
    // @c fConvDontReset flag (and clear it in the process).
    // Instead of looping over branches here, we factor out
    // that part into @c ReadSubBranches.
    bool dont_reset = fConvDontReset;
    fConvDontReset = false;
    switch(fSTLtype) {
    case TClassEdit::kSet:
    case TClassEdit::kMultiSet:
    case TClassEdit::kMap:
    case TClassEdit::kMultiMap:
      break;
    default:
      // Read non-container composite and list/vector.
      // Associative containers will get entirely read
      // within the @c GetEntry call above (when it calls @c ReadLeaves).
      nbytes += ReadSubBranches (entry, getall, dont_reset);
      break;
    }
  }
  else {
    // -- Terminal branch.
    // Difference from base implementation: pick up the dont_reset flag here.
    bool dont_reset = fConvDontReset;
    fConvDontReset = false;

    if (fBranchCount && (fBranchCount->GetReadEntry() != entry)) {
      Int_t nb = fBranchCount->TBranch::GetEntry(entry, getall);
      if (nb < 0) {
        return nb;
      }
      nbytes += nb;
    }
    Int_t nb = 0;

    // The base implementation just calls @c TBranch::GetEntry here.
    // But we need to pay attention to the dont_reset flag.
    // If it's set, we can't call @c TBranch::GetEntry (because
    // that would reset the buffer pointer back to the
    // beginning of the container).  Instead, we pick up the
    // buffer pointer and call @c ReadLeaves `by hand'.
    if (dont_reset) {
      TBasket* basket = (TBasket*)GetBasket(GetReadBasket());
      TBuffer* buffer = basket->GetBufferRef();
      Int_t bufbegin = buffer->Length();
      (this->*fReadLeaves) (*buffer);
      nb = buffer->Length() - bufbegin;
    }
    else
      nb = TBranch::GetEntry (entry, getall);

    // Rest is the same as the base implementation.
    if (nb < 0) {
      return nb;
    }
    nbytes += nb;
  }
  
  if (fTree->Debug() > 0) {
    if ((entry >= fTree->GetDebugMin()) && (entry <= fTree->GetDebugMax())) {
      Info("GetEntry", "%lld, branch=%s, nbytes=%d", entry, GetName(), nbytes);
    }
  }
  return nbytes;
}


/**
 * @brief Read leaves into I/O buffers for this branch.
 * @param b Root I/O buffer.
 */
void TConvertingBranchElement::ReadLeavesCollectionConverting(TBuffer& b)
{
   // -- Read leaves into i/o buffers for this branch.
   // Case of a collection (fType == 4).
   
   ValidateAddress();
   if (fObject == 0)
   {
      // We have nowhere to copy the data (probably because the data member was
      // 'dropped' from the current schema) so let's no copy it in a random place.
      return;
   }
   
   // STL container master branch (has only the number of elements).
   Int_t n;
   b >> n;
   if ((n < 0) || (n > fMaximum)) {
      if (IsMissingCollection()) {
         n = 0;
         b.SetBufferOffset(b.Length()-sizeof(n));
      } else {
         Error("ReadLeaves", "Incorrect size read for the container in %s\n\tThe size read is %d while the maximum is %d\n\tThe size is reset to 0 for this entry (%lld)", GetName(), n, fMaximum, GetReadEntry());
         n = 0;
      }
   }
   fNdata = n;

   R__PushCache onfileObject(((TBufferFile&)b),fOnfileObject,n);   

   if (!fObject) {
      return;
   }
   // Note: Proxy-helper needs to "embrace" the entire
   //       streaming of this STL container if the container
   //       is a set/multiset/map/multimap (what we do not
   //       know here).
   //       For vector/list/deque Allocate == Resize
   //                         and Commit   == noop.
   // TODO: Exception safety a la TPushPop
   TVirtualCollectionProxy* proxy = GetCollectionProxy();
   TVirtualCollectionProxy::TPushPop helper(proxy, fObject);
   void* alternate = proxy->Allocate(fNdata, true);
   if(fSTLtype != TClassEdit::kVector && proxy->HasPointers() && fSplitLevel > TTree::kSplitCollectionOfPointers ) {
      fPtrIterators->CreateIterators(alternate, proxy);
   } else {
      fIterators->CreateIterators(alternate, proxy);
   }      
   
   //Int_t nbranches = fBranches.GetEntriesFast();
   switch (fSTLtype) {
      case TClassEdit::kSet:
      case TClassEdit::kMultiSet:
      case TClassEdit::kMap:
      case TClassEdit::kMultiMap:
        {
          // Change for conversions:
          // Use @c ReadSubBranches and obey @c fConvDontReset.
          bool dont_reset = fConvDontReset;
          fConvDontReset = false;
          ReadSubBranches (GetReadEntry(), 1, dont_reset);
        }
        break;
      default:
         break;
   }
   //------------------------------------------------------------------------
   // We have split this stuff, so we need to create the the pointers
   //-----------------------------------------------------------------------
   if( proxy->HasPointers() && fSplitLevel > TTree::kSplitCollectionOfPointers )
   {
      TClass *elClass = proxy->GetValueClass();
      
      //--------------------------------------------------------------------
      // The allocation is done in this strange way because ReadLeaves
      // is being called many times by TTreeFormula!!!
      //--------------------------------------------------------------------
      Int_t i = 0;
      if( !fNdata || *(void**)proxy->At( 0 ) != 0 )
         i = fNdata;
      
      for( ; i < fNdata; ++i )
      {
         void **el = (void**)proxy->At( i );
         // coverity[dereference] since this is a member streaming action by definition the collection contains objects and elClass is not null.
         *el = elClass->New();
      }
   }
   
   proxy->Commit(alternate);   
}


void TConvertingBranchElement::ReadLeavesMemberBranchCountConverting(TBuffer& b)
{
   // -- Read leaves into i/o buffers for this branch.
   // For split-class branch, base class branch, data member branch, or top-level branch.
   // which do have a branch count and are not a counter.
   
   R__ASSERT(fStreamerType != TVirtualStreamerInfo::kCounter);

   ValidateAddress();
   if (fObject == 0)
   {
      // We have nowhere to copy the data (probably because the data member was
      // 'dropped' from the current schema) so let's no copy it in a random place.
      return;
   }

   R__PushCache onfileObject(((TBufferFile&)b),fOnfileObject,1);
   // If not a TClonesArray or STL container master branch
   // or sub-branch and branch inherits from tobject,
   // then register with the buffer so that pointers are
   // handled properly.
   if (TestBit(kBranchObject)) {
      b.ResetMap();
      b.MapObject((TObject*) fObject);
   } else if (TestBit(kBranchAny)) {
      b.ResetMap();
      b.MapObject(fObject, fBranchClass);
   }
   
   fNdata = (Int_t) fBranchCount->GetValue(0, 0);
   TStreamerInfo *info = GetInfo();
   if (!info) {
      return;
   }
   // Since info is not null, fReadActionSequence is not null either.
   //b.ReadSequence(*fReadActionSequence, fObject);
   // FIXME!
   // ReadSequence doesn't work here, since it gets structure offsets
   // from TConfiguration, and those haven't been adjusted to take
   // into account the use of the temporary conversion objects.
   // FIXME!
   std::abort();
   //doesn't work
   //info->ReadBuffer (b, (char**)&fObject, fID);
}


/**
 * @branch Read in the subbranches of this branch.
 * @param entry The entry number to read.
 * @param getall If true, read all branches, even if disabled.
 * @param dont_reset If true, don't reset the @c TBuffer read pointer.
 * @return The number of bytes read.
 *
 * This is used to factor out the handling of conversions in containers.
 */
Int_t TConvertingBranchElement::ReadSubBranches(Long64_t entry,
                                                Int_t getall,
                                                bool dont_reset)
{
  Int_t nbytes = 0;
  Int_t nbranches = fBranches.GetEntriesFast();

  if (fConvContainerFlag) {
    if (fNdata > 0) {
      // We're reading a container that needs a conversion.
      // We're going to invert the loop order so that we read
      // a single object at a time,
      // Remember the number of entries in the container, but change
      // the number we advertise to 1.
      UInt_t save_ndata = fNdata;
      fNdata = 1;

      // Get either the container proxy or the clones array, as appropriate.
      TVirtualCollectionProxy* proxy = 0;
      TClonesArray* clones = 0;
      if (fType == 4)
        proxy = GetCollectionProxy();
      else if (fType == 3)
        clones = (TClonesArray*)fObject;
      else
        abort();

      // If we had a proxy, associate it with the container object.
      // This is safe for the case where proxy is null.
      TPushPopSafe helper (proxy,fObject);

      // For each element, we may have to call @c SetAddress for each
      // branch.  This is used to avoid doing this repeatedly
      // with the same address (as would happen if we're reading into
      // the temporary persistent object).
      std::vector<char*> last_addrs (nbranches, (char*)0);

      // Loop over elements.
      for (UInt_t j = 0; j < save_ndata; j++) {
        // Find the address of the element we want to initialize.
        // How we do this depends on whether we have a @c TClonesArray
        // or a proxy.
        char* addr = 0;
        if (proxy)
          addr = (char*)(proxy->At(j));
        else {
          if (!clones) std::abort();
          addr = (char*)((*clones)[j]);
        }

        // The address of the object we want to read.
        // If we have a conversion on the class contained directly
        // in the container, then this will be the temporary persistent
        // object.  Otherwise, it will be the address we found in the
        // last line.
        char* obj = fConvObject ? fConvObject : addr;

        // Loop over branches for a single object.
        for (Int_t i = 0; i < nbranches; ++i) {
          TBranch* branch = (TBranch*) fBranches[i];

          // If this isn't the first element in the container,
          // then we'll need to set the dont_reset flag.
          if (j > 0) {
            TConvertingBranchElement* be =
              dynamic_cast<TConvertingBranchElement*>(branch);
            if (be)
              be->fConvDontReset = true;
          }

          // Find the address for this member,
          char* this_addr = obj + fBranchOffset[i];

          // Call @c SetAddress if it's different from the last one we set.
          if (this_addr != last_addrs[i]) {
            last_addrs[i] = this_addr;
            branch->SetAddress (obj + fBranchOffset[i]);
          }

          // Read the branch.
          Int_t nb = branch->GetEntry(entry, getall);

          // Bookkeeping.
          if (nb < 0) {
            return nb;
          }
          nbytes += nb;
        }

        // End of loop over branches.
        // We just read one complete object.
        // Call the converter, if needed.
        if (fConv)
          fConv->ConvertVoid (addr, obj);
      }

      // End of loop over entries.  Restore @c fNdata.
      fNdata = save_ndata;
    }
  }
  else {
    // Non-container case.
    // We're reading a single object.
    // Loop over branches.
    for (Int_t i = 0; i < nbranches; ++i) {
      TBranch* branch = (TBranch*) fBranches[i];

      // If @c dont_reset is set, we need to propagate it to our children.
      if (dont_reset) {
        TConvertingBranchElement* be =
          dynamic_cast<TConvertingBranchElement*>(branch);
        if (!be) return -1;
        be->fConvDontReset = true;
      }

      // Read the branch.
      Int_t nb = branch->GetEntry(entry, getall);

      // Bookkeeping.
      if (nb < 0) {
        return nb;
      }
      nbytes += nb;
    }

    // Done reading the object.  Call the converter if needed.
    if (fConv)
      fConv->ConvertVoid (fObject, fConvObject);
  }
  return nbytes;
}


/**
 * @brief Read or write this object.
 * @param R__b The Root buffer.
 *
 * We wrap around the base class streamer.
 * If we made any alterations to the persistent data members,
 * we undo those changes here before calling the base streamer.
 * This way the conversion handling won't change the persistent
 * representation of the object.
 */
void TConvertingBranchElement::Streamer(TBuffer& R__b)
{
  TObjArray brsave;
  Int_t type_save = 0;

  if (!R__b.IsReading()) {
    if (fConvOrigType == -1)
      fConvOrigType = fType;

    type_save = fType;
    fType = fConvOrigType;
    if (fConvOrigBranches) {
      brsave = fBranches;
      fBranches = *fConvOrigBranches;
    }
  }

  TBranchElement::Streamer (R__b);

  if (!R__b.IsReading()) {
    fType = type_save;
    if (fConvOrigBranches) {
      fBranches = brsave;
    }
  }
}



/**
 * @brief Initialize data member offsets.
 * (Overridden internal method.)
 */
void TConvertingBranchElement::InitializeOffsets()
{
  // previously in InitializeOffsets, after:
  //  if (fType == 1) {
  //    pClass = branchElem->GetClassPointer();
  //  } else {
  //    pClass = subBranch->GetParentClass();
  //  }
  //
  // i had:
  //
  //  if (fConvClass) pClass = fConvClass;
  //
  // This is a hack so we don't need to modify the base class
  // InitializeOffsets.
  //
  TStreamerElement* branchElem = 0;
  TClass* cl_orig = 0;
  if (fConvClass && fID > -1) {
    TStreamerInfo* si = GetInfo();
    branchElem = si->GetElem(fID);
    if (branchElem) {
      if (branchElem && branchElem->IsBase()) {
        cl_orig = branchElem->GetClassPointer();
        branchElem->Update (cl_orig, fConvClass);
      }
      else
        branchElem = 0;
    }
  }

  if (fConvClass) {
    Int_t nbranches = fBranches.GetEntriesFast();
    for (Int_t subBranchIdx = 0; subBranchIdx < nbranches; ++subBranchIdx) {
      TConvertingBranchElement* br = 
        dynamic_cast<TConvertingBranchElement*> (fBranches[subBranchIdx]);
      if (br) {
        br->fParentClass = fConvClass;
      }
    }
  }

  TBranchElement::InitializeOffsets();

  if (branchElem)
    branchElem->Update (fConvClass, cl_orig);
}


/**
 * @brief Set the address of the object to use for I/O.
 * @param add Object address.
 *
 * Overridden from the base class: if we have a temporary persistent
 * object for conversions, then we pass down the address of that object
 * rather than what we were given.
 */
void TConvertingBranchElement::SetAddress(void* add)
{
   TBranchElement::SetAddress (add);
   if (fConvObject) {
     Int_t nbranches = fBranches.GetEntriesFast();
     for (Int_t i = 0; i < nbranches; ++i) {
       TBranch* abranch = (TBranch*) fBranches[i];
       abranch->SetAddress(fConvObject + fBranchOffset[i]);
     }
   }
}



/**
 * @brief To allow calling Initialize without having to depend on the header.
 */
void TConvertingBranchElement_init()
{
  TConvertingBranchElement::Initialize();
}


/**
 * @brief Reset branch addresses and maybe delete the object.
 *
 * We have this here because the functionality of getting the object
 * is disabled in standard root, for reasons which aren't entirely clear.
 * We want to add the option to turn it on again, to prevent leaks.
 */
void TConvertingBranchElement::ResetAddress()
{
  char* object = fObject;
  char* address = fAddress;
  TBranchElement::ResetAddress();

  if (fgDoDel) {
    fObject = object;
    fAddress = address;

    // This is copied from the disabled code of TBranchElement::ReleaseObject.
    if (fID < 0) {
      // -- We are a top-level branch.
      if (fAddress && (*((char**) fAddress) != fObject)) {
        // The semantics of fAddress and fObject are violated.
        // Assume the user changed the pointer on us.
        if (TestBit(kDeleteObject)) {
          Warning("ReleaseObject", "branch: %s, You have overwritten the pointer to an object which I owned!", GetName());
          Warning("ReleaseObject", "This is a memory leak.  Please use SetAddress() to change the pointer instead.");
          ResetBit(kDeleteObject);
        }
      }
    }

    // Delete any object we may have allocated during a call to SetAddress.
    // (sss - added fAddress check to fix coverity warning)
    if (fAddress && fObject && TestBit(kDeleteObject)) {
      ResetBit(kDeleteObject);
      if (fType == 3) {
        // -- We are a TClonesArray master branch.
        TClonesArray::Class()->Destructor(fObject);
        fObject = 0;
        if ((fStreamerType == TVirtualStreamerInfo::kObjectp) ||
            (fStreamerType == TVirtualStreamerInfo::kObjectP)) {
          // -- We are a pointer to a TClonesArray.
          // We must zero the pointer in the object.
          *((char**) fAddress) = 0;
        }
      } else if (fType == 4) {
        // -- We are an STL container master branch.
        TVirtualCollectionProxy* proxy = GetCollectionProxy();
        if (!proxy) {
          Warning("ResetAddress", "Cannot delete allocated STL container because I do not have a proxy!  branch: %s", GetName());
          fObject = 0;
        } else {
          proxy->Destructor(fObject);
          fObject = 0;
        }
        if (fStreamerType == TVirtualStreamerInfo::kSTLp) {
          // -- We are a pointer to an STL container.
          // We must zero the pointer in the object.
          *((char**) fAddress) = 0;
        }
      } else {
        // We are *not* a TClonesArray master branch and we are *not* an STL container master branch.
        TClass* cl = fBranchClass.GetClass();
        if (!cl) {
          Warning("ResetAddress", "Cannot delete allocated object because I cannot instantiate a TClass object for its class!  branch: '%s' class: '%s'", GetName(), fBranchClass.GetClassName());
          fObject = 0;
        } else {
          cl->Destructor(fObject);
          fObject = 0;
        }
      }
    }

    fObject = 0;
    fAddress = 0;
  }
}


/**
 * @brief Set the deletion flag.
 * @param flag If true, try to delete the branch object on branch deletion.
 */
void TConvertingBranchElement::SetDoDel (bool flag)
{
  fgDoDel = flag;
}




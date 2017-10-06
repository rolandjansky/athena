/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PyROOTTTreePatch.cxx,v 1.5 2009-02-17 19:26:31 ssnyder Exp $
/**
 * @file RootUtils/src/pyroot/PyROOTTTreePatch.cxx
 * @author scott snyder
 * @date Nov 2007
 * @brief Performance improvements and fixes for root's pythonized @c TTree.
 */

// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "RootUtils/PyROOTTTreePatch.h"
#include "Utility.h"
#include "Converters.h"

#include "Python.h"
#include "TClass.h"
#include "TClassRef.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "RootUtils/TBranchElementClang.h"
#include "TLeaf.h"
#include "TLeafObject.h"
#include "TROOT.h"
#include "TPython.h"
#include "TInterpreterValue.h"
#include <map>
#include <vector>


//***************************************************************************
// Most of the PyROOT headers are not exported.
// So we need to copy pieces of some PyROOT declarations here.
//

namespace PyROOT {

class ObjectProxy {
public:
  enum EFlags { kNone = 0x0, kIsOwner = 0x0001, kIsReference = 0x0002, kIsValue = 0x0004 };

  PyObject_HEAD
  void*     fObject;
  int       fFlags;
};

} // namespace PyROOT

using namespace PyROOT;


//=========================================================================



namespace RootUtils {


// Interned string constants that we use for dictionary lookups.
static PyObject* elements_str = 0;
static PyObject* notifier_str = 0;
static PyObject* pynotify_str = 0;


/**
 * @brief Tree notification handler.
 *
 * This is the tree notification handler.  When it's @c Notify
 * method is called, we need to do two things.  First, clear
 * the cached branches/leaves.  Second, call any Python notification
 * handler.
 */
class TreeNotifier
  : public TNamed
{
public:
  /**
   * @brief Constructor.
   * @param tree The tree for which we're creating the notification.
   * @param treeobj_ref A weak reference to the tree for which we're creating
   *                    the notification.  We take ownership of the reference
   *                    object.
   * @param elements The __elements__ dictionary from the tree.
   */
  TreeNotifier (TTree* tree, PyObject* treeobj_ref, PyObject* elements);

  /// Destructor.
  virtual ~TreeNotifier();

  /// Root notification hook.
  virtual Bool_t Notify();

private:
  /// A weak reference to the Python object for the tree.
  /// Making it a reference prevents a reference cycle.
  PyObject* m_treeobj_ref;

  /// The tree's __elements__ dictionary.
  PyObject* m_elements;

  /// Notification tree.
  TObject* m_chain;
};


/**
 * @brief Constructor.
 * @param tree The tree for which we're creating the notification.
 * @param treeobj_ref A weak reference to the tree for which we're creating
 *                    the notification.  We take ownership of the reference
 *                    object.
 * @param elements The __elements__ dictionary from the tree.
 */
TreeNotifier::TreeNotifier (TTree* tree,
                            PyObject* treeobj_ref,
                            PyObject* elements)
  : m_treeobj_ref (treeobj_ref),
    m_elements (elements),
    m_chain (tree->GetNotify())
{
  // Construct a name.
  std::string name = "Notifier for tree ";
  name += tree->GetName();
  SetName (name.c_str());

  // Manage refcounts.
  Py_INCREF (m_elements);

  // We acquire ownership of treeobj_ref.
}


/**
 * @brief Destructor.
 */
TreeNotifier::~TreeNotifier()
{
  Py_XDECREF (m_elements);
  Py_XDECREF (m_treeobj_ref);
}


/**
 * @brief Root notification hook.
 */
Bool_t TreeNotifier::Notify()
{
  // Clear the element cache.
  PyDict_Clear (m_elements);

  // Intern __pynotify__ if needed.
  if (pynotify_str == 0)
    pynotify_str = PyString_InternFromString("__pynotify__");

  // Look for a notification object.
  PyObject* treeobj = PyWeakref_GetObject (m_treeobj_ref);
  if (treeobj) {
    PyObject** dictptr = _PyObject_GetDictPtr (treeobj);
    if (dictptr && *dictptr) {
      PyObject* notobj = PyObject_GetItem (*dictptr, pynotify_str);
      if (notobj) {
        // Got it --- call @c Notify.
        PyObject* ret =
          PyObject_CallMethod (notobj, const_cast<char*> ("Notify"), NULL);
        if (!ret) return 0;
        Py_DECREF (ret);
      }
      else
        PyErr_Clear();
    }
  }

  if (m_chain) m_chain->Notify();
  return true;
}


/**
 * @brief Convert a leaf to a Python value.
 * @param leaf The leaf to convert.
 * @return The Python object.
 */
PyObject* leafToValue (TLeaf* leaf)
{
  if ( 1 < leaf->GetLenStatic() || leaf->GetLeafCount() ) {
    // array types
    std::string typeName = leaf->GetTypeName();
    std::string xname = typeName;
    xname += '*';
    TConverter* pcnv = CreateConverter( xname, leaf->GetNdata() );
    void* address = (void*)leaf->GetValuePointer();
    PyObject* value = pcnv->FromMemory( &address );
    delete pcnv;
      
    return value;
  }
  else {
    // value types
    TConverter* pcnv = CreateConverter( leaf->GetTypeName() );
    void* ptr = leaf->GetValuePointer();
    if (dynamic_cast<TLeafObject*>(leaf))
      ptr = *(void**)ptr;
    
    PyObject* value = pcnv->FromMemory( ptr );
    delete pcnv;

    return value;
  }
}


/**
 * @brief Convert a leaf to a Python value, and enter into the element cache.
 * @param leaf The leaf to convert.
 * @param nameobj Python object for the element name.
 * @param elements The element cache dictionary.
 * @return The Python object.
 */
PyObject*
leafToValueCache (TLeaf* leaf, PyObject* nameobj, PyObject* elements)
{
  // Get a Python object holding the leaf.
  TClass* tleaf_cls = TClass::GetClass ("TLeaf");
  TClass* act_cls = tleaf_cls->GetActualClass (leaf);
  PyObject* leafobj = TPython::ObjectProxy_FromVoidPtr (leaf, act_cls->GetName());
  if (!leafobj)
    return 0;

  // Pack it into a tuple.
  PyObject* elt = PyTuple_Pack (1, leafobj);
  Py_XDECREF (leafobj);
  if (!elt)
    return 0;

  // And add it to the elements dictionary.
  int stat = PyObject_SetItem (elements, nameobj, elt);
  Py_XDECREF(elt);
  if (stat < 0)
    return 0;

  // Now convert the leaf to its value.
  return leafToValue (leaf);
}


/**
 * @brief Convert a branch to a Python value and cache it.
 * @param branch The branch.
 * @param branchobj The branch, as a Python object.
 * @param nameobj Python object for the element name.
 * @param elements The element cache dictionary.
 */
PyObject*
branchToValueCache (TBranch* branch,
                    PyObject* branchobj,
                    PyObject* nameobj,
                    PyObject* elements)
{
  // Get the branch class.
  // Could think about caching the class too --- but probably not worth it.
  TClass* klass = gROOT->GetClass( branch->GetClassName() );
  PyObject* ret = 0;

  // Bind the branch's object to a Python object.
  if ( klass && branch->GetAddress() )
    ret = TPython::ObjectProxy_FromVoidPtr( *(char**)branch->GetAddress(),
                                            klass->GetName() );

  // If we succeeded, add to the cache.
  // A 2-tuple with the branch and the object.
  if (ret) {
    PyObject* elt = PyTuple_Pack (2, branchobj, ret);
    if (!elt) {
      Py_XDECREF(ret);
      return 0;
    }
    if (PyObject_SetItem (elements, nameobj, elt) < 0) {
      Py_XDECREF(elt);
      Py_XDECREF(ret);
      return 0;
    }
  }
  return ret;
}


/**
 * @brief Convert a branch to a Python value and cache it.
 * @param branch The branch.
 * @param nameobj Python object for the element name.
 * @param elements The element cache dictionary.
 */
static PyObject*
branchToValueCache (TBranch* branch,
                    PyObject* nameobj,
                    PyObject* elements)
{
  // First need to make a Python object holding the branch.
  TClass* tbranch_cls = TClass::GetClass ("TBranch");
  TClass* act_cls = tbranch_cls->GetActualClass (branch);
  PyObject* branchobj = TPython::ObjectProxy_FromVoidPtr (branch, act_cls->GetName());
  if (!branchobj)
    return 0;
  PyObject* ret = branchToValueCache (branch, branchobj, nameobj, elements);
  Py_XDECREF (branchobj);
  return ret;
}


/**
 * @brief Enable a @c TBranch and any descendants it might have.
 * @param branch The branch to enable.
 */
static void enableBranch (TBranch* branch)
{
  if (TBranchElement* br = dynamic_cast<TBranchElement*> (branch)) {
    TObjArray* branches = br->GetListOfBranches();
    int nbranches = branches->GetEntriesFast();
    for (int i = 0; i < nbranches; i++) {
      TBranch* b = dynamic_cast<TBranch*> (branches->At(i));
      enableBranch (b);
    }
  }

  if (branch) {
    if (strcmp (branch->GetName(), "TObject") != 0 &&
        strcmp (branch->GetName(), "TMatrixTBase<float>") != 0)
    {
      branch->ResetBit (kDoNotProcess);

      // Saved read entry may not be correct now --- invalidate it.
      branch->ResetReadEntry();
    }
  }
}


/**
 * @brief Enable a branch if not already enabled.
 * @param branch The branch.
 */
void checkEnable (TBranch* branch)
{
  // User has requested this branch for the first time for this tree.
  // If it's not enabled, force it on.
  // Note: TTree::SetBranchStatus has problems if there are multiple
  // branches with the same name (can happen with split branches).
  // So don't use that here.
  if (branch->TestBit (kDoNotProcess)) {
    enableBranch (branch);

    TTree* tree = branch->GetTree();

    // We're likely not at the correct entry.
    // Move to the proper place.
    Long64_t entry = tree->GetReadEntry();

    // Work around the root TChain::GetReadEntry bug.
    // Note that this is fixed in later root versions, so check
    // that we actually need to fix it.
    if (TChain* chain = dynamic_cast<TChain*> (tree)) {
      int treenum = tree->GetTreeNumber();
      if (treenum != -1) {
        Long64_t offs = chain->GetTreeOffset()[treenum];
        if (entry >= 0 && entry < offs)
          entry += offs;
      }
    }

    Long64_t local_entry = tree->LoadTree (entry);
    (void)branch->GetEntry (local_entry);
  }
}


/**
 * @brief getattr implementation for @c TTree.
 * @param args The Python arguments.
 * @return The Python return value.
 */
PyObject* treeGetattr( PyObject*, PyObject* args )
{
  // Allow access to branches/leaves as if they are data members

  // Decode the arguments.
  PyObject* self = 0;
  const char* name = 0;
  if ( ! PyArg_ParseTuple( args, const_cast< char* >( "Os:__getattr__" ),
                           &self, &name ) )
  {
    return 0;
  }
  if (!TPython::ObjectProxy_Check (self))
  {
    PyErr_Format( PyExc_TypeError,
                  "TTree::__getattr__ must be called on a root object" );
    return 0;
  }

  // Avoid wasting time for special python names.
  if (name[0] == '_' && name[1] == '_') {
    PyErr_Format( PyExc_AttributeError,
                  "TTree object has no attribute \'%s\'", name );
    return 0;
  }

  // Intern strings.
  if (elements_str == 0) {
    elements_str = PyString_InternFromString("__elements__");
    notifier_str = PyString_InternFromString("__notifier__");
  }

  // Get our name argument as a py string.
  // This must succeed if the ParseTuple did.
  // Note that this is a borrowed reference.
  PyObject* nameobj = PyTuple_GET_ITEM (args, 1);

  // Find the __elements__ dictionary.  If it's there, try looking
  // up the name there first.
  PyObject** dictptr = _PyObject_GetDictPtr (self);
  PyObject* elements = 0;
  if (dictptr && *dictptr) {
    // Note: returns a borrowed reference.
    elements = PyDict_GetItem (*dictptr, elements_str);
  }
  if (elements) {
    PyObject* elt = PyObject_GetItem (elements, nameobj);
    if (elt) {
      // Got it.  Now figure out what we have, and see if any saved
      // buffer is still valid.
      int len = PyObject_Length (elt);
      if (len == 1) {
        // Leaf.  elt[0] is the leaf.
        // Possible further optimizations:
        //  For an atomic leaf, we can cache the converter across calls.
        //  Not done yet because there's no good place to put the TConverter
        //  (I'm too lazy to make a new type, etc. to represent it to python).
        //  For array leaves the converter and buffer can be reused
        //  only if the size of the array doesn't change.
        PyObject* leafobj = PySequence_GetItem (elt, 0);
        if (TPython::ObjectProxy_Check (leafobj)) {
          TLeaf* leaf =
            (TLeaf*)objectIsA(leafobj)->DynamicCast
              ( TLeaf::Class(),
                TPython::ObjectProxy_AsVoidPtr (leafobj) );
          if (leaf) {
            checkEnable (leaf->GetBranch());
            PyObject* ret = leafToValue (leaf);
            Py_XDECREF (leafobj);
            Py_XDECREF(elt);
            return ret;
          }
        }
        Py_DECREF (leafobj);
      }

      else if (len == 2) {
        // Branch type.
        // elt[0] is the branch, elt[1] is the obj.
        PyObject* branchobj = PySequence_GetItem (elt, 0);
        if (TPython::ObjectProxy_Check (branchobj)) {
          TBranch* branch =
            (TBranch*)objectIsA(branchobj)->DynamicCast
              ( TBranch::Class(),
                TPython::ObjectProxy_AsVoidPtr (branchobj) );
          if (branch) {
            PyObject* objobj = PySequence_GetItem (elt, 1);
            if (TPython::ObjectProxy_Check (objobj)) {
              checkEnable (branch);
              // Now check that the address hasn't moved.
              // If not, we can just return the object as-is.
              // Otherwise, we need to make a new one.
              if (*(char**)branch->GetAddress() != TPython::ObjectProxy_AsVoidPtr(objobj))
                objobj = branchToValueCache (branch, branchobj,
                                             nameobj, elements);
              Py_XDECREF (elt);
              Py_XDECREF (branchobj);
              return objobj;
            }
            Py_DECREF (objobj);
          }
        }
        Py_DECREF (branchobj);
      }

      // Something wasn't right.
      // Continue, and remake the cache.
      printf ("drop cache!\n");
      Py_XDECREF (elt);
    }
    else
      PyErr_Clear();
  }

  // get hold of actual tree
  TTree* tree =
    (TTree*)objectIsA(self)->DynamicCast
      ( TTree::Class(), TPython::ObjectProxy_AsVoidPtr (self) );

  if (!elements) {
    // Need to make the __elements__ dict.
    PyErr_Clear();
    elements = PyDict_New();
    if (!elements)
      return 0;
    int stat = PyObject_SetAttr (self, elements_str, elements);
    Py_XDECREF (elements);
    if (stat < 0)
      return 0;

    // Make the notifier too.
    PyObject* treeobj_ref = PyWeakref_NewRef (self, 0);
    if (!treeobj_ref)
      return 0;
    TNamed* notifier = new TreeNotifier (tree, treeobj_ref, elements);
    PyObject* notobj = TPython::ObjectProxy_FromVoidPtr (notifier, "TNamed");
    setOwnership (notobj, true);
    stat = PyObject_SetAttr (self, notifier_str, notobj);
    Py_XDECREF (notobj);
    if (stat < 0)
      return 0;
    tree->SetNotify (notifier);
  }

  // Search for branch first (typical for objects)
  TBranch* branch = tree->GetBranch( name );
  if ( branch ) {
    checkEnable (branch);
    PyObject* ret = branchToValueCache (branch, nameobj, elements);
    if (ret)
      return ret;
  }

  // If not, try leaf
  TLeaf* leaf = tree->GetLeaf( name );
  if ( leaf ) {
    checkEnable (leaf->GetBranch());
    return leafToValueCache (leaf, nameobj, elements);
  }

  // Didn't find it.
  PyErr_Format( PyExc_AttributeError,
                "\'%s\' object has no attribute \'%s\'",
                tree->IsA()->GetName(), name );
  return 0;
}


/**
 * @brief Implementation for pythonization of @c TTree::SetNotify.
 * @param args The Python arguments.
 */
PyObject* treeSetNotify (PyObject*, PyObject* args)
{
  // Decode the objects --- the tree and the notifier object.
  PyObject* self = 0;
  PyObject* obj = 0;
  if ( ! PyArg_ParseTuple( args, const_cast< char* >( "OO:SetNotify" ),
                           &self, &obj ) )
    return 0;

  // Intern string if needed.
  if (pynotify_str == 0)
    pynotify_str = PyString_InternFromString("__pynotify__");

  // Install the object.
  int stat = PyObject_SetAttr (self, pynotify_str, obj);
  if (stat < 0)
    return 0;

  Py_INCREF (Py_None);
  return Py_None;
}


/**
 * @brief Implementation for pythonization of @c TTree::SetNotify.
 * @param args The Python arguments.
 */
PyObject* treeGetNotify (PyObject*, PyObject* args)
{
  // Decode the objects --- the tree.
  PyObject* self = 0;
  if ( ! PyArg_ParseTuple( args, const_cast< char* >( "O:SetNotify" ),
                           &self ) )
    return 0;

  // Intern string if needed.
  if (pynotify_str == 0)
    pynotify_str = PyString_InternFromString("__pynotify__");

  // Retrieve the notifier.
  PyObject* ret = PyObject_GetAttr (self, pynotify_str);
  if (!ret) {
    PyErr_Clear();
    Py_INCREF (Py_None);
    ret = Py_None;
  }
  return ret;
}


/**
 * @brief Pythonization of @c TBranch::SetAddress.
 * @param args The Python arguments.
 */
PyObject* branchSetAddress (PyObject*, PyObject* args)
{
  // Decode arguments --- the branch and the buffer.
  PyObject* self = 0;
  PyObject* address = 0;
  if ( ! PyArg_ParseTuple( args, const_cast< char* >( "OO:SetBranchAddress" ),
                           &self, &address ) )
    return 0;

  // The branch as a Root object.
  TBranch* branch = 0;
  if (TPython::ObjectProxy_Check (self)) {
    branch =  (TBranch*)objectIsA(self)->DynamicCast
      ( TBranch::Class(),
        TPython::ObjectProxy_AsVoidPtr(self) );
  }

  if ( ! branch ) {
    PyErr_SetString( PyExc_TypeError,
                     "TBranch::SetAddress must be called with a "
                     "TBranch instance as first argument" );
    return 0;
  }

  // Convert the buffer argument to an address.
  void* buf = 0;
  if ( TPython::ObjectProxy_Check( address ) ) {
    if ( ((ObjectProxy*)address)->fFlags & ObjectProxy::kIsReference )
      buf = (void*)((ObjectProxy*)address)->fObject;
    else
      buf = (void*)&((ObjectProxy*)address)->fObject;
  } else
    RootUtils::GetBuffer( address, '*', 1, buf, kFALSE );

  // Make the call and return.
  if ( buf != 0 )
    branch->SetAddress( buf );

  Py_INCREF( Py_None );
  return Py_None;
}


/**
 * @brief Helper to install a method in a Python class.
 * @param pyclass The Python class.
 * @param pdef A PyMethodDef object.
 * @param name The method name.
 * @param cfunc The C function to install.
 */
void installMethod (PyObject* pyclass,
                    PyMethodDef& pdef, 
                    const char* name,
                    PyCFunction cfunc)
{
  pdef.ml_name = const_cast<char*> (name);
  pdef.ml_meth = cfunc;
  pdef.ml_flags = METH_VARARGS;
  pdef.ml_doc = 0;

  PyObject* func = PyCFunction_New (&pdef, 0);
  PyObject* method = PyMethod_New (func, 0, pyclass);
  Bool_t isOK = PyObject_SetAttrString (pyclass, pdef.ml_name, method) == 0;

  if (PyErr_Occurred())
    fprintf (stderr, "Py error");
  else if (!isOK) {
    fprintf (stderr, "Could not add method %s\n", name);
  }
}


/**
 * @brief Install the PyROOT patches.
 * @param tree_pyclass The @c TTree Python class.
 * @param chain_pyclass The @c TChain Python class.
 * @param branch_pyclass The @c TBranch Python class.
 */
void PyROOTTTreePatch::Initialize (PyObject* tree_pyclass,
                                   PyObject* /*chain_pyclass*/,
                                   PyObject* branch_pyclass)
{
# define INSTALL_METHOD(pyclass, name, func) do {  \
  static PyMethodDef pdef;                     \
  installMethod (pyclass, pdef, name, (PyCFunction)func); \
  } while(0)

  INSTALL_METHOD (tree_pyclass, "__getattr__", treeGetattr);
  INSTALL_METHOD (tree_pyclass, "SetNotify",   treeSetNotify);
  INSTALL_METHOD (tree_pyclass, "GetNotify",   treeGetNotify);
  INSTALL_METHOD (branch_pyclass, "SetAddress",   branchSetAddress);
#undef INSTALL_METHOD
}


} // namespace RootUtils

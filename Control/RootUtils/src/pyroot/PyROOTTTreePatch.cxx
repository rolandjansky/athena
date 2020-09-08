/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootUtils/src/pyroot/PyROOTTTreePatch.cxx
 * @author scott snyder
 * @date Nov 2007, revised Aug 2020
 * @brief Additions to root's pythonized @c TTree.
 */

// Called from python, so only excuted single-threaded (GIL).
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "RootUtils/PyROOTTTreePatch.h"
#include "RootUtils/PyAthenaGILStateEnsure.h"
#include "Utility.h"

#include "Python.h"
#include "TTree.h"
#include "TBranch.h"
#include "TPython.h"
#include "TNotifyLink.h"


//***************************************************************************
// Most of the PyROOT headers are not exported.
// So we need to copy pieces of some PyROOT declarations here.
//

namespace PyROOT {

class CPPInstance {
public:
#if ROOT_VERSION_CODE < ROOT_VERSION(6,22,0)
  enum EFlags { kIsReference = 0x0002 };
#else
  enum EFlags { kIsReference = 0x0008 };
#endif

  PyObject_HEAD
  void*     fObject;
  int       fFlags;
};

} // namespace PyROOT

using namespace PyROOT;


//=========================================================================



namespace RootUtils {


// Interned string constants that we use for dictionary lookups.
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
  : public TNotifyLinkBase
{
public:
  /**
   * @brief Constructor.
   * @param tree The tree for which we're creating the notification.
   * @param treeobj_ref A weak reference to the tree for which we're creating
   *                    the notification.  We take ownership of the reference
   *                    object.
   */
  TreeNotifier (TTree* tree, PyObject* treeobj_ref);

  /// Destructor.
  virtual ~TreeNotifier();

  /// Root notification hook.
  virtual Bool_t Notify() override;

  /// Called when the tree we're attached to is deleted.
  virtual void Clear (Option_t* /*option*/ = "") override;


private:
  /// A weak reference to the Python object for the tree.
  /// Making it a reference prevents a reference cycle.
  PyObject* m_treeobj_ref;

  /// Notification tree.
  TObject* m_chain;

  /// The tree to which we're attached.
  TTree* m_tree;
};


/**
 * @brief Constructor.
 * @param tree The tree for which we're creating the notification.
 * @param treeobj_ref A weak reference to the tree for which we're creating
 *                    the notification.  We take ownership of the reference
 *                    object.
 */
TreeNotifier::TreeNotifier (TTree* tree,
                            PyObject* treeobj_ref)
  : m_treeobj_ref (treeobj_ref),
    m_chain (tree->GetNotify()),
    m_tree (tree)
{
  // We acquire ownership of treeobj_ref.
}


/**
 * @brief Destructor.
 */
TreeNotifier::~TreeNotifier()
{
  RootUtils::PyGILStateEnsure gil;
  if (m_tree) {
    // Clear the reference from the tree to this object.
    m_tree->SetNotify (m_chain);
  }
  Py_XDECREF (m_treeobj_ref);
}


/**
 * @brief Root notification hook.
 */
Bool_t TreeNotifier::Notify()
{
  RootUtils::PyGILStateEnsure gil;

  // Intern __pynotify__ if needed.
  if (pynotify_str == 0)
    pynotify_str = PyROOT_PyUnicode_InternFromString("__pynotify__");

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


/// Called when the tree we're attached to is deleted.
void TreeNotifier::Clear (Option_t* /*option = ""*/)
{
  m_tree = nullptr;
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

  // Intern strings if needed.
  if (pynotify_str == 0) {
    pynotify_str = PyROOT_PyUnicode_InternFromString("__pynotify__");
  }
  if (notifier_str == 0) {
    notifier_str = PyROOT_PyUnicode_InternFromString("__notifier__");
  }

  // Set up notifier.
  if (!PyObject_HasAttr (self, notifier_str)) {
    // get hold of actual tree
    void* vp = TPython::CPPInstance_AsVoidPtr (self);
    TTree* tree =
      (TTree*)objectIsA(self)->DynamicCast ( TTree::Class(), vp );

    PyObject* treeobj_ref = PyWeakref_NewRef (self, 0);
    if (!treeobj_ref)
      return 0;
    TObject* notifier = new TreeNotifier (tree, treeobj_ref);
    PyObject* notobj = TPython::CPPInstance_FromVoidPtr (notifier, "TObject");
    setOwnership (notobj, true);
    int stat = PyObject_SetAttr (self, notifier_str, notobj);
    Py_XDECREF (notobj);
    if (stat < 0)
      return 0;
    tree->SetNotify (notifier);
  }

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
    pynotify_str = PyROOT_PyUnicode_InternFromString("__pynotify__");

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
  if (TPython::CPPInstance_Check (self)) {
    branch =  (TBranch*)objectIsA(self)->DynamicCast
      ( TBranch::Class(),
        TPython::CPPInstance_AsVoidPtr(self) );
  }

  if ( ! branch ) {
    PyErr_SetString( PyExc_TypeError,
                     "TBranch::SetAddress must be called with a "
                     "TBranch instance as first argument" );
    return 0;
  }

  // Convert the buffer argument to an address.
  void* buf = 0;
  if ( TPython::CPPInstance_Check( address ) ) {
    if ( ((CPPInstance*)address)->fFlags & CPPInstance::kIsReference )
      buf = (void*)((CPPInstance*)address)->fObject;
    else
      buf = (void*)&((CPPInstance*)address)->fObject;
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
#if PY_VERSION_HEX >= 0x03000000
  PyObject* method = PyInstanceMethod_New (func);
#else
  PyObject* method = PyMethod_New (func, 0, pyclass);
#endif
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

  INSTALL_METHOD (tree_pyclass, "SetNotify",   treeSetNotify);
  INSTALL_METHOD (tree_pyclass, "GetNotify",   treeGetNotify);
  INSTALL_METHOD (branch_pyclass, "SetAddress",   branchSetAddress);
#undef INSTALL_METHOD
}


} // namespace RootUtils

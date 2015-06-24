/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This file intentionally sparse on comments
// It only exists for dictionary generation.
// Please see quick_retrieve.cxx for detailed information on these functions.


#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE
#endif
#include <Python.h>

#include <CoolKernel/ChannelSelection.h>
#include <CoolKernel/IObject.h>
#include <CoolKernel/IObjectIterator.h>
#include <CoolKernel/IFolder.h>
#include <CoolKernel/IDatabase.h>
#include <CoolKernel/IDatabaseSvc.h>

#include <CoolKernel/IRecordSelection.h>
#include <CoolKernel/FieldSelection.h>

#include <CoolApplication/DatabaseSvcFactory.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

using cool::DatabaseSvcFactory;
using cool::IDatabasePtr;
using cool::IObject;
using cool::IFolder;
using cool::IFolderPtr;
using cool::ChannelSelection;
using cool::IObjectIteratorPtr;
using cool::IObjectVectorPtr;
using cool::ValidityKey;

#include <string>
#include <vector>
using std::string;
using std::vector;

namespace quick_retrieve_dict {
struct dict {
	//cool::IRecordSelection* dummy_irecordselection;
	const vector<const cool::IRecordSelection*> dummy_recordselection_vector;
};
}

// In pythonic_coracool.cxx

const cool::RecordSpecification 
    get_coracool_payload_spec(IDatabasePtr cooldb, string folder);

PyObject *browse_coracool(IDatabasePtr cooldb, string folder, 
                          ValidityKey since, ValidityKey until,
                          const ChannelSelection &cs = ChannelSelection::all(), 
                          const char *tag="",
                          PyObject *to_fetch = NULL,
                          PyObject *object_converter = NULL,
                          PyObject *inner_object_converter = NULL,
                          PyObject *iovkey_wrapper = NULL);
                          
// In quick_retrieve.cxx

PyObject* quick_retrieve(const IObjectIteratorPtr& objects, 
                         PyObject *object_converter,
                         PyObject *to_fetch = NULL,
                         const long max_records = -1,
                         const bool with_channel = true,
                         const bool loud = false,
                         PyObject *iovkey_wrapper = NULL,
                         PyObject *channel_name_mapping = NULL,
                         const bool with_time = false,
                         const bool as_unicode = false);

cool::FieldSelection* make_fieldselection(
	const std::string& name, 
	const cool::StorageType::TypeId typeId, 
	cool::FieldSelection::Relation relation, 
	PyObject* refValue);

vector<const cool::IRecordSelection*> make_selection_vector();

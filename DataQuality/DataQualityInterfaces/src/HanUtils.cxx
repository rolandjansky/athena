/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TList.h"
#include "TObjArray.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TROOT.h"
#include <string>
#include "DataQualityInterfaces/HanUtils.h"

namespace dqi {
TSeqCollection* newTList( const char *name, TObject *obj )
{
  TList *ret = new TList();
  ret->SetName(name);
  if (obj != 0)
    ret->Add(obj);
  return ret;
}

TSeqCollection* newTObjArray( const char *name, TObject *obj, Int_t size )
{
  TObjArray *ret = new TObjArray(size);
  ret->SetName(name);
  if (obj != NULL)
    ret->Add(obj);
  return ret;
}

TKey* getObjKey( TDirectory* dir, const std::string path )
{
  if( dir == 0 )
    return 0;

  std::string::size_type i = path.find_first_of('/');
  if( i != std::string::npos ) {
    std::string dName( path, 0, i );
    std::string pName( path, i+1, std::string::npos );
    if( dName != "" ) {
      TDirectory* subDir = dir->GetDirectory(dName.c_str());
      if (subDir != 0) {
      	TKey* rv = getObjKey( subDir, pName );
      	//delete subDir;
      	return rv;
      }
      // TKey* key = dir->FindKey( dName.c_str() );
      // if( key != 0 ) {
      //   TDirectory* subDir = dynamic_cast<TDirectory*>( key->ReadObj() );
      //   TKey* rv = getObjKey( subDir, pName );
      // 	//	delete subDir;
      // 	return rv;
      // }
      return 0;
    }
    return getObjKey( dir, pName );
  }

  return dir->FindKey( path.c_str() );
}

DisableMustClean::DisableMustClean() :
  m_lock(root_mutex),
  m_useRecursiveDelete(gROOT->MustClean()) 
{
  gROOT->SetMustClean(false);
}

DisableMustClean::~DisableMustClean()
{
  gROOT->SetMustClean(m_useRecursiveDelete);
}

HanHistogramLink::
HanHistogramLink(TDirectory* dir, const std::string path)
  : m_dir(dir)
  , m_path(path)
{}

TObject* HanHistogramLink::
getObject()
{
  //std::cout << "Getting " << m_dir->GetPath() << " " << m_path << std::endl;
  return m_dir->Get(m_path.c_str());
  /*  std::unique_ptr<TKey> key(getObjKey(m_dir, m_path));
  if (key.get() == 0) return 0;
  TObject* rv = key->ReadObj();
  if (TH1* hptr = dynamic_cast<TH1*>(rv)) { hptr->SetDirectory(0); }
  return rv; */
}

boost::mutex root_mutex;

}

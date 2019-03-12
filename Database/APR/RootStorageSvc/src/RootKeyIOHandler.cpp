/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//    Package    : RootStorageSvc (The POOL project)
//
//    Author     : M.Frank
//====================================================================
#include "RootKeyIOHandler.h"
#include "RootCallEnv.h"
#include "TKey.h"
#include "TFile.h"
#include "TClass.h"
#include "TROOT.h"
#include "TBufferFile.h"
#define G__DICTIONARY
#include "RtypesImp.h"

#include <iostream>

extern "C" void R__zip (Int_t cxlevel, Int_t *nin, char *bufin, Int_t *lout, char *bufout, Int_t *nout);
extern "C" void R__unzip(Int_t *nin, UChar_t *bufin, Int_t *lout, char *bufout, Int_t *nout);
static const Int_t kMAXBUF = 0xffffff;

namespace pool {

  class Key : public TKey {
  public:
    Key(const char *name, const char* title, TClass* cl, const void* obj, int bsiz)    {
      Int_t lbuf, nout, noutot, bufmax, nzip;
      SetName(name);
      SetTitle(title);
      fClassName = cl->GetName();
      fNbytes    = 0;
      fBuffer    = 0;
      if ( !bsiz ) bsiz = gFile->GetBestBuffer();
      fBufferRef = new TBufferFile(TBuffer::kWrite, bsiz);
      fBufferRef->SetParent(gFile);
      fCycle    = gDirectory->AppendKey(this);
      fObjlen   = 0;
      fKeylen   = 0;
      fSeekKey  = 0;
      fSeekPdir = 0;

      fVersion = TKey::Class_Version();
      if (gFile && gFile->GetEND() > TFile::kStartBigFile) fVersion += 1000;

      Streamer(*fBufferRef);         //write key itself
      fKeylen    = fBufferRef->Length();
      auto _obj = const_cast<void*>(obj);
      fBufferRef->MapObject(_obj, cl);    //register obj in map in case of self reference
      cl->WriteBuffer(*fBufferRef, _obj); //write object
      lbuf       = fBufferRef->Length();
      fObjlen    = lbuf - fKeylen;
      Int_t cxlevel = gFile->GetCompressionLevel();
      if (cxlevel && fObjlen > 256) {
        if (cxlevel == 2) cxlevel--;
        Int_t nbuffers = fObjlen/kMAXBUF;
        Int_t buflen = TMath::Max(512,fKeylen + fObjlen + 9*nbuffers + 8); //add 8 bytes in case object is placed in a deleted gap
        fBuffer = new char[buflen];
        char *objbuf = fBufferRef->Buffer() + fKeylen;
        char *bufcur = &fBuffer[fKeylen];
        noutot = 0;
        nzip   = 0;
        for (Int_t i=0;i<=nbuffers;i++) {
          if (i == nbuffers) bufmax = fObjlen -nzip;
          else               bufmax = kMAXBUF;
          R__zip(cxlevel, &bufmax, objbuf, &bufmax, bufcur, &nout);
          if (nout == 0 || nout >= fObjlen) { //this happens when the buffer cannot be compressed
              fBuffer = fBufferRef->Buffer();
              Create(fObjlen);
              fBufferRef->SetBufferOffset(0);
              Streamer(*fBufferRef);         //write key itself again
              return;
          }
          bufcur += nout;
          noutot += nout;
          objbuf += kMAXBUF;
          nzip   += kMAXBUF;
        }
        Create(noutot);
        fBufferRef->SetBufferOffset(0);
        Streamer(*fBufferRef);         //write key itself again
        memcpy(fBuffer,fBufferRef->Buffer(),fKeylen);
        delete fBufferRef; fBufferRef = 0;
      }
      else   {
        fBuffer = fBufferRef->Buffer();
        Create(fObjlen);
        fBufferRef->SetBufferOffset(0);
        Streamer(*fBufferRef);         //write key itself again
      }
    }

    int readObject(void** tobj)  {
      fBufferRef = new TBufferFile(TBuffer::kRead, fObjlen+fKeylen);
      if (!fBufferRef) {
        std::cout << "ReadObj> Cannot allocate buffer: fObjlen =" 
                  << fObjlen << std::endl;
        return 0;
      }
      if (!gFile) return 0;
      fBufferRef->SetParent(gFile);
      if (fObjlen > fNbytes-fKeylen) {
        fBuffer = new char[fNbytes];
        ReadFile();                    //Read object structure from file
        memcpy(fBufferRef->Buffer(),fBuffer,fKeylen);
      }
      else {
        fBuffer = fBufferRef->Buffer();
        ReadFile();                    //Read object structure from file
      }
      UInt_t start, bcnt;
      fBufferRef->SetBufferOffset(sizeof(fNbytes));
      // get version of key
      Version_t kvers = fBufferRef->ReadVersion(&start, &bcnt);
      fBufferRef->SetBufferOffset(fKeylen);

      TClass* cl = 0;
      TDirectory *cursav = gDirectory;
      //      int tagnum = G__defined_tagname(fClassName.Data(), 2);
      //      if ( tagnum > 0 )  {
      //        G__ClassInfo gcl(tagnum);
      //        if ( gcl.IsLoaded() ) {
          cl = ROOT::GetROOT()->GetClass(fClassName.Data());
          //        }
          //      }
      if (!cl) {
        std::cout << "ReadObj> Unknown class:" << fClassName.Data()
                  << "Error reading object." << std::endl;
        return 0;
      }
      // Create an instance of this class
      if ( 0 == *tobj )  {
        *tobj = (char*)cl->New();
      }
      if (!*tobj) {
        std::cout << "ReadObj> Cannot create new object of class "
                  << fClassName.Data() << std::endl;
        return 0;
      }
      if (kvers > 1)
        fBufferRef->MapObject(*tobj,cl);  //register obj in map to handle self reference

      if (fObjlen > fNbytes-fKeylen) {
        char *objbuf = fBufferRef->Buffer() + fKeylen;
        UChar_t *bufcur = (UChar_t *)&fBuffer[fKeylen];
        Int_t nin, nout, nbuf;
        Int_t noutot = 0;
        bool loop = true;
        while (loop) {
          nin  = 9 + ((Int_t)bufcur[3] | ((Int_t)bufcur[4] << 8) | ((Int_t)bufcur[5] << 16));
          nbuf = (Int_t)bufcur[6] | ((Int_t)bufcur[7] << 8) | ((Int_t)bufcur[8] << 16);
          R__unzip(&nin, bufcur, &nbuf, objbuf, &nout);
          if (!nout) {
            break;
          }
          noutot += nout;
          if (noutot >= fObjlen) break;
          bufcur += nin;
          objbuf += nout;
        }
        if (nout) {
          fBufferRef->ReadClassBuffer(cl, *tobj); //read object
          delete [] fBuffer;
        } 
        else {
          delete [] fBuffer;
          cl->Destructor(*tobj);
          *tobj = 0;
          goto CLEAR;
        }
      }
      else {
        fBufferRef->ReadClassBuffer(cl, *tobj); //read object
      }

    CLEAR:
      delete fBufferRef;
      fBufferRef = 0;
      fBuffer    = 0;
      gDirectory = cursav;
      return fObjlen;
    }
  };
}

void pool::RootKeyIOHandler::release()    {
  delete this;
}

int pool::RootKeyIOHandler::destroy(const char* knam) const   {
  if ( knam != 0 && gFile && gFile->IsWritable() ) {
    if ( !gFile->TestBit(TFile::kWriteError) ) {
      if ( strchr(knam,'*') )  {
        TCollection* coll = gDirectory->GetListOfKeys();
        coll->GetSize();
        gDirectory->Delete(knam);
        return 1;
      }
      else {
          TKey* key = (TKey*)gDirectory->GetListOfKeys()->FindObject(knam);
          if (key) {
            key->Delete();
            delete key;
          return 1;
        }
        }
    }
  }
  return 0;
}

int pool::RootKeyIOHandler::write(TClass* cl,
                                  const char*   knam, 
                                  const void*   obj, 
                                  int           opt,
                                  int           bsiz) const
{
  if ( cl )  {
    return write(cl, knam, cl->GetName(), obj, opt, bsiz);
  }
  return 0;
}

int pool::RootKeyIOHandler::write(TClass* cl,
                                  const char*   knam, 
                                  const char*   title,
                                  const void*   obj,
                                  int           opt,
                                  int           bsiz) const
{
  int nbytes = 0;
  if ( !cl )  {
    std::cout << "The specified object class cannot be determined."
              << " Cannot write object:" << knam << std::endl;
    return nbytes;
  }
  if ( gFile && knam != 0 ) {
    if ( gFile->IsWritable() ) {
      if ( !gFile->TestBit(TFile::kWriteError) ) {
        if (opt & TObject::kOverwrite) {
          TKey* key = (TKey*)gDirectory->GetListOfKeys()->FindObject(knam);
          if (key) {
            key->Delete();
            delete key;
          }
        }
        TKey* oldkey = 0;
        if ( opt & TKey::kWriteDelete ) {
          oldkey = (TKey*)gDirectory->GetListOfKeys()->FindObject(knam);
        }
        Key* key = new Key(knam, title, cl, obj, bsiz);
        if ( key->GetSeekKey() ) {
          nbytes = key->WriteFile(0);
          gFile->SumBuffer(nbytes);
          if ( gDebug > 1 ) {
            std::cout << "::writeObject> File " << gFile->GetName() 
                      << " object " << knam << " with length:" << nbytes << " read." << std::endl;
          }
          if ( oldkey ) {
            oldkey->Delete();
            delete oldkey;
          }
          return nbytes;
        }
        std::cout << "::writeObject> File " << gFile->GetName() 
                  << ": Cannot write object." << knam << std::endl;
        gDirectory->GetListOfKeys()->Remove(key);
        delete key;
        return nbytes;
      }
    }
    // Do not print the error if the file already had a SysError.
    std::cout << "::writeObject> File " << gFile->GetName() 
              << " is not writable." << std::endl;
  }
  std::cout << "File is not open. "
            << " Cannot write object:" << (knam?knam:"(null)") << std::endl;
  return nbytes;
}

int pool::RootKeyIOHandler::read(const char* knam, void** obj) const    {
  if ( gFile )  {
    TKey *key = (TKey*)gDirectory->GetListOfKeys()->FindObject(knam);
    if ( key )  {
      return read(key, obj);
    }
    std::cout << "::readObject> File " << gFile->GetName() 
              << " object " << knam << " with key:" << knam << " not found." << std::endl;
    return 0;
  }
  std::cout << "::readObject> File is not open. " 
            << " Cannot read object:" << knam << std::endl;
  return 0;
}

int pool::RootKeyIOHandler::read(TKey* key, void** obj) const   {
  if ( key )  { 
    Key* mkey = (Key*)key;// Extremely ugly, but it works, because
    // of single inheritance and no virtuality.
    int nbytes = mkey->readObject(obj);
    const char* knam = key->GetName();
    if ( gDebug > 1 ) {
      std::cout << "::readObject> File " << gFile->GetName() 
        << " object " << knam << " with length:" << nbytes << " read." << std::endl;
    }
    if ( nbytes > 0 ) {
      return nbytes;
    }
    // Do not print the error if the file already had a SysError.
    std::cout << "::readObject> File " << gFile->GetName() 
      << " object " << knam << " with key:" << knam << " not found." << std::endl;
  }
  return 0;
}

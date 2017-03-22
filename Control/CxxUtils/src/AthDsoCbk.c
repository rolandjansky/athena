/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* implementation for AthDsoCbk */

/* CxxUtils includes */
#include "CxxUtils/AthDsoCbk.h"
#include "CxxUtils/unused.h"

#ifndef __linux__

/* stubs for macosx */
int
ath_dso_cbk_register(ath_dso_event_cbk_t cbk, void *userdata)
{
  if( cbk || userdata ) {}
  return 0;
}

/* unregister a callback function with the dso-cbk framework */
int
ath_dso_cbk_unregister(ath_dso_event_cbk_t cbk)
{
  if( cbk ) {}
  return 0;
}

#else /*linux*/

#define _GNU_SOURCE 1

/* stl includes */
#include <stdlib.h>

#include <stdio.h>
#include <pthread.h>

#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h> /*for sysconf and page size*/

#include "CxxUtils/libcalg/libcalg.h"

/* return the VMem in kilo-bytes */
static
float fetch_vmem (void);

/* internal structure to hold a callback and its associated userdata (if any)
 */
struct ath_dso_event_cbk_impl
{
  ath_dso_event_cbk_t cbk;
  void *data;
};
typedef struct ath_dso_event_cbk_impl ath_dso_event_cbk;

/* list of registered callbacks */
static ArrayList *g_dso_callbacks = NULL;

/* default callback function */
static
int
ath_dso_event_cbk_default(const struct ath_dso_event* event, void *userdata);

struct dlfcn_hook
{
  void *(*dlopen) (const char *file, int mode, void *dl_caller);
  int (*dlclose) (void *handle);
  void *(*dlsym) (void *handle, const char *name, void *dl_caller);
  void *(*dlvsym) (void *handle, const char *name, const char *version,
                   void *dl_caller);
  char *(*dlerror) (void);
  int (*dladdr) (const void *address, Dl_info *info);
  int (*dladdr1) (const void *address, Dl_info *info,
                  void **extra_info, int flags);
  int (*dlinfo) (void *handle, int request, void *arg, void *dl_caller);
  void *(*dlmopen) (Lmid_t nsid, const char *file, int mode, void *dl_caller);
  void *pad[4];
};

extern struct dlfcn_hook *_dlfcn_hook __attribute__ ((nocommon));

void *ath_dlopen( const char *fname, int mode, void *dl_caller );
int ath_dlclose( void *handle );
void *ath_dlsym( void *handle, const char *name, void *dl_caller );
void *ath_dlvsym( void *handle, const char *name, const char *version, void *dl_caller );
char *ath_dlerror( void );
int ath_dladdr( const void *address, Dl_info *info );
int ath_dladdr1( const void *address, Dl_info *info, void **extra_info, int flags );
int ath_dlinfo( void *handle, int request, void *arg, void *dl_caller );
void *ath_dlmopen( Lmid_t nsid, const char *file, int mode, void *dl_caller );

static struct dlfcn_hook ath_dl_hook = {
  ath_dlopen, ath_dlclose, ath_dlsym, ath_dlvsym, ath_dlerror,
  ath_dladdr, ath_dladdr1, ath_dlinfo, ath_dlmopen, { 0, 0, 0, 0 }
};
static pthread_mutex_t ath_dl_hook_lock = PTHREAD_MUTEX_INITIALIZER; 

void* 
ath_dlopen( const char *fname, int mode, void *UNUSED(dl_caller) ) 
{
  struct ath_dso_event dso_evt;
  int idx;
  void *result;

  result = NULL;
  pthread_mutex_lock(&ath_dl_hook_lock);

  _dlfcn_hook = 0;
  dso_evt.fname = fname;
  dso_evt.step = 0;
  for (idx=0; idx != g_dso_callbacks->length; ++idx) {
    ath_dso_event_cbk* cbk = g_dso_callbacks->data[idx];
    (*cbk->cbk)(&dso_evt, cbk->data);
  }

  result = dlopen( fname, mode );

  dso_evt.step = 1;
  for (idx=g_dso_callbacks->length; idx != 0; --idx) {
    ath_dso_event_cbk* cbk = g_dso_callbacks->data[idx-1];
    (*cbk->cbk)(&dso_evt, cbk->data);
  }

  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);

  return result;
}

int
ath_dlclose( void *handle ) 
{
  int result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dlclose( handle );
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

void*
ath_dlsym( void *handle, const char *name, void *UNUSED(dl_caller) ) 
{
  void *result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dlsym( handle, name );
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

void*
ath_dlvsym( void *handle, const char *name, const char *version, void *UNUSED(dl_caller) ) 
{
  void *result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dlvsym( handle, name, version );
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

char*
ath_dlerror( void ) 
{
  char *result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dlerror();
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

int
ath_dladdr( const void *address, Dl_info *info ) 
{
  int result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dladdr( address, info );
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

int
ath_dladdr1( const void *address, Dl_info *info, void **extra_info, int flags ) 
{
  int result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dladdr1( address, info, extra_info, flags );
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

int
ath_dlinfo( void *handle, int request, void *arg, void *UNUSED(dl_caller) ) 
{
  if( dl_caller ) {}
  int result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dlinfo( handle, request, arg );
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

void*
ath_dlmopen( Lmid_t nsid, const char *file, int mode, void *UNUSED(dl_caller) ) 
{
  if( dl_caller ) {}
  void *result = 0;
  pthread_mutex_lock(&ath_dl_hook_lock);
  _dlfcn_hook = 0;
  result = dlmopen( nsid, file, mode );
  _dlfcn_hook = &ath_dl_hook;
  pthread_mutex_unlock(&ath_dl_hook_lock);
  return result;
}

void
ath_dl_hook_install() 
{
  pthread_mutex_init(&ath_dl_hook_lock, NULL);
  /*printf("AthDsoCbk: installing mutex in tid [%d]\n", pthread_self());*/
  /*   printf ("AthDsoCbk: setting dlopen hooks\n" ); */
  _dlfcn_hook = &ath_dl_hook;
}

void ath_dl_hook_release ()
{
  _dlfcn_hook = 0;
  /*printf ("AthDsoCbk: releasing library\n");*/
  pthread_mutex_destroy(&ath_dl_hook_lock);
}

float
fetch_vmem (void)
{
  unsigned siz, rss, shd;
  FILE *fp = fopen ("/proc/self/statm", "r");
  if (fp == NULL) {
    return -999;
  }
  
  /* FIXME: error handling... */
  (void)fscanf(fp, "%80u%80u%80u", &siz, &rss, &shd);
  fclose (fp);

  double pg_sz = sysconf(_SC_PAGESIZE);
  return (pg_sz * siz)/ (float)1024;
}

/* default callback function */
int
ath_dso_event_cbk_default(const struct ath_dso_event* evt, void *userdata)
{
  int pid = getpid();
  float vmem = fetch_vmem();
  if (userdata) {}
  printf ("AthLdPreload: [%d] loading [%s] vmem = [%8.3f] (%d)",
	  pid, evt->fname, vmem, evt->step);

  return 0;
}

/* register a callback function with the dso-cbk framework 
 * @return 0 on success
 *        -1 on failure
 */
int
ath_dso_cbk_register(ath_dso_event_cbk_t cbk, void *userdata)
{
  ath_dso_event_cbk *dso_callback = (ath_dso_event_cbk*)malloc(sizeof(ath_dso_event_cbk));
  dso_callback->cbk = cbk != NULL
    ? cbk
    : ath_dso_event_cbk_default;
  dso_callback->data = userdata;

  if (!arraylist_append(g_dso_callbacks, dso_callback)) {
    free(dso_callback);
    return -1;
  }

  return 0;
}

/* unregister a callback function with the dso-cbk framework */
int
ath_dso_cbk_unregister(ath_dso_event_cbk_t cbk)
{
  int i = 0;
  for (i = 0; i != g_dso_callbacks->length; ++i) {
    ath_dso_event_cbk * elem = g_dso_callbacks->data[i];
    if (elem->cbk == cbk) {
      free(elem); elem = NULL;
      arraylist_remove(g_dso_callbacks, i);
      return 0;
    }
  }
  /* no such callback */
  return -1;
}

/* constructors and destructors */

static void setup() __attribute__ ((constructor));
static void cleanup() __attribute__ ((destructor));

void 
setup()
{
  static int initialized = 0;
  if (!initialized) {
    initialized = 1;

    /* list of registered callbacks */
    g_dso_callbacks = arraylist_new(0);

    ath_dl_hook_install();
    
  }
}

void
cleanup()
{
  static int finalized = 0;
  if (!finalized) {
    finalized = 1;
    ath_dl_hook_release();
    
    /* free list of registered callbacks */
    arraylist_free(g_dso_callbacks);
  }
}

#endif


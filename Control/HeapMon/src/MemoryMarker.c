/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#ifndef HEAPMON_HEAPMON_H
//#define HEAPMON_HEAPMON_H
#if defined(__linux__)

#define _GNU_SOURCE 1

#include <stdio.h>

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#ifdef _FILE_OFFSET_BITS
#undef _FILE_OFFSET_BITS
#endif

#include "Python.h"

#include <execinfo.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <gperftools/malloc_hook_c.h>

//#include <google/malloc_extension_c.h>
void MallocExtension_GetStats(char*, int);
size_t MallocExtension_GetAllocatedSize(void*);
int mm_check(int);
int mm_clearCheckPoint(int);
int mm_depth(int);
int mm_setCheckPoint(void);
void get_trace(long);
void get_key_list(void);
void trace(const char*);


// copied from malloc.c
/* -------------- Early definitions for debugging hooks ---------------- */
/* Define and initialize the hook variables.  These weak definitions must
   appear before any use of the variables in a function (arena.c uses one).  */
#ifndef weak_variable
#ifndef _LIBC
#define weak_variable /**/
#else
/* In GNU libc we want the hook variables to be weak definitions to
   avoid a problem with Emacs.  */
#define weak_variable weak_function
#endif
#endif


/*- data ------------------------------------------------------------------- */

/* holder to locate this shared library */
void mm_dummy() {}

// defined below
static  void tc_MallocHook(const void* ptr, size_t size);
static  void tc_FreeHook(const void* ptr);

static MallocHook_NewHook old_tcMalloc_hook_ = NULL ;
static MallocHook_DeleteHook old_tcFree_hook_ = NULL;

/* forward declarations */
static void mm_Hooks_uninstall();
static void *mm_MallocHook( size_t size, const void *caller );
static void *mm_ReallocHook( void *ptr, size_t size, const void *caller );
static void mm_FreeHook( void *ptr, const void *caller );

/* holders for original hooks */
static void *(*m_OldMallocHook)  ( size_t size, const void *caller );
static void *(*m_OldReallocHook) ( void *ptr, size_t size, const void *caller );
static void (*m_OldFreeHook)     ( void *ptr, const void *caller );

/* available flags */
#define IS_RUNNING          1
#define LEAK_CHECK          2
#define PROFILE             4
#define HIDEMEMADDR         8
#define QUICK              16
#define STL_CHECKS         32
#define FILTER_STL         64
#define IS_MARKING		  128

#define SIZE_MARK 		'\xFB'

static unsigned char FREE_MARKER	= 	'\xCD';
static unsigned char NEW_MARKER 	=	'\xBC';
//static short int MAX_EVENT_NUMBER 	=	10;

static unsigned char USETCMALLOC = 1;

/* module-level configuration flags */
static long gFlags = 0;
static size_t  LOWER_THREE_BITS = (0x01|0x02|0x04);
static long event_counter = 0;

static unsigned long long total_freed = 0;
static unsigned long long total_allocated = 0;
static unsigned long long number_freed = 0;
static unsigned long long number_allocated = 0;
static unsigned long long number_zero_allocated = 0;
static unsigned long long number_zeroptr_freed = 0;
static struct mallinfo mem_struct;

/* handle to output stream used */
static FILE *file_output = 0;

void * old_ptr;
static char buffer[512];



/* _________________________________________________________________________ */
static void mm_report( void ) {
	printf("	MemoryMarker->mm_report(): gFlags = %ld\n", gFlags);
}


/* _________________________________________________________________________ */
static void ignore( const char *name ) {
	gFlags = gFlags & ~IS_MARKING;
	printf("	MemoryMarker->ignore(): IS_MARKING flag dropped. gFlags = %ld\n", gFlags);
}


/* _________________________________________________________________________ */
static void mm_Hooks_save() {
	switch(USETCMALLOC){
	case 0:
		/* store old glibc hooks in buffer for reset later; prevent storing ourselves in
		   case of a double-call by the user (e.g. from python) */
		//printf("	MemoryMarker->mm_Hooks_save ptmalloc gFlags = %u\n", gFlags);

		   /*if ( __malloc_hook != mm_MallocHook )
		      m_OldMallocHook  = __malloc_hook;
		   if ( __realloc_hook != mm_ReallocHook )
		      m_OldReallocHook = __realloc_hook; */
		   if ( __free_hook != mm_FreeHook )
		      m_OldFreeHook    = __free_hook;
		break;
	case 1:
		printf("	MemoryMarker->mm_Hooks_save tcmalloc gFlags = %ld\n", gFlags);
		break;
	default:
		printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
	}
}

/* _________________________________________________________________________ */
static long configure( long flags ) {
	gFlags |= IS_RUNNING;
	gFlags |= flags;

	printf("	MemoryMarker->configure(): IS_RUNNING flag raised. gFlags = %ld\n", gFlags);

	//setup();
	switch(USETCMALLOC){
	case 0:
		mm_Hooks_save();
		break;
	case 1:
		printf("	MemoryMarker->configure() tcmalloc gFlags = %ld\n", gFlags);
		break;
	default:
		printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
	}
	return gFlags;
}

static void set_marker(char m) {
	FREE_MARKER = m;
}

static void get_test(long lkey) {

	struct mallinfo mem_struct;

	switch(USETCMALLOC){
	case 0:
			printf("MemoryMarker->get_test()->Malloc Stats:\n");
			printf("--------------------------MALLOC INFO --------------\n");
			malloc_stats();

			mem_struct = mallinfo();
			printf("	sbrk-Allocated Memory( ARENA) size: %u\n", mem_struct.arena);
			printf(" 	MMAP-Allocated Memory: %u\n", mem_struct.hblkhd);
			printf(" 	Number of chunks allocated by MMAP: %u\n", mem_struct.hblks);
			printf(" 	MALLOC-In Use: %u\n", mem_struct.uordblks );
			printf(" 	MALLOC-Free: %u\n", mem_struct.fordblks);
			printf(" 	Number of chunks from OS not in use by MALLOC: %u\n", mem_struct.ordblks);
			printf("    Top-releasable chunk keepcost: %u\n", mem_struct.keepcost);

			printf("-----------------------------------------------------\n\n");

			/*
			printf("   gnu_libc_version= %s\n",gnu_get_libc_version());
			printf("------------------Sizes and Ranges of Integer Types-------------------\n");
			printf(" NUMBER OF BITS in types: \n");

			printf("   CHAR_BIT=%u, SHRT_BIT=%u, INT_BIT=%u, LONG_BIT=%u, LONG_LONG_BIT=%u\n",
			             CHAR_BIT, sizeof(signed short int)*CHAR_BIT,sizeof(signed int)*CHAR_BIT,
			         sizeof(signed long int)*CHAR_BIT, sizeof(signed long long int)*CHAR_BIT);
			printf(" \n");
			printf("   sizeof(size_t)=%u\t\tsizeof(ptrdiff_t)=%u\n",sizeof(size_t), sizeof(ptrdiff_t));
			printf(" TYPE RANGES:\n");
			printf("   CHAR_MIN=%d\t\tCHAR_MAX=%d\t\tUCHAR_MAX=%u\n", CHAR_MIN, CHAR_MAX, UCHAR_MAX);
			printf("   SHRT_MIN=%d\t\tSHRT_MAX=%d\t\tUINT_MAX=%u\n", SHRT_MIN, SHRT_MAX, USHRT_MAX);
			printf("   INT_MIN=%d\t\tINT_MAX=%d\t\tUINT_MAX=%u\n", INT_MIN, INT_MAX, UINT_MAX);
			printf("   LONG_MIN=%e\t\tLONG_MAX=%e\tULONG_MAX=%e\n", LONG_MIN*1.0, LONG_MAX*1.0,ULONG_MAX*1.0 );
			printf("   LONG_LONG_MIN=%e\tLONG_LONG_MAX=%e\tULONG_LONG_MAX=%e\n", LONG_LONG_MIN*1.0, LONG_LONG_MAX*1.0, ULONG_LONG_MAX*1.0);
			printf("----------------------------------------------------------------------\n");
			*/
		break;
	case 1:
		mem_struct = mallinfo();
		printf("	MemoryMarker->get_test tcmalloc gFlags = %ld\n", gFlags);
		printf("	sbrk-Allocated Memory( ARENA) size: %u\n", mem_struct.arena);
		//printf(" 	MMAP-Allocated Memory: %u\n", mem_struct.hblkhd);
		//printf(" 	Number of chunks allocated by MMAP: %u\n", mem_struct.hblks);
		printf(" 	MALLOC-In Use: %u\n", mem_struct.uordblks );
		printf(" 	MALLOC-Free: %u\n", mem_struct.fordblks);
		//printf(" 	Number of chunks from OS not in use by MALLOC: %u\n", mem_struct.ordblks);
		//printf("    Top-releasable chunk keepcost: %u\n", mem_struct.keepcost);
		break;
	default:
		printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
	}

}

static void store_malloc_info() {
	//printf("MemoryMarker->store_malloc_info() store malloc data into file:\n");
	malloc_stats();
}

static void show_malloc_info() {

	switch(USETCMALLOC){
	case 0:
		printf("\n--------- GLIBC ptmalloc( --stdcmalloc) INFO --------\n");
		malloc_stats();
		mem_struct = mallinfo();
		printf("  SBRK-allocated memory( ARENA) size: %u bytes\n", mem_struct.arena);
		printf("  MMAP-allocated memory: %u bytes\n", mem_struct.hblkhd);
		printf("  MMAP-allocated number of chunks: %u\n", mem_struct.hblks);
		printf("  MALLOC-In-use: %u bytes \n", mem_struct.uordblks );
		printf("  MALLOC-Free: %u bytes\n", mem_struct.fordblks);
		printf("  Number of chunks from OS not in use by MALLOC: %u \n", mem_struct.ordblks);
		printf("  TOP-releasable chunk keepcost: %u bytes \n\n", mem_struct.keepcost);

		printf("\n-------------- HEAPMON MEMORY MARKER SPECIFIC INFO -------\n");
		printf(" Cumulative allocated memory size: %llu bytes\n", total_allocated);
		printf(" Cumulative deallocated memory: %llu bytes \n", total_freed);
		printf(" Number of allocations: %llu bytes \n", number_allocated);
		printf(" Number of deallocations: %llu  \n", number_freed);
		printf(" Number of zero allocations: %llu\n", number_zero_allocated);
		printf(" Number of zero ptr deallocations: %llu\n", number_zeroptr_freed);
		printf("-----------------------------------------------------------\n\n");
		break;
	case 1:

		mem_struct = mallinfo();
		printf("\n-------------- TCMALLOC INFO -------------------------\n");
		printf("	MemoryMarker->get_test tcmalloc gFlags = %ld\n", gFlags);
		printf("	SBRK-allocated Memory( ARENA) size: %u bytes\n", mem_struct.arena);
		//printf(" 	MMAP-Allocated Memory: %u\n", mem_struct.hblkhd);
		//printf(" 	Number of chunks allocated by MMAP: %u\n", mem_struct.hblks);
		printf(" 	MALLOC-In Use: %u bytes \n", mem_struct.uordblks );
		printf(" 	MALLOC-Free: %u bytes \n", mem_struct.fordblks);
		//printf(" 	Number of chunks from OS not in use by MALLOC: %u\n", mem_struct.ordblks);
		//printf("    Top-releasable chunk keepcost: %u\n", mem_struct.keepcost);
		printf("-----------------------------------------------------------\n\n");


		MallocExtension_GetStats(buffer, 512);
		printf("  MemoryMarker->get_malloc_info() tcmalloc gFlags = %ld\n", gFlags);
		printf("  tcmalloc stats: %s", buffer);

		printf("\n-------------- HEAPMON MEMORY MARKER SPECIFIC INFO -------\n");
		printf("	Cumulative allocated memory size: %llu\n", total_allocated);
		printf(" 	Cumulative deallocated memory: %llu\n", total_freed);
		printf(" 	Number of allocations: %llu\n", number_allocated);
		printf(" 	Number of deallocations: %llu\n", number_freed);
		printf(" 	Number of zero allocations: %llu\n", number_zero_allocated);
		printf(" 	Number of zero ptr deallocations: %llu\n", number_zeroptr_freed);
		printf("-----------------------------------------------------------\n\n");


		break;
	default:
		printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
		printf("\n-------------- HEAPMON MEMORY MARKER SPECIFIC INFO -------\n");
		printf("	Cumulative allocated memory size: %llu\n", total_allocated);
		printf(" 	Cumulative deallocated memory: %llu\n", total_freed);
		printf(" 	Number of allocations: %llu\n", number_allocated);
		printf(" 	Number of deallocations: %llu\n", number_freed);
		printf(" 	Number of zero allocations: %llu\n", number_zero_allocated);
		printf(" 	Number of zero ptr deallocations: %llu\n", number_zeroptr_freed);
		printf("-----------------------------------------------------------\n\n");

	}
}

/* _________________________________________________________________________ */
static void mm_Hooks_install() {

   if ( gFlags & IS_RUNNING ) {
		switch(USETCMALLOC){
		case 0:
			//printf("	MemoryMarker->mm_Hooks_install(): mm_Hooks installed. gFlags = %u\n", gFlags);
			__free_hook    = mm_FreeHook;
			/*__malloc_hook  = mm_MallocHook;
			__realloc_hook = mm_ReallocHook;*/
			break;
		case 1:
			printf("	MemoryMarker->mm_Hooks_install() tcmalloc gFlags = %ld\n", gFlags);
			break;
		default:
			printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
		}

   } else{
	   printf("	MemoryMarker->mm_Hooks_install(): mm_Hooks failed to install. IS_RUNNING not set, gFlags = %ld\n", gFlags);
   }
   return ;
}

static void tc_Hooks_install() {
   if ( gFlags & IS_RUNNING ) {
	   printf(" MemoryMarker->tc_Hooks_install(): do nothing! gFlags = %ld\n", gFlags);

   } else{
	   printf("	MemoryMarker->tc_Hooks_install(): 'IS_RUNNING' not set, gFlags = %ld  do nothing!\n", gFlags);
   }
   return ;
}

/* _________________________________________________________________________ */
static void mm_Hooks_uninstall() {
	switch(USETCMALLOC){
	case 0:
		//printf("	MemoryMarker->mm_Hooks_uninstall(), gFlags = %u\n", gFlags);
		__free_hook    = 	m_OldFreeHook;
		/*__malloc_hook  = 	m_OldMallocHook;
		__realloc_hook = 	m_OldReallocHook;*/
		break;
	case 1:
		printf("	MemoryMarker->mm_Hooks_uninstall() tcmalloc gFlags = %ld\n", gFlags);
		break;
	default:
		printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
	}
}

/* _________________________________________________________________________ */
static void mm_Hooks_start() {
	if ( (gFlags & IS_RUNNING) ){
		switch(USETCMALLOC){
		case 0:
			printf("	MemoryMarker->mm_Hooks_start().");
			mm_Hooks_save();
			mm_Hooks_install();//__free_hook    = mm_FreeHook;
			break;
		case 1:
			if (!(gFlags & IS_MARKING)){
				printf("	MemoryMarker->tc_Hooks_start().");
				old_tcFree_hook_ = MallocHook_SetDeleteHook(tc_FreeHook);
				//old_tcMalloc_hook_ = MallocHook_SetNewHook(tc_MallocHook);
				if (old_tcFree_hook_)
					printf("   MemoryMarker-> HeapMon Delete hook set and old_tcFree_hook successfully saved");
				//if (old_tcMalloc_hook_ != NULL)
					//printf("   MemoryMarker-> HeapMon New hook set and old_tcMalloc_hook successfully saved");
			} else{
				printf("   MemoryMarker-> HeapMon hook has been already set");
			}
			break;
		default:
			printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
		}
	   gFlags |= IS_MARKING;
	   printf("	IS_MARKING flag raised, gFlags = %ld!\n", gFlags);
	} else {
	   printf("	MemoryMarker->mm_Hooks_start(): Attempt failed: IS_RUNNING flag is not set, gFlags = %ld!\n", gFlags);
	}
}

/* _________________________________________________________________________ */
static void mm_Hooks_stop() {
	if (gFlags & IS_MARKING){
		switch(USETCMALLOC){
		case 0:
			printf("	MemoryMarker->mm_Hooks_stop(): old hooks restored and IS_MARKING dropped");
			//__free_hook   = m_OldFreeHook;
			mm_Hooks_uninstall();
			break;
		case 1:
			printf("	MemoryMarker->mm_Hooks_stop(): old tc_hooks restored\n");
			if (MallocHook_SetDeleteHook(old_tcFree_hook_)!=NULL)
				printf("	old tcmalloc Delete hook restored!\n");
			//if (MallocHook_SetNewHook(old_tcMalloc_hook_)!=NULL)
				//printf("	old tcmalloc New hook restored!\n");
			break;
		default:
			printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
		}

		gFlags &= gFlags ^ IS_MARKING;
		printf(", gFlags = %ld\n", gFlags);
	} else{
		printf("	MemoryMarker->mm_Hooks_stop(): Attempt failed (IS_MARKING  not raised) gFlags = %ld. \n", gFlags);
	}
}

/* _________________________________________________________________________ */
static void mm_Hooks_atexit() {
   printf("	MemoryMarker->mm_Hooks_atexit(): mm_Hooks _uninstall(). gFlags = %ld\n", gFlags);

   switch(USETCMALLOC){
   case 0:
	   /* reset (system/NULL) hook to prevent tracking */
	   mm_Hooks_uninstall();
	   break;
   case 1:
	   printf("	MemoryMarker->mm_Hooks_atexit(): tc_Hooks _uninstall()\n");
	   if (old_tcFree_hook_!=NULL){
		   if (MallocHook_SetDeleteHook(old_tcFree_hook_)!=NULL)
			   printf("			old tcmalloc Delete hook restored successfull!\n");
		   //if (MallocHook_SetNewHook(old_tcMalloc_hook_)!=NULL)
			   //printf("			old tcmalloc New hook restored successfull!\n");
	   }
	   break;
   default:
	   printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
   }

   gFlags &= gFlags ^ IS_MARKING;
   gFlags &= gFlags ^ IS_RUNNING;
   printf("	MemoryMarker->mm_Hooks_atexit(): IS_MARKING and IS_RUNNING flags dropped  gFlags = %ld\n", gFlags);
}


/* _________________________________________________________________________ */
static void *mm_MallocHook( size_t size, const void *unused ) {
   void *result;

/* reset (system/NULL) hook to prevent recursion */
   mm_Hooks_uninstall();

/* let malloc do its thing */
   result = malloc( size );

   if (size > 8 && result!=0)
	   memset(result, NEW_MARKER, size - 8);

/* store current hooks */
   mm_Hooks_save();

/* reset tracker hook */
   mm_Hooks_install();

   return result;
}

/* _________________________________________________________________________ */
static void *mm_ReallocHook( void *ptr, size_t size, const void *unused ) {
   void *result;

/* reset (system/NULL) hook to prevent recursion */
   mm_Hooks_uninstall();

/* let realloc do its thing */
   result = realloc( ptr, size );

   //memset(result, NEW_MARKER, size);

/* store current hooks */
   mm_Hooks_save();

/* reset tracker hook */
   mm_Hooks_install();

   return result;
}

void *secureMemset(void *v, int c,size_t n) {
	volatile char *p = v;
	while (n--)
		*p++ = c;

	return v;
}

static void tc_MallocHook(const void* ptr, size_t size){
	if (size == 0){
		//printf("  alloc_size=0  ");
		number_zero_allocated+=1;
	} else {

		total_allocated +=size;
		number_allocated +=1;

		/*if (size > 1)
			memset(ptr, FREE_MARKER, object_size - 1);*/

		if (size > 50000)
			printf("malloc=%zu", size);
	}

}

static void tc_FreeHook(const void *ptrv) {
	size_t* ptr = (size_t *) ptrv;
	if( ptr!=NULL && ptr!=0){
		//printf("ptr=%x", ptr);
		size_t object_size = 0;
		object_size = MallocExtension_GetAllocatedSize(ptr);

		if (object_size < 1)
			return;
		total_freed +=object_size;
		number_freed +=1;
		if (object_size < 2){
			printf("free=1");
			memset(ptr , FREE_MARKER, 		object_size - 2 );
		} else if (object_size < 10){
			//printf("`");
			memset(ptr, FREE_MARKER, 		object_size - 2 );
		} else if (object_size < 250){ //14 size_t memory slots
			//printf("!");
			memset(ptr , FREE_MARKER, 	object_size - 8 );
		} else if (object_size < 350){
			//printf("#");
			memset(ptr + 50, FREE_MARKER, 16);
			memset(ptr + 52, SIZE_MARK, 12);

			char* cp_ptr = (char*)(ptr + 53); //pointer to the memory where object_size is recorded byte-by-byte
			char* cp_object_size = (char *)(&object_size); // get the value stored at pI
			int x = 0;
			for( x = 0; x < sizeof(object_size); x++){
				char temp = *cp_object_size; // get the value of first byte of object_size
				memset(cp_ptr + x, temp, 1);
				cp_object_size++; // Point to next byte of object_size
			}
		} else {

			//printf("$");
			//memset(ptr, FREE_MARKER, object_size - 8);
			memset(ptr + 50, FREE_MARKER, 16);
			memset(ptr + 52, SIZE_MARK, 12);

			char* cp_ptr = (char*)(ptr + 53); //pointer to the memory where object_size is recorded byte-by-byte
			char* cp_object_size = (char *)(&object_size); // get the value stored at pI
			int x = 0;
			for( x = 0; x < sizeof(object_size); x++){
				char temp = *cp_object_size; // get the value of the x-th byte of object_size
				memset(cp_ptr + x, temp, 1);
				cp_object_size++; // Point to next byte of object_size
			}
			/*if(object_size >= 350 && object_size <=450){
				int y=0;
				for(y = 0; y < 5; y++){
					//printf(" ptr=%x, value=%u(%x) ",ptr+y, 	*((long *) (ptr+y)),  *((long *) (ptr+y)));
				}
			}*/
  		}
	} else{
		//printf("ptr=NULL/0");
		number_zeroptr_freed += 1;
	}
}

/* _________________________________________________________________________ */
static void mm_FreeHook( void *ptr, const void *caller ) {
/* reset (system/NULL) hook to prevent recursion */
   mm_Hooks_uninstall();

     size_t* chunk_ptr ;
     size_t* next_chunk_ptr;
     unsigned long   object_size = 0;
     unsigned long   chunk_size = 0;

     if(ptr){
                chunk_ptr = (size_t*)((char*)ptr-1*sizeof(size_t));
		chunk_size = (*chunk_ptr) & ~LOWER_THREE_BITS;
		//object_size = ((*chunk_ptr) & ~LOWER_THREE_BITS) - 2*sizeof(size_t);
		object_size = chunk_size - 2*sizeof(size_t);
		memset(ptr, FREE_MARKER, object_size);
     }
     free( ptr );

/* store current hooks */
   mm_Hooks_save();

/* reset tracker hook */
   mm_Hooks_install();

}


/*========================================================================== */
/* startup and shutdown */
/*========================================================================== */
static void setup() __attribute__ ((constructor));
static void report() __attribute__ ((destructor));

/* _________________________________________________________________________ */
static void setup() {
   printf("MemoryMarker->setup()->mm_Hooks_save()\n");
   //find out if tcmalloc was loaded
   USETCMALLOC = strcmp(getenv("USETCMALLOC"), "true") ? 0 : 1;
   printf("  USETCMALLOC=%d", USETCMALLOC);
   if (USETCMALLOC) {
	   printf("  tcmalloc is loaded \n");
   } else {
	   printf("  glibc malloc is working \n");
	   mm_Hooks_save();
   }

}

/* _________________________________________________________________________ */
static void report() {
   static int finalized = 0;
	printf("MemoryMarker->report()->mm_Hooks_uninstall()\n");
   //unsigned int iname;
    //  finalized = 1;

   /* reset (system/NULL) as we're done now */
      mm_Hooks_uninstall();

}


/*========================================================================== */
/* python interface module */
/*========================================================================== */
#define CPPTOPYTHON( fname )                                                  \
static PyObject* hep_##fname( PyObject* unused, PyObject* args ) {            \
   if ( ! PyArg_ParseTuple( args, (char*)":"#fname ) )                        \
      return 0;                                                               \
                                                                              \
   mm_Hooks_##fname();                                                       \
                                                                              \
   Py_INCREF( Py_None );                                                      \
   return Py_None;                                                            \
}
CPPTOPYTHON( install )
CPPTOPYTHON( start )
CPPTOPYTHON( stop )
CPPTOPYTHON( uninstall )
CPPTOPYTHON( atexit )

/* _________________________________________________________________________ */
#define CPPTOPYTHONWITHSTRING( fname )                                        \
static PyObject* hep_##fname( PyObject* unused, PyObject* args ) {            \
   const char *name;                                                          \
   PyStringObject* pyname = 0;                                                \
   if ( ! PyArg_ParseTuple( args, (char*)"S:"#fname, &pyname ) )              \
      return 0;                                                               \
                                                                              \
   name = PyString_AS_STRING( pyname );                                       \
   fname( name );                                                             \
                                                                              \
   Py_INCREF( Py_None );                                                      \
   return Py_None;                                                            \
}
CPPTOPYTHONWITHSTRING( trace )
CPPTOPYTHONWITHSTRING( ignore )

/* _________________________________________________________________________ */
#define CPPTOPYTHONWITHINT( fname )                                           \
static PyObject* hep_##fname( PyObject* unused, PyObject* args ) {            \
  int i,res;                                                                  \
   if ( ! PyArg_ParseTuple( args, "i", &i ) )                                 \
      return 0;                                                               \
                                                                              \
   res = mm_##fname( i );                                                    \
   return Py_BuildValue( "i", res );                                          \
}
CPPTOPYTHONWITHINT( check )
CPPTOPYTHONWITHINT( clearCheckPoint )
CPPTOPYTHONWITHINT( depth )

static PyObject* hep_set_marker(PyObject* unused, PyObject* args ){
	char c;
	if (! PyArg_ParseTuple(args, "c", &c) )
		return 0;
	set_marker(c);

	Py_INCREF( Py_None );                                                      \
	return Py_None;
}

static PyObject* hep_get_marker(PyObject* unused, PyObject* args ){
	PyObject* py_return = 0;
	py_return = Py_BuildValue("c", FREE_MARKER);
	return py_return;
}

/* _________________________________________________________________________ */
static PyObject* hep_setCheckPoint( PyObject* unused, PyObject* args ) {
  int res = mm_setCheckPoint();
  return Py_BuildValue( "i", res );
}

/* _________________________________________________________________________ */
static PyObject* hep_configure( PyObject* unused, PyObject* args ) {
   long flags = 0;
   if ( ! PyArg_ParseTuple( args, (char*)"|l:configure", &flags ) )
      return 0;

   flags = configure( flags );

   return PyLong_FromLong( flags );
}

static PyObject* hep_get_trace( PyObject* unused, PyObject* args ) {
   long key = 0;
   if ( ! PyArg_ParseTuple( args, (char*)"|l:get_trace", &key ) )
      return 0;

   get_trace( key );

   Py_INCREF( Py_None );
   return Py_None;
}

static PyObject* hep_get_test( PyObject* unused, PyObject* args ) {
   long key = 0;
   if ( ! PyArg_ParseTuple( args, (char*)"|l:get_test", &key ) )
      return 0;

   get_test( key );

   Py_INCREF( Py_None );
   return Py_None;
}

static PyObject* hep_store_malloc_info( PyObject* unused, PyObject* args ) {
   unsigned int number = 0;
   const char * name;
   PyStringObject* pyname = 0;

   if ( ! PyArg_ParseTuple( args, (char*)"is:store_malloc_info", &number, &pyname) )
      return 0;

   name = PyString_AS_STRING( pyname );
   //store_malloc_info(number, name);

   Py_INCREF( Py_None );
   return Py_None;
}

static PyObject* hep_get_malloc_info( PyObject* unused, PyObject* args ) {
   PyStringObject* pyname = 0;
   PyObject* py_return = 0;
   struct mallinfo mem_struct;
   mem_struct = mallinfo();

   switch(USETCMALLOC){
   case 0: //ptmalloc(glibc)
	   /* reset (system/NULL) hook to prevent tracking */
		/*printf("	sbrk-Allocated Memory( ARENA) size: %lu\n", mem_struct.arena);
		printf(" 	MMAP-Allocated Memory: %lu\n", mem_struct.hblkhd);
		printf(" 	Number of chunks allocated by MMAP: %lu\n", mem_struct.hblks);
		printf(" 	MALLOC-In Use: %lu\n", mem_struct.uordblks );
		printf(" 	MALLOC-Free: %lu\n", mem_struct.fordblks);
		printf(" 	Number of chunks from OS not in use by MALLOC: %lu\n", mem_struct.ordblks);
		printf("    Top-releasable chunk keepcost: %lu\n", mem_struct.keepcost);*/

	   py_return = Py_BuildValue("llll",
			   mem_struct.arena/1024, 	//sbrk
			   mem_struct.hblkhd/1024, 	//mmap
			   mem_struct.uordblks/1024, //inuse
			   mem_struct.fordblks/1024);//free
	   break;
   case 1: //tcmalloc
	   py_return = Py_BuildValue("llll",
			   mem_struct.arena/1024, 	//sbrk
			   1, 	//for tcmalloc not defined
			   mem_struct.uordblks/1024, //inuse
			   mem_struct.fordblks/1024);//free
	   //get_malloc_info();
	   break;
   default:
	   printf(" Invalid USETCMALLOC key: %d", USETCMALLOC);
	   show_malloc_info();
   }

   return py_return;
}

static PyObject* hep_show_info( PyObject* unused, PyObject* args ) {
	   show_malloc_info( );
	   Py_INCREF( Py_None );
	   return Py_None;
}

static PyObject* hep_get_key_list( PyObject* unused, PyObject* args ) {
   long key = 0;
   if ( ! PyArg_ParseTuple( args, (char*)"|l:get_key_list", &key ) )
      return 0;

   get_key_list( );

   Py_INCREF( Py_None );
   return Py_None;
}

/* _________________________________________________________________________ */
static PyObject* hep_outstream( PyObject* unused, PyObject* args ) {
   PyObject* pyfile;
   if ( ! PyArg_ParseTuple( args, "O!", &PyFile_Type, &pyfile ) )
      return 0;

   FILE* fp = PyFile_AsFile( pyfile );
   if ( ! fp )
      return 0;

   int fd = dup( fileno( fp ) );

   file_output = fdopen( fd, "w" );

   Py_INCREF( Py_None );
   return Py_None;
}

/* _________________________________________________________________________ */
static PyMethodDef gMemoryMarkerMethods[] = {
   { (char*)"install",   (PyCFunction)hep_install,   METH_VARARGS, (char*)"install/start marking" },
   { (char*)"start",     (PyCFunction)hep_start,     METH_VARARGS, (char*)"start marking" },
   { (char*)"stop",      (PyCFunction)hep_stop,      METH_VARARGS, (char*)"stop marking" },
   { (char*)"uninstall", (PyCFunction)hep_uninstall, METH_VARARGS, (char*)"uninstall marking" },
   { (char*)"atexit",    (PyCFunction)hep_atexit,    METH_VARARGS, (char*)"atexit handler" },
   { (char*)"ignore",    (PyCFunction)hep_ignore,    METH_VARARGS, (char*)"ignore memory marking by dropping IS_MARKING flag" },
   { (char*)"configure", (PyCFunction)hep_configure, METH_VARARGS, (char*)"prepare for memory marking by raising IS_MARKING flag" },
   { (char*)"outstream", (PyCFunction)hep_outstream, METH_VARARGS, (char*)"set new outstream" },
   { (char*)"get_test",  (PyCFunction)hep_get_test,  METH_VARARGS, (char*)"get test information" },
   { (char*)"store_malloc_info", (PyCFunction)hep_store_malloc_info,  	METH_VARARGS, (char*)"store Malloc-info into ROOT file using C++ extern function - doesn't work for now" },
   { (char*)"get_malloc_info", (PyCFunction)hep_get_malloc_info,  		METH_VARARGS, (char*)"get Malloc-info in the tuple" },
   { (char*)"show_info", (PyCFunction)hep_show_info,  					METH_VARARGS, (char*)"show mempory usage info" },
   { (char*)"set_marker",  (PyCFunction)hep_set_marker,  METH_VARARGS, (char*)"set marker_char to build marker" },
   { (char*)"get_marker",  (PyCFunction)hep_get_marker,  METH_VARARGS, (char*)"get current marker_char" },
   { NULL, NULL, 0, NULL }
};


/* _________________________________________________________________________ */
void initMemoryMarker() {
   PyObject *memmark;

   memmark = Py_InitModule( (char*)"MemoryMarker", gMemoryMarkerMethods );

/* configuration flags */
   PyModule_AddObject( memmark, (char*)"LEAK_CHECK", PyLong_FromLong( LEAK_CHECK ) );
   PyModule_AddObject( memmark, (char*)"PROFILE",    PyLong_FromLong( PROFILE ) );
   PyModule_AddObject( memmark, (char*)"HIDEMEMADDR",PyLong_FromLong( HIDEMEMADDR ) );
   PyModule_AddObject( memmark, (char*)"QUICK",      PyLong_FromLong( QUICK ) );
   PyModule_AddObject( memmark, (char*)"STL_CHECKS", PyLong_FromLong( STL_CHECKS ) );
   PyModule_AddObject( memmark, (char*)"FILTER_STL", PyLong_FromLong( FILTER_STL ) );
   PyModule_AddObject( memmark, (char*)"IS_MARKING", PyLong_FromLong( IS_MARKING ) );
   PyModule_AddObject( memmark, (char*)"IS_RUNNING", PyLong_FromLong( IS_RUNNING ) );
}

#endif

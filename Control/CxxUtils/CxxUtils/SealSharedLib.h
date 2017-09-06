/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/SealSharedLib.h
 * @author Lassi Tuura (original author)
 * @author Wim Lavrijsen <WLavrijsen@lbl.gov> (responsible in ATLAS)
 * @date Oct, 2008
 *
 *        Search for `wlav' to find changes from the SEAL version. I
 *        also dropped all ASSERT macro's in favor of assert.
 */

#ifndef CXXUTILS_SEAL_SHAREDLIB_H // wlav SEAL_BASE_SHARED_LIBRARY_H
#define CXXUTILS_SEAL_SHAREDLIB_H // wlav SEAL_BASE_SHARED_LIBRARY_H


#include "CxxUtils/checker_macros.h"
//# include "SealBase/SharedLibraryError.h"       wlav
//# include "SealBase/Callback.h"                 wlav
# include <string>
# include <list>
# include <exception>                          // wlav

//namespace seal {                                wlav
namespace Athena {                             // wlav



// wlav from SealBase/Callback.h
template <class T1>
class Callback1Rep
{
public:
    Callback1Rep (void) : m_refs (0) { }
    virtual ~Callback1Rep (void) { }

    virtual void	call (T1) = 0;
    virtual bool	equal (const Callback1Rep *x) const = 0;

    void		ref (void)	{ ++m_refs; }
    void		unref (void)	{ if (--m_refs == 0) delete this; }

private:
    int			m_refs;
};

template <class T1>
class Callback1
{
public:
    Callback1 (Callback1Rep<T1> *implementation = 0);
    Callback1 (const Callback1 &x);
    ~Callback1 (void);
    Callback1 &	operator= (const Callback1 &x);

    bool		operator== (const Callback1 &x) const;
    /**/		operator bool (void) const;
    void		operator() (T1) const;

private:
    Callback1Rep<T1>	*m_rep;
};

template <class T1, class T2>
class CallbackImpF11 : public Callback1Rep<T1>
{
    typedef CallbackImpF11 self;
public:
    CallbackImpF11 (void (*function) (T1, T2),
		    const T2 &fill_2)
	: m_function (function),
	  m_fill_2 (fill_2)
    { }

    virtual void call (T1 a)
    { (*m_function) (a, m_fill_2); }

    virtual bool equal (const Callback1Rep<T1> *other) const
	{ const self *x = dynamic_cast<const self *> (other);
	  return x && x->m_function == m_function && x->m_fill_2 == m_fill_2; }

private:
    void	(*m_function) (T1, T2);
    T2		m_fill_2;
};

template <class T1>
inline
Callback1<T1>::Callback1 (Callback1Rep<T1> *implementation /* = 0 */)
    : m_rep (implementation)
{ if (m_rep) m_rep->ref (); }

template <class T1>
inline
Callback1<T1>::Callback1 (const Callback1<T1> &x)
    : m_rep (x.m_rep)
{ if (m_rep) m_rep->ref (); }

template <class T1>
inline
Callback1<T1>::~Callback1 (void)
{ if (m_rep) m_rep->unref (); }

template <class T1>
inline Callback1<T1> &
Callback1<T1>::operator= (const Callback1<T1> &x)
{
    if (m_rep != x.m_rep)
    {
	if (m_rep) m_rep->unref ();
	m_rep = x.m_rep;
	if (m_rep) m_rep->ref ();
    }
    return *this;
}

template <class T1>
inline bool
Callback1<T1>::operator== (const Callback1<T1> &x) const
{ return m_rep == x.m_rep || (m_rep && x.m_rep && m_rep->equal (x.m_rep)); }

template <class T1>
inline
Callback1<T1>::operator bool (void) const
{ return m_rep ? true : false; } // FIXME: for Sun CC 4.2 (no bool)

template <class T1>
inline void
Callback1<T1>::operator() (T1 a) const
{ m_rep->call (a); }

template <class T1, class T2>
inline Callback1Rep<T1> *
CreateCallback (void (*function) (T1, T2),
		const T2 &fill_2)
{ return new CallbackImpF11<T1,T2> (function, fill_2); }


// wlav modified from SealBase/SharedLibraryError.h
/** Error in a shared library operation. */
class SharedLibraryError : public std::exception
{
public:
    SharedLibraryError( const std::string& context, const std::string& cause );
    virtual ~SharedLibraryError() throw() {}

    virtual const char* what() const throw();

private:
    std::string		m_message;
};


/** Shared library services.  */
class SharedLibrary
{
public:
    typedef void *		Data;
    typedef void		(*Function) (void);

    /** Information about a currently loaded shared library.  */
    struct LibraryInfo
    {
	unsigned long		m_text_start;	//< Start of text segment
	unsigned long		m_text_end;	//< End of text segment
	unsigned long		m_data_start;	//< Start of data segment
	unsigned long		m_data_end;	//< End of data segment
	unsigned long		m_bss_start;	//< Start of common
	unsigned long		m_bss_end;	//< End of common
	const char		*m_filename;	//< Filename
    };

    typedef Callback1<const LibraryInfo &> InfoHandler;

    static std::string		path (void);
    static void			path ATLAS_NOT_THREAD_SAFE (const std::string &path);
    static std::string		libname (const std::string &name);
    static std::string		symname (const std::string &name);

    static SharedLibrary *	self (void);
    static SharedLibrary *	load (const std::string &name);
    static void			loaded (const InfoHandler &handler);

    void			release (void);
    void			abandon (void);

    Data			data (const std::string &name, bool mangle = true) const;
    Function			function (const std::string &name, bool mangle = true) const;

protected:
    SharedLibrary (void *handle);
    ~SharedLibrary (void);

private:
    void			*m_handle;

    // undefined semantics
    SharedLibrary (const SharedLibrary &);
    SharedLibrary &operator= (const SharedLibrary &);
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

//} // namespace seal                             wlav
} // namespace Athena                             wlav
#endif // CXXUTILS_SEAL_SHAREDLIB_H wlav SEAL_BASE_SHARED_LIBRARY_H

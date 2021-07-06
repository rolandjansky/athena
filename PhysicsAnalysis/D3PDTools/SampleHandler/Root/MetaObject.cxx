/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//          - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

//protect
#include <SampleHandler/MetaObject.h>

#include <cstdlib>
#include <memory>
#include <sstream>
#include <TList.h>
#include <TNamed.h>
#include <TBuffer.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaData.h>

//
// method implementations
//

ClassImp (SH::MetaObject)

namespace SH
{
  namespace
  {
    template<class T> struct TypeName
    {
      static std::string name () {return typeid (T).name();}
    };
#define TYPENAME(X)					\
    template<> struct TypeName<X> {			\
      static std::string name () {return #X;}; };
    TYPENAME (std::string)
    TYPENAME (double)
    TYPENAME (int)
    TYPENAME (bool)
#undef TYPENAME


    template<class From,class To> struct Convert
    {
      static void convert (const std::string& /*from*/, To& /*to*/, const std::string& field)
      {
	if (field.empty())
	  return;
	RCU_THROW_MSG ("no conversion defined from type " + TypeName<From>::name + " to type " + TypeName<To>::name + " for field " + field);
      }
    };

    // the conversion string => type is a bit problematic, as it is
    // expected to produce an exception if the conversion cannot be
    // performed. this requires several lines of code. in order to avoid
    // duplication, this has been made into an additional preprocessor
    // macro. while a template function would have been nicer, this
    // would not allow to add the target typename to the error message,
    // which is why this approach has been chosen.
    template<class To> struct Convert<std::string,To>
    {
      static void convert (const std::string& from, To& to, const std::string& field)
      {
	std::stringstream ss (from);
	To retval;
	ss >> retval;
	if (!(ss.fail() || ss.rdbuf()->in_avail() > 0))
	{
	  std::swap (retval, to);
	  return;
	}
	if (field.empty())
	  return;
	RCU_THROW_MSG ("unable to convert string '" + from + "' to type " + TypeName<To>::name() + " for field " + field);
      }
    };

    template<class Type> struct Convert<Type,Type>
    {
      static void convert (const Type& from, Type& to, const std::string& /*field*/)
      {
	to = from;
      }
    };

    template<class From> struct Convert<From,std::string>
    {
      static void convert (const From& from, std::string& to, const std::string& /*field*/)
      {
	std::ostringstream ss;
	ss << from;
	to = ss.str();
      }
    };

    template<> struct Convert<bool,std::string>
    {
      static void convert (const bool& from, std::string& to, const std::string& /*field*/)
      {
	if (from)
	  to = "true";
	else
	  to = "false";
      }
    };

    template<> struct Convert<std::string,bool>
    {
      static void convert (const std::string& from, bool& to, const std::string& field)
      {
	if (from == "true" || from == "True" || from == "TRUE" || from == "1")
	{
	  to = true;
	  return;
	}
	if (from == "false" || from == "False" || from  == "FALSE" || from == "0")
	{
	  to = false;
	  return;
	}
	if (field.empty())
	  return;
	RCU_THROW_MSG ("unable to convert string '" + from + "' to type bool for field " + field);
      }
    };

#define DEFAULT_CONVERT(FROM,TO)					\
    template<> struct Convert<FROM,TO> {				\
      static void convert (const FROM& from, TO& to, const std::string& /*field*/) { \
	to = from; } };
    DEFAULT_CONVERT (std::string, std::string)
    DEFAULT_CONVERT (bool, double)
    DEFAULT_CONVERT (int, double)
    DEFAULT_CONVERT (bool, int)
    DEFAULT_CONVERT (double, int)
    DEFAULT_CONVERT (int, bool)
    DEFAULT_CONVERT (double, bool)
#undef DEFAULT_CONVERT
    

    template<class From,class To> bool
    convertSingle (const TObject *from, To& to, const std::string& field)
    {
      const MetaData<From> *myfrom =
	dynamic_cast<const MetaData<From>* >(from);
      if (myfrom == 0)
	return false;
      Convert<From,To>::convert (myfrom->value, to, field);
      return true;
    }

    template<class To> void
    convert (const TObject *from, To& to, const std::string& field)
    {
      if (convertSingle<std::string> (from, to, field)) return;
      if (convertSingle<double> (from, to, field)) return;
      if (convertSingle<int> (from, to, field)) return;
      if (convertSingle<bool> (from, to, field)) return;
      if (!field.empty())
	RCU_THROW_MSG ("unkown input type " + std::string (typeid(*from).name()) + " for field " + field);
    }

    /// \brief trim leading/trailing spaces from the given string
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    std::string trim (const std::string& str)
    {
      size_t endpos = str.find_last_not_of(" \t");
      size_t startpos = str.find_first_not_of(" \t");
      if( std::string::npos == endpos ) return "";
      if(str[startpos] == '"' || str[startpos] == '\'') startpos++;
      if(str[endpos]   == '"' || str[endpos]   == '\'') endpos--;
      return str.substr( startpos, endpos+1-startpos );
    }


    /*
    /// \brief the meta-data element with the given name
    ///
    /// \return the meta-data with the given name, or def_value if
    ///   that meta-data is not available or does not have the right
    ///   type
    /// \param name the name of the meta-data object
    /// \param dev_val the value to return if no meta-data object of
    ///   the given name exists, or if it does not have the right type
    /// \par Guarantee
    ///   no-fail
  public:
    template<class T> T getT(const std::string& name, T def_val) const;

    /// \brief convert a meta-data element into another data type
    ///
    /// \return content of the given meta data element in the desired
    ///   data type
    /// \param meta input meta data element
    /// \param dev_val the value to return if there is no known
    ///   conversion for this type
    /// \par Guarantee
    ///   no-fail
  public:
    template<class T> static T convertT(const TObject* meta, T defval);
    */
  }



  std::string dbg (const MetaObject& /*obj*/, unsigned /*verbosity*/)
  {
    return "MetaObject";
  }



  void swap (MetaObject& a, MetaObject& b)
  {
    a.swap (b);
  }



  void MetaObject ::
  testInvariant () const
  {
    RCU_INVARIANT (m_dataList != 0);
 }



  MetaObject ::
  MetaObject ()
    : m_dataList (new TList)
  {
    m_dataList->SetOwner(true);
    RCU_NEW_INVARIANT (this);
  }



  MetaObject ::
  MetaObject (const MetaObject& that)
    : TCollection (), m_dataList (new TList)
  {
    m_dataList->SetOwner(true);
    TIter iter (&that);
    TObject *object = 0;
    while ((object = iter.Next()))
    {
      m_dataList->Add (object->Clone());
    };

    RCU_NEW_INVARIANT (this);
  }



  MetaObject ::
  ~MetaObject ()
  {
    RCU_DESTROY_INVARIANT (this);

    delete m_dataList;
  }



  MetaObject& MetaObject ::
  operator = (const MetaObject& that)
  {
    // no invariant used
    MetaObject tmp (that);
    tmp.swap (*this);
    return *this;
  }



  void MetaObject ::
  swap (MetaObject& that)
  {
    std::swap (m_dataList, that.m_dataList);
  }



  void MetaObject ::
  remove (const std::string& name)
  {
    RCU_CHANGE_INVARIANT (this);

    TObject *object = 0;
    while ((object = m_dataList->FindObject (name.c_str())))
      delete m_dataList->Remove (object);
  }



  void MetaObject ::
  addReplace (TNamed *meta_swallow)
  {
    std::unique_ptr<TNamed> meta (meta_swallow);
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (meta_swallow != 0);

    remove (meta_swallow->GetName());
    m_dataList->Add (meta.release());
  }



  TObject *MetaObject ::
  get (const std::string& name)
  {
    RCU_READ_INVARIANT (this);
    return m_dataList->FindObject (name.c_str());
  }



  const TObject *MetaObject ::
  get (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return m_dataList->FindObject (name.c_str());
  }



  double MetaObject ::
  getDouble (const std::string& name, double def_val) const
  {
    // no invariant used
    return castDouble (name, def_val, CAST_NOCAST_DEFAULT);
  }



  std::string MetaObject ::
  getString (const std::string& name, const std::string& def_val) const
  {
    // no invariant used
    return castString (name, def_val, CAST_NOCAST_DEFAULT);
  }



  double MetaObject ::
  castDouble (const std::string& name, double def_val,
	      CastMode mode) const
  {
    return castT (name, def_val, mode);
  }



  std::string MetaObject ::
  castString (const std::string& name, const std::string& def_val,
	      CastMode mode) const
  {
    return castT(name,def_val, mode);
  }


  int MetaObject ::
  castInteger (const std::string& name, int def_val,
	       CastMode mode) const
  {
    return castT(name,def_val, mode);
  }


  bool MetaObject ::
  castBool (const std::string& name, bool def_val,
	    CastMode mode) const
  {
    return castT(name,def_val, mode);
  }


  void MetaObject ::
  setDouble (const std::string& name, double value)
  {
    setT (name, value);
  }



  void MetaObject ::
  setString (const std::string& name, const std::string& value)
  {
    setT(name,value);
  }


  void MetaObject ::
  setInteger (const std::string& name, int value)
  {
    setT(name,value);
  }


  void MetaObject ::
  setBool (const std::string& name, bool value)
  {
    setT(name,value);
  }


  void MetaObject ::
  fetch (const MetaObject& source)
  {
    RCU_CHANGE_INVARIANT (this);

    TIter iter (&source);
    TObject *object = 0;
    while ((object = iter.Next()))
    {
      TNamed *const named = dynamic_cast<TNamed*>(object);
      if (!named)
      {
	m_dataList->Add (object->Clone());
      } else if (strncmp (named->GetName(), "nc_", 3) != 0)
      {
	addReplace (dynamic_cast<TNamed*>(named->Clone ()));
      };
    };
  }



  void MetaObject ::
  fetchDefaults (const MetaObject& source)
  {
    RCU_CHANGE_INVARIANT (this);

    TIter iter (&source);
    TObject *object = 0;
    while ((object = iter.Next()))
    {
      if (get (object->GetName ()) == 0)
	m_dataList->Add (object->Clone());
    };
  }



  void MetaObject ::
  Add (TObject *meta_swallow)
  {
    std::unique_ptr<TObject> meta (meta_swallow);
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (meta_swallow != 0);
    m_dataList->Add (meta.release());
  }



  void MetaObject ::
  Clear (Option_t *option)
  {
    RCU_CHANGE_INVARIANT (this);
    m_dataList->Clear (option);
  }



  void MetaObject ::
  Delete (Option_t *option)
  {
    RCU_CHANGE_INVARIANT (this);
    m_dataList->Delete (option);
  }



  TObject **MetaObject ::
  GetObjectRef (const TObject *const meta) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (meta != 0);

    TObject **const result = m_dataList->GetObjectRef (meta);

    RCU_PROVIDE (result != 0);
    RCU_PROVIDE (*result == meta);
    return result;
  }



  TIterator *MetaObject ::
  MakeIterator (Bool_t dir) const
  {
    RCU_READ_INVARIANT (this);

    std::unique_ptr<TIterator> result (m_dataList->MakeIterator (dir));

    RCU_PROVIDE2 (result.get() != 0, "result != 0");
    return result.release();
  }



  TObject *MetaObject ::
  Remove (TObject *meta)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (meta != 0);
    return m_dataList->Remove (meta);
  }



  void MetaObject ::
  Streamer (TBuffer& b)
  {
    if (b.IsReading())
    {
      RCU_CHANGE_INVARIANT (this);
      TObject::Streamer (b);
      m_dataList->Clear ();
      m_dataList->Streamer (b);
    } else
    {
      RCU_READ_INVARIANT (this);
      TObject::Streamer (b);
      m_dataList->Streamer (b);
    };
  }

  
  template<class T> T MetaObject ::
  castT (const std::string& name, T def_val, CastMode mode) const 
  {
    RCU_READ_INVARIANT (this);
    const TObject* meta = get (name);
    if (meta == 0)
      return def_val;
    const MetaData<T> *const retval = dynamic_cast<const MetaData<T>*>(meta);
    if (retval) 
      return retval->value;
    switch (mode)
    {
    case CAST_ERROR_THROW:
      {
	T result = def_val;
	convert (meta, result, name);
	return result;
      }
    case CAST_ERROR_DEFAULT:
      {
	T result = def_val;
	convert (meta, result, "");
	return result;
      }
    case CAST_NOCAST_THROW:
      RCU_THROW_MSG ("invalid input value for " + name);
      return def_val;
    case CAST_NOCAST_DEFAULT:
      return def_val;
    }
    RCU_ASSERT0 ("invalid cast value");
    return def_val; //compiler dummy
  }
  

  void MetaObject ::
  fetchFromString(const std::string& source){
    size_t pos=1;
    size_t oldpos = 0;
    while(pos<source.size() && pos > 0){
      size_t split = source.find("=",oldpos);
      pos = source.find(",",split);
      if(split < pos){
	std::string key = trim(source.substr(oldpos,split-oldpos));
	std::string value = trim(source.substr(split+1,pos-split-1));
	this->setString(key,value);
      } else {
	RCU_THROW_MSG ("unable to parse string '"+source+"'");
      }
      oldpos = pos+1;
    }
  }
  
  std::string MetaObject :: 
  dumpToString(){
    std::stringstream ss;
    TIterator* itr = this->MakeIterator();
    TObject* obj = NULL;
    while( (obj = itr->Next()) ){
      Meta* m = dynamic_cast<Meta*>(obj);
      if(!m) continue;
      if(!ss.str().empty()) ss << ", ";
      ss << m->GetName() << "=";
      MetaData<std::string>* s = dynamic_cast<MetaData<std::string>* >(m);
      if(s){
	ss << "\"" << s->value << "\"";
      } else {
	std::string mystring;
	convert<std::string> (m, mystring, m->GetName());
	ss << mystring;
      }
    }
    return ss.str();
  }
  
  Int_t MetaObject ::
  GetEntries() const {
    return this->m_dataList->GetEntries();
  }
  
}

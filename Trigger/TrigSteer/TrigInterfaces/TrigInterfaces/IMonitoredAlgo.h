/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_IMONITOREDALGO_H
#define TRIGINTERFACES_IMONITOREDALGO_H
#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <string>
#include <map>
#include <limits>

#include <stdlib.h>
#include <cxxabi.h>
#include <string>
#include <iostream>


#include <DataModel/DataVector.h>
/**
 * @brief Interface for HLT algorithms monitoring.
 *
 * @author Tomasz Bold      <Tomasz.Bold@cern.ch>     - U. of California, Irvine & UST AGH Krakow
 *
 * This is the interface class used to implement monitoring inside HLT algorithms; it allows:
 * - to declare a new variable to be monitored; the possible types of variables to be monitored is defined
 *   in the MonitoredVariableType enumeration; methods for declaring monitored variables of the different
 *   types are provided;
 * - to list the current set of monitored variables.
 */

//#define HERE "\n" << __FILE__<< ":" <<__LINE__<< ": "


namespace {

  
  // template working for containers containing PODs, the "retrieve operation is then trival pass over)
  // thanks to it we can reuse code from the SmartContainerGetter
  template<typename T> 
  T noop(T arg) {
    return arg;
  }



  template<class CONTAINER>
  struct contained_type {
    typedef typename boost::remove_reference<CONTAINER>::type UNREF_CONTAINER;
    typedef typename boost::remove_pointer<UNREF_CONTAINER>::type CONTAINER_TYPE;    
    typedef typename CONTAINER_TYPE::value_type type; // this works because std::vector and DataVector both decided to public value_type

  };


  
  template<class CONTAINED> 
  struct getter_arg_type {
    typedef typename boost::add_const<typename boost::remove_pointer<CONTAINED>::type>::type BASE;
    typedef typename boost::mpl::if_c<boost::is_pointer<CONTAINED>::value, 
				      typename boost::add_pointer<BASE>::type, 
				      typename boost::add_reference<BASE>::type >::type type;
  };

  template<class CONTAINER>
  struct nonconst_type {
    typedef typename boost::remove_const<typename boost::remove_pointer<CONTAINER>::type>::type BASE;
    typedef typename boost::mpl::if_c<boost::is_pointer<CONTAINER>::value, 
				      typename boost::add_pointer<BASE>::type, 
				      BASE >::type type;
    
  };

  template<class T>
  std::string rtti_real_name() {
    std::string name(__PRETTY_FUNCTION__);   
    size_t op = name.find('=');
    size_t cl = name.find(']');
    return name.substr(op+1, cl-op-1);
  }


  template<class T, bool is_pointer> struct dereference;

  template<class T> 
  struct dereference<T, true> {
    typedef typename boost::remove_pointer<T>::type type;
    static type& go(T& c) {
      return *c;
    }
  };

  template<class T> 
  struct dereference<T, false> {
    typedef typename boost::remove_pointer<T>::type type;
    static type& go(T& c) {
      return c;
    }
  };

  template<class T, bool is_const> struct container_reset {
    static void go(T&,  bool) {
    }

  };

  template<class T>
  struct container_reset<T, false> { // the object is const, resetting is not allowed
    static void go(T& container, bool do_reset) {
      //typedef typename dereference<T, boost::is_pointer<T>::value>::type deref;

      if (do_reset){        	
	dereference<T, boost::is_pointer<T>::value>::go(container).clear();
      }
    }
  };



  // In principle this can be done with succint specialization for T and T*.
  // But in practice boost::is_pointer used as a second arg is better,
  // as it handles nicelly all the const's afound the object.
  template<class T, bool is_pointer> struct is_null_ptr;

  template<class T>
  struct is_null_ptr<T, false> {
    static bool check(T&) {
      return false;
    }
  };

  template<class T>  
  struct is_null_ptr<T, true> {
    static bool check(T& obj) {
      return obj == 0;
    }
  };

  // adapter for the external monitoring function arguments
  template<class FROM, class TO, bool is_from_a_pointer, bool is_to_a_pointer > 
  struct adapt {
    static TO  go(FROM& a) {
      return a;
    }
  };


  template<class FROM, class TO> 
  struct adapt<FROM, TO, true, false >  {
    static TO go(FROM a) {
      return *a;
    }
  };


  template<class FROM, class TO> 
  struct adapt<FROM, TO, false, true >  {
    static TO go(const FROM& a) {
      return &a;
    }
  };


  


}


class IMonitoredAlgo {
 public:

  /**
   * @brief defines different policies for the reset of monitored containers:
   * - NoClear: do not clear the container
   * - AutoClear: clear the container before each algorithm execution
   */
  enum ContainerReset { NoClear, AutoClear };
  
  /**
   * @brief Base class for monitored variable getter classes.
   *
   *
   * This class provides a basic interface for:
   * - obtaining the size of a variable container;
   * - retrieving the elements of the variable container by indexed access.
   * - resetting the variable
   */
  class IGetter {
  public:
    /** @brief Constructor. */
    IGetter (const std::string& name) : m_name(name) {}                            
    IGetter () {}                            
    /** @brief Destructor. */
    virtual ~IGetter () {}
    /** @brief Accessor method for the size of variables list.
     *  @return Size of the variables list.
     */
    virtual unsigned int size() const { return 0;}
    /** @brief Variable getter method from the variables list; performs indexed access.
     *  @param i index of the variable to get inside the available list.
     *  @return Variable content after casting to double.
     */
    virtual double get(unsigned) const { return 0;}
    /** @brief Reset all monitored variables to default values
     *
     *  This methods is called by the HLT::Algo base class before
     *  each execution of the algorithms.
     */
    virtual void reset() { }

    inline void name(const std::string& name) { m_name = name; }
    inline const std::string& name() { return m_name; }
  private:
    std::string m_name;

  };
  

  /**
   * @brief Getter class for simple monitored variables (by reference).
   *
   */
  template<class T, class U>
  class SimpleGetterByRef : public IGetter {
  public:
    /** @brief Constructor taking variable reference
     *  @param variable Reference to monitored variable
     */
    SimpleGetterByRef(T& variable) : m_variable(variable), m_hasDefault(false) {}
    /** @brief Constructor taking variable reference and default value
     *  @param variable Reference to monitored variable
     *  @param defVal   Default value to be set in reset()
     */    
    SimpleGetterByRef(T& variable, const U& defVal) :
      m_variable(variable), m_defVal(defVal), m_hasDefault(true) {}
    /** @brief Destructor. */
    virtual ~SimpleGetterByRef () {}
    /** @brief Accessor method for the size of variables list.
     *  @return Size of the variables list, always 1 in this case.
     */
    virtual unsigned size() const { return 1; }
    /** @brief Variable getter method.
     *  @param i variable index; discarded in this case.
     *  @return Variable content after casting to double.
     */
    virtual double get(unsigned) const { return double(m_variable); }  
    /** @brief Reset the variable to its default value */
    virtual void reset() { if ( m_hasDefault) m_variable = m_defVal; }

  private:
    /** @brief Reference to the monitored variable. */
    T& m_variable;
    /** @brief Copy of the default value used for reset*/
    T m_defVal;
    /** @brief True if default value was set */
    bool m_hasDefault;
  };

  
  template<class T, class U>
  class StdContainerSmartGetter : public IGetter {
  public:
    
    typedef typename contained_type<T>::type CONTAINED;
    typedef typename getter_arg_type<CONTAINED>::type GETTER_ARG_TYPE;
    typedef boost::function1<U, GETTER_ARG_TYPE> MethodType;
    typedef T & REF;

    StdContainerSmartGetter(REF container, MethodType method, ContainerReset rpolicy=NoClear) 
      :m_container(container), 
       m_method(method), 
       m_resetPolicy(rpolicy) {      
      /*
      std::cout << "StdContainerSmartGetter types " << rtti_real_name<T>()
		<< " __Contained__  " << rtti_real_name<CONTAINED>() 
		<< " __REF__  " << rtti_real_name<REF>() 
	
		<< " __GART__  " << rtti_real_name<GETTER_ARG_TYPE>() 
		<< " __Removed pointer__ " << rtti_real_name< typename boost::remove_pointer<T>::type >() << std::endl;
      */
    }

    virtual double get(unsigned i = 0) const {       
      return m_method(dereference<T, boost::is_pointer<T>::value>::go(m_container)[i]);      
    }    

    virtual unsigned int size() const { 
      return is_null_ptr<T, boost::is_pointer<T>::value>::check(m_container) ? 0 	
	: dereference<T, boost::is_pointer<T>::value>::go(m_container).size();

    }

    virtual void reset() {             
      container_reset<T, boost::is_const<T>::value>::go(m_container, m_resetPolicy==AutoClear);      
    }
  private:
    REF m_container;
    MethodType  m_method;
    ContainerReset m_resetPolicy;

    StdContainerSmartGetter(const StdContainerSmartGetter<T, U>&) { std::cout  << "StdContainerSmartGetter invoking copy???" << std::endl; }
  };
  

  /**
   * getter for objects, null pointer served
   */
  template<class T, class U, class METHOD_ARG>
  class ObjectSmartGetter : public IGetter {
    typedef boost::function1<U, METHOD_ARG> MethodType;
  public:
    ObjectSmartGetter(T& obj, MethodType method) 
      : m_object(obj),
	m_method(method)
    {
    }

    virtual unsigned int size() const { 
      return  is_null_ptr<T, boost::is_pointer<T>::value>::check(m_object) ? 0 : 1;

    }    

    virtual double get(unsigned) const {
      return m_method(  adapt<T, METHOD_ARG, 
      			boost::is_pointer<T>::value, boost::is_pointer<METHOD_ARG>::value>::go( m_object ) );

    }    

  private:
    T& m_object;
    MethodType m_method;
  };
  

  /**
   * @brief Method to add a simple variable to the monitored list.
   * @param name name of the variable to add.
   * @param var reference to the variable to add.
   * @return handle to the newly added MonitoredVariable.
   *
   * Before each execution of the algorithm the variable referenced by var
   * will be reset to "nan".
   */
  template <class U>
  IGetter* declareMonitoredVariable(const std::string& name, U& var)
    { return declareMonitoredVariable(name, var, std::numeric_limits<U>::quiet_NaN()); }
  
  /**
   * @brief Method to add a simple variable with default value to the monitored list.
   * @param name name of the variable to add.
   * @param var reference to the variable to add.
   * @param defaultValue value to set before each execution of the algorithm.
   * @return handle to the newly added MonitoredVariable.
   *
   * The variable and the default value can have different types but they need to be
   * assignment-compatible, i.e. there must exist an implicit type conversion from the
   * type of defaultValue to the type of var (e.g. from int/double to float).
   */
  template <class U, class V> 
  IGetter* declareMonitoredVariable(const std::string& name, U& var, const V& defaultValue) {
    
    return declareMonitoredCustomVariable(name,  new SimpleGetterByRef<U,V>(var, defaultValue)); 
  }


  /**
   * @brief Method to add a standard container of variables to the monitored list.
   * @param name name of the standard container of variables to add.
   * @param var reference to the standard container of variables to add.
   * @param resetPolicy if container should be cleared before each algorithm execution.
   * @return handle to the newly added IGeter
   */
  template <class T> 
  IGetter*
  declareMonitoredStdContainer(const std::string& name, 
			       T& container, 
			       ContainerReset resetPolicy = NoClear) {

    typedef typename contained_type<T>::type CONTAINED;


    return declareMonitoredCustomVariable(name, 
					  new StdContainerSmartGetter<T, CONTAINED >(container,
										     noop<CONTAINED>,
										     resetPolicy));    
  }


  /**
   * @brief Method to add a collection (DataVector) of variables to the monitored list.
   * @param name name of the collection of variables to add.
   * @param var reference to the pointer to the collection of variables to add.
   * @param method a member method of the objects in the collection, used to access their value (must be a void-argument function).
   * @return handle to the newly added IGetter.
   */
  template <class T, class U> 
  IGetter*
  declareMonitoredCollection(const std::string& name,  
			     const DataVector<T>* const & coll, U (T::*method)() const ) {

    return declareMonitoredCustomVariable(name, new StdContainerSmartGetter<const DataVector<T>*const, U >(coll, method));
  }

  /**
   * @brief Method to add a collection (DataVector) of variables to the monitored list.
   * @param name name of the collection of variables to add.
   * @param var reference to the pointer to the collection of variables to add.
   * @param method an external method, used to access object value (must take a pointer to objects in the collection as argument).
   * @return handle to the newly added IGetter
   */
  template <class T, class U> 
  IGetter*
  declareMonitoredCollection(const std::string& name, 
			     const DataVector<T>* const & coll, U (*method)(const T*)) {
 
    return declareMonitoredCustomVariable(name, new StdContainerSmartGetter<const DataVector<T>*const, U >(coll, method));
  }


  // Explicit specialization needed to avoid clang compilation error.
  template <class T> 
  IGetter*
  declareMonitoredCollection(const std::string& name, 
                            const DataVector<T>* const & coll, float (*method)(const T*)) {
 
    return declareMonitoredCustomVariable(name, new StdContainerSmartGetter<const DataVector<T>*const, float >(coll, method));
  }


  template <class T, class U> 
  IGetter*
  declareMonitoredCollection(const std::string& name, 
			     const std::vector<T> & coll, U (*method)(const T&)) {
 
    return declareMonitoredCustomVariable(name, new StdContainerSmartGetter<const std::vector<T>, U >(coll, method));
  }


  template <class T, class U> 
  IGetter*
  declareMonitoredCollection(const std::string& name, 
			     const std::vector<T> & coll, U (T::*method)() const) {

    return declareMonitoredCustomVariable(name, new StdContainerSmartGetter<const std::vector<T>, U >(coll, method));
  }




  /**
   * @brief Method to add a single-object variable to the monitored list.
   * @param name name of the variable to add.
   * @param obj is reference or pointer to the variable.
   * @param method a member method of the object, used to access its value (must be a void-argument function).
   * @return handle to the newly added IGetter.
   */
  template <class T, class U> 
  IGetter* declareMonitoredObject(const std::string& name, 
				  const T& obj, U (T::*method)() const ) {

    return declareMonitoredCustomVariable(name, new ObjectSmartGetter<const T, U, const T& >(obj, method));
  }


  /**
   * @brief Obj by ref, accessor by const ref
   */  
  template <class T, class U> 
  IGetter* declareMonitoredObject(const std::string& name, 
				  const T& obj, U (*method)(const T&) ) {
    
    return declareMonitoredCustomVariable(name, new ObjectSmartGetter<const T, U, const T&>(obj, method));
  }

    //

  /**
   * @brief Obj by ptr, accessor is const method
   */  
  template <class T, class U> 
  IGetter* declareMonitoredObject(const std::string& name, 
				  T const* const & obj, U (T::*method)() const ) {

    return declareMonitoredCustomVariable(name, new ObjectSmartGetter<const T * const, U, const T*>(obj, method));
  }

  /**
   * @brief Obj by ptr, accessor by const ptr
   */  
  template <class T, class U> 
  IGetter* declareMonitoredObject(const std::string& name, 
				  const T * const & obj, U (*method)(const T*) ) {

    return declareMonitoredCustomVariable(name, new ObjectSmartGetter<const T* const, U, const T*>(obj, method));
  }

  /**
   * @brief Obj by ref, accessor by const ptr
   */  
  template <class T, class U> 
  IGetter* declareMonitoredObject(const std::string& name, 
				  const T & obj, U (*method)(const T*) ) {

    return declareMonitoredCustomVariable(name, new ObjectSmartGetter<const T, U, const T*>(obj, method));
  }


  /**
   * @brief Obj by ptr, accessor by const ref
   */  
  template <class T, class U> 
  IGetter* declareMonitoredObject(const std::string& name, 
				  const T * const & obj, U (*method)(const T&) ) {

    return declareMonitoredCustomVariable(name, new ObjectSmartGetter<const T* const, U, const T&>(obj, method) );
  }
  
  /**
   * @brief Method to add custom monitoring
   */  
  IGetter* declareMonitoredCustomVariable(const std::string& name, IGetter* g );

  /**
   * @brief Method accessing the map of monitored variables, addressed by their names.
   * @return map of monitored variables, addressed by their names; this is to be used for inspecting the variables to be monitored.
   */
  typedef std::map<std::string, IMonitoredAlgo::IGetter*> VariablesMap;
  const VariablesMap& variables() const { 
    return m_proxies; 
  }

  ~IMonitoredAlgo();

private:
  /** @brief checks if algorithm already exported varaible of that name */
  bool assesName(const std::string& name); 
  
  /** @brief Map of monitored variables, addressed by their name. */
  VariablesMap  m_proxies; 
};

//////////////////////////////////////////////////////
// implementation



#endif

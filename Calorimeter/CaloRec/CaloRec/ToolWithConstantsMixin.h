// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  ToolWithConstantsMixin.h
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006, updated from June, 2004
 * @brief A mixin class to hold a set of constants for an Algorithm,
 *        Tool, or Service.
 */

#ifndef TOOLWITHCONSTANTSMIXIN_H
#define TOOLWITHCONSTANTSMIXIN_H


#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloConditions/Array.h"
#include "CaloConditions/ToolConstants.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/DataHandle.h" 

class Blob2ToolConstants;

namespace CaloRec {

/**
 * @class ToolWithConstantsMixin
 * @brief A mixin class to hold a set of constants for an Algorithm,
 *        Tool, or Service.
 *
 * Certain tools, such as correction tools, have a large number of constants
 * associated with them.  One does not want to hard-code these constants;
 * instead, the tools should be able to get them either from job options
 * or from a pool database.
 *
 * The interface works just like the property service, except that instead
 * of using @c declareProperty, one uses @c declareConstant.
 * This may be used either for fundamental types, like floats or ints,
 * or for the @c Array<N> classes.
 *
 * This is structured as a mixin class.  To use it, derive both from
 * this class (the `mixin class') and some class that derives from
 * one of @c Algorithm, @c AlgTool, or @c Service (the `base class').
 * Add a call to @c finish_ctor to your class's constructor.
 * You then must then define the three methods @c initialize
 * and @c setProperty with the signatures given below.
 * In each, you should first call the corresponding method
 * from the base class, followed by the one from the mixin class
 * (followed by anything needed by your class).
 *

 * The constants can come from three sources: jobOptions, a POOL file 
 * or COOL-inline storage. The object stored in the database is the
 * @c ToolConstants class defined in CaloConditions. In case of COOL
 * inline storage, a method from ToolConstants2Blob.h is used to 
 * convert the AttributeListCollection into a ToolConstants object.
 * The constants for several different correction tools may be merged 
 * together in a single @c ToolConstants object; each tool has a string 
 * prefix (unique within such a group) that is added to the constant name
 * when looking it up in the @c ToolConstants object.  The constants
 * object itself is located in the detector store under the key
 * given by the @c m_detStoreKey property.  The @c ToolConstants
 * instance found in StoreGate (if any) is referenced via
 * @c m_DBconstants; the private @c m_constants object holds
 * settings which were made via job options.
 *
 * Here is how things work: When @c declareConstant is called, we declare
 * the name to the property service with @c declareProperty.  If the
 * call is for a fundamental type, we can do it directly; otherwise,
 * if the call is for an @c Array<N>, we allocate a @c std::string
 * as a buffer and use that as the target.
 *
 * After everything is declared, the job options service will
 * make the actual property settings by calling the virtual
 * method @c setProperty for each variable that is indeed 
 * by jobOptions.We override this method so that we can
 * get control.  When we get called, we take the value we're given
 * and use it to make an entry in the @c ToolConstants object.
 * Further, if the call is for an @c Array<N>, we also at this point
 * initialize the original target @c Array from the @c Arrayrep.
 *
 * In our @c initialize method, we check if a @c ToolConstants object with 
 * the key @c m_detStoreKey is present in the DetectorStore.  If so, we 
 * register a IOV-Callback function for this object that gets in general 
 * called on the first event.  If no such object can be found, all constants 
 * must have been initialized by job options.  We check if this is the case
 * and complain if not. 
 *
 * In the callback function, we loop through all declared constants and 
 * check which ones have been already initialized by the JobOptionsSvc.
 * The ones that have not been initialized at this point are now initialized 
 * by the values loaded from the database.  Thus, constants can be either
 * set from job options or from the database.
 *
 * Possible problems and desired improvements:
 *
 * 1) I am not sure what happens if the first event is outside of any 
 * valid IOV. If the callback function is not triggered, some constants
 * might be uninitialized without any warning or error message. 
 *
 * 2) If (some of) the constants get initialized by jobOptions, no up-to-date 
 * @c ToolConstants objects is recorded to the DetectorStore. Some downstream
 * algorithm (E.g. @c MyAnalysis) that wants to know what constants have been
 * used might not get the right values. Some problems related the const-ness
 * of objects loaded from the DB need to be solved.
 */
class ToolWithConstantsMixin
  : virtual public IProperty,
    virtual public INamedInterface
{
public:
  // Constructor.
  ToolWithConstantsMixin();

  /**
   * @brief Remainder of constructor code, that needs the complete
   *        class to be initialized.
   * @param toolcls Name of derived C++ class.
   *                This is used only for @c mergeConstants; further,
   *                it can be safely defaulted for the @c AlgTool case.
   *
   * This needs to be called from the derived class's constructor.
   */
  void finish_ctor (const std::string& toolcls = "");


  /**
   * @brief Destructor.
   */
  virtual ~ToolWithConstantsMixin();

  /**
   * @brief Initialize method.
   * @return Gaudi status code.
   *
   * Here, we check that all declared constants actually received values.
   *
   * The class using this must override @c initialize and call first
   * the standard @c AlgTool::initialize followed by
   * @c ToolWithConstantsMixin::initialize.
   */
  virtual StatusCode initialize();


  /**
   * @brief IOVDB callback function
   * @param Macro defined in AthenaKernel/IOVSvcDefs.h
   *
   * This function is called when ToolConstants are loaded from pool.
   * Calls fillFromDB
   */
  virtual StatusCode processConstantsFromDB(IOVSVC_CALLBACK_ARGS);   


  /* @brief Set parameters coming from DB 
   * @param constants Pointer to the ToolConstants object loaded from DB 
   *
   * This method 
   * It checks which constants have been already set by the PropertySvc,
   * other constants are set by this function. 
   */
  virtual StatusCode fillFromDB(const ToolConstants* constants);

  /* @brief Dump method (for debugging)
   * @param stream Ostream to write to 
   * @param name Name to go in output
   */
  virtual void writeConstants(std::ostream& stream, const std::string& name) const;


  /**
   * @brief Merge our constants into @c out with the proper prefix.
   * @param[out] out Object to receive our constants.
   */
  virtual StatusCode mergeConstants (ToolConstants& out) const;

  /**
   * @brief Declare a constant.
   * @param name The name of the constant.
   * @param c Variable to receive the constant value.
   * @param deflt If true, then this constant may be defaulted.
   *
   * This should be called from a tool constructor.
   * The @a c variable will be initialized either from the pool
   * database or from job options, as appropriate.  If the initialization
   * is from job options, it won't happen until after the constructor
   * returns.
   *
   * The @a c variable may be a fundamental numeric type, or it may
   * be an instance of one of the @c Array<N> classes.
   */
  template <class T>
  void declareConstant (const std::string& name,
                        T& c,
                        bool deflt = false);


  /**
   * @brief Return the version number for this tool.
   *
   * A saved set of constants includes both the C++ class name and
   * a version number.  The idea is that the version number can
   * be bumped whenever there's a backwards-incompatible change;
   * this gives some protection against trying to use an old version
   * of a tool with an incompatible newer set of constants.
   *
   * If you want a tool to have a version number, override this method.
   * Otherwise, it will default to a version number of 0.
   */
  virtual int version() const;


  //***********************************************************************
  // The following methods are called only internally.
  //

  using IProperty::setProperty;
  /**
   * @brief Method to set a property value.
   * @param propname The name of the property to set.
   * @param value The value to which to set it.
   * @return Gaudi status code.
   *
   * The property service calls this method when it actually makes a property
   * setting.  We override it in order to hook our own code into the process.
   * If this property corresponds to one of our declared
   * constants, we install it in the @c ToolConstants structure.
   * If needed, we also complete initialization of the declared
   * variable here.
   *
   * The class using this must override this method, and call first
   * @c AlgTool::setProperty
   * followed by @c ToolWithConstantsMixin::setProperty.
   * You may need to use a using directive to prevent warnings
   * (like above, but using the other base class).
   *
   * This version is used by Gaudi up to at least v16.
   */
  virtual StatusCode setProperty (const std::string& propname,
                                  const std::string& value);

  /**
   * @brief Method to set a property value.
   * @param p The property name/value to set.
   * @return Gaudi status code.
   *
   * The property service calls this method when it actually makes a property
   * setting.  We override it in order to hook our own code into the process.
   * If this property corresponds to one of our declared
   * constants, we install it in the @c ToolConstants structure.
   * If needed, we also complete initialization of the declared
   * variable here.
   *
   * The class using this must override this method, and call first
   * @c AlgTool::setProperty
   * followed by @c ToolWithConstantsMixin::setProperty.
   * You may need to use a using directive to prevent warnings
   * (like above, but using the other base class).
   *
   * This version is used by Gaudi as of v19.
   */
  virtual StatusCode setProperty (const Gaudi::Details::PropertyBase& p);


private:
  /**
   * @class Propinfo_Base
   * @brief Base class for registered constants.
   *
   * For each constant that's registered, we create an instance of a
   * class deriving from @c Propinfo_Base.  The derived class depends
   * on the type of constant we're setting.  In the base class, we record
   * the constant name and a flag telling whether this constant has
   * been initialized.
   */
  struct Propinfo_Base
  {
    /**
     * @brief Constructor.
     * @param name The name of the constant.
     * @param deflt If true, then this constant may be defaulted.
     */
    Propinfo_Base (const std::string& name, bool deflt);

    /**
     * @brief Destructor.
     *
     * Just so that this class gets a vtable.
     */
    virtual ~Propinfo_Base();

    /**
     * @brief Finish up initialization of this constant.
     * @param rep The @c Arrayrep for this constant.
     * @param tool The @c ToolWithConstants instance, for error reporting.
     *
     * This is called from @c setProperty after the property manager
     * has finished its work, and after we've installed the constant
     * in the @c ToolConstants structure.  We get the
     * @c Arrayrep from the @c ToolConstants for this constant.
     *
     * The action needed here depends on the constant type.
     * This default implementation doesn't do anything.
     */
    virtual StatusCode set (const Arrayrep& rep,
                            ToolWithConstantsMixin& tool);


    /**
     * @brief Set a constant from the Database.
     * @param rep The @c Arrayrep for this constant.
     * @param tool The @c ToolWithConstants instance, for error reporting.
     *
     * Called explicitly by the callback function.
     *
     * The action needed here depends on the constant type.
     * This default implementation doesn't do anything.
     */
    virtual StatusCode setFromDB (const Arrayrep& rep,
				  ToolWithConstantsMixin& tool); 

    /// The constant name.
    std::string m_name;

    /// This is set to true once this constant has been initialized by jobO.
    bool m_set;

    /// This is set to true once this constant has been filled from the DB
    bool m_setDB;

    /// This is set of this constant has a usable default value.
    /// If true, don't complain if this constant doesn't get set.
    bool m_default;

  };


  /**
   * @class Propinfo_Val
   * @brief @c Propinfo class for numeric constants.
   *
   * This class derives from @c Propinfo_Base and serves to describe
   * constants that are simple numbers.
   */
  template <class T>
  struct Propinfo_Val
    : public Propinfo_Base
  {
    /**
     * @brief Constructor.
     * @param name The name of the constant.
     * @param c Reference to the constant.
     * @param deflt If true, then this constant may be defaulted.
     *
     * The constant gets declared to the property service.
     */
    Propinfo_Val (const std::string& name, T& c, bool deflt);

    /**
     * @brief Set a constant (not an Array) from the DB.
     * @param rep The @c Arrayrep for this constant.
     * @param tool The @c ToolWithConstants instance, for error reporting.
     *
     * The data-base representation of a constant is Arrayrep event if is a
     * single number. We require that the array size is 1 and take the 
     * element [0]. It is casted from const float to the templated type T.
     */
    StatusCode setFromDB(const Arrayrep& rep,
			 ToolWithConstantsMixin& tool);

    /**
     * @brief Finish up initialization of this constant.
     * @param rep The @c Arrayrep for this constant.
     * @param tool The @c ToolWithConstants instance, for error reporting.
     *
     * This is called from @c setProperty after the property manager
     * has finished its work, and after we've installed the constant
     * in the @c ToolConstants structure.  We get the
     * @c Arrayrep from the @c ToolConstants for this constant.
     *
     * For a simple number, the target will have already been filled
     * in by Gaudi.  However, it turns out that the floating-point
     * parsing done by Gaudi isn't exactly equivalent to that done
     * by C++.  The Array parsing machinery uses the C++ iostream
     * mechanism to do the conversion, and will convert the string
     * `1.525' to the float 1.52499998.  Gaudi, however, uses
     * the Spirit parser, which will convert the same string
     * to the float 1.5250001.  So, reset the target here
     * for consistency.
     */
    virtual StatusCode set (const Arrayrep& rep,
                            ToolWithConstantsMixin& tool);

    /// Reference to the constant.
    T& m_c;                               
  };


  /**
   * @class Propinfo_Array<N>
   * @brief @c Propinfo class for multidimensional arrays.
   *
   * This class derives from @c Propinfo_Base and serves to describe
   * constants that are instances of @c Array<N>.
   */
  template <unsigned int N>
  struct Propinfo_Array
    : public Propinfo_Base
  {
    /**
     * @brief Constructor.
     * @param name The name of the constant.
     * @param c Reference to the constant.
     * @param deflt If true, then this constant may be defaulted.
     */
    Propinfo_Array (const std::string& name, Array<N>& c, bool deflt);

    /**
     * @brief Finish up initialization of this constant.
     * @param rep The @c Arrayrep for this constant.
     * @param tool The @c ToolWithConstants instance, for error reporting.
     *
     * This is called from @c setProperty after the property manager
     * has finished its work, and after we've installed the constant
     * in the @c ToolConstants structure.  We get the
     * @c Arrayrep from the @c ToolConstants for this constant.
     *
     * For an @c Array<N>, we need to initialize the actual @c Array<N>
     * instance from the @c Arrayrep we've gotten from the
     * @c ToolConstants structure.
     */
    virtual StatusCode set (const Arrayrep& rep,
                            ToolWithConstantsMixin& tool);

    
    /**
     * @brief Setting an Array-constant from the DB.
     * @param rep The @c Arrayrep for this constant.
     * @param tool The @c ToolWithConstants instance, for error reporting.
     *
     * Simply forwards to the normal set function
     */
    virtual StatusCode setFromDB (const Arrayrep& rep,
				  ToolWithConstantsMixin& tool); 

    /// Reference to the constant.
    Array<N>& m_c;

    /// String buffer that we use in the declaration to the property service.
    std::string m_buf;
  };


  /**
   * @brief Add a new constant to our store.
   * @param pi The @c Propinfo instance describing this constant.
   */
  void add_constant (Propinfo_Base* pi);


  /**
   * @brief Construct a new @c Propinfo_* instance for @a c.
   * @param name The name of the constant being declared.
   * @param c The constant being declared.
   * @param deflt If true, then this constant may be defaulted.
   * @return A new instance of a class deriving from @c Propinfo_Base.
   *
   * This family of overloaded functions creates a new instance
   * of a class deriving from @c Propinfo_Base that is appropriate
   * for the type of the constant @c.
   *
   * This one makes a @c Propinfo_Val<T> instance,
   * appropriate for numeric types, and declares it to the property service.
   */
  template <class T>
  Propinfo_Base* makeinfo (const std::string& name, T& c, bool deflt);


  /**
   * @brief Construct a new @c Propinfo_* instance for @a c.
   * @param name The name of the constant being declared.
   * @param c The constant being declared.
   * @param deflt If true, then this constant may be defaulted.
   * @return A new instance of a class deriving from @c Propinfo_Base.
   *
   * This family of overloaded functions creates a new instance
   * of a class deriving from @c Propinfo_Base that is appropriate
   * for the type of the constant @c.
   *
   * This one makes a @c Propinfo_Array<N> instance,
   * appropriate for multidimensional array types, and declares
   * it to the property service.
   */
  template <unsigned int N>
  Propinfo_Base* makeinfo (const std::string& name, Array<N>& c, bool deflt);


  /// Information about declared constants.
  std::vector<Propinfo_Base*> m_infos;

  /// Contains a copy of all declared constants.
  ToolConstants m_constants;

  /// Point to object loaded from Database:
  const DataHandle<ToolConstants> m_DBconstants;

  /// Point to AttributeListCollection for COOL inline storage (alternativ to m_DBconstants)
  //const DataHandle<CondAttrListCollection>  m_COOLconstants;

  /// The COOL folder in case of COOL inline storage of the constants
  std::string m_folderName;

  /// Detector Store Service:
  StoreGateSvc* m_detStore;


protected:
  /// StoreGate key for conditions object.
  /// If empty, then we don't try to read from the database.
  std::string m_detStoreKey;

  /// Prefix for finding our constants within the ToolConstants
  /// object we retrieve from SG.
  std::string m_prefix;

  /// Flag that we want to set up the callback for the constants.
  /// This should be on for reading from cool, and off for reading
  /// from pool.
  // (Why should it be off for pool?  If it's on, then IOVSvc will try
  // to call CondProxyProvider::updateAddress on our proxy, and this
  // will fail [just returns FAILURE].  This will then cause IOVSvc
  // to give up on making any callbacks, which can break other tools.)
  bool m_use_callback;

  /// Name of the tool/algorithm/service.
  std::string m_toolcls;

  /// Used to fix the ordering of tools when we're initializing from COOL
  /// based on a hierarchical tag.  Tools should be executed in order
  /// of increasing m_order.
  int m_order;

  /// If true, then this is a dummy tool that should not be executed.
  /// This is used for the case of reading from COOL using hierarchical
  /// tags: we need to have such tags associated with some object in
  /// each folder, regardless of whether or not the correction from
  /// that folder is actually used.
  /// [Every folder that IOVDbSvc knows about at configuration time
  /// needs to have a valid object for the configured tag, else IOVDbSvc
  /// will raise a fatal error.  But we don't know at configuration time
  /// which folders we're actually going to need, so we gotta configure
  /// all of them.]
  bool m_isdummy;

  /// Public AlgTool to convert COOL inline data into ToolConstants objects
  Blob2ToolConstants* m_coolInlineTool;

  ///Pointer to ToolSvc
  IToolSvc* m_toolSvc;

};


} // namespace CaloRec


#include "CaloRec/ToolWithConstantsMixin.icc"


#endif // not TOOLWITHCONSTANTS_H


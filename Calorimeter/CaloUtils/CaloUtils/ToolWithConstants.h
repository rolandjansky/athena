// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloUtils/ToolWithConstants.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Hold constants for a tool.
 *
 * An extension to Gaudi components to manage a set of Constants,
 * which are like properties but are set from COOL with the possibility
 * of overriding them from job options.
 *
 * See the class @c ToolWithConstants below for more information.
 */

#ifndef CALOUTILS_TOOLWITHCONSTANTS_H
#define CALOUTILS_TOOLWITHCONSTANTS_H


#include "CaloUtils/exceptions.h"
#include "CaloConditions/ToolConstants.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/concepts.h"
#include "Gaudi/Property.h"
#include <string>
#include <sstream>
#include <type_traits>


namespace Gaudi {
namespace Parsers {


// Unused, because we override fromString() below, but required
// in order to compile the overridden fromString() from Gaudi.
template <unsigned int N>
StatusCode parse (CaloRec::Array<N>&, const std::string&)
{
  return StatusCode::FAILURE;
}


}} // namespace Gaudi::Parsers


namespace CaloUtils {


/**
 * @brief Context object for retrieving @c ToolConstant values.
 *
 * Should be treated as opaque by clients, except to call ctx().
 */
struct ToolConstantsContext
{
public:
  const EventContext& ctx() const { return m_ctx; }

  ToolConstantsContext (const CaloRec::ToolConstants* constants,
                        const EventContext& ctx)
    : m_constants (constants),
      m_ctx (ctx)
  {
  }
  const CaloRec::ToolConstants* m_constants;
  const EventContext& m_ctx;
};



/**
 * @brief Internal interface for dealing with a collection of @c ToolConstants.
 */
class IToolConstant
{
public:
  /// Virtual destructor.
  virtual ~IToolConstant() = default;

  /// The name of the constant.
  virtual std::string name() const = 0;

  /// True if this constant was initialized from job options.
  virtual bool setFromJO() const = 0;
};


//***********************************************************************


/**
 * @brief Internal implementation object.
 *
 * Allows factoring out code which doesn't depend on template parameters.
 */
class ToolWithConstantsImpl
{
public:
  using Context = ToolConstantsContext;


  /**
   * @brief Constructor
   * @param toolName Name of the tool.
   * @param prefix Constant prefix in COOL.
   * @param DBHandle Handle key for the conditions object.
   */
  ToolWithConstantsImpl (const std::string& toolName,
                         const std::string& prefix,
                         SG::ReadCondHandleKey<CaloRec::ToolConstants>& DBHandle);


  /**
   * @brief Initialize.
   */
  StatusCode initialize();


  /**
   * @brief Create a @c Context object.
   *
   * This can then be passed to @c Constant::operator().
   */
  Context context (const EventContext& ctx) const;


  /**
   * @brief Add a new constant to the tool.
   * @param c Constant to add.
   */
  void addConstant (IToolConstant* c);


  /**
   * @brief Dump method (for debugging)
   * @param stream Ostream to which to write.
   * @param name Name to go in output
   * @param ctx Event context.
   */
  void writeConstants (std::ostream& stream,
                       const std::string& name,
                       const EventContext& ctx) const;


  /**
   * @brief Merge our constants into @c out with the proper prefix.
   * @param typeName Name of the type of the tool.
   * @param toolVersion Version of the tool.
   * @param myctx Internal Context structure.
   * @param isDummy isDummy constant from the tool
   * @param[out] out Object to receive our constants.
   */
  StatusCode
  mergeConstants (const std::string& typeName,
                  int toolVersion,
                  Context& myctx,
                  bool isDummy,
                  CaloRec::ToolConstants& out) const;


  /// Name of the owning tool.
  std::string m_toolName;

  /// The prefix of this constant in COOL.
  // Note: Held by a reference, not by value.
  // This comes from a property of the owning tool, and is not set
  // until we run initialize() on that tool.
  const std::string& m_prefix;

  /// For settings made from JO.
  CaloRec::ToolConstants m_constants;

  /// Handle to a ToolConstants conditions object.
  SG::ReadCondHandleKey<CaloRec::ToolConstants>& m_DBHandle;

  /// All constants declared to this tool.
  std::vector<IToolConstant*> m_declaredConstants;
};


//***********************************************************************


/**
 * @brief Constant of a ToolWithConstants.
 *
 * See @c ToolWithContants below for usage information.
 */
template <class T>
  // Must be able to initialize a T with fromArrayrep
  // Doesn't actaully work like this, though, as there's a conflict
  // with the friend declaration below.
  //ATH_REQUIRES( requires (T t, const CxxUtils::Arrayrep& rep) { { CxxUtils::fromArrayrep (rep, t); } } )
class ToolConstant
  : public IToolConstant
{
public:
  /// Alias for the context object
  using Context = ToolConstantsContext;


  /**
   * @brief Declare a constant with no default.
   * @param owner The owning @c ToolWithConstants.
   * @param name Name of the constant.
   * @param doc Documentation string.
   */
  template <class OWNER>
  ToolConstant (OWNER* owner,
                const std::string& name,
                const std::string& doc = "");


  /**
   * @brief Declare a constant with no default.
   * @param owner The owning @c ToolWithConstants.
   * @param name Name of the constant.
   * @param doc Documentation string.
   *
   * (Needed in addition to the previous signature in order to avoid ambiguities;
   * otherwise, if a char* is given for the third argument, it would
   * match the following signature instead of the previous one.)
   */
  template <class OWNER>
  ToolConstant (OWNER* owner,
                const std::string& name,
                const char* doc);


  /**
   * @brief Declare a constant with a default.
   * @param owner The owning @c ToolWithConstants.
   * @param name Name of the constant.
   * @param deflt Default value.
   * @param doc Documentation string.
   * 
   * Only possible for arithmetic types, not @c Array<N>.
   */
  template <class OWNER,
            typename U = T,
            typename = std::enable_if_t<std::is_arithmetic_v<U> > >
  ToolConstant (OWNER* owner,
                const std::string& name,
                const T& deflt,
                const std::string& doc = "");


  /** 
   * @brief Retrieve the value of a constant.
   * @param c Context for accessing condtions (as returned from context()).
   */
  T operator() (const Context& c) const;


  /** 
   * @brief Retrieve the value of a constant.
   *
   * This variant may only be used if the constant was initialized via JO.
   */
  T operator()() const;


  /**
   * @brief Return the name of this constant.
   */
  virtual std::string name() const override;


  /**
   * @brief Was this constant set through job options?
   */
  virtual bool setFromJO() const override;

  
private:
  /**
   * @brief Internal wrapper for Gaudi::Property.
   *        Handles the setFromJO flag and the Arrayrep conversion.
   */
  class CProperty
    : public Gaudi::Property<T>
  {
  public:
    /** 
     * @brief Constructor, no default value.
     * @param name Constant name.
     * @param doc Documentation string.
     * @param impl Internal implementation object.
     */
    CProperty (const std::string& name,
               const std::string& doc,
               ToolWithConstantsImpl& impl);

      
    /** 
     * @brief Constructor, with default value.
     * @param name Constant name.
     * @param doc Documentation string.
     * @param impl Internal implementation object.
     * @param deflt Default value.
     */
    CProperty (const std::string& name,
               const std::string& doc,
               ToolWithConstantsImpl& impl,
               const T& deflt);


    // To be able to assign to this wrapper.
    using Gaudi::Property<T>::operator=;


    /**
     * @brief Return the value of this property as a string.
     */
    virtual std::string toString() const override;


    /**
     * @brief Print the value of this property to a stream.
     * @param out Stream to which to print.
     */
    virtual void toStream( std::ostream& out ) const override;


    /** 
     * @brief Initialize this propery's value from a string.
     * @param value String from which to initialize.
     */
    virtual StatusCode fromString( const std::string& value ) override;


    /// Reference to implementation object.
    ToolWithConstantsImpl& m_impl;

    /// Has this constant been set from JO?
    bool m_setFromJO = false;

    /// Does this constant have a default?
    bool m_hasDefault = false;
  };


  /// Internal Property instance, for getting JO settings from Gaudi.
  CProperty m_prop;

  /// Version of the owning tool.
  int m_toolVersion;
};


//***************************************************************************


/**
 * @brief Hold constants for a tool.
 *
 * Certain tools, such as correction tools, have a large number of constants
 * associated with them.  One does not want to hard-code these constants;
 * instead, the tools should be able to get them either from job options
 * or from a COOL database.
 *
 * This class extends Gaudi components to manage a set of `Constants',
 * which are like properties but can be set either from job options
 * or from COOL (or possibly defaulted).  The constants may either
 * be scalar arithmetic types (floating-point, integer, boolean)
 * or an specialization of CxxUtils::Array<N>.  (Note, however, that
 * the values are always stored as single-precision floating-point.)
 *
 * A Constant may be declared similarly to a Gaudi property.  To retrieve
 * its value, call context() and then pass that to the operator() method
 * of the Constant.
 *
 * This is probably best shown as an example.
 *
 *@code
 *
 *  class MyTool
 *    : public CaloUtils::ToolWithConstants<AthAlgTool>
 *  {
 *    // Define a floating-point constant, with a default value.
 *    Constant<float> m_cf
 *    { this, "FloatConst", 1.5, "A description" };
 *
 *    // Define an integer constant, with no default value.
 *    Constant<int> m_ci
 *    { this, "IntConst", "A description" };
 *
 *    // Define a two-dimensional array constant.
 *    // Array constants do not have defaults.
 *    Constant<CxxUtils::Array<2> > m_ca
 *    { this, "ArrayConst", "A description" };
 *
 *  public:
 *    // The constructor can be delegated to the base class.
 *    using base_base::base_class;
 *
 *    // If you have an initialize() method, you must call the
 *    // base class version.
 *    virtual StatusCode initialize() override
 *    {
 *       ATH_CHECK( base_class::initialize() );
 *       return StatusCode::SUCCESS;
 *    }
 *
 *    void doit (const EventContext& ctx) const
 *    {
 *      // Make a local context object.
 *      Context myctx = context (ctx);
 *
 *      // Retrieve the constants.
 *      float cf = m_cf (myctx);
 *      int ci = m_ci (myctx);
 *      CxxUtils::Array<2> ca = m_ca (myctx);
 @endcode
 *
 * Constants that come from COOL are kept in a @c ToolConstants conditions
 * object; the key of this object is set from the @c DBHandleKey property.
 * Constants for multiple tools may be combined in a single @c ToolConstants
 * object.  Each tool has a string  prefix (unique within such a group)
 * that is added to the constant name when it is looked up in the
 * @c ToolConstants object. 
 * 
 * For data that are stored as inline COOL data, a conditions algorithm
 * is used to populate a @c ToolConstants object from COOL.
 *
 * Settings from COOL may be overridden with settings from job options.
 * For values that are arrays, the value should be set as a string.
 */
template <class BASE>
  ATH_REQUIRES( requires (BASE& b, Gaudi::Property<int>& p) { { b.declareProperty(p) }; } )
class ToolWithConstants
  : public BASE
{
public:
  /// Shorthand for derived classes.
  typedef ToolWithConstants base_class;


  /// Inherit constructor.
  using BASE::BASE;


  /// Convenient alias for the Context type.
  using Context = CaloUtils::ToolConstantsContext;


  /// Alias for the Constant type.
  template <class T>
  using Constant = ToolConstant<T>;


  /**
   * @brief Initialize method.  Derived classes must call this.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Create a @c Context object.
   *
   * This can then be passed to @c Constant::operator().
   */
  Context context (const EventContext& ctx) const;


  /**
   * @brief Dump method (for debugging)
   * @param stream Ostream to which to write.
   * @param name Name to go in output
   * @param ctx Event context.
   */
  virtual void writeConstants (std::ostream& stream,
                               const std::string& name,
                               const EventContext& ctx) const;


  /**
   * @brief Merge our constants into @c out with the proper prefix.
   * @param[out] out Object to receive our constants.
   * @param ctx Event context.
   */
  virtual StatusCode mergeConstants (CaloRec::ToolConstants& out,
                                     const EventContext& ctx) const;


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
  virtual int toolVersion() const;


private:
  // Make this a friend so it can access m_impl.
  template <class T>
  friend class CaloUtils::ToolConstant;

  /// Handle to a ToolConstants conditions object.
  SG::ReadCondHandleKey<CaloRec::ToolConstants> m_DBHandle
  { this, "DBHandleKey", "", "" };

  /// Prefix for finding our constants within the ToolConstants object.
  StringProperty m_prefix
  { this, "prefix", "", "" };

  /// Internal implementation object.
  ToolWithConstantsImpl m_impl { this->name(), m_prefix, m_DBHandle };

  /// Used to fix the ordering of tools when we're initializing from COOL
  /// based on a hierarchical tag.  Tools should be executed in order
  /// of increasing m_order.
  Constant<int> m_order
    { this, "order", 0 };

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
  Constant<bool> m_isdummy
    {this, "isDummy", false };
};


} // namespace CaloUtils


#include "CaloUtils/ToolWithConstants.icc"


#endif // not CALOUTILS_TOOLWITHCONSTANTS_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRingerProcedure.h 694257 2015-09-10 22:45:27Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_IPROCEDURE_H
#define RINGERSELECTORTOOLS_PROCEDURES_IPROCEDURE_H

// STL includes:
#include <vector>

// Root includes:
#include <TDirectory.h>
//#include <TClass.h>

// Local includes:
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/IRedirectMsgStream.h"
#include "RingerSelectorTools/tools/cxx/StaticAssert.h"
#include "RingerSelectorTools/tools/cxx/is_base_of.h"
#include "RingerSelectorTools/tools/cxx/conditional.h"
#include "RingerSelectorTools/tools/cxx/enable_if.h"

#ifndef INCLUDE_HEADER_ONLY
#define INCLUDE_HEADER_ONLY
#endif // INCLUDE_HEADER_ONLY
#include "RingerSelectorTools/procedures/Types.h"
#undef INCLUDE_HEADER_ONLY

/**
 * Define ringer interface default methods
 *
 * It defines static template member type, that can be used to retrieve
 * procedure type as an enumeration type. In order for this to work, make sure
 * to add the enumeration value on Types.h file and add it properly to the
 * conversion string<->enumeration function in the Types.cxx file.
 *
 * Beware that it changes scope inside class to public.
 *  
 **/
#define RINGER_DEFINE_INTERFACE(self)                                          \
                                                                               \
  public:                                                                      \
                                                                               \
    template <typename T = const char*>                                        \
    static T procType();                                                       \
                                                                               \
    virtual const char* name() const = 0;                                      \
                                                                               \
    virtual void print(MSG::Level lvl) const = 0;

/**
 * Define Ringer interface procedure type methods.
 **/
#define RINGER_DEFINE_INTERFACE_DEFAULT_METHODS(self)                          \
                                                                               \
  template<>                                                                   \
  inline                                                                       \
  const char* self::procType()                                                 \
  {                                                                            \
    return #self;                                                              \
  }                                                                            \
                                                                               \
  template<>                                                                   \
  inline                                                                       \
  Ringer::RingerProcedureType<self>::procEnum_t self::procType()               \
  {                                                                            \
    return Ringer::getType<                                                    \
        Ringer::RingerProcedureType<self>::procEnum_t>(#self);                 \
  }

#define __RINGER_DEFINE_PROCEDURE_STANDARD_METHODS__(self)                     \
                                                                               \
    virtual const char* name() const                                           \
      ATH_RINGER_OVERRIDE;

/**
 * Use this macro when procedure has member properties
 **/
#define RINGER_DEFINE_PROCEDURE(self)                                          \
                                                                               \
  public:                                                                      \
    __RINGER_DEFINE_PROCEDURE_STANDARD_METHODS__(self)                         \
                                                                               \
    virtual void print(MSG::Level lvl) const                                   \
      ATH_RINGER_OVERRIDE;

/**
 * Use this macro when procedure hasn't member properties 
 **/
#define RINGER_DEFINE_NOMEMBER_PROCEDURE(self)                                 \
                                                                               \
  public:                                                                      \
    __RINGER_DEFINE_PROCEDURE_STANDARD_METHODS__(self)                         \
                                                                               \
    virtual void print(MSG::Level lvl) const                                   \
      ATH_RINGER_OVERRIDE                                                      \
    {                                                                          \
      if ( !this->isStreamAvailable() ) {                                      \
        std::cerr << "Cannot print " << this->name() << ", stream unavailable" \
          << std::endl;                                                        \
      }                                                                        \
      if ( this->level() > lvl ) {                                             \
        return;                                                                \
      }                                                                        \
      ATH_MSG_LVL(lvl, "Procedure hasn't any property.");                      \
    }
/// @}

/**
 * This macro should be defined for every Ringer Procedure defined after the
 * class is defined, within same scope and translation unit where it is
 * defined.
 *
 * You will have to define the procedure print method yourself.
 **/
#define RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS(self)                          \
                                                                               \
  /*RINGER_DEFINE_INTERFACE_DEFAULT_METHODS(self)*/                            \
                                                                               \
  inline                                                                       \
  const char* self::name() const                                               \
  {                                                                            \
    return this->procType<const char*>();                                      \
  }



/** 
 * @brief Namespace dedicated for Ringer utilities 
 **/
namespace Ringer {

// Declare interface base for all Ringer procedure types:
//
/**
 * @class IRingerProcedure
 * @brief The base interface for all Ringer procedures.
 *
 * Note: Every Ringer Procedure redirects message stream
 **/
class IRingerProcedure : virtual public IRedirectMsgStream { 
  public:
    /**
     * @brief Write Ringer procedure to configuration directory
     **/
    virtual void write(TDirectory *configDir, const char*idxStr = "") 
        const = 0;

    /** Ensures virtual dtor for all inherited classes */
    virtual ~IRingerProcedure(){;}

  protected: 

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(IRingerProcedure,0)
};

// Forward declare base procedure types:
namespace PreProcessing {
  class IPreProcessor;
  class IPreProcessorVarDep;
}

namespace Discrimination {
  class IDiscriminator;
  class IDiscriminatorVarDep;
  class IThreshold;
  class IThresholdVarDep;
}

/**
 * @class RingerProcedureType
 * @brief Check Ringer Procedure type.
 *
 * Only available to this translation unit.
 *
 *  - Use RingerProcedure<Procedure>::is_pre_processor to get if procedure is
 * of pre-processing type
 *
 *  - Use RingerProcedure<Procedure>::is_discriminator to get if procedure is
 * of discriminator type
 *
 *  - Use RingerProcedure<Procedure>::is_threshold to get if procedure is
 * of threshold type
 **/
template<typename procedure_t > 
struct RingerProcedureType {

  RINGER_STATIC_ASSERT( (Ringer::is_base_of<IRingerProcedure,procedure_t>::value),
      "Requested to check Ringer procedure type from class that is not a IRingerProcedure.");

  // Determine which procedure type it is:
#if RINGER_USE_NEW_CPP_FEATURES || defined(FORCE_RINGER_PROCEDURE_TYPE_CONST_EXPR)
  static constexpr bool is_pre_processor =
    Ringer::is_base_of<PreProcessing::IPreProcessor,procedure_t>::value;
  static constexpr bool is_discriminator =
    Ringer::is_base_of<Discrimination::IDiscriminator,procedure_t>::value;
  static constexpr bool is_threshold = 
    Ringer::is_base_of<Discrimination::IThreshold,procedure_t>::value;

  RINGER_STATIC_ASSERT( ( is_pre_processor || is_discriminator || is_threshold ), 
      "Couldn't find a procedure type.");

  // Determine which enumType it should have been declared
  typedef typename Ringer::conditional< is_pre_processor, 
    preProcEnum_t,  // true, it is_pre_processor
      typename Ringer::conditional< is_discriminator, 
    discrEnum_t, // true, it is discriminator
      thresEnum_t >::type >::type procEnum_t;

  // Boolean to determine whether this procedure inherits from VariableDependency
  static constexpr bool inherits_from_var_dep = Ringer::is_base_of< VariableDependency, 
                                                             procedure_t>::value;

  // Determine which interface this Ringer procedure inherits from
  typedef typename Ringer::conditional< is_pre_processor, 
    // true, it is_pre_processor
    typename Ringer::conditional< inherits_from_var_dep,
        PreProcessing::IPreProcessorVarDep, PreProcessing::IPreProcessor>::type, 
    // not pre_processor, check if is_discriminator
    typename Ringer::conditional< is_discriminator, 
      // true, it is_discriminator
      typename Ringer::conditional< inherits_from_var_dep,
          Discrimination::IDiscriminatorVarDep, Discrimination::IDiscriminator>::type, 
      // otherwise it has to be threshold
      typename Ringer::conditional< inherits_from_var_dep,
          Discrimination::IThresholdVarDep, Discrimination::IThreshold>::type   
    >::type 
  >::type baseInterface_t;

#else
  static const bool is_pre_processor;
  static const bool is_discriminator;
  static const bool is_threshold;
#endif
};

/**
 * @brief RingerProcedureType specialization to ensure that procedure_t is not
 *        a pointer
 **/ 
template<typename procedure_t >
struct RingerProcedureType<procedure_t*> : public RingerProcedureType<procedure_t>{ };

/** 
 * @brief Namespace dedicated for Ringer pre-processing utilities 
 **/
namespace PreProcessing {

/**
 * @brief PreProcessing interface to be inherited by PreProcessing procedures.
 **/
class IPreProcessor : virtual public IRingerProcedure 
{

  RINGER_DEFINE_INTERFACE( IPreProcessor )

  protected:
    /** Note: https://sft.its.cern.ch/jira/browse/CFHEP-87 */
    IPreProcessor& operator=(IPreProcessor&&);
  public:
    /**
     * @brief Execute transform @name inputSpace to a new space representation.
     **/
    virtual void execute(std::vector<float> &inputSpace) const = 0;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(IPreProcessor,0)
};

/**
 * @brief PreProcessor interface to be used by Ringer Wrapper.
 **/
class IPreProcessorVarDep : public virtual VariableDependency,
                            public virtual IPreProcessor
{

  RINGER_DEFINE_INTERFACE( IPreProcessorVarDep )

  protected:
    /** Note: https://sft.its.cern.ch/jira/browse/CFHEP-87 */
    IPreProcessorVarDep& operator=(IPreProcessorVarDep&&);
  public:
    //IPreProcessorVarDep(){;}

    // This is needed for parsing wrapper
    static IPreProcessorVarDep* read(TDirectory *){ return nullptr;}

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(IPreProcessorVarDep,0)
};

} // namespace PreProcessor

/** 
 * @brief Namespace dedicated for Ringer Discrimination utilities 
 **/
namespace Discrimination {

/**
 * @brief Discriminator interface to be inherited by discrimination procedures.
 **/
class IDiscriminator : virtual public IRingerProcedure 
{

  RINGER_DEFINE_INTERFACE( IDiscriminator )

  protected:
    /** Note: https://sft.its.cern.ch/jira/browse/CFHEP-87 */
    IDiscriminator& operator=(IDiscriminator&&);
  public:
    virtual void execute(const std::vector<float> &input, 
        std::vector<float> &output) const = 0;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(IDiscriminator,0)
};

/**
 * @brief Discriminator interface to be used by Ringer Wrapper.
 **/
class IDiscriminatorVarDep : public virtual VariableDependency,
                             public virtual IDiscriminator
{

  RINGER_DEFINE_INTERFACE( IDiscriminatorVarDep )

  protected:
    /** Note: https://sft.its.cern.ch/jira/browse/CFHEP-87 */
    IDiscriminatorVarDep& operator=(IDiscriminatorVarDep&&);
  public:
    //IDiscriminatorVarDep(){;}

    // This prototype is needed for parsing wrapper
    static IDiscriminatorVarDep* read(TDirectory *){ return nullptr;}

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(IDiscriminatorVarDep,0)
};

/**
 * @brief Threshold interface to be inherited by thresholding procedures.
 **/
class IThreshold : virtual public IRingerProcedure
{

  RINGER_DEFINE_INTERFACE( IThreshold )

  protected:
    /** Note: https://sft.its.cern.ch/jira/browse/CFHEP-87 */
    IThreshold& operator=(IThreshold&&);
  public:
    /**
     * @brief Execute threshold for input and retrieve throw output.
     **/
    virtual void execute(const std::vector<float> &input,
            std::vector<bool> &output) const = 0;
    
    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(IThreshold,0)
};

/**
 * @brief Threshold interface to be used by Ringer Wrapper.
 **/
class IThresholdVarDep : public virtual VariableDependency,
                         public virtual IThreshold
{

  RINGER_DEFINE_INTERFACE( IThresholdVarDep )

  protected:
    /** Note: https://sft.its.cern.ch/jira/browse/CFHEP-87 */
    IThresholdVarDep& operator=(IThresholdVarDep&&);
  public:
    //IThresholdVarDep(){;}
    
    // This prototype is needed for parsing wrapper
    static IThresholdVarDep* read(TDirectory *){ return nullptr;}

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(IThresholdVarDep,0)
};

} // namespace Discrimination

// Declare procedure type member functions
namespace PreProcessing {
RINGER_DEFINE_INTERFACE_DEFAULT_METHODS( IPreProcessor )
RINGER_DEFINE_INTERFACE_DEFAULT_METHODS( IPreProcessorVarDep )
} // namespace PreProcessing
namespace Discrimination {
RINGER_DEFINE_INTERFACE_DEFAULT_METHODS( IThreshold )
RINGER_DEFINE_INTERFACE_DEFAULT_METHODS( IThresholdVarDep )
RINGER_DEFINE_INTERFACE_DEFAULT_METHODS( IDiscriminator )
RINGER_DEFINE_INTERFACE_DEFAULT_METHODS( IDiscriminatorVarDep )
} // namespace Discrimination

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_PROCEDURES_IPROCEDURE_H

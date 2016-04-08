/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerIOVarDepObj.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_RINGERIOVARDEPOBJ_H
#define RINGERSELECTORTOOLS_TOOLS_RINGERIOVARDEPOBJ_H

// STL includes:
#include <string>

// Root includes:
#include <TFile.h>

// Local includes:
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"
#include "RingerSelectorTools/procedures/IRingerProcedure.h"
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/cxx/StaticAssert.h"
#include "RingerSelectorTools/tools/cxx/constexpr.h"
#include "RingerSelectorTools/tools/cxx/is_base_of.h"
#include "RingerSelectorTools/tools/cxx/override.h"
#include "RingerSelectorTools/tools/cxx/mutable.h"
#include "RingerSelectorTools/tools/cxx/final.h"

/**
 * @brief Create RingerIOVarDepObj base object prototype methods
 *
 * Every time you create a class that use this mechanism, make sure to add it
 * to "RingerSelectorTools/procedures/Types.h"
 *
 *
 * When you create a class that implements a RingerProcedure and it will be a
 * writable object, then this object must have defined this macro and
 * implement the read and write methods as:
 *
 * file NewProcedure.h:
 * /code
 * class NewProcedure: virtual public IDiscriminator
 *                     // or virtual public IPreProcessor
 *                     // or virtual public IThreshold
 * {
 *   // Given as example
 *   float m_var;
 *
 *   RINGER_IO_VARDEP_BASE( NewProcedure )
 *   // ...
 * }
 *
 * RINGER_DEFINE_PROCTYPE_MEMBER_FCN( NewProcedure )
 *
 * } // namespace Ringer
 * /endcode
 *
 * file NewProcedure.cxx
 * /code
 * namespace Ringer {
 *
 * void NewProcedure::read(NewProcedure* newObj,
 *    TDirectory *configDir,
 *    unsigned version )
 * {
 *   IOHelperFcns::readVar( configDir, "var",  newObj->m_var );
 * }
 *
 * void NewProcedure::write(TDirectory *configDir) const
 * {
 *   IOHelperFcns::writeVar(configDir, "var",  m_var  );
 * }
 *
 * void NewProcedure::print(MSG::Level lvl) const
 * {
 *   if ( !this->isStreamAvailable() ) {
 *     std::cerr << "Cannot print NewProcedure, stream unavailable"
 *       << std::endl;
 *   }
 *   if ( this->level() > lvl ) {
 *     return;
 *   }
 *   msg() << lvl << this->name() << " configuration:" << endreq;
 *   msg() << lvl << "var:" << m_var << endreq;
 * }
 *
 * } // namespace Ringer
 * /endcode
 *
 * If the procedure has no variable, it can use the
 * RINGER_IO_VARDEP_BASE_NOMEMBER macro instead.
 *
 * The previous structure will need also to implement the VariableDependency
 * class, which will inherit from the RingerIOVarDepObj using the
 * RINGER_IO_VARDEP_OBJ macro described next.
 *
 * /code
 * class NewProcedureVarDep : virtual public IDiscriminatorVarDep,
 *                      public RingerIOVarDepObj<NewProcedureVarDep>,
 *                      public NewProcedure
 * {
 *     RINGER_IO_VARDEP_OBJ(NewProcedureVarDep, NewProcedure)
 *     // ...
 * }
 * /endcode
 **/

#define RINGER_IO_VARDEP_BASE(self)                                            \
  public:                                                                      \
                                                                               \
    RINGER_DEFINE_PROCEDURE(self)                                              \
                                                                               \
    static void read(self *newObj,                                             \
        TDirectory *configDir,                                                 \
        unsigned version );                                                    \
                                                                               \
    virtual void write(TDirectory *configDir, const char* idxStr = "")         \
      const ATH_RINGER_OVERRIDE;

/**
 * @brief Same as RINGER_IO_VARDEP_BASE, but when class has no member
 **/
#define RINGER_IO_VARDEP_BASE_NOMEMBER(self)                                   \
  public:                                                                      \
                                                                               \
     RINGER_DEFINE_NOMEMBER_PROCEDURE(self)                                    \
                                                                               \
    static void read(self *,                                                   \
        TDirectory *,                                                          \
        unsigned ) {;}                                                         \
                                                                               \
    virtual void write(TDirectory *, const char*) const                        \
      ATH_RINGER_OVERRIDE {;}

/**
 * @brief Add necessary information to RingerIOVarDepObj
 *
 * When declaring the new procedure variable dependent object, you will need to
 * add this macro adding to add its base type where you used the
 * RINGER_IO_VARDEP_BASE macro.
 *
 *
 **/
#define RINGER_IO_VARDEP_OBJ(self, base)                                       \
                                                                               \
  public:                                                                      \
    typedef base base_t;                                                       \
                                                                               \
    using base::procType;                                                      \
                                                                               \
    virtual const char* name() const                                           \
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL                                     \
    {                                                                          \
      return this->RingerIOVarDepObj<self>::name();                            \
    }                                                                          \
                                                                               \
    void print(MSG::Level lvl) const                                           \
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL                                     \
    {                                                                          \
      this->base_t::print(lvl);                                                \
    }                                                                          \
                                                                               \
    virtual void write(TDirectory *baseDir, const char* idxStr = "") const     \
      ATH_RINGER_OVERRIDE ATH_RINGER_FINAL                                     \
    {                                                                          \
      this->RingerIOVarDepObj<self>::write(baseDir, idxStr);                   \
    }                                                                          \
                                                                               \
    static self* read(TDirectory *configDir){                                  \
      return RingerIOVarDepObj<self>::read(configDir);                         \
    }                                                                          \
                                                                               \
    typedef typename Ringer::template RingerProcedureType<base>::              \
      baseInterface_t baseInterface_t;                                         \
                                                                               \
    using baseInterface_t::procType;

namespace Ringer {

/**
 * @class RingerIOVarDepObj
 * @brief Objects that can be recorded/read into/from configuration files.
 *
 * Use CRTP mechanism so that we can force static read method to be
 * implemented.
 *
 * The CRTP class will automatically inherit from BaseProcedure and
 * VariableDependency, the late using etaDependency and etDependency given
 * values.
 *
 * It also implements the final version of the IRingerProcedure write pure
 * virtual method, which will call both VariableDependency and
 * the BaseProcedure write methods. It also make sure that the configuration
 * directory written to Root will be named as:
 *
 * "BaseProcedure::type()[VariableDependency::type()]"
 *
 **/
template < class CRTP  >
class RingerIOVarDepObj : virtual public VariableDependency
{

  public:
    /**
     * Typedef to CRTP type:
     **/
    //typedef CRTP crtp_t;

    // Make sure we initialize the variable dependency
    RingerIOVarDepObj():VariableDependency(){;}
    
    /**
     * Retrieve name for this RingerIOVarDepObj
     **/
    const char* name() const;

    /**
     * @brief Read Ringer procedure from configuration dir
     **/
    static CRTP* read(TDirectory *configDir);

    /**
     * @brief Write Ringer procedure to configuration directory
     **/
    void write(TDirectory *baseDir, const char* idxStr="")
      const;

  private:
    ATH_RINGER_MUTABLE std::string m_name;

};

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_TOOLS_RINGERIOVARDEPOBJ_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#include "RingerIOVarDepObj.icc"
#endif // INCLUDE_HEADER_ONLY

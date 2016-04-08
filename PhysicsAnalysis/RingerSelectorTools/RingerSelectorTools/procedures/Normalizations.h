/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Normalizations.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_NORMALIZATIONS_H
#define RINGERSELECTORTOOLS_PROCEDURES_NORMALIZATIONS_H

//#undef NDEBUG

// STL includes:
#include <vector>
#include <stdexcept>
#include <string>
#include <cmath>
#include <iostream>

// Gaudi includes:
#if !defined(XAOD_STANDALONE) && !defined(RINGER_STANDALONE)
# include "GaudiKernel/SystemOfUnits.h"
#endif


// Local includes:
#include "RingerSelectorTools/procedures/IRingerProcedure.h"
#include "RingerSelectorTools/tools/RingerIOVarDepObj.h"
#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/RedirectMsgStream.h"
#include "RingerSelectorTools/tools/cxx/final.h"
#include "RingerSelectorTools/tools/cxx/override.h"

/** 
 * @brief Namespace dedicated for Ringer utilities 
 **/
namespace Ringer
{

/** 
* @brief Namespace dedicated for Ringer pre-processing utilities 
**/
namespace PreProcessing
{

/** 
 * @brief Namespace dedicated for Ringer normalization utilities 
 **/
namespace Norm {

/// Normalization base classes:
/// @{
class Norm1 : virtual public IPreProcessor,
              public RedirectMsgStream 
{

  RINGER_IO_VARDEP_BASE_NOMEMBER( Norm1 )

  private:
    Norm1(Norm1&&);

  public:
    Norm1(){;}

    virtual void execute(std::vector<float> &inputSpace) const ATH_RINGER_FINAL
      ATH_RINGER_OVERRIDE;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(Norm1,1)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( Norm1 )

class Norm2 : public virtual IPreProcessor,
              public RedirectMsgStream 
{

  RINGER_IO_VARDEP_BASE_NOMEMBER( Norm2 )

  private:
    Norm2(Norm2&&);

  public:
    Norm2(){;}

    virtual void execute(std::vector<float> &inputSpace) const ATH_RINGER_FINAL
      ATH_RINGER_OVERRIDE;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(Norm2,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( Norm2 )

class Sqrt : public virtual IPreProcessor,
             public RedirectMsgStream 
{

  RINGER_IO_VARDEP_BASE_NOMEMBER( Sqrt )

  private:
    Sqrt(Sqrt&&);

  public:
    Sqrt(){};

    virtual void execute(std::vector<float> &inputSpace) const ATH_RINGER_FINAL
      ATH_RINGER_OVERRIDE;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(Sqrt,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( Sqrt )

class ConstantValue : public virtual IPreProcessor,
                      public RedirectMsgStream 
{

  RINGER_IO_VARDEP_BASE( ConstantValue )

  private:
    ConstantValue(ConstantValue&&);
    /// Inverse value of the constant value normalization
    float m_constantInv; 

  public:

    ConstantValue() 
      : m_constantInv(1){;}
    ConstantValue(
        const float constantValue)
      : m_constantInv(1/constantValue)
    {
      if (!m_constantInv) {
        throw std::runtime_error(
            std::string("Initialized ConstantValueVarDep norm with zero constant"));
      }
    }

    virtual void execute(std::vector<float> &inputSpace) const ATH_RINGER_FINAL
      ATH_RINGER_OVERRIDE;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(ConstantValue,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( ConstantValue )

class Sequential : public virtual IPreProcessor,
                   public RedirectMsgStream 
{
  RINGER_IO_VARDEP_BASE( Sequential )

  private:
    Sequential(Sequential&&);
    /// @brief The stop energy threshold for increasing noise
    float m_stopEnergy;
    /// @brief The energy threshold for choosing 
    float m_energyThres;

  public:

    Sequential()
      : m_stopEnergy(0),
        m_energyThres(0){;}

    Sequential(
        const float stopEnergy,
        const float energyThres)
      : m_stopEnergy(stopEnergy),
        m_energyThres(energyThres){;}

    virtual void execute(std::vector<float> &inputSpace) const ATH_RINGER_FINAL
      ATH_RINGER_OVERRIDE;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(Sequential,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( Sequential )

class Spherization : public virtual IPreProcessor,
                     public RedirectMsgStream
{
  RINGER_IO_VARDEP_BASE( Spherization )

  private:
    Spherization(Spherization&&);
    /// @brief The training sample data mean
    std::vector<float> m_deslocation;
    /// @brief The inverse of the training sample data standard deviation
    std::vector<float> m_normInv;
    /// Input dimension (transient variable)
    size_t m_dim;
  public:

    Spherization()
        : m_dim(0){;}

    Spherization(
        const std::vector<float> &dataMean,
        const std::vector<float> &dataStd);

    virtual void execute(std::vector<float> &inputSpace) const ATH_RINGER_FINAL
      ATH_RINGER_OVERRIDE;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(Spherization,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( Spherization )

class MinMax : public virtual IPreProcessor,
               public RedirectMsgStream
{
  RINGER_IO_VARDEP_BASE( MinMax )

  private:
    MinMax(MinMax&&);
    /// @brief The training sample data min
    std::vector<float> m_deslocation;
    /// @brief The inverse of sample delta(max,min)
    std::vector<float> m_normInv;
    /// Input dimension (transient variable)
    size_t m_dim;
  public:
    MinMax()
      : m_dim(0){;}

    MinMax(
        const std::vector<float> min,
        const std::vector<float> max);
  public:

    virtual void execute(std::vector<float> &inputSpace) const ATH_RINGER_FINAL
      ATH_RINGER_OVERRIDE;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(MinMax,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( MinMax )
/// @}

/// @ Usable normalizations:
/// @{
/**
 * @brief Use norm-1 as rings normalization factor
 **/
class Norm1VarDep : virtual public IPreProcessorVarDep,
                    public RingerIOVarDepObj < Norm1VarDep >,
                    public Norm1
{ 
  RINGER_IO_VARDEP_OBJ(Norm1VarDep, Norm1)

  public:

    Norm1VarDep(){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(Norm1VarDep,1)
};

/**
 * @brief Use norm-2 as rings normalization factor
 **/
class Norm2VarDep : virtual public IPreProcessorVarDep,
                    public RingerIOVarDepObj < Norm2VarDep >,
                    public Norm2
{

  RINGER_IO_VARDEP_OBJ(Norm2VarDep, Norm2)

  public:

    Norm2VarDep(){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(Norm2VarDep,1)
};

/**
 * @brief Use SqrtVarDep of norm-1 as rings normalization factor
 **/
class SqrtVarDep : virtual public IPreProcessorVarDep,
                   public RingerIOVarDepObj < SqrtVarDep >,
                   public Sqrt
{

  RINGER_IO_VARDEP_OBJ(SqrtVarDep, Sqrt)

  public:

    SqrtVarDep(){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(SqrtVarDep,1)
};

/**
 * @brief Use constant value as rings normalization factor
 **/
class ConstantValueVarDep : virtual public IPreProcessorVarDep,
                            public RingerIOVarDepObj < ConstantValueVarDep >,
                            public ConstantValue
{

  RINGER_IO_VARDEP_OBJ(ConstantValueVarDep, ConstantValue)

  public:

    ConstantValueVarDep(){;}

    ConstantValueVarDep(const float constantValue)
      : ConstantValue(constantValue){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(ConstantValueVarDep,1)
};

/**
 * @brief Scale rings energy to GeV
 **/
class MevToGevVarDep: virtual public IPreProcessorVarDep,
                      public RingerIOVarDepObj < MevToGevVarDep >,
                      public ConstantValue
{

  RINGER_IO_VARDEP_OBJ(MevToGevVarDep, ConstantValue)

  public:

    // Normalization is the inverse of the Units transformation (MeV -> GeV)
#if !defined(XAOD_STANDALONE) && !defined(RINGER_STANDALONE)
    MevToGevVarDep()
      : ConstantValue( (Gaudi::Units::GeV) / (Gaudi::Units::MeV) ){;}
#else // We suppose that we are working with MeV as reference unit.
    MevToGevVarDep()
      : ConstantValue( 1000 / 1 ){;}
#endif

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(MevToGevVarDep,1)
};

/**
 * @brief Use sequential normalization.
 *
 * For more information check: 
 *
 * https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigHypothesis/TrigMultiVarHypo/trunk/TrigMultiVarHypo/TrigRingerNeuralFex.h#L140
 **/
class SequentialVarDep : virtual public IPreProcessorVarDep,
                         public RingerIOVarDepObj < SequentialVarDep >,
                         public Sequential
{

  RINGER_IO_VARDEP_OBJ(SequentialVarDep, Sequential)

  public:

    SequentialVarDep(){;}

    SequentialVarDep(
        const float stopEnergy,
        const float energyThres):
      Sequential(stopEnergy,energyThres){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(SequentialVarDep,1)
};

/**
 * @brief Normalize data to have zero mean and unitary standard deviation.
 **/
class SpherizationVarDep : virtual public IPreProcessorVarDep,
                           public RingerIOVarDepObj < SpherizationVarDep >,
                           public Spherization
{

  RINGER_IO_VARDEP_OBJ(SpherizationVarDep, Spherization)

  public:

    SpherizationVarDep(){;}

    SpherizationVarDep(
        const std::vector<float> &dataMean,
        const std::vector<float> &dataStd):
      Spherization(dataMean,dataStd){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(SpherizationVarDep,1)
};

/**
 * @brief Normalize data to be bounded within [-1,1] range.
 **/
class MinMaxVarDep : virtual public IPreProcessorVarDep,
                     public RingerIOVarDepObj < MinMaxVarDep >,
                     public MinMax
{
  RINGER_IO_VARDEP_OBJ(MinMaxVarDep, MinMax)

  public:

    MinMaxVarDep(){;}

    MinMaxVarDep(
        const std::vector<float> &min,
        const std::vector<float> &max):
      MinMax(min,max){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(MinMaxVarDep,1)
};
/// @}

} // Norm namespace;
} // Pre-processing namespace;
} // Ringer namespace


#endif // RINGERSELECTORTOOLS_PROCEDURES_NORMALIZATIONS_H

// Include inline and template declarations:
#ifndef INCLUDE_HEADER_ONLY // Protect against circular includes
#include "Normalizations.icc"
#endif // INCLUDE_HEADER_ONLY

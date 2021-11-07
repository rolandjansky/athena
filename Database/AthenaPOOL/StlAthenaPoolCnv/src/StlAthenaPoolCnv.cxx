/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
//#include "GaudiKernel/DeclareFactoryEntries.h"

// explicit instantiations

template <class T>
class StlAthenaPoolCnv : public T_AthenaPoolCnv<T> 
{
  friend class CnvFactory< StlAthenaPoolCnv<T> >;
  typedef T_AthenaPoolCnv<T> BaseCnv_t;
  typedef StlAthenaPoolCnv<T> Self_t;
public:
  /// Constructor
  StlAthenaPoolCnv(ISvcLocator* svcloc) : BaseCnv_t(svcloc) {}

  /// Gaudi Service Interface method implementations:
  virtual StatusCode initialize()
  {
    MsgStream msg( this->msgSvc(), ClassName<Self_t>::name());
    if ( !this->BaseCnv_t::initialize().isSuccess() ) {
      msg << MSG::ERROR << "Could not initialize base class' converter ["
	  << ClassName<BaseCnv_t>::name() << "] !"
	  << endmsg;
      return StatusCode::FAILURE;
    }
    const std::type_info& ti = typeid(T);
    RootType rflx_type = RootType(ti);
    if (!rflx_type) {
      msg << MSG::ERROR
	  << "could not get Reflex::Type from type_info ["
	  << ti.name() 
	  << "] for class ["
	  << ClassName<T>::name() << "] !"
	  << endmsg;
      return StatusCode::FAILURE;
    }
    this->BaseCnv_t::m_classDesc = rflx_type;
    return StatusCode::SUCCESS;
  }
};


#define DECL_CNV(NAME, TDEF) \
  typedef StlAthenaPoolCnv< NAME > TDEF; \
  template class StlAthenaPoolCnv< NAME >; \
  DECLARE_CONVERTER(TDEF)

#define DECL2_CNV(N1, N2, TDEF)	\
  typedef StlAthenaPoolCnv< N1, N2 > TDEF; \
  template class StlAthenaPoolCnv< N1, N2 >; \
  DECLARE_CONVERTER(TDEF)

#include "SGTools/BuiltinsClids.h"
// cppcheck-suppress unknownMacro
DECL_CNV(int, AthenaPoolIntCnv)
DECL_CNV(unsigned int, AthenaPoolUIntCnv)
DECL_CNV(float, AthenaPoolFloatCnv)
DECL_CNV(double, AthenaPoolDoubleCnv)
DECL_CNV(std::string, AthenaPoolStdStringCnv)

#include "SGTools/StlVectorClids.h"
DECL_CNV(std::vector<int>, AthenaPoolStdVectorIntCnv)
DECL_CNV(std::vector<unsigned int>, AthenaPoolStdVectorUIntCnv)
DECL_CNV(std::vector<float>, AthenaPoolStdVectorFloatCnv)
DECL_CNV(std::vector<double>, AthenaPoolStdVectorDoubleCnv)

#include "SGTools/StlMapClids.h"
DECL2_CNV(std::map<int, int>, AthenaPoolStdMapIntIntCnv)
DECL2_CNV(std::map<int, float>, AthenaPoolStdMapIntFloatCnv)
DECL2_CNV(std::map<int, double>, AthenaPoolStdMapIntDoubleCnv)

DECL2_CNV(std::map<std::string, int>, AthenaPoolStdMapStringIntCnv)
DECL2_CNV(std::map<std::string, unsigned int>, AthenaPoolStdMapStringUIntCnv)
DECL2_CNV(std::map<std::string, float>, AthenaPoolStdMapStringFloatCnv)
DECL2_CNV(std::map<std::string, double>, AthenaPoolStdMapStringDoubleCnv)


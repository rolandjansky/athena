#ifndef INDETEVENTATHENAPOOL_CREATETRANSIENTTEMPLATE_H
#define INDETEVENTATHENAPOOL_CREATETRANSIENTTEMPLATE_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetRawData/TRT_RDORawData.h"
#include "InDetRawData/SCT_RDORawData.h"


namespace TPCnv {

  template<> class CreateTransient<InDetRawDataContainer<InDetRawDataCollection<PixelRDORawData> >> {
    public: static std::unique_ptr<InDetRawDataContainer<InDetRawDataCollection<PixelRDORawData> >> create();
  };

  template<> class CreateTransient<InDetRawDataContainer<InDetRawDataCollection<TRT_RDORawData> >> {
    public: static std::unique_ptr<InDetRawDataContainer<InDetRawDataCollection<TRT_RDORawData> >> create();
  };

  template<> class CreateTransient<InDetRawDataContainer<InDetRawDataCollection<SCT_RDORawData> >> {
    public: static std::unique_ptr<InDetRawDataContainer<InDetRawDataCollection<SCT_RDORawData> >> create();
  };

}
//InDetRawDataContainer<InDetRawDataCollection<SCT_RDORawData> >

#endif


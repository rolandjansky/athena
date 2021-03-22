/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCRDOCONTRAWEVENTTOOL_H
#define MUONTGC_CNVTOOLS_TGCRDOCONTRAWEVENTTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonTGC_CnvTools/ITGC_RDOtoByteStreamTool.h"

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "TGC_Hid2RESrcID.h"

class TgcRdoContainer; 
class MsgStream ; 

namespace Muon {

  /** @class TgcRdoContByteStreamTool
   *  An AthAlgTool provides conversion between TGC bareRDO and ByteStream,
   *  and fills it in RawEvent.
   * 
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   *
   *  This class was developed by Tadashi Maeno based on 
   *  RpcPadContByteStream written by Ketevi A. Assamagan. 
   */

  class TgcRdoContByteStreamTool : 
    public extends<AthAlgTool, ITGC_RDOtoByteStreamTool>
    {
    public:
  
      /** Constructor */
      TgcRdoContByteStreamTool (const std::string& type, const std::string& name,
				const IInterface* parent);

      /** Destructor */ 
      virtual ~TgcRdoContByteStreamTool();

      /** Initializer */
      virtual StatusCode initialize() override;
      /** Finalizer */
      virtual StatusCode finalize() override;

      /** Convert RDO container to Byte Stream */
      virtual StatusCode convert(const TgcRdoContainer* cont) const override;

    private: 
      ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc
      { this, "ByteStreamCnvSvc", "ByteStreamCnvSvc" };

      TGC_Hid2RESrcID * m_hid2re; 
    };
} // end of namespace 

#endif // MUONTGC_CNVTOOLS_TGCRDOCONTRAWEVENTTOOL_H

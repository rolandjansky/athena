/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCRDOCONTRAWEVENTTOOL_H
#define MUONTGC_CNVTOOLS_TGCRDOCONTRAWEVENTTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonTGC_CnvTools/ITGC_RDOtoByteStreamTool.h"

#include "ByteStreamData/RawEvent.h" 
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
    public AthAlgTool, 
    virtual public ITGC_RDOtoByteStreamTool 
    {
    public:
  
      /** Constructor */
      TgcRdoContByteStreamTool (const std::string& type, const std::string& name,
				const IInterface* parent);

      /** Destructor */ 
      virtual ~TgcRdoContByteStreamTool();

      /** AthAlgTool InterfaceID */
      static const InterfaceID& interfaceID( ) ;

      /** Initializer */
      virtual StatusCode initialize();
      /** Finalizer */
      virtual StatusCode finalize();

      /** Convert RDO container to Byte Stream */
      StatusCode convert(const TgcRdoContainer* cont, RawEventWrite* re, MsgStream& log);

    private: 
      TGC_Hid2RESrcID * m_hid2re; 
      FullEventAssembler<TGC_Hid2RESrcID> m_fea;    
    };
} // end of namespace 

#endif // MUONTGC_CNVTOOLS_TGCRDOCONTRAWEVENTTOOL_H

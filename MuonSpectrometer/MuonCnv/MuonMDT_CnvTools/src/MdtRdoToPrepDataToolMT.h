/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMdtRdoToPrepDataToolMT_H
#define MUONMdtRdoToPrepDataToolMT_H

#include "MdtRdoToPrepDataToolCore.h"
#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"

namespace Muon {

    /** @class MdtRdoToPrepDataTool

        This is for the Doxygen-Documentation.
        Please delete these lines and fill in information about
        the Algorithm!
        Please precede every member function declaration with a
        short Doxygen comment stating the purpose of this function.

        @author  Edward Moyse <Edward.Moyse@cern.ch>
    */

    class MdtRdoToPrepDataToolMT : public extends<MdtRdoToPrepDataToolCore, IMuonRdoToPrepDataTool> {
    public:
        MdtRdoToPrepDataToolMT(const std::string&, const std::string&, const IInterface*);

        /** default destructor */
        virtual ~MdtRdoToPrepDataToolMT() = default;

        /** standard Athena-Algorithm method */
        virtual StatusCode initialize() override;

        virtual void printPrepData() const override;

    protected:
        virtual Muon::MdtPrepDataContainer* setupMdtPrepDataContainer(unsigned int sizeVectorRequested, bool& fullEventDone) const override;

    private:
        /// This is the key for the cache for the MDT PRD containers, can be empty
        SG::UpdateHandleKey<MdtPrepDataCollection_Cache> m_prdContainerCacheKey;
    };
}  // namespace Muon

#endif

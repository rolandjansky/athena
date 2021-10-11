/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMdtRdoToPrepDataTool_H
#define MUONMdtRdoToPrepDataTool_H

#include "CxxUtils/checker_macros.h"
#include "MdtRdoToPrepDataToolCore.h"

namespace Muon {

    /** @class MdtRdoToPrepDataTool

        This is for the Doxygen-Documentation.
        Please delete these lines and fill in information about
        the Algorithm!
        Please precede every member function declaration with a
        short Doxygen comment stating the purpose of this function.

        @author  Edward Moyse <Edward.Moyse@cern.ch>
    */

    class ATLAS_NOT_THREAD_SAFE MdtRdoToPrepDataTool : public extends<MdtRdoToPrepDataToolCore, IMuonRdoToPrepDataTool> {
    public:
        MdtRdoToPrepDataTool(const std::string&, const std::string&, const IInterface*);

        /** default destructor */
        virtual ~MdtRdoToPrepDataTool() = default;

        /** standard Athena-Algorithm method */
        virtual StatusCode initialize() override;

        virtual void printPrepData() const override;

    protected:
        virtual Muon::MdtPrepDataContainer* setupMdtPrepDataContainer(unsigned int sizeVectorRequested, bool& fullEventDone) const override;

    private:
        mutable Muon::MdtPrepDataContainer* m_mdtPrepDataContainer = nullptr;

        // keepTrackOfFullEventDecoding
        mutable bool m_fullEventDone = false;
    };
}  // namespace Muon

#endif

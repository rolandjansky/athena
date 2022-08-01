
#ifndef EFEX_TOB_PACKER_H
#define EFEX_TOB_PACKER_H

#include <vector>
#include <cstdint>
#include "infraL1Calo/FibrePackerBase.h"   
#include "defsL1Calo/EfexDefs.h"

class EfexTobPacker : public FibrePackerBase{
/**
 * \brief Class implementing packing and unpacking data into eFex TOB format
 *
 * This is a relatively simple packing class
 */

public:
             EfexTobPacker() {}
    virtual ~EfexTobPacker() {}

    virtual std::vector<myDataWord> getPackedData(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const override;   
    virtual std::vector<myDataWord> getPackedControl(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const override;

    virtual bool checkCRC(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;
    virtual myDataWord getBcNumber(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;

    virtual std::vector<myDataWord> getUnpackedData(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;

private:

};

#endif

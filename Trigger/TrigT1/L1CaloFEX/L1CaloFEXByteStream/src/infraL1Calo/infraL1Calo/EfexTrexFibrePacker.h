
#ifndef EFEX_TREX_FIBRE_PACKER_H
#define EFEX_TREX_FIBRE_PACKER_H

#include <vector>
#include <cstdint>
#include "infraL1Calo/FibrePackerBase.h"   
#include "defsL1Calo/EfexDefs.h"

class EfexTrexFibrePacker : public FibrePackerBase{
/**
 * \brief Class implementing packing and unpacking data into TREX eFex format
 *
 * The class does heavy lifting of packing and unpacking TREX data packet. The format
 * is similar to one used by LATOME and stored at
 * https://gitlab.cern.ch/atlas-lar-be-firmware/LATOME/LATOME/raw/master/LATOME/doc/LAr-LATOME-FW/LAr-LATOME-FW.pdf
 * but different in few important details! Most recent version is called TREX-FEX Real-Time Data Formats v3 14.10.2019
 */

public:
             EfexTrexFibrePacker() {}
    virtual ~EfexTrexFibrePacker() {}

    /**
     *  \brief Function packing the data into the LATOME format, either standard or alignement frame
     *
     *  The function expects input data in vector inFrame. For normal frame, this is a vector of supercell energies.
     *  For alignement frame the function doesn't need any additional data.
     *
     */

    virtual std::vector<myDataWord> getPackedData(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const override;   
    virtual std::vector<myDataWord> getPackedControl(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const override;

    virtual bool checkCRC(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;
    virtual myDataWord getBcNumber(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;

    /**
     *  \brief Function unpacking the data from LATOME format, either standard or alignement frame
     *
     *  For normal frame, the function returns a vector of supercell energies.
     *  For alignement frame the function returns empty vector.
     *
     */
    virtual std::vector<myDataWord> getUnpackedData(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;

private:

};

#endif

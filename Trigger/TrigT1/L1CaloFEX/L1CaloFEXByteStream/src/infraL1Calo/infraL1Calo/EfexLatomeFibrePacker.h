
#ifndef EFEX_LATOME_FIBRE_PACKER_H
#define EFEX_LATOME_FIBRE_PACKER_H

#include <vector>
#include <cstdint>
#include "infraL1Calo/FibrePackerBase.h"   
#include "defsL1Calo/EfexDefs.h"

class EfexLatomeFibrePacker : public FibrePackerBase{
/**
 * \brief Class implementing packing and unpacking data into LAr LATOME eFex format
 *
 * The class does heavy lifting of packing and unpacking LATOME data packet. The format
 * is taken from 
 * https://gitlab.cern.ch/atlas-lar-be-firmware/LATOME/LATOME/raw/master/LATOME/doc/LAr-LATOME-FW/LAr-LATOME-FW.pdf
 * version from 29th of January 2020
 */

public:
             EfexLatomeFibrePacker() {}
    virtual ~EfexLatomeFibrePacker() {}

    /**
     *  \brief Function packing the data into the LATOME format, either standard or alignement frame
     *
     *  The function expects input data in vector inFrame. For normal frame, this is a vector of supercell energies.
     *  For alignement frame this function expects LATOME meta information in following format:
     *  inFrame[0] ... latome_id
     *  inFrame[1] ... latome_src_id
     *  inFrame[2] ... fiber_id
     *
     *  FEX_ID is always set to zero because it's eFex
     */

    virtual std::vector<myDataWord> getPackedData(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const override;   
    virtual std::vector<myDataWord> getPackedControl(const std::vector<myDataWord>& inFrame,myDataWord bcNumber, InputDataFrameType frameType) const override;

    virtual bool checkCRC(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;
    virtual myDataWord getBcNumber(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;

    /**
     *  \brief Function unpacking the data from LATOME format, either standard or alignement frame
     *
     *  For normal frame, the function returns a vector of supercell energies.
     *  For alignement frame the function returns LATOME meta information in following format:
     *  inFrame[0] ... latome_id
     *  inFrame[1] ... latome_src_id
     *  inFrame[2] ... fiber_id
     *
     */
    virtual std::vector<myDataWord> getUnpackedData(const std::vector<myDataWord>& encodedData, InputDataFrameType frameType) const override;

private:

};

#endif

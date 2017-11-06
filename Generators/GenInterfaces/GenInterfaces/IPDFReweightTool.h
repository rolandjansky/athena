/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPDFREWEIGHTTOOL_H
#define IPDFREWEIGHTTOOL_H

#include "GaudiKernel/IAlgTool.h"
//#include "AthenaBaseComps/AthAlgTool.h"

static const InterfaceID IID_IPDFReweightTool("IPDFReweightTool", 1, 0);

/**
@class     PDFReweightTool.h

@brief     Interface AlgTool base class for PDF reweighting tool
        
@author    Gia Khoriauli  and  Markus Cristinziani
        Universitat Bonn

        May, 2008
*/
class IPDFReweightTool : virtual public IAlgTool {
        public:
        static const InterfaceID& interfaceID();       
	
};


inline const InterfaceID& IPDFReweightTool::interfaceID()
{ 
        return IID_IPDFReweightTool; 
}

#endif

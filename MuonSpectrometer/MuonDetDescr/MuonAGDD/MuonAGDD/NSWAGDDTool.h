/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWAGDDTool_H
#define NSWAGDDTool_H

#include "AGDDControl/AGDDToolBase.h"

class NSWAGDDTool: public AGDDToolBase
{
public:
    NSWAGDDTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~NSWAGDDTool()=default;
    virtual StatusCode construct() override;
    virtual StatusCode initialize() override;

private:
    Gaudi::Property<int> m_outFileActV{this,"OutputFileACTVERS",0,"active version number"};
    Gaudi::Property<int> m_outFileAlgV{this,"OutputFileALGVERS",0,"alignment version number"};
    Gaudi::Property<int> m_outFilePasV{this,"OutputFilePASVERS",0,"passive structure version number"};

    Gaudi::Property<std::string> m_outFileActN{this,"OutputFileACTVNAME","","active version string"};
    Gaudi::Property<std::string> m_outFileAlgN{this,"OutputFileALGVNAME","","alignment version string"};
    Gaudi::Property<std::string> m_outFilePasN{this,"OutputFilePASVNAME","","passive structure version string"};
    Gaudi::Property<std::string> m_outFileForm{this,"OutputFileFORMAT","AGDDXML","format of output file"};
    Gaudi::Property<std::string> m_outFileType{this,"OutputFileType","NSWD","name for database table"};

    std::string m_outFileInName;
    std::string m_outPREsqlName;

    bool WritePREsqlFile() const;
};

#endif

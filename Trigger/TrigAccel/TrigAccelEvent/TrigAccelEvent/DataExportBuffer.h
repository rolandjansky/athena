/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGACCELEVENT_DATAEXPORTBUFFER_H
#define TRIGACCELEVENT_DATAEXPORTBUFFER_H


#include <string>
#include <fstream>

namespace TrigAccel {

  typedef struct DataExportBuffer {
  public:
  DataExportBuffer() : m_size(0), m_buffer(0) {};
  DataExportBuffer(size_t s) : m_size(s) {
    m_buffer = new char[s];
  }
    ~DataExportBuffer() {delete[] m_buffer;}

    inline bool fit(size_t s) {
      return s<=m_size;
    }

    void reallocate(size_t s) {
      delete[] m_buffer;
      m_buffer = new char[s];
      m_size = s;
    }

    void save(const std::string& name) const {
      std::ofstream binFile(name, std::ios::binary);
      binFile.write(m_buffer, m_size);
      binFile.close();
    }

    size_t load(const std::string& name) {
      std::ifstream binFile(name, std::ios::binary);
      if (!binFile) {
	return 0;
      }
      binFile.seekg(0, binFile.end);
      size_t fileSize = binFile.tellg();
      binFile.seekg (0, binFile.beg);
      reallocate(fileSize);
      binFile.read(m_buffer, m_size);
      binFile.close();
      return fileSize;
    }

    size_t m_size;
    char* m_buffer;
  } DATA_EXPORT_BUFFER;
}

#endif

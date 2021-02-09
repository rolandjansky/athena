/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "HDF5Utils/HdfTuple.h"
#include "HDF5Utils/common.h"

#include "H5Cpp.h"

#include <cassert>
#include <set>
#include <stdexcept>

namespace H5Utils {

  namespace {

    H5::CompType build_type(const VariableFillers& fillers) {
      using internal::data_buffer_t;
      std::set<std::string> inserted; // check for repeat names
      H5::CompType type(fillers.size() * sizeof(data_buffer_t));
      size_t dt_offset = 0;
      for (const auto& filler: fillers) {
        const std::string& name = filler->name();
        if (inserted.count(name)) {
          throw std::logic_error(name + " inserted twice");
        }
        inserted.insert(name);
        type.insertMember(filler->name(), dt_offset, filler->get_type());
        dt_offset += sizeof(data_buffer_t);
      }
      return type;
    }

  }

// _______________________________________________________________________
// Xd writter
//

  std::vector<size_t> WriterXd::s_NONE = {};

  WriterXd::WriterXd(H5::Group& group, const std::string& name,
                     VariableFillers fillers,
                     std::vector<hsize_t> max_length,
                     hsize_t batch_size):
    m_type(build_type(fillers)),
    m_max_length(max_length),
    m_dim_stride(max_length),
    m_batch_size(batch_size),
    m_offset(0),
    m_fillers(fillers)
  {
    if (batch_size < 1) {
      throw std::logic_error("batch size must be > 0");
    }
    // create space
    H5::DataSpace space = internal::getUnlimitedSpace(max_length);

    // create params
    H5::DSetCreatPropList params = internal::getChunckedDatasetParams(
      max_length, batch_size);

    // calculate striding
    m_dim_stride = internal::getStriding(max_length);

    // create ds
    internal::throwIfExists(name, group);
    m_ds = group.createDataSet(name, internal::packed(m_type), space, params);
  }

  WriterXd::~WriterXd() {
    try {
      flush();
    } catch (H5::Exception& err) {
      internal::printDestructorError(err.getDetailMsg());
    } catch (std::exception& err) {
      internal::printDestructorError(err.what());
    }
  }

  void WriterXd::fillWhileIncrementing(std::vector<size_t>& indices) {
    if (buffer_size() == m_batch_size) {
      flush();
    }
    indices.resize(m_max_length.size());

    // build buffer and _then_ insert it so that exceptions don't leave
    // the buffer in a weird state
    std::vector<internal::data_buffer_t> temp;

    std::fill(indices.begin(), indices.end(), 0);
    for (size_t gidx = 0; gidx < m_dim_stride.front(); gidx++) {

      // we might be able to make this more efficient and less cryptic
      for (size_t iii = 0; iii < indices.size(); iii++) {
        indices.at(iii) = (gidx % m_dim_stride.at(iii)) /
          m_dim_stride.at(iii+1);
      }

      for (const auto& filler: m_fillers) {
        temp.push_back(filler->get_buffer());
      }
    }
    m_buffer.insert(m_buffer.end(), temp.begin(), temp.end());
  }
  void WriterXd::flush() {
    if (buffer_size() == 0) return;
    // extend the ds
    std::vector<hsize_t> slab_dims{buffer_size()};
    slab_dims.insert(slab_dims.end(), m_max_length.begin(), m_max_length.end());
    std::vector<hsize_t> total_dims{buffer_size() + m_offset};
    total_dims.insert(total_dims.end(), m_max_length.begin(), m_max_length.end());
    m_ds.extend(total_dims.data());

    // setup dataspaces
    H5::DataSpace file_space = m_ds.getSpace();
    H5::DataSpace mem_space(slab_dims.size(), slab_dims.data());
    std::vector<hsize_t> offset_dims{m_offset};
    offset_dims.resize(slab_dims.size(), 0);
    file_space.selectHyperslab(H5S_SELECT_SET,
                               slab_dims.data(), offset_dims.data());

    // write out
    assert(static_cast<size_t>(file_space.getSelectNpoints())
           == m_buffer.size() / m_fillers.size());
    m_ds.write(m_buffer.data(), m_type, mem_space, file_space);
    m_offset += buffer_size();
    m_buffer.clear();
  }

  size_t WriterXd::index() const {
    return m_offset + buffer_size();
  }

  hsize_t WriterXd::buffer_size() const {
    size_t n_entries = m_buffer.size() / m_fillers.size();
    assert(n_entries % m_dim_stride.front() == 0);
    return n_entries / m_dim_stride.front();
  }

}

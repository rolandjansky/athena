#include <stdint.h>
#include <array>

class MdtHistCoder {

 public:
  enum {
    region=0,
    layer=1,
    crate_region=2,
    crate=3,
    phi=4
  }; //bitshifts                                                                                                                                                                                                     

 private:
  static constexpr std::array<uint16_t,5> m_bitshift{{0, 2, 4, 6, 8}}; //region, layer, crate_region, crate, phi                                                                                                     
  static constexpr std::array<uint16_t,5> m_bitmask{{0x3, 0x3, 0x3, 0x3, 0xF}};
  //region (on two bits)                                                                                  
  //layer (on two bits)                                                                                                  
  //crate_region (on two bits)                                                                                                                                        
  //crate (on two bits)                                                                                                                                              
  //phi (on four bits)                                                                                                                                          

 public:
  static uint16_t encode(const uint16_t regionIn, const uint16_t layerIn,
                         const uint16_t phiIn, const uint16_t crate_regionIn,
                         const uint16_t crateIn) {
    uint16_t result=
      (regionIn       & m_bitmask[region])       << m_bitshift[region]       |
      (layerIn        & m_bitmask[layer])        << m_bitshift[layer]        |
      (phiIn          & m_bitmask[phi])          << m_bitshift[phi]          |
      (crate_regionIn & m_bitmask[crate_region]) << m_bitshift[crate_region] |
      (crateIn        & m_bitmask[crate])        << m_bitshift[crate];

    return result;
  }

  static uint16_t decode(const uint16_t value, const uint16_t field) {
    return (value >> m_bitshift[field]) & m_bitmask[field];
  }

};

# TrigCompositeUtils

Package to host helper classes used both online in HLT execution and offline in the analysis of the HLT result.

## TrigCompositeUtils

Quality of life helpers and navigation helpers to aid in creating and interpreting `xAOD::TrigComposite` (also known as `Decision`) objects linked together to form a directed acyclic graph structure. This graph is referred to as the trigger navigation.  

## HLTIdentifier

Wrapper function around a HLT chain's name (or chain's leg's name), allowing easier access to both the `std::string` and hashed `uint32_t` representations along with iostream functions.

## Combinators

Placeholder for combinatoric iteration helper code over combined chains

# JetRecConfig/python

## Module contents

### JetRecConfig.py

* This is the top-level jet reconstruction module, containing the
  function JetRecCfg() that returns a configured ComponentAccumulator
  with the standalone jet reconstruction sequence.
* The function resolveDependencies() can be called to retrieve a
  dict containing the full set of input collections, ghost associations
  and jet modifiers needed to satisfy the user's needs as encoded
  in the provided JetDefinition.

### JetDefinition.py

* This module defines a number of configuration classes used to define
  a jet reconstruction job. The classes are listed below together with
  the information that they contain.
  * JetDefinition -- defines the jet collection
     * jet clustering algorithm (kt, Cambridge/Aachen, anti-kt)
     * radius
     * input type (JetConstit object)
     * name
     * minimum pt (separately for fastjet and xAOD jet conversion)
     * ghost list
     * modifier list
  * JetConstit -- defines inputs to jet finding
     * base type (type of xAOD object)
     * modifier list (corrections/filters applied before jet finding)
  * JetGhost -- defines objects to be associated with ghost association
     * input type (type of xAOD object)
     * label (identifier, e.g. type of truth particle [TruthPartons], ...)
  * JetModifier -- defines tools that operate on the jet collection
     * tooltype
     * tool name
     * a helper function defining how to configure the tool
     * a string describing to the helper function how to configure the tool
     * a flag stating whether the jet definition is needed to configure the tool
     * a list of prerequisites (inputs, ghosts, other mods) or a helper function returning such a list

### ConstModHelpers.py

* This module assists setup of constituent modifiers, i.e. operations
  like filtering and four-momentum corrections applied prior to jet finding.
* It is called from JetRecConfig.py

### JetModConfig.py

* This module assists configuration of modifiers, including resolving
  their dependencies as the job is scheduled.
* When a list of desired modifiers is provided, a full list of all
  modifiers needing to be run such that the desired modifiers will
  be executed is returned, along with the sets of ghosts and input
  collections needing to be set up.

### StandardJetDefs.py

* This module holds a library of standard jet and constituent definitions
  that are regularly used in reconstruction.

### StandardJetMods.py

* This module holds a library of standard jet modifiers that are
  frequently scheduled in reconstruction.

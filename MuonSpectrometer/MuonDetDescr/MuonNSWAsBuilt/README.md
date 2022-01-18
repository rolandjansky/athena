[[_TOC_]]

# NswAsBuilt: compute as-built NSW strip positions

This package is able to compute strip positions for the NSW (currently only for
the MicroMegas). This package is provided as a standalone library, and can be
included in athena trivially.

NSW as-built databases are in JSON format and are available at
[this link][NswAsBuiltFiles]. In the future they will be available in the
conditions DB.

## Basic usage

The as-built parameters are stored in a JSON file containing all the parameters
needed to reconstruct strip positions in the (athena-compatible) frame of the
quadruplet.

The main interface of this library is the class
[`NswAsBuilt::StripCalculator`](NswAsBuilt/StripCalculator.h). An example is
provided in the program [`testStrip`](src/testStrip.cxx).

At initialization step, a `StripCalculator` instance must be created and the JSON
file must be parsed:

```c++
    // Init StripCalculator
    NswAsBuilt::StripCalculator sc;
    // Parse the JSON file
    std::ifstream json_in("nsw_asbuilt_json.txt");
    sc.parseJSON(json_in);
```

Strip positions are accessed through their athena indices:
   * `stationName`
   * `stationEta`
   * `stationPhi`
   * `multilayer`
   * `layer`
   * `strip`

The first four indices make up the quadruplet identifier. Appending the final
two make the strip identifier. The identifiers used in this package are
documented in the class [`Identifier`](NswAsBuilt/Identifier.h).

Given the athena indices of a strip, the position in the quadruplet from may be
obtained from the `StripCalculator`:

```c++
    NswAsBuilt::stripIdentifier_t strip_id;
    strip_id.quadruplet = {stationName, stationEta, stationPhi, multilayer};
    strip_id.ilayer = ilayer;
    strip_id.istrip = istrip;
    strip_t strip = sc.getStrip(iclass, strip_id);
```

The parameter iclass is either `NOMINAL` or `CORRECTION`, to obtain either the
nominal geometry, or the actually measured as-built geometry. See
[`Element::ParameterClass`](NswAsBuilt/Element.h#L59) The returned object is:

```c++
    struct strip_t {
      IsValid isvalid;
      Amg::Vector3D center;
      Amg::Vector3D left;
      Amg::Vector3D right;
    };
```

In other words, three points are returned along the strip, in the coordinate
system of the quadruplet. Note that the strips are deformed, hence the three
points are not generally in line (except in the `NOMINAL` geometry).

## Dependencies

The list of external dependencies is deliberately kept small, for easy
integration with other software.

   * [`Eigen`][Eigen]: linear algebra package providing here the
     `Eigen::Vector3d` class, geometry transformations, and auto-vectorizing
     features which speed up the calculation considerably. `Eigen` is also the
     linear algebra/geometry package used in athena.
   * [`json.hpp`][json.hpp]: an easy-to-use JSON parser. A copy is provided in
     this repository.
   * [`EventPrimitives`](EventPrimitives) and [`GeoPrimitives`](GeoPrimitives):
     athena packages which here are only used to provide various typedefs for
     the `Eigen` library, such as `Amg::Vector3D`. A copy of these two packages
     is provided in this repository for convenience.
   * [`MTools`](MTools) (optional): a simple `Makefile` package for compiling
     this package into a standalone library

## Installation instructions for a standalone library

Check out the code:

    # Clone this repository
    git clone ssh://git@gitlab.cern.ch:7999/giraudpf/NswAsBuilt.git
    cd NswAsBuilt/

Check out and set up `MTools`, the Makefile package:

    # Clone MTools
    git clone ssh://git@gitlab.cern.ch:7999/giraudpf/MTools.git
    MTools/setupmtools.sh

Link `Eigen`

    ln -s <path_to_eigen>/Eigen

Compile:

    make -j8

Run the test program:

    . setup.sh
    testStrip <path_to_nsw_asbuilt_json_file>

The JSON files containing the database of as-built parameters are available at [this link][NswAsBuiltFiles]


[NswAsBuiltFiles]: https://cernbox.cern.ch/index.php/s/RlS9lYsotj0yzi9
[Eigen]: https://eigen.tuxfamily.org/index.php?title=Main_Page
[json.hpp]: https://github.com/nlohmann/json
[MTools]: https://gitlab.cern.ch/giraudpf/MTools


